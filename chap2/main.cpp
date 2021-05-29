//
// Created by 16337 on 2021/5/21 0021.
//
#include "BmpTool.cpp"
#include <cstdio>
#include <ctime>

using namespace std;

void t2()
{
    BmpTool tool;
    tool.read24BitBmp2Img("../resource/H0201Rgb.bmp");
    BYTE *pResImg = new BYTE[tool.getWidth() * tool.getHeight()];
    tool.rgb2gry2(pResImg);    // 使用查找表
//    tool.rgb2gry1(pResImg);  // 不使用查找表
    tool.write8BitImg2Bmp("../resource/H0201Gry.bmp");
    delete [] pResImg
}

void t3(double trgBright, double trgContrast)
{
    BmpTool tool;
    tool.read8BitBmp2Img("../resource/H0201Gry.bmp");
    auto *histogram = new int[256];
    auto *bright = new double;
    auto *contrast = new double;
    *bright = 0.0;
    *contrast = 1.0;
    tool.getHistogram(histogram);
    tool.getBrightContrast(histogram, bright, contrast);
    tool.meanVarRegular(*bright, *contrast, trgBright, trgContrast);
    tool.write8BitImg2Bmp("../output/t3.bmp");
    printf("t3.bmp:\nbright: %.2f\ncontrast: %.2f\n", trgBright, trgContrast);
    delete [] histogram;
    delete bright;
    delete contrast;
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
    BYTE *pResImg = new BYTE[tool.getWidth() * tool.getHeight()];
    tool.histogramEqualize(p14Img, pResImg);
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
//      t3(150, 40);
//    t4();
//    t5();
    t7();
    return 0;
}

