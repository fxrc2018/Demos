#include <iostream>
#include <vector>
using namespace std;

// 判断一个数是否在递增数组中存在
bool bs(vector<int> &a, int x){
    int l = 0;
    int r = a.size() - 1;
    while(l <= r){
        int mid = (l + r) >> 1;
        if(a[mid] < x){
            l = mid + 1;
        }else if(a[mid] > x){
            r = mid - 1;
        }else{
            return true;
        }
    }
    return false;
}

// 在单调递增数组中查找大于等于x的数中最小的一个
int bs1(vector<int> &a, int x){
    int l = 0;
    int r = a.size() - 1;
    while(l < r ){
        int mid = (l + r) >> 1;
        if(a[mid] >= x){
            r = mid;
        }else{
            l = mid + 1;
        }
    }
    return a[l];
}

// 在单调递增数组中查找小于等于x的数中最大的一个
int bs2(vector<int> &a, int x){
    int l = 0;
    int r = a.size() - 1;
    while(l < r ){
        int mid = (l + r + 1) >> 1;
        if(a[mid] <= x){
            l = mid;
        }else{
            r = mid - 1;
        }
    }
    return a[l];
}

// 求平方根 x >= 0
double squareRoot(double x){
    double l = 0;
    double r = x;
    while(r - l > 1e-8){
        double mid = (l + r) / 2;
        if(mid * mid > x){
            r = mid;
        }else{
            l = mid;
        }
    }
    return l;
}