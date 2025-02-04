#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iomanip>

// Function to count the frequency of all byte values (0-255) in a file
std::vector<std::pair<unsigned char, int>> countByteFrequency(const std::string& fileName) {
    // Create an unordered map to store byte frequencies (0-255)
    std::unordered_map<unsigned char, int> byteFrequency;

    // Open the file in binary mode
    std::ifstream file(fileName, std::ios::binary);

    // Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file!" << std::endl;
        return 0;
    }

    // Read the file byte by byte
    unsigned char byte;
    while (file.get(reinterpret_cast<char&>(byte))) {
        byteFrequency[byte]++;
    }

    // Close the file
    file.close();

    // Convert the unordered map to a vector of pairs for sorting
    std::vector<std::pair<unsigned char, int>> sortedBytes(byteFrequency.begin(), byteFrequency.end());

    // Sort the vector by frequency in descending order
    std::sort(sortedBytes.begin(), sortedBytes.end(), 
        [](const std::pair<unsigned char, int>& a, const std::pair<unsigned char, int>& b) {
            return a.second > b.second;  // Sort by frequency, descending
        });

    // Display the sorted frequency of each byte (0-255)
    std::cout << "Byte Frequency in file (sorted):" << std::endl;
    for (const auto& pair : sortedBytes) {
	    std::cout << "Byte " << std::setw(3) << static_cast<int>(pair.first) 
		    << " Frequency: " << pair.second << std::endl;
    }
    return (sortedBytes);
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return (0);
	std::string fileName = argv[1]; // Replace with your file path

    // Call the function to count byte frequencies
	std::vector<std::pair<unsigned char, int>> sortedBytes = countByteFrequency(fileName);
    return 0;
}

