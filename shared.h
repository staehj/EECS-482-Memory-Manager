#ifndef SHARED_H
#define SHARED_H

#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "page_state.h"
#include "clock.h"
#include "swap_manager.h"
#include "vm_pager.h"


extern std::unordered_map<std::string,
  std::unordered_map<unsigned int, std::shared_ptr<PageState>>> file_table;
extern std::unordered_map<page_table_t*, std::vector<std::shared_ptr<PageState>>> arenas;
extern std::unordered_map<pid_t, page_table_t*> page_tables;
extern std::vector<std::shared_ptr<PageState>> phys_mem_pages;
extern Clock clock_;
extern SwapManager swap_manager;

void sanitize_page_table(page_table_t* pt);

bool file_in_file_table(const char* filename, unsigned int block);

bool file_is_resident(const char* filename, unsigned int block);

void remove_file_table_entry(const char* filename, unsigned int block);

unsigned int vpn_to_ppn(unsigned int vpn);

void update_pte(unsigned int vpn, unsigned int ppn,
                unsigned int read, unsigned int write, page_table_t* ptbr);

intptr_t ppn_to_mem_addr(unsigned int ppn);

unsigned int va_to_vpn(const void* va);

unsigned int evict_or_get_free_ppn();

unsigned int disk_to_mem(const char *filename, unsigned int block);

void make_page_dirty(unsigned int vpn, std::shared_ptr<PageState> page_state);

unsigned int last_addr_in_ppn(unsigned int ppn);

const char* get_filename(const char* va);

bool filename_valid_in_arena(const char* filename);

unsigned int lowest_invalid_vpn();

#endif
