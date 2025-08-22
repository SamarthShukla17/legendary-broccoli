#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

// Node struct for Huffman tree
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    Node(char c, int f, Node* l, Node* r) : ch(c), freq(f), left(l), right(r) {}
};

// Comparator for priority queue (min-heap)
struct compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Generate Huffman codes by traversing the tree
void generateCodes(Node* root, string str, unordered_map<char, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }
    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

// Delete tree to avoid memory leak
void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// Write frequency table to output file (for decompression later)
void writeFrequencyTable(ofstream& outFile, unordered_map<char, int>& freq) {
    size_t size = freq.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (auto& p : freq) {
        outFile.write(reinterpret_cast<const char*>(&p.first), sizeof(p.first));
        outFile.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
    }
}

// Read frequency table from input file (during decompression)
unordered_map<char, int> readFrequencyTable(ifstream& inFile) {
    unordered_map<char, int> freq;
    size_t size;
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
    for (size_t i = 0; i < size; i++) {
        char ch;
        int f;
        inFile.read(reinterpret_cast<char*>(&ch), sizeof(ch));
        inFile.read(reinterpret_cast<char*>(&f), sizeof(f));
        freq[ch] = f;
    }
    return freq;
}

// Write bits to output file, buffering bits into bytes
class BitWriter {
    ofstream& outFile;
    unsigned char buffer;
    int bitCount;
public:
    BitWriter(ofstream& out) : outFile(out), buffer(0), bitCount(0) {}
    void writeBit(bool bit) {
        buffer = (buffer << 1) | bit;
        bitCount++;
        if (bitCount == 8) {
            outFile.put(buffer);
            bitCount = 0;
            buffer = 0;
        }
    }
    void writeBits(const string& bits) {
        for (char c : bits) {
            writeBit(c == '1');
        }
    }
    void flush() {
        if (bitCount > 0) {
            buffer <<= (8 - bitCount);  // pad remaining bits with 0s on the right
            outFile.put(buffer);
        }
    }
};

// Read bits from input file, one by one
class BitReader {
    ifstream& inFile;
    unsigned char buffer;
    int bitCount;
public:
    BitReader(ifstream& in) : inFile(in), buffer(0), bitCount(0) {}
    // Read next bit, returns -1 if EOF
    int readBit() {
        if (bitCount == 0) {
            if (!inFile.get(reinterpret_cast<char&>(buffer))) return -1;
            bitCount = 8;
        }
        int bit = (buffer >> (bitCount - 1)) & 1;
        bitCount--;
        return bit;
    }
};

// Build Huffman Tree from frequency table
Node* buildTree(const unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, compare> pq;
    for (auto& p : freq) {
        pq.push(new Node(p.first, p.second));
    }
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* node = new Node('\0', left->freq + right->freq, left, right);
        pq.push(node);
    }
    return pq.empty() ? nullptr : pq.top();
}

// Compress file: read input, build tree, write freq table + bits
void compressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    if(!inFile) {
        cerr << "Can't open input file.\n"; return;
    }
    ofstream outFile(outputFile, ios::binary);
    if(!outFile) {
        cerr << "Can't open output file.\n"; return;
    }

    // Read full input file content
    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());

    // Frequency map
    unordered_map<char,int> freq;
    for(char c : content) freq[c]++;

    // Write frequency table
    writeFrequencyTable(outFile, freq);

    // Build Huffman tree & codes
    Node* root = buildTree(freq);
    unordered_map<char,string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // Write encoded bits to file
    BitWriter writer(outFile);
    for(char c : content) {
        writer.writeBits(huffmanCode[c]);
    }
    writer.flush();

    deleteTree(root);

    cout << "Compression done. Original size: " << content.size()*8 
         << " bits, Compressed size: " << outFile.tellp()*8 << " bits.\n";
}

// Decompress file: read freq table, reconstruct tree, decode bitstream
void decompressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    if(!inFile) {
        cerr << "Can't open input file.\n"; return;
    }
    ofstream outFile(outputFile, ios::binary);
    if(!outFile) {
        cerr << "Can't open output file.\n"; return;
    }

    // Read frequency table
    unordered_map<char,int> freq = readFrequencyTable(inFile);

    // Build Huffman tree
    Node* root = buildTree(freq);

    // Decode bitstream using tree
    BitReader reader(inFile);
    Node* current = root;
    int bit;
    while((bit = reader.readBit()) != -1) {
        if(bit == 0)
            current = current->left;
        else
            current = current->right;

        if(!current->left && !current->right) { // leaf node
            outFile.put(current->ch);
            current = root;
        }
    }

    deleteTree(root);
    cout << "Decompression done.\n";
}

// Command-line interface
int main(int argc, char* argv[]) {
    if(argc != 4) {
        cout << "Usage:\n";
        cout << "  " << argv[0] << " compress <inputfile> <outputfile>\n";
        cout << "  " << argv << " decompress <inputfile> <outputfile>\n";
        return 1;
    }

    string op = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];

    if(op == "compress") {
        compressFile(inputFile, outputFile);
    } else if(op == "decompress") {
        decompressFile(inputFile, outputFile);
    } else {
        cerr << "Unknown operation: " << op << "\n";
        return 1;
    }
    return 0;
}

