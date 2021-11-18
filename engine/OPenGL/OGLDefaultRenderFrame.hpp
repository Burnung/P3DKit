#include "../defaultRenderFrame.h"
#pragma  once
BEGIN_P3D_NS
class OGLDefaultRenderFrame :public DefaultRenderFrame {
public:
	explicit OGLDefaultRenderFrame(uint32 w, uint32 h, uint32 fboId=0) :DefaultRenderFrame(w, h, 1) {
		fboId = fboId;
	};
	virtual ~OGLDefaultRenderFrame(){};
public:
	virtual void apply() {
		glGetIntegerv(GL_VIEWPORT, lastViewArg);
		static uint32 renderTarget1[] = { GL_COLOR_ATTACHMENT0 };// , GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		//glDrawBuffers(1, );
		glViewport(0, 0, w, h);
	};
	virtual void apply0() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(lastViewArg[0], lastViewArg[1], lastViewArg[2], lastViewArg[3]);
	};
private:
	uint32 fboId = 0;
	int32 lastViewArg[4];
};
//typedef std::shared_ptr<RenderFrame> pRenderFrame;
//using pRenderFrame = std::shared_ptr<RenderFrame>;
END_P3D_NS