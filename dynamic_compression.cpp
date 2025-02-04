#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <bitset>

using namespace std;

const int INIT_BIT_SIZE = 5;
const int MAX_BIT_SIZE = 7;
//const int MIN_BIT_SIZE = 2;
//const int PAGE_PREV = 0b00000;
const int PAGE_NEXT = 0b11111;

// Function to write packed bits to file
void write_packed_bits(ofstream &output, vector<bool> &bitstream) {
    while (bitstream.size() >= 8) {
        unsigned char byte = 0;
        for (int i = 0; i < 8; i++) {
            byte = (byte << 1) | bitstream[i];
        }
        output.put(byte);
        bitstream.erase(bitstream.begin(), bitstream.begin() + 8);
    }
}

// Function to compress a file
void compress_file(const string &input_file, const string &output_file) {
    ifstream input(input_file, ios::binary);
    ofstream output(output_file, ios::binary);
    if (!input || !output) {
        cerr << "Error opening files!" << endl;
        return;
    }

    unordered_map<string, int> dictionary;
    int dict_size = 32; // 5-bit dictionary size initially
    int bit_size = INIT_BIT_SIZE;

    for (int i = 0; i < 256; i++) {
        dictionary[string(1, i)] = i;
    }

    string buffer;
    vector<bool> bitstream;
    char c;
    
    while (input.get(c)) {
        buffer += c;
        if (dictionary.find(buffer) == dictionary.end()) {
            int code = dictionary[buffer.substr(0, buffer.size() - 1)];
            bitset<7> bits(code);
            for (int i = 0; i < bit_size; i++) {
                bitstream.push_back(bits[bit_size - 1 - i]);
            }
            write_packed_bits(output, bitstream);
            
            if (dict_size < (1 << MAX_BIT_SIZE)) {
                dictionary[buffer] = dict_size++;
                if (dict_size > (1 << bit_size)) {
                    bit_size = min(bit_size + 1, MAX_BIT_SIZE);
                    bitstream.insert(bitstream.end(), 5, 0); // PAGE_NEXT marker
                }
            }
            buffer = c;
        }
    }

    if (!buffer.empty()) {
        int code = dictionary[buffer];
        bitset<7> bits(code);
        for (int i = 0; i < bit_size; i++) {
            bitstream.push_back(bits[bit_size - 1 - i]);
        }
    }

    while (bitstream.size() % 8 != 0) {
        bitstream.push_back(0); // Padding bits
    }
    write_packed_bits(output, bitstream);

    input.close();
    output.close();
}

// Function to read packed bits from file
vector<bool> read_packed_bits(ifstream &input) {
    vector<bool> bitstream;
    char byte;
    while (input.get(byte)) {
        for (int i = 7; i >= 0; i--) {
            bitstream.push_back((byte >> i) & 1);
        }
    }
    return bitstream;
}

// Function to decompress a file
void decompress_file(const string &input_file, const string &output_file) {
    ifstream input(input_file, ios::binary);
    ofstream output(output_file, ios::binary);
    if (!input || !output) {
        cerr << "Error opening files!" << endl;
        return;
    }

    vector<bool> bitstream = read_packed_bits(input);
    unordered_map<int, string> dictionary;
    int dict_size = 32;
    int bit_size = INIT_BIT_SIZE;

    for (int i = 0; i < 256; i++) {
        dictionary[i] = string(1, i);
    }

    string buffer;
    size_t index = 0;
    while (index + bit_size <= bitstream.size()) {
        int code = 0;
        for (int i = 0; i < bit_size; i++) {
            code = (code << 1) | bitstream[index++];
        }

        if (code == PAGE_NEXT) {
            bit_size = min(bit_size + 1, MAX_BIT_SIZE);
            continue;
        }

        if (dictionary.find(code) != dictionary.end()) {
            output << dictionary[code];
            if (!buffer.empty()) {
                dictionary[dict_size++] = buffer + dictionary[code][0];
            }
            buffer = dictionary[code];
        } else {
            dictionary[dict_size++] = buffer + buffer[0];
            output << dictionary[dict_size - 1];
            buffer = dictionary[dict_size - 1];
        }
    }

    input.close();
    output.close();
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <compress|decompress> <input file> <output file>" << endl;
        return 1;
    }

    string mode = argv[1];
    if (mode == "compress") {
        compress_file(argv[2], argv[3]);
        cout << "Compression complete!" << endl;
    } else if (mode == "decompress") {
        decompress_file(argv[2], argv[3]);
        cout << "Decompression complete!" << endl;
    } else {
        cerr << "Invalid mode. Use 'compress' or 'decompress'." << endl;
        return 1;
    }

    return 0;
}
