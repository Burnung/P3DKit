#pragma once
#
#include "common/RenderCommon.h"
#include "engine/P3dEngine.h"
#include "common/NdArray.h"
#include "engine/renderTechnique.h"
#include "engine/Window.h"
BEGIN_P3D_NS


class GeneralRenderTech :public P3DObject {
public:
	GeneralRenderTech();
	virtual ~GeneralRenderTech() {
	
	}

public:
	void setImgSize(int w, int h);
	void setCamTranslate(std::vector<float>& srMat, std::vector<float>& t);

	void addShader(const std::string& shaderName, const std::string& vsPath, const std::string& psPath);
	
	void addHair(const std::string& nodeName, const std::string& hairPath, const std::string& shaderName);

	void addComp(const std::string& nodeName, const std::string& objPath, const std::string& shaderName);
	//void addComp(const std::string& nodeName, PNdArrayF vps, PNdArrayF tl,const std::string& shaderName);
	void addCompData(const std::string& nodeNamt, PNdArrayF vpsData, PNdArrayF tlsData,PNdArrayF uvs, const std::string& shaderName,PNdArrayF uvTl);

	void setNodeTranslate(const std::string& nodeName, const std::vector<float>& srMat, const std::vector<float>& t);
	void setCamFov(float fov);

	void addTexture(const std::string& texName, PNdArrayU8 tex);
	void setShaderTex(const std::string& shaderName,const std::string& texName,const std::string& uniformName);
	void setShaderFloat(const std::string& shaderName, const std::string& uniformName, float v);

	void clearScene();

	void setShowWin(bool showW) { showWin = showW; }


	PNdArrayF render();
	void renderWin();


private:
	int imgW, imgH;
	float camFov = 1.0 / PI * 180.0;
	bool showWin = false;

	pWindow mWin;
	pScene mScene;
	pCamera mCam;
	pRenderFrame mFbo;

	std::unordered_map<std::string, pShader> shaders;
	std::unordered_map<std::string, pTexture> texs;
	std::set<std::string> nodes;

};


END_P3D_NS
