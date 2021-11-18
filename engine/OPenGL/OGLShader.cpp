#include "OGLShader.h"
#include "common/P3D_Utils.h"
#include <iostream>
#include "../P3dEngine.h"
#include "../mgs/sceneManager.hpp"

using namespace std;

BEGIN_P3D_NS
GLShader::~GLShader() {
	if (programId != GL_IVALID_UINT32)
		glDeleteProgram(programId);
}
void GLShader::reload() {
	if (vsPath.empty() || fsPath.empty())
		return;
	if (programId != GL_IVALID_UINT32)
		glDeleteProgram(programId);
	programId = GL_IVALID_UINT32;
	loadFromFile(vsPath, fsPath);
}
bool GLShader::load(const std::string& vs,const std::string& ps){
    uint32 vsId = glCreateShader(GL_VERTEX_SHADER);
    int tl = vs.length();
    GLchar* tmpVs[] = {(GLchar*)vs.c_str()};
    glShaderSource(vsId,1,tmpVs,&tl);
    glCompileShader(vsId);
    int32 status;
    glGetShaderiv(vsId,GL_COMPILE_STATUS,&status);
    if(status != GL_TRUE){
        int len = 0;
        glGetShaderiv(vsId, GL_INFO_LOG_LENGTH, &len);
        char* infoLog = new char[len + 1];
        glGetShaderInfoLog(vsId,len + 1,&len,infoLog);
        cout<<infoLog<<endl;
        delete[] infoLog;
		return false;
      //  throw invalid_argument(Utils::makeStr("error vs Shader"));
    }
    uint32 psId = glCreateShader(GL_FRAGMENT_SHADER);
    tl = ps.length();

    GLchar* tmpPs[] = {(GLchar*)ps.c_str()};
    glShaderSource(psId,1,tmpPs,&tl);
    glCompileShader(psId);
    glGetShaderiv(psId,GL_COMPILE_STATUS,&status);
    if(status != GL_TRUE){
        int len = 0;
        glGetShaderiv(psId, GL_INFO_LOG_LENGTH, &len);
        char* infoLog = new char[len + 1];
        glGetShaderInfoLog(psId,len + 1,&len,infoLog);
        cout<<infoLog<<endl;
        delete infoLog;
		return false;
       // throw invalid_argument(Utils::makeStr("error ps Shader"));
    }

    this->programId =  glCreateProgram();
    glAttachShader(this->programId,vsId);
    glAttachShader(this->programId,psId);
    glLinkProgram(this->programId);

    glGetProgramiv(this->programId,GL_LINK_STATUS,&status);
    if(status != GL_TRUE){
        int32 len = 0;
        glGetProgramiv(this->programId,GL_INFO_LOG_LENGTH,&len);
        DataBlock t(len+ 1);
        glGetProgramInfoLog(this->programId,len+1,&len,t.rawData<char>());
        cout<<t.rawData<char>()<<endl;
		return false;
       // throw invalid_argument(Utils::makeStr("error link program"));
    }
    glDeleteShader(vsId);
    glDeleteShader(psId);
    return true;
}
bool GLShader::loadFromFile(const std::string& vsPath,const std::string& psPath){
	this->vsPath = vsPath;
	this->fsPath = psPath;
    auto vs = Utils::flattenFileStr(vsPath);
    auto fs = Utils::flattenFileStr(psPath);
    return load(vs,fs);
}
bool GLShader::loadComputeShader(const std::string& csPath) {
	auto cs = Utils::readFileString(csPath);
	uint32 csId = glCreateShader(GL_COMPUTE_SHADER);
	int tl = cs.length();
	GLchar* tmpCs[] = { (GLchar*)cs.c_str() };
	glShaderSource(csId, 1, tmpCs, &tl);
	glCompileShader(csId);
	int32 status;
	glGetShaderiv(csId, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		int len = 0;
		glGetShaderiv(csId, GL_INFO_LOG_LENGTH, &len);
		char* infoLog = new char[len + 1];
		glGetShaderInfoLog(csId, len + 1, &len, infoLog);
		cout << infoLog << endl;
		delete[] infoLog;
		throw invalid_argument(Utils::makeStr("error cs Shader"));
	}
	this->programId = glCreateProgram();
	glAttachShader(this->programId, csId);
	glLinkProgram(this->programId);

	glGetProgramiv(this->programId, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		int32 len = 0;
		glGetProgramiv(this->programId, GL_INFO_LOG_LENGTH, &len);
		DataBlock t(len + 1);
		glGetProgramInfoLog(this->programId, len + 1, &len, t.rawData<char>());
		cout << t.rawData<char>() << endl;
		throw invalid_argument(Utils::makeStr("error link cs program"));
	}
	glDeleteShader(csId);
	isCs = true;
}
void GLShader::setUniformF(const std::string& key,float v){
	addUniform(std::make_shared<ShaderUniform>(key, v));
}

void GLShader::setUniformF2(const std::string& key,const Vec2f& v){
	addUniform(std::make_shared<ShaderUniform>(key, v));
}

void GLShader::setUniformF3s(const std::string&key, const std::vector<Vec3f>& vs) {
	addUniform(std::make_shared<ShaderUniform>(key, vs));
}

void GLShader::setUniformF3(const std::string& key,const Vec3f& v){
	addUniform(std::make_shared<ShaderUniform>(key, v));
}

void GLShader::setUniformF4(const std::string& key,const Vec4f& v){
	addUniform(std::make_shared<ShaderUniform>(key, v));
}

void GLShader::setUniformFv(const std::string& key,const std::vector<float32>& v ){
	addUniform(std::make_shared<ShaderUniform>(key, v));
}
void GLShader::setUniformI(const std::string& key,int32 v){
	addUniform(std::make_shared<ShaderUniform>(key, v));
}
void GLShader::setUniformIv(const std::string& key,const std::vector<int32>& v){
	addUniform(std::make_shared<ShaderUniform>(key, v));
}
void GLShader::setUniformTex(const std::string& key, pTexture pTex){
	addUniform(std::make_shared<ShaderUniform>(key, pTex));
}

void GLShader::applay(){
	glUseProgram(programId);
	applayCamera();
  
	for (const auto& item : uniformBuffer) {
		auto tLoc = glGetUniformBlockIndex(this->programId, item.first.c_str());
		if (tLoc != -1) {
			std::get<1>(item.second)->applay(std::get<0>(item.second));
			glUniformBlockBinding(this->programId, tLoc, std::get<0>(item.second));
		}
	}
	applyFreeUniform();

}

void GLShader::applyFreeUniform() {
	appCount = 0;
	for (auto& tk : uniforms) {
		auto uni = tk.second;
		auto loc = glGetUniformLocation(programId, tk.first.c_str());
		if(loc ==-1)
			continue;
		auto tType = uni->getDataType();
		if (tType == ShaderUniform::TYPE_INT) {
            glUniform1i(loc,*(int*)uni->getData());
		}
		else if (tType == ShaderUniform::TYPE_FLOAT) {
			float v = *(float*)uni->getData();
            glUniform1f(loc,*(float*)uni->getData());
		}
		else if (tType == ShaderUniform::TYPE_VEC2) {
			auto tData = (Vec2f*)uni->getData();
			glUniform2f(loc, tData->x, tData->y);
		}
		else if (tType == ShaderUniform::TYPE_VEC3) {
			auto tData = (Vec3f*)uni->getData();
			glUniform3f(loc, tData->x, tData->y,tData->z);
		}
		else if (tType == ShaderUniform::TYPE_VEC4) {
			auto tData = *(Vec4f*)uni->getData();
			glUniform4f(loc, tData.x,tData.y,tData.z,tData.w);
		}
		else if (tType == ShaderUniform::TYPE_VEC3S) {
			glUniform3fv(loc, uni->getDataCount(), (const GLfloat*)uni->getData());
		}
		else if (tType == ShaderUniform::TYPE_VEC4S) {
			glUniform4fv(loc, uni->getDataCount(),(const GLfloat*) uni->getData());
		}
		else if (tType == ShaderUniform::TYPE_MAT4S) {
			glUniformMatrix4fv(loc, uni->getDataCount(),GL_TRUE,(const GLfloat*) uni->getData());
		}
		else if (tType == ShaderUniform::TYPE_MAT4) {
			glUniformMatrix4fv(loc, uni->getDataCount(),GL_TRUE,(const GLfloat*) uni->getData());
		}
		else if (tType == ShaderUniform::TYPE_TEX) {
			auto pTex = (Texture*)uni->getData();
			pTex->applyShader(appCount);
			glUniform1i(loc, appCount);
			appCount++;
		}
		else if (tType == ShaderUniform::TYPE_FLOATS) {
			glUniform1fv(loc, uni->getDataCount(), (const GLfloat*)uni->getData());
		}
		else if (tType == ShaderUniform::TYPE_INTS) {
			glUniform1iv(loc, uni->getDataCount(), (const GLint*)uni->getData());
		}
		else if (tType == ShaderUniform::TYPE_TEX_NAME) {
			const char* texName = (char*)uni->getData();
			auto pTex = P3DEngine::instance().getSceneManager()->getTex(texName);
			pTex->applyShader(appCount);
			glUniform1i(loc, appCount);
			appCount++;
		}

	
	}
}
void  GLShader::setTextureImg(pTexture tTex, int32 bindId,int32 mipMapLevel) {
	uniformTexImages[bindId] = { tTex,mipMapLevel };
}

void GLShader::applayCamera() {
	using namespace SHADER_KEY_WORDS;
	if (!mCamera)
		return;
	if (mCamera->getType() == Camera::CAMERA_TYPE_PRO) {
		setUniformMat(PRO_MAT, mCamera->getProjMatrix());
		setUniformMat(VIEW_MAT, mCamera->getViewMatrix());
		setUniformMat(MVP, mCamera->getViewProjMatrix());
		setUniformF3(CAMERA_POS, mCamera->getEyePos());
	}
	else if(mCamera->getType() == Camera::CAMERA_TYPE_ORTH) {
		setUniformF3(CAMERA_POS, mCamera->getEyePos());
		setUniformF3(CAMEERA_DIR, mCamera->getLookVec());
		setUniformMat(VIEW_MAT, mCamera->getViewMatrix());
		setUniformMat(PRO_MAT, mCamera->getOrthMatrix());
		setUniformMat(MVP, mCamera->getViewOrthMatrx());
	}
}
void GLShader::applay0() {
	glUseProgram(0);
}
void GLShader::setUniformMat(const std::string& key, const Mat4f& tMat) {
	addUniform(std::make_shared<ShaderUniform>(key, tMat));
}
void GLShader::setUniformMats(const std::string& key, const std::vector<Mat4f>& vals) {
	addUniform(std::make_shared<ShaderUniform>(key, vals));
}
int32 GLShader::getAttriLoc(const std::string& key) {
	return glGetAttribLocation(this->programId, key.c_str());
}

void GLShader::setCamera(pCamera tCamera) {
	mCamera = tCamera;
}
void GLShader::setUniformBuffer(const std::string&key, pVertexBuffer pBuffer, uint32 bindingPoint) {
	uniformBuffer[key] = { bindingPoint,pBuffer };
}
void GLShader::run(const Vec3<uint32>& tSize) {
	if (!isCs)
		return;
	applay();
	for (const auto& texImg : uniformTexImages) {
		auto tTex = std::get<0>(texImg.second);
		auto mipLevel = std::get<1>(texImg.second);
		//auto glUsage = tTex->getBufferUsage();
		auto baseType = GLPixelUtil::getGLInternalFormat(tTex->getFormat());
		glBindImageTexture(texImg.first, tTex->getHandler(), mipLevel, GL_TRUE, 0, GL_WRITE_ONLY, baseType);
	}
	glDispatchCompute(tSize.x, tSize.y, tSize.z);
	GLUtils::checkGLError("run compute Shader");
	applay0();

}
END_P3D_NS
