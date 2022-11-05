#include <memory>
#include <unordered_map>
#include <vector>

#include "page_state.h"
#include "file_block.h"
#include "clock.h"

extern std::unordered_map<FileBlock, std::shared_ptr<PageState>> file_table;
extern std::unordered_map<page_table_t*, std::vector<std::shared_ptr<PageState>>> arenas;
extern std::unordered_map<pid_t, page_table_t*> page_tables;
extern Clock clock;