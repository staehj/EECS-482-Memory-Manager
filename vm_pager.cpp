#include "vm_pager.h"

#include <cstring>

#include "page_state.h"
#include "shared.h"

extern PageState* page_states;

void vm_init(unsigned int memory_pages, unsigned int swap_blocks) {
  // Initialize 0 page
  memset(vm_physmem, 0, VM_PAGESIZE);



}

int vm_create(pid_t parent_pid, pid_t child_pid {

}

void vm_switch(pid_t pid) {
  page_table_base_register = page_tables[pid];
}

int vm_fault(const void* addr, bool write_flag) {

}

void vm_destroy() {

}

void *vm_map(const char *filename, unsigned int block) {

}
