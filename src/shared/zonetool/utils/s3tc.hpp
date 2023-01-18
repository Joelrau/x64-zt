#ifndef S3TC_H
#define S3TC_H

unsigned int PackRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void DecompressBlockDXT1(unsigned int x, unsigned int y, unsigned int width, const unsigned char* blockStorage, unsigned int* image);
void BlockDecompressImageDXT1(unsigned int width, unsigned int height, const unsigned char* blockStorage, unsigned int* image);
void DecompressBlockDXT5(unsigned int x, unsigned int y, unsigned int width, const unsigned char* blockStorage, unsigned int* image);
void BlockDecompressImageDXT5(unsigned int width, unsigned int height, const unsigned char* blockStorage, unsigned int* image);

unsigned int CompressedBlockSizeDXT1(unsigned int width, unsigned int height);
unsigned int CompressedBlockSizeDXT5(unsigned int width, unsigned int height);

#endif // S3TC_H