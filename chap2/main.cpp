//
// Created by 16337 on 2021/5/21 0021.
//
#include "BmpTool.cpp"
#include <cstdio>
#include <malloc.h>
#include <time.h>

using namespace std;

void t2()
{
    BmpTool tool;
    tool.read24BitBmp2Img("../resource/H0201Rgb.bmp");
    tool.rgb2gry2();
    tool.write8BitImg2Bmp("../resource/H0201Gry.bmp");
}

void t3()
{
    BmpTool tool;
    tool.read8BitBmp2Img("../resource/H0201Gry.bmp");
    int *histogram = new int[256];
    double *bright = (double *) malloc(sizeof(double));
    double *contrast = (double *) malloc(sizeof(double));
    *bright = 0.0;
    *contrast = 1.0;
    tool.getHistogram(histogram);
    tool.getBrightContrast(histogram, bright, contrast);
    tool.meanVarRegular(*bright, *contrast, 50, 20);
    tool.write8BitImg2Bmp("../output/t3_1.bmp");
    tool.meanVarRegular(*bright, *contrast, 100, 30);
    tool.write8BitImg2Bmp("../output/t3_2.bmp");
    tool.meanVarRegular(*bright, *contrast, 150, 40);
    tool.write8BitImg2Bmp("../output/t3_3.bmp");
}

void t4()
{
    BmpTool tool;
    tool.read8BitBmp2Img("../resource/H0202Plane.bmp");
    tool.histogramEqualize();
    tool.write8BitImg2Bmp("../output/H0202Plane_Res.bmp");
}

void t5()
{
    BmpTool tool;
    tool.read24BitBmp2Img("../resource/H0201Rgb.bmp");
    tool.histogramEqualizeRgb();
    tool.write24BitImg2Bmp("../output/H0201Rgb_Res.bmp");
}

void t7()
{
    BmpTool tool;
    short int *p14Img = tool.read14bitRaw2Img("../resource/H0204IR14bit.raw", 640, 480);
    tool.histogramEqualize(p14Img);
    tool.write8BitImg2Bmp("../output/H0204IR8bit.bmp");
}

int main()
{
    // t2 计算时间
//    clock_t start = clock(), end;
//    for (int i = 0; i < 1000; i++)
//    {
//        t2();
//    }
//    end = clock();
//    double dur = (double)(end - start);
//    printf("Use Time:%fs\n",(dur/CLOCKS_PER_SEC));
//    t3();
//    t4();
//    t5();
//    t7();
    return 0;
}

