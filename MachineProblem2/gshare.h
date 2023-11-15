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

    // if predict wrong, return false
    bool predict(const std::string &address, bool taken) {
        bool ret = true;

        bool predict_taken = predict_only(address);
        if (predict_taken != taken) {
            ret = false;
        }

        update_only(taken, predict_taken, address);

        return ret;
    }

    // return predict result, true if taken
    bool predict_only(const std::string &address) {
        int index = gshare_index(address);

        bool predict_taken = gshare_[index].predict_only();
        return predict_taken;
    }

    void update_only(bool taken, bool predict_taken, const std::string &address) {
        ++predictions_;
        if (predict_taken != taken) {
            ++mispredictions_;
        }

        int index = gshare_index(address);

        gshare_[index].update_only(taken, predict_taken);

        if (n_ != 0) {
            // most significant bit of shift register
            int msb = (int)taken << (n_ - 1);
            shift_register_ = (shift_register_ >> 1) | msb;
        }

    }

    void update_shift_register(bool taken) {
        if (n_ != 0) {
            // most significant bit of shift register
            int msb = (int)taken << (n_ - 1);
            shift_register_ = (shift_register_ >> 1) | msb;
        }
    }

    void print_summary() {
        std::cout << "OUTPUT" << std::endl;
        std::cout << "number of predictions:\t\t" << predictions_ << std::endl;
        std::cout << "number of mispredictions:\t" << mispredictions_ << std::endl;
        std::cout << "misprediction rate:\t\t" << std::format("{:.2f}%", (double)mispredictions_ / predictions_ * 100) << std::endl;
        print_content();
    }
    void print_content() {
        if (n_ != 0)
            std::cout << "FINAL GSHARE CONTENTS" << std::endl;
        else
            std::cout << "FINAL BIMODAL CONTENTS" << std::endl;

        for (size_t i = 0; i < gshare_.size(); ++i) {
            std::cout << std::to_string(i) << "\t" << gshare_[i].content() << std::endl;
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

    int gshare_index(const std::string &address) {
        int address_int = std::stoi(address, nullptr, 16);

        // use m+1 to 2 bits of pc
        int pc_index = (address_int & ((1 << (m_ + 2)) - 1) ) >> 2;

        int index;
        if (n_ == 0) { // bimodal
            index = pc_index;
        } else {  // gshare
            // m+1 to 2 bits of pc xor shift register
            index = (pc_index ^ shift_register_);
        }
        return index;
    }
};


#endif // !GSHARE_H