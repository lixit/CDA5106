#ifndef SET_H
#define SET_H

#include <vector>
#include <string>

enum ReplacementPolicy {
        LRU,
        FIFO,
        RANDOM
};

enum InclusionPolicy {
    NON_INCLUSIVE,
    INCLUSIVE,
    EXCLUSIVE,
};

enum Mode {
    READ,
    WRITE
};

struct CacheBlock {
    std::string tag;
    // a valid bit to the tag to say whether or not this entry contains a valid address.
    // If the bit is not set, there cannot be a match on this address
    bool valid = false;
    // If it is clean, the block is not written back on a miss
    bool dirty = false;
    std::string address_hex;
};

class Set {
public:
    Set(int associativity, ReplacementPolicy replace, InclusionPolicy inclusion = NON_INCLUSIVE);
    ~Set() = default;

    bool fifo_access(const CacheBlock &block, std::string &viticm_hex, Mode mode, bool &set_dirty, bool &victim_dirty);

    // if missed, return true, and the address of the block to be write to child
    bool lru_access(const CacheBlock &block, std::string &viticm_hex, Mode mode, bool &set_dirty, bool &victim_dirty);

    CacheBlock& operator[](int);

private:
    int fifo_hit_index(const CacheBlock &block);
    int fifo_empty_index();

    int lru_empty_index();
    int dirty_index();
    int lru_hit_index(const CacheBlock &block);
    void set_row_unset_column(int i);
    int all_0_row();


    std::vector<CacheBlock> blocks_;
    std::vector<std::vector<bool>> lru_matrix_;
    
    int associativity_;
    ReplacementPolicy replace_;
    InclusionPolicy inclusion_;


    // Used for FIFO
    int first_;
    int last_;
    int count_;
};

#endif