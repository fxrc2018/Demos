#ifndef __MODEL_H__
#define __MODEL_H__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "FreeImage.h"
#include "geometry.h"

class Model{
public:
    int nfaces;
    Model(const std::string &filename);
    std::vector<Vec3f> getFace(int i);
    std::vector<Vec3f> getVt(int i);
    void loadDiffuseTexture(const std::string &filename);
    RGBQUAD getVtColor(float x, float y);
    FIBITMAP *vtmap;
    
private:
    std::vector<Vec3f> verts;
    std::vector<Vec3f> vts;
    std::vector<std::vector<Vec3i>> faces;
};

#endif