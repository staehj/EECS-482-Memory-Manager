#include "shared.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "vm_pager.h"

//
// Initialize global variables
//

// Initialize Clock (empty)
Clock clock_;

// Initialize SwapManager
SwapManager swap_manager;

// Initialize file_table (empty)
std::unordered_map<std::string,
    std::unordered_map<unsigned int, std::shared_ptr<PageState>>> file_table;

// Initialize arenas (empty)
std::unordered_map<page_table_t*, std::vector<std::shared_ptr<PageState>>> arenas;

// Initialize page_tables (empty)
std::unordered_map<pid_t, page_table_t*> page_tables;

// Initialize phys_mem_pages
std::vector<std::shared_ptr<PageState>> phys_mem_pages;


void sanitize_page_table(page_table_t* pt) {
    for (unsigned int i = 0; i < VM_ARENA_SIZE/VM_PAGESIZE; ++i) {
        pt->ptes[i].ppage = 0;
        pt->ptes[i].read_enable = 0;
        pt->ptes[i].write_enable = 0;
    }
}

bool file_in_file_table(const char* filename, unsigned int block) {
    std::string filename_string = std::string(filename);
    if (file_table.find(filename_string) != file_table.end()) {
        std::unordered_map<unsigned int, std::shared_ptr<PageState>> inner
            = file_table[filename_string];
        if (inner.find(block) != inner.end()) {
            return true;
        }
    }
    return false;
}

bool file_is_resident(const char* filename, unsigned int block) {
    std::string filename_string = std::string(filename);
    if (file_table.find(filename_string) != file_table.end()) {
        std::unordered_map<unsigned int, std::shared_ptr<PageState>> &inner
            = file_table[filename_string];
        if (inner.find(block) != inner.end()) {
            std::shared_ptr<PageState> page_state = inner[block];
            if (page_state->resident) {
                return true;
            }
        }
    }
    return false;
}

unsigned int vpn_to_ppn(unsigned int vpn) {
    return page_table_base_register->ptes[vpn].ppage;
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
    return ((intptr_t)va - (intptr_t)VM_ARENA_BASEADDR)>>16;
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
//// arena
// - both   : N/A
// > evict  : vpn stays the same, linked PageState is updated
// > g_f_ppn: vpn stays the same, PageState must be updated after this function
//            because future PageState not guaranteed

// returns 0 if file_write fails in evicted case
unsigned int evict_or_get_free_ppn() {
    // if memory is full
    unsigned int free_ppn;
    if (clock_.is_full()) {
        // evict using clock algorithm
        free_ppn = clock_.evict();
        if (free_ppn == 0) {
            return 0;
        }
    }
    else {
        free_ppn = clock_.get_free_ppn();
    }
    
    return free_ppn;
}

// returns 0 if file_read or file_write (in evicted case) fails
unsigned int disk_to_mem(const char *filename, unsigned int block) {
    unsigned int free_ppn = evict_or_get_free_ppn();

    if (file_read(filename, block, (void*)ppn_to_mem_addr(free_ppn)) == -1) {
        // make free_ppn free (since, in either case, we push back again to active_pages)
        clock_.make_free(free_ppn);
        return 0;
    }

    return free_ppn;
}

void make_page_dirty(unsigned int vpn, std::shared_ptr<PageState> page_state) {
    // update page state
    page_state->referenced = true;
    assert(page_state->resident == true);
    page_state->dirty = true;

    if (page_state->type == PAGE_TYPE::SWAP_BACKED) {
        // update pte
        update_pte(vpn, page_state->ppn, 1, 1, page_state->owner_ptbr);
    }
    else {
        page_state->update_ptes(page_state->ppn, 1, 1);
    }
}

unsigned int last_addr_in_ppn(unsigned int ppn) {
    return (ppn+1)*VM_PAGESIZE - 1;
}

const char* get_filename(const char* va) {
    // split va into vpn + offset
    unsigned int offset = (intptr_t) va & 0xffff;
    unsigned int vpn_index = va_to_vpn(va);

    std::string file_name_string = "";

    std::vector<std::shared_ptr<PageState>> &arena = arenas[page_table_base_register];

    while (true) {
        // check if vpn is in valid range
        if ((intptr_t) va < (intptr_t) VM_ARENA_BASEADDR
        || vpn_index >= lowest_invalid_vpn()) {
            return nullptr;
        }

        // check PTE for r:1, w:0, fault if not
        if (page_table_base_register->ptes[vpn_index].read_enable == 0) {
            if (vm_fault(va, false) == -1) {
                return nullptr;
            }
        }

        // get page state of vpn
        std::shared_ptr<PageState> page_state = arena[vpn_index];

        unsigned int ppn = page_state->ppn;

        // use ppn + offset and start reading byte-by-byte
        unsigned int cur_addr = (VM_PAGESIZE * ppn) + offset;
        char cur_char = ((char*)vm_physmem)[cur_addr];

        // read until null character or end of page
        while (cur_addr <= last_addr_in_ppn(ppn) && cur_char != '\0') {
            file_name_string += cur_char;

            // update condition
            cur_addr += 1;
            if (cur_addr <= last_addr_in_ppn(ppn)) {
                cur_char = ((char*)vm_physmem)[cur_addr];
            }
        }

        // if null character not reached by end, increment vpn and continue
        if (cur_char == '\0') {
            // turn vector into C-string
            file_name_string += '\0';
            char* file_name_c_string = new char[file_name_string.length()+1];
            strcpy(file_name_c_string, file_name_string.c_str());
            return file_name_c_string;
        }

        // update values to read from start of next page
        vpn_index++;
        offset = 0;
    }
}

unsigned int lowest_invalid_vpn() {
    return arenas[page_table_base_register].size();
}
