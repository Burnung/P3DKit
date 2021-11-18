#include"Camera.h"
#pragma once
BEGIN_P3D_NS
class Viewport :public NoCopyable,public P3DObject{
public:
    Viewport();
    Viewport(uint32 left,uint32 top,uint32 w,uint32 h);
public:
    inline uint32 getLeft(){return left;}
    inline uint32 getTop() {return top;}
    inline uint32 getWidth() {return width;}
    inline uint32 getHeight() {return height;}
    inline pCamera getCamera(){return camera;} 

public:
    void setViewPort(uint32 left,uint32 top,uint32 width,uint32 height);
    void setCamera(pCamera tCamera){camera = tCamera;}

    private:
    uint32 left = 0;
    uint32 top = 0;
    uint32 width = 512;
    uint32 height = 512;
    pCamera camera;
};
typedef std::shared_ptr<Viewport> pVeiwport;

END_P3D_NS