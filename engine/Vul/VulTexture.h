//
// Created by bunring on 19-3-11.
//
#include"../Texture.h"

#pragma once

BEGIN_P3D_NS
class GLTexture :public Texture {
public:
    GLTexture();
    GLTexture(uint32 w,uint32 h,TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage,PixelFormat pixelformat,uint32 ssa);
	virtual ~GLTexture();

public:
    virtual pTexture copyTexture();
   


    virtual pCPuImage dump2Buffer(bool src = false) ;
    virtual void updateSource(pCPuImage buffer);

    virtual void applyShader(uint32 point);

	virtual void render2Texture(std::shared_ptr<Texture> dst);
public:
    uint32 getTextureId(){return texId;}


private:
    uint32 getGLTextureType();
    uint32 getGLSamplerType();

    
private:
    void generateTexture();

private:
	uint32 texId = 0;
};


END_P3D_NS
