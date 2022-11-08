#include <memory>
#include <unordered_map>
#include <vector>

#include "page_state.h"
#include "file_block.h"
#include "clock.h"
#include "swap_manager.h"
#include "vm_pager.h"


extern std::unordered_map<FileBlock, std::shared_ptr<PageState>> file_table;
extern std::unordered_map<page_table_t*, std::vector<std::shared_ptr<PageState>>> arenas;
extern std::unordered_map<pid_t, page_table_t*> page_tables;
extern std::unordered_map<page_table_t*, unsigned int> lowest_invalid_vpns;
extern Clock clock;
extern SwapManager swap_manager;
extern char buffer[VM_PAGESIZE];

unsigned int vpn_to_ppn(unsigned int vpn);

bool filename_valid_in_arena(const char* filename, unsigned int lowest_invalid_vpn);

void update_pte(unsigned int vpn, unsigned int ppn,
                unsigned int read, unsigned int write, page_table_t* ptbr);
