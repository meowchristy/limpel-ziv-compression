# lempel-ziv-compression

implementation of lz78 compression algorithm

## building
```
  $ make
  $ make all
```
## running
```
  $ ./encode -i <input file> -o <output file>
    -i <input file>  : specifies input to be compressed
    -o <output file> : specifies output of compressed input
    -v               : prints compression statistics, optional
    -h               : displays program synopsis and usage
```
```
  $ ./decode -i <input file> -o <output file>
    -i <input file>  : specifies input to be decompressed
    -o <output file> : specifies output of decompressed input
    -v               : prints decompression statistics, optional
    -h               : displays program synopsis and usage
```
## cleaning
```
  $ make clean
```
