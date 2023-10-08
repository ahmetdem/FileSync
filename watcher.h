#include <iostream>
#include <unistd.h>
#include <sys/inotify.h>
#include <vector>
#include "json.h" 

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

    const uint32_t eventMask = IN_MODIFY | IN_CREATE | IN_DELETE;

    std::cout << numFiles << std::endl;

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
    std::string dest {} ;
    while (true) {
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead == -1) {
            std::cerr << "Error reading inotify events." << std::endl;
            return 1;
        }

        // Process inotify events (same as before)
        for (char* ptr = buffer; ptr < buffer + bytesRead;) {
            struct inotify_event* event = reinterpret_cast<struct inotify_event*>(ptr);

            // Print event information
            std::cout << "Event for file: " << event->wd << " ";

            if (event->wd == 1) { goto restartPoint; } 
            
            if (event->mask & IN_CREATE)
                std::cout << "created ";
            if (event->mask & IN_DELETE)
                std::cout << "deleted ";
            if (event->mask & IN_MODIFY)
                std::cout << "modified ";
            std::cout << std::endl;

            dest = desById( event->wd  );
            std::cout << "destination is: " << dest << std::endl;

            ptr += sizeof(struct inotify_event) + event->len;
        }
    }

    // Close the inotify file descriptor
    close(fd);

    for (const char* filePath : filesToMonitor) {
        free(const_cast<char*>(filePath));
    }

    return 0;
}