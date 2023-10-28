#include "set.h"

Set::Set(int associativity, ReplacementPolicy replace, InclusionPolicy inclusion) 
    : associativity_(associativity), replace_(replace), inclusion_(inclusion),first_(0), last_(0), count_(0) {

    
    blocks_ = std::vector<CacheBlock>(associativity_, CacheBlock());
    lru_matrix_ = std::vector<std::vector<bool>>(associativity_, std::vector<bool>(associativity_, false));
}

bool Set::fifo_full() {
    return count_ == associativity_;
}

bool Set::fifo_hit(int tag) {
    for (int i = 0; i < associativity_; i++) {
        if (blocks_[i].valid && blocks_[i].tag == tag) {
            return true;
        }
    }
    return false;
}

void Set::fifo_push(const CacheBlock &block) {
    if (fifo_full()) {
        fifo_pop();
    }
    blocks_[last_] = block;
    last_ = (last_ + 1) % associativity_;
    count_++;
}

CacheBlock Set::fifo_pop() {
    CacheBlock block = blocks_[first_];
    blocks_[first_] = CacheBlock();
    first_ = (first_ + 1) % associativity_;
    count_--;
    return block;
}

// only viticm and dirty need to write to child. 
bool Set::lru_read(const CacheBlock &block, std::string &viticm_hex) {
    // lru_hit always update lru matrix
    if (lru_hit(block)) {  // hit
        // hit
        return true;
    } else if (-1 != lru_empty_index()) { // not full, push
        lru_push(block);

    } else if (-1 != dirty_index()) { // have dirty index
        int ditry_index = dirty_index();
        // write to child
        viticm_hex = blocks_[ditry_index].address_hex;
        // replace
        blocks_[ditry_index] = block;

    } else { // full, no dirty index
        int victim_index = all_0_row();
        
        // replace
        blocks_[victim_index] = block;
    }

    return false;
}

// only viticm and dirty need to write to child.
bool Set::lru_write(const CacheBlock &block, std::string &viticm_hex) {
    if (lru_hit(block, true)) {  // hit
        // hit and make dirty
        return true;
    } else if (-1 != lru_empty_index()) { // not full, push
        lru_push(block);
        // non inclusive, don't do anythin
        // if inclusive, write to child
    } else if (-1 != dirty_index()) { // have dirty index
        int ditry_index = dirty_index();
        CacheBlock victim = blocks_[ditry_index];
        // must write to child
        viticm_hex = victim.address_hex;
        // replace
        blocks_[ditry_index] = block;
        // if inclusive, write to child

    } else { // full, no dirty index
        int victim_index = all_0_row();
        // if non-inclusive, don't do anything
        blocks_[victim_index] = block;
    }

    return false;
}

// if hit, update the lru matrix
bool Set::lru_hit(const CacheBlock &block, bool make_dirty) {
    for (int i = 0; i < associativity_; i++) {
        if (blocks_[i].valid && blocks_[i].tag == block.tag) {
            // set the row, unset the column
            set_row_unset_column(i);
            if (make_dirty) {
                blocks_[i].dirty = true;
            }
            return true;
        }
    }
    return false;
}

void Set::lru_push(const CacheBlock &block) {
    int empty_index = lru_empty_index();
    blocks_[empty_index] = block;
    // new block is dirty
    blocks_[empty_index].dirty = true;
    set_row_unset_column(empty_index);
}

// return empty index
int Set::lru_empty_index() {
    for (int i = 0; i < associativity_; i++) {
        if (!blocks_[i].valid) {
            return i;
        }
    }
    return -1;
}

int Set::dirty_index() {
    for (int i = 0; i < associativity_; i++) {
        if (blocks_[i].valid && blocks_[i].dirty) {
            return i;
        }
    }
    return -1;
}

void Set::set_row_unset_column(int i) {
     for (int j = 0; j < associativity_; j++) {
        // set the row
        lru_matrix_[i][j] = true;
        // unset the column
        lru_matrix_[j][i] = false;
    }
}

int Set::all_0_row() {
    for (int i = 0; i < associativity_; i++) {
        bool all_0 = true;
        for (int j = 0; j < associativity_; j++) {
            if (lru_matrix_[i][j]) {
                all_0 = false;
                break;
            }
        }
        if (all_0) {
            return i;
        }
    }
    return -1;
}


CacheBlock& Set::operator[](int index) {
    return blocks_[index];
}