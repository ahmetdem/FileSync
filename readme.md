# File Synchronization and Monitoring Tool

This project is a command-line tool for file synchronization and monitoring. It provides a way to synchronize files between source and destination directories or files and monitor files listed in a `data.json` file for changes.


## Features

-   **File Synchronization**: Synchronize files between source and destination directories.
- **File Monitoring**: Continuously monitor files listed in the `data.json` file for changes.


## Requirements

To use this tool, you need the following:

-   C++ Compiler
-   [Jsoncpp Library](https://github.com/open-source-parsers/jsoncpp)

## Compilation

To compile the project, follow these steps:

1.  **Clone the Repository**: First, clone this repository to your local machine using Git:
	 
	 ```bash
    git clone https://github.com/ahmetdem/FileSync.git
    ```
2.  **Create a Build Directory**: Navigate to the project directory and create a build directory. This is where CMake will generate the build files:
    ```bash 
    mkdir build
    ``` 
    
3.  **Configure the Build**: Use CMake to configure the build. This will generate the necessary build files for your platform and compiler. Run CMake from within the build directory:

	```bash 
	cd build
	cmake ..
	```

	Adjust the CMake configuration as needed, specifying any build options or flags required by your project.
    
4.  **Build the Project**: After the configuration is complete, use your preferred build tool (e.g., `make` on Unix-like systems) to compile the project:
    
	```bash
	make 
	```
    
    The build tool will handle the compilation of both source files (`.cpp`) and header files (`.h` or `.hpp`) as needed.
    
5.  **Run the Executable**: After a successful build, you'll have an executable file (e.g., `sync_tool`). You can run it using the following command:
   
   ```bash 
    ./main
```

## Usage

### Synchronize Files

To synchronize files, you need to enter the `sync` command followed by a flag, source directory, and destination directory.

```bash 
./main sync <flag> <source> <destination>
 ```

-   `<flag>`: Choose either `-one` for one-way sync or `-two` for two-way sync.
-   `<source>`: Specify the source directory.
-   `<destination>`: Specify the destination directory.


Example:

```bash 
./main sync -one /path/to/source /path/to/destination
 ```

### Monitor Files

To monitor files listed in the `data.json` file, simply use the following command:

```bash 
./main --monitor
```

This will monitor the files specified in the `data.json` file for any changes.

## Command-Line Options

-   `sync`: Synchronize files between directories.
-   `monitor`: Continuously monitor files listed in the `data.json` file.

## Installation and Usage

1.  Compile the project using a C++ compiler.
2.  Ensure the `data.json` file is present and correctly formatted.
3.  Run the compiled executable with the appropriate command-line options.

## JSON Data

The `data.json` file is used to specify the files to be monitored. Ensure that the file is correctly formatted with a list of files, their paths, and IDs.

Example `data.json` format:

```json 
{
    "synced_files": 
	[
		{
			"destination" : "/",
			"id" : 1,
			"source": "/path/to/json/file"
		},
		{
			"id": 2,
			"source": "/path/to/source/file.txt",
			"destination": "/path/to/destination/file.txt",
			"way": true
		}
    	]
}
```

**Warning:** The first element of the json file should always be the path of itself!!
