#include <iostream>
#include <getopt.h>
#include <format>
#include <fstream>
#include <sstream>
#include "cache.h"

namespace {

void usage(const std::string& program_name) {
    std::cerr << "Usage: " << program_name << " <BLOCKSIZE> <L1_SIZE> <L1_ASSOC> <L2_SIZE> <L2_ASSOC> <REPLACEMENT_POLICY> <INCLUSION_PROPERTY> <trace_ﬁle>" << std::endl;
}

} // namespace

// ./sim_cache 32 8192 4 262144 8 0 0 ../traces/gcc_trace.txt

int main(int argc, char* argv[]) {

    // for (int i = 0; i < argc; ++i) {
    //     std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    // }

    // Parse the command line arguments
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
    if (argc - optind != 8) {
        std::cerr << "Argument count must be 8!" << std::endl;
        usage(argv[0]);
        exit(1);
    } else {
        int block_size, l1_size, l1_assoc, l2_size, l2_assoc;
        try {
            block_size = std::stoi(argv[optind]);
            l1_size = std::stoi(argv[optind + 1]);
            l1_assoc = std::stoi(argv[optind + 2]);
            l2_size = std::stoi(argv[optind + 3]);
            l2_assoc = std::stoi(argv[optind + 4]);
        } catch (std::invalid_argument const& ex)
        {
            std::cout << "std::invalid_argument::what(): " << ex.what() << '\n';
            exit(1);
        }
        catch (std::out_of_range const& ex)
        {
            std::cout << "std::out_of_range::what(): " << ex.what() << '\n';
            exit(1);
        }
        std::string replacement_policy(argv[optind + 5]);
        std::string inclusion_property(argv[optind + 6]);
        std::string trace_file(argv[optind + 7]);

        std::cout << "===== Simulator configuration =====" << std::endl;
        std::cout << std::format("{:<23}", "BLOCKSIZE = ") << block_size << std::endl;
        std::cout << std::format("{:<23}", "L1_SIZE = ") << l1_size << std::endl;
        std::cout << std::format("{:<23}", "L1_ASSOC = ") << l1_assoc << std::endl;
        std::cout << std::format("{:<23}", "L2_SIZE = ") << l2_size << std::endl;
        std::cout << std::format("{:<23}", "L2_ASSOC = ") << l2_assoc << std::endl;
        std::cout << std::format("{:<23}", "REPLACEMENT_POLICY = ") << replacement_policy << std::endl;
        std::cout << std::format("{:<23}", "INCLUSION_PROPERTY = ") << inclusion_property << std::endl;
        std::cout << std::format("{:<23}", "trace_file = ") << trace_file << std::endl;
        std::cout << "===================================" << std::endl;

        ReplacementPolicy replacement;
        if (replacement_policy == "0") {
            replacement = LRU;
        } else if (replacement_policy == "1") {
            replacement = FIFO;
        } else {
            std::cerr << "Invalid replacement policy!" << std::endl;
            exit(1);
        }
        InclusionPolicy inclusion;
        if (inclusion_property == "0") {
            inclusion = NON_INCLUSIVE;
        } else if (inclusion_property == "1") {
            inclusion = INCLUSIVE;
        } else if (inclusion_property == "2") {
            inclusion = EXCLUSIVE;
        } else {
            std::cerr << "Invalid inclusion policy!" << std::endl;
            exit(1);
        }

        // Create the cache hierarchy
        Cache l1 = Cache(l1_size, block_size, l1_assoc, replacement, inclusion);
        if (l2_size != 0) {
            auto l2 = std::make_shared<Cache>(l2_size, block_size, l2_assoc, replacement, inclusion);
            l1.set_child(l2);
        }

        // Read the trace file, and start the simulation
        std::ifstream infile(trace_file);
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            char operation;
            std::string address;
            if (!(iss >> operation >> address)) { 
                std::cerr << "Invalid trace file!" << std::endl;
                exit(1);
                break;
            }

            // std::cout << operation << " " << std::stoi(address, nullptr, 16) << std::endl;

            if (operation == 'r') {
                l1.read(address);
            } else if (operation == 'w') {
                l1.write(address);
            } else {
                std::cerr << "Invalid operation!" << std::endl;
                exit(1);
            }
        }
        l1.print_cache("L1 contents");
        if (l2_size != 0) {
            l1.get_child()->print_cache("L2 contents");
        }
        l1.print_summary("L1");
    }

}
