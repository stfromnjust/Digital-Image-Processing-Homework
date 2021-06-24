//
// Created by 16337 on 2021/6/21 0021.
//
#include "bmpFile.h"
#include <string>

using namespace std;

const int MAX_POINT_NUM = 512 * 512;
const double PI = acos(-1);

void t2_1()
{
    int width, height, N, maxRho;
    int bstTheta, bstRho;
    double A, B, C;
    BYTE *pImg = read8BitBmp2Img("../resource/H0602Bin.bmp", &width, &height);
    BYTE *pResImg = read8BitBmp2Img("../resource/H0602Bin.bmp", &width, &height);
    BYTE *pCountImg;
    int *xIdx, *yIdx;
    int *pCount;
    // 申请边缘点空间
    xIdx = new int[MAX_POINT_NUM];
    yIdx = new int[MAX_POINT_NUM];
    // 得到maxRho, 霍夫计数器空间大小
    maxRho = int(sqrt(1.0 * width * width + height * height)) + 1;
    // 申请霍夫计数器空间
    pCount = new int[(maxRho * 2 + 1) * 180]();
    pCountImg = new BYTE[(maxRho * 2 + 1) * 180]();
    for (int i = 0; i < 4; i++)
    {
        // 清空边缘点空间
        memset(xIdx, 0, sizeof(int) * MAX_POINT_NUM);
        memset(yIdx, 0, sizeof(int) * MAX_POINT_NUM);
        // 获取边缘点
        N = collectEdgePoints_H(pImg, width, height, xIdx, yIdx);
        // 计数
        getHoughCount_Line(xIdx, yIdx, N, maxRho, pCount);
#ifdef MY_DEBUG
        // 调试 输出计数器图片
        getCountImg(pCount, maxRho * 2 + 1, 180, pCountImg);
        write8BitImg2Bmp(pCountImg, maxRho * 2 + 1, 180, ("../output/t2_1/CountImg"+ to_string(i) + ".bmp").c_str());
#endif
        // 取霍夫计数器中最大值对应直线
        getBstHoughLine(pCount, maxRho * 2 + 1, 180, &bstTheta, &bstRho);
        A = cos(bstTheta * PI / 180);
        B = sin(bstTheta * PI / 180);
        C = -bstRho;
        // 画出直线
        drawABCLine(pResImg, width, height, A, B, C, 200 + i);
#ifdef MY_DEBUG
        write8BitImg2BmpMark(pResImg, width, height, ("../output/t2_1/H0602Bin_step" + to_string(i) + ".bmp").c_str());
#endif
        // 擦去该直线附近的点
        eraseABCLine(pImg, width, height, A, B, C, 255);
    }
    write8BitImg2BmpMark(pResImg, width, height, "../output/t2_1/H0602Bin_result.bmp");
    // 释放空间
    delete [] pCountImg;
    delete [] pCount;
    delete [] yIdx;
    delete [] xIdx;
}

void t2_2()
{
    int width, height, N, maxRho;
    int bstTheta, bstRho;
    double A, B, C;
    BYTE *pImg = read8BitBmp2Img("../resource/H0602Bin.bmp", &width, &height);
    BYTE *pCountImg;
    int *xIdx, *yIdx, *pCount;
    // 申请边缘点空间
    xIdx = new int[MAX_POINT_NUM]();
    yIdx = new int[MAX_POINT_NUM]();
    // 得到maxRho, 霍夫计数器空间大小
    maxRho = int(sqrt(1.0 * width * width + height * height)) + 1;
    // 申请霍夫计数器空间
    pCount = new int[(maxRho * 2 + 1) * 180]();
    pCountImg = new BYTE[(maxRho * 2 + 1) * 180]();
    // 获取边缘点
    N = collectEdgePoints_H(pImg, width, height, xIdx, yIdx);
    // 计数
    getHoughCount_Line(xIdx, yIdx, N, maxRho, pCount);
    for (int i = 0; i < 4; i++)
    {
#ifdef MY_DEBUG
        // 调试 输出计数器图片
        getCountImg(pCount, maxRho * 2 + 1, 180, pCountImg);
        write8BitImg2Bmp(pCountImg, maxRho * 2 + 1, 180, ("../output/t2_2/CountImg"+ to_string(i) + ".bmp").c_str());
#endif
        // 取霍夫计数器中最大值对应直线
        getBstHoughLine(pCount, maxRho * 2 + 1, 180, &bstTheta, &bstRho);
        A = cos(bstTheta * PI / 180);
        B = sin(bstTheta * PI / 180);
        C = -bstRho;
        // 画出直线
        drawABCLine(pImg, width, height, A, B, C, 200 + i);
        // 霍夫计数器周围清0
        eraseCountImg(pCount, maxRho * 2 + 1, 180, &bstTheta, &bstRho, 10);
        if (bstTheta <= 10)
        {
            bstTheta = 180 - bstTheta;
            bstRho = -bstRho;
            eraseCountImg(pCount, maxRho * 2 + 1, 180, &bstTheta, &bstRho, 10);
        }
#ifdef MY_DEBUG
        write8BitImg2BmpMark(pImg, width, height, ("../output/t2_2/H0602Bin_step" + to_string(i) + ".bmp").c_str());
#endif
    }
    write8BitImg2BmpMark(pImg, width, height, "../output/t2_2/H0602Bin_result.bmp");
    // 释放空间
    delete [] pCountImg;
    delete [] pCount;
    delete [] yIdx;
    delete [] xIdx;
}

void t2_3()
{
    int width, height, N, maxRho;
    int bstTheta, bstRho;

    double A, B, C;


}

void t3_1()
{
    int width, height;
    int circleX, circleY;
    BYTE *pImg = read8BitBmp2Img("../resource/H0603Bin.bmp", &width, &height);
    BYTE *pCountXImg, *pCountYImg;
    int *pCountX, *pCountY;
    // 获取X/Y直方图空间
    pCountX = new int[width]();
    pCountY = new int[height]();
    pCountXImg = new BYTE[width * 100];
    pCountYImg = new BYTE[height * 100];
    // 计数
    circleX = getCircleX(pImg, width, height, pCountX);
    circleY = getCircleY(pImg, width, height, pCountY);
#ifdef MY_DEBUG
    // 调试, 输出圆心位置, 获取直方图
    cout << "circleX: " << circleX << "\tcircleY: " << circleY << endl;
    getCountImg(pCountX, width, 1.0, pCountXImg, width, 100);
    getCountImg(pCountY, height, 1.0, pCountYImg, height, 100);
    write8BitImg2Bmp(pCountXImg, width, 100, "../output/t3_1/H0603Bin_CountXImg.bmp");
    write8BitImg2Bmp(pCountYImg, height, 100, "../output/t3_1/H0603Bin_CountYImg.bmp");
#endif

}

void gen_test_img()
{
    int width = 8, height = 8;
    int y, x;
    BYTE *pImg = new BYTE[width * height];
    // 全部设为白色
    memset(pImg, 0, width * height);
    int idx[26] = {9, 10, 11, 12, 13, 14,
                   17, 21, 22,
                   25, 29, 30,
                   33, 34, 35, 36, 37, 38,
                   41, 46,
                   49, 50, 51, 52, 53, 54};
    for (int i = 0; i < 26; i++)
    {
        pImg[idx[i]] = 255;
    }
    write8BitImg2Bmp(pImg, 8, 8, "../resource/Figtest_1.bmp");
}

void t4_test()
{
    int width, height, N;
    BYTE *pImg = read8BitBmp2Img("../resource/Figtest_1.bmp", &width, &height);
    BYTE *pChainCode = new BYTE[1000];
    // 图像边框设为0, 方便处理
    N = traceContourRmw(pImg, width, height, 1, 1, true, pChainCode, 1000);
    for (int i = 0; i < N; i++)
    {
        printf("->%d", pChainCode[i]);
    }
    drawContour(pImg, width, 1, 1, pChainCode, N, 200);
    write8BitImg2BmpMark(pImg, width, height, "../output/t4/Figtest_1.bmp");
}



int main(void)
{
    int repeatTimes = 1000;
    clock_t start = clock(), end;
    for (int i = 0; i < repeatTimes; i++)
    {
//        t2_1();   // Release: 4741ms
//        t2_2();     // Release: 2588ms
//        t3_1();
//        gen_test_img();
//        t4_test();
    }
    end = clock();
    printf("Time: %d ms\n", end - start);
    return 0;
}

