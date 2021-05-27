//
// Created by 16337 on 2021/5/25 0025.
//
#include <iostream>
#include "bmpFile.h"

using namespace std;

void t3()
{
    int width, height;
    BYTE *pImg, *pResImg;
    pImg = read24BitBmp2Img("../resource/H0301Rgb.bmp", &width, &height);
    pResImg = new BYTE[width * height];
    clock_t start = clock(), end;
    for (int i = 0; i < 1000; i++)
    {
//        rgb2gry1(pImg, width, height, pResImg);
        rgb2gry2(pImg, width, height, pResImg);
    }
    end = clock();
    printf("Time: %d ms\n", end - start);
    write8BitImg2Bmp(pResImg, width, height, "../output/H0301Gry.bmp");
    delete [] pResImg;
}

void t4()
{
    int width, height;
    BYTE *pImg, *pResImg;
    pImg = read8BitBmp2Img("../resource/H0302Gry.bmp", &width, &height);
    pResImg = new BYTE[width * height]();
    meanFilterBySumCol(pImg, width, height, 21, pResImg);   // 一维 m列 1行
//    meanFilterBySumCol(pImg, width, height, 21, 1, pResImg);  // 二维
    write8BitImg2Bmp(pResImg, width, height, "../output/H0302Gry_t4.bmp");
    delete [] pResImg;
}

void t5()
{
    int width, height;
    BYTE *pImg, *pResImg;
    pImg = read8BitBmp2Img("../resource/H0302Gry.bmp", &width, &height);
    pResImg = new BYTE[width * height]();
    int *pIntegral = new int[width * height]();
    get1DIntegral(pImg, width, height, pIntegral);
    meanFilterBy1DIntegral(pIntegral, width, height, 21, pResImg);  // 一维 m列 1行
//    get2DIntegral(pImg, width, height, pIntegral);
//    meanFilterBy2DIntegral(pIntegral, width, height, 21, 1, pResImg); // 二维
    write8BitImg2Bmp(pResImg, width, height, "../output/H0302Gry_t5.bmp");
    delete [] pResImg;
    delete [] pIntegral;
}

void t6()
{
    int width, height;
    BYTE *pImg;
    pImg = read8BitBmp2Img("../resource/H0302Gry.bmp", &width, &height);
    invertImgSSE(pImg, width, height);
    write8BitImg2Bmp(pImg, width, height, "../output/H0302Gry_t6.bmp");
}

void t7()
{
    int width, height;
    BYTE *pImg, *pResImg;
    pImg = read8BitBmp2Img("../resource/H0302Gry.bmp", &width, &height);
    pResImg = new BYTE[width * height]();
    double avgCmpTimes = medianFilter(pImg, width, height, 21, 1, pResImg);
    write8BitImg2Bmp(pResImg, width, height, "../output/H0302Gry_t7.bmp");
    printf("avgCmpTimes: %.2f\n", avgCmpTimes);
    delete [] pResImg;
}

void t12()
{
    int width, height;
    BYTE *pImg, *pTemp1Img, *pTemp2Img, *pTemp3Img, *pResImg;
    pImg = read8BitBmp2Img("../resource/H0303Gry.bmp", &width, &height);
    pResImg = new BYTE[width * height]();
    pTemp1Img = new BYTE[width * height]();
    pTemp2Img = new BYTE[width * height]();
    pTemp3Img = new BYTE[width * height]();
    double std = 3.0;
    int gaussianSize = 2 * lround(3 * std) + 1;
    int *pGaussian = new int [gaussianSize];
    getGaussianFilter(std, pGaussian);
//    for (int i = 0; i < gaussianSize; i++)
//    {
//        printf("%d\t", pGaussian[i]);
//    }
    gaussianFilter(pImg, width, height, pGaussian, gaussianSize, pTemp1Img);
    transposeImg(pTemp1Img, width, height, pTemp2Img);
    gaussianFilter(pTemp2Img, width, height, pGaussian, gaussianSize, pTemp3Img);
    transposeImg(pTemp3Img, width, height, pResImg);
    write8BitImg2Bmp(pResImg, width, height, "../output/H0303Gry_t12.bmp");
    delete [] pGaussian;
    delete [] pTemp1Img;
    delete [] pTemp2Img;
    delete [] pTemp3Img;
    delete [] pResImg;
}

void testTranspose()
{
    int width, height;
    BYTE *pImg, *pResImg;
    pImg = read8BitBmp2Img("../resource/H0303Gry.bmp", &width, &height);
    pResImg = new BYTE[width * height]();
    transposeImg(pImg, width, height, pResImg);
    write8BitImg2Bmp(pResImg, width, height, "../output/H0303Gry_transpose.bmp");
    delete [] pResImg;
}

int main(void)
{
//    test();
//    t3();
//    t4();
//    t5();
    t6();
//    t7();
//    t12();
//    testTranspose();
    return 0;
}
