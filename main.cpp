#include <iostream>

#include "boost/program_options.hpp"

using namespace std;
namespace po = boost::program_options;

po::variables_map parse_args(int argc, char **argv) {
    po::options_description desc("Accepted options");
    desc.add_options()
        ("help", "produce help message")
        ("temp", "sort by temperature instead of location")
        ("location", po::value<vector<string> >(), "location(s)")
    ;

    po::positional_options_description p;
    p.add("location", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
    }

    return vm;
}

int main(int argc, char **argv) {
    po::variables_map vm = parse_args(argc, argv);
    if (vm.count("help")) {
        return 1;
    }

    if (vm.count("location") == 0) {
        cerr << "No locations given" << endl;
        return 2;
    }

    if (vm.count("temp")) {
        cout << "All righty, we'll sort by temperature instead of location" << endl;
    }

    return 0;
}
