#ifndef HYBRID_H
#define HYBRID_H

#include "smith.h"
#include "gshare.h"

class Hybrid {
public:
    Hybrid(int k, int m1, int n, int m2) 
        : k_(k), m1_(m1), n_(n), m2_(m2), max_n_((1 << n) - 1), chooser_table_(1 << k, 1),
         gshare_(m1, n), bimodal_(m2, 0), predictions_(0), mispredictions_(0) {   

        if (n > m1) {
            std::cerr << "n must <= m1!" << std::endl;
            exit(1);
        }

            
    }

    void predict(const std::string &address, bool taken) {
        ++predictions_;

        bool gshare_taken = gshare_.predict_only(address);
        bool bimodal_taken = bimodal_.predict_only(address);

        int address_int = std::stoi(address, nullptr, 16);

        // use k+1 to 2 bits of pc
        int chooser_index = (address_int & ((1 << (k_ + 2)) - 1) ) >> 2;

        bool overall_prediction = false;
        if (chooser_table_[chooser_index] >= 2) {
            overall_prediction = gshare_taken;
            gshare_.update_only(taken, gshare_taken, address);
        } else {
            overall_prediction = bimodal_taken;
            bimodal_.update_only(taken, bimodal_taken, address);
            // gshare's shift register must be updated, even if bimodal is chosen
            gshare_.update_shift_register(taken);
        }

        if (overall_prediction != taken) {
            ++mispredictions_;
        }

        // update chooser table
        if (gshare_taken == bimodal_taken) { // both correct or both wrong
            // do nothing
            ;
        } else if (gshare_taken == taken) { // gshare correct, bimodal wrong
            if (chooser_table_[chooser_index] != 3) {
                ++chooser_table_[chooser_index];
            }
        } else {
            if (chooser_table_[chooser_index] != 0) { // gshare wrong, bimodal correct
                --chooser_table_[chooser_index];
            }
        }


    }

    void print_summary() {
        std::cout << "OUTPUT" << std::endl;
        std::cout << "number of predictions:\t\t" << predictions_ << std::endl;
        std::cout << "number of mispredictions:\t" << mispredictions_ << std::endl;
        std::cout << "misprediction rate:\t\t" << std::format("{:.2f}%", (double)mispredictions_ / predictions_ * 100) << std::endl;
        std::cout << "FINAL CHOOSER CONTENTS" << std::endl;
        for (size_t i = 0; i < chooser_table_.size(); ++i) {
            std::cout << std::to_string(i) << "\t" << chooser_table_[i] << std::endl;
        }
        gshare_.print_content();
        bimodal_.print_content();
    }

private:
    int k_;

    int m1_;
    int n_;

    int m2_;

    int max_n_;
    
    // using a chooser table of 2^k 2-bit counters. All counters are initialized to 01.
    std::vector<int> chooser_table_;

    Gshare gshare_;
    Gshare bimodal_;

    int predictions_;
    int mispredictions_;
};







#endif // HYBRID_H