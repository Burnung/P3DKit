#include <unordered_map>
#include "../Shader.h"
#include "../Texture.h"
#include "OGLCommon.h"
#include"../../common/P3D_Utils.h"
#include "../Camera.h"
#pragma once
BEGIN_P3D_NS
class GLShader :public Shader{
public:
    GLShader()=default;
    virtual ~GLShader();
public:
    virtual bool load(const std::string& vs,const std::string& ps);
    virtual bool loadFromFile(const std::string& vsPath,const std::string& psPath);
	virtual bool loadComputeShader(const std::string& cs);

    virtual void setUniformF(const std::string& key,float v);

    virtual void setUniformF2(const std::string& key,const Vec2f& v);
    virtual void setUniformF3(const std::string& key,const Vec3f& v);
    virtual void setUniformF4(const std::string& key,const Vec4f& v);

    virtual void setUniformFv(const std::string& key,const std::vector<float32>& v );
    virtual void setUniformI(const std::string& key,int32 v);
    virtual void setUniformIv(const std::string& key,const std::vector<int32>& v);
	virtual void setUniformMat(const std::string& key, const Mat4f&);
	virtual void setUniformMats(const std::string& key, const std::vector<Mat4f>& vals);
	virtual void setUniformF3s(const std::string&key, const std::vector<Vec3f>& vs);
	virtual void setUniformBuffer(const std::string&key, pVertexBuffer pBuffer, uint32 bindingPoint);
	virtual void addUniform(pShaderUniform tUniform) {
		uniforms[tUniform->getLocName()] = tUniform;
	}

    virtual void setUniformTex(const std::string& key, pTexture pTex);
	virtual void setCamera(pCamera tCamera);
	virtual void applay();
	virtual void applay0();
	virtual void run(const Vec3<uint32>& tSize);

	virtual void setTextureImg(pTexture tTex, int32 bindId,int mipLevel = 0);
	virtual void reload();

	int32 getAttriLoc(const std::string& key);
	virtual void clearUniforms() {
		uniforms.clear();
	}
    
private:
	void applayCamera();
	void applyFreeUniform();
private:
    uint32 programId = GL_IVALID_UINT32;
	pCamera mCamera = nullptr;
	uint32 appCount = 0;
    
	std::unordered_map<std::string, std::pair<uint32, pVertexBuffer>> uniformBuffer;
	std::unordered_map<int32, std::pair<pTexture,int32>> uniformTexImages;

	std::unordered_map<std::string,pShaderUniform> uniforms;
};

typedef std::shared_ptr<GLShader> pGLShader;
END_P3D_NS


