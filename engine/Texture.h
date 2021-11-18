//
// Created by bunring on 19-3-11.
//

#pragma once

#include"common/P3D_Utils.h"
#include "common/RenderCommon.h"

BEGIN_P3D_NS

class Texture: public P3DObject,public std::enable_shared_from_this<Texture>{
public:
	static std::shared_ptr<Texture> make(uint32 appType, uint32 w, uint32 h, TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage, PixelFormat pixelformat, uint32 ssa,uint32 mipNum);
	static std::shared_ptr<Texture> makeFromCpu(uint32 appType, pCPuImage tImg, SampleType smaple_quality, BUFFER_USE_TYPE usage,uint32 ssa);
public:
    Texture(uint32 w,uint32 h,TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage,PixelFormat pixelformat,uint32 ssa);

    Texture(){};
    //virtual ~Texture();

    // Gets the name of texture
    // std::string const & getName() const;

    // Gets the number of mipmaps to be used for this texture.
    uint32 getNumMipMap() const {return numMipMaps;}
    // Gets the size of texture array

    // Returns the width of the texture.
    uint32 Width(uint32_t level = 0) ;
    // Returns the height of the texture.
    uint32 Height(uint32 level = 0) ;
    // Returns the depth of the texture (only for 3D texture).
    uint32 Depth(uint32 level =0);
	uint32 getSsa() { return ssa; }

    // Returns the pixel format for the texture surface.
    PixelFormat getFormat() {return format;}
	BUFFER_USE_TYPE const getBufferUsage() { return usage; }

    // Returns the texture type of the texture.
    TextureType getType() {return type;}
	virtual uint32 getHandler() = 0;

public:

	virtual std::shared_ptr<Texture> copyTexture() { return nullptr; }

	virtual void render2Texture(std::shared_ptr<Texture> dst) {};

    //virtual void buildMipSubLevels() = 0;

	virtual pCPuImage dump2Buffer(bool use_src,PixelFormat fomat = PIXEL_RGBA8){ return nullptr; };

	virtual void updateSource(pCPuImage buffer) {};
	virtual void copyFromTex(std::shared_ptr<Texture> src, int srcLevel=0, int mipmapLevel=0) {};

	virtual void applyShader(uint32 t = 0) {};
	virtual void genMipMaps() {};
	//virtual void fromCpuImage(pCPuImage tImg) {};


protected:
    uint32                  width = 512;
    uint32                  height = 512;
    uint32                  depth = 1;
    uint32		            numMipMaps = 1;
	uint32	               ssa = 1;
    //uint32_t		array_size_;
    BUFFER_USE_TYPE         usage = READ_ONLY;
    PixelFormat	            format  = PIXEL_RGBA8;
    TextureType	            type = TEXTURE_2D;
    SampleType	             sampleType =SAMPLE_LINER;
};
typedef std::shared_ptr<Texture> pTexture;
END_P3D_NS
