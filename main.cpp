#include <iostream>
#include <future>

#include "boost/program_options.hpp"
#include "boost/asio.hpp"

using namespace std;
namespace po = boost::program_options;
using boost::asio::ip::tcp;

const string HOSTNAME = "api.openweathermap.org";
const string BASEPATH = "/data/2.5/weather?q=";

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
    WeatherData(string loc, string raw) {
        this->loc = loc;
        this->raw = raw;
    }

    string description() const {
        return this->loc + ": " + this->raw;
    }

    string location() const {
        return this->loc;
    }

private:
    string raw;
    string loc;
};

WeatherData *get_data(string location) {
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    string path = BASEPATH + location;
    tcp::resolver::query query(HOSTNAME, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << HOSTNAME << "\r\n";
    request_stream << "Accept: application/json\r\n";
    request_stream << "Connection: close\r\n\r\n";
    boost::asio::write(socket, request);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      std::cout << "Invalid response\n";
      return nullptr;
    }
    if (status_code != 200)
    {
      std::cout << "Response returned with status code " << status_code << "\n";
      return nullptr;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
      ;

    stringstream content;
    // Write whatever content we already have to output.
    if (response.size() > 0)
      content << &response;

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while (boost::asio::read(socket, response,
          boost::asio::transfer_at_least(1), error))
      content << &response;
    if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);

    return new WeatherData(location, content.str());
}

int main(int argc, char **argv) {
    // First, parse input arguments
    po::variables_map vm = parse_args(argc, argv);

    // ... and quit if only 'help' was wanted
    if (vm.count("help")) {
        return 1;
    }

    // Ensure that we actually want to get weather for 1-or-more locations
    if (vm.count("location") < 1) {
        cerr << "No locations given" << endl;
        return 2;
    }
    auto locations = vm["location"].as<vector<string> >();

    // Check sorting order
    if (vm.count("temp")) {
        cout << "All righty, we'll sort by temperature instead of location" << endl;
    }

    // Fetch location data asynchronously
    vector<future<WeatherData*> > futures;
    for (auto location: locations)
    {
        futures.push_back(async(launch::async, get_data, location));
    }

    // Wait for the data to come back
    vector<WeatherData> data;
    for (auto &f: futures) {
        auto wd = f.get();
        if (wd) {
            data.push_back(*wd);
        }
    }

    // Set up and select sorting lambda to use (based on "--temp" input arg)
    auto location_sorter = [](const WeatherData l, const WeatherData r) -> bool {
        return l.location() < r.location();
    };
    auto temperature_sorter = [](const WeatherData l, const WeatherData r) -> bool {
        return l.location() > r.location(); // FIXME
    };
    auto selected_sorter = vm.count("temp") ? temperature_sorter : location_sorter;

    // Sort the data
    sort(data.begin(), data.end(), selected_sorter);

    // .. and finally display it
    for (auto wd: data) {
        cout << wd.description() << endl;
    }
    return 0;
}
