#include <iostream>
#include <string>
#include <getopt.h>
#include <format>
#include <fstream>
#include <sstream>
#include "smith.h"
#include "gshare.h"
#include "hybrid.h"

namespace {

void usage(const std::string& program_name) {
    std::cerr << "Usage: " << std::endl 
        << program_name << " smith <B> <tracefile>" << std::endl << 
        program_name << " bimodal <M2> <tracefile>" << std::endl <<
        program_name << " gshare <M1> <N> <tracefile>" << std::endl <<
        program_name << " hybrid <K> <M1> <N> <M2> <tracefile>" << std::endl;
}

} // namespace


int main(int argc, char *argv[]) {

    std::cout << "COMMAND" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;
    
    int opt;
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
                exit(0);
            default:
                usage(argv[0]);
                exit(1);
        }
    }
    if (optind + 3 > argc) {
        std::cerr << "Argument count must >= 3!" << std::endl;
        usage(argv[0]);
        exit(1);
    }

    std::string predictor(argv[optind]);
    if (predictor == "smith") {
        int counter_bits = std::stoi(argv[optind + 1]);
        std::string trace_file(argv[optind + 2]);

        SmithPredictor smith_predictor(counter_bits);

        // Read the trace file, and start the simulation
        std::ifstream infile(trace_file);
        if (!infile.is_open()) {
            std::cerr << "Invalid trace file!" << std::endl;
            exit(1);
        }
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string address;
            std::string ground_truth;
            if (!(iss >> address >> ground_truth)) { 
                std::cerr << "Invalid trace file!" << std::endl;
                exit(1);
                break;
            }
            smith_predictor.predict(ground_truth == "t");
        }

        smith_predictor.print_summary();


    } else if (predictor == "bimodal") {
        int pc_bits = std::stoi(argv[optind + 1]);
        std::string tracefile(argv[optind + 2]);

        Gshare gshare(pc_bits, 0);

        std::ifstream infile(tracefile);
        if (!infile.is_open()) {
            std::cerr << "Invalid trace file!" << std::endl;
            exit(1);
        }
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string address;
            std::string ground_truth;
            if (!(iss >> address >> ground_truth)) { 
                std::cerr << "Invalid trace file!" << std::endl;
                exit(1);
                break;
            }
            gshare.predict(address, ground_truth == "t");
        }

        gshare.print_summary();

    } else if (predictor == "gshare") {
        int pc_bits = std::stoi(argv[optind + 1]);
        int history_bits = std::stoi(argv[optind + 2]);
        std::string tracefile(argv[optind + 3]);


        Gshare gshare(pc_bits, history_bits);

        std::ifstream infile(tracefile);
        if (!infile.is_open()) {
            std::cerr << "Invalid trace file!" << std::endl;
            exit(1);
        }
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string address;
            std::string ground_truth;
            if (!(iss >> address >> ground_truth)) { 
                std::cerr << "Invalid trace file!" << std::endl;
                exit(1);
                break;
            }
            gshare.predict(address, ground_truth == "t");
        }

        gshare.print_summary();


    } else if (predictor == "hybrid") {
        // the number of PC bits used to index the chooser table
        int k = std::stoi(argv[optind + 1]);

        // same as gshare
        int pc_bits = std::stoi(argv[optind + 2]);
        int history_bits = std::stoi(argv[optind + 3]);

        //the number of PC bits used to index the bimodal table.
        int m2 = std::stoi(argv[optind + 4]);

        std::string tracefile(argv[optind + 5]);
        std::ifstream infile(tracefile);
        if (!infile.is_open()) {
            std::cerr << "Invalid trace file!" << std::endl;
            exit(1);
        }
        Hybrid hybrid(k, pc_bits, history_bits, m2);

        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string address;
            std::string ground_truth;
            if (!(iss >> address >> ground_truth)) { 
                std::cerr << "Invalid trace file!" << std::endl;
                exit(1);
                break;
            }
            hybrid.predict(address, ground_truth == "t");
        }

        hybrid.print_summary();

    } else {
        std::cerr << "Invalid predictor type!" << std::endl;
        usage(argv[0]);
        exit(1);
    }

    
    return 0;
}
