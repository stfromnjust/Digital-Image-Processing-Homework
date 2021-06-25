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
    int width, height, N = 0;
    int circleX, circleY, circleR;
    int maxR;
    BYTE *pImg = read8BitBmp2Img("../resource/H0604Bin.bmp", &width, &height);
    BYTE *pCountXImg, *pCountYImg, *pCountRImg;
    int *pCountX, *pCountY, *pCountR;
    int *xIdx, *yIdx;
    maxR = max(width, height);
    // 获取X/Y直方图空间, 边缘点空间
    pCountX = new int[width]();
    pCountY = new int[height]();
    pCountR = new int[maxR]();
    pCountXImg = new BYTE[width * 100]();
    pCountYImg = new BYTE[height * 100]();
    pCountRImg = new BYTE[maxR * 100]();
    xIdx = new int[MAX_POINT_NUM]();
    yIdx = new int[MAX_POINT_NUM]();
    // 计数
    circleX = getCircleX(pImg, width, height, xIdx, yIdx, &N, pCountX);
    circleY = getCircleY(pImg, width, height, xIdx, yIdx, &N, pCountY);
#ifdef MY_DEBUG
    // 调试, 输出圆心位置, 获取直方图
    cout << "circleX: " << circleX << "\tcircleY: " << circleY << endl;
    getCountImg(pCountX, width, 1.0, pCountXImg, width, 100);
    getCountImg(pCountY, height, 1.0, pCountYImg, height, 100);
    write8BitImg2Bmp(pCountXImg, width, 100, "../output/t3_1/H0604Bin_CountXImg.bmp");
    write8BitImg2Bmp(pCountYImg, height, 100, "../output/t3_1/H0604Bin_CountYImg.bmp");
#endif
    // 获取圆心
    circleR = getCircleR(xIdx, yIdx, N, circleY, circleY, maxR, pCountR);
#ifdef MY_DEBUG
    // 调试, 输出半径大小, 获取直方图
    cout << "circleR: " << circleR << endl;
    getCountImg(pCountR, maxR, 1.0, pCountRImg, maxR, 100);
    write8BitImg2Bmp(pCountRImg, maxR, 100, "../output/t3_1/H0604Bin_CountRImg.bmp");
#endif
    drawCircle(pImg, width, height, circleX, circleY, circleR, 200);
    write8BitImg2BmpMark(pImg, width, height, "../output/t3_1/H0604Bin_result.bmp");
    // 释放空间
    delete [] yIdx;
    delete [] xIdx;
    delete [] pCountRImg;
    delete [] pCountYImg;
    delete [] pCountXImg;
    delete [] pCountR;
    delete [] pCountY;
    delete [] pCountX;
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
    int y, x;
    int maxChainCodeNum = 512 * 512;
    BYTE *pImg = read8BitBmp2Img("../resource/H0605Bin.bmp", &width, &height);
    BYTE *pCur;
    BYTE *pChainCode = new BYTE[maxChainCodeNum];
    // 图像边框设为0, 方便处理
    setImgBoundary(pImg, width, height, 0);
    // 跟踪
    for (y = 1, pCur = pImg + y * width; y < height - 1; y++)
    {
        pCur++; // 跳过左部
        for (x = 1; x < width - 1; x++, pCur++)
        {
            if ((*pCur) == 255 && *(pCur - 1) < 2)  // 发现外轮廓
            {
                N = traceContourRmw(pImg, width, height, x, y, true, pChainCode, maxChainCodeNum);
#ifdef MY_DEBUG
                printf("\nouter: (y: %d, x: %d)", y, x);
                for (int i = 0; i < N; i++)
                {
                    printf("->%d", pChainCode[i]);
                }
#endif
                drawContour(pImg, width, x, y, pChainCode, N, 200);
            }
        }
        pCur++; // 跳过右部
    }
    write8BitImg2BmpMark(pImg, width, height, "../output/t4/H0605Bin.bmp");
    delete [] pChainCode;
}

void inverseImg(BYTE *pImg, int width, int height)
{
    BYTE *pCur, *pEnd = pImg + width * height;
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        *pCur = ~*pCur;
    }
}

void testInverseImg()
{
    int width, height;
    BYTE *pImg = read8BitBmp2Img("../resource/Fig6_23_ppt.bmp", &width, &height);
    inverseImg(pImg, width, height);
    write8BitImg2Bmp(pImg, width, height, "../resource/Fig_6_23_ppt.bmp");
}

int main()
{
    int repeatTimes = 1;
    clock_t start = clock(), end;
    for (int i = 0; i < repeatTimes; i++)
    {
//        t2_1();   // Release: 4741ms
//        t2_2();     // Release: 2588ms
        t3_1();
//        gen_test_img();
//        t4_test();
    }
    end = clock();
    printf("Time: %d ms\n", end - start);
    return 0;
}

