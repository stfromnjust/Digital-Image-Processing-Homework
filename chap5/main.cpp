//
// Created by 16337 on 2021/6/10 0010.
//
#include "bmpFile.h"

using namespace std;

void t2()
{
    int width, height;
    int pHistogram[256];
    BYTE *pImg = read8BitBmp2Img("../resource/H0501Gry.bmp", &width, &height);
    getHistogram(pImg, width, height, pHistogram);
    BYTE *pHistImg = new BYTE[256 * 100];
    drawHistogram2Img(pHistogram, 256, 1.0, pHistImg, 256, 100);
    write8BitImg2Bmp(pHistImg, 256, 100, "../output/H0501Gry_t2_Histogram.bmp");
    int threshold = getOtsuThreshold(pHistogram);
    printf("threshold: %d", threshold);
    thresholdBin(pImg, width, height, threshold);
    write8BitImg2Bmp(pImg, width, height, "../output/H0501Gry_t2_otsu.bmp");
}

void t3()
{
    int width, height;
    int pHistogram[256];
    BYTE *pImg = read8BitBmp2Img("../resource/H0501Gry.bmp", &width, &height);
    getHistogram(pImg, width, height, pHistogram);
    BYTE *pHistImg = new BYTE[256 * 100];
    drawHistogram2Img(pHistogram, 256, 1.0, pHistImg, 256, 100);
    write8BitImg2Bmp(pHistImg, 256, 100, "../output/H0501Gry_t3_Histogram.bmp");
    int pHistogramSmooth[256];
    histogramSmooth(pHistogram, 5, 256, pHistogramSmooth);
#ifdef MY_DEBUG
    for (int i = 0; i < 256; i++)
    {
        printf("%d ", pHistogram[i]);
    }
    printf("\n");
    for (int i = 0; i < 256; i++)
    {
        printf("%d ", pHistogramSmooth[i]);
    }
#endif
    drawHistogram2Img(pHistogramSmooth, 256, 1.0, pHistImg, 256, 100);
    write8BitImg2Bmp(pHistImg, 256, 100, "../output/H0501Gry_t3_HistogramSmooth.bmp");
    int threshold = getOtsuThreshold(pHistogram);
    int threshold1 = getOtsuThreshold(pHistogramSmooth);
    printf("\nthreshold: %d, threshold(smooth): %d\n", threshold, threshold1);
}

void t4()
{
    int width, height;
    int pHistogram[256];
    BYTE *pImg = read8BitBmp2Img("../resource/H0501Gry.bmp", &width, &height);
    getHistogram(pImg, width, height, pHistogram);
    BYTE *pHistImg = new BYTE[256 * 100];
    drawHistogram2Img(pHistogram, 256, 1.0, pHistImg, 256, 100);
    write8BitImg2Bmp(pHistImg, 256, 100, "../output/H0501Gry_t4_Histogram.bmp");
    int threshold = getOtsuThreshold(pHistogram, 256);
    reduceGrayscale(pImg, width, height, 2);
    getHistogram(pImg, width, height, pHistogram);
    drawHistogram2Img(pHistogram, 256, 1.0, pHistImg, 256, 100);
    write8BitImg2Bmp(pHistImg, 256, 100, "../output/H0501Gry_t4_HistogramReduce.bmp");
    int threshold1 = getOtsuThreshold(pHistogram, 256);
    printf("threshold: %d, threshold(reduce): %d\n", threshold, threshold1);
}

void t6_1() //  光照估计+图像减法+Otsu阈值法
{
//    1. 读取图像
    int width, height;
    int i;
    BYTE *pCur, *pBkg;
    int pHistogram[256];
    int thre;
    BYTE *pImg = read8BitBmp2Img("../resource/H0502Gry.bmp", &width, &height);
//    2. 高斯滤波, 获得背景光照图像
    BYTE *pBkgImg = read8BitBmp2Img("../resource/H0502Gry_bkg.bmp", &width, &height);
//    3. 消除光照
    for (i = 0, pCur = pImg, pBkg = pBkgImg; i < width * height; i++, pCur++, pBkg++)
    {
        *pCur = max(0, (*pCur - *pBkg));
    }
#ifdef MY_DEBUG
    write8BitImg2Bmp(pImg, width, height, "../output/H0502Gry_diff.bmp");
#endif
//    4. Otsu
    getHistogram(pImg, width, height, pHistogram);
    thre = getOtsuThreshold(pHistogram, 256);
    thresholdBin(pImg, width, height, thre);
//    5. 输出
    write8BitImg2Bmp(pImg, width, height, "../output/H0502Gry_t6_1.bmp");
}

void t6_2() //  多次分割法
{

}

void t6_3() //  聚类分割法
{
    int width, height;
    int pHistogram[256];
    int k, step, i, j, value, pre, mid;
    double center[10];  //  最多10类
    int LUT[256];

    BYTE *pImg = read8BitBmp2Img("../resource/H0502Gry.bmp", &width, &height);
    getHistogram(pImg, width, height, pHistogram);
    k = 3;  // 聚类数
    bool ret = clusterHistogram(pHistogram, k, 1.0, center);
    if (ret)
    {
        step = 255 / (k - 1);
        mid = (int)lround((center[0] + center[1]) / 2);
        for (i = 0; i < mid; i++)
        {
            LUT[i] = 0;
        }
        for (pre = mid, value = step, i = 1; i < k - 1; i++)
        {
            mid = (int)lround((center[i] + center[i + 1]) / 2);
            for (j = pre; j < mid; j++)
            {
                LUT[j] = value;
            }
            value += step;
            pre = mid;
        }
        for (i = mid; i < 256; i++)
        {
            LUT[i] = 255;
        }
        for (i = 0; i < width * height; i++)
        {
            pImg[i] = LUT[pImg[i]];
        }
        write8BitImg2Bmp(pImg, width, height, "../output/H0502Gry_t6_3.bmp");
    }
}

void t7()
{
    int width, height;
    int pHistogram[256];
    int k, step, i, j, value, pre, mid;
    double center[10];  //  最多10类
    int LUT[256];

    BYTE *pImg = read8BitBmp2Img("../resource/H0501Gry.bmp", &width, &height);
    getHistogram(pImg, width, height, pHistogram);
    k = 2;  // 聚类数
    bool ret = clusterHistogram(pHistogram, k, 1.0, center);
    if (ret)
    {
        step = 255 / (k - 1);
        mid = (int)lround((center[0] + center[1]) / 2);
        for (i = 0; i < mid; i++)
        {
            LUT[i] = 0;
        }
        for (pre = mid, value = step, i = 1; i < k - 1; i++)
        {
            mid = (int)lround((center[i] + center[i + 1]) / 2);
            for (j = pre; j < mid; j++)
            {
                LUT[j] = value;
            }
            value += step;
            pre = mid;
        }
        for (i = mid; i < 256; i++)
        {
            LUT[i] = 255;
        }
        for (i = 0; i < width * height; i++)
        {
            pImg[i] = LUT[pImg[i]];
        }
        write8BitImg2Bmp(pImg, width, height, "../output/H0501Gry_t7_2.bmp");
    }
}

int main()
{
//    t2();
//    t3();
    t4();
//    t6_1();
//    t6_3();
//    t7();
//    test();
}

