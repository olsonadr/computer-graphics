// bmp.hpp
#ifndef BMP_HPP // include guard
#define BMP_HPP

#include <stdio.h>
#include <math.h>

#define VERBOSE false
#define BMP_MAGIC_NUMBER 0x4d42
#ifndef BI_RGB
    #define BI_RGB 0
    #define BI_RLE8 1
    #define BI_RLE4 2
#endif

// bmp file header:
inline struct bmfh
{
    short bfType; // BMP_MAGIC_NUMBER = "BM"
    int bfSize;   // size of this file in bytes
    short bfReserved1;
    short bfReserved2;
    int bfOffBytes; // # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
inline struct bmih
{
    int biSize;        // info header size, should be 40
    int biWidth;       // image width
    int biHeight;      // image height
    short biPlanes;    // #color planes, should be 1
    short biBitCount;  // #bits/pixel, should be 1, 4, 8, 16, 24, 32
    int biCompression; // BI_RGB, BI_RLE4, BI_RLE8
    int biSizeImage;
    int biXPixelsPerMeter;
    int biYPixelsPerMeter;
    int biClrUsed; // # colors in the palette
    int biClrImportant;
} InfoHeader;

// function prototypes
unsigned char *BmpToTexture(const char *filename, int *width, int *height);
int ReadInt(FILE *fp);
short ReadShort(FILE *fp);
void HsvRgb(float hsv[3], float rgb[3]);

#endif // BMP_HPP