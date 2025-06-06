# Huffman Compression Tool

A lightweight and efficient **file compression-decompression tool** implemented in **C++** using the **Huffman Coding algorithm**. This project demonstrates encoding optimization techniques, bit-level data handling, and practical file operations for binary compression.

## Technologies & Tools
- **Programming Language:** C++
- **Core Concepts:**
  - Huffman Encoding and Tree Structures
  - Priority Queues (Min-Heap)
  - Bit Manipulation
  - File I/O in Binary Mode
- **Compiler:** g++ with C++17 standard
- **IDE:** Visual Studio Code / Terminal

## Features
- **Compression**
  - Reads input text and compresses it into a binary file.
  - Efficient bit-packing using custom Huffman codes.
  - Stores frequency table and padding info in the binary file header.
- **Decompression**
  - Reads the frequency table from the header.
  - Reconstructs the Huffman tree.
  - Extracts and decodes the original content bit by bit.
- **File Support**
  - Works on plain text files of varying sizes.
  - Can handle all ASCII characters (0–255).


## Sample Menu Options

<img width="835" alt="Screenshot 2025-06-06 at 4 34 37 PM" src="https://github.com/user-attachments/assets/0255aaab-1d0b-4925-b191-cea52853cb57" />


## 🔧 Build & Run Instructions

> Use the following terminal commands to compile and run:

g++ -std=c++17 main.cpp -o FileCompressor

./FileCompressor

1. Open the folder in **Visual Studio Code** or open the terminal and navigate to the project directory.
2. Run the above g++ command to compile.
3. Execute ./FileCompressor to launch the menu-driven interface.


## Motivation Behind the Project
This project was born from a desire to dive deep into data compression techniques and understand how real-world encoders work at the bit level. Huffman coding offered a powerful yet intuitive entry point into compression algorithms. Beyond academics, this tool lays a strong foundation for exploring more advanced compression techniques in the future and serves as a clean example of working with binary files, memory safety, and low-level C++.
