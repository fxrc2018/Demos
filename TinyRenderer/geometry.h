#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

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
};

template<int DIM, typename T>
Vec<DIM, T> operator-(const Vec<DIM, T> &v1, const Vec<DIM, T> &v2){
    Vec<DIM, T> difference;
    for(int i=0;i<DIM;i++){
        difference[i] = v1[i] - v2[i];
    }
    return difference;
} 


template<int DimCols, int DimRows, typename T>
class Mat{

};

// typedef Vec<float,2> Vec2f;
typedef Vec<3, int> Vec3i;
typedef Vec<3, float> Vec3f;
typedef Vec<4, float> Vec4f;

#endif