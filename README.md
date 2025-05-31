## HuffZip - Huffman Coding File Compressor
HuffZip is a command-line file compression tool implementing Huffman coding, a lossless data compression algorithm.

## Features
1. Lossless file compression/decompression
2. Huffman coding algorithm implementation
3. Compression statistics (size, ratio)
4. Binary file handling
5. Error handling for file operations
6. Clean console interface

## Installation & Usage
### Compilation
 g++ -std=c++11 huffzip.cpp -o huffzip
 
### Compression
./huffzip
###### Choose 1 (Compress)
###### Enter input file: document.txt
###### Enter output file: compressed.huff

### Decompression
./huffzip
###### Choose 2 (Decompress)
###### Enter input file: compressed.huff
###### Enter output file: decompressed.txt
