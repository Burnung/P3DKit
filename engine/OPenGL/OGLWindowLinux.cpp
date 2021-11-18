
//
#if IS_LINUX_USE

#include <GL/glew.h>
#include "common/P3D_Utils.h"
#include "common/RenderCommon.h"
#include "OGLWindowLinux.h"
#include "OGLCommon.h"
BEGIN_P3D_NS

GLWindow::~GLWindow() {
	eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(mDisplay, mContext);
	eglDestroySurface(mDisplay, mSurface);
	eglReleaseThread();
	eglTerminate(mDisplay);

	mContext = EGL_NO_CONTEXT;
	mDisplay = EGL_NO_DISPLAY;
}

void GLWindow::initEGLContext() {
	const EGLint configAttribs[] = {
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
			EGL_NONE
	};

	const EGLint pbufferAttribs[] = {
		EGL_WIDTH, 1600,
		EGL_HEIGHT, 900,
		EGL_NONE,
	};

	static const int MAX_DEVICES = 4;
	EGLDeviceEXT eglDevs[MAX_DEVICES];
	EGLint numDevices;

	PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =
		(PFNEGLQUERYDEVICESEXTPROC)
		eglGetProcAddress("eglQueryDevicesEXT");

	eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);

	printf("Detected %d devices\n", numDevices);

	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
		(PFNEGLGETPLATFORMDISPLAYEXTPROC)
		eglGetProcAddress("eglGetPlatformDisplayEXT");

	mDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT,
		eglDevs[0], 0);

	EGLBoolean result = eglInitialize(mDisplay, nullptr, nullptr);

	EGLint numConfigs;
	EGLConfig eglConfig;
	result = eglChooseConfig(mDisplay, configAttribs, &eglConfig, 1, &numConfigs);

	mSurface = eglCreatePbufferSurface(mDisplay, eglConfig, pbufferAttribs);

	eglBindAPI(EGL_OPENGL_API);

	mContext = eglCreateContext(mDisplay, eglConfig, EGL_NO_CONTEXT, nullptr);
	eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);
}

GLWindow::GLWindow(uint32 width, uint32 height):P3DWindow(width,height) {
	appType = APP_OPENGL;
	initEGLContext();
	//int fs = fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW;


	if (glewInit() != GLEW_OK) {
		logOut.log("create glew error");
	}
	const GLubyte* name = glGetString(GL_VENDOR); //���ظ���ǰOpenGLʵ�ֳ��̵�����
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //����һ����Ⱦ����ʶ����ͨ���Ǹ�Ӳ��ƽ̨
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //���ص�ǰOpenGLʵ�ֵİ汾��

	printf("OpenGL vendor��%s\n", name);
	printf("display card��%s\n", biaoshifu);
	printf("OpenGL ver��%s\n", OpenGLVersion);

	float lineRange[10];
	//glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineRange);
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);

	//glLineWidth(0.001);
	GLUtils::init();
}


void GLWindow::render() {
//	GLUtils::clearBuffers(1.0, 1.0, 1.0, 0.0);
	scene->render();
}
END_P3D_NS
#endif
