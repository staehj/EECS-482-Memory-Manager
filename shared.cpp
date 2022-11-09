#include "shared.h"

#include "vm_pager.h"

unsigned int vpn_to_ppn(unsigned int vpn) {
    return page_table_base_register->ptes[vpn].ppage;
}

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

void* ppn_to_mem_addr(unsigned int ppn) {
    return vm_physmem+(ppn*VM_PAGESIZE);
}

unsigned int va_to_vpn(const void* va){
    intptr_t va_int = (intptr_t) va;
    return va_int>>16 - (intptr_t) VM_ARENA_BASEADDR;
}

unsigned int evict_or_get_free_ppn() {
    // if memory is full
    unsigned int free_ppn;
    if (clock.is_full()) {
        // evict using clock algorithm
        free_ppn = clock.evict();
    }
    else {
        free_ppn = clock.get_free_ppn();
    }
    
    return free_ppn;
}

unsigned int disk_to_mem(const char *filename, unsigned int block) {
    // read in file into buffer
    file_read(filename, block, buffer);

    unsigned int free_ppn = evict_or_get_free_ppn();

    // write contents of buffer into memory
    std::memcpy(ppn_to_mem_addr(free_ppn) , buffer, VM_PAGESIZE);

    return free_ppn;
}

void make_page_dirty(unsigned int vpn, std::shared_ptr<PageState> page_state) {
    // update page state
    page_state->referenced = true;
    assert(page_state->resident == true);
    page_state->dirty = true;

    // update pte
    update_pte(vpn, page_state->ppn, 1, 1, page_table_base_register);
}

