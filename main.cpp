#include <iostream>
#include <future>

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

class WeatherData {
public:
    WeatherData(string content) {
        this->content = content;
    }

    string description() {
        return content;
    }

private:
    string content;
};

WeatherData *get_data(int i) {
    int s = 500000 + rand() % 2000000;
    stringstream ss;
    usleep(s);
    ss << "hey " << s;
    return new WeatherData(ss.str());
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

    vector<future<WeatherData*> > futures;
    futures.push_back(async(launch::async, get_data, 1));
    futures.push_back(async(launch::async, get_data, 2));
    futures.push_back(async(launch::async, get_data, 3));
    futures.push_back(async(launch::async, get_data, 4));

    vector<WeatherData> data;
    for (auto &f: futures) {
        auto wd = *f.get();
        data.push_back(wd);
        cout << wd.description() << endl;
    }

    cout << data.size() << endl;
    return 0;
}
