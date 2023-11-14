#ifndef SMITH_H
#define SMITH_H

#include <iostream>
#include <format>

class SmithPredictor {
public:
    SmithPredictor(int counter_bits) 
        : counter_bits_(counter_bits), content_(0), max_value_((1 << counter_bits) - 1),
        predictions_(0), mispredictions_(0) 
    {
        switch (counter_bits_)
        {
        case 1:
            content_ = 1;
            break;
        case 2:
            content_ = 2;
            break;
        case 3:
            content_ = 4;
            break;
        case 4:
            content_ = 8;
            break;
        default:
            std::cerr << "Invalid counter bits!" << std::endl;
            exit(1);
            break;
        }
        
    }

    // if predict wrong, return false
    bool update(bool taken) {
        bool ret = true;
        ++predictions_;
        bool predict_taken = content_ >= ((max_value_ + 1) / 2 );
        if (predict_taken != taken) {
            ++mispredictions_;
            ret = false;
        }

        // update content
        if (taken) {
            if (content_ != max_value_) {
                ++content_;
            }
        } else {
            if (content_ != 0) {
                --content_;
            }
        }

        return ret;
    }

    int content() const {
        return content_;
    }

    void print_summary() {
        std::cout << "OUTPUT" << std::endl;
        std::cout << std::format("{:<29}", "number of predictions:") << predictions_ << std::endl;
        std::cout << std::format("{:<29}", "number of mispredictions:") << mispredictions_ << std::endl;
        std::cout << std::format("{:<29}", "misprediction rate:") << std::format("{:.2f}%", (double)mispredictions_ / predictions_ * 100) << std::endl;
        std::cout << std::format("{:<29}", "FINAL CONTENTS OF COUNTERS") << content_ << std::endl;
    }

private:
    int counter_bits_;
    int content_;
    int max_value_;
    int predictions_;
    int mispredictions_;

};

#endif // SMITH_H