#ifndef WATCHER_H
#define WATCHER_H

#include <iostream>
#include <unistd.h>
#include <sys/inotify.h>
#include <vector>
#include "json.h" 

void sync(const fs::path& source, const fs::path& dest, const bool& isOneWay) 
{
    static bool written = true;

    // FIXME fix the problem where the directory synced to destination path initally. 
    if (fs::is_directory(source))
    {
        fs::copy(source, dest, fs::copy_options::update_existing | fs::copy_options::recursive);
        std::cout << "Directory Sync Is Successfull." << std::endl;
        return;
    }

    if (!isOneWay && written) // dest -> source
    {
        fs::copy(source, dest, fs::copy_options::update_existing);
        written = false;
        std::cout << "Written is (first): " << written << "\n" << std::endl;
        return;

    } else if (!isOneWay)
    {
        written = true;
        std::cout << "Written is: (second): " << written << "\n" << std::endl;
        return;
    }
    

    fs::copy(source, dest, fs::copy_options::update_existing);
    std::cout << "Sync is successfull! \n" << std::endl;

    /* FIXME (Seems to be fixed, but could potentially be a problem.)
    when i make another json object for watching destination, 
    the program changes source but because it changes source,
    it also changes destination. Thats an infinite loop. */

}

void printBufferContent(const char* buffer, ssize_t bytesRead) {
    std::cout << "Buffer Content (Hex): ";
    for (ssize_t i = 0; i < bytesRead; ++i) {
        std::cout << std::hex << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}


int watcher()
{
    restartPoint:

    int fd = inotify_init();
    if (fd == -1) {
        std::cerr << "Error initializing inotify." << std::endl;
        return 1;
    }

    std::vector<const char*> filesToMonitor = getFiles();
    int numFiles = filesToMonitor.size();

    const uint32_t eventMask = IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVED_TO;

    std::cout << "Number of files monitoring is: " << numFiles << std::endl;

    for (int i = 0; i < numFiles; ++i) {
        const char* file = filesToMonitor[i];

        int wd = inotify_add_watch(fd, file, eventMask);
        if (wd == -1) {
            std::cerr << "Error adding watch to " << file << std::endl;
            return 1;
        }

        std::cout << "Watching: " << file << std::endl;
    }

    char buffer[4096];
    while (true) {

        ssize_t bytesRead = read(fd, buffer, sizeof(buffer)); // the problem part

        if (bytesRead == -1) {
            std::cerr << "Error reading inotify events." << std::endl;
            return 1;
        }

        // printBufferContent(buffer, bytesRead);

        // Process inotify events
        for (char* ptr = buffer; ptr < buffer + bytesRead;) {
            struct inotify_event* event = reinterpret_cast<struct inotify_event*>(ptr);

            // Print event information
            std::cout << "Event for file: " << event->wd;

            if (event->wd == 1) { goto restartPoint; } 
            
            if (event->mask & IN_CREATE)
                std::cout << " created \n";
            if (event->mask & IN_DELETE)
                std::cout << " deleted \n";
            if (event->mask & IN_MODIFY)
                std::cout << " modified \n";
            if (event->mask & IN_MOVED_TO)
                std::cout << " moved \n";

            auto [dest, source, isOneWay] = desOrSourceById( event->wd );
            std::cout << "Source is: " << source << std::endl;
            std::cout << "Destination is: " << dest << std::endl;

            if (event->wd != 1)
            {
                sync(source, dest, isOneWay);
            }

            /* TODO Fix potential error of files deleted and renamed.*/

            ptr += sizeof(struct inotify_event) + event->len;
        }
    }

    // Close the inotify file descriptor
    close(fd);

    // release the memory
    for (const char* filePath : filesToMonitor) {
        free(const_cast<char*>(filePath));
    }

    return 0;
}

#endif // WATCHER_H