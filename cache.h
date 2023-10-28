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

private:
    void lru_access(const std::string &address_hex, Mode mode);
    void fifo_access(const std::string &address_hex, Mode mode);

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

    int reads_ = 0;
    int read_misses_ = 0;
    int writes_ = 0;
    int write_misses_ = 0;
    int writebacks_ = 0;
};


#endif