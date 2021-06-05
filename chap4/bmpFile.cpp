//
// Created by 16337 on 2021/5/31 0031.
//
#include "bmpFile.h"

//#define Debug
#define PI 3.1415926

using namespace std;

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

void invertImg(BYTE *pImg, int width, int height)
{
    BYTE *pCur, *pEnd = pImg + width * height;
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        *pCur = ~*pCur;
    }
}

void gradientOp(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pRes;
    int x, y, dx, dy;
    for (y = 0, pCur = pImg, pRes = pResImg; y < height - 1; y++)
    {
        for (x = 0; x < width - 1; x++, pCur++, pRes++)
        {
            dx = *pCur - *(pCur + 1);
            dy = *pCur - *(pCur + width);
            *pRes = BYTE(min(255.0, sqrt(double(dx * dx + dy * dy))));
        }
        pCur++;
        *(pRes++) = 0;
    }
    memset(pRes, 0, width);
}

void robertOp(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pRes;
    int x, y, dx, dy;
    for (y = 0, pCur = pImg, pRes = pResImg; y < height - 1; y++)
    {
        for (x = 0; x < width - 1; x++, pCur++, pRes++)
        {
            dx = *pCur - *(pCur + 1 + width);
            dy = *(pCur + 1) - *(pCur + width);
            *pRes = max(abs(dx), abs(dy));
        }
        pCur++;
        *(pRes++) = 0;
    }
    memset(pRes, 0, width);
}

void sobelOp(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pRes;
    int x, y, dx, dy;
    memset(pResImg, 0, width);  // 首行不做
    for (y = 1, pCur = pImg + width, pRes = pResImg + width; y < height - 1; y++)
    {
        *(pRes++) = 0;  // 首列不做
        pCur++;
        for (x = 1; x < width - 1; x++, pCur++, pRes++)
        {
            dx = *(pCur - 1 - width) + 2 * *(pCur - 1) + *(pCur - 1 + width);
            dx -= *(pCur + 1 - width) + 2 * *(pCur + 1) + *(pCur + 1 + width);
            dy = *(pCur - 1 - width) + 2 * *(pCur - width) + *(pCur + 1 - width);
            dy -= *(pCur - 1 + width) + 2 * *(pCur + width) + *(pCur + 1 + width);
            *pRes = min(255, abs(dx) + abs(dy));
        }
        *(pRes++) = 0;  // 末列不做
        pCur++;
    }
    memset(pRes, 0, width); //  末行不做
}

void sobelOp(BYTE *pImg, int width, int height, int step, int *pGradDir, BYTE *pResImg)
{
//    step表示先跳几行几列
    BYTE *pCur, *pRes;
    int *pDir;
    int x, y, dx, dy;
    double dir;
    memset(pResImg, 0, width * (step + 1));  // 首行不做
    for (y = step + 1, pCur = pImg + width * y, pRes = pResImg + width * y, pDir = pGradDir + width * y;
         y < height - step - 1;
         y++)
    {
        // 首step+1列不做
        for (x = 0; x < step + 1; x++, pRes++, pCur++, pDir++)
        {
            *pRes = 0;
        }
        for (x = step + 1; x < width - 1 - step; x++, pCur++, pRes++, pDir++)
        {
            dx = *(pCur - 1 - width) + 2 * *(pCur - 1) + *(pCur - 1 + width);
            dx -= *(pCur + 1 - width) + 2 * *(pCur + 1) + *(pCur + 1 + width);
            dy = *(pCur - 1 - width) + 2 * *(pCur - width) + *(pCur + 1 - width);
            dy -= *(pCur - 1 + width) + 2 * *(pCur + width) + *(pCur + 1 + width);
            dir = atan2(dy, dx) * 180 / PI;
            if (abs(dir) <= 22.5 || abs(dir) >= 157.5)
            {
                *pDir = 1;  //  水平
            }
            else if ((dir >= 67.5 && dir <= 112.5) || (dir >= -112.5 && dir <= -67.5))
            {
                *pDir = 2;  //  竖直
            }
            else if ((dir >= 22.5 && dir <= 67.5) || (dir >= -157.5 && dir <= -112.5))
            {
                *pDir = 3;  //  45: "/"
            }
            else
            {
                *pDir = 4;  //  45: "\"
            }
//            printf("%.2f, pDir: %d\n", dir, *pDir);
            *pRes = min(255, abs(dx) + abs(dy));
        }
        for (x = 0; x < step + 1; x++, pRes++, pCur++, pDir++)
        {
            *pRes = 0;
        }
    }
    memset(pRes, 0, width * (step + 1)); //  末行不做
}

void prewittOp(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pRes;
    int x, y;
    int d90, d45, d0, d135;
    int v1, v2;
    int leftUp, up, rightUp, left, right, leftDown, down, rightDown;
    memset(pResImg, 0, width);  //  首行不做
    for (y = 1, pCur = pImg + width, pRes = pResImg + width; y < height - 1; y++)
    {
        *(pRes++) = 0;  // 首列不做
        pCur++;
        for (x = 1; x < width - 1; x++, pCur++, pRes++)
        {
            leftUp = *(pCur - 1 - width);
            up = *(pCur - width);
            rightUp = *(pCur + 1 - width);
            left = *(pCur - 1);
            right = *(pCur + 1);
            leftDown = *(pCur - 1 + width);
            down = *(pCur + width);
            rightDown = *(pCur + 1 + width);

            d90 = leftUp + up + rightUp;
            d90 -= leftDown + down + rightDown;

            d45 = up + rightUp + right;
            d45 -= left + leftDown + down;

            d0 = leftUp + left + leftDown;
            d0 -= rightUp + right + rightDown;

            d135 = leftUp + up + left;
            d135 -= right + down + rightDown;

            v1 = max(abs(d90), abs(d45));
            v2 = max(abs(d0), abs(d135));
            *pRes = min(255, max(v1, v2));
        }
        *(pRes++) = 0;  // 末列不做
        pCur++;
    }
    memset(pRes, 0, width); // 末行不做
}

void robinsonOp(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pRes;
    int x, y;
    int da, db, dc, dd, de, df, dg, dh;
    int v1, v2, v3, v4, v5, v6;
    int leftUp, up, rightUp, left, right, leftDown, down, rightDown;
    memset(pResImg, 0, width);  //  首行不做
    for (y = 1, pCur = pImg + width, pRes = pResImg + width; y < height - 1; y++)
    {
        *(pRes++) = 0;  // 首列不做
        pCur++;
        for (x = 1; x < width - 1; x++, pCur++, pRes++)
        {
            leftUp = *(pCur - 1 - width);
            up = *(pCur - width);
            rightUp = *(pCur + 1 - width);
            left = *(pCur - 1);
            right = *(pCur + 1);
            leftDown = *(pCur - 1 + width);
            down = *(pCur + width);
            rightDown = *(pCur + 1 + width);

            da = leftUp + up + rightUp + right + rightDown;
            da -= left + 2 * *(pCur) + leftDown + down;

            db = up + rightUp + right + down + rightDown;
            db -= leftUp + left + 2 * *(pCur) + leftDown;

            dc = rightUp + right + leftDown + down + rightDown;
            dc -= leftUp + up + left + 2 * *(pCur);

            dd = left + right + leftDown + down + rightDown;
            dd -= leftUp + up + rightUp + 2 * *(pCur);

            de = leftUp + left + leftDown + down + rightDown;
            de -= up + rightUp + 2 * *(pCur) + right;

            df = leftUp + up + left + leftDown + down;
            df -= rightUp + 2 * *(pCur) + right + rightDown;

            dg = leftUp + up + rightUp + left + leftDown;
            dg -= 2 * *(pCur) + right + down + rightDown;

            dh = leftUp + up + rightUp + left + right;
            dh -= 2 * *(pCur) + leftDown + down + rightDown;

            v1 = max(abs(da), abs(db));
            v2 = max(abs(dc), abs(dd));
            v3 = max(abs(de), abs(df));
            v4 = max(abs(dg), abs(dh));
            v5 = max(v1, v2);
            v6 = max(v3, v4);
            *pRes = min(255, max(v5, v6));
        }
        *(pRes++) = 0;  // 末列不做
        pCur++;
    }
    memset(pRes, 0, width); // 末行不做
}

void kirschOp(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pRes;
    int x, y;
    int da, db, dc, dd, de, df, dg, dh;
    int v1, v2, v3, v4, v5, v6;
    int leftUp, up, rightUp, left, right, leftDown, down, rightDown;
    memset(pResImg, 0, width);
    for (y = 1, pCur = pImg + width, pRes = pResImg + width; y < height - 1; y++)
    {
        *(pRes++) = 0;
        pCur++;
        for (x = 1; x < width - 1; x++, pCur++, pRes++)
        {
            leftUp = *(pCur - 1 - width);
            up = *(pCur - width);
            rightUp = *(pCur + 1 - width);
            left = *(pCur - 1);
            right = *(pCur + 1);
            leftDown = *(pCur - 1 + width);
            down = *(pCur + width);
            rightDown = *(pCur + 1 + width);
            da = 5 * (leftUp + up + rightUp);
            da -= 3 * (left + right + leftDown + down + rightDown);

            db = 5 * (up + rightUp + right);
            db -= 3 * (leftUp + left + leftDown + down + rightDown);

            dc = 5 * (rightUp + right + rightDown);
            dc -= 3 * (leftUp + up + left + leftDown + down);

            dd = 5 * (right + down + rightDown);
            dd -= 3 * (leftUp + up + rightUp + left + leftDown);

            de = 5 * (leftDown + down + rightDown);
            de -= 3 * (leftUp + up + rightUp + left + right);

            df = 5 * (left + leftDown + down);
            df -= 3 * (leftUp + up + rightUp + right + rightDown);

            dg = 5 * (leftUp + left + leftDown);
            dg -= 3 * (up + rightUp + right + down + rightDown);

            dh = 5 * (leftUp + up + left);
            dh -= 3 * (rightUp + right + leftDown + down + rightDown);

            v1 = max(abs(da), abs(db));
            v2 = max(abs(dc), abs(dd));
            v3 = max(abs(de), abs(df));
            v4 = max(abs(dg), abs(dh));
            v5 = max(v1, v2);
            v6 = max(v3, v4);
            *pRes = min(255, max(v5, v6));
        }
        *(pRes++) = 0;
        pCur++;
    }
}

void shenJunOp(BYTE *pImg, BYTE *pTempImg, int width, int height, double a0, BYTE *pResImg)
{
    BYTE *pCur, *pTempCur, *pRes, *pEnd;
    int LUT[512], *ALUT; //a0查找表
    int x, y, pre, dif;

    // step.1------------初始化查找表---------------------//
    a0 = min(max(0.01, a0), 0.99); //安全性检查
    //a0查找表,进行了四舍五入
    ALUT = LUT + 256;
    for (ALUT[0] = 0, dif = 1; dif < 256; dif++)
    {
        ALUT[dif] = (int) (dif * a0 + 0.5);
        ALUT[-dif] = (int) (-dif * a0 - 0.5);
    }
    // step.2------------递推实现指数滤波-----------------//
    //按行滤波
    for (y = 0, pCur = pImg, pTempCur = pTempImg; y < height; y++)
    {
        //1.从左向右: p1(y,x)=p1(y,x-1)+a*[p(y,x)-p1(y,x-1)]
        *(pTempCur++) = pre = *(pCur++);
        for (x = 1; x < width; x++, pCur++) *(pTempCur++) = pre = pre + ALUT[*pCur - pre];
        pTempCur--; //回到行尾
        //2.从右向左: p2(y,x)=p2(y,x+1)-a*[p1(y,x)-p2(y,x+1)]
        for (x = width - 2, pTempCur = pTempCur - 1; x >= 0; x--) *(pTempCur--) = pre = pre + ALUT[*pTempCur - pre];
        pTempCur += (width + 1); //回到下一行的开始
    }
    //按列滤波
    for (x = 0, pTempCur = pTempImg; x < width; x++, pTempCur = pTempImg + x)
    {
        //3.从上向下: p3(y,x)=p3(y-1,x)+a*[p2(y,x)-p3(y-1,x)]
        pre = *pTempCur;
        for (y = 1, pTempCur += width; y < height; y++, pTempCur += width)
            *pTempCur = pre = pre + ALUT[*pTempCur - pre];
        pTempCur -= width; //回到列尾
        //4.从下向上: p4(i,j)=p4(i+1,j)+a*[p3(i,j)-p4(i+1,j)]
        for (y = height - 2, pTempCur -= width; y >= 0; y--, pTempCur -= width)
            *pTempCur = pre = pre + ALUT[*pTempCur - pre];
    }
    // step.3------------正导数=1,负导数为0,0必须也是0----//
    pEnd = pTempImg + width * height;
    for (pTempCur = pTempImg, pCur = pImg; pTempCur < pEnd; pCur++)
    {
        *(pTempCur++) = (*pTempCur > *pCur);
    }
    // step.4------------过零点检测----------------------//
    memset(pResImg, 0, width * height); //边缘强度赋0
    pRes = pResImg + width;
    pTempCur = pTempImg + width; //首行不做
    for (y = 1; y < height - 1; y++)
    {
        pRes++;
        pTempCur++;  //首列不做
        for (x = 1; x < width - 1; x++, pCur++, pTempCur++, pRes++)
        {
            if (*pTempCur)  //正导数
            {   //下面使用4邻域,边缘为8连通,不能保证4连通;使用8邻域才能保证边缘4连通
                if ((!*(pTempCur - 1)) || //左,必须<=0,不能<0
                    (!*(pTempCur + 1)) || //右,必须<=0,不能<0
                    (!*(pTempCur - width)) || //上,必须<=0,不能<0
                    (!*(pTempCur + width)) //下,必须<=0,不能<0
                        )
                {
                    *pRes = 255; //周围有导数小于等于0
                }
            }
        }
        pRes++;
        pTempCur++;  //尾列不做
    }
}

void sj_sobelOp(BYTE *pImg, BYTE *pTempImg,
                int width, int height, double a0, int threshold,
                BYTE *pResImg)
{
    shenJunOp(pImg, pTempImg, width, height, a0, pResImg);
    sobelOp(pImg, width, height, pTempImg);
    for (int i = 0; i < width * height; i++)
    {
        *(pResImg + i) = (pResImg[i] && (pTempImg[i] > threshold)) * 255;
    }
}

void zoomOutImg(BYTE *pImg, int width, int height, BYTE *pResImg, int k, int flag = 1)
{
    // flag = 0高度缩放 flag = 1宽度缩放
    int x, y;
    BYTE *pCur, *pRes;
    if (flag == 0)
    {
        int newHeight = height / 4;
        for (y = 0, pRes = pResImg; y < newHeight; y++)
        {
            pCur = pImg + width * y * 4;
            for (x = 0; x < width; x++, pCur++, pRes++)
            {
                *pRes = *pCur;
            }
        }
    }
    else if (flag == 1)
    {
        int newWidth = width / 4;
        for (y = 0, pRes = pResImg; y < height; y++)
        {
            pCur = pImg + width * y;
            for (x = 0; x < newWidth; x++, pCur += 4, pRes++)
            {
                *pRes = *pCur;
            }
        }
    }
}

void get2DIntegral(BYTE *pImg, int width, int height, int *pIntegral)
{
    BYTE *pCur;
    int *pRes;
    int x, y;
    int sumCol[4096];
    memset(sumCol, 0, sizeof(int) * width);
    for (y = 0, pCur = pImg, pRes = pIntegral; y < height; y++)
    {
        sumCol[0] += *(pCur++);
        *(pRes++) = sumCol[0];
        for (x = 1; x < width; x++, pCur++, pRes++)
        {
            sumCol[x] += *(pCur);
            *pRes = *(pRes - 1) + sumCol[x];
        }
    }
}

void getTextPos(BYTE *pImg, int width, int height, int *pIntegral, int para1, int para2, int *resX, int *resY)
{
    int x, y;
    para1 = para1 / 2 * 2 + 1;
    int halfx = para2 + para1 / 2, halfy = halfx;
    BYTE *pCur;
    int *pCurIrg;
    int sum1, sum2, sum3, sum4;
    int curVal, maxVal = INT_MIN;
    int offset = para1 / 2;

    for (y = halfy + 1, pCur = pImg + y * width, pCurIrg = pIntegral + y * width; y < height - para1 / 2; y++)
    {
        pCur += halfx + 1;
        pCurIrg += halfx + 1;
        for (x = halfx + 1; x < width - halfx; x++, pCur++, pCurIrg++)
        {
            sum1 = *(pCurIrg + offset + offset * width) + *(pCurIrg - offset - 1 - (offset + 1) * width);
            sum1 -= *(pCurIrg - offset - 1 + offset * width) + *(pCurIrg + offset - (offset + 1) * width);

            sum2 = *(pCurIrg - offset - 1 + offset * width) +
                   *(pCurIrg - offset - 1 + offset * width - para2 - para1 * width);
            sum2 -= *(pCurIrg - offset - 1 + offset * width - para2) +
                    *(pCurIrg - offset - 1 + offset * width - para1 * width);

            sum3 = *(pCurIrg + offset - (offset + 1) * width) +
                   *(pCurIrg + offset - (offset + 1) * width - para1 - para2 * width);
            sum3 -= *(pCurIrg + offset - (offset + 1) * width - para1) +
                    *(pCurIrg + offset - (offset + 1) * width - para2 * width);

            sum4 = *(pCurIrg + offset + para2 + offset * width) + *(pCurIrg + offset - (offset + 1) * width);
            sum4 -= *(pCurIrg + offset - (offset + 1) * width + para1 * width) +
                    *(pCurIrg + offset - (offset + 1) * width + para2);
#ifdef Debug
            if (y == 106 && x == 25)
            {
                printf("sum1: %d, sum2: %d, sum3: %d, sum4: %d\n");
            }
#endif
            curVal = sum1 - sum2 - sum3 - sum4;
#ifdef Debug
            printf("(%d, %d), %d\n", y, x, curVal);
#endif
            if (curVal > maxVal)
            {
                maxVal = curVal;
                *resX = x;
                *resY = y;
            }
        }
        pCur += halfx;
        pCurIrg += halfx;
    }
    printf("maxVal: %d\n", maxVal);
}

void cannyOp(BYTE *pImg, int width, int height,
             double std, int *pGradDir,
             BYTE lowThreshold, BYTE highThreshold,
             BYTE *pTempImg)
{
    int step = lround(3 * std);
    gaussianFilter2D(pImg, width, height, std, pTempImg);
    memset(pImg, 0, width * height * sizeof(BYTE));
    sobelOp(pTempImg, width, height, step, pGradDir, pImg);
    nonMaximumSuppression(pImg, width, height, step, pGradDir);
    doubleThreshold(pImg, width, height, step, lowThreshold, highThreshold);
    doubleThresholdLink(pImg, width, height, step, lowThreshold, highThreshold);
}

void getGaussianFilter(double std, int *pGaussian)
{
    int halfSize = lround(3 * std);  // 上取整
    double val = 0.0;
    double sum = (1 / (sqrt(2 * PI) * std));
    double temp = sum;
    for (int i = 1; i <= halfSize; i++)
    {
        val = temp * exp((-(i * i)) / (2 * std * std));
        sum += 2 * val;
    }
    for (int i = 0; i <= halfSize; i++)
    {
        val = temp * exp((-(i * i)) / (2 * std * std));
        pGaussian[halfSize - i] = pGaussian[halfSize + i] = int(val / sum * 1024);
    }
}

void gaussianFilter1D(BYTE *pImg, int width, int height, int *pGaussian, int m, BYTE *pResImg)
{
    // 横向高斯滤波
    BYTE *pCur, *pRes;
    int *pGau = pGaussian;
    int halfx = m / 2;
    int x, y, x1, x2, i;
    for (y = 0, pCur = pImg, pRes = pResImg; y < height; y++, pCur += width)
    {
        pRes += halfx;
        for (x = halfx, x1 = 0, x2 = m; x < width - halfx; x++, x1++, x2++, pRes++)
        {
            pGau = pGaussian;
            for (i = x1; i < x2; i++, pGau++)
            {
                *pRes += (*(pCur + i) * (*pGau)) >> 10;
            }
        }
        pRes += halfx;
    }
}

void transposeImg(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pRes;
    int x, y;
    for (y = 0, pCur = pImg; y < height; y++)
    {
        pRes = pResImg + y;
        for (x = 0; x < width; x++, pCur++, pRes += height)
        {
            *pRes = *pCur;
        }
    }
}

void gaussianFilter2D(BYTE *pImg, int width, int height, double std, BYTE *pResImg)
{
    int gaussianSize = 2 * lround(3 * std) + 1;
    int *pGaussian = new int[gaussianSize];
    getGaussianFilter(std, pGaussian);
    gaussianFilter1D(pImg, width, height, pGaussian, gaussianSize, pResImg);
    transposeImg(pResImg, width, height, pImg);
    memset(pResImg, 0, width * height * sizeof(BYTE));
    gaussianFilter1D(pImg, height, width, pGaussian, gaussianSize, pResImg);
    transposeImg(pResImg, height, width, pImg);
    memcpy(pResImg, pImg, width * height);
}

void nonMaximumSuppression(BYTE *pImg, int width, int height, int step, int *pGradDir)
{
    BYTE *pCur;
    int *pDir;
    int x, y;
    for (y = step + 1, pCur = pImg + width * y, pDir = pGradDir + width * y;
         y < height - step - 1;
         y++)
    {
        // 首step+1列不做
        pCur += step + 1;
        pDir += step + 1;
        for (x = step + 1; x < width - 1 - step; x++, pCur++, pDir++)
        {
            if (*pDir == 1)    //  水平
            {
                if (!(*pCur > *(pCur - 1) && *pCur > *(pCur + 1)))
                {
                    *pCur = 0;
                }
            }
            else if (*pDir == 2)    //  垂直
            {
                if (!(*pCur > *(pCur - width) && *pCur > *(pCur + width)))
                {
                    *pCur = 0;
                }
            }
            else if (*pDir == 3)    //  45: "/"
            {
                if (!(*pCur > *(pCur + 1 - width) && *pCur > *(pCur - 1 + width)))
                {
                    *pCur = 0;
                }
            }
            else    //  45: "\"
            {
                if (!(*pCur > *(pCur - 1 - width) && *pCur > *(pCur + 1 + width)))
                {
                    *pCur = 0;
                }
            }
        }
        pCur += step + 1;
        pDir += step + 1;
    }
}

void doubleThreshold(BYTE *pImg, int width, int height, int step, BYTE lowThreshold, BYTE highThreshold)
{
    BYTE *pCur;
    int x, y;
    for (y = step + 1, pCur = pImg + width * y;
         y < height - step - 1;
         y++)
    {
        // 首step+1列不做
        pCur += step + 1;
        for (x = step + 1; x < width - 1 - step; x++, pCur++)
        {
            if (*pCur < lowThreshold)
            {
                *pCur = 0;
            }
            else if (*pCur > highThreshold)
            {
                *pCur = 255;
            }
        }
        pCur += step + 1;
    }
}

void doubleThresholdLink(BYTE *pImg, int width, int height, int step, BYTE lowThreshold, BYTE highThreshold)
{
    BYTE *pCur;
    int x, y;
    for (y = step + 1, pCur = pImg + width * y;
         y < height - step - 1;
         y++)
    {
        // 首step+1列不做
        pCur += step + 1;
        for (x = step + 1; x < width - 1 - step; x++, pCur++)
        {
            if (*pCur >= lowThreshold && *pCur < 255)
            {
                if (*(pCur - 1 - width) == 255 || *(pCur - width) == 255 || *(pCur + 1 - width) == 255 ||
                    *(pCur - 1) == 255 || *(pCur + 1) == 255 || *(pCur - 1 + width) == 255 ||
                    *(pCur + width) == 255 || *(pCur + 1 + width) == 255)
                {
                    *pCur = 255;
                    doubleThresholdLink(pImg, width, height, step, lowThreshold, highThreshold);
                }
                else
                {
                    *pCur = 0;
                }
            }
        }
        pCur += step + 1;
    }


}

