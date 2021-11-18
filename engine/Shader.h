#pragma once

#include"common/P3D_Utils.h"
#include "common/RenderCommon.h"
#include "common/mathIncludes.h"
#include "Texture.h"
#include "Camera.h"
#include "p3dVertexBuffer.h"
#include "shaderUniform.hpp"
BEGIN_P3D_NS
class Shader :public P3DObject{
public:
	virtual ~Shader() {};
	static std::shared_ptr<Shader> make(uint t);
    virtual bool load(const std::string& vs,const std::string& ps) = 0;
	virtual bool loadComputeShader(const std::string& cs) = 0;

	virtual bool loadFromFile(const std::string& vsPath,const std::string& psPath) = 0;
    virtual void setUniformF(const std::string& key,float v) = 0;
	virtual void addUniform(pShaderUniform tUniform) = 0;

    virtual void setUniformF2(const std::string& key,const Vec2f& v) = 0;
    virtual void setUniformF3(const std::string& key,const Vec3f& v) = 0;
    virtual void setUniformF4(const std::string& key,const Vec4f& v) = 0;

    virtual void setUniformFv(const std::string& key,const std::vector<float32>& v ) = 0;
    virtual void setUniformI(const std::string& key,int32 v) = 0;
    virtual void setUniformIv(const std::string& key,const std::vector<int32>& v) = 0;
	virtual void setUniformMat(const std::string& key, const Mat4f& m) = 0;
	virtual void setUniformMats(const std::string& key, const std::vector<Mat4f>& mats) = 0;
	virtual void setUniformF3s(const std::string&key, const std::vector<Vec3f>& vs) = 0;

    virtual void setUniformTex(const std::string& key, pTexture pTex) = 0;
	virtual void setUniformBuffer(const std::string&key, pVertexBuffer pBuffer, uint32 bindingPoint) = 0;
	virtual void setCamera(pCamera tCamera) = 0;

	virtual void setTextureImg(pTexture tTex, int32 bindId,int32 mipLevel=0) = 0;
	virtual void run(const Vec3<uint32>& tSize) = 0;
    
    virtual void applay() = 0;
    virtual void applay0() = 0;
	virtual void reload() = 0;
	virtual void clearUniforms() = 0;
protected:
	bool isCs = false;
	std::string vsPath;
	std::string fsPath;

};
typedef std::shared_ptr<Shader> pShader;
END_P3D_NS