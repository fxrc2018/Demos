#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "glm/glm.hpp"
using namespace glm;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float PI = acos(-1.0f);

vec3 red(0.4f, 0.4f, 0.3f);
vec3 COLOR_BG(0.2f, 0.7f, 0.8f);


vec3 pixels[WIDTH * HEIGHT];
string filename = "output.ppm";

struct Ray{
    vec3 pos;
    vec3 dir;
};



struct Sphere{
    vec3 pos; 
    float r;

    Sphere(vec3 pos, float r):pos(pos),r(r){}

    //判断一条光线是否和自己相交
    bool isIntersect(Ray ray){
        vec3 oc = ray.pos - pos;
        float a = dot(ray.dir, ray.dir);
        float b = 2.0 * dot(ray.dir, oc);
        float c = dot(oc, oc) - r * r;
        float discriminant = b * b - 4 * a * c;
        return discriminant > 0;
    }
};

bool isIntersectScene(Ray ray){
    Sphere sp(vec3(-3, 0, -16),2);
    return sp.isIntersect(ray);
}

void render(){
    //产生光线
    //屏幕在(0,0,-1) 相机在(0,0,0) 相机看向的是-z轴 相机的fov=(h/2)/1 屏幕的宽高比为WITHD/HEIGHT
    for(int i=0;i<WIDTH;i++){
        for(int j=0;j<HEIGHT;j++){
            float y = -(2.0f* (j+0.5f)/(float)HEIGHT - 1); //归一化到[-1 1] 像素在中心位置，所以要加上0.5f
            float x = (2.0f* (i+0.5f)/(float)WIDTH - 1) * (float)WIDTH/HEIGHT; //归一化到[-WIDTH/HEIGHT,WIDTH/HEIGHT]
            float fov = PI/3.0f; //60度
            y *= tan(fov/2);
            x *= tan(fov/2);
            Ray ray;
            ray.pos = vec3(0.0f,0.0f,0.0f);
            ray.dir = normalize(vec3(x,y,-1.0f) - ray.pos);
            if(isIntersectScene(ray)){
                pixels[i+j*WIDTH] = red;
            }else{
                pixels[i+j*WIDTH] = COLOR_BG;
            }
        }
    }
}


void outputImage(){
    ofstream ofs;
    ofs.open(filename);
    ofs<<"P3\n"<<WIDTH<<" "<<HEIGHT<<"\n255\n";
    for(int i=0;i<WIDTH*HEIGHT;i++){
        vec3 v = pixels[i];
        v.x = clamp(v.x,0.0f,1.0f);
        v.y = clamp(v.y,0.0f,1.0f);
        v.z = clamp(v.z,0.0f,1.0f);
        ofs<<(int)(255 * v.x)<<" "<<(int)(255 * v.y) << " "<<(int)(255*v.z)<<"\n";
    }
    ofs.close();
}

int main(int argc, char const *argv[])
{
    render();
    outputImage();
    system("Start output.ppm");
    system("pause");
    return 0;
}
