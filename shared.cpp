#include "shared.h"

#include <memory>
#include <string>
#include <unordered_map>

#include "vm_pager.h"


// Initialize global variables
// Initialize Clock (empty)
Clock clock_;

// Initialize SwapManager
SwapManager swap_manager;

// Initialize file_table (empty)
// std::unordered_map<FileBlock, std::shared_ptr<PageState>> file_table;
std::unordered_map<const char*, std::unordered_map<unsigned int, std::shared_ptr<PageState>>> file_table;

// Initialize page_states (empty)
std::unordered_map<page_table_t*, std::vector<std::shared_ptr<PageState>>> arenas;

// Initialize page_tables (empty)
std::unordered_map<pid_t, page_table_t*> page_tables;

// Initialize phys_mem_pages
std::vector<std::shared_ptr<PageState>> phys_mem_pages;

// Initialize lowest_invalid_vpns (empty)
std::unordered_map<page_table_t*, unsigned int> lowest_invalid_vpns;

// Initialize buffer
char buffer[VM_PAGESIZE];



bool file_in_file_table(const char* filename, unsigned int block) {
    if (file_table.find(filename) != file_table.end()) {
        std::unordered_map<unsigned int, std::shared_ptr<PageState>> inner = file_table[filename];
        if (inner.find(block) != inner.end()) {
            return true;
        }
    }
    return false;
}

void remove_file_table_entry(const char* filename, unsigned int block) {
    assert(file_in_file_table(filename, block));
    file_table[filename].erase(block);
    if (file_table[filename].empty()) {
        file_table.erase(filename);
    }
}

unsigned int vpn_to_ppn(unsigned int vpn) {
    return page_table_base_register->ptes[vpn].ppage;
}

// TODO: strlen assumes contiguous in memory, but page need not be
// better idea to read through byte-by-byte and check for null character
intptr_t end_of_filename(const char* filename) {
    return (intptr_t)filename + strlen(filename) + 1;
}

bool filename_valid_in_arena(const char* filename, unsigned int lowest_invalid_vpn) {
    if (end_of_filename(filename) > (intptr_t)VM_ARENA_BASEADDR+lowest_invalid_vpn){
        return false;
    }
    return true;
}

// DOES NOT increment vpn (lowest invalid)
void update_pte(unsigned int vpn, unsigned int ppn,
                unsigned int read, unsigned int write, page_table_t* ptbr) {
    ptbr->ptes[vpn].ppage = ppn;
    ptbr->ptes[vpn].read_enable = read;
    ptbr->ptes[vpn].write_enable = write;
}

intptr_t ppn_to_mem_addr(unsigned int ppn) {
    return (intptr_t)vm_physmem+(ppn*VM_PAGESIZE);
}

unsigned int va_to_vpn(const void* va){
    intptr_t va_int = (intptr_t) va;
    return va_int>>16 - (intptr_t) VM_ARENA_BASEADDR;
}

//// free_ppn
// - before: either active_pages or free_pages
// - after : active_pages
//
//// phy_mem_pages (after)
// - evict  : still points to evicted page
// - g_f_ppn: nullptr
//
//// PageState (after)
// - evict  : updated
// - g_f_ppn: N/A (no page associated before)
//
//// pte (after)
// - evict  : r:0, w:0 (ppn:0, arbitrary)
// - g_f_ppn: N/A (no pte associated before)
//
//// buffer
// - both   : N/A (even evict never touches buffer)
//
//// arena
// - both   : N/A
// > evict  : vpn stays the same, linked PageState is updated
// > g_f_ppn: vpn stays the same, PageState must be updated after this function
//            because future PageState not guaranteed
unsigned int evict_or_get_free_ppn() {
    // if memory is full
    unsigned int free_ppn;
    if (clock_.is_full()) {
        // evict using clock algorithm
        free_ppn = clock_.evict();
    }
    else {
        free_ppn = clock_.get_free_ppn();
    }
    
    return free_ppn;
}

// buffer: relevant logic contained
// - can be used freely before/after this function
// 
unsigned int disk_to_mem(const char *filename, unsigned int block) {
    // read in file into buffer
    file_read(filename, block, buffer);

    unsigned int free_ppn = evict_or_get_free_ppn();

    // write contents of buffer into memory
    std::memcpy((void*)ppn_to_mem_addr(free_ppn) , buffer, VM_PAGESIZE);

    return free_ppn;
}

void make_page_dirty(unsigned int vpn, std::shared_ptr<PageState> page_state) {
    // update page state
    page_state->referenced = true;
    assert(page_state->resident == true);  // TODO: remove from this abstraction?
    page_state->dirty = true;

    // update pte
    update_pte(vpn, page_state->ppn, 1, 1, page_table_base_register);
}

unsigned int last_addr_in_ppn(unsigned int ppn) {
    return (ppn+1)*VM_PAGESIZE - 1;
}

const char* get_filename(const char* va) {
    // split va into vpn + offset
    unsigned int offset = (intptr_t) va & 0xffff;
    unsigned int vpn = (intptr_t) va >> 16;

    unsigned int lowest_invalid_vpn = lowest_invalid_vpns[page_table_base_register];
    std::string file_name_string;

    unsigned int vpn_index = vpn - (intptr_t) VM_ARENA_BASEADDR;

    std::vector<std::shared_ptr<PageState>> arena = arenas[page_table_base_register];

    while (true) {
        // check if vpn is in valid range
        if (vpn <= (intptr_t) VM_ARENA_BASEADDR || vpn >= lowest_invalid_vpn) {
            return nullptr;
        }

        // get page state of vpn
        std::shared_ptr<PageState> page_state = arena[vpn_index];

        // get ppn of vpn (evicting if necessary)
        unsigned int ppn;
        if (page_state->resident) {
            ppn = page_state->ppn;
        }
        else {
            const char* page_filename;
            unsigned int page_block;
            if (page_state->type == PAGE_TYPE::SWAP_BACKED) {
                page_filename = nullptr;
                page_block = page_state->swap_block;
            }
            else {
                page_filename = page_state->filename;
                page_block = page_state->file_block;
            }
            ppn = disk_to_mem(page_filename, page_block);
        }

        // use ppn + offset and start reading byte-by-byte
        unsigned int cur_addr = (VM_PAGESIZE * ppn) + offset;
        char cur_char = ((char*)vm_physmem)[cur_addr];

        // read until null character or end of page
        while (cur_char != '\0' || cur_addr == last_addr_in_ppn(ppn)) {
            file_name_string += cur_char;
        }

        // if null character not reached by end, increment vpn and continue
        if (cur_char == '\0') {
            // turn vector into C-string
            return file_name_string.c_str();
        }

        // update values to read from start of next page
        vpn++;
        vpn_index++;
        offset = 0;
    }
}
