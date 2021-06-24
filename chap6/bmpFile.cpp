//
// Created by 16337 on 2021/6/21 0021.
//
#include "bmpFile.h"

const double PI = acos(-1);

BYTE *read8BitBmp2Img(const char *filename, int *width, int *height)
{   //BmpHeader记录的真实的宽度,
    //读出的是真实的宽度,去掉了4对齐的扩展.
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    BYTE *pImg;
    unsigned int size;
    int Suc = 1, w, h;
    int k, extend, tmp;

    // Open File
    *width = *height = 0;
    if ((fp = fopen(filename, "rb")) == nullptr) return nullptr;
    // Read Struct Info
    if (fread((void *) &FileHeader, 1, sizeof(FileHeader), fp) != sizeof(FileHeader)) Suc = -1;
    if (fread((void *) &BmpHeader, 1, sizeof(BmpHeader), fp) != sizeof(BmpHeader)) Suc = -1;
    if ((Suc == -1) ||
        (FileHeader.bfOffBits < sizeof(FileHeader) + sizeof(BmpHeader))
            )
    {
        fclose(fp);
        return nullptr;
    }
    // Read Image Data
    *width = w = BmpHeader.biWidth;
    *height = h = BmpHeader.biHeight;
    extend = (w + 3) / 4 * 4 - w;
    size = BmpHeader.biWidth * BmpHeader.biHeight;
    fseek(fp, (long) FileHeader.bfOffBits, SEEK_SET);
    if ((pImg = new BYTE[size]) != nullptr)
    {
        for (int i = 0; i < h; i++)  // 0,1,2,3,4(5): 400-499
        {
            if ((int) fread(pImg + (h - 1 - i) * w, sizeof(BYTE), w, fp) != w)
            {
                fclose(fp);
                delete pImg;
                pImg = nullptr;
                return nullptr;
            }
            //读掉扩充的数据
            for (k = 0; k < extend; k++)
            {
                if (fread(&tmp, 1, 1, fp) != 1)
                {
                    fclose(fp);
                    delete pImg;
                    pImg = nullptr;
                    return nullptr;

                }
            }
        }
    }
    fclose(fp);
    return pImg;
}

bool write8BitImg2Bmp(BYTE *pImg, int width, int height, const char *filename)
{   //BmpHeader记录的真实的宽度
    //当每行字节个数不是4的倍数时,需要进行4对齐
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    int i, extend;
    bool Suc = true;
    BYTE p[4], *pCur;

    // Open File
    if ((fp = fopen(filename, "w+b")) == nullptr)
    { return false; }
    // Fill the FileHeader
    FileHeader.bfType = ((WORD) ('M' << 8) | 'B');
    FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4L;
    FileHeader.bfSize = FileHeader.bfOffBits + width * height;
    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    if (fwrite((void *) &FileHeader, 1, sizeof(FileHeader), fp) != sizeof(FileHeader)) Suc = false;
    // Fill the ImgHeader
    BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
    BmpHeader.biHeight = height;
    BmpHeader.biPlanes = 1;
    BmpHeader.biBitCount = 8;
    BmpHeader.biCompression = 0;
    BmpHeader.biSizeImage = 0;
    BmpHeader.biXPelsPerMeter = 0;
    BmpHeader.biYPelsPerMeter = 0;
    BmpHeader.biClrUsed = 0;
    BmpHeader.biClrImportant = 0;
    if (fwrite((void *) &BmpHeader, 1, sizeof(BmpHeader), fp) != sizeof(BmpHeader)) Suc = false;
    // write Palette
    for (i = 0; i < 256; i++)
    {
        p[3] = 0;
        p[0] = p[1] = p[2] = i; //blue,green,red
        if (fwrite((void *) p, 1, 4, fp) != 4)
        {
            Suc = false;
            break;
        }
    }
    // write image data
    extend = (width + 3) / 4 * 4 - width; //每行字节个数需要按4的倍数对齐
    if (extend == 0)
    {
        for (pCur = pImg + (height - 1) * width; pCur >= pImg; pCur -= width)
        {
            if (fwrite((void *) pCur, 1, width, fp) != (unsigned int) width) Suc = false; //真实的数据
        }
    }
    else
    {
        for (pCur = pImg + (height - 1) * width; pCur >= pImg; pCur -= width)
        {
            if (fwrite((void *) pCur, 1, width, fp) != (unsigned int) width) Suc = false; //真实的数据
            for (i = 0; i < extend; i++) //每行按4字节对齐进行扩充
            {
                if (fwrite((void *) (pCur + width - 1), 1, 1, fp) != 1) Suc = false;
            }
        }
    }
    // return;
    fclose(fp);
    return Suc;
}

BYTE *read24BitBmp2Img(const char *filename, int *width, int *height)
{   //BmpHeader记录的真实的宽度,
    //读出的是真实的宽度,去掉了4对齐的扩展.
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    BYTE *pImg;
    unsigned int size;
    int Suc = 1, w, h, k, extend, tmp;

    // Open File
    *width = *height = 0;
    if ((fp = fopen(filename, "rb")) == nullptr) return nullptr;
    // Read Struct Info
    if (fread((void *) &FileHeader, 1, sizeof(FileHeader), fp) != sizeof(FileHeader)) Suc = -1;
    if (fread((void *) &BmpHeader, 1, sizeof(BmpHeader), fp) != sizeof(BmpHeader)) Suc = -1;
    if ((Suc == -1) ||
        (FileHeader.bfOffBits < sizeof(FileHeader) + sizeof(BmpHeader))
            )
    {
        fclose(fp);
        return nullptr;
    }
    // Read Image Data
    *width = w = BmpHeader.biWidth;
    *height = h = BmpHeader.biHeight;
    extend = ((w * 3 + 3) / 4 * 4) - w * 3; //每行字节个数需要按4的倍数对齐
    size = (*width) * (*height) * 3;
    fseek(fp, (long) FileHeader.bfOffBits, SEEK_SET); //bfOffBytes
    if ((pImg = new BYTE[size]) != nullptr)
    {
        for (int i = 0; i < h; i++)
        {
            if ((int) fread(pImg + (h - 1 - i) * w * 3, sizeof(BYTE), w * 3, fp) != w * 3)
            {
                fclose(fp);
                delete pImg;
                pImg = nullptr;
                return nullptr;
            }
            for (k = 0; k < extend; k++) //每行按4字节对齐进行扩充
            {
                if (fwrite((void *) &tmp, sizeof(BYTE), 1, fp) != 1) Suc = false;
            }
        }
    }
    fclose(fp);
    return pImg;
}

bool write24BitImg2Bmp(BYTE *pImg, int width, int height, const char *filename)
{   //BmpHeader记录的真实的宽度
    //当每行字节个数不是4的倍数时,需要进行4对齐
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    bool Suc = true;
    int i, extend;
    BYTE *pCur;
    // Open File
    if ((fp = fopen(filename, "w+b")) == nullptr)
    {
        return false;
    }
    // Fill the FileHeader
    FileHeader.bfType = ((WORD) ('M' << 8) | 'B');
    FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    FileHeader.bfSize = FileHeader.bfOffBits + width * height * 3L;
    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    if (fwrite((void *) &FileHeader, 1, sizeof(BITMAPFILEHEADER), fp) != sizeof(BITMAPFILEHEADER)) Suc = false;
    // Fill the ImgHeader
    BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
    BmpHeader.biHeight = height;
    BmpHeader.biPlanes = 1;
    BmpHeader.biBitCount = 24;
    BmpHeader.biCompression = 0;
    BmpHeader.biSizeImage = 0;
    BmpHeader.biXPelsPerMeter = 0;
    BmpHeader.biYPelsPerMeter = 0;
    BmpHeader.biClrUsed = 0;
    BmpHeader.biClrImportant = 0;
    if (fwrite((void *) &BmpHeader, 1, sizeof(BITMAPINFOHEADER), fp) != sizeof(BITMAPINFOHEADER)) Suc = false;
    // write image data
    extend = ((width * 3 + 3) / 4 * 4) - width * 3; //每行字节个数需要按4的倍数对齐
    if (extend == 0)
    {
        for (pCur = pImg + (height - 1) * 3 * width; pCur >= pImg; pCur -= 3 * width)
        {
            if (fwrite((void *) pCur, 1, width * 3, fp) != (unsigned int) (3 * width)) Suc = false; //真实的数据
        }
    }
    else
    {
        for (pCur = pImg + (height - 1) * 3 * width; pCur >= pImg; pCur -= 3 * width)
        {
            if (fwrite((void *) pCur, 1, width * 3, fp) != (unsigned int) (3 * width)) Suc = false; //真实的数据
            for (i = 0; i < extend; i++) //扩充的数据
            {
                if (fwrite((void *) (pCur + 3 * (width - 1) + 0), 1, 1, fp) != 1) Suc = false;
            }
        }
    }
    // return;
    fclose(fp);
    return Suc;
}

bool write8BitImg2BmpMark(BYTE *pImg, int width, int height, const char *filename)
{
    //BmpHeader记录的真实的宽度
    //当每行字节个数不是4的倍数时,需要进行4对齐
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    int i, extend;
    bool Suc = true;
    BYTE p[4], *pCur;

    // Open File
    if ((fp = fopen(filename, "w+b")) == NULL)
    { return false; }
    // Fill the FileHeader
    FileHeader.bfType = ((WORD) ('M' << 8) | 'B');
    FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4L;
    FileHeader.bfSize = FileHeader.bfOffBits + width * height;
    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    if (fwrite((void *) &FileHeader, 1, sizeof(FileHeader), fp) != sizeof(FileHeader)) Suc = false;
    // Fill the ImgHeader
    BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
    BmpHeader.biHeight = height;
    BmpHeader.biPlanes = 1;
    BmpHeader.biBitCount = 8;
    BmpHeader.biCompression = 0;
    BmpHeader.biSizeImage = 0;
    BmpHeader.biXPelsPerMeter = 0;
    BmpHeader.biYPelsPerMeter = 0;
    BmpHeader.biClrUsed = 0;
    BmpHeader.biClrImportant = 0;
    if (fwrite((void *) &BmpHeader, 1, sizeof(BmpHeader), fp) != sizeof(BmpHeader)) Suc = false;
    // write Palette
    for (i = 0, p[3] = 0; i < 256; i++)
    {
        p[3] = 0;
        if (i == 200)   // 红色
        {
            p[0] = 0;
            p[1] = 0;
            p[2] = 255;
        }
        else if (i == 201)  // 绿色
        {
            p[0] = 0;
            p[1] = 255;
            p[2] = 0;
        }
        else if (i == 202)  // 红色
        {
            p[0] = 255;
            p[1] = 0;
            p[2] = 0;
        }
        else if (i == 203)  // 绿色 + 蓝色
        {
            p[0] = 0;
            p[1] = 255;
            p[2] = 255;
        }
        else
        {
            p[0] = p[1] = p[2] = i; //blue,green,red
        }
        if (fwrite((void *) p, 1, 4, fp) != 4)
        {
            Suc = false;
            break;
        }
    }
    // write image data
    extend = (width + 3) / 4 * 4 - width; //每行字节个数需要按4的倍数对齐
    if (extend == 0)
    {
        for (pCur = pImg + (height - 1) * width; pCur >= pImg; pCur -= width)
        {
            if (fwrite((void *) pCur, 1, width, fp) != (unsigned int) width) Suc = false; //真实的数据
        }
    }
    else
    {
        for (pCur = pImg + (height - 1) * width; pCur >= pImg; pCur -= width)
        {
            if (fwrite((void *) pCur, 1, width, fp) != (unsigned int) width) Suc = false; //真实的数据
            for (i = 0; i < extend; i++) //每行按4字节对齐进行扩充
            {
                if (fwrite((void *) (pCur + width - 1), 1, 1, fp) != 1) Suc = false;
            }
        }
    }
    // return;
    fclose(fp);
    return Suc;
}

int collectEdgePoints_Fig(BYTE *pImg, int width, int height,
                          int *xIdx, int *yIdx)
{
    BYTE *pCur;
    int x, y;
    int cnt = 0;
    for (y = 0, pCur = pImg; y < height; y++)
    {
        for (x = 0; x < width; x++, pCur++)
        {
            if (*pCur)  // 白色
            {
                *pCur = 0;
                xIdx[cnt] = x;
                yIdx[cnt] = y;
                cnt++;
            }
            else
            {
                *pCur = 200;
            }
        }
    }
    return cnt;
}

int collectEdgePoints_H(BYTE *pImg, int width, int height,
                        int *xIdx, int *yIdx)
{
    int x, y, cnt = 0;
    BYTE *pRow, *pCol;
    // 左右
    for (y = 0, pRow = pImg; y < height; y++, pRow += width)
    {
        for (x = 0; x < width; x++)
        {
            if (!pRow[x]) // 黑色
            {
                xIdx[cnt] = x;
                yIdx[cnt] = y;
                cnt++;
                break;
            }
        }

        for (x = width - 1; x >= 0; x--)
        {
            if (!pRow[x]) // 黑色
            {
                xIdx[cnt] = x;
                yIdx[cnt] = y;
                cnt++;
                break;
            }
        }
    }
    // 上下
    for (x = 0, pCol = pImg; x < width; x++, pCol++)
    {
        for (y = 0; y < height; y++)
        {
            if (!pCol[width * y])
            {
                xIdx[cnt] = x;
                yIdx[cnt] = y;
                cnt++;
                break;
            }
        }
        for (y = height - 1; y >= 0; y--)
        {
            if (!pCol[width * y])
            {
                xIdx[cnt] = x;
                yIdx[cnt] = y;
                cnt++;
                break;
            }
        }
    }
    return cnt;
}

void drawABCLine(BYTE *pImg, int width, int height,
                 double A, double B, double C,
                 int color)
{
    // Ax + By + C = 0
    int x, y;
    if (fabs(B) > fabs(A))  // |k| < 1
    {
        for (x = 0; x < width; x++)
        {
            // y = (-Ax - C) / B
            y = lround(-(A * x + C) / B);
            if (y >= 0 && y < height)
            {
                *(pImg + y * width + x) = color;
            }
        }
    }
    else    //  |k| >= 1
    {
        for (y = 0; y < height; y++)
        {
            // x = (-By - C) / A
            x = lround(-(B * y + C) / A);
            if (x >= 0 && x < width)
            {
                *(pImg + y * width + x) = color;
            }
        }
    }
}

void eraseABCLine(BYTE *pImg, int width, int height,
                  double A, double B, double C,
                  int color)
{
    // Ax + By + C = 0
    int x, y;
    if (fabs(B) > fabs(A))  // |k| < 1
    {
        for (x = 0; x < width; x++)
        {
            // y = (-Ax - C) / B
            for (int i = -10; i <= 10; i++)
            {
                y = lround(-(A * x + C) / B) + i;
                if (y >= 0 && y < height)
                {
                    // 将周围的几个点全部置为color
                    *(pImg + y * width + x) = color;
                }
            }
        }
    }
    else    //  |k| >= 1
    {
        for (y = 0; y < height; y++)
        {
            // x = (-By - C) / A
            for (int i = -10; i <= 10; i++)
            {
                x = lround(-(B * y + C) / A) + i;
                if (x >= 0 && x < width)
                {
                    *(pImg + y * width + x) = color;
                }
            }
        }
    }
}

void getHoughCount_Line(int *x, int *y, int N,
                        int maxRho, int *pCount)
{
    memset(pCount, 0, sizeof(int) * (maxRho * 2 + 1) * 180);
    int theta, rho, sinVal, cosVal, i;
    int *pCenter;
    for (theta = 0; theta < 180; theta++)
    {
        cosVal = int(cos(theta * PI / 180) * 8192); // 2 ^ 13
        sinVal = int(sin(theta * PI / 180) * 8192); // 2 ^ 13
        pCenter = pCount + theta * (maxRho * 2 + 1) + maxRho;
        for (i = 0; i < N; i++)
        {
            rho = (x[i] * cosVal + y[i] * sinVal) >> 13;
            pCenter[rho]++;
        }
    }
}

void getHoughCount_Circle(int *x, int *y, int N,
                          int r,
                          int width, int height, int *pCount)
{
    int theta, cosVal, sinVal, i;
    memset(pCount, 0, sizeof(int) * width * height);
    for (theta = 0; theta < 360; theta++)
    {
        cosVal = int(cos(theta * PI / 180) * 2048);
        sinVal = int(sin(theta * PI / 180) * 2048);
        for (i = 0; i < N; i++)
        {

        }
    }
}

void getCountImg(int *pCount, int width, int height, BYTE *pImg)
{
    // get maxCount and minCount
    int maxCount = INT_MIN, minCount = INT_MAX, rho, theta;
    int *pCountCur, *pCountEnd = pCount + width * height;
    BYTE *pImgCur, *pImgEnd = pImg + width * height;
    for (pCountCur = pCount; pCountCur < pCountEnd; pCountCur++)
    {
        if (*pCountCur > maxCount)
        {
            maxCount = *pCountCur;
        }
        if (*pCountCur < minCount)
        {
            minCount = *pCountCur;
        }
    }
    // get CountImg
    // maxCount->255 minCount->0
    for (pCountCur = pCount, pImgCur = pImg; pCountCur < pCountEnd; pCountCur++, pImgCur++)
    {
        *pImgCur = (*pCountCur - minCount) * 255 / (maxCount - minCount);
    }
}

void getCountImg(int *pCount, int histSize,  // 总共多少项
                 double maxScale,   // 最大直方图所占图片比例
                 BYTE *pImg, int width, int height)
{
    int i, maxV;
    int y1, x1, x2, y, x;
    // 1. 初始化
    memset(pImg, 255, width * height);
    // 2. 最大值
    maxV = 0;
    for (i = 0; i < histSize; i++)
    {
        maxV = max(pCount[i], maxV);
    }
    maxV = int(maxV * maxScale);
    // 3. 像素填充
    for (i = 0; i < histSize; i++)
    {
        y1 = height - 1 - pCount[i] * (height - 1) / (maxV + 1);
        y1 = max(0, y1);
        x1 = i * width / histSize;
        x2 = (i + 1) * width / histSize;
        for (x = x1; x < x2; x++)
        {
            for (y = y1; y < height - 1; y++)
            {
                pImg[y * width + x] = 0;
            }
        }
    }
}

void getBstHoughLine(int *pCount, int width, int height,
                     int *bstTheta, int *bstRho)
{
    int theta, rho, maxRho = width / 2, maxCount = INT_MIN;
    int *pCountCur;
    for (theta = 0, pCountCur = pCount; theta < height; theta++)
    {
        for (rho = 0; rho < width; rho++, pCountCur++)
        {
            if (*pCountCur > maxCount)
            {
                maxCount = *pCountCur;
                *bstTheta = theta;
                *bstRho = rho - maxRho;
            }
        }
    }
}

void eraseCountImg(int *pCount, int width, int height,
                   int *eraseTheta, int *eraseRho, int radius)
{
    int startTheta, endTheta, startRho, endRho, maxRho = width / 2;
    int theta, rho;
    int *pCenter;
    if (*eraseTheta - radius < 0) startTheta = 0; else startTheta = *eraseTheta - radius;
    if (*eraseTheta + radius >= height) endTheta = height - 1; else endTheta = *eraseTheta + radius;
    if (*eraseRho - radius < -maxRho) startRho = -maxRho; else startRho = *eraseRho - radius;
    if (*eraseRho + radius > maxRho) endRho = maxRho; else endRho = *eraseRho + radius;
    for (theta = startTheta; theta <= endTheta; theta++)
    {
        pCenter = pCount + theta * (maxRho * 2 + 1) + maxRho;
        for (rho = startRho; rho <= endRho; rho++)
        {
            pCenter[rho] = 0;
        }
    }
}

int getCircleX(BYTE *pImg, int width, int height, int *pCount)
{
    int x, y, x1 = 0, x2 = width - 1, maxCount = 0, bstX = 0;
    BYTE *pRow;
    memset(pCount, 0, sizeof(int) * width);
    for (y = 0, pRow = pImg; y < height; y++, pRow += width)
    {
        for (x = 0; x < width; x++)
        {
            if (!pRow[x])
            {
                x1 = x;
                break;
            }
        }
        for (x = width - 1; x >= 0; x--)
        {
            if (!pRow[x])
            {
                x2 = x;
                break;
            }
        }
        pCount[(x1 + x2) / 2]++;
    }
    for (x = 0; x < width; x++)
    {
        if (pCount[x] > maxCount)
        {
            maxCount = pCount[x];
            bstX = x;
        }
    }
    return bstX;
}

int getCircleY(BYTE *pImg, int width, int height, int *pCount)
{
    int x, y, y1 = 0, y2 = height - 1, maxCount = 0, bstY = 0;
    BYTE *pCol;
    memset(pCount, 0, sizeof(int) * height);
    for (x = 0, pCol = pImg; x < width; x++, pCol++)
    {
        for (y = 0; y < height; y++)
        {
            if (!pCol[width * y])
            {
                y1 = y;
                break;
            }
        }
        for (y = height - 1; y >= 0; y--)
        {
            if (!pCol[width * y])
            {
                y2 = y;
                break;
            }
        }
        pCount[(y1 + y2) / 2]++;
    }
    for (y = 0; y < height; y++)
    {
        if (pCount[y] > maxCount)
        {
            maxCount = pCount[y];
            bstY = y;
        }
    }
    return bstY;
}

int traceContourRmw(BYTE *pImg, int width, int height,  // 二值图像
                    int x0, int y0, // 轮廓起点
                    bool isOuter,   //  是否是外轮廓
                    BYTE *pChainCode,   //  链码序列
                    int maxChainCodeNum    // 最长序列
)
{
    static int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    static int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};
    static int initCode[8] = {7, 7, 1, 1, 3, 3, 5, 5};
    int offset[8];
    int i = 0, N = 0;
    // 开始链码
    int code, beginCode, prevCode, returnCode;
    BYTE *pCur, *pBegin, *pCheck;
    // 不同链码对应的偏移量
    for (i = 0; i < 8; i++)
    {
        offset[i] = dy[i] * width + dx[i];
    }
    pCur = pBegin = pImg + width * y0 + x0;
    // 黑色0: 背景, 白色255: 轮廓
    if (isOuter) // 外轮廓
    {
        *(pCur - 1) = 1;
        beginCode = 7;
    }
    else         // 内轮廓
    {
        *(pCur + 1) = 1;
        beginCode = 3;
    }
    prevCode = beginCode;
    do
    {
        // 将轮廓设为254
        *pCur = 254;
        // 这里遍历7个方向
        // code 当前尝试方向  prevCode 上一步方向
        for (i = 0, code = initCode[prevCode]; i < 7; i++, code = (code + 1) % 8)
        {
            pCheck = pCur + offset[code];
            if (*pCheck < 2)    // 是背景
            {
                *pCheck = 1;
            }
            else    // 是轮廓
            {
                if (N < maxChainCodeNum)   // 记录方向
                {
                    pChainCode[N] = code;
                    N++;
                }
                if (pCheck == pBegin)   // 回到开始点?
                {
                    returnCode = (code + 4) % 8;
                    for (i = 0, code = initCode[beginCode]; i < 7; i++, code = (code + 1) % 8)
                    {
                        if (code == returnCode) break;
                    }
                    // 遍历没有用完的方向
                    for (i = i + 1, code = (code + 1) % 8; i < 7; i++, code = (code + 1) % 8)
                    {
                        pCheck = pBegin + offset[code];
                        if (*pCheck < 2)    // 是背景
                        {
                            *pCheck = 1;
                        }
                        else    // 是轮廓
                        {
                            if (N < maxChainCodeNum)   // 记录方向
                            {
                                pChainCode[N] = code;
                                N++;
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }
        pCur = pCheck;
        prevCode = code;
    } while (i < 7);
    return N;
}

void drawContour(BYTE *pImg, int width,
                 int x0, int y0,    // 轮廓起点
                 BYTE *pChainCode, int N,    //  链码序列, 链码长度
                 int color)
{
    static int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    static int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};
    int offset[8];
    int i;
    BYTE *pCur = pImg + y0 * width + x0;
    for (i = 0; i < 8; i++)
    {
        offset[i] = dy[i] * width + dx[i];
    }
    for (i = 0; i < N; i++)
    {
        *pCur = color;
        pCur += offset[pChainCode[i]];
    }
}