//
// Created by 16337 on 2021/5/25 0025.
//

#ifndef CHAP3_BMPFILE_H
#define CHAP3_BMPFILE_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <time.h>
#include <algorithm>
#include <iostream>
#include <mmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>

BYTE *read8BitBmp2Img(const char *filename, int *width, int *height);

bool write8BitImg2Bmp(BYTE *pImg, int width, int height, const char *filename);

BYTE *read24BitBmp2Img(const char *filename, int *width, int *height);

bool write24BitImg2Bmp(BYTE *pImg, int width, int height, const char *filename);

void rgb2gry1(BYTE *pImg, int width, int height, BYTE *pResImg);

void rgb2gry2(BYTE *pImg, int width, int height, BYTE *pResImg);

void meanFilterBySumCol(BYTE *pImg, int width, int height, int m, int n, BYTE *pResImg);

void meanFilterBySumCol(BYTE *pImg, int width, int height, int m, BYTE *pResImg);

void get2DIntegral(BYTE *pImg, int width, int height, int *pIntegral);

void get1DIntegral(BYTE *pImg, int width, int height, int *pIntegral);

void meanFilterBy2DIntegral(int *pIntegral, int width, int height, int m, int n, BYTE *pResImg);

void meanFilterBy1DIntegral(int *pIntegral, int width, int height, int m, BYTE *pResImg);

double medianFilter(BYTE *pImg, int width, int height, int m, int n, BYTE *pResImg);

void invertImg(BYTE *pImg, int width, int height);

void invertImgMMX(BYTE *pImg, int width, int height);

void invertImgSSE(BYTE *pImg, int width, int height);

void invertImgAVX(BYTE *pImg, int width, int height);

void getGaussianFilter(double std, int *pGaussian);

void gaussianFilter1D(BYTE *pImg, int width, int height, int *pGaussian, int m, BYTE *pResImg);

void transposeImg(BYTE *pImg, int width, int height, BYTE *pResImg);

void gaussianFilter2D(BYTE *pImg, int width, int height, double std, BYTE *pResImg);

#endif //CHAP3_BMPFILE_H
