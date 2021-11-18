#include "OGLFBO.h"
#include "OGLCommon.h"
#include "../P3dEngine.h"
BEGIN_P3D_NS
GLFbo::GLFbo(uint32 w, uint32 h, uint32 ssa):RenderFrame(w,h,ssa) {
	glGenRenderbuffers(1, &depthPbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depthPbo);
	if (ssa > 1) {
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, ssa, GL_DEPTH_COMPONENT32F, w, h);
		
	}
	else {
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, w, h);
	}
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthPbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	defaultTex = P3DEngine::instance().createTexture(w, h, TEXTURE_2D, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, ssa);
	addRenderTexture(defaultTex);
	this->w = w;
	this->h = h;
}
void GLFbo::resize(int32 w, int32 h) {
	if (w == this->w && h == this->h)
		return;
	release();
	this->w = w, this->h = h;
	glGenRenderbuffers(1, &depthPbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depthPbo);
	if (ssa > 1) {
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, ssa, GL_DEPTH_COMPONENT32F, w, h);

	}
	else {
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, w, h);
	}
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthPbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//defaultTex = P3DEngine::instance().createTexture(w, h, TEXTURE_2D, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, ssa);
	//addRenderTexture(defaultTex);
	auto tmpRenderCount = renderCount;
	renderCount = 0;
	addRenderTextures(tmpRenderCount);
}

void GLFbo::release() {
	renderTex.clear();
	if (depthPbo > 0)
		glDeleteRenderbuffers(1, &depthPbo);
	if (fboId > 0)
		glDeleteFramebuffers(1, &fboId);
}
GLFbo::~GLFbo() {
	release();
}
void GLFbo::addRenderTexture(pTexture tTexture) {
	auto tex = std::dynamic_pointer_cast<GLTexture>(tTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
#if IS_IOS
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + renderCount, GL_TEXTURE_2D, tex->getTextureId(), 0);
#else
	if (ssa > 1) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + renderCount, GL_TEXTURE_2D_MULTISAMPLE, tex->getTextureId(), 0);
	}
	else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + renderCount, GL_TEXTURE_2D, tex->getTextureId(), 0);
	}
#endif
	renderTex.push_back(tTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderCount++;
}
void GLFbo::setRenderTexture(pTexture tTexture) {

	auto tex = std::dynamic_pointer_cast<GLTexture>(tTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
#if IS_IOS
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, tex->getTextureId(), 0);
#else
	if (ssa > 1) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D_MULTISAMPLE, tex->getTextureId(), 0);
	}
	else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, tex->getTextureId(), 0);
	}
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void GLFbo::applyMulti(uint32 nCount) {
	glGetIntegerv(GL_VIEWPORT, lastViewArg);
	P3D_ENSURE(nCount <= renderCount, "too many render target");
	static uint32 renderTarget[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5,GL_COLOR_ATTACHMENT6,GL_COLOR_ATTACHMENT7 };
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glDrawBuffers(nCount, renderTarget);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, w, h);

}
void GLFbo::apply() {
	glGetIntegerv(GL_VIEWPORT, lastViewArg);
	static uint32 renderTarget1[] = { GL_COLOR_ATTACHMENT0 };// , GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glDrawBuffers(1, renderTarget1);
	glViewport(0, 0, w, h);
}

void GLFbo::apply0() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(lastViewArg[0], lastViewArg[1], lastViewArg[2], lastViewArg[3]);
}


void GLFbo::addRenderTextures(uint32 nCount) {
	doFor(i, nCount) {
		auto pTexture = std::make_shared<GLTexture>(w, h, TEXTURE_2D, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF,ssa,1);
		addRenderTexture(pTexture);
	}
}
END_P3D_NS
