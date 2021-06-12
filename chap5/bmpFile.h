//
// Created by 16337 on 2021/6/10 0010.
//
#ifndef CHAP5_BMPFILE_H
#define CHAP5_BMPFILE_H

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

void getHistogram(BYTE *pImg, int width, int height, int *pHistogram);

void histogramSmooth(int *pHistogram, int m, int nSize, int *pResHistogram);

void drawHistogram2Img(int *pHistogram, int histSize,
                       double maxScale, // 最大值的占比
                       BYTE *pHistImg, int width, int height);

void thresholdBin(BYTE *pImg, int width, int height, int threshold);

int getOtsuThreshold(int *pHistogram, int nSize=256);

void reduceGrayscale(BYTE *pImg, int width, int height, int offset);

bool clusterHistogram(int *pHistogram, int nClass, double decision, double *center);

#endif //CHAP5_BMPFILE_H