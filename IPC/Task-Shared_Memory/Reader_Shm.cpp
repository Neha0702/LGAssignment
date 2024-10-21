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
        std::cerr << "Failed to get shared memory: " << strerror(errno) << std::endl;
        return 1;
    }

    char* shmPtr = static_cast<char*>(shmat(shmId, nullptr, 0));
    if (shmPtr == (char*)(-1)) {
        std::cerr << "Failed to attach shared memory: " << strerror(errno) << std::endl;
        return 1;
    }


    const char* outFilename = "received_video.mp4"; // Output filename
    FILE* outFile = fopen(outFilename, "wb");
    if (!outFile) {
        std::cerr << "Failed to create output video file: " << strerror(errno) << std::endl;
        shmdt(shmPtr);
        return 1;
    }
    
    fwrite(shmPtr, 1, shmSize, outFile); 

    fclose(outFile);
    shmdt(shmPtr);
    shmctl(shmId, IPC_RMID, nullptr); 

    return 0;
}