// create_file.cpp
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#define FILE_SIZE_MB 10
#define BUFFER_SIZE 1024

int main() {
    std::ofstream outfile("file_to_send5.txt");
    if (!outfile) {
        std::cerr << "Error creating file" << std::endl;
        return 1;
    }

    // Seed for random number generation
    std::srand(std::time(0));
    
    // Write random text to the file
    for (size_t i = 0; i < FILE_SIZE_MB * 1024; ++i) {
        for (size_t j = 0; j < BUFFER_SIZE; ++j) {
            // Generate random lowercase letters
            char random_char = 'a' + std::rand() % 26;
            outfile << random_char;
        }
        outfile << "\n"; // Newline for better readability
    }

    std::cout << "File 'file_to_send.txt' created with size " << FILE_SIZE_MB << " MB." << std::endl;

    outfile.close();
    return 0;
}
