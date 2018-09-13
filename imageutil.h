#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include <QImage>
#include <cmath>
#include <iostream>

namespace ImageUtil {


void shiftUp(QImage &image) {
    uchar *data = image.bits();
    uchar *dest = image.bits();
    memmove(dest, data + image.bytesPerLine(), image.byteCount() - image.bytesPerLine());
}

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int clamp(int x, int upper, int lower) {
    return min(upper, max(x, lower));
}

void blur(uint *in, uint *out, int width, int height) {
    constexpr int radius = 3;

    int widthMinus1 = width - 1;
    constexpr int tableSize = 2 * radius + 1;
    uint divide[256 * tableSize];

    for(int i = 0; i < 256 * tableSize; i++)
        divide[i] = i / tableSize;

    int inIndex = 0;

    for(int y = 0; y < height; y++) {
        int outIndex = y;
        int ta = 0, tr = 0, tg = 0, tb = 0;

        for(int i = -radius; i <= radius; i++) {
            int rgb = in[inIndex + clamp(i, 0, width - 1)];
            ta += (rgb >> 24) & 0xff;
            tr += (rgb >> 16) & 0xff;
            tg += (rgb >> 8) & 0xff;
            tb += rgb & 0xff;
        }

        for(int x = 0; x < width; x++) {
            out[outIndex] = (divide[ta] << 24) | (divide[tr] << 16) | (divide[tg] << 8) | divide[tb];

            int i1 = x + radius + 1;
            if(i1 > widthMinus1)
                i1 = widthMinus1;
            int i2 = x - radius;
            if(i2 < 0)
                i2 = 0;
            int rgb1 = in[inIndex + i1];
            int rgb2 = in[inIndex + i2];

            ta += ((rgb1 >> 24) & 0xff) - ((rgb2 >> 24) & 0xff);
            tr += ((rgb1 & 0xff0000) - (rgb2 & 0xff0000)) >> 16;
            tg += ((rgb1 & 0xff00) - (rgb2 & 0xff00)) >> 8;
            tb += (rgb1 & 0xff) - (rgb2 & 0xff);
            outIndex += height;
        }
        inIndex += width;
    }
}

void blur2(uchar *in, uchar *out, int width, int height) {
    constexpr int radius = 3;
    constexpr int row = 10;


    for(int i = 0; i < width; i++) {

        for(int x = max(i - radius, 0); x < min(i + radius, width); x++) {

        }
    }

}

void boxBlurRow(QImage &image, int row) {
    QImage temp(image);

    std::cout << image.format() << std::endl;

    uint *src = (uint *) image.bits();
    uint *dst = (uint *) temp.bits();

    int w = image.bytesPerLine();
    //int h = image.height();


    blur(src, dst, image.width(), image.height());

    memcpy(src, dst, image.byteCount());

}

}

#endif // IMAGEUTIL_H
