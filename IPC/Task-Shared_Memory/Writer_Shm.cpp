#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cerrno>

const int shmSize = 10 * 1024 * 1024; // 10 MB
const int shmKey = 1234;

int main() {
    
    int shmId = shmget(shmKey, shmSize, IPC_CREAT | 0666);
    if (shmId == -1) {
        std::cerr << "Failed to create shared memory: " << strerror(errno) << std::endl;
        return 1;
    }

    
    char* shmPtr = static_cast<char*>(shmat(shmId, nullptr, 0));
    if (shmPtr == (char*)(-1)) {
        std::cerr << "Failed to attach shared memory: " << strerror(errno) << std::endl;
        return 1;
    }


    const char* filename = "Video.mp4"; 
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Failed to open the video file: " << strerror(errno) << std::endl;
        shmdt(shmPtr);
        return 1;
    }

    size_t bytesRead;
    size_t totalBytesRead = 0;
    while (totalBytesRead < shmSize && (bytesRead = fread(shmPtr + totalBytesRead, 1, shmSize - totalBytesRead, file)) > 0) {
        totalBytesRead += bytesRead;
    }

    // Clean up
    fclose(file);
    shmdt(shmPtr);
    return 0;
}
