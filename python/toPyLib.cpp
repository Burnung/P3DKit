#include<common/mathIncludes.h>
#include <cv_common/CvUtil.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/pytypes.h>
#include "hairGLRender.h"
#include "renderTech/hairAttris.h"
#include "renderTech/hairImage.h"
#include "renderTech/simpleRender.h"
#include "renderTech/blendShapeRender.h"
#include "renderTech/generalRenderTech.h"
//#include "tools/deformEyes/deformEyes.h"
//#include "tools/deformEyes/hairFix.h"
//#include "HairSmoother.h"
#include "products/loader/P3DLoader.h"
//#include "products/LapDeform/laplacian.h"
#include "renderTech/PyRender.hpp"
#include "pyFace3D.hpp"
#include "pyApp.hpp"

using namespace std;
using namespace  P3D;
//using namespace ksu;
namespace py = pybind11;
#include "pyutils.h"
#include "pyhelper.h"

//sing namespace P3D;
PYBIND11_MODULE(pyP3D,m){
    m.doc() = "P3DKit python port";
	/*
	py::class_<VrayRender> pyVrayRender(m, "VrayRender");
	
	pyVrayRender.def(py::init<>())
		.def("init", &VrayRender::init, py::arg("w") = 720, py::arg("h") = 720, py::arg("visiable") = false)
		.def("load_scene", &VrayRender::loadScene)
		.def("load_hair_mtl", &VrayRender::loadHairMtl, py::arg("mtl_path"))
		.def("set_img_size", &VrayRender::setImgSize)
		.def("render", &VrayRender::render, py::arg("save_path"), py::arg("time_max") = -1, py::arg("Id_path") = "")
		.def("set_model_euler", &VrayRender::setModelEuler)
		.def("set_camera_pos", &VrayRender::setCameraPos)
		.def("set_camera_fov", &VrayRender::setCameraFov)
		.def("set_hair", &VrayRender::setHair, py::arg("hair_path"), py::arg("hair_width") = 0.0005)
		.def("set_env_tex", &VrayRender::setEnvironmentTex, py::arg("img_path"), py::arg("intensity") = 1.0, py::arg("rot") = std::vector<float>(), py::arg("no_back") = false, py::arg("as_light") = true)
		.def("set_env_color", &VrayRender::setEnvironmentColor, py::arg("color"))
		.def("add_light", &VrayRender::addLight, py::arg("type"), py::arg("name"), py::arg("pos"), py::arg("rot"), py::arg("color") = std::vector<float>(3, 1.0f), py::arg("intensity") = 1.0, py::arg("extra") = std::vector<float>())
		.def("export_scene", &VrayRender::exportScene)
		.def("set_camera_type", &VrayRender::setCameraType, py::arg("type"), py::arg("orth_wdith") = 0.6)
		.def("set_domelight_tex", &VrayRender::setDomeLightTex, py::arg("tex_path"), py::arg("use_specular") = true)
		.def("set_orth_width", &VrayRender::setOrthoWidth)
		.def("set_light", &VrayRender::setLight, py::arg("type"), py::arg("name"), py::arg("color"), py::arg("pos"), py::arg("intensity"), py::arg("extra_data") = std::vector<float32>({1.0f,1.0f}))
		.def("add_obj", &VrayRender::addObject, py::arg("obj_path"))
		.def("render_raw", &VrayRender::renderRaw, py::arg("max_time") = -1)
		.def("set_domelight_enable", &VrayRender::setDomeLightEnable)
		.def("clear_lit", &VrayRender::clearLights)
		.def("set_scene_euler", &VrayRender::setSceneEuler)
		.def("add_render_channel", &VrayRender::addRenderChannel)
		.def("get_channel", &VrayRender::getChannel)
		.def("set_tex", &VrayRender::setTexture, py::arg("img_path"), py::arg("bitmap_name"), py::arg("tex_name"),py::arg("color_space")=1)
		.def("load_comps", &VrayRender::loadComp, py::arg("comps_list"))
		.def("update_model", &VrayRender::updateModel, py::arg("node_name"), py::arg("bs_coeffs"))
		.def("set_node_translation", &VrayRender::setNodeTranslation, py::arg("node_name"), py::arg("rot_scale"), py::arg("trans"))
		.def("set_cam_mat", &VrayRender::setCamMat, py::arg("rot_mat"), py::arg("trans"))
		.def("set_mesh_node", &VrayRender::setMeshNode, py::arg("node_name"), py::arg("obj_path"))
		.def("set_hair_node", &VrayRender::setHairNode, py::arg("node_name"), py::arg("hair_path"), py::arg("src_width"))
		.def("set_mesh_node_data", &VrayRender::setMeshNodeData, py::arg("node_name"), py::arg("vps"), py::arg("vns"), py::arg("uvs"), py::arg("tls"),py::arg("uvtls"),py::arg("calcN")=false)
		.def("set_node_material", &VrayRender::setNodeMaterial, py::arg("node_name"), py::arg("mtl_name"))
		.def("set_node_visiable", &VrayRender::setNodeVisiable, py::arg("node_name"), py::arg("visiable"));

	py::class_<VrayRenderController> pyVrayController(m, "VrayController");
	pyVrayController.def(py::init<>())
	.def("init_form_cfg", &VrayRenderController::initFromConfig, py::arg("cfg_path"))
		.def("set_animation", &VrayRenderController::setAnimation, py::arg("bone_pos"), py::arg("bs"))
		.def("render_img", &VrayRenderController::renderImg, py::arg("img_path"),py::arg("gamma")=2.2)
		.def("render_raw", &VrayRenderController::renderRaw);
		*/

	py::class_<Face3DPython> pyFace3D(m, "Face3D");
	pyFace3D.def(py::init<>())
		.def("init",&Face3DPython::init,py::arg("ycnnPath"),py::arg("face3DModel"),py::arg("mode")="video",py::arg("use_lightflow")=true)
		.def("calc",&Face3DPython::calc)
		.def("calc_lds",&Face3DPython::calcLandmarks)
		.def("set_fov", [](Face3DPython*face3D, float tFov) {
				face3D->fov = tFov;
			})
		.def("get_rt", [](Face3DPython* face3D) {
				std::vector<float> ret(6);
				auto pre_so = face3D->faceTracker->pre_solution;
				for (int i = 0; i < 3; ++i)
					ret[i] = pre_so->R[i];
				for (int i = 0; i < 3; ++i)
					ret[i + 3] = pre_so->t[i];
				return ret;
			})
		.def("get_shapes",[](Face3DPython* face3D) {
				const auto& tShape = face3D->faceTracker->pre_solution->id;
				PNdArrayF retArray({ int(tShape.size()) });
				for (int i = 0; i < tShape.size(); ++i)
					retArray[{i}] = tShape[i];
				return retArray;
			})
			.def("get_exps", [](Face3DPython* face3D) {
				const auto& tExp = face3D->faceTracker->pre_solution->exp;
				PNdArrayF retArray({ int(tExp.size()) });
				for (int i = 0; i < tExp.size(); ++i)
					retArray[{i}] = tExp[i];
				return retArray;
				})
				.def("get_lds", [](Face3DPython * face3D){
					return face3D->landmarks2D;
					})
				.def("get_calc_lds", [](Face3DPython * face3D){
					return face3D->calcLds;
					})
				.def("get_all_calc_lds", [](Face3DPython * face3D){
					return face3D->allcalcLandmars;
					})
				.def("get_face_mesh_vps", [](Face3DPython* face3D) {
						const auto faceMesh = face3D->faceRet->face_mesh->vps;
						return faceMesh;
					})
				.def("calc_multi_imgs", &Face3DPython::calcMultiImg, py::arg("imgs"), py::arg("exps") = std::vector<std::vector<float>>())
				.def("get_face_mesh_tl", [](Face3DPython* face3D) {
						const auto& tl = face3D->faceRet->face_mesh->tl;
						std::vector<uint32> faceTl(tl.size());
						for (int i = 0; i < tl.size(); ++i)
							faceTl[i] = tl[i];
						return faceTl;
					})
				.def("get_face_mat", [](Face3DPython* face3D) {
					return face3D->faceRet->face_mesh->outerMatrix;
					})
				.def("get_face_mesh_uv", [](Face3DPython* face3D) {
					return face3D->faceRet->face_mesh->uvs0;
					})
				.def("get_all_face_mesh_vps", [](Face3DPython* face3D){
					std::vector<vector<float>> rets;
					if (face3D->faceRets.empty()) {
						rets.push_back(face3D->faceRet->face_mesh->vps);
						return rets;
					}
					for (auto tRet : face3D->faceRets) {
						rets.push_back(tRet->face_mesh->vps);
					}
					return rets;
					})
				.def("get_all_face_mat", [](Face3DPython* face3D) {
						std::vector<vector<float>> ret;
						if (face3D->faceRets.empty()) {
							ret.push_back(face3D->faceRet->face_mesh->outerMatrix);
							return ret;
						}
						for (auto tRet : face3D->faceRets)
							ret.push_back(tRet->face_mesh->outerMatrix);
						return ret;

					})
					.def("get_all_face_lds", [](Face3DPython* face3D) {
						std::vector<PNdArrayF> rets;
						if (face3D->faceRets.empty()) {
							rets.push_back(face3D->landmarks2D);
							return rets;
						}
						return face3D->allLandmars;
					});

	py::class_<PyRender> PypyRender(m, "PyRender");
	PypyRender.def(py::init<>())
		.def("init", &PyRender::init)
		.def("load_render_cfg", &PyRender::loadRenderCfg)
		.def("set_rendernode_frame_size", &PyRender::setRenderNodeFrameSize)
		.def("set_tex", &PyRender::addTex)
		.def("set_comp_mat", &PyRender::setCompModelMat)
		.def("add_comp_data", &PyRender::addCompData, py::arg("name"),py::arg("vps"), py::arg("tls"), py::arg("uv") = std::vector<float>(), py::arg("normals")=std::vector<float>())
		.def("update_comp", &PyRender::updateComp)
		.def("render", &PyRender::render)
		.def("get_render_img", &PyRender::getRenderImg)
		.def("get_render_img_u8", &PyRender::getRenderImgU8);

	py::class_<GeneralAppPython> pyGeneralApp(m, "PyApp");
	pyGeneralApp.def(py::init<>())
		.def("init", &GeneralAppPython::init, py::arg("w"), py::arg("h"), py::arg("title") = "pyApp")
		.def("set_cfgs", &GeneralAppPython::setConfigs)
		.def("set_rendernode_frame_size", &GeneralAppPython::setRenderNodeFrameSize)
		.def("set_tex", &GeneralAppPython::addTex)
		.def("resize_win", &GeneralAppPython::resizeWinSize)
		.def("set_comp_mat", &GeneralAppPython::setCompModelMat)
		.def("add_comp_data", &GeneralAppPython::addCompData, py::arg("name"), py::arg("vps"), py::arg("tls"), py::arg("uv") = std::vector<float>(), py::arg("normals") = std::vector<float>())
		.def("update_comp", &GeneralAppPython::updateComp)
		.def("get_render_img", &GeneralAppPython::getRenderImg)
		.def("get_render_img_u8", &GeneralAppPython::getRenderImgU8)
		.def("run_win", &GeneralAppPython::runWindow)
		.def("render", &GeneralAppPython::runAFrame)
		.def("get_valuesf", &GeneralAppPython::getValuesF)
		.def("get_valuef", &GeneralAppPython::getValueF)
		.def("set_values", &GeneralAppPython::setValues)
		.def("set_value", &GeneralAppPython::setValue)
		.def("add_comp_file", &GeneralAppPython::addCompFile)
		.def("release", &GeneralAppPython::release)
		.def("set_cam_pos", &GeneralAppPython::setCameraPos)
		.def("set_cam_fov", &GeneralAppPython::setCameraFov);


	py::class_<SimpleRender> pyHairRender(m, "HairRender");
	pyHairRender.def(py::init<>())
		.def("init", &SimpleRender::init)
		.def("render", &SimpleRender::render)
		.def("set_orth_width", &SimpleRender::setOrthWidth)
		.def("set_win_size", &SimpleRender::setWinSize);


	//Render Technique
	py::class_<HairAttrisTech> pyHairAttrisTech(m, "HairAttrisTech");
	pyHairAttrisTech.def(py::init<>())
		.def("init", &HairAttrisTech::init, py::arg("workDir"), py::arg("w"), py::arg("h"), py::arg("orthW") = 0.6,py::arg("nFbo")=1)
		.def("render", &HairAttrisTech::render, py::arg("camPos"));

	py::class_<HairImageTech> pyHairImageTech(m, "HairImageTech");
	pyHairImageTech.def(py::init<>())
		.def("init", &HairImageTech::init, py::arg("workDir"))
		.def("render", &HairImageTech::render, py::arg("src_img"), py::arg("model_mat"));
	
	py::class_<BlendShapeRender> pyBlendShapeRender(m, "BlendShapeRender");
	pyBlendShapeRender.def(py::init<>())
		.def("init", &BlendShapeRender::init, py::arg("obj_list"),py::arg("vs_path"),py::arg("ps_path"),py::arg("mrt")=1)
		.def("set_orth_width", &BlendShapeRender::setOrthWidth)
		.def("set_win_size", &BlendShapeRender::setWinSize)
		.def("render", &BlendShapeRender::render, py::arg("bs_coeff"), py::arg("cam_x"), py::arg("cam_y"), py::arg("cam_z"));


	//general render tech
	py::class_<GeneralRenderTech> pyGeneralRenderTech(m, "GeneralRenderTech");
	pyGeneralRenderTech.def(py::init<>())
		.def("add_shader", &GeneralRenderTech::addShader, py::arg("shader_name"), py::arg("vs_path"), py::arg("fs_path"))
		.def("set_comp", &GeneralRenderTech::addComp, py::arg("node_name"), py::arg("obj_path"), py::arg("shader_name"))
		.def("set_comp_data", &GeneralRenderTech::addCompData, py::arg("node_name"), py::arg("vs_data"), py::arg("tl_data"), py::arg("uvs"),py::arg("shader_name"),py::arg("uv_tl"))
		.def("set_hair", &GeneralRenderTech::addHair, py::arg("node_name"), py::arg("hair_path"), py::arg("shader_name"))
		.def("set_node_translation", &GeneralRenderTech::setNodeTranslate, py::arg("node_name"), py::arg("sr_mat"), py::arg("t"))
		.def("render", &GeneralRenderTech::render)
		.def("set_cam_fov", &GeneralRenderTech::setCamFov)
		.def("set_img_size", &GeneralRenderTech::setImgSize)
		.def("add_tex", &GeneralRenderTech::addTexture)
		.def("clear_scene",&GeneralRenderTech::clearScene)
		.def("set_shader_float",&GeneralRenderTech::setShaderFloat,py::arg("shader_name"),py::arg("uniform_name"),py::arg("value"))
		//.def("set_com")
		.def("set_shader_tex", &GeneralRenderTech::setShaderTex,py::arg("shader_name"),py::arg("tex_name"),py::arg("uniform_name")="")
		.def("render_win", &GeneralRenderTech::renderWin);

	/*
	// tools eye deform
	py::class_<EyeDeformer> pyEyeDeformer(m, "EyeDeformer");
	pyEyeDeformer.def(py::init<>())
		.def("init", &EyeDeformer::init, py::arg("l_eye_path"), py::arg("r_eye_path"), py::arg("l_hard_idx"), py::arg("l_extra_idx"), py::arg("r_hard_idx"), py::arg("r_extra_idx"), py::arg("l_center"), py::arg("r_center"))
		.def("set_head", &EyeDeformer::setHeadObj, py::arg("head_path"))
		.def("solve", &EyeDeformer::solve, py::arg("delta"), py::arg("neibour_size"), py::arg("mult_delta"))
		.def("solve_by_hard_pos", &EyeDeformer::solveByHardPos, py::arg("left_pos"), py::arg("right_pos"), py::arg("neibour_size"), py::arg("mult_delta"))
		.def("compute_normals", &EyeDeformer::recacleCompNormals, py::arg("obj_path"));

	py::class_<LaplacianDeform> pyLaplacianDeform(m, "SampleLap");
	pyLaplacianDeform.def(py::init<>())
		.def("solve", &LaplacianDeform::sampleDeform, py::arg("all_src_vps"), py::arg("tls"), py::arg("new_id"), py::arg("new_pos"), py::arg("neibour_size")=5)
		.def("diffusion_vps", &LaplacianDeform::diffusionVps);

	// tools for voumlue 2 hair Strand
	py::class_<CHairSmoother> pyHairSmoother(m, "HairSmoother");
	pyHairSmoother.def(py::init<>())
		.def("init", &CHairSmoother::init, py::arg("rootPosPath"))
		.def("set_grid_size", &CHairSmoother::setGridSize, py::arg("grid_size"), py::arg("min_AABB"), py::arg("max_AABB"))
		.def("set_tensors", &CHairSmoother::setOrientationField, py::arg("V_tensor"), py::arg("F_tensor"),py::arg("threshold")=0.0)
		.def("test_array",&CHairSmoother::testNdArray)
		.def("com_new_strands", &CHairSmoother::compNewStrands, py::arg("dump_path"));

	// tools for hair fix 
	void solve(const std::string& hairPaht,const std::vector<float32>& rotMatVec,const std::vector<float32>& tVec,const std::string&dstPath, int radius = 10);
	py::class_<HairFix> pyHairFix(m, "HairFix");
	pyHairFix.def(py::init<>())
		.def("init", &HairFix::init, py::arg("headPath"))
		.def("solve", &HairFix::solve, py::arg("hair_path"), py::arg("rot_array"), py::arg("t_array"), py::arg("dst_patg"), py::arg("radius") = 10)
		.def("solve_collision", &HairFix::solveHairCollisionData, py::arg("hair_data"), py::arg("hair_normal"), py::arg("head_path"))
		.def("solve_collision2", &HairFix::solveHairCollisionData2, py::arg("hair_data"), py::arg("hair_normal"), py::arg("head_vps"), py::arg("head_tls"))
		.def("solve_common_collision", &HairFix::solveCommonCollision, py::arg("head_vps"), py::arg("head_tls"), py::arg("src_vps"), py::arg("hit_dir"),py::arg("t_delta"));
	*/
}


