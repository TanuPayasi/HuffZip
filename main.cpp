#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <bitset>
using namespace std;

// Huffman Tree Node
struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode *left, *right;
    
    HuffmanNode(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue (min-heap)
struct NodeComparator {
    bool operator()(HuffmanNode* left, HuffmanNode* right) {
        return left->freq > right->freq;
    }
};

// Build character frequency table
map<char, int> buildFrequencyTable(const string& content) {
    map<char, int> freqTable;
    for (char c : content) {
        freqTable[c]++;
    }
    return freqTable;
}

// Build Huffman Tree using min-heap
HuffmanNode* buildHuffmanTree(const map<char, int>& freqTable) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, NodeComparator> minHeap;
    
    // Create leaf nodes and push to min-heap
    for (auto& pair : freqTable) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }
    
    // Build tree by combining nodes
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        
        HuffmanNode* internalNode = new HuffmanNode('\0', left->freq + right->freq);
        internalNode->left = left;
        internalNode->right = right;
        minHeap.push(internalNode);
    }
    
    return minHeap.top();
}

// Generate Huffman Codes (DFS traversal)
void generateHuffmanCodes(HuffmanNode* root, string code, map<char, string>& huffmanCodes) {
    if (!root) return;
    
    if (root->data != '\0') {
        huffmanCodes[root->data] = code;
    }
    
    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Compress file using Huffman coding
void compressFile(const string& inputFile, const string& outputFile) {
    // Read input file
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cerr << "Error: Cannot open input file!" << endl;
        return;
    }
    
    string content((istreambuf_iterator<char>(inFile)), 
                   (istreambuf_iterator<char>()));
    inFile.close();
    
    if (content.empty()) {
        cerr << "Error: File is empty!" << endl;
        return;
    }
    
    // Build frequency table
    map<char, int> freqTable = buildFrequencyTable(content);
    
    // Build Huffman tree
    HuffmanNode* huffmanTree = buildHuffmanTree(freqTable);
    
    // Generate Huffman codes
    map<char, string> huffmanCodes;
    generateHuffmanCodes(huffmanTree, "", huffmanCodes);
    
    // Write header (frequency table)
    ofstream outFile(outputFile, ios::binary);
    outFile << freqTable.size() << '\n';
    for (auto& pair : freqTable) {
        outFile << static_cast<int>(pair.first) << ' ' << pair.second << '\n';
    }
    
    // Encode content
    string encoded = "";
    for (char c : content) {
        encoded += huffmanCodes[c];
    }
    
    // Add padding if needed
    int padding = 8 - (encoded.length() % 8);
    if (padding != 8) {
        encoded += string(padding, '0');
    }
    
    // Write compressed data (bit-level)
    for (size_t i = 0; i < encoded.length(); i += 8) {
        bitset<8> bits(encoded.substr(i, 8));
        outFile.put(static_cast<char>(bits.to_ulong()));
    }
    
    outFile.close();
    
    // Calculate compression ratio
    double originalSize = content.size();
    double compressedSize = (encoded.length()/8) + (freqTable.size() * 10);
    double ratio = (1 - (compressedSize / originalSize)) * 100;
    
    cout << "\nCompression successful!" << endl;
    cout << "Original size: " << originalSize << " bytes" << endl;
    cout << "Compressed size: " << compressedSize << " bytes" << endl;
    cout << "Compression ratio: " << ratio << "%" << endl;
    cout << "Saved as: " << outputFile << endl;
}

// Decompress file
void decompressFile(const string& inputFile, const string& outputFile) {
    // Read input file
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cerr << "Error: Cannot open input file!" << endl;
        return;
    }
    
    // Read header
    int tableSize;
    inFile >> tableSize;
    inFile.ignore();
    
    map<char, int> freqTable;
    for (int i = 0; i < tableSize; ++i) {
        int charValue;
        int freq;
        inFile >> charValue >> freq;
        inFile.ignore();
        freqTable[static_cast<char>(charValue)] = freq;
    }
    
    // Read compressed data
    string encoded = "";
    char byte;
    while (inFile.get(byte)) {
        bitset<8> bits(byte);
        encoded += bits.to_string();
    }
    inFile.close();
    
    // Rebuild Huffman tree
    HuffmanNode* huffmanTree = buildHuffmanTree(freqTable);
    
    // Decode data
    ofstream outFile(outputFile, ios::binary);
    HuffmanNode* current = huffmanTree;
    int decodedCount = 0;
    int totalChars = 0;
    
    for (auto& pair : freqTable) {
        totalChars += pair.second;
    }
    
    for (char bit : encoded) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        if (current->left == nullptr && current->right == nullptr) {
            outFile.put(current->data);
            current = huffmanTree;
            if (++decodedCount == totalChars) break;
        }
    }
    
    outFile.close();
    cout << "\nDecompression successful! Saved as: " << outputFile << endl;
}

int main() {
    cout << "╔══════════════════════════════════════╗" << endl;
    cout << "║             HuffZip                  ║" << endl;
    cout << "║  Huffman Coding File Compressor      ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;
    
    int choice;
    string inputFile, outputFile;
    
    cout << "\nMenu:\n";
    cout << "1. Compress File\n";
    cout << "2. Decompress File\n";
    cout << "3. Exit\n";
    cout << "Enter choice: ";
    cin >> choice;
    
    if (choice == 3) {
        cout << "Exiting...\n";
        return 0;
    }
    
    cout << "Input filename: ";
    cin >> inputFile;
    cout << "Output filename: ";
    cin >> outputFile;
    
    if (choice == 1) {
        compressFile(inputFile, outputFile);
    } else if (choice == 2) {
        decompressFile(inputFile, outputFile);
    } else {
        cerr << "Invalid choice!" << endl;
    }
    
    return 0;
}
