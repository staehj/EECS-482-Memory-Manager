#include <unordered_map>
#include <vector>

#include "page_state.h"
#include "file_block.h"

extern std::unordered_map<FileBlock, PageState> file_table;
extern std::unordered_map<pid_t, std::vector<PageState>> swap_tables;
extern std::unordered_map<pid_t, page_table_t*> page_tables;
