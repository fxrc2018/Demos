#include <iostream>
#include <vector>
using namespace std;

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
