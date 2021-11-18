#include"ViewPort.h"

BEGIN_P3D_NS

Viewport::Viewport(uint32 l,uint32 u,uint32 w,uint32 h):left(l),top(u),width(w),height(h){
    camera = std::make_shared<Camera>();
}


Viewport::Viewport(){
    camera = std::make_shared<Camera>();
}

void Viewport::setViewPort(uint32 l,uint32 t,uint32 w,uint32 h){
    left = l;
    top = t;
    width = w;
    height = h;
}
END_P3D_NS