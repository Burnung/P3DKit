//
// Created by bunring on 19-3-11.
//

#pragma once
#if !IS_IOS
#include <GL/glew.h>
#else
#include<OpenGLES/ES3/gl.h>
#include<OpenGLES/ES3/glext.h>
#endif
#include "common/P3D_Utils.h"
#include"common/RenderCommon.h"
#include "common/mathVector.hpp"
#include "../Texture.h"
#if IS_WINDOWS ||  IS_MACOS
#include <GLFW/glfw3.h>
#endif


BEGIN_P3D_NS

enum ATTRILOCATION{
	LOC_VPS,
	LOC_UVS,
	LOC_VNS,
	LOC_NUM
};

extern const char* ATTRINAME[];// = { "InPos","InUv","InVn" };

class GLPixelUtil{
public:
    static uint32 getGLOriginFormat(PixelFormat format);
    static uint32 getGLOriginDataType(PixelFormat format);
    static uint32 getGLInternalFormat(PixelFormat format);
};

const uint32 GLVertexBufferType[VERTEX_BUFFER_NUM] = { GL_ARRAY_BUFFER,GL_ELEMENT_ARRAY_BUFFER,GL_UNIFORM_BUFFER };
const uint32 GLVertexBufferUsage[VBSAGET_NUM] = { GL_STATIC_READ,GL_STATIC_COPY,GL_STATIC_DRAW,GL_DYNAMIC_DRAW };
const uint32 OGLGEOTYPE[GEO_TYPE_NUM] = { GL_TRIANGLES,GL_LINES,GL_POINTS };
const uint32 OGLMaxDirLitNum = 8;
//const uint32 OGLBASEDATETYPE[DATA_TYPE_NUM] = {GL_NONE,GL_BYTE,GL_SHORT,GL_INT,GL_UNSIGNED_BYTE,GL_UNSIGNED_SHORT,GL_UNSIGNED_INT,GL_HALF_FLOAT,GL_FLOAT,GL_NONE};
class GLTexture;
class GLVbo;
class GLShader;

class GLUtils {
public:
	static void clearBuffers(float r, float g, float b, float a = 1.0);
	static void clearDepth();
	static void  render2Quad(std::shared_ptr<Texture> tex);
	static void init();
	static void checkGLError(std::string tag);
	static uint32 getGLBaseType(DType baseType);
	static std::shared_ptr<GLVbo> quadVbo;
	static std::shared_ptr<GLShader> texShader;
	static void finish() {
		glFinish();
	}
};

class OGLGlobalRenderState: public RenderState {
public:
	virtual void saveRenderState();
	virtual void recoverRenderState();
private:
	GLboolean enableDepthTest, enableBlend, enableCullFace, enableStencilTest, enableScissorTest;
	GLenum blendSrcRGB, blendDstRGB, blendSrcAlpha, blendDstAlpha, depthTestFunc;
	
};

struct OGLDirLits {
	struct DirLit {
		Vec4f dir;
		Vec4f radiance;
	}lits[OGLMaxDirLitNum];
	int32 litNum;
};

#define  GL_IVALID_UINT32  -1

#if IS_WINDOWS || IS_MACOS

const uint32 glwKeyboardMaps[] = {
	GLFW_KEY_ESCAPE,
	GLFW_KEY_W,
	GLFW_KEY_A,
	GLFW_KEY_S,
	GLFW_KEY_D,
	GLFW_KEY_Q,
	GLFW_KEY_F,
	GLFW_KEY_G,
	GLFW_KEY_I,
	GLFW_KEY_K,
	GLFW_KEY_J,
	GLFW_KEY_L,

	GLFW_KEY_0,
	GLFW_KEY_1,
	GLFW_KEY_2,
	GLFW_KEY_3,
	GLFW_KEY_4,
	GLFW_KEY_5,
	GLFW_KEY_6,
	GLFW_KEY_7,
	GLFW_KEY_8,
	GLFW_KEY_9,

	GLFW_KEY_U,

	GLFW_KEY_SPACE,
};

const uint32 glwMouseButtonMaps[]{
	GLFW_MOUSE_BUTTON_LEFT,
	GLFW_MOUSE_BUTTON_RIGHT,
	GLFW_MOUSE_BUTTON_MIDDLE,
};

#endif

class OGLRenderStateFactory : public RenderStateFactory {
public:
	 virtual pRenderStateSetting createRenderState(const std::string& settingType, const std::vector<std::string> params);

};

END_P3D_NS
