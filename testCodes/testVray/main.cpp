
#include<iostream>
#include "../render/vrayRender.h" 
#include <string>
#include "common/NdArray.h"
#include "../render/vrayRenderController.h"
using namespace P3D;
using namespace std;


int main(){
	
	auto render = std::make_shared<VrayRender>();
	render->init(720, 1280, true);
	//auto hairPath = R"(D:\Work\data\Hair\hairstyles\hairstyles\strands00269.data)";
	//auto hairPath = R"(D:\Work\data\Hair\hairstyles\hairstyles\strands00001.data)";
	//auto hairPath = R"(D:\Work\Projects\ProNew\Pixar\hair\hair-modeling\resource\smooth.data)";
	//auto hairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\hair.data)";
//	auto hairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\dst_hair.data)";
	auto hairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\hair\hair_data0000_n.data)";
	auto headPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\head_new.obj)";
	//auto hairPath = R"(D:\Work\Projects\ProNew\render_research\tmp\hair.data)";
//	auto hairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairAttri\redshift\hair_mid.data)";
//	auto hairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairAttri\redshift\hair_big.data)";
	auto saveHairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairAttri\menglu\hair_simple_yellow_gai.data)";
	//auto mengliScene = R"(D:\Work\Projects\ProNew\Vray\tmp\vrayScene\menglu\bighair_yellow_v2\hair_big.vrscene)";
//	auto mengliScene = R"(D:\Work\Projects\ProNew\Vray\tmp\vrayScene\menglu\yelow_simple\hair_simple.vrscene)";
	auto mengliScene = R"(D:\Work\Projects\ProNew\Vray\tmp\vrayScene\menglu\menglu_simple_yellow_gai1\hair_simple.vrscene)";
	auto buddhaPath = R"(D:\Work\data\Render\nnRenderScene\max\buddha\buddha_vray\buddha_yu.vrscene)";


	//auto hairPath = R"(D:\Work\Projects\ProNew\render_research\tmp\simple_hair_5000.data)";
	//auto hairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\simple_hair_1000.data)";
	auto saveImgPath = R"(tmp\save_filter_60_color.png)";
	auto miniScene = R"(tmp\mini.vrscene)";
	auto envTexPath = R"(D:\Work\data\v-ray\flame\envTex\yellow_field_4k.hdr)";
	auto hairMtlPath = R"(D:\Work\Projects\ProNew\Vray\tmp\vrayScene\hairMtl.vrscene)";
	auto faceMtlPath = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\renderer\vray_scene\mtl_lit_1\vary_materials.vrscene)";
	
	auto emilyPath = R"(D:\Work\data\v-ray\emily\vray_scene\flame.vrscene)";
	auto hairScene = R"(C:\Users\zhang\Desktop\nineW\hair_simple\hair.vrscene)";
	auto saveImgPathNew = R"(tmp\src_sphere.png)";
	auto animaionScene = R"(D:\Work\data\face_data\ldk3dTest\animation.vrscene)";

	auto wangguanPath = R"(D:\Work\data\Render\nnRenderScene\wangguan\evil_mesh.vrscene)";
	auto wangguanSpPath = R"(D:\Work\Projects\ProNew\Vray\tmp\vrayScene\sphere\wangguan_sphere\evil_mesh.vrscene)";
	auto hdrPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testVray\023.hdr)";
	auto bitMapName = "bitmapBuffer_0_g1_0";
	auto texName = "HDR@tex_4";
	auto zhangyu_vray = R"(D:\Work\data\Render\nnRenderScene\max\zhangyu\vray_scene\zhangyu.vrscene)";
	auto zhangyu_vray_all = R"(D:\Work\data\Render\nnRenderScene\max\zhangyu_819\vray_new\zhangyu_819.vrscene)";
	auto zhangyu_826 =	R"(D:\Work\data\Render\nnRenderScene\max\zhangyu_0826_yu\vray_sene\zhangyu_0826_yu.vrscene)";
	
	auto teeth_vray = R"(D:\Work\data\Render\nnRenderScene\max\zhangyu_teeth\teeth_vray\teeth.vrscene)";
	auto teeth_path1 = R"(D:\Work\data\Render\nnRenderScene\max\teeth\teeth_obj\base_zup.obj)";
	auto teeth_path2 = R"(D:\Work\data\Render\nnRenderScene\max\teeth\teeth_obj\exp_24_zup.obj)";
	//auto objHeadPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\ftne10_p00001_net-0150_frame00005\face_world.obj)";
	auto objHeadPath = R"(D:\Work\Projects\ProNew\Vray\tmp\vrayScene\tou4.obj)";
	auto vrayConf = R"(D:\Work\Projects\ProNew\Vray\tmp\vrayController\control_cfg.json)";
	auto kfmPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\hair\kfm_head\obj_hi1_uv.obj)";
	auto facePath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\hair\tmp.obj)";
	auto objPath = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\renderer\vray_scene\outOBJ\pixar_head.obj)";
	//auto pixarV2 = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\renderer\vray_scene\vray_mtl_2\vary.vrscene)";
	auto pixarV2 = R"(D:\Work\Projects\ProNew\Pixar\vrayScene\scene2\vary_s\vary.vrscene)";
	auto leftEye1 = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\renderer\vray_scene\outOBJ\pixar_left_eye_1.obj)";
	auto leftEye2 = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\renderer\vray_scene\outOBJ\pixar_left_eye_2.obj)";

	auto eye_vps = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\style_transfer\vps.npy)";
	auto eye_vns = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\style_transfer\normal.npy)";
	auto eye_uvs = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\style_transfer\uv.npy)";
	auto eye_tl = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\style_transfer\face.npy)";
	auto eye_tl_uv = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\style_transfer\face_uv.npy)";

	auto shadowScene = R"(D:\Work\data\Render\toolScene\shadow_vary\shadow.vrscene)";
	auto vrayCfg = std::make_shared<VrayRenderController>();
	render->loadScene(shadowScene);
	render->setMeshNode("glasses@node", R"(D:\Work\data\Render\toolScene\glass_1.obj)");
	render->render(R"(D:\Work\data\Render\toolScene\shadow_vary\shadow.png)");
	//vrayCfg->initFromConfig(R"(D:\Work\data\Render\mayaScene\vray\xiaoshami\new_scene\vray_scenes\layer1\render_cfg.json)");
//	vrayCfg.reset();
	//vrayCfg = std::make_shared<VrayRenderController>();
	//vrayCfg->initFromConfig(R"(D:\Work\data\Render\mayaScene\vray\xiaoshami\new_scene\vray_scenes\layer1\render_cfg.json)");

	return 0;

	render->loadScene(pixarV2);
//	render->setEnvironmentColor({ 1.0,1.0,1.0,1.0 });
	//render->setEnvironmentTex();
	render->setLight(1, "directionalLightShape1", { 1.0,1.0,0.0 }, { 1,0,0 }, 0.5);
//	render->loadHairMtl(pixarV2);
	//render->setImgSize(512, 512);
//	render->setCameraType(VrayRender::CAMERA_TYPE_PERSPECTIVE);
//	render->setCameraFov(0.5829136 / PI * 180.0);
//	render->setCameraPos(-24.0963, 4.26148, -61.3145);
	
	//render->setMeshNode("bodyShape@node", objPath);
	//render->setNodeMaterial("bodyShape@node", "face_mat@mtl");
	render->setMeshNode("leftEye_1", leftEye1);
	render->setNodeMaterial("leftEye_1", "eye1_mat@material_left_eyeShape3@renderStats");
	auto vps_data = NdUtils::readNpy(eye_vps).asFloat();
	auto vns_data = NdUtils::readNpy(eye_vns).asFloat();
	auto uvs_data = NdUtils::readNpy(eye_uvs).asFloat();
	auto tl_data = NdUtils::readNpy(eye_tl).asFloat();
	auto tl_uv_data = NdUtils::readNpy(eye_tl_uv).asFloat();
	
	
	render->setMeshNode("leftEye_2", leftEye2);
	//render->setMeshNodeData("leftEye_2", vps_data, vns_data, uvs_data, tl_data, tl_uv_data, false);
//	render->setNodeMaterial("leftEye_2", "eye1_mat@material_left_eyeShape3@renderStats");
	render->setNodeMaterial("leftEye_2", "eye2_mat@mtl");
	render->render(R"(D:\360Downloads\win_render_0.png)");
	return 0;
	/*
	auto vrayController = std::make_shared<VrayRenderController>();
	vrayController->initFromConfig(vrayConf);
	std::unordered_map<std::string, std::vector<float32>> tb;
	std::vector<float32> bs(52, 0.0);
	vrayController->setAnimation(tb, bs);
	vrayController->renderImg("tmp.png", 1.0);
	return 0;*/
	auto vps = PNdArrayF({ 100,3 });
	auto vns = PNdArrayF({ 100,3 });
	auto uvs = PNdArrayF({ 100,3 });
	auto tls = PNdArrayF({ 100,3 });

	render->loadHairMtl(faceMtlPath);
	render->setImgSize(720, 1280);
	render->debugJust();
	//render->setHairNode("hair_node",hairPath,0.0005);
	//render->setHair(hairPath, 0.0005);
	render->setMeshNode("head_node", objPath);
	render->setNodeMaterial("head_node","head_skin_mtl");
//	render->setEnvironmentTex()
//	render->setCameraType(VrayRender::CAMERA_TYPE_ORTHOGONAL, 1.0);
	render->setCameraType(VrayRender::CAMERA_TYPE_PERSPECTIVE);
	render->setCameraFov(1.0*180.0 / PI);
	
	//render->setMeshNode("head_node", facePath);
//	render->setMeshNode("head_node", kfmPath);

	std::vector<float32> rotMatArray = { 0.9970107078552246, -9.234259778168052e-06, 0.0772632360458374, 0.03006068989634514, 0.9212551712989807, -0.3877953588962555, -0.07117557525634766, 0.38895872235298157, 0.9185015559196472 };
	render->setNodeTranslation("head_node", rotMatArray, { 1.081424593925476, 5.402594089508057, -55.87290954589844 });
//	render->setNodeTranslation("hair_node", { 1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0 }, { 0.0,0.0,0.0});
	render->setNodeMaterial("head_node", "head_skin_mtl");
	//render->setCameraFov(10.0);
	render->setCameraPos(0.0, 0.0, 1.0,0.0,1.0,0.0);
	render->render(R"(D:\360Downloads\win_render.png)");
	return 0;
	render->setEnvironmentColor({ 1.0,1.0,1.0 });
//	render->setEnvironmentTex(envTexPath, 1.0);
	

//	render->loadScene(teeth_vray);
//	render->setCameraType(VrayRender::CAMERA_TYPE_ORTHOGONAL, 10.0);
	//render->setTexture(hdrPath, bitMapName, texName);
	//render->setCameraPos(00.0f, 50.0f, 0.0f, 0, 0, 1);
	//render->setCameraPos(-0.4330127018922193 * 5, -0.75 * 5, -0.5 * 5, 0.0f,0.0f,1.0);
	//render->loadComp({teeth_path1,teeth_path2});
	auto rotMat = Mat4f::getRotation({ 0.21682593 ,0.10071934 ,0.02945451 });

	//std::vector<float32> rotMatArray = { 0.9945006 ,-0.00713882,0.10448787,0.029301,0.97679865,-0.21214579,-0.10054914 ,0.21404071 ,0.971636 };
/*
	doFor(r,3)
		doFor(c, 3) {
		rotMat[{r, c}] = rotMatArray[r * 3 + c];
	}
	rotMat[{3, 3}] = 1.0;
	*/

	render->setCameraType(VrayRender::CAMERA_TYPE_PERSPECTIVE);
	render->setImgSize(1280, 1280);

	render->setCamMat(Mat3f::getIdentity().toVector(), { 0.0,0.0,0.0 });
	render->setCameraFov(1.0 / PI * 180.0);
	render->setMeshNode("tmp_face", objHeadPath);
	std::vector<float32> rotMatArray2 = { 1.0392647, -0.03781079, -0.019240275, 0.031632528, 0.95449996, -0.296657, 0.030770892, 0.29580483, 0.9547905 };
	std::vector<float32> offArray2 = { -0.020526137, -0.13097312, -5.3356223 };
	render->setNodeTranslation("tmp_face", rotMatArray, offArray2);
	render->setNodeMaterial("tmp_face", "head_mtl");
	//render->setNodeMat("tmp_face", rotMatArray, { 0.08243224,  0.06763427, -2.4784849 });
	//render->setNodeMat("tmp_face", rotMatArray, { 0.0,0.0,-2.4784849 });
	//render->updateModel()
	//render->updateModel("exp24_teeth@node_121", { 0.5f });
	//render->loadHairMtl(hairMtlPath);
	//render->setHair(hairPath,0.0005);
	//render->setMeshNode("head_node", headPath, "head_mtl");

	//render->setModelEuler(0.0, 90.0, 0.0, 0.0, 0.0, 0.0);
	//render->setCameraType(VrayRender::CAMERA_TYPE_ORTHOGONAL, 0.1);
	//render->setCameraFov(10.0);
	//render->setCameraPos(0.0, 1.5, 1.0,0.0,1.0,0.0);
	//render->debugJust();
	//render->setOrthoWidth(1);
	//render->setCameraPos(0.101, -0.965, 0.242, 0.0, 0.0, 1.0);// 0.8938703584711671);
//	render->setCameraPos(0.0, -1.0, 0.0, 0.0, 0.0, 1.0);// 0.8938703584711671);

//	render->render(saveImgPathNew, -1);
	//return 0;
	//render->debugJust();
	std::vector<std::string> hairNames = { "hair4_02__songhuiqiao12__shav_hair__polySurface1_descriptionShape@hair4_02__songhuiqiao12__shav_hair__collectionqiane@hair4_02__songhuiqiao12__shav_hair__description@xgen_patch",
	"hair4_02__songhuiqiao12__shav_hair__polySurface1_description3Shape@hair4_02__songhuiqiao12__shav_hair__collectionqiane@hair4_02__songhuiqiao12__shav_hair__description3@xgen_patch",
		"hair4_02__songhuiqiao12__shav_hair__polySurface1_description4Shape@hair4_02__songhuiqiao12__shav_hair__collectionqiane@hair4_02__songhuiqiao12__shav_hair__description4@xgen_patch",
		"hair4_02__songhuiqiao12__shav_hair__polySurface1_description_copyShape@hair4_02__songhuiqiao12__shav_hair__collectionqiane@hair4_02__songhuiqiao12__shav_hair__description_copy@xgen_patch",
	"hair4_02__songhuiqiao12__shav_hair__polySurface1_description2Shape@hair4_02__songhuiqiao12__shav_hair__collectionqiane@hair4_02__songhuiqiao12__shav_hair__description2@xgen_patch"
	};
//	render->dumpHair(saveHairPath,hairNames);
	//render->render(saveImgPathNew, -1);
//	render->addRenderChannel("nodeId");
	//render->addRenderChannel("SSS");
	//render->addRenderChannel("specular");
	//render->addRenderChannel("normal");
	//auto rawImg = render->renderAll();

	//NdUtils::writeNpy(R"(tmp\zhangyu_raw.npy)", rawImg);
	auto idMap = render->getChannel("nodeId");
	render->render(R"(D:\360Downloads\win_render.png)");
	//while (true);
	//NdUtils::writeNpy(R"(tmp\zhangyu_id.npy)", idMap);
	
	return 0;
//	render->setDomeLightTex(envTexPath,true);
//	render->loadScene(emilyPath);
	//render->loadScene(animaionScene);
	//render->debugJust();
	//render->render(saveImgPathNew, -1);
	//return 0;
	//render->dumpHair(hairPath);
	//return 0;
	//(0.19, -5.945, -3.534, 0.0, -5.581, 0.0
//	render->setSceneEuler(0.19, -5.945, -3.534, 0.0, -12.0, 0.0);
	//render->setCameraFov(5);
//	render->setImgSize(64, 64);
	//render->setCameraPos(0.940322*60, 0.0, -0.304429*60);

	//render->setCameraPos(0.0, 0.0, 60.0);
	render->setCameraPos(-25.98, -30.0, 45);
	render->setOrthoWidth(32);
	std::vector<std::vector<float32>> motionSque = {
		{0.0,0.0,0.0,0.0,0.0,0.0},
		{0.0,0.0,0.0,15,0.0,0.0}
	};

	render->addMotionSqueeze(motionSque);
	render->renderSqueeze(R"(D:\Work\Projects\ProNew\Vray\tmp\vrayOut\motion\)",0,1,1,true);

//	render->render(saveImgPathNew, -1);

	return 0;
	//render->loadScene(hairScene);
	//render->dumpHair(hairPath);
	//return 0;
	//render->render(saveImgPathNew, -1);
	//return 0;
	//render->dumpHair(R"(tmp\hairAttri\redshift\hair_big.data)");
	//render->clearScene();
/*
	render->setImgSize(720, 1080);
	render->setCameraFov(7.5);
	//render->render("tmp\emily.png");
	//render->setSceneScale(0.1);
	render->setSceneEuler(0.19, -5.945, -3.534, 0.0, -5.581, 0.0);
	render->setCameraPos(0.0, 0.0, 150);
	render->clearLights();
	render->setLight(1, "FlashShape", { 1.0,1.0,1.0 }, { 0.0,-1.0,0.0 }, 1.0, {15,7.5});
	//render->setLight(1, "dirct", { 1.0,1.0,1.0 }, { 0.0,0.0,-1.0 }, 1.0);
	//render->render("tmp\emily.png");
	render->addRenderChannel("specular");
	render->addRenderChannel("diffuse");
	render->addRenderChannel("SSS");

	auto rawColor = render->renderRaw(-1);
	auto specular =  render->getChannel("specular");
	auto diffuse = render->getChannel("SSS");

	NdUtils::writeNpy("tmp\color.npy", rawColor);
	NdUtils::writeNpy("tmp\specular.npy", specular);
	NdUtils::writeNpy("tmp\diffuse.npy", diffuse);*/
	
	
	
	//render->loadScene(miniScene);
	render->loadHairMtl(hairMtlPath);

//	render->setHair(hairPath);
	render->setCameraFov(60.0f);
	render->setHair(hairPath,0.02);
	//render->setEnvironmentTex(envTexPath, 1.0, {},true,false);
	render->setDomeLightTex(envTexPath,true);
	render->setDomeLightEnable(true);
//	render->setEnvironmentColor({ 0.3,0.3,0.3 });
	
	//render->setEnvironmentColor({ 0.3,0.3,0.3 });
//	render->setLight(1, "dirctLight", {0.9,0.9,0.9 }, { -1.0,1.0,0.0 }, 1.0);
	//render->setLight(1, "dirctLight", { 1.0,1.0,1.0 }, { 1.0,1.0,0.0 }, 1.0);
	//render->setEnvironmentColor({ 1.0,1.0,1.0 });
	//render->setEnvironmentTex(envTexPath, 1.0, {},true,false);
	//render->addLight(2, "direct1", { 0.0,0.0,20.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0,1.0 }, 1.0);
	//render->setCameraPos(-0.547399,0.667333,-0.487739);
	//render->setCameraPos(-0.39,0.71,-0.5758);

	render->setCameraType(VrayRender::CAMERA_TYPE_ORTHOGONAL, 32);
	render->setCameraPos(0.0,0.0,40.0);
	//render->setCameraPos(0.940322, 0.0, -0.305529);
	//render->setCameraPos(-0.360796,0.888975,0.267063);
	//render->setModelEuler(0.0, 0.0, 0.0, 0.0, -1.55, 0.0);
	render->setModelEuler(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

//	std::string headPath = R"(D:\Work\Projects\ProNew\Vray\tmp\head_model.obj)";
	//auto objName = render->addObject(headPath);

	//render->renderMask(objName, R"(tmp\mask_img.png)");

//	render->setCameraType(VrayRender::CAMERA_TYPE_ORTHOGONAL);
	//render->setCameraFov(60.0f);
	//render->render(saveImgPath,-1);
	auto newEnvTex = R"(D:\Work\data\v-ray\vray_tmp\curl_long2\curl_long\sourceimages\test_rotate.exr)";
	//render->setDomeLightTex(newEnvTex);
	//render->render(saveImgPathNew,-1,R"(tmp\mask.npy)");
	auto ndArray = render->renderRaw(-1);
	NdUtils::writeNpy(R"(tmp\rawImg.npy)", ndArray);
	//render->exportScene(R"(tmp\scene_1.vrscene)");
	return 0;
}

/*
#define VRAY_RUNTIME_LOAD_PRIMARY
#define _USE_MATH_DEFINES
#include <vraysdk.hpp>
#include <vrayplugins.hpp>

using namespace VRay;
using namespace Plugins;
using namespace std;

void logMessage(VRay::VRayRenderer& renderer, const char* message, VRay::MessageLevel level, double instant, void* userData) {
	switch (level) {
	case VRay::MessageError:
		printf("[ERROR] %s\n", message);
		break;
	case VRay::MessageWarning:
		printf("[Warning] %s\n", message);
		break;
	case VRay::MessageInfo:
		printf("[info] %s\n", message);
		break;
	case VRay::MessageDebug:
		// Uncomment for testing, but you might want to ignore these in real code
		printf("[debug] %s\n", message);
		break;
	}
}

void printProgress(VRay::VRayRenderer& renderer, const char* message, int progress, int total, double instant, void* userData) {
	printf("Progress: %s %.2f%%\n", message, 100.0 * progress / total);
}

Transform makeTransform(float offX, float offY, float offZ) {
	Vector pos(offX, offY, offZ);
	Matrix rot;
	rot.makeDiagonal(Vector(1, 1, 1));
	return Transform(rot, pos);
}

GeomMayaHair BuildHair(VRayRenderer& renderer) {
	auto hair = renderer.newPlugin<GeomMayaHair>();
	hair.set_num_hair_vertices(IntList({ 4 }));
	hair.set_hair_vertices({ {-1,-1,0},{-1,1,0},{1,1,0},{1,-1,0} });
	float width = 0.01f;
	hair.set_widths({ width, width, width, width });
	return hair;
}

GeomSphere BuildGeom(VRayRenderer& renderer) {
	auto geom = renderer.newPlugin<GeomSphere>();
	geom.set_subdivs(64);
	return geom;
}

// Sets up a scene with camera and light.
void setUpScene(VRayRenderer& renderer) {
	auto renderView = renderer.newPlugin<RenderView>();
	//camera transform
	renderView.set_transform(makeTransform(0, 0, 5));
	//light
	auto light = renderer.newPlugin<LightDirect>();
	//material
	auto brdf = renderer.newPlugin<BRDFVRayMtl>();
	brdf.set_diffuse(AColor(1, 1, 1, 1.0));
	brdf.set_reflect(AColor(0.8, 0.8, 0.8, 1.0));
	brdf.set_fresnel(true);
	auto material = renderer.newPlugin<MtlSingleBRDF>();
	material.set_brdf(brdf);
	//geometry
	auto geom = BuildHair(renderer); //BuildGeom(renderer);
	//node
	auto node = renderer.newPlugin<Node>();
	node.set_transform(makeTransform(0, 0, 0));
	node.set_geometry(geom);
	node.set_material(material);
}

int main() {
	VRayInit init(NULL, true);
	VRayRenderer renderer;
	// It's recommended to always have a console log
	renderer.setOnLogMessage(logMessage);
	setUpScene(renderer);
	renderer.start();
	renderer.waitForRenderEnd();
	LocalVRayImage image(renderer.getImage());
	image->saveToJpeg("test_20191227.jpg");
	
	renderer.exportScene(R"(tmp\scene_2.vrscene)");
	return 0;
}
*/
