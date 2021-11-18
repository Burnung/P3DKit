#pragma once

#if IS_LINUX_USE
#include "common/P3D_Utils.h"
#include"common/RenderCommon.h"
#include "../Window.h"
#include <GL/glew.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

BEGIN_P3D_NS

class GLWindow : public P3DWindow {
public:
	virtual ~GLWindow();
public:
	explicit GLWindow(uint32 width, uint32 height);
	//virtual void addComp(pComp comp, pShader shader) override;
	virtual void render();
	//virtual void* getWinHandler() { return win; }
protected:
	void initEGLContext();
private:
	EGLDisplay mDisplay;
	EGLContext mContext;
	EGLSurface mSurface;
};
typedef std::shared_ptr<GLWindow> pGLWindow;

END_P3D_NS
#endif
