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

//用于保存渲染中的一些状态
// struct Shader : IShader{
//     const Model &model;

//     Shader(const Model &model):model(model){} 

//     virtual Vec4f vertex(const int iface, const int nthvert){
//         return Vec4f();
//     }
// };

Vec3f       eye(0,1,3);
Vec3f    center(0,0,0);
Vec3f        up(0,1,0);

const int WIDTH = 800;
const int HEIGHT = 800;

const RGBQUAD COLOR_WHITE = {255,255,255,255};
const RGBQUAD COLOR_RED = {0,0,255,255};

//渲染上下文
struct RendererContext{
    vector<vector<float>> zBuffer;
    vector<Vec3f> pts; //原坐标
    Mat<4,4,float> vm; //
    
    vector<Vec3f> scPts; //屏幕坐标
    vector<Vec3f> orgPts;
    vector<Vec3f> txPts; //tx
    vector<Vec3f> vnPts; //tx
    Model *model;
    FIBITMAP *tx;
    int faceIdx;
    int vertexIdx;
    FIBITMAP *target;
    RGBQUAD pixelColor;
    Vec4f color;
    Mat4f viewMat;
    Mat4f projMat;
    Mat4f viewport;

    Vec3f light;
    Vec3f bc;

    void vertex(){

    }

    vector<Vec3f> getOriginPoints(){
        return model->getFace(faceIdx);
    }

    vector<Vec3f> getTexturePoints(){
        return model->getVt(faceIdx);
    }

    vector<Vec3f> getNorms(){
        return model->getVn(faceIdx);
    }

    vector<Vec3f> getScreenPoints(){
        vector<Vec3f> pts = model->getFace(faceIdx);
        for(int j=0;j<3;j++){
            // pts[j] = proj<3,4,float>( rotateY(-45.0f) * embed<4,3,float>(pts[j],1) ); 
            pts[j] = proj<3,4,float>( projMat * viewMat * embed<4,3,float>(pts[j],1) ); 
            
            pts[j].x = (pts[j].x + 1) *   WIDTH / 2;
            pts[j].y = (pts[j].y + 1) * WIDTH / 2;
        }
        return pts;
    }


};

RendererContext context;

void vertex(RendererContext &context){

}

void frag(RendererContext &context){

}

void init(){
    context.target = FreeImage_Allocate(WIDTH, HEIGHT, 32);
    for(int i=0;i<WIDTH;i++){
        for(int j=0;j<HEIGHT;j++){
            RGBQUAD color = {0,0,0,255};
            FreeImage_SetPixelColor(context.target,i,j,&color);
        }
    }
    context.zBuffer = vector<vector<float>>(WIDTH,vector<float>(HEIGHT,-FLT_MAX));
    context.light = Vec3f(1.0f,1.0f,1.0f).normalize();
    context.viewMat = lookat(eye,center,up);
    context.projMat = projection(-1.f/(eye-center).norm());
    context.viewport = viewport(WIDTH/8, HEIGHT/8, WIDTH*3/4, HEIGHT*3/4);
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


void drawPoint(float x, float y, RendererContext &context){
    int x1 = floor(x);
    int y1 = floor(y);
    if(x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT){

        FreeImage_SetPixelColor(context.target,x1,y1,&context.pixelColor);
    }
}

void drawLine(float x1, float y1, float x2, float y2, RendererContext &context){
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
            drawPoint(y,x,context);
        }else{
            drawPoint(x,y,context);
        }
    }
}

void drawTriangle(RendererContext &context){
    vector<Vec3f> pts = context.getScreenPoints();
    vector<Vec3f> po = context.getOriginPoints();
    vector<Vec3f> drawPts = context.getTexturePoints();
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
            if(bc.x < 0 || bc.y < 0 || bc.z < 0 || context.zBuffer[x][y] > d){
                continue;
            }
            context.zBuffer[x][y] = d;
            // 决定这一点的颜色
            float tx = bc.x * drawPts[0].x + bc.y * drawPts[1].x + bc.z * drawPts[2].x; 
            float ty = bc.x * drawPts[0].y + bc.y * drawPts[1].y + bc.z * drawPts[2].y;
            RGBQUAD color = context.model->getVtColor(tx, ty);
            Vec3f n;
            vector<Vec3f> vns = context.getNorms();
            for(int i=0;i<3;i++){
                n = n + vns[i] * bc[i];
            }
            n.normalize();
            float intensity = max(0.0f,context.light * n);

            if(intensity <= 0){
                continue;
            }
            // cout<<intensity<<endl;
            color.rgbRed = (int)color.rgbRed * intensity;
            color.rgbBlue = (int)color.rgbBlue * intensity;
            color.rgbGreen = (int)color.rgbGreen * intensity;
            context.pixelColor = color;
            // color = model.getVtColor(tx,ty);
            // color = model.getVtColor(drawPts[0].x,drawPts[0].y);
            // color = model.getVtColor(x,y);
            drawPoint(x,y,context);
        }
    }
}

void drawModel(string filename){
    context.model = new Model(filename);
    
    for(int i=0;i<context.model->nfaces;i++){
        // vector<Vec3f> pts = context.model->getFace(i);
        // context.orgPts = pts;
        // context.orgPts = context.model->getVt(i);
        // for(int j=0;j<3;j++){
        //     pts[j].x = (pts[j].x + 1) *   WIDTH / 2;
        //     pts[j].y = (pts[j].y + 1) * WIDTH / 2;
        // }
        // context.scPts = pts;

        // context.pixelColor = RGBQUAD{rand()%255,rand()%255,rand()%255,255};
        context.faceIdx = i;
        drawTriangle(context);

        // drawTriangle(pts,RGBQUAD{rand()%255,rand()%255,rand()%255,255});
    }
}

int main(int argc, char const *argv[])
{


    init();
    drawModel("obj/african_head.obj");
    FreeImage_Save(FIF_PNG,context.target,"test.png");
    system("Start test.png"); //打开图像
    system("pause");
    return 0;
}
