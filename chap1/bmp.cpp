#include "bmp.h"
#include <cstdio>
#include <malloc.h>

void showBmpFileHeader(BmpFileHeader* fileHeader)
{
    printf("//BMP File Header\n");
    printf("bfSize: %d\n", fileHeader->bfSize/1024);
    printf("bfReserved1: %d\n",  fileHeader->bfReserved1);
    printf("bfReserved2: %d\n",  fileHeader->bfReserved2);
    printf("bfOffBits: %d\n",  fileHeader->bfOffBits);
}

void showBmpInfoHeader(BmpInfoHeader* infoHeader)
{
    printf("//BMP Info Header\n");
    printf("biSize: %d\n", infoHeader->biSize);
    printf("biWidth: %d\n", infoHeader->biWidth);
    printf("biHeight: %d\n", infoHeader->biHeight);
    printf("biPlanes(num of BmpRGBQuad: default: 1): %d\n", infoHeader->biPlanes);
    printf("biBitCount: %d\n", infoHeader->biBitCount);
    printf("biCompression: %d\n", infoHeader->biCompression);
    printf("biSizeImage: %d\n", infoHeader->biSizeImage);
    printf("biXPixelsPerMeter: %d\n", infoHeader->biXPixelsPerMeter);
    printf("biYPixelsPerMeter: %d\n", infoHeader->biYPixelsPerMeter);
    printf("biClrUsed: %d\n", infoHeader->biClrUsed);
    printf("biClrImportant: %d\n", infoHeader->biClrImportant);
}

void inverseBmp(unsigned char* pImg, unsigned int imgPixelSize)
{
    unsigned char* pCur, *pEnd = pImg + imgPixelSize;
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        *pCur = 255 - *pCur;
    }
}

void pseudoColor(BmpRGBQuad* palette)
{

    for (int i = 0; i < 256; i++)
    {
//        rgbRed
        if (palette[i].rgbRed >= 0x00 && palette[i].rgbRed <= 0x80)
        {
            palette[i].rgbRed = 0x00;
        }
        else if(palette[i].rgbRed >= 0xc0)
        {
            palette[i].rgbRed = 0xff;
        }
        else
        {
            palette[i].rgbRed = 0x04 * (palette[i].rgbRed - 0xc0);
        }
//        rgbBlue
        if (palette[i].rgbBlue >= 0x00 && palette[i].rgbBlue <= 0x40)
        {
            palette[i].rgbBlue = 0xff;
        }
        else if(palette[i].rgbBlue >= 0x80)
        {
            palette[i].rgbBlue = 0x00;
        }
        else
        {
            palette[i].rgbBlue = -0x04 * (palette[i].rgbBlue - 0x40) + 0xff;
        }
//        rgbGreen
        if (palette[i].rgbGreen <= 0x40)
        {
            palette[i].rgbGreen = 0x04 * palette[i].rgbGreen;
        }
        else if (palette[i].rgbGreen >= 0xc0)
        {
            palette[i].rgbGreen = -0x04 * (palette[i].rgbGreen - 0xff);
        }
        else
        {
            palette[i].rgbGreen = 0xff;
        }
    }
}

void write8bitBmpTarget(BmpFileHeader* fileHeader, BmpInfoHeader* infoHeader, BmpRGBQuad* palette,
                        unsigned char* pImg, FILE* trg)
{
    unsigned short fileType = 0x4d42;
    fwrite(&fileType, sizeof(unsigned short), 1, trg);
    fwrite(fileHeader, sizeof(BmpFileHeader), 1, trg);
    fwrite(infoHeader, sizeof(BmpInfoHeader), 1, trg);
    fwrite(palette, sizeof(BmpRGBQuad), 256, trg);
    fwrite(pImg, sizeof(unsigned char), infoHeader->biSizeImage, trg);
}

void write24bitBmpTarget(BmpFileHeader* fileHeader, BmpInfoHeader* infoHeader,
                        unsigned char* pImg, FILE* trg)
{
    unsigned short fileType = 0x4d42;
    fwrite(&fileType, sizeof(unsigned short), 1, trg);
    fwrite(fileHeader, sizeof(BmpFileHeader), 1, trg);
    fwrite(infoHeader, sizeof(BmpInfoHeader), 1, trg);
    fwrite(pImg, sizeof(unsigned char), infoHeader->biSizeImage, trg);
}


int main()
{
    FILE* src;
    FILE* trgInverse;
    FILE* trgPseudoColor;
    src = fopen("../H0102Rgb.bmp", "rb");
    trgInverse = fopen("../targetInverse.bmp", "wb");
    trgPseudoColor = fopen("../targetPseudoColor.bmp", "wb");

    if(src == nullptr)
    {
        printf("fail to open this file.\n");
        return -1;
    }
    if (trgInverse == nullptr)
    {
        printf("fail to create target Inverse file.\n");
        return -1;
    }
    if (trgPseudoColor == nullptr)
    {
        printf("fail to create target Pseudo Color file.\n");
        return -1;
    }


//    检查是否是bmp, 文件信息描述, 数据信息描述
    unsigned short fileType;
    BmpFileHeader* fileHeader;
    BmpInfoHeader* infoHeader;
    BmpRGBQuad* palette;

    fileHeader = (BmpFileHeader*)malloc(sizeof(BmpFileHeader));
    infoHeader = (BmpInfoHeader*)malloc(sizeof(BmpInfoHeader));

    fread(&fileType, 1, sizeof(unsigned short), src);

    if (fileType != 0x4d42)
    {
        fclose(src);
        return -1;
    }

    printf("It is BMP!\n");
    printf("bfType: %d\n", fileType);
    fread(fileHeader, sizeof(BmpFileHeader), 1, src);
    showBmpFileHeader(fileHeader);
    fread(infoHeader, sizeof(BmpInfoHeader), 1, src);
    showBmpInfoHeader(infoHeader);

//    有调色板?
    if (infoHeader->biBitCount == 8)
    {
        palette = (BmpRGBQuad*)malloc(sizeof(BmpRGBQuad) * 256);
        fread(palette, sizeof(BmpRGBQuad), 256, src);
        unsigned int imgPixelSize = infoHeader->biSizeImage;
        auto* pImg = (unsigned char*)malloc(sizeof(unsigned char) * imgPixelSize);
        fread(pImg, sizeof(unsigned char), imgPixelSize, src);
//        8位Bmp反相
        inverseBmp(pImg, imgPixelSize);
        write8bitBmpTarget(fileHeader, infoHeader, palette, pImg, trgInverse);
//        8位伪彩色
//        恢复pImg
        fread(pImg, sizeof(unsigned char), imgPixelSize, src);
//        伪彩色需要更改Palette
        pseudoColor(palette);
        write8bitBmpTarget(fileHeader, infoHeader, palette, pImg, trgPseudoColor);
    }
    else if (infoHeader->biBitCount == 24)
    {
        unsigned int imgPixelSize = infoHeader->biSizeImage;
        auto* pImg = (unsigned char*)malloc(sizeof(unsigned char) * imgPixelSize);
        fread(pImg, sizeof(unsigned char), imgPixelSize, src);
//        24位Bmp反相
        inverseBmp(pImg, imgPixelSize);
        write24bitBmpTarget(fileHeader, infoHeader, pImg, trgInverse);
    }

    fclose(src);
    return 0;
}