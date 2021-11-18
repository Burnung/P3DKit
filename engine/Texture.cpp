//
// Created by bunring on 19-3-11.
//

#include"Texture.h"
#include "OPenGL/OGLTexture.h"

BEGIN_P3D_NS

//Texture(uint32 w, uint32 h, TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage, PixelFormat pixelformat)
Texture::Texture(uint32 w,uint32 h,TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage,PixelFormat pixelformat,uint32 ssa): type(type),sampleType(sample_quality),usage(usage),
                                                                                            format(pixelformat),width(w),height(h),ssa(ssa){

}
std::shared_ptr<Texture> Texture::make(uint32 appType, uint32 w, uint32 h, TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage, PixelFormat pixelformat, uint32 ssa,uint32 mipNum) {
	std::shared_ptr<Texture> ret = nullptr;
	if(appType == APP_OPENGL)
		ret =  std::make_shared<GLTexture>(w, h, type, sample_quality, usage, pixelformat,ssa,mipNum);
#if VULKAN_ENABLE
	if (appType == APP_VULKAN)
		ret = std::make_shared<GLTexture>(w, h, type, sample_quality, usage, pixelformat, ssa,mipNum);
#endif
	return ret;
}

std::shared_ptr<Texture> Texture::makeFromCpu(uint32 appType,pCPuImage tImg, SampleType sample_quality, BUFFER_USE_TYPE usage, uint32 ssa) {
	auto w = tImg->getWidth(), h = tImg->getHeight();
	auto ret = make(appType, w, h, tImg->getTextureType(), sample_quality, usage, tImg->getPiexFormat(), ssa,1);
	ret->updateSource(tImg);
	return ret;
}

uint32 Texture::Width(uint32_t level){
    uint32 w = width;
    while(level > 0){
        w >> 1;
        --level;
    }
    return w;
}
    // Returns the height of the texture.
uint32_t Texture::Height(uint32_t level){
    uint32  h = height;
    while(level > 0){
        h >> 1;
        --level;
    }
    return h;
}
    // Returns the depth of the texture (only for 3D texture).
uint32_t Texture::Depth(uint32_t level){
	if (type == TEXTURE_2D)
		return 1;
	if (type == TEXTURE_CUBE)
		return 6;
    uint32 d = depth;
    while(level > 0){
        d >> 1;
        --level;
    }
    return d;
}
END_P3D_NS