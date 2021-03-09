#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cfloat>
#include "Model.h"
#include "geometry.h"
#include "FreeImage.h"
#include "ourgl.h"
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 800 ;

//用于保存渲染中的一些状态
struct Shader : IShader{

    const Model &model;

    Shader(const Model &model):model(model){} 

    virtual Vec4f vertex(const int iface, const int nthvert){
        
    }
};

FIBITMAP *screen = NULL;
float ZBuffer[WIDTH][HEIGHT]; //只是记录，只要一一对应即可

RGBQUAD COLOR_WHITE = {255,255,255,255};
RGBQUAD COLOR_RED = {0,0,255,255};

vector<Vec3f> drawPts;

    Model model("obj/african_head.obj");

void init(){
    screen = FreeImage_Allocate(WIDTH, HEIGHT, 32);
    for(int i=0;i<WIDTH;i++){
        for(int j=0;j<HEIGHT;j++){
            RGBQUAD color = {0,0,0,255};
            FreeImage_SetPixelColor(screen,i,j,&color);
        }
    }
    for(int i=0;i<WIDTH;i++){
        for(int j=0;j<HEIGHT;j++){
            ZBuffer[i][j] = -FLT_MAX;
        }
    }
}

Vec3f barycentric(const vector<Vec3f> &pts, const Vec3f &p){
    Vec3f ab = pts[1] - pts[0];
    Vec3f ac = pts[2] - pts[0];
    Vec3f pa = pts[0] - p;
    float u = ac.x*pa.y - ac.y*pa.x;
    float v = -ab.x*pa.y + ab.y*pa.x;
    float t = ab.x*ac.y - ab.y*ac.x;
    if(t == 0){
        return Vec3f(-1,1,1);
    }
    return Vec3f(1.0f-(u+v)/t,u/t,v/t);
}


void drawPoint(float x, float y, RGBQUAD color){
    int x1 = floor(x);
    int y1 = floor(y);
    if(x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT){
        FreeImage_SetPixelColor(screen,x1,y1,&color);
    }
}

void drawLine(float x1, float y1, float x2, float y2, RGBQUAD color){
    bool steep = false;
    if(fabs(x2 - x1) < fabs(y2 - y1)){
        swap(x1,y1);
        swap(x2,y2);
        steep = true;
    }
    if(x1 > x2){
        swap(x1,x2);
        swap(y1,y2);
    }
    for(int x=x1;x<=x2;x++){
        float t = (x - x1) / (x2 - x1);
        float y = y1 + (y2 - y1) * t;
        if(steep){
            drawPoint(y,x,color);
        }else{
            drawPoint(x,y,color);
        }
    }
}

void drawTriangle(const vector<Vec3f> &pts, RGBQUAD color = COLOR_RED){
    float left = pts[0].x, right = pts[0].x, up = pts[0].y, down = pts[0].y;
    for(int i=1;i<3;i++){
        left = min(left,pts[i].x);
        right = max(right,pts[i].x);
        up = max(up,pts[i].y);
        down = min(down,pts[i].y);
    }
    left = max(left,(float)0);
    right = min(right,(float)WIDTH-1);
    down = max(down,(float)0);
    up = min(up,(float)HEIGHT-1);
    for(int x = left;x<=right;x++){
        for(int y=down;y<=up;y++){
            Vec3f bc = barycentric(pts,Vec3f(x,y,0));
            float d = bc.x * pts[0].z + bc.y * pts[1].z + bc.z * pts[2].z;
            if(bc.x < 0 || bc.y < 0 || bc.z < 0 || ZBuffer[x][y] > d){
                continue;
            }
            ZBuffer[x][y] = d;
            // 决定这一点的颜色
            float tx = bc.x * drawPts[0].x + bc.y * drawPts[1].x + bc.z * drawPts[2].x; 
            float ty = bc.x * drawPts[0].y + bc.y * drawPts[1].y + bc.z * drawPts[2].y;
            color = model.getVtColor(tx,ty);
            // color = model.getVtColor(drawPts[0].x,drawPts[0].y);
            // color = model.getVtColor(x,y);
            drawPoint(x,y,color);
        }
    }
}

void drawModel(){

}

int main(int argc, char const *argv[])
{

    init();
    // drawLine(13, 20, 80, 40, COLOR_WHITE); 
    // drawLine(20, 13, 40, 80, COLOR_RED); 
    // drawLine(80, 40, 13, 20, COLOR_RED);

    vector<Vec3f> p1= {{10,70,0},{50,160,0},{70,80,0}};
    vector<Vec3f> p2= {{180,50,0},{150,1,0},{70,180,0}};
    vector<Vec3f> p3= {{180,150,0},{120,160,0},{130,180,0}};

//     Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
// Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
// Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 

    // drawTriangle(p1);
    // drawTriangle(p2);
    // drawTriangle(p3);


    for(int i=0;i<model.nfaces;i++){
        auto pts = model.getFace(i);
        drawPts = model.getVt(i);
        for(int j=0;j<3;j++){
            pts[j].x = (pts[j].x + 1) *   WIDTH / 2;
            pts[j].y = (pts[j].y + 1) * WIDTH / 2;
        }
        drawTriangle(pts,RGBQUAD{rand()%255,rand()%255,rand()%255,255});
    }

    //测试图像是否正常
    // for(int i=0;i<WIDTH;i++){
    //     for(int j=0;j<HEIGHT;j++){
    //         drawPoint(i,j,model.getVtColor(i,j));
    //     }
    // }

    FreeImage_Save(FIF_PNG,screen,"test.png");
    system("Start test.png"); //打开图像
    system("pause");
    return 0;
}
