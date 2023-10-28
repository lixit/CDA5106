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

struct CacheBlock {
    int tag = 0;
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

    void fifo_read(int tag);
    void fifo_write(int tag);

    // if missed, return true, and the address of the block to be write to child
    bool lru_read(const CacheBlock &block, std::string &viticm_hex);
    bool lru_write(const CacheBlock &block, std::string &viticm_hex);

    CacheBlock& operator[](int);

private:
    void fifo_push(const CacheBlock &block);
    CacheBlock fifo_pop();
    bool fifo_full();
    bool fifo_hit(int tag);

    void lru_push(const CacheBlock &block);
    int lru_empty_index();
    int dirty_index();
    bool lru_hit(const CacheBlock &block, bool make_dirty = false);
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