#include "shared.h"

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
