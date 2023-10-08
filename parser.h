#include <iostream>
#include <vector>
#include <functional>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

class CommandLineParser {
public:

    // Define a structure to represent a option to parse.
    struct Option {
        const std::string name;
        const std::string description;
        std::function<void()> function;

        // Constructor overload for functions without arguments
        Option(const std::string& n, const std::string& desc, std::function<void()> func)
            : name(n), description(desc), function(func) {}
    };

    struct ShortFlag
    {
        const std::string name;
        const fs::copy_options optionRef;
    };

    CommandLineParser() {
        sFlags.push_back(recursiveFlag);
        sFlags.push_back(overwriteFlag);
        sFlags.push_back(directoriesFlag);
        sFlags.push_back(symlinksFlag);
        sFlags.push_back(updateFlag);

        options.push_back(versionOption);
        options.push_back(helpOption);
        options.push_back(copyOption);
    }

    /* add option to add shortflags to sFlags */
    void add_custom_flag(const ShortFlag flag) { sFlags.push_back( flag ); }

    /* Option to add custom functions. */
    void add_custom_option(const Option& option) { options.push_back(option); }
    

    // Method to parse command-line arguments and execute associated functions
    void parse(const int argc, const char* argv[]) {
        for (int i = 1; i < argc; ++i) {

            if (!strncmp(argv[i], "-", 1) && strncmp(argv[i], "--", 2))
            {

                if (std::find_if(sFlags.begin(), sFlags.end(), [&i, &argv](const ShortFlag& flag) 
                    {return flag.name == argv[i]; }) != sFlags.end()) 
                {
                    usedFlags.push_back(FlagFromName(argv[i]));

                } else { throw std::runtime_error("Unidentified Flag(s)."); }
            }
        
            // if (argv[i] == copyOption.name) {
            //     try
            //     {
            //         from = argv[i+1];
            //         to = argv[i+2] + stripLastWord(from);

            //     } catch(const std::exception& e) {std::cerr << e.what() << '\n';}

            //     copyOption.function();

            //     break;
            // } 
            // else if ( argv[i][0] != '-' ) 
            // { 
            //     std::cout << argv[i] << std::endl;
            //     OptionFromName(argv[i]).function() ;
            // }  
            // else if ( strncmp(argv[i], "--", 2) == 0 )
            //     OptionFromName(argv[i]).function() ;

            if (argv[i] == copyOption.name)
            {
                try {
                    from = argv[i+1];
                    to = argv[i+2] + stripLastWord(from);
                } catch(const std::exception& e) {std::cerr << e.what() << '\n';}

                copyOption.function();
                break;
            } 
            else if (argv[i] == "say")
            {
                OptionFromName(argv[i]).function();
            }
            

            for (const Option op : options)
            {
                if (argv[i] == op.name) {
                    op.function();
                }
            }
            
            
        }
    }

private:

    ShortFlag FlagFromName(const char* name) {
        for (ShortFlag flag : sFlags)
        {
            if( flag.name == name ) 
                return flag;
        }
        throw std::runtime_error("Flag not found");
    }

    Option OptionFromName(const char* name) {
        for (Option op : options)
        {
            if ( op.name == name )
            {
                return op;
            }
        }
        throw std::runtime_error("Option not found");
    }

    std::string stripLastWord(const std::string& path) {
        
        int index { 0 }; 
        for (size_t i = path.size() - 1; i > 0; i--)
        {
            if (path[i] == '/')
            {
                index = i;
                break;   
            }
        }
        return path.substr(index, path.size());
    }
    
    std::vector<ShortFlag> sFlags {};
    std::vector<Option> options {};
    std::vector<ShortFlag> usedFlags {}; 

    ShortFlag recursiveFlag {"-r", fs::copy_options::recursive};
    ShortFlag overwriteFlag {"-w", fs::copy_options::overwrite_existing};
    ShortFlag directoriesFlag {"-d", fs::copy_options::directories_only};
    ShortFlag updateFlag {"-u", fs::copy_options::update_existing};
    ShortFlag symlinksFlag {"-c", fs::copy_options::copy_symlinks};


    Option helpOption { "--help", "Display help Message", []() {
        std::cout << "Help: Help has been arrived." << std::endl;
    }};

    Option versionOption {"--version", "Version Check", []() {
        std::cout << "Version: Version is 9.10" << std::endl;
    }};

    
    void copyFiles() {

        fs::copy_options combinedOptions = fs::copy_options::none;

        for (const ShortFlag& flag : usedFlags) {
            combinedOptions |= flag.optionRef;
            // std::cout << flag.name << std::endl;
        }
        // std::cout << from << std::endl;
        // std::cout << to << std::endl;
        fs::copy(from, to, combinedOptions);
        // std::cout << "Copying is successfull." << std::endl;
    }

    Option copyOption { "copy", "copy files or directories", [this]() { copyFiles(); }} ;
    std::string from {};
    std::string to {};
};

