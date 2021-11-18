#include"OGLCommon.h"
#include "OGLVbo.h"
#include "OGLShader.h"
#include "OGLTexture.h"
BEGIN_P3D_NS

struct GLPixelFormatDescription {
        uint32 format;
        uint32 type;
        uint32 internalFormat;
};

static GLPixelFormatDescription glPixelFormats[PIXEL_FORMAT_COUNT] = {
    {GL_NONE},
    {GL_RED,GL_UNSIGNED_BYTE,GL_R8},
    {GL_RG,GL_UNSIGNED_BYTE,GL_RG8},
    {GL_RGB,GL_UNSIGNED_BYTE,GL_RGB8},
    {GL_RGBA,GL_UNSIGNED_BYTE,GL_RGBA8},
    {GL_RGB,GL_FLOAT,GL_RGB32F},
    {GL_RGBA,GL_FLOAT,GL_RGBA32F},
    {GL_DEPTH_COMPONENT,GL_FLOAT,GL_DEPTH_COMPONENT32F},

    {GL_BGR,GL_UNSIGNED_BYTE,GL_RGB8},
    {GL_BGR,GL_FLOAT,GL_RGB32F},
    {GL_BGRA,GL_UNSIGNED_BYTE,GL_RGBA8},
    {GL_BGRA,GL_FLOAT,GL_RGBA32F},
};

const char* ATTRINAME[] = { "InPos","InUv","InVn" };
uint32 GLPixelUtil::getGLOriginFormat(PixelFormat format){
    return glPixelFormats[format].format;
}

uint32 GLPixelUtil::getGLOriginDataType(PixelFormat format){
    return glPixelFormats[format].type;
}

uint32 GLPixelUtil::getGLInternalFormat(PixelFormat format){
    return glPixelFormats[format].internalFormat;
}
std::shared_ptr<GLVbo> GLUtils::quadVbo = nullptr;
std::shared_ptr<GLShader> GLUtils::texShader = nullptr;
namespace GL_SHADER_STR {
	const std::string quadVS = R"GL_SHADER(
#version 410
layout(location = 0) in vec3 InPos;
layout(location =0) out vec2 uv;
void main(){
	gl_Position = vec4(InPos,1.0);
	uv = vec2(0.5 * InPos.x + 0.5,0.5 * InPos.y + 0.5);
}
)GL_SHADER";
	const std::string quadFS = R"GL_SHADER(
	#version 410
	layout(location = 0) in vec2 uv;
	layout(location = 0) out vec4 outColor;
	uniform int texW;
	uniform int texH;
	uniform int nSample;
	uniform sampler2D tex;
void main(){
	vec3 tColor;
	tColor = texture(tex,uv).rgb;
	outColor =  vec4(tColor,1.0);
	//outColor =  vec4(uv,0.0,1.0);
}
)GL_SHADER";
}
void  GLUtils::init() {
	if (quadVbo && texShader)
		return;
	if (!texShader) {
		texShader = std::make_shared<GLShader>();
		texShader->load(GL_SHADER_STR::quadVS,GL_SHADER_STR::quadFS);
	}
	if (!quadVbo) {
		quadVbo = std::make_shared<GLVbo>();
		quadVbo->setShader(texShader);
		std::vector<Vec3f> vps = { {-1.0f,-1.0f,0.0f},{1.0f,-1.0f,0.0f},{1.0f,1.0f,0.0f},{-1.0f,1.0f,0.0f} };
		std::vector<uint8> tl = { 0,1,2,2,3,0 };
		auto vpsBlock = std::make_shared<DataBlock>(sizeof(Vec3f) * vps.size(),DType::Float32);
		vpsBlock->setFromMem((uint8*)vps.data(), vpsBlock->getSize());
		auto tlBlock = std::make_shared<DataBlock>(tl.size() * sizeof(uint8),DType::UInt8);
		tlBlock->setFromMem((uint8*)tl.data(), tlBlock->getSize());
		quadVbo->init(vpsBlock, tlBlock, nullptr, nullptr);
	}
}
void  GLUtils::render2Quad(std::shared_ptr<Texture> tex) {
	GLUtils::clearBuffers(0.0, 0.0, 0.0, 1.0);
	if (tex->getSsa() > 1) {
		texShader->setUniformTex("texMS", tex);
		texShader->setUniformI("texW", tex->Width(0));
		texShader->setUniformI("texH", tex->Height(0));
	}
	else
		texShader->setUniformTex("tex", tex);
	texShader->setUniformI("nSample", tex->getSsa());
	quadVbo->draw();
}

void GLUtils::clearDepth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}
void  GLUtils::clearBuffers(float r, float g, float b, float a ) {
	glClearColor(r , g, b, a);
	glClearDepthf(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

uint32 GLUtils::getGLBaseType(DType baseType) {

	static uint32 oglBaseType[] = {GL_NONE,GL_FLOAT,GL_INT,GL_UNSIGNED_BYTE,GL_INT64_ARB,GL_SHORT,GL_DOUBLE,GL_UNSIGNED_SHORT,GL_UNSIGNED_INT};
	static int cnt = sizeof(oglBaseType) / sizeof(uint32);
	if(int(baseType) >=0 && int(baseType) < cnt)
		return oglBaseType[int(baseType)];
	return GL_NONE;
	
}
void GLUtils::checkGLError(std::string tag) {
	GLenum errorCode;
	if ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM: // 1280
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE: // 1281
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION: // 1282
			error = "INVALID_OPERATION";
			break;
		case GL_OUT_OF_MEMORY: // 1285
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: //1286
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		logOut.log(std::string("GLUtils::checkGLError find error! tag = ") + tag + " error = " + error + "\n");
	}
	else
		logOut.log(std::string("GLUtils::checkGLError tag = ") + tag + " no error!\n");

}

void OGLGlobalRenderState::saveRenderState() {
	//depth test
	enableBlend = glIsEnabled(GL_BLEND);
	enableDepthTest = glIsEnabled(GL_DEPTH_TEST);
	enableCullFace = glIsEnabled(GL_CULL_FACE);
    enableStencilTest = glIsEnabled(GL_STENCIL_TEST);
    enableScissorTest = glIsEnabled(GL_SCISSOR_TEST);

	glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&blendSrcRGB);
	glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&blendDstRGB);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&blendSrcAlpha);
	glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&blendDstAlpha);

	glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&depthTestFunc);

	//glEnable(GL_BLEND);
}
void OGLGlobalRenderState::recoverRenderState() {
	if (enableBlend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	
	if (enableDepthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (enableCullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if (enableStencilTest)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);

	if (enableStencilTest)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);

	glBlendFuncSeparate(blendSrcRGB, blendDstRGB, blendSrcAlpha, blendDstAlpha);
	glDepthFunc(depthTestFunc);
}

END_P3D_NS
