#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <string>
#include <filesystem>
#include "HuffmanNode.h"

using namespace std;

const int CHAR_RANGE = 256; //ASCII range

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq; // Min-heap
    }
};

void countFrequencies(const string &filename, vector<int>& freqArray) {
    ifstream infile(filename, ios::in);
    if (!infile.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }

    unsigned char data;
    while (infile.read(reinterpret_cast<char*>(&data), 1)) {
        freqArray[data]++; //Count of frequency of each character
    }

    infile.close();
}


// void printFrequencyMap(const vector<int>& freqArray) {
//     cout << "Character Frequencies:\n";
//     for (int i = 0; i < CHAR_RANGE; i++){
//         if (freqArray[i] > 0) {
//             if (i == '\n') cout << "'\\n'";
//             else if (i == '\t') cout << "'\\t'";
//             else if (i == ' ') cout << "' '";
//             else if (i < 32 || i >= 127) cout << "0x" << hex << i << dec;
//             else cout << "'" << static_cast<char>(i) << "'";
//             cout << " : " << freqArray[i] << endl;
//         }
//     }
// }

HuffmanNode* buildHuffmanTree(const vector<int>& freqArray) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> minHeap;

    // Step 1: Push all characters as leaf nodes
    for (int i = 0; i < CHAR_RANGE; i++) {
        if (freqArray[i] > 0) {
            minHeap.push(new HuffmanNode(static_cast<char>(i), freqArray[i]));
        }
    }

    // Step 2: Build the tree
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();

        HuffmanNode* merged = new HuffmanNode('\0', left->freq + right->freq);

        merged->left = left;
        merged->right = right;

        minHeap.push(merged);
    }

    // Final node is the root
    return minHeap.empty() ? nullptr : minHeap.top();
}

// void printHuffmanTree(HuffmanNode* root, const string& code = "") {
//     if (!root) return;

//     if (!root->left && !root->right) {
//         cout << "'";
//         switch (root->data) {
//             case '\n': cout << "\\n"; break;
//             case '\t': cout << "\\t"; break;
//             case ' ': cout << " "; break;
//             default : cout << root->data;
//         }
//         cout << "' : " << code << "\n";
//     }

//     printHuffmanTree(root->left, code + "0");
//     printHuffmanTree(root->right, code + "1");
// }

void generateCodes(HuffmanNode* root, unordered_map<char, string>& codeMap, string currentCode = "") {
    if (!root) return;

    if (!root->left && !root->right) {
        codeMap[root->data] = currentCode; // Example :- "d = 1001"
        return;
    }

    generateCodes(root->left, codeMap, currentCode + "0");
    generateCodes(root->right, codeMap, currentCode + "1");
}

string getEncodedString(const string& filename, const unordered_map<char, string>& codeMap) {
    ifstream in(filename);
    string encodedStr;
    char ch;

    while (in.get(ch)) {
        encodedStr += codeMap.at(ch); //Creating a single string of bits
    }

    return encodedStr;
}

void compress(const string& encodedStr, const string& outFilename, const vector<int>& freqArray) {
    ofstream outFile(outFilename, ios::binary);

    // Write all 256 frequencies (4 bytes each) to file header
    for (int i = 0; i < CHAR_RANGE; i++) {
        uint32_t freq = freqArray[i];
        for (int j = 3; j >= 0; --j) {
            outFile.put(static_cast<char>((freq >> (8 * j)) & 0xFF));
        }
    }

    // Write padding placeholder
    streampos paddingPos = outFile.tellp();
    outFile.put(0);

    // Write encoded data bits
    unsigned char byte = 0;
    int bitCount = 0;

    for (char bitChar : encodedStr) {
        byte <<= 1;
        if (bitChar == '1') byte |= 1;
        bitCount++;

        if (bitCount == 8) {
            outFile.put(byte);
            byte = 0;
            bitCount = 0;
        }
    }

    int paddingBits = 0;
    if (bitCount > 0) {
        paddingBits = 8 - bitCount;
        byte <<= paddingBits;
        outFile.put(byte);
    }

    // Go back and write padding bits count
    outFile.seekp(paddingPos);
    outFile.put(static_cast<char>(paddingBits));

    outFile.close();
}

void deleteTree(HuffmanNode* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

void decompress(const string& inputBin, const string& outputFile) {
    ifstream in(inputBin, ios::binary);
    vector<int> freqArray(CHAR_RANGE, 0);

    // Read all 256 frequencies (4 bytes each)
    for (int i = 0; i < CHAR_RANGE; i++) {
        uint32_t freq = 0;
        for (int j = 0; j < 4; j++) {
            freq = (freq << 8) | static_cast<unsigned char>(in.get());
        }
        freqArray[i] = freq;
    }

    //Get number of paddingBits at rear
    uint8_t paddingBits = static_cast<uint8_t>(in.get());

    //Create the same huffman tree
    HuffmanNode* root = buildHuffmanTree(freqArray);

    //create the same string of encoded characters
    string bitStream;
    char byte;
    while (in.read(&byte, 1)) {
        for (int i = 7; i >= 0; --i)
            bitStream += ((byte >> i) & 1) ? '1' : '0';
    }

    //Remove padding bits from rear
    bitStream = bitStream.substr(0, bitStream.size() - paddingBits);

    //Recover the text file
    ofstream out(outputFile);
    HuffmanNode* curr = root;
    for (char bit : bitStream) {
        curr = (bit == '0') ? curr->left : curr->right;
        if (!curr->left && !curr->right) {
            out.put(curr->data);
            curr = root;
        }
    }

    in.close();
    out.close();
    cout << "\nDecompression complete! Output written to " << outputFile << endl;
    deleteTree(root);
}

int main() {
    while (true) {
        cout << "\n========== Huffman Compression Menu ==========\n";
        cout << "1. Compress a file\n";
        cout << "2. Decompress a file\n";
        cout << "3. Exit\n";
        cout << "Choose an option (1-3): ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            string inputFile, outputFile;
            cout << "Enter the input text file name (e.g., input.txt): ";
            cin >> inputFile;
            cout << "Enter the output compressed file name (e.g., compressed.bin): ";
            cin >> outputFile;

            // Step 1 : count frequency of each char
            vector<int> freqArray(CHAR_RANGE, 0); 
            countFrequencies(inputFile, freqArray); 
            // printFrequencyMap(freqArray);

            // Step 2 : Building the huffman Tree
            HuffmanNode* root = buildHuffmanTree(freqArray);

            if (!root) {
                cout << "Original file is empty. No compression performed.\n";
                continue;
            }

            // Step 3 : Assign code to each char
            unordered_map<char, string> codeMap;
            generateCodes(root, codeMap);
            // printHuffmanTree(root, "");
            
            //Step 4 : Compressing the file
            string encodedStr = getEncodedString(inputFile, codeMap);
            compress(encodedStr, outputFile, freqArray);

            uintmax_t inputSize = filesystem::file_size(inputFile);
            uintmax_t compressedSize = filesystem::file_size(outputFile);

            double ratio = (1.0 - (double)compressedSize / inputSize) * 100;
            cout << fixed << setprecision(2);
            cout << "\nFile compressed and saved as " << outputFile << "\n";
            cout << "Reduced size by " << ratio << "%\n";

            deleteTree(root); // Clean up memory
        } 
        else if (choice == 2) {
            string compressedFile, outputFile;
            cout << "Enter the compressed file name (e.g., compressed.bin): ";
            cin >> compressedFile;
            cout << "Enter the output decompressed file name (e.g., output.txt): ";
            cin >> outputFile;

            decompress(compressedFile, outputFile);
        } 
        else if (choice == 3) {
            cout << "\nExiting the program. Goodbye!\n";
            break;
        } 
        else {
            cout << "\nInvalid choice. Please enter 1, 2, or 3.\n";
        }
    }

    return 0;
}