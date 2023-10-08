#include "parser.h"
#include "watcher.h"

void oneWay(const std::string& source, const std::string& destination) {

    if (!fs::exists(source)) { throw std::runtime_error(source + " does not exists."); }
    else if (!fs::exists(destination)) { throw std::runtime_error(destination + " does not exists."); }

    saveNewPair(source, destination);
}

int main(int argc, char const *argv[])
{
    CommandLineParser parser;

    CommandLineParser::Option oneWayOption {"--oneway", "One Way Sync.", 
        [&argv]() { oneWay( argv[2], argv[3] ) ; }};

    CommandLineParser::Option monitorOption {"--monitor", "Monitor the files in the data.json", []() {
        watcher();
    }};

    parser.add_custom_option( oneWayOption );
    parser.add_custom_option( monitorOption );
    parser.parse(argc, argv);

    return 0;
}
