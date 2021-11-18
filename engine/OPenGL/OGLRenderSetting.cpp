#include"OGLCommon.h"

BEGIN_P3D_NS

class OGLBlendEnableSetting : public RenderStateSetting {
public:
	OGLBlendEnableSetting(const std::vector<std::string>& parmas) {
		isAble = parmas[0] == "enable" ? true : false;
	}
	virtual void apply() {
		if (isAble)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}
private:
	bool isAble;
};

class OGLCullFaceEnableSetting :public RenderStateSetting {
public:
	OGLCullFaceEnableSetting(const std::vector<std::string>& params) {
		isAble = params[0] == "enable" ? true : false;
	}
	virtual void apply() {
		if (isAble)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}
private:
	bool isAble;
};
class OGLDepthEnableSetting :public RenderStateSetting {
public:
	OGLDepthEnableSetting(const std::vector<std::string>& params) {
		isAble = params[0] == "enable" ? true : false;
	}
	virtual void apply() {
		if (isAble)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
private:
	bool isAble;
};
pRenderStateSetting OGLRenderStateFactory::createRenderState(const std::string& settingType, const std::vector<std::string> params) {
	if (settingType == "blend")
		return std::make_shared<OGLBlendEnableSetting>(params);
	if (settingType == "depth")
		return std::make_shared<OGLDepthEnableSetting>(params);
	if (settingType == "cull_face")
		return std::make_shared<OGLCullFaceEnableSetting>(params);
}
END_P3D_NS