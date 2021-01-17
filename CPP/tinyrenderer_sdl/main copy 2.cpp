#include <iostream>
#include <algorithm>
#include <climits>
#include <cfloat>
extern "C"{
    #include <SDL2/SDL.h>
}
#include "geometry.h"
#include "model.h"
using namespace std;

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
Model *model = NULL;
int width = 800;
int height = 800;
float *zbuffer = NULL;
Uint32 black, white, red, green;

void init(){
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SDL", 100, 100, width, height, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
    black = SDL_MapRGBA(surface->format, 0, 0, 0, 0xff);
    white = SDL_MapRGBA(surface->format, 0xff, 0xff, 0xff, 0xff);
    red = SDL_MapRGBA(surface->format, 0xff, 0, 0, 0xff);
    green = SDL_MapRGBA(surface->format, 0, 0xff, 0, 0xff);
    zbuffer = (float*)malloc(width * height * sizeof(float));
}

void drawPixel(int x, int y, Uint32 color){
    Uint32 *pixels = (Uint32*)surface->pixels;
    int pos = x + surface->w * y;
    // 要检查是否越界
    if(pos >=0 && pos < surface->w * surface->h){
        pixels[x + surface->w * y] = color;
    }
}

void flipY(){
    Uint32 *pixels = (Uint32*)surface->pixels;
    for(int y =0;y<surface->h /2;y++){
        for(int x =0; x<surface->w; x++){
            swap(pixels[x + surface->w * y], pixels[x + surface->w * (surface->h - 1 - y)]);
        }
    }
}

void drawLine(int x1, int y1, int x2, int y2, Uint32 color){
    bool steep = false;
    if(abs(x2 - x1) < abs(y2 - y1)){
        steep = true; //交换x和y的含义 保证从相差较大的坐标开始遍历
        swap(x1,y1); 
        swap(x2,y2);
    }
    if(x2 < x1){ //从小到大
        swap(x1,x2);
        swap(y1,y2);
    }
    for(int x=x1;x<x2;x++){
        float t = (x - x1) / (float)(x2 - x1);
        int y = y1 + (y2 - y1) * t;
        if(steep){
            drawPixel(y,x,color); //x y交换过
        }else{
            drawPixel(x,y,color);
        }
    }
}

Vec3f cross(const Vec3f &v1, const Vec3f &v2) {
    return Vec3f{v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}

Vec3f barycentric(Vec3f *pts, Vec3f P) { 
    Vec3f AB = pts[1] - pts[0];
    Vec3f AC = pts[2] - pts[0];
    Vec3f PA = pts[0] - P;
    int u = AC.x*PA.y - AC.y*PA.x;
    int v = -AB.x*PA.y + AB.y*PA.x;
    int t = AB.x*AC.y - AB.y*AC.x;
    if(std::abs(t) > 1e-2){
        return Vec3f(1.0f-(u+v)/(float)t,u/(float)t,v/(float)t);
    }
    return Vec3f(-1,1,1);
} 

void drawTriangle(Vec3f *pts, Uint32 color){
    Vec2f bboxmin(INT_MAX, INT_MAX);
    Vec2f bboxmax(INT_MIN, INT_MIN);
    for (int i=0; i<3; i++){ //找到上下左右的边界
        bboxmin.x = min(bboxmin.x,pts[i].x);
        bboxmin.y = min(bboxmin.y,pts[i].y);
        bboxmax.x = max(bboxmax.x,pts[i].x);
        bboxmax.y = max(bboxmax.y,pts[i].y);
    }
    //将边界修正在屏幕内
    bboxmax.x = min(bboxmax.x,(float)surface->w-1);
    bboxmax.y = min(bboxmax.y,(float)surface->h-1);
    bboxmin.x = max(bboxmin.x,0.0f);
    bboxmin.y = max(bboxmin.y,0.0f);
    Vec3f p;
    for(p.x = bboxmin.x;p.x<=bboxmax.x;p.x++){
        for(p.y=bboxmin.y;p.y<bboxmax.y;p.y++){
            Vec3f bc = barycentric(pts,p);
            if(bc.x <0||bc.y < 0||bc.z<0){
                continue;
            } 
            //对z轴进行差值
            p.z = bc.x * pts[0].z + bc.y * pts[1].z + bc.z * pts[2].z;
            // cout<<bc<<" "<<zbuffer[(int)(p.x + p.y*surface->w)]<<" "<<p.z<<endl;
            if(zbuffer[(int)(p.x + p.y*surface->w)] < p.z){
                zbuffer[(int)(p.x + p.y*surface->w)] = p.z;
                drawPixel(p.x,p.y,color);
            }
        }
    }
}

Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}

void drawModel(Model *m){
    for (int i=0; i<m->nfaces(); i++) { 
        std::vector<int> face = m->face(i); 
        Vec3f screen_coords[3]; 
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) { 
            world_coords[j] = m->vert(face[j]); 
            // screen_coords[j] = Vec2i((world_coords.x+1.)*surface->w/2., (world_coords.y+1.)*surface->h/2.); 
            screen_coords[j] = world2screen(world_coords[j]);
        } 
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
        n.normalize(); 
        Vec3f light_dir(0,0,-1);
        float intensity = n*light_dir; 
        if (intensity>0) { 
            drawTriangle(screen_coords,SDL_MapRGBA(surface->format,intensity * 255, intensity *255, intensity *255, 255));
        } 
        // drawTriangle(screen_coords, SDL_MapRGBA(surface->format,rand()%255, rand()%255, rand()%255, 255)); 
        
    }
}



void drawScreen(){
    SDL_LockSurface(surface); 
    SDL_FillRect(surface,NULL,black);
    // memset(zbuffer,FLT_MIN,sizeof(float));
    for(int i=0;i<surface->w * surface->h;i++){
        zbuffer[i] = FLT_MIN;
    }
    // cout<<zbuffer[100]<<endl;
    drawModel(model);
    // Vec2i screen_coords[3]; 
    // screen_coords[0] = Vec2i(0,0);
    // screen_coords[1] = Vec2i(0,400);
    // screen_coords[2] = Vec2i(400,0);
    // drawTriangle(screen_coords,red);
    flipY();
    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);
}


int main(int argv, char** args)
{
    //初始化
    init();
    // model = new Model("obj/african_head.obj");
    model = new Model("D:/Project/VSCode/Render/obj/african_head.obj");

    //事件处理
    bool  running = true ;
    while(running){
        drawScreen();
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
            }
        }
                // running = false;

    }
    // system("pause");
    //退出
    SDL_FreeSurface(surface);
    SDL_Quit();
    return 0;
}