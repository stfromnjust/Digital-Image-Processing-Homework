//
// Created by 16337 on 2021/5/21 0021.
//
#include "BmpTool.h"
#include <cstdio>
#include <cmath>

using namespace std;


bool BmpTool::read8BitBmp2Img(const char *filename)
{
    delete pImg;
    FILE *fp;
    unsigned int size;
    int Suc = 1, w, h;
    int k, extend, tmp;
    // Open File
    width = height = 0;
    fp = fopen(filename, "rb");
    if (fp == nullptr)
    {
        printf("Fail to open file.\n");
        return false;
    }
    // Read Struct Info
    if (fread((void *) &fileHeader, 1, sizeof(fileHeader), fp) != sizeof(fileHeader)) Suc = 0;
    if (fread((void *) &infoHeader, 1, sizeof(infoHeader), fp) != sizeof(infoHeader)) Suc = 0;
    if (!Suc || (fileHeader.bfOffBits < sizeof(fileHeader) + sizeof(infoHeader)))
    {
        fclose(fp);
        return false;
    }
    // Read Image Data
    width = w = infoHeader.biWidth;
    height = h = infoHeader.biHeight;

    extend = (w + 3) / 4 * 4 - w;
    size = infoHeader.biWidth * infoHeader.biHeight;
    fseek(fp, (long) fileHeader.bfOffBits, SEEK_SET);
    if ((pImg = new BYTE[size]) != nullptr)
    {
        for (int i = 0; i < h; i++)  // 0,1,2,3,4(5): 400-499
        {
            if ((int) fread(pImg + (h - 1 - i) * w, sizeof(BYTE), w, fp) != w)
            {
                fclose(fp);
                delete pImg;
                pImg = nullptr;
                return false;
            }

            for (k = 0; k < extend; k++)
            {
                if (fread(&tmp, 1, 1, fp) != 1)
                {
                    fclose(fp);
                    delete pImg;
                    pImg = nullptr;
                    return false;
                }
            }
        }
    }
    fclose(fp);
    return true;
}

short int *BmpTool::read14bitRaw2Img(const char *filename, long w, long h)
{
    FILE *fp;
    fp = fopen(filename, "rb");
    if (fp == nullptr)
    {
        printf("Fail to open file.\n");
        return nullptr;
    }
    width = w, height = h;
    auto *p14Img = new short int[w * h];
    fread(p14Img, sizeof(short int), width * height, fp);
    fclose(fp);
    return p14Img;
}

bool BmpTool::read24BitBmp2Img(const char *filename)
{
    delete pImg;
    FILE *fp;
    unsigned int size;
    int Suc = 1, w, h;
    int k, extend, tmp;
    // Open File
    width = height = 0;
    fp = fopen(filename, "rb");
    if (fp == nullptr)
    {
        printf("Fail to open file.\n");
        return false;
    }
    // Read Struct Info
    if (fread((void *) &fileHeader, 1, sizeof(fileHeader), fp) != sizeof(fileHeader)) Suc = 0;
    if (fread((void *) &infoHeader, 1, sizeof(infoHeader), fp) != sizeof(infoHeader)) Suc = 0;
    if (!Suc || (fileHeader.bfOffBits < sizeof(fileHeader) + sizeof(infoHeader)))
    {
        fclose(fp);
        return false;
    }
    // Read Image Data
    width = w = infoHeader.biWidth;
    height = h = infoHeader.biHeight;

    extend = (w * 3 + 3) / 4 * 4 - w * 3;
    size = infoHeader.biWidth * infoHeader.biHeight * 3;
    fseek(fp, (long) fileHeader.bfOffBits, SEEK_SET);
    if ((pImg = new BYTE[size]) != nullptr)
    {
        for (int i = 0; i < h; i++)  // 0,1,2,3,4(5): 400-499
        {
            if ((int) fread(pImg + (h - 1 - i) * w * 3, sizeof(BYTE), w * 3, fp) != w * 3)
            {
                fclose(fp);
                delete pImg;
                pImg = nullptr;
                return false;
            }

            for (k = 0; k < extend; k++)
            {
                if (fread(&tmp, 1, 1, fp) != 1)
                {
                    fclose(fp);
                    delete pImg;
                    pImg = nullptr;
                    return false;
                }
            }
        }
    }
    fclose(fp);
    return true;
}

bool BmpTool::write8BitImg2Bmp(const char *filename)
{
    FILE *fp;
    int i, extend;
    bool Suc = true;
    BYTE p[4], *pCur;
    // Open File
    if ((fp = fopen(filename, "w+b")) == nullptr)
    { return false; }
    // Fill the fileHeader
    fileHeader.bfType = ((WORD) ('M' << 8) | 'B');
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4L;
    fileHeader.bfSize = fileHeader.bfOffBits + width * height;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    if (fwrite((void *) &fileHeader, 1, sizeof(fileHeader), fp) != sizeof(fileHeader)) Suc = false;
    // Fill the ImgHeader
    infoHeader.biSize = 40;
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 8;
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;
    if (fwrite((void *) &infoHeader, 1, sizeof(infoHeader), fp) != sizeof(infoHeader)) Suc = false;

    // write Palette
    for (i = 0; i < 256; i++)
    {
        p[3] = 0;
        p[0] = p[1] = p[2] = i; //blue,green,red
        if (fwrite((void *) p, 1, 4, fp) != 4)
        {
            Suc = false;
            break;
        }
    }
    // write image data
    extend = (width + 3) / 4 * 4 - width;
    if (extend == 0)
    {
        for (pCur = pImg + (height - 1) * width; pCur >= pImg; pCur -= width)
        {
            if (fwrite((void *) pCur, 1, width, fp) != (unsigned int) width) Suc = false;
        }
    }
    else
    {
        for (pCur = pImg + (height - 1) * width; pCur >= pImg; pCur -= width)
        {
            if (fwrite((void *) pCur, 1, width, fp) != (unsigned int) width) Suc = false;
            for (i = 0; i < extend; i++)
            {
                if (fwrite((void *) (pCur + width - 1), 1, 1, fp) != 1) Suc = false;
            }
        }
    }
    // return;
    fclose(fp);
    return Suc;
}

bool BmpTool::write24BitImg2Bmp(const char *filename)
{
    FILE *fp;
    bool Suc = true;
    int i, extend;
    BYTE *pCur;

    // Open File
    if ((fp = fopen(filename, "w+b")) == nullptr)
    { return false; }
    // Fill the fileHeader
    fileHeader.bfType = ((WORD) ('M' << 8) | 'B');
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    fileHeader.bfSize = fileHeader.bfOffBits + width * height * 3L;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    if (fwrite((void *) &fileHeader, 1, sizeof(BITMAPFILEHEADER), fp) != sizeof(BITMAPFILEHEADER)) Suc = false;
    // Fill the ImgHeader
    infoHeader.biSize = 40;
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;
    if (fwrite((void *) &infoHeader, 1, sizeof(BITMAPINFOHEADER), fp) != sizeof(BITMAPINFOHEADER)) Suc = false;
    // write image data
    extend = ((width * 3 + 3) / 4 * 4) - width * 3;
    if (extend == 0)
    {
        for (pCur = pImg + (height - 1) * 3 * width; pCur >= pImg; pCur -= 3 * width)
        {
            if (fwrite((void *) pCur, 1, width * 3, fp) != (unsigned int) (3 * width)) Suc = false;
        }
    }
    else
    {
        for (pCur = pImg + (height - 1) * 3 * width; pCur >= pImg; pCur -= 3 * width)
        {
            if (fwrite((void *) pCur, 1, width * 3, fp) != (unsigned int) (3 * width)) Suc = false;
            for (i = 0; i < extend; i++)
            {
                if (fwrite((void *) (pCur + 3 * (width - 1) + 0), 1, 1, fp) != 1) Suc = false;
            }
        }
    }
    // return;
    fclose(fp);
    return Suc;
}

void BmpTool::rgb2gry1()
{
    BYTE *pCur, *pEnd = pImg + width * height * 3, *pResImg = new BYTE[width * height];
    BYTE *pRet = pResImg;
    for (pCur = pImg; pCur < pEnd; pCur += 3, pResImg++)
    {
        BYTE r = *pCur;
        BYTE g = *(pCur + 1);
        BYTE b = *(pCur + 2);
        *pResImg = max(0, min(255, int(299 * r + 587 * g + 114 * b) / 1000));
    }
    delete pImg;
    pImg = pRet;
}

void BmpTool::rgb2gry2()
{
    int LUTRed[256], LUTGreen[256], LUTBlue[256];
    for (int g = 0; g < 256; g++)
    {
        LUTRed[g] = 299 * g;
        LUTGreen[g] = 587 * g;
        LUTBlue[g] = 114 * g;
    }
    BYTE *pCur, *pEnd = pImg + width * height * 3, *pResImg = new BYTE[width * height];
    BYTE *pRet = pResImg;
    for (pCur = pImg; pCur < pEnd; pCur += 3, pResImg++)
    {
        BYTE r = *pCur;
        BYTE g = *(pCur + 1);
        BYTE b = *(pCur + 2);
        *pResImg = (LUTRed[r] + LUTGreen[g] + LUTBlue[b]) / 1000;
    }
    delete pImg;
    pImg = pRet;
}

void BmpTool::linearStretch(double k, double b)
{
    BYTE *pCur, *pEnd;
    BYTE LUT[256];
    for (int g = 0; g < 256; g++)
    {
        LUT[g] = max(0, min(255, (int) (k * g + b)));
    }
    pEnd = pImg + width * height;
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        *(pCur) = LUT[*pCur];
    }
}

void BmpTool::getBrightContrast(const int *histogram, double *bright, double *contrast)
{
    int g;
    int sum, num;
    double fsum;
    for (sum = num = 0, g = 0; g < 256; g++)
    {
        sum += histogram[g] * g;
        num += histogram[g];
    }
    *bright = sum * 1.0 / num;
    for (fsum = 0.0, g = 0; g < 256; g++)
    {
        fsum += histogram[g] * (g - *bright) * (g - *bright);
    }
    *contrast = sqrt(fsum / (num - 1));
}

void BmpTool::getHistogram(int *histogram)
{
    BYTE *pCur, *pEnd = pImg + width * height;
    memset(histogram, 0, sizeof(int) * 256);
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        histogram[*(pCur)]++;
    }
}

void BmpTool::meanVarRegular(double bright, double contrast, double trgBright, double trgContrast)
{
    double k = trgContrast / contrast;
    double b = trgBright - bright * k;
    linearStretch(k, b);
}

void BmpTool::histogramEqualize()
{
    BYTE *pCur, *pEnd = pImg + width * height;
    int histogram[256], LUT[256], A, g;
    // get histogram
    memset(histogram, 0, sizeof(int) * 256);
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        histogram[*(pCur)]++;
    }
    // get LUT
    A = histogram[0];
    LUT[0] = 255 * A / (width * height);
    for (g = 1; g < 256; g++)
    {
        A += histogram[g];
        LUT[g] = 255 * A / (width * height);
    }
    // Equalize
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        *(pCur) = LUT[*pCur];
    }
}

void BmpTool::histogramEqualize(const short int *p14Img)
{
    int histogram[1 << 14], LUT[1 << 14], A, i, g;
    pImg = new BYTE[width * height];
    // get Histogram
    memset(histogram, 0, sizeof(int) * (1 << 14));
    for (i = 0; i < width * height; i++)
    {
        histogram[*(p14Img + i)]++;
    }
    A = histogram[0];
    LUT[0] = 255 * A / (width * height);
    for (g = 1; g < (1 << 14); g++)
    {
        A += histogram[g];
        LUT[g] = 255 * A / (width * height);
    }
    for (i = 0; i < width * height; i++)
    {
        pImg[i] = LUT[*(p14Img + i)];
    }
}

void BmpTool::histogramEqualizeRgb()
{
    BYTE *pCur, *pEnd = pImg + width * height * 3;
    int histRed[256], histGreen[256], histBlue[256], Ar, Ag, Ab, g;
    int LUTRed[256], LUTGreen[256], LUTBlue[256];
    // get Histogram
    memset(histRed, 0, sizeof(int) * 256);
    memset(histGreen, 0, sizeof(int) * 256);
    memset(histBlue, 0, sizeof(int) * 256);
    for (pCur = pImg; pCur < pEnd; pCur += 3)
    {
        histRed[*(pCur)]++;
        histGreen[*(pCur + 1)]++;
        histBlue[*(pCur + 2)]++;
    }
    // get LUT
    Ar = histRed[0], Ag = histGreen[0], Ab = histBlue[0];
    LUTRed[0] = 255 * Ar / (width * height);
    LUTGreen[0] = 255 * Ag / (width * height);
    LUTBlue[0] = 255 * Ab / (width * height);
    for (g = 1; g < 256; g++)
    {
        Ar += histRed[g];
        Ag += histGreen[g];
        Ab += histBlue[g];
        LUTRed[g] = 255 * Ar / (width * height);
        LUTGreen[g] = 255 * Ag / (width * height);
        LUTBlue[g] = 255 * Ab / (width * height);
    }
    // Equalize
    for (pCur = pImg; pCur < pEnd; pCur += 3)
    {
        *(pCur) = LUTRed[*(pCur)];
        *(pCur + 1) = LUTGreen[*(pCur + 1)];
        *(pCur + 2) = LUTBlue[*(pCur + 2)];
    }
}

// TODO bad result
//void BmpTool::histogramEqualizeRgb1()
//{
//    BYTE *pCur, *pEnd = pImg + width * height * 3;
//    auto *pHsiImg = new double[width * height * 3];
//    double *pHsiEnd = pHsiImg + width * height * 3;
//    double *pHsiCur;
//    for (pCur = pImg, pHsiCur = pHsiImg; pCur < pEnd; pCur += 3, pHsiCur += 3)
//    {
//        BYTE r = *(pCur), g = *(pCur + 1), b = *(pCur + 2);
//        double h, s, i;
//        double theta = acos((0.5 * (r - g + r - b)) / (sqrt((r - g) * (r - g) + (r - b) * (g - b) + 1e-9)));
//        if (b <= g) h = theta; else h = 2 * PI - theta;
//        s = 1 - 3.0 * min(b, min(r, g)) / (r + g + b);
//        i = (r + g + b) / 3.0;
//        *(pHsiCur) = h / (2 * PI);
//        *(pHsiCur + 1) = s;
//        *(pHsiCur + 2) = i;
//    }
//    // Equalize i
//    // get hist
//    int histI[256];
//    int LUTI[256];
//    for (pHsiCur = pHsiImg; pHsiCur < pHsiEnd; pHsiCur += 3)
//    {
//        histI[int(*(pHsiCur + 2))]++;
//    }
//    // get LUTI
//    int ai = histI[0];
//    LUTI[0] = 255 * ai / (width * height);
//    for (int g = 1; g < 256; g++)
//    {
//        ai += histI[g];
//        LUTI[g] = 255 * ai / (width * height);
//    }
//    for (pCur = pImg, pHsiCur = pHsiImg; pCur < pEnd; pCur += 3, pHsiCur += 3)
//    {
//        *(pHsiCur + 2) = LUTI[int(*(pHsiCur + 2))];
//        double h, s, i;
//        h = *(pHsiCur) * 2 * PI;
//        s = *(pHsiCur + 1);
//        i = *(pHsiCur + 2);
//        if (h >= 0 && h < 2 / 3 * PI)
//        {
//            *(pCur) = i * (1 + s * cos(h) / (cos(PI / 3 - h) + 1e-9));
//            *(pCur + 2) = i * (1 - s);
//            *(pCur + 1) = 3 * i - (*(pCur) + *(pCur + 2));
//        }
//        else if (h >= 2 / 3 * PI && h < 4 / 3 * PI)
//        {
//            h = h - 2 * PI / 3;
//            *(pCur) = i * (1 - s);
//            *(pCur + 1) = i * (1 + s * cos(h) / (cos(PI / 3 - h) + 1e-9));
//            *(pCur + 2) = 3 * i - (*(pCur) + *(pCur + 1));
//        }
//        else if (h >= 4 / 3 * PI && h <= 2 * PI)
//        {
//
//            h = h - 4 * PI / 3;
//            *(pCur + 1) = i * (1 - s);
//            *(pCur + 2) = i * (1 + s * cos(h) / (cos(PI / 3 - h) + 1e-9));
//            *(pCur) = 3 * i - (*(pCur + 1) + *(pCur + 2));
//        }
//    }
//}

BmpTool::~BmpTool()
{
    delete pImg;
}








