#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

const char* pipePath = "/tmp/myfifo";

int main() {
    // Create the named pipe (FIFO)
    if (mkfifo(pipePath, 0777) == -1 && errno != EEXIST) {
        std::cerr << "Failed to create FIFO: " << strerror(errno) << std::endl;
        return 1;
    }

    // Open the pipe for writing
    int fd = open(pipePath, O_WRONLY);
    if (fd == -1) {
        std::cerr << "Failed to open pipe for writing." << std::endl;
        return 1;
    }

    // Open the video file to send
    const char* filename = "Video.mp4"; 
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Failed to open the video file." << std::endl;
        close(fd);
        return 1;
    }

    // Send the video file data through the pipe
    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(fd, buffer, bytesRead);
    }

    // Clean up
    fclose(file);
    close(fd);
    return 0;
}