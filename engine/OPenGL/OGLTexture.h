//
// Created by bunring on 19-3-11.
//
#include"../Texture.h"

#pragma once

BEGIN_P3D_NS
class GLTexture :public Texture {
public:
    GLTexture();
    GLTexture(uint32 w,uint32 h,TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage,PixelFormat pixelformat,uint32 ssa,uint32 mipNum);
	virtual ~GLTexture();

public:
    virtual pTexture copyTexture();
   


    virtual pCPuImage dump2Buffer(bool src = false,PixelFormat pixFromat=PIXEL_BGRA8) ;
    virtual void updateSource(pCPuImage buffer);

    virtual void applyShader(uint32 point);

	virtual void render2Texture(std::shared_ptr<Texture> dst);
	virtual void copyFromTex(std::shared_ptr<Texture> src,int srcLevel,int mipmapLevel);

	//virtual void fromCpuImage(pCPuImage tImg);

public:
    uint32 getTextureId(){return texId;}
	virtual uint32 getHandler() { return texId; }
	virtual void genMipMaps();


private:
    uint32 getGLTextureType();
    uint32 getGLSamplerType();

    
private:
    void generateTexture();
	void createGLTexture();

private:
	uint32 texId = 0;
};


END_P3D_NS
