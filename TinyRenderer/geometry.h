#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <cmath>
#include <cassert>


template<int DIM, typename T>
struct Vec{
    T data[DIM];
    Vec(){
        for(int i=0;i<DIM;i++){
            data[i] = T();
        }
    }
    T& operator[](int i){
        return data[i];
    }
    const T& operator[](int i) const{
        return data[i];
    }
};

template<typename T>
struct Vec<3,T>{
    T x,y,z;
    Vec():x(T()),y(T()),z(T()){}
    Vec(T x, T y, T z):x(x),y(y),z(z){}
    T& operator[](int i){
        if(i == 0){
            return x;
        }else if(i == 1){
            return y;
        }else{
            return z;
        }
    }

    const T& operator[](int i) const{
        if(i == 0){
            return x;
        }else if(i == 1){
            return y;
        }else{
            return z;
        }
    }

    float norm() { 
        return std::sqrt(x*x+y*y+z*z); 
    }

    Vec<3,T> & normalize(T l=1) { 
        *this = (*this)*(l/norm()); 
        return *this; 
    }
};

//对向量进行扩充
template<int LEN, int DIM, typename T>
Vec<LEN, T> embed(const Vec<DIM,T> &v, T fill=1){
    Vec<LEN,T> ret;
    for(int i=0;i<DIM;i++){
        ret[i] = v[i];
    }
    for(int i=DIM;i<LEN;i++){
        ret[i] = fill;
    }
    return ret;
}

//对向量进行截断
template<int LEN, int DIM, typename T>
Vec<LEN, T> proj(const Vec<DIM,T> &v){
    Vec<LEN,T> ret;
    for(int i=0;i<LEN;i++){
        ret[i] = v[i];
    }
    return ret;
}

//两个向量相加等于各个分量相加
template<int DIM, typename T>
Vec<DIM, T> operator+(const Vec<DIM, T> &v1, const Vec<DIM, T> &v2){
    Vec<DIM, T> difference;
    for(int i=0;i<DIM;i++){
        difference[i] = v1[i] + v2[i];
    }
    return difference;
}

//两个向量相减等于各个分量相减
template<int DIM, typename T>
Vec<DIM, T> operator-(const Vec<DIM, T> &v1, const Vec<DIM, T> &v2){
    Vec<DIM, T> difference;
    for(int i=0;i<DIM;i++){
        difference[i] = v1[i] - v2[i];
    }
    return difference;
} 

//向量乘以一个常数
template<int DIM, typename T>
Vec<DIM,T> operator*(const Vec<DIM,T> &v, T t){
    Vec<DIM,T> ret;
    for(int i=0;i<DIM;i++){
        ret[i] = v[i] * t;
    }
    return ret;
}

//向量点乘
template<int DIM,typename T>
T operator*(const Vec<DIM,T> &v1, const Vec<DIM, T> &v2){
    T ret = T(); //写T ret()会报错
    for(int i=0;i<DIM;i++){
        ret += v1[i] * v2[i];
    }
    return ret;
}

//向量叉乘，只在三维向量上有定义
template<typename T>
Vec<3,T> operator^(const Vec<3,T> &v1, const Vec<3, T> &v2){
    return Vec<3,T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

template<int DimRows, int DimCols, typename T>
class Mat{
    Vec<DimCols,T> rows[DimRows];
public:
    Mat(){}

    Vec<DimCols,T>& operator[](const int idx){
        assert(idx < DimRows);
        return rows[idx];
    }

    const Vec<DimCols, T>&operator[](const int idx) const{
        assert(idx < DimRows);
        return rows[idx];
    }

    //得到一列
    Vec<DimRows, T> col(const int idx) const{
        assert(idx < DimCols);
        Vec<DimRows,T> ret;
        for(int i=0;i<DimCols;i++){
            ret[i] = rows[i][idx];
        }
        return ret;
    }

    //设置一列
    void setCol(int idx, const Vec<DimRows,T> &v){
        assert(idx < DimCols);
        for(int i=0;i<DimRows;i++){
            rows[i][idx] = v[i];
        }
    }

    static Mat<DimRows,DimCols,T> identity() {
        Mat<DimRows,DimCols,T> ret;
        for(int i=0;i<DimRows;i++){
            for(int j=0;j<DimCols;j++){
                ret[i][j] = i == j?1:0;
            }
        }
        return ret;
    }


};


// template<size_t DimRows,size_t DimCols,typename T> vec<DimRows,T> operator*(const mat<DimRows,DimCols,T>& lhs, const vec<DimCols,T>& rhs) {
//     vec<DimRows,T> ret;
//     for (size_t i=DimRows; i--; ret[i]=lhs[i]*rhs);
//     return ret;
// }

//矩阵乘以一个向量
template<int DimRows, int DimCols, typename T>
Vec<DimRows,T> operator*(const Mat<DimRows, DimCols,T> &mat, const Vec<DimCols, T> &vec){
    Vec<DimRows, T> ret;
    for(int i=0;i<DimRows;i++){
        ret[i] = mat[i] * vec;
    }
    return ret;
}

//矩阵乘法
template<int R1, int C1, int C2, typename T>
Mat<R1,C2,T> operator*(const Mat<R1,C1, T> &m1, const Mat<C1,C2,T> &m2){
    Mat<R1,C2,T> ret;
    for(int i=0;i<R1;i++){
        for(int j=0;j<C2;j++){
            ret[i][j] = m1[i] * m2.col(j);
        }
    }
    return ret;
}

// typedef Vec<float,2> Vec2f;
typedef Vec<3, int> Vec3i;
typedef Vec<3, float> Vec3f;
typedef Vec<4, float> Vec4f;
typedef Mat<4,4,float> Mat4f;

Mat4f rotateY(float angle);
Mat4f lookat(Vec3f eye, Vec3f center, Vec3f up);
Mat4f projection(float coeff);
Mat4f viewport(int x, int y, int w, int h);

#endif