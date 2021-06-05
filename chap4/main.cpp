//
// Created by 16337 on 2021/5/31 0031.
//
#include "bmpFile.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void t5()
{
    BYTE *pImg;
    int width, height;
    pImg = read8BitBmp2Img("../resource/H0401Gry.bmp", &width, &height);
    BYTE *pResImg = new BYTE[width * height];
//    gradientOp(pImg, width, height, pResImg);
//    robertOp(pImg, width, height, pResImg);
//    sobelOp(pImg, width, height, pResImg);
    prewittOp(pImg, width, height, pResImg);
//    robinsonOp(pImg, width, height, pResImg);
//    kirschOp(pImg, width, height, pResImg);
    BYTE *pTempImg = new BYTE[width * height];
//    shenJunOp(pImg, pTempImg, width, height, 0.5, pResImg);
//    sj_sobelOp(pImg, pTempImg, width, height, 0.5, 64, pResImg);
    invertImg(pResImg, width, height);
    write8BitImg2Bmp(pResImg, width, height, "../output/H0401Gry_prewitt.bmp");
    delete [] pTempImg; // if needed
    delete [] pResImg;
}

void t6_1(const string& imgName, int k)
{
    BYTE *pImg;
    int width, height;
//    step 1: 求边缘强度图
    pImg = read8BitBmp2Img(("../resource/" + imgName + ".bmp").c_str(), &width, &height);
    BYTE *pTempImg = new BYTE[width * height];
    BYTE *pResImg = new BYTE[width * height];
    sj_sobelOp(pImg, pTempImg, width, height, 0.5, 64, pResImg);
    write8BitImg2Bmp(pResImg, width, height, ("../output/" + imgName + "_sj_sobel.bmp").c_str());
//    step 2: 将边缘强度图缩小
    BYTE *pTempImg1 = new BYTE[(width / k) * height];
    zoomOutImg(pResImg, width, height, pTempImg1, k, 1);   // 宽度缩小
    BYTE *pResImg1 = new BYTE[(width / k) * (height / k)];
    zoomOutImg(pTempImg1, width / k, height, pResImg1, k, 0);    // 高度缩小
    write8BitImg2Bmp(pResImg1, width / k, height / k, ("../output/" + imgName + "_sj_sobel_zoomOut.bmp").c_str());
    delete [] pTempImg;
    delete [] pResImg;
    delete [] pTempImg1;
    delete [] pResImg1;
}

void t6_2(const string& imgName, int *resX, int *resY, int para1, int para2)
{
    BYTE *pImg;
    int width, height;
    pImg = read8BitBmp2Img(("../output/" + imgName + "_sj_sobel_zoomOut.bmp").c_str(), &width, &height);
    int *pIntegral = new int [width * height];
    get2DIntegral(pImg, width, height, pIntegral);
    getTextPos(pImg, width, height, pIntegral, para1, para2, resX, resY);
    delete [] pIntegral;
}

void t6(const string& imgName, int k, int para1, int para2)
{
    t6_1(imgName, k);
    int *resX = new int, *resY = new int;
    para1 /= k, para2 /= k;
    t6_2(imgName, resX, resY, para1, para2);
    printf("x: %d, y: %d", *resX, *resY);
//    利用opencv将区域标记出来
    Mat src = imread("../resource/" + imgName + ".bmp");
    rectangle(src, Point((*resX - para1 / 2) * k, (*resY - para1 / 2) * k),
              Point((*resX + para1 / 2) * k, (*resY + para1 / 2) * k),
              Scalar(255, 0, 0),
              3);
    imwrite("../output/" + imgName + "_result.bmp", src);
    delete resX;
    delete resY;
}

void t7()
{
    int width, height;
    BYTE *pImg;
    pImg = read8BitBmp2Img("../resource/H0401Gry.bmp", &width, &height);
    BYTE *pTempImg = new BYTE[width * height]();
    int *pGradDir = new int [width * height]();
    cannyOp(pImg, width, height, 1.4, pGradDir, 50, 100, pTempImg);
    write8BitImg2Bmp(pImg, width, height, "../output/H0401Gry_canny.bmp");
    delete [] pGradDir;
    delete [] pTempImg;
}

int main()
{
//    t5();
//    t6("H0402Gry",4, 116, 32);
    t7();
    return 0;
}

