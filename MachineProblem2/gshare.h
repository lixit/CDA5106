#ifndef GSHARE_H
#define GSHARE_H

#include "smith.h"
#include <vector>

class Gshare {
public:
    Gshare(int m, int n) 
        : m_(m), n_(n), max_n_((1 << n) - 1), shift_register_(0),
         predictions_(0), mispredictions_(0) {   

        if (n > m) {
            std::cerr << "n must <= m!" << std::endl;
            exit(1);
        }

        for (int i = 0; i < (1 << m); ++i) {
            gshare_.push_back(SmithPredictor(3));
        }

            
    }

    void predict(const std::string &address, bool taken) {
        ++predictions_;

        int address_int = std::stoi(address, nullptr, 16);

        // use m+1 to 2 bits of pc
        int pc_index = (address_int & ((1 << m_ + 2) - 1) ) >> 2;

        int index;
        if (n_ == 0) {
            index = pc_index;
        } else {
            index = (pc_index ^ shift_register_) & max_n_;
        }
         

        bool ret = gshare_[index].update(taken);
        if (!ret) {
            ++mispredictions_;
        }

        if (n_ != 0) {
            // update shift register
            shift_register_ = (shift_register_ >> 1) | (int)taken << (n_ - 1);
        }

    }

    void print_summary() {
        std::cout << "OUTPUT" << std::endl;
        std::cout << std::format("{:<29}", "number of predictions:") << predictions_ << std::endl;
        std::cout << std::format("{:<29}", "number of mispredictions:") << mispredictions_ << std::endl;
        std::cout << std::format("{:<29}", "misprediction rate:") << std::format("{:.2f}%", (double)mispredictions_ / predictions_ * 100) << std::endl;
        if (n_ != 0)
            std::cout << std::format("{:<29}", "FINAL GSHARE CONTENTS") << std::endl;
        else
            std::cout << std::format("{:<29}", "FINAL BIMODAL CONTENTS") << std::endl;

        for (int i = 0; i < gshare_.size(); ++i) {
            std::cout << std::format("{:<5}", std::to_string(i)) << gshare_[i].content() << std::endl;
        }
    }


private:
    
    int m_;
    int n_;

    // max value of shift register
    int max_n_;
    int shift_register_;

    int predictions_;
    int mispredictions_;

    std::vector<SmithPredictor> gshare_;
};


#endif // !GSHARE_H