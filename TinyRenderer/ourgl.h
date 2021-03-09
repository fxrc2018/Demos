#ifndef __OURGL_H__
#define __OURGL_H__

#include "FreeImage.h"
#include "geometry.h"

struct IShader {
    virtual Vec4f vertex(const int iface, const int nthvert) = 0;
    virtual bool fragment(const Vec3f bar, RGBQUAD &color) = 0;
};

#endif