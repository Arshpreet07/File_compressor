#pragma once
struct HuffmanNode { //this is the Node structure of 
    char data;         // the huffman tree
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode() : data('\0'), freq(0), left(nullptr), right(nullptr) {}
    HuffmanNode(char ch, int f) : data(ch), freq(f), left(nullptr), right(nullptr) {}
};