#include "set.h"

Set::Set(int associativity, ReplacementPolicy replace, InclusionPolicy inclusion) 
    : associativity_(associativity), replace_(replace), inclusion_(inclusion),first_(0), last_(0), count_(0) {

    
    blocks_ = std::vector<CacheBlock>(associativity_, CacheBlock());
    if (replace_ == LRU) {
        lru_matrix_ = std::vector<std::vector<bool>>(associativity_, std::vector<bool>(associativity_, false));
    }
}


int Set::fifo_hit_index(const CacheBlock &block) {
    for (int i = 0; i < associativity_; ++i) {
        if (block.tag == blocks_[i].tag) {
            return i;
        }
    }
    return -1;
}

int Set::fifo_empty_index() {
    if (count_ != associativity_) {
        return last_;
    }
    return -1;
}

// return `if hit`, if hit return true, if miss return false
bool Set::lru_access(const CacheBlock &block, std::string &viticm_hex, Mode mode, bool &set_dirty, bool &victim_dirty, int &writeback_memory) {
    // for debug
    set_dirty = false;
    victim_dirty = false;
    if (mode == INVALIDATE && -1 == lru_hit_index(block)) { // invalidate miss, do nothing
        return true;
    }
    if (-1 != lru_hit_index(block)) {  // hit
        int hit_index = lru_hit_index(block);
        set_row_unset_column(hit_index);
        if (mode == WRITE) {
            blocks_[hit_index] = block; // stupid mistake
            blocks_[hit_index].dirty = true;
            set_dirty = true;
        } else if (mode == INVALIDATE) {
            if (blocks_[hit_index].dirty) {
                ++writeback_memory; // L1 block to be invalidated is dirty, write to main memory directly.
            }
            blocks_[hit_index].valid = false;
        }
        return true;
    } else if (-1 != lru_empty_index()) { // not full, push

        const int empty_index = lru_empty_index();
        blocks_[empty_index] = block;
        set_row_unset_column(empty_index);

        if (mode == WRITE) {
            blocks_[empty_index].dirty = true;
            set_dirty = true;
        }

    } else { // full
    
        int victim_index = all_0_row();
        set_row_unset_column(victim_index);
        viticm_hex = blocks_[victim_index].address_hex;
        victim_dirty = blocks_[victim_index].dirty;
        blocks_[victim_index] = block;

        if (mode == WRITE) {
            blocks_[victim_index].dirty = true;
            set_dirty = true;
        }
    }

    return false;
}

// return hit, if hit return true, if miss return false
bool Set::fifo_access(const CacheBlock &block, std::string &viticm_hex, Mode mode, bool &set_dirty, bool &victim_dirty, int &writeback_memory) {
    // for debug
    set_dirty = false;
    victim_dirty = false;
    int hit_index = fifo_hit_index(block);
    if (mode == INVALIDATE && -1 == hit_index) { // invalidate miss, do nothing
        return true;
    }
    if (-1 != hit_index) { // hit
        if (mode == WRITE) {
            blocks_[hit_index].dirty = true;
            set_dirty = true;
        } else if (mode == INVALIDATE) {
            if (blocks_[hit_index].dirty) {
                ++writeback_memory; // L1 block to be invalidated is dirty, write to main memory directly.
            }
            blocks_[hit_index].valid = false;
        }
        return true;

    } else if (-1 != fifo_empty_index()) { // miss, not full, push
        const int empty_index = fifo_empty_index();
        blocks_[empty_index] = block;
        if (mode == WRITE) {
            blocks_[empty_index].dirty = true;
            set_dirty = true;
        }
        // update fifo pointers
        ++last_;
        ++count_;
    } else { // miss, full, fifo
        int victim_index = first_;
        int push_index = last_ % associativity_;

        viticm_hex = blocks_[victim_index].address_hex;
        victim_dirty = blocks_[victim_index].dirty;
        blocks_[push_index] = block;
        if (mode == WRITE) {
            blocks_[push_index].dirty = true;
            set_dirty = true;
        }

        // update fifo pointers
        first_ = (first_ + 1) % associativity_;
        last_ = (last_ + 1) % associativity_;
    }

    return false;
}

// if hit, update the lru matrix
int Set::lru_hit_index(const CacheBlock &block) {
    for (int i = 0; i < associativity_; i++) {
        if (blocks_[i].valid && blocks_[i].tag == block.tag) {
            return i;
        }
    }
    return -1;
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