//
// Created by 16337 on 2021/6/21 0021.
//
#ifndef CHAP6_BMPFILE_H
#define CHAP6_BMPFILE_H

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

bool write8BitImg2BmpMark(BYTE *pImg, int width, int height, const char *filename);

int collectEdgePoints_Fig(BYTE *pImg, int width, int height,
                          int *xIdx, int *yIdx);

int collectEdgePoints_H(BYTE *pImg, int width, int height,
                          int *xIdx, int *yIdx);

void drawABCLine(BYTE *pImg, int width, int height,
                 double A, double B, double C,
                 int color);

void eraseABCLine(BYTE *pImg, int width, int height,
                  double A, double B, double C,
                  int color);

void getHoughCount_Line(int *x, int *y, int N,
                          int maxRho, int *pCount);

void getHoughCount_Circle(int *x, int *y, int N,
                          int r,    // 已知半径r
                          int width, int height, int *pCount);

// 霍夫直线空间计数器
void getCountImg(int *pCount, int width, int height, BYTE *pImg);

// 分治法圆心位置计数器
void getCountImg(int *pCount, int histSize,  // 一个X/Y对应直方图宽度
                 double maxScale,   // 最大直方图所占图片比例
                 BYTE *pImg, int width, int height);

void getBstHoughLine(int *pCount, int width, int height,
                     int *bstTheta, int *bstRho);

void eraseCountImg(int *pCount, int width, int height,
                   int *eraseTheta, int *eraseRho, int radius);
// 分治法求圆心
int getCircleX(BYTE *pImg, int width, int height, int *pCount);

int getCircleY(BYTE *pImg, int width, int height, int *pCount);

//链码跟踪
int traceContourRmw(BYTE *pImg, int width, int height,  // 二值图像
                    int x0, int y0, // 轮廓起点
                    bool isOuter,   //  是否是外轮廓
                    BYTE *pChainCode,   //  链码序列
                    int maxChainCodeNum    // 最长序列
                    );

// 可视化链码序列
void drawContour(BYTE *pImg, int width,
                 int x0, int y0,    // 轮廓起点
                 BYTE *pChainCode, int N,    //  链码序列, 链码长度
                 int color);
#endif //CHAP6_BMPFILE_H
