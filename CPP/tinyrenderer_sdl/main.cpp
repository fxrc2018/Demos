#include <iostream>
#include <algorithm>
#include <limits>
#include <ctime>
#include <unordered_map>
extern "C"{
    #include <SDL2/SDL.h>
}
#include "geometry.h"
#include "model.h"
using namespace std;

Vec3f applyMatrix(Matrix m, Vec3f v){
    Vec4f v4 = embed<4,3,float>(v,1.0f);
    Vec4f res = m * v4;
    for(int i=0;i<3;i++){
        res[i] /= res[3];
    }
    return proj<3,4,float>(res);
}

class Camera{
public:
    void lookAt(Vec3f eye, Vec3f center, Vec3f up){
        position = eye;
        z = (eye-center).normalize(); //看向-z轴 所以z轴是eye-target
        x = cross(up,z).normalize();
        y = cross(z,x).normalize();
    }

    Vec3f position;
    Vec3f x;
    Vec3f y;
    Vec3f z;

    void walk(float d){
        position = position + z * d;
    }


    void strafe(float d){
        position = position + x * d;
    }

    Matrix getViewMatrix(){
        Matrix ModelView = Matrix::identity();
        Vec3f *arr[3] = {&x,&y,&z};
        for (int i=0; i<3; i++) {
            ModelView[0][i] = x[i];
            ModelView[1][i] = y[i];
            ModelView[2][i] = z[i];
            ModelView[i][3] = -((*arr[i])*position);
        }
        // cout<<position<<endl;
        return ModelView;
    }

    Matrix getRotateMatrix(Vec3f a, float angle){
        Matrix rm = Matrix::identity();
        float c = cos(angle);
        float s = sin(angle);
        rm[0][0] = c + (1 - c)*a.x*a.x;
        rm[0][1] = (1-c)*a.x*a.y - s*a.z;
        rm[0][2] = (1-c)*a.x*a.z + s*a.y;

        rm[1][0] = (1-c)*a.x*a.y + s*a.z;
        rm[1][1] = c + (1-c)*a.y*a.y;
        rm[1][2] = (1-c)*a.y*a.z - s*a.x;

        rm[2][0] = (1-c)*a.x*a.z - s*a.y;
        rm[2][1] = (1-c)*a.y*a.z + s*a.x;
        rm[2][2] = c + (1-c)*a.z*a.z;

        return rm;
    }

    void rotateY(float angle){
        cout<<angle<<endl;
        Matrix rm = getRotateMatrix(y,angle);
        x = applyMatrix(rm,x).normalize();
        z = applyMatrix(rm,z).normalize();
    }

    void rotateX(float angle){
        cout<<angle<<endl;
        Matrix rm = getRotateMatrix(x,angle);
        y = applyMatrix(rm,y).normalize();
        z = applyMatrix(rm,z).normalize();
    }

private:
    bool viewDirty = true;
};

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Surface* picture = NULL;
Model *model = NULL;
int width = 800;
int height = 800;
float *zbuffer = NULL;
Uint32 black, white, red, green;
float gr = 0.0f;

Vec3f       eye(0,1,3);
Vec3f    center(0,0,0);
Vec3f        up(0,1,0);

Camera camera;

unordered_map<SDL_Keycode,bool> keys;

void init(){
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SDL", 100, 100, width, height, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
    black = SDL_MapRGBA(surface->format, 0, 0, 0, 0xff);
    white = SDL_MapRGBA(surface->format, 0xff, 0xff, 0xff, 0xff);
    red = SDL_MapRGBA(surface->format, 0xff, 0, 0, 0xff);
    green = SDL_MapRGBA(surface->format, 0, 0xff, 0, 0xff);
    zbuffer = new float[width*height];
}

void drawPixel(int x, int y, Uint32 color){
    Uint32 *pixels = (Uint32*)surface->pixels;
    int pos = x + surface->w * y;
    // 要检查是否越界
    if(pos >=0 && pos < surface->w * surface->h){
        pixels[x + surface->w * y] = color;
    }
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    if(x<0 || y<0 || x > surface->w || y > surface->h){
        cout<<"x or y out of bound"<<endl;
        return black;
    } 
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0; /* shouldn't happen, but avoids warnings */
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


Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}
Vec3f barycentric(Vec3f *pts, Vec3f P) { 
    Vec3f AB = pts[1] - pts[0];
    Vec3f AC = pts[2] - pts[0];
    Vec3f PA = pts[0] - P;
    float u = AC.x*PA.y - AC.y*PA.x;
    float v = -AB.x*PA.y + AB.y*PA.x;
    float t = AB.x*AC.y - AB.y*AC.x;
    if(std::abs(t) > 1e-2){
        return Vec3f(1.0f-(u+v)/(float)t,u/(float)t,v/(float)t);
    }
    return Vec3f(-1,1,1);
} 

void drawTriangle(Vec3f *pts, Uint32 color){
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
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
             //对z进行差值
            p.z = bc.x*pts[0].z + bc.y*pts[1].z + bc.z*pts[2].z;
            if(bc.x <0||bc.y < 0||bc.z<0 || p.z >= 0){
                continue;
            } 
           
            int pos = p.x + p.y*surface->w;
            if(zbuffer[pos] < p.z ){
                zbuffer[pos] = p.z;
                drawPixel(p.x,p.y,color);
            }
        }
    }
}

void drawTriangle(Vec3f *pts, Vec2f *uv, Vec3f *wc){
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
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
    Vec3f puv;
    for(p.x = bboxmin.x;p.x<=bboxmax.x;p.x++){
        for(p.y=bboxmin.y;p.y<bboxmax.y;p.y++){
            Vec3f bc = barycentric(pts,p);
            
            if(bc.x <0||bc.y < 0||bc.z<0){
                continue;
            } 
            //对z进行差值
            p.z = bc.x*pts[0].z + bc.y*pts[1].z + bc.z*pts[2].z;
            puv.x = bc.x*uv[0].x + bc.y * uv[1].x + bc.z * uv[2].x;
            puv.y = bc.x*uv[0].y + bc.y * uv[1].y + bc.z * uv[2].y;


            Vec3f bc2 = bc;
            bc2.x = bc2.x / (1 + gr * wc[0].z);
            bc2.y = bc2.y / (1 + gr * wc[1].z);
            bc2.z = bc2.z / (1 + gr * wc[2].z);
            float co = 1.0f/(bc2.x + bc2.y + bc2.z);

            float c = -1.0f/gr;
            float z1 = wc[0].z - c;
            float z2 = wc[1].z - c;
            float z3 = wc[2].z - c;
            float zt = 1.0f / (bc.x/z1 + bc.y/z2 + bc.z/z3);
            puv.x = (uv[0].x*bc.x/z1 + uv[1].x*bc.y/z2 + uv[2].x*bc.z/z3) * zt;
            puv.y = (uv[0].y*bc.x/z1 + uv[1].y*bc.y/z2 + uv[2].y*bc.z/z3) * zt;
            //  puv.x = (uv[0].x*bc2.x + uv[1].x*bc2.y + uv[2].x*bc2.z) * co;
            // puv.y = (uv[0].y*bc2.x + uv[1].y*bc2.y + uv[2].y*bc2.z) * co;

            // Vec3f bc2 = bc;
            // bc2.x = bc2.x / (1 + gr * wc[0].z);
            // bc2.y = bc2.y / (1 + gr * wc[1].z);
            // bc2.z = bc2.z / (1 + gr * wc[2].z);
            // float co = 1.0f/(bc2.x + bc2.y + bc2.z);
            // puv.x = (bc2.x*uv[0].x + bc2.y * uv[1].x + bc2.z * uv[2].x)*co;
            // puv.y = (bc2.x*uv[0].y + bc2.y * uv[1].y + bc2.z * uv[2].y)*co;

           
            puv.x = (int)(puv.x * picture->w);
            puv.y = picture->h - (int)(puv.y * picture->h);
            // cout<<puv<<endl;
            int pos = p.x + p.y*surface->w;
            if(zbuffer[pos] < p.z ){
                zbuffer[pos] = p.z;
                
                drawPixel(p.x,p.y,getpixel(picture,puv.x,puv.y));
            }
        }
    }
}

Matrix projection(float coeff) {
    gr = coeff;
    Matrix Projection = Matrix::identity();
    Projection[3][2] = coeff;
    return Projection;
}

Matrix scale(float x, float y, float z){
    Matrix s = Matrix::identity();
    s[0][0] = x;
    s[1][1] = y;
    s[2][2] = z;
    return s;
}

Matrix translate(float x, float y, float z){
    Matrix t = Matrix::identity();
    t[0][3] = x;
    t[1][3] = y;
    t[2][3] = z;
    return t;
}




Matrix rotate(float x, float y, float z){
    Matrix r = Matrix::identity();
    y =  y/90 * acos(0.0f);
    r[0][0] = cos(y);
    r[0][2] = sin(y);
    r[2][0] = -sin(y);
    r[2][2] = cos(y);
    return r;
}

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize(); //看向-z轴 所以z轴是eye-target
    Vec3f x = cross(up,z).normalize();
    Vec3f y = cross(z,x).normalize();
    Matrix ModelView = Matrix::identity();
    Vec3f *arr[3] = {&x,&y,&z};
    for (int i=0; i<3; i++) {
        ModelView[0][i] = x[i];
        ModelView[1][i] = y[i];
        ModelView[2][i] = z[i];
        ModelView[i][3] = -((*arr[i])*eye);
    }
    return ModelView;
}

Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.0f+0.5f), int((v.y+1.)*height/2.0f+0.5f), v.z);
}

void drawModel(Model *m){

    

    Matrix la = camera.getViewMatrix();
    // viewport(width/8, height/8, width*3/4, height*3/4);
    Matrix pr = projection(-1.f/(eye-center).norm());

    Matrix tr = translate(0,0,1);

    for (int i=0; i<m->nfaces(); i++) { 
        std::vector<int> face = m->face(i); 
        Vec3f screen_coords[3]; 
        Vec3f world_coords[3];
        Vec2f uv[3];
        for (int j=0; j<3; j++) { 
            world_coords[j] = m->vert(face[j]); 
            Vec3f t = applyMatrix(pr * la * tr ,world_coords[j]);
            world_coords[j] = applyMatrix(la * tr ,world_coords[j]);
            // screen_coords[j] = Vec3f((world_coords[j].x+1.0f)*surface->w/2.0f, (world_coords[j].y+1.0f)*surface->h/2.0f,world_coords[j].z); 
            screen_coords[j] = world2screen(t);
            uv[j] = model->uv(i,j);
        } 
        // Vec3f n = cross ((world_coords[2]-world_coords[0]), (world_coords[1]-world_coords[0])); 
        // n.normalize(); 
        // Vec3f light_dir(0,0,-1);
        // float intensity = n*light_dir; 
        // if (intensity>0) { 
        //     drawTriangle(screen_coords,SDL_MapRGBA(surface->format,intensity * 255, intensity *255, intensity *255, 255));
        // } 
        drawTriangle(screen_coords,uv,world_coords);

    }
}



void drawScreen(){
    SDL_LockSurface(surface); 
    SDL_FillRect(surface,NULL,black);
    for(int i=0;i<surface->w * surface->h;i++){
        zbuffer[i] = -std::numeric_limits<float>::max();
    }
    drawModel(model);

    flipY();
    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);
}


void handKeydown(SDL_Keycode keycode, float deltaTime){
    switch (keycode){
        case SDLK_w:
            eye = eye + Vec3f(0.0f,0.0f,0.2f);
            break;
        case SDLK_d:
            eye = eye - Vec3f(0.0f,0.0f,0.2f);
            break;
        default:
            break;
    }
}

void update(float deltaTime){
    float speed = 1.5f;
    if(keys[SDLK_w]){
        camera.walk(-speed * deltaTime);
    }
    if(keys[SDLK_s]){
        camera.walk(speed * deltaTime);
    }
    if(keys[SDLK_a]){
        camera.strafe(-speed * deltaTime);
    }
    if(keys[SDLK_d]){
        camera.strafe(speed * deltaTime);
    }
}

int main(int argv, char** args)
{
    //初始化
    init();
    camera.lookAt(eye,center,up);
    
    // model = new Model("obj/african_head.obj");
    model = new Model("D:/Project/VSCode/Render/obj/african_head.obj");
    picture = SDL_LoadBMP("D:/Project/VSCode/Render/obj/african_head_diffuse.bmp");
    // model = new Model("D:/Project/VSCode/Render/obj/floor.obj");
    // picture = SDL_LoadBMP("D:/Project/VSCode/Render/obj/floor_diffuse.bmp");
    // SDL_BlitSurface(picture, NULL, surface, NULL );
    // SDL_UpdateWindowSurface(window);
    // system("pause");

    //事件处理
    bool  running = true ;
    
    float totalTime = 0.0f;
    float deltaTime = 0.0f;
    int frames = 0;
    clock_t preTime = clock();
    clock_t curTime = clock();
    while(running){
        //统计帧率
        curTime = clock();
        deltaTime = (float)(curTime - preTime)/(1000.0f);
        preTime = curTime;
        totalTime += deltaTime;
        frames++;
        if(totalTime > 1.0f){
            string s = "fps:" + to_string(frames);
            SDL_SetWindowTitle(window,s.c_str());
            frames = 0;
            totalTime = 0.0f;
        }

        //处理事件
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    keys[event.key.keysym.sym] = true;
                    break;
                case SDL_KEYUP:
                    keys[event.key.keysym.sym] = false;
                    break;
                case SDL_MOUSEMOTION:{
                    float xr = (float)event.motion.xrel / -100.0f;
                    camera.rotateY(xr);
                    float yr = (float)event.motion.yrel / -100.0f;
                    camera.rotateX(yr);
                }   
                    break;
                default:
                    break;
            }
        }

        update(deltaTime);

        drawScreen();
    }
    // system("pause");
    //退出
    SDL_FreeSurface(surface);
    SDL_Quit();
    return 0;
}
