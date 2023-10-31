#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <deque>
#include <memory>
#include "set.h"

class Cache {

public:
    Cache(int size, int block_size, int associativity, ReplacementPolicy replacement = LRU,
        InclusionPolicy inclusion = NON_INCLUSIVE);
    ~Cache() = default;

    void read(const std::string &address_hex);
    void write(const std::string &address_hex);

    void set_child(std::shared_ptr<Cache> child);
    void set_parent(std::shared_ptr<Cache> parent);
    std::shared_ptr<Cache> get_child();

    void print_cache(const std::string &cache_name);
    void print_summary(const std::string &cache_name, char start_char);
    void print_debug(const std::string &cache_name);

private:
    void access(const std::string &address_hex, Mode mode);

private:
    int size_;
    int block_size_;
    int associativity_;
    int set_count_;

    // data structure for FIFO
    // std::vector<Set> fifo_queue_;

    // data structure for LRU
    std::vector<Set> sets_;
    
    // std::vector<std::vector<CacheBlock>> cache_;
    // std::vector<std::vector<bool>> lru_matrix_;

    // policies
    ReplacementPolicy replacement_;
    InclusionPolicy inclusion_;

    // child and parent
    std::shared_ptr<Cache> child_;
    std::shared_ptr<Cache> parent_;

    // for debug output
    int count_ = 0;
    CacheBlock current_block_;
    std::string current_effective_address_;
    int current_set_index_ = 0;
    Mode current_mode_;
    std::string current_victim_;
    bool current_set_dirty_ = false;
    bool current_missed_ = false;
    bool current_victim_dirty_ = false;
    std::string current_victim_tag_;
    int current_victim_index_ = 0;
    std::string current_victim_effective_address_;

    int reads_ = 0;
    int read_misses_ = 0;
    int writes_ = 0;
    int write_misses_ = 0;
    int writebacks_ = 0;
};


#endif