#include "cache.h"
#include <iostream>
#include <format>
#include <string>
#include <cmath>

Cache::Cache(int size, int block_size, int associativity, ReplacementPolicy replacement, InclusionPolicy inclusion)
    :
    size_(size), block_size_(block_size), associativity_(associativity),
    set_count_(size / (block_size * associativity)), 
    replacement_(replacement), inclusion_(inclusion) {

    if (block_size % 2 != 0) {
        std::cerr << "block_size must be a power of 2" << std::endl;
        exit(1);
    }
    
    if (set_count_ % 2 != 0) {
        std::cerr << "set_count must be power of 2" << std::endl;
        exit(1);
    }

    sets_ = std::vector<Set>(set_count_, Set(associativity_, replacement_, inclusion_));
}

void Cache::set_child(std::shared_ptr<Cache> child) {
    child_ = child;
}
void Cache::set_parent(std::shared_ptr<Cache> parent) {
    parent_ = parent;
}

void Cache::read(const std::string &address_hex) {
    if (replacement_ == LRU)
        lru_access(address_hex, READ);
    else if (replacement_ == FIFO)
        fifo_access(address_hex, READ);
}

void Cache::write(const std::string &address_hex) {
    if (replacement_ == LRU)
        lru_access(address_hex, WRITE);
    else if (replacement_ == FIFO)
        fifo_access(address_hex, WRITE);
}

void Cache::lru_access(const std::string &address_hex, Mode mode) {
    if (mode == READ) {
        ++reads_;
    } else if (mode == WRITE) {
        ++writes_;
    }
    // address is 32 bits, 8 hex digits, 4 bits per hex digit
    // block_size_ bits is log2(block_size_). for log2(32) = 5
    // set_count_ bits is log2(set_count_). for log2(8) = 3
    // tag + index + block_offset = 32
    int address_bits = address_hex.size() * 4;
    int offset_bits = log2(block_size_);
    int index_bits = log2(set_count_);
    int tag_bits = address_bits - offset_bits - index_bits;

    int address = std::stoi(address_hex, nullptr, 16);

    // for simulator, don't care about block offset
    int index = (address >> offset_bits) & ((1 << index_bits) - 1);
    int tag = (address >> (offset_bits + index_bits)) & ((1 << tag_bits) - 1);

    index = index % set_count_;

    // valid and not dirty
    CacheBlock block{tag, true, false, address_hex};

    // 1. if hit, don't do anything
    // 2. if miss
        // 2.1 if have empty slot
            // 2.1.1 if non inclusive, don't do anything
            // 2.1.2 if inclusive, write to child
        // 2.2 if no empty slot
            // 2.2.1 if have dirty slot, write back
                // 2.2.1.1 if non inclusive, don't do anything
                // 2.2.1.2 if inclusive, write to child
            // 2.2.2 if no dirty slot, replace one
                // same as 2.2.1
    std::string victim_address;
    bool hitted = sets_[index].lru_access(block, victim_address, mode);
    if (!hitted) {
        if (mode == READ) {
            ++read_misses_;
        } else if (mode == WRITE) {
            ++write_misses_;
        }
        // CACHE issues a write request (only if there is a victim block and it is dirty)
        if (victim_address != "") {
            ++writebacks_;
            if (child_ != nullptr) {
                child_->write(victim_address);
            }
        }
        // followed by a read request
        if (child_ != nullptr) {
            child_->read(address_hex);
        }
    }
}

void Cache::fifo_access(const std::string &address_hex, Mode mode) {

}

/*
The only situa:on where CACHE must interact with the next level
below it (either another CACHE or main memory) is when the read or write request misses in
CACHE. If not miss, update the block and make it dirty.
*/

/*
CACHE should use the WBWA (write-back + write-allocate) write policy.
oWrite-allocate: A write that misses in CACHE will cause a block to be allocated in CACHE.
Therefore, both write misses and read misses cause blocks to be allocated in CACHE.
oWrite-back: A write updates the corresponding block in CACHE, making the block dirty. It
does not update the next level in the memory hierarchy (next level of cache or memory). If
a dirty block is evicted from CACHE, a writeback (i.e., a write of the en:re block) will be sent
to the next level in the memory hierarchy.

*/


/*
1. if read or write missed, allocate a block in cache
        if have empty slot, allocate one
        if dirty
            write 'victim' back to next level
        if all valid
            according to replacement policy, replace one
             
        read to the next level.
*/

void Cache::print_cache(const std::string &cache_name) {
    std::cout << "===== " << cache_name << " =====" << std::endl;
    for (int i = 0; i < set_count_; i++) {
        std::cout <<  std::format("{:<8}", "set") << std::format("{:<8}", std::to_string(i) + ":");
        for (int j = 0; j < associativity_; j++) {
            // show tag as hex
            std::string s = std::format("{:x}", sets_[i][j].tag);
            // append "D" if dirty
            s += ((sets_[i][j].dirty) ? " D" : "");
            std::cout << std::format("{:<10}", s); //
        }
        std::cout << std::endl;
    }
}

void Cache::print_summary(const std::string &cache_name, char start_char) {

    std::cout << start_char << ". " << std::format("{:<27}", "number of " + cache_name + " reads:") << reads_ << std::endl;
    std::cout << char(start_char + 1) << ". " << std::format("{:<27}", "number of " + cache_name + " read misses:") << read_misses_ << std::endl;
    std::cout << char(start_char + 2) << ". " << std::format("{:<27}", "number of " + cache_name + " writes:") << writes_ << std::endl;
    std::cout << char(start_char + 3) << ". " << std::format("{:<27}", "number of " + cache_name + " write misses:") << write_misses_ << std::endl;
    std::cout << char(start_char + 4) << ". " << std::format("{:<27}", cache_name + " miss rate:") << std::format("{:.6f}", (read_misses_ + write_misses_) / (double)(reads_ + writes_)) << std::endl;
    std::cout << char(start_char + 5) << ". " << std::format("{:<27}", "number of " + cache_name + " writebacks:") << writebacks_ << std::endl;
}


std::shared_ptr<Cache> Cache::get_child() {
    return child_;
}