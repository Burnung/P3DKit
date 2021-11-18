#include "common/RenderCommon.h"
#include "engine/P3dEngine.h"
#include "common/NdArray.h"
#include "engine/renderTechnique.h"
BEGIN_P3D_NS
class HairAttrisTech :public P3DObject {
public:
	HairAttrisTech();
	virtual ~HairAttrisTech() {};
public:
	void init(const std::string& hairPath,float w,float h,float orthW,int32 nFbo = 1);
	 PNdArrayF render(std::vector<float> camPos);
	pRenderFrame renderTech(const std::vector<float>& camPos);
public:
	int32 nFbo = 1;
	float32 w = 512, h = 512;
	pWindow win;
	pCamera cam;
	pRenderFrame fbo;
	pTechnique pTech;
};



END_P3D_NS