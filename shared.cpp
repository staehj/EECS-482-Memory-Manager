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

void* ppn_to_mem(unsigned int ppn) {
    return vm_physmem+(ppn*VM_PAGESIZE);
}