#include <iostream>

#include "boost/program_options.hpp"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char **argv) {

    // Declare the supported options.
    po::options_description desc("Accepted options");
    desc.add_options()
        ("help", "produce help message")
        ("temperature", "sort by temperature")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("temperature")) {
        cout << "All righty, we'll sort by temperature.\n";
    } else {
        cout << "We'll sort randomly. Like a bawse.\n";
    }

    return 0;
}
