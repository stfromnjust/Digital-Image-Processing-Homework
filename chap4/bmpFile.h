//
// Created by 16337 on 2021/5/31 0031.
//

#ifndef CHAP4_BMPFILE_H
#define CHAP4_BMPFILE_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <time.h>
#include <algorithm>
#include <iostream>

BYTE *read8BitBmp2Img(const char *filename, int *width, int *height);

bool write8BitImg2Bmp(BYTE *pImg, int width, int height, const char *filename);

BYTE *read24BitBmp2Img(const char *filename, int *width, int *height);

bool write24BitImg2Bmp(BYTE *pImg, int width, int height, const char *filename);

void invertImg(BYTE *pImg, int width, int height);

void gradientOp(BYTE *pImg, int width, int height, BYTE *pResImg);  // 梯度算子

void robertOp(BYTE *pImg, int width, int height, BYTE *pResImg); // robert算子

void sobelOp(BYTE *pImg, int width, int height, BYTE *pResImg); // sobel算子

void sobelOp(BYTE *pImg, int width, int height, int step, int *pGradDir, BYTE *pResImg);

void prewittOp(BYTE *pImg, int width, int height, BYTE *pResImg); // prewitt算子

void robinsonOp(BYTE *pImg, int width, int height, BYTE *pResImg); // robinson算子

void kirschOp(BYTE *pImg, int width, int height, BYTE *pResImg);    //  kirsch算子

void shenJunOp(BYTE *pImg, BYTE *pTempImg, int width, int height, double a0, BYTE *pResImg);   // Shen Jun算子

void sj_sobelOp(BYTE *pImg, BYTE *pTempImg,
                int width, int height, double a0, int threshold,
                BYTE *pResImg); //  ShenJun + sobel算子

void zoomOutImg(BYTE *pImg, int width, int height, BYTE *pResImg, int k, int flag);  // 缩小k倍

void get2DIntegral(BYTE *pImg, int width, int height, int *pIntegral);  // 二维积分图

void getTextPos(BYTE *pImg, int width, int height, int *pIntegral, int para1, int para2, int *resX, int *resY);

void cannyOp(BYTE *pImg, int width, int height, double std, int *pGradDir, BYTE lowThreshold, BYTE highThreshold, BYTE *pTempImg);

void getGaussianFilter(double std, int *pGaussian);

void gaussianFilter1D(BYTE *pImg, int width, int height, int *pGaussian, int m, BYTE *pResImg);

void transposeImg(BYTE *pImg, int width, int height, BYTE *pResImg);

void gaussianFilter2D(BYTE *pImg ,int width, int height, double std, BYTE *pResImg);

void nonMaximumSuppression(BYTE *pImg, int width, int height, int step, int *pGradDir);

void doubleThreshold(BYTE *pImg, int width, int height, int step, BYTE lowThreshold, BYTE highThreshold);

void doubleThresholdLink(BYTE *pImg, int width, int height, int step, BYTE lowThreshold, BYTE highThreshold);

#endif //CHAP4_BMPFILE_H
