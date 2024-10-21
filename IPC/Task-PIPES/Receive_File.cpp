#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

const char* pipePath = "/tmp/myfifo";

int main() {
    // Open the pipe for reading
    int fd = open(pipePath, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open pipe for reading." << std::endl;
        return 1;
    }

    // Open a file to write the received video data
    const char* outFilename = "received_video.mp4"; // Output filename
    FILE* outFile = fopen(outFilename, "wb");
    if (!outFile) {
        std::cerr << "Failed to create output video file." << std::endl;
        close(fd);
        return 1;
    }

    // Read from the pipe and write to the output video file
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, bytesRead, outFile);
    }

    // Clean up
    fclose(outFile);
    close(fd);
    
    // Remove the named pipe
    unlink(pipePath);

    return 0;
}