
#pragma once
#include "vraysdk.hpp"
#include "vrayplugins.hpp"
#include "common/mathIncludes.h"
#include "geo/hairModel.h"
#include "geo/comp.h"

BEGIN_P3D_NS
class VrayRender : public P3DObject {
public:
	enum {
		COLOR_SPACE_LINEAR=0,
		COLOR_SPACE_GAMMA,
		COLOR_SPACE_SRGB,
	};
	enum {
		LIGHT_SPHERE,
		LIGHT_OMNI,
		LIGHT_RECT,
		LIGHT_SPOT,
	};
	enum {
		CAMERA_TYPE_PERSPECTIVE,
		CAMERA_TYPE_ORTHOGONAL,
		CAMERA_TYPE_NUM,
	};

public:
	VrayRender() = default;
	virtual ~VrayRender();

public:

	bool init(uint32 w = 720,uint32 h = 720, bool use_window = false);
	
	bool loadHairMtl(const std::string& matPath);

	void loadScene(const std::string& vrayScenePath);
	void clearScene();

	void setImgSize(uint32 w, uint32 h);

	bool render(const std::string& imgFilePath, int32 timeMax = -1,const std::string& idPath = "") ; // save img
	bool renderImg(const std::string& imgPath, int32 timeMax = -1, float32 gamma = 2.2);

	bool renderAll();

	PNdArray getChannel(const std::string& cName);

	void addRenderChannel(const std::string& cName);
	PNdArrayF renderRaw(int timeMax = -1);

	void setModelEuler(float32 pitch, float32 yaw, float32 roll, float32 x, float32 y, float32 z);
	
	void setCameraPos(float32 x, float32 y, float32 z,float32 ux=0.0f,float32 uy=1.0f,float32 uz=0.0f);  

	void setNodeTranslation(const std::string& nodeName, const std::vector<float32>& rotScale,const std::vector<float32>& trans);
	void setNodeMaterial(const std::string& nodeName, const std::string& mtlName);
	void setCamMat(const std::vector<float32>& rotMat,const std::vector<float32>& camPos);
	void setCameraFov(float32 fov);

	void setHairNode(const std::string& nodeName, const std::string& hairPath, float32 srcWidth = 0.0005);

	void setMeshNode(const std::string& nodeName, const std::string& meshPath);

	//void setHairNodeData(const std::string& nodeName, PNdArrayF hairData, float32 srcWidth = 0.0005);
	void setMeshNodeData(const std::string& nodeName, P3D::PNdArrayF vps, P3D::PNdArrayF vns,P3D::PNdArrayF uvs,P3D::PNdArrayF tls,P3D::PNdArrayF uvTls,bool calcN);

	void setHair(const std::string& hairPath,float32 srcWidth=0.0005);


	VRay::Plugins::MtlSingleBRDF getVrayMtl(const std::string&mtlName, const std::vector<float>& color);


	void setEnvironmentTex(const std::string& img_path, float32 intensity = 1.0, const std::vector<float32> &rot = {}, bool noBack = false,bool asLight = true);
	void setDomeLightTex(const std::string& img_path,bool useSepcular = true);
	void setDomeLightEnable(bool enable);
	void setTexture(const std::string& img_path,const std::string bitMapName,const std::string& texNodeName,int colorSpace=COLOR_SPACE_LINEAR);

	void setEnvironmentColor(const std::vector<float32>& color);

//	void addLight(int32 type, const string& name, const std::vector<float>& pos, const std::vector<float>&rot, const std::vector<float>& color, float32 intensity, const std::vector<float>& extraData) {
	void addLight(uint32 type, const std::string& name, const std::vector<float32>& pos, const std::vector<float32>& rot, const std::vector<float32>& color, float32 intensity = 1.0, const std::vector<float32>&extraData = {});
	void setLight(uint32 type, const std::string& name, const std::vector<float32>&color, const std::vector<float32>& pos, float intensity, const std::vector<float32>& extraData = {});
	void clearLights();

	bool exportScene(const std::string& filePath);
	void exportHair(const std::string& filePath);
	void setCameraType(uint32 t,float32 orthWidth = 0.6);
	void setOrthoWidth(float32 w);
	void dumpHair(const std::string& dstPahth,const std::vector<std::string>& hairNames);
	std::string addObject(const std::string& fPath);// , const std::string& alinPos, const std::vector<float>& pos = { 0.0,0.0,0.0 });
	
	void setSceneEuler(const std::vector<float32>& euler);
	void addMotionSqueeze(const std::vector<std::vector<float>>& eulerS);
	void renderSqueeze(const std::string& dirPath, int start, int end, int step = 1, bool mBlur = true);
	void debugJust();

	//laod obj mesh pathes[0] is base mesh , 1,2..n are the blendshapes
	void loadComp(const std::vector<std::string > &pathes);
	void updateModel(const std::string& nodeName, const std::vector<float32>& bsCoeff);

	void addMeshNode(const std::string& nodeName, pComp tComp);
	void setNodeVisiable(const std::string& nodeName, bool visiable);
protected:
	void setVrayTexByName(const std::string& bitMapName, const std::string& texName, const std::string& imgPath, int32 colorSpace);
	void initRenderSetting();
	void initHairNode();
	VRay::Matrix convertMat2VrayMatrix(const Mat4f& mat);
	
	Mat4f getMatFromVray(VRay::Transform);
	VRay::Plugins::GeomMayaHair convertCommonHair2Vray(std::shared_ptr<HairModel> hair);
	void updateHair(const std::string&hairNode,std::shared_ptr<HairModel> hairP,float32 srcWidth=0.0005);
	void setModelsTransform(const VRay::Transform& trans);
	VRay::Plugins::GeomStaticMesh convertComp2Vray(pComp);

protected:
	int32 imgW, imgH;
	uint32 cameraType = CAMERA_TYPE_ORTHOGONAL;
	Mat4f modelMat;  //
	Vec3f camPos = Vec3f(0.0f,0.0f,5.0f);
	std::shared_ptr<VRay::VRayRenderer> vrayRender;
	std::shared_ptr<VRay::VRayInit> vrayIniter;
	std::vector<std::string> objNames;
	std::vector<std::string> renderChannels;
	pComp mComp = nullptr;
	int nFrame = 0;
};
END_P3D_NS
