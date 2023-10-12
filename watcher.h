#include <iostream>
#include <unistd.h>
#include <sys/inotify.h>
#include <vector>
#include "json.h" 

void sync(const fs::path& source, const fs::path& dest, const bool& isOneWay) 
{
    fs::copy(source, dest, fs::copy_options::update_existing);

    if (!isOneWay) {
        std::cout << "hey" << std::endl;
        fs::copy(dest, source, fs::copy_options::update_existing);
    }
    std::cout << "Sync is successfull! " << std::endl;

    // TODO add an option if the source is a directory
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
            std::cout << "Event for file: " << event->wd << std::endl;
            /* FIXME Fix the problem on the id's. The target id becomes greater than the json file id's 
            because two way sync does not create another json variable. */  

            if (event->wd == 1) { goto restartPoint; } 
            
            if (event->mask & IN_CREATE)
                std::cout << "created ";
            if (event->mask & IN_DELETE)
                std::cout << "deleted ";
            if (event->mask & IN_MODIFY)
                std::cout << "modified ";
            if (event->mask & IN_MOVED_TO)
                std::cout << "moved";
            
            std::cout << "\n" << std::endl;

            auto [dest, source, isOneWay] = desOrSourceById( event->wd );
            std::cout << "Destination is: " << dest << std::endl;
            std::cout << "Source is: " << source << "\n" << std::endl;
            
            if (event->wd != 1)
            {
                sync(source, dest, isOneWay);
            }
            

            /*  TODO Do some other algorithm for directories.  */

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