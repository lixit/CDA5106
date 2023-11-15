#ifndef SMITH_H
#define SMITH_H

#include <iostream>
#include <format>

class SmithPredictor {
public:
    SmithPredictor(int counter_bits) 
        : counter_bits_(counter_bits), content_((1 << counter_bits_) / 2), max_value_((1 << counter_bits) - 1),
        predictions_(0), mispredictions_(0) 
    {   
    }

    // if predict wrong, return false
    bool predict(bool taken) {
        bool ret = true;
        
        bool predict_taken = predict_only();
    
        // update content
        update_only(taken, predict_taken);

        if (predict_taken != taken) {
            ret = false;
        }
        return ret;
    }

    // return predict result, true if taken
    bool predict_only() {
        bool predict_taken = content_ >= ((max_value_ + 1) / 2 );
        return predict_taken;
    }


    void update_only(bool taken, bool predict_taken) {
        ++predictions_;
        if (predict_taken != taken) {
            ++mispredictions_;
        }

        if (taken) {
            if (content_ != max_value_) {
                ++content_;
            }
        } else {
            if (content_ != 0) {
                --content_;
            }
        }
    }

    int content() const {
        return content_;
    }

    void print_summary() {
        std::cout << "OUTPUT" << std::endl;
        std::cout << "number of predictions:\t\t" << predictions_ << std::endl;
        std::cout << "number of mispredictions:\t" << mispredictions_ << std::endl;
        std::cout << "misprediction rate:\t\t" << std::format("{:.2f}%", (double)mispredictions_ / predictions_ * 100) << std::endl;
        std::cout << "FINAL COUNTER CONTENT:\t\t" << content_;
    }

private:
    int counter_bits_;
    int content_;
    int max_value_;
    int predictions_;
    int mispredictions_;
};

#endif // SMITH_H