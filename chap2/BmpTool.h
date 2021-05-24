//
// Created by 16337 on 2021/5/21 0021.
//
#ifndef CHAP2_BMPTOOL_H
#define CHAP2_BMPTOOL_H

#include <Windows.h>
#include <cstdio>
#include <iostream>

class BmpTool
{
private:
    BYTE *pImg = nullptr;
    long width = 0.0, height = 0.0;
    BITMAPFILEHEADER fileHeader{};
    BITMAPINFOHEADER infoHeader{};

public:
    ~BmpTool();

    bool read8BitBmp2Img(const char *filename);

    bool read24BitBmp2Img(const char *filename);

    short int *read14bitRaw2Img(const char *filename, long w, long h);

    bool write8BitImg2Bmp(const char *filename);

    bool write24BitImg2Bmp(const char *filename);

    void rgb2gry1();

    void rgb2gry2();

    void linearStretch(double k, double b);

    void getHistogram(int *histogram);

    static void getBrightContrast(const int *histogram, double *bright, double *contrast);

    void meanVarRegular(double bright, double contrast, double trgBright, double trgContrast);

    void histogramEqualize();

    void histogramEqualize(const short int* p14Img);

    void histogramEqualizeRgb();

//    void histogramEqualizeRgb1();





};


#endif //CHAP2_BMPTOOL_H
