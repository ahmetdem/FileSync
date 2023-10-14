#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <fstream>
#include <tuple>
#include <sys/file.h>
#include <json/json.h>

// Function to acquire an exclusive file lock
bool acquireExclusiveLock(int fd) {
    if (flock(fd, LOCK_EX) == 0) {
        return true;
    } else {
        return false;
    }
}

// Function to release a file lock
void releaseLock(int fd) {
    flock(fd, LOCK_UN);
}

// Function to read JSON data from the file
std::vector<const char*> getFiles() {
    Json::Value jsonData;
    std::vector<const char*> files; // Use std::vector to store the file paths

    int fileDescriptor = open("../src/data.json", O_RDONLY);
    if (fileDescriptor == -1) {
        throw std::runtime_error("Error opening file for reading.");
    }

    // Acquire a shared lock while reading
    if (acquireExclusiveLock(fileDescriptor)) {
        
        std::ifstream file("../src/data.json");
        if (file) {
            file >> jsonData;
            file.close();

            const Json::Value& syncedFilesArray = jsonData["synced_files"];
            int numFiles = syncedFilesArray.size(); // Get the number of files

            for (int i = 0; i < numFiles; ++i) {
                
                /* TODO: Maybe add pointers to avoid string assignment? */
                const fs::path source = syncedFilesArray[i]["source"].asString();
                
                files.push_back(strdup(source.c_str()));
            }
        }
        releaseLock(fileDescriptor);
    } else {
        std::cerr << "Failed to acquire an exclusive lock for reading." << std::endl;
    }

    close(fileDescriptor);

    return files;
}

/* Save new pair of ${source} ${destination} to the json file. */
void saveNewPair (const fs::path& source, const fs::path& destination, const std::string& arg ) 
{
    int fileDescriptor = open("../src/data.json", O_WRONLY | O_CREAT, 0666);
    if (fileDescriptor == -1) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }
    // Open the existing JSON file for reading

    std::ifstream inputFile("../src/data.json");
    if (!inputFile.is_open()) { throw std::runtime_error("Failed to open the input file.") ; }

    if (acquireExclusiveLock(fileDescriptor))
    {
        // Parse the JSON data from the file into a Json::Value object
        Json::Value jsonData;
        inputFile >> jsonData;
        inputFile.close();

        const Json::Value& syncedFilesArray = jsonData["synced_files"];
        int id = syncedFilesArray.size(); // Get the number of files

        const fs::path newDes = destination / source.filename();

        bool isWay { (arg == "-one") ? true : false };

        if (!fs::exists(newDes))
            fs::copy(source, destination);
            
        Json::Value newSyncedFile;

        newSyncedFile["source"] = source.string();
        newSyncedFile["destination"] = newDes.string();
        newSyncedFile["id"] = id+1;
        newSyncedFile["way"] = isWay;

        jsonData["synced_files"].append(newSyncedFile);

        if (!isWay)
        {
            Json::Value newSyncedFile2;

            newSyncedFile2["source"] = newDes.string();
            newSyncedFile2["destination"] = source.string();
            newSyncedFile2["id"] = id+2;
            newSyncedFile2["way"] = isWay;

            jsonData["synced_files"].append(newSyncedFile2);
        }

        // Save the modified JSON data back to the file
        std::ofstream outputFile("../src/data.json");
        if (!outputFile.is_open()) { throw std::runtime_error("Failed to open the input file.") ; }

        outputFile << jsonData;
        outputFile.close();

    } else {
        std::cerr << "Failed to acquire an exclusive lock for writing." << std::endl;
    }

    close(fileDescriptor);
}

std::tuple<fs::path, fs::path, bool> desOrSourceById(const int& targetId) {
    int fileDescriptor = open("../src/data.json", O_RDONLY);
    if (fileDescriptor == -1) {
        throw std::runtime_error("Error opening file for reading.");
    }

    if (acquireExclusiveLock(fileDescriptor)) {
        Json::Value jsonData;

        std::ifstream file("../src/data.json");
        if (!file) {
            releaseLock(fileDescriptor); // Release the lock if file open fails
            throw std::runtime_error("Error opening file for reading.");
        }

        file >> jsonData;
        file.close();

        const Json::Value& syncedFilesArray = jsonData["synced_files"];
        if (targetId < 0 || targetId > syncedFilesArray.size()) {
            releaseLock(fileDescriptor); // Release the lock before returning
            std::cerr << "Target id does not exist." << std::endl;
            close(fileDescriptor);
            return std::make_tuple("", "", false); // Return an empty string if the target ID doesn't exist
        }

        // Iterate to find the desired ID
        // TODO Maybe implement Binary Search Here
        for (const auto& syncedFile : syncedFilesArray) {
            int id = syncedFile["id"].asInt();
            if (id == targetId) {
                // Release the lock before returning
                releaseLock(fileDescriptor);
                close(fileDescriptor);
                return std::make_tuple(syncedFile["destination"].asString(), syncedFile["source"].asString(), syncedFile["way"].asBool());
            }
        }

        // Release the lock before returning
        releaseLock(fileDescriptor);
    } else {
        std::cerr << "Failed to acquire an exclusive lock for reading." << std::endl;
    }

    // Close the file descriptor
    close(fileDescriptor);
    return std::make_tuple("", "", false);
}

#endif // JSON_H


