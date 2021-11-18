//
// Created by bunring on 19-3-11.
//
/*
#include "OGLTexture.h"
#include"OGLCommon.h"
#include "../P3dEngine.h"
BEGIN_P3D_NS
GLTexture::GLTexture():Texture(){
    generateTexture();
}
GLTexture::GLTexture(uint32 w,uint32 h,TextureType type, SampleType sample_quality, BUFFER_USE_TYPE usage,PixelFormat pixelformat,uint32 ssa):
            Texture(w,h,type,sample_quality,usage, pixelformat,ssa){
    generateTexture();
}
GLTexture::~GLTexture() {
	if (texId != 0)
		glDeleteTextures(1, &texId);
}

 void GLTexture::generateTexture(){
     if(texId != 0){
         glDeleteTextures(1,&texId);
		 texId = 0;
     }
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
		 case GL_TEXTURE_2D_MULTISAMPLE: {
			 glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texId);
			 glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ssa, iformat, width, height, GL_FALSE);
			 break;
		 }
     }
     glTexParameteri(texType,GL_TEXTURE_MAX_LEVEL,numMipMaps);
     glGenerateMipmap(texType);
     glTexParameteri(texType,GL_TEXTURE_MAG_FILTER,tsamplertype);
     glTexParameteri(texType,GL_TEXTURE_MIN_FILTER,tsamplertype);
	 glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	 glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

     glBindTexture(texType,0);
     
 }
pTexture GLTexture::copyTexture(){
    auto ret = std::make_shared<GLTexture>(width,height,type,sampleType,usage, format,ssa);
    auto tType = getGLTextureType();
    glCopyImageSubData(texId,tType,0,0,0,0,ret->getTextureId(),tType,\
                        0,0,0,0,width,height,depth);
    glBindTexture(tType,0);
	return ret;
}
uint32 GLTexture::getGLTextureType() {
	if (ssa > 1)
		return GL_TEXTURE_2D_MULTISAMPLE;
	static uint32 glTexType[] = { GL_TEXTURE_1D,GL_TEXTURE_2D,GL_TEXTURE_3D,GL_TEXTURE_CUBE_MAP };
	return glTexType[type];
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
pCPuImage GLTexture::dump2Buffer(bool src){
    auto retBuffer = std::make_shared<CpuImage>(type,format,width,height,depth,!src);
    auto tType = getGLTextureType();
	auto tform = getFromat();
    uint32 oformat = GLPixelUtil::getGLOriginFormat(tform);
    uint32 dataType = GLPixelUtil::getGLOriginDataType(tform);
	if (!src)
		dataType = GL_UNSIGNED_BYTE;
    if(type != TEXTURE_CUBE){
        glBindTexture(tType,texId);
      //  glGetTexImage(tType,0,oformat,dataType,retBuffer->getBufferSize(),(void*)retBuffer->getRawData());
		glGetTexImage(tType, 0, oformat, dataType, retBuffer->getRawData());

    }
    glBindTexture(tType,0);
	return retBuffer;
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
        glBindTexture(GL_TEXTURE_2D,texId);
        //glTexImage2D(GL_TEXTURE_2D,0,iformat,width,height,0,oformat,dataType,(void*)buffer->getRawData());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, oformat, dataType, buffer->getRawData());
    }

}

void GLTexture::applyShader(uint32 point){
    auto tType = getGLTextureType();
    glBindTexture(tType,this->texId);
    glActiveTexture(GL_TEXTURE0 + point);
}

END_P3D_NS
*/