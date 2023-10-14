#include "parser.h"
#include "watcher.h"

bool isStartsWith(const std::string& str, const std::string& prefix) {
    return str.rfind(prefix, 0) == 0;
}

bool isValidFlag(const std::string& arg) {
    if (!isStartsWith(arg, "-")) return false;
    if (arg != "-one" && arg != "-two") return false;
    return true;
}


void savePair(char const *argv[], int& argc) {
    if (argc != 5) {
        throw std::runtime_error("Usage: " + std::string(argv[0]) + " <flag> <source> <destination>");
    }
    
    const std::string flag = argv[2];
    fs::path source = argv[3];
    fs::path destination = argv[4];

    if (!isValidFlag(flag)) {
        throw std::runtime_error("Invalid flag. Please use '-one' for one-way sync or '-two' for two-way sync.");
    }

    if (!fs::exists(source)) {
        throw std::runtime_error("Source directory '" + source.string() + "' does not exist.");
    } else if (!fs::exists(destination)) {
        throw std::runtime_error("Destination directory '" + destination.string() + "' does not exist.");
    }

    saveNewPair(source, destination, flag);
}

// TODO Add the daemon support for Monitoring now that all is done.

int main(int argc, char const *argv[])
{
    CommandLineParser parser;
    
    CommandLineParser::Option syncOption {"sync", " Sync.", 
        [&argv, &argc]() { savePair( argv, argc ) ; }};

    CommandLineParser::Option monitorOption {"--monitor", "Monitor the files in the data.json", []() {
        watcher();
    }};

    parser.add_custom_option( syncOption );
    parser.add_custom_option( monitorOption );
    parser.parse(argc, argv);

    return 0;
}
