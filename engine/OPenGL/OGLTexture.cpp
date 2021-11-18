//
// Created by bunring on 19-3-11.
//

#include "OGLTexture.h"
#include"OGLCommon.h"
#include "../P3dEngine.h"
BEGIN_P3D_NS
GLTexture::GLTexture():Texture(){
    generateTexture();
}
GLTexture::GLTexture(uint32 w,uint32 h,TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage,PixelFormat pixelformat,uint32 ssa,uint32 mipNum):
            Texture(w,h,type,sample_quality,usage, pixelformat,ssa){
	this->numMipMaps = mipNum;
    generateTexture();
}
GLTexture::~GLTexture() {
	if (texId != 0)
		glDeleteTextures(1, &texId);
}
void GLTexture::createGLTexture() {
	GLUtils::checkGLError("create texture begin");
	auto texType = getGLTextureType();
	if (type == TEXTURE_2D)
		depth = 1;
	else if (type == TEXTURE_CUBE)
		depth = 6;
	glCreateTextures(texType, 1, &texId);
	GLUtils::checkGLError("create texture 1");
    uint32 iformat = GLPixelUtil::getGLInternalFormat(format);
	if(numMipMaps < 1)
		numMipMaps = Utils::numMipmapLevels(width, height);
	glTextureStorage2D(texId, numMipMaps, iformat, width, height);
	GLUtils::checkGLError("create texture 2");
     uint32 tsamplertype = getGLSamplerType();

	 if (tsamplertype == GL_LINEAR) {
		glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER,  numMipMaps > 1 ? GL_LINEAR_MIPMAP_LINEAR:GL_LINEAR);
	 }
	 else {
		glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER,  tsamplertype);

	 }
	glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, tsamplertype);
	glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameterf(texId, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);
	GLUtils::checkGLError("create texture end");
	if (numMipMaps > 1)
		glGenerateTextureMipmap(texId);
}

 void GLTexture::generateTexture(){
     if(texId != 0){
         glDeleteTextures(1,&texId);
		 texId = 0;
     }
#ifdef _WIN32
	 createGLTexture();
	 return;
#endif
     glGenTextures(1,&texId);

     uint32 oformat = GLPixelUtil::getGLOriginFormat(format);
     uint32 iformat = GLPixelUtil::getGLInternalFormat(format);
     uint32 dataType = GLPixelUtil::getGLOriginDataType(format);
     uint32 tsamplertype = getGLSamplerType();

     uint32 texType = getGLTextureType();
     switch(texType){
         case GL_TEXTURE_2D:{
             glBindTexture(GL_TEXTURE_2D,texId);
             glTexImage2D(GL_TEXTURE_2D,0,iformat,width,height,0,oformat,dataType,nullptr);
             break;
         }
		 case GL_TEXTURE_CUBE_MAP: {
			 glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
			 glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, iformat, width, height, 0, oformat, dataType, nullptr);
		 }
#if !IS_IOS
		 case GL_TEXTURE_2D_MULTISAMPLE: {
			 glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texId);
			 glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ssa, iformat, width, height, GL_FALSE);
			 break;
		 }
#endif
     }
    // glTexParameteri(texType,GL_TEXTURE_MAX_LEVEL,numMipMaps);
     //glGenerateMipmap(texType);
     glTexParameteri(texType,GL_TEXTURE_MAG_FILTER,tsamplertype);
     glTexParameteri(texType,GL_TEXTURE_MIN_FILTER,tsamplertype);
	 glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	 glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

     glBindTexture(texType,0);
     
 }
 
void GLTexture::copyFromTex(std::shared_ptr<Texture> src, int srcLevel,int dstLevel) {
	//P3D_ENSURE()
	auto texType = getGLTextureType();
	glCopyImageSubData(src->getHandler(),texType, srcLevel, 0, 0, 0,
		texId, texType, dstLevel, 0, 0, 0,
		src->Width(),src->Height(),src->Depth());
	GLUtils::checkGLError("copy from texture");

}
pTexture GLTexture::copyTexture(){
#if IS_IOS
    return nullptr;
#else
    auto ret = std::make_shared<GLTexture>(width,height,type,sampleType,usage, format,ssa,numMipMaps);
    auto tType = getGLTextureType();
	
	for (int i = 0; i < numMipMaps; ++i) {
		glCopyImageSubData(texId, tType,i, 0, 0, 0, ret->getTextureId(), tType, \
			i, 0, 0, 0, width, height, depth);
	}
    glBindTexture(tType,0);
	return ret;
#endif
}

uint32 GLTexture::getGLTextureType() {
#if IS_IOS
    return GL_TEXTURE_2D;
#else
	if (ssa > 1)
		return GL_TEXTURE_2D_MULTISAMPLE;
	static uint32 glTexType[] = { GL_TEXTURE_1D,GL_TEXTURE_2D,GL_TEXTURE_3D,GL_TEXTURE_CUBE_MAP };
	return glTexType[type];
#endif
}
uint32 GLTexture::getGLSamplerType() {
	static uint32 glSampleTypep[] = { GL_NEAREST,GL_LINEAR };
	return glSampleTypep[sampleType];
}
void GLTexture::render2Texture(std::shared_ptr<Texture> dst) {
	auto pFbo = P3DEngine::instance().getIntenalRenderFrame(dst->Width(0),dst->Height(0), dst->getSsa());
	pFbo->setRenderTexture(dst);
	pFbo->apply();
	GLUtils::render2Quad(shared_from_this());
	pFbo->apply0();

}
pCPuImage GLTexture::dump2Buffer(bool src,PixelFormat pixFromat){
#if IS_IOS
    return nullptr;
#else
    auto retBuffer = std::make_shared<CpuImage>(type,pixFromat,width,height,depth,!src);
    auto tType = getGLTextureType();
	auto tform = getFormat();
    uint32 oformat = GLPixelUtil::getGLOriginFormat(pixFromat);
    uint32 dataType = GLPixelUtil::getGLOriginDataType(tform);
	if (!src)
		dataType = GL_UNSIGNED_BYTE;
    if(type != TEXTURE_CUBE){
        glBindTexture(tType,texId);
		glGetTexImage(tType, 0, oformat, dataType, retBuffer->getRawData());
		//glGetTexImage(tType, 0, oformat, dataType, retBuffer->getRawData());

    }
    glBindTexture(tType,0);
	return retBuffer;
#endif
}

void GLTexture::updateSource(pCPuImage buffer){
    P3D_ENSURE(type == buffer->type,"texture update type error");
    if(width != buffer->width || height != buffer->height || depth != buffer->depth || format != buffer->pixelFormat){
        width = buffer->width;
        height = buffer->height;
        depth = buffer->depth;
		format = buffer->pixelFormat;
        generateTexture();
    }
    uint32 oformat = GLPixelUtil::getGLOriginFormat(format);
    uint32 iformat = GLPixelUtil::getGLInternalFormat(format);
    uint32 dataType = GLPixelUtil::getGLOriginDataType(format);
    uint32 tsamplertype = getGLSamplerType();
    if(type == TEXTURE_2D){
       // glBindTexture(GL_TEXTURE_2D,texId);
		GLUtils::checkGLError("update GLtexure1");
        //glTexImage2D(GL_TEXTURE_2D,0,iformat,width,height,0,oformat,dataType,(void*)buffer->getRawData());
		//auto tDataBlock = std::make_shared<DataBlock>(816 * 1024 * 3);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, oformat, dataType, buffer->getRawData());
		glTextureSubImage2D(texId, 0, 0, 0, width, height, oformat, dataType, buffer->getRawData());
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer->getRawData());
		GLUtils::checkGLError("update GLtexure");
    }
	if (numMipMaps > 1)
		glGenerateTextureMipmap(texId);

}

void GLTexture::applyShader(uint32 point){
   // auto tType = getGLTextureType();
  //  glActiveTexture(GL_TEXTURE0 + point);
   // glBindTexture(tType,this->texId);
	glBindTextureUnit(point, this->texId);
}
void GLTexture::genMipMaps() {
	if (numMipMaps <= 1)
		return;
	glGenerateTextureMipmap(texId);
}
/*
void GLTexture::fromCpuImage(pCPuImage tImg) {
	P3D_ENSURE((type != tImg->getTextureType()), "cpu Image type should be same");
	P3D_ENSURE((width == tImg->getWidth() && height == tImg->getHeight()), "cpu Image size should be same");
	
	auto oformat = GLPixelUtil::getGLOriginFormat(tImg->getPiexFormat());
	auto iformat = GLPixelUtil::getGLInternalFormat(format);
	auto dataType = GLPixelUtil::getGLOriginFormat(format);
	if (type == TEXTURE_2D) {
		glBindTexture(GL_TEXTURE_2D, texId);
	}

}
*/
END_P3D_NS
