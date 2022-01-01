#include "stats.h"

long getRed(RGBAPixel *pixel) {
    return pixel->r;
}

long getGreen(RGBAPixel *pixel) {
    return pixel->g;
}

long getBlue(RGBAPixel *pixel) {
    return pixel->b;
}

long square(long n) {
    return n * n;
}

void initializeSum(const PNG & im, vector< vector< long >>& v, long (*getColor)(RGBAPixel *pixel)) {
    
    long width = im.width();
    long height = im.height();

    long total = 0;
    for (long x = 0; x < width; x++) {
        total += getColor(im.getPixel(x,0));
        v[x][0] = total;
    }
    total = 0;
    for (long y = 0; y < height; y++) {
        total += getColor(im.getPixel(0,y));
        v[0][y] = total;
    }

    for (long x = 1; x < width; x++) {
        for (long y = 1; y < height; y++) {
            v[x][y] = v[x-1][y] + v[x][y-1] - v[x-1][y-1] + getColor(im.getPixel(x,y));
        }
    }
}

void initializeSumSq(const PNG & im, vector< vector< long >>& v, long (*getColor)(RGBAPixel *pixel)) {
    
    long width = im.width();
    long height = im.height();

    long total = 0;
    for (long x = 0; x < width; x++) {
        total += square(getColor(im.getPixel(x,0)));
        v[x][0] = total;
    }
    total = 0;
    for (long y = 0; y < height; y++) {
        total += square(getColor(im.getPixel(0,y)));
        v[0][y] = total;
    }

    for (long x = 1; x < width; x++) {
        for (long y = 1; y < height; y++) {
            v[x][y] = v[x-1][y] + v[x][y-1] - v[x-1][y-1] + square(getColor(im.getPixel(x,y)));
        }
    }
}



stats::stats(PNG & im){
    unsigned int height = im.height();
    unsigned int width = im.width();
    sumRed = vector<vector<long>> (width, vector<long>(height));
    sumGreen = vector<vector<long>> (width, vector<long>(height));
    sumBlue = vector<vector<long>> (width, vector<long>(height));
    sumsqRed = vector<vector<long>> (width, vector<long>(height));
    sumsqGreen = vector<vector<long>> (width, vector<long>(height));
    sumsqBlue = vector<vector<long>> (width, vector<long>(height));
    initializeSum(im, sumRed, getRed);
    initializeSum(im, sumGreen, getGreen);
    initializeSum(im, sumBlue, getBlue);
    initializeSumSq(im, sumsqRed, getRed);
    initializeSumSq(im, sumsqGreen, getGreen);
    initializeSumSq(im, sumsqBlue, getBlue);
    // for (auto c: sumRed) {
    //     for (auto r: c)
    //         cout << r << ", ";
    //     cout << endl;
    // }
    // cout << endl;
    // for (auto c: sumsqRed) {
    //     for (auto r: c)
    //         cout << r << ", ";
    //     cout << endl;
    // }
}


long stats::getSum(char channel, pair<int,int> ul, int w, int h){

    int x = ul.first - 1;
    int y = ul.second - 1;
    vector< vector< long >>* p;

    if (channel == 'r') p = &sumRed;
    else if (channel == 'g') p = &sumGreen;
    else p = &sumBlue;

    vector< vector< long >>& v = *p;

    if (w <= 0 || h <= 0) return 0;

    return v[x+w][y+h] + ((x < 0 || y < 0) ? 0 : v[x][y]) - (y < 0 ? 0 : v[x+w][y]) - (x < 0 ? 0 : v[x][y+h]);
}

long stats::getSumSq(char channel, pair<int,int> ul, int w, int h){
    int x = ul.first - 1;
    int y = ul.second - 1;
    vector< vector< long >>* p;

    if (channel == 'r') p = &sumsqRed;
    else if (channel == 'g') p = &sumsqGreen;
    else p = &sumsqBlue;

    vector< vector< long >>& v = *p;

    if (w <= 0 || h <= 0) return 0;

    return v[x+w][y+h] + ((x < 0 || y < 0) ? 0 : v[x][y]) - (y < 0 ? 0 : v[x+w][y]) - (x < 0 ? 0 : v[x][y+h]);
}

// given a rectangle, compute its sum of squared deviations from mean, over all color channels.
// see written specification for a description of this function.
double stats::getVar(pair<int,int> ul, int w, int h){
    if (w <= 0 || h <= 0) return 0;
    double area = (double) w * (double) h;
    return ((double) getSumSq('r', ul, w, h) - (double) square(getSum('r', ul, w, h)) / area)
         + ((double) getSumSq('g', ul, w, h) - (double) square(getSum('g', ul, w, h)) / area)
         + ((double) getSumSq('b', ul, w, h) - (double) square(getSum('b', ul, w, h)) / area);

}
		
RGBAPixel stats::getAvg(pair<int,int> ul, int w, int h){
    int area = w * h;
    return RGBAPixel(
        ((int) getSum('r', ul, w, h)) / area,
        ((int) getSum('g', ul, w, h)) / area,
        ((int) getSum('b', ul, w, h)) / area
    );
}
