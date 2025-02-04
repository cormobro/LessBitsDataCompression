#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <algorithm>

// Function to count the frequency of all byte values (0-255) in a file
std::vector<std::pair<unsigned char, int>> countByteFrequency(const std::string& fileName) {
    // Create an unordered map to store byte frequencies (0-255)
    std::unordered_map<unsigned char, int> byteFrequency;

    // Open the file in binary mode
    std::ifstream file(fileName, std::ios::binary);

    // Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file!" << std::endl;
        return (std::vector<std::pair<unsigned char, int>>());
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

// Function to find sequences of the same following bytes in a file and write them to a new file
void findByteSequencesAndRepush(const std::string& inputFileName, const std::string& outputFileName, const unsigned char least1, const unsigned char least2) {
	// Open the input file in binary mode
	std::ifstream inputFile(inputFileName, std::ios::binary);

	// Check if the input file opened successfully
	if (!inputFile.is_open()) {
		std::cerr << "Error: Could not open input file!" << std::endl;
		return;
	}

	// Open the output file in binary mode
	std::ofstream outputFile(outputFileName, std::ios::binary);

	// Check if the output file opened successfully
	if (!outputFile.is_open()) {
		std::cerr << "Error: Could not open output file!" << std::endl;
		return;
	}

	// Variables to store the current byte and the previous byte
	unsigned char currentByte, previousByte;
	bool firstByte = true;  // Flag to check if we are processing the first byte
	int count = 1;  // Count the length of the current sequence

	// Read the input file byte by byte
	while (inputFile.get(reinterpret_cast<char&>(currentByte))) {
		if (!firstByte && currentByte == previousByte) {
			// If the current byte matches the previous byte, increment the sequence count
			count++;
		} else {
			// If the current byte doesn't match the previous byte and we're not on the first byte
			if (!firstByte) {
				if (previousByte == least1) {
					// Write 254, then 255 to the output file
					for (int i = 0; i < count; ++i) {
						outputFile.put(static_cast<unsigned char>(least1));
						outputFile.put(static_cast<unsigned char>(least2));
					}
				}
				else if (previousByte == least2) {
					// Write 254, then 255 to the output file
					for (int i = 0; i < count; ++i) {
						outputFile.put(static_cast<unsigned char>(least2));
						outputFile.put(static_cast<unsigned char>(least1));
					}
				}
				else if (count > 1)
				{
					int i = count % 2;
					if (i == 0)
					{
						while (i < count)
						{
							outputFile.put(static_cast<unsigned char>(least1));
							i += 2;
						}
					}
					else
					{
						while (i < count)
						{
							outputFile.put(static_cast<unsigned char>(least2));
							i += 2;
						}
					}
					outputFile.put(previousByte);
				}
				else
					outputFile.put(previousByte);
			}
			// Reset for the new sequence
			count = 1;
		}

		// Set the previous byte to the current byte for the next iteration
		previousByte = currentByte;
		firstByte = false;
	}

	// Write the last sequence (if any)
	if (!firstByte) {
		for (int i = 0; i < count; ++i) {
			outputFile.put(previousByte);
		}
	}

	// Close both files
	inputFile.close();
	outputFile.close();

	std::cout << "Byte sequences have been written to the output file successfully!" << std::endl;
}

int main(int argc, char **argv) {
	if (argc != 3)
		return (0);
	std::string inputFileName = argv[1];  // Replace with your input file path
					      // Output file name (Replace with your desired output file path)
	std::string outputFileName = argv[2];  // Replace with your output file path

	std::vector<std::pair<unsigned char, int>> sortedBytes = countByteFrequency(inputFileName);
	const unsigned char least1 = static_cast<unsigned char>(sortedBytes[sortedBytes.size() - 1].first);
	const unsigned char least2 = static_cast<unsigned char>(sortedBytes[sortedBytes.size() - 2].first);
	// Call the function to find byte sequences and write them to a new file
	findByteSequencesAndRepush(inputFileName, outputFileName, least1, least2);

	return 0;
}
