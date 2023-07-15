Christy Miao 
cmiao4
Winter 2023
# Assignment 6 - Lempel-Ziv Compression

This program aims to create two programs: encode and decode. They are both expected to perform LZ78 compression and decompression, respectively. 

## Building

To build, run '$ make or '$ make all' on the command line within the assignment 6's directory. This should create two executable binary files titled 'encode', and 'decode', which the user can then run. If you would like to create one of the executable binary files, run '$ make' followed by 'encode', or 'decode'. 

## Running

Typing '$ ./encode', followed by one or more of the following valid arguments (-v, -i, -o, -h) will allow you to compress any file, text, or binary.

-v prints compression statistics to stderr, -i specifies input to compress, -o specifies output of compressed input, -h displays program synopsis and usage.

Typing '$ ./decode', followed by one or more of the following valid arguments (-v, -i, -o, -h) will allow you to decompress any file, text, or binary, that was compressed with encode.

-v prints decompression statistics to stderr, -i specifies input to decompress, -o specifies output of decompressed input, -h displays program synopsis and usage.
