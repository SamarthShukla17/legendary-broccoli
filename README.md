Huffman Coding File Compressor & Decompressor
A command-line C++ tool for lossless file compression and decompression using the classic Huffman coding algorithm. This utility supports text, binary, and image files, delivering efficient space savings for files with repeating patterns.

Table of Contents
Features

How It Works

Getting Started

Compilation

Usage

Supported File Types

Compression Analytics

Contributing

License

Features
🔗 Lossless Compression & Decompression: 100% data preservation—original file restored upon decompressing.

📦 Works with Any File Type: Text, images, executables… if you can save it, you can compress it.

🚀 Efficient Bitwise Encoding: Packs bits into bytes using true Huffman codes for real disk savings.

🗂️ Frequency Table Storage: Automatically stores code-mapping inside compressed archives for reliable decompression.

💻 Simple CLI: Compress or decompress with a single command.

How It Works
Compress Mode

Reads your input file byte by byte.

Counts the frequency of every byte.

Builds a Huffman tree assigning shorter codes to frequent bytes.

Encodes the data, packing bits efficiently.

Stores the frequency table in the output for later decoding.

Decompress Mode

Reads the frequency table and compressed bits.

Rebuilds the Huffman tree.

Decodes the bits to restore original file content, byte-for-byte.

Getting Started
Compilation
Ensure you have a standard C++ compiler (like GCC or MinGW).
To compile:

text
g++ -o HuffmanHuff HuffmanProject.cpp
Usage
text
# To compress any file:
./HuffmanHuff compress input.txt compressed.huff

# To decompress back:
./HuffmanHuff decompress compressed.huff output.txt
Replace input.txt and compressed.huff with your actual file names.

Works similarly for images, binaries, logs, etc.

Supported File Types
Plain text files (.txt, .csv, .log)

Images (.bmp, .png, etc. - non-lossy formats recommended)

Executables (.exe, .bin)

Any other file type—just remember decompressed output matches the original byte-for-byte!

Compression Analytics
Typical compression ratio:

File Type	Original Size	Compressed Size	Savings
Plain Text	100%	40–60%	40–60%
Simple BMP Image	100%	50–60%	40–50%
Random/ZIP/JPG	100%	90–110%	-10–10%
Compression rates depend on file content. Best results seen in files with repeating patterns or characters.

Contributing
Contributions are welcome!

Fork the repository

Create your feature branch (git checkout -b feature/AmazingFeature)

Commit your changes (git commit -m 'Add some AmazingFeature')

Push to the branch (git push origin feature/AmazingFeature)

Open a Pull Request

Bug reports and feature requests?
Use Issues.

License
This project is open source, available under the MIT License.

Credits
Based on the classic Huffman coding algorithm.
Thanks to open-source community and algorithm resources for design and benchmarking.
