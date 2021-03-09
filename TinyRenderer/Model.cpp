#include "Model.h"

Model::Model(const std::string &filename){
    nfaces = 0;
    std::ifstream in;
    in.open(filename,std::ifstream::in);
    if(in.fail()){
        std::cout<<in.failbit<<std::endl;
        return;
    }
    std::string line;
    std::string op;
    Vec3f vf;
    Vec3i vi;
    char c;
    int i1,i2,i3;
    while(!in.eof()){
        std::getline(in,line);
        std::istringstream iss(line);
        iss>>op;
        if(op == "v"){
            iss>>vf[0]>>vf[1]>>vf[2];
            verts.push_back(vf);
        }else if(op == "f"){
            nfaces++;
            std::vector<Vec3i> tmp;
            for(int i=0;i<3;i++){
                iss>>vi.x>>c>>vi.y>>c>>vi.z;
                tmp.push_back(vi);
            }
            faces.push_back(tmp);
        }else if(op == "vt"){
            iss>>vf[0]>>vf[1]>>vf[2];
            vts.push_back(vf);
        }
    }
    in.close();
    std::cout<<"vt"<<vts.size()<<std::endl;
    loadDiffuseTexture(filename);
}

std::vector<Vec3f> Model::getFace(int i){
    std::vector<Vec3f> face;
    for(int j=0;j<3;j++){
        Vec3f vf = verts[faces[i][j][0] - 1];
        face.push_back(vf);
    }
    return face;
}

std::vector<Vec3f> Model::getVt(int i){
    std::vector<Vec3f> face;
    for(int j=0;j<3;j++){
        Vec3f vf = vts[faces[i][j][1] - 1];
        face.push_back(vf);
    }
    return face;
}

void Model::loadDiffuseTexture(const std::string &filename){
    std::string fn = filename.substr(0,filename.size()-4);
    fn.append("_diffuse.png");
    std::cout<<fn<<std::endl;
    vtmap = FreeImage_Load(FIF_PNG,fn.c_str());
}

RGBQUAD Model::getVtColor(float x, float y){
    RGBQUAD color; 
    // int w = FreeImage_GetWidth(vtmap);
    // int h = FreeImage_GetHeight(vtmap);
    x = x * FreeImage_GetWidth(vtmap);
    y = y * FreeImage_GetHeight(vtmap);
    FreeImage_GetPixelColor(vtmap,(unsigned int)x,(unsigned int)y,&color);
    color.rgbReserved = 255;
    return color;
}