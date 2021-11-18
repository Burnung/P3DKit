
#include<iostream>
#include "render/vrayRender.h" 
#include <string>
#include "engine/P3dEngine.h"
#include <opencv2/opencv.hpp>
#include "geo/meshDataIO.h"
#include "engine/Camera.h"
using namespace P3D;
using namespace std;


int main(){
    cout<<"hello world"<<endl;
	P3DEngine::instance().init(APP_VULKAN);
	auto win = P3DEngine::instance().createWindow(512, 512);
	cout << "create vulkan window ok" << endl;
	return 0;
	/*
	auto hairPath = R"(D:\Work\data\Hair\hairstyles\hairstyles\strands00172.data)";

	auto vs = R"(D:\Work\Projects\ProNew\Vray\testCodes\shaders\hair\hair.vert)";
	auto ps = R"(D:\Work\Projects\ProNew\Vray\testCodes\shaders\hair\hair.frag)";
	auto texPath = R"(D:\TMPDATA\DataSwaps\jinlong_fov_50.png)";
	auto quadPath = R"(D:\Work\Projects\ProNew\Vray\testCodes\shaders\objs\quad.obj)";

	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(512, 512);
	auto shader = P3DEngine::instance().createShader();
	shader->loadFromFile(vs, ps);
	auto scene = win->getScene();
	auto hair = std::make_shared<HairModel>();
	hair->loadFromFile(hairPath);
	//scene->addHair(hair,shader);
	

	auto cvImg = cv::imread(texPath);
	auto tex = P3DEngine::instance().createTexture(cvImg.cols, cvImg.rows, TextureType::TEXTURE_2D, SampleType::SAMPLE_LINER, BUFFER_USE_TYPE::READ_WRITE, PixelFormat::PIXEL_RGB8,1);

	auto fbo = P3DEngine::instance().createRenderFrame(5120, 5120, 1);

	fbo->addRenderTextures(1);
	//	scene->addComp(quad, shader);
	scene->addHair(hair,shader);
	auto modelMat = Mat4f::getTranslate({ 0.0f,-1.55f,0.0f });
	auto modelRot = Mat4f::getRotY(45.0f);
	hair->setModelMat(modelMat * modelRot);
	auto camera = scene->getCamera();
	camera->setCameraType(Camera::CAMERA_TYPE_ORTH);
	camera->setOrthParams(0.9, 0.9, -100.0, 100.0);
	fbo->apply();
	scene->render();
	auto retTex = P3DEngine::instance().createTexture(512, 512, TEXTURE_2D, SAMPLE_LINER, READ_WRITE, PIXEL_RGBA8, 1);
	auto fboTex = fbo->getRenderTexture(0);
	fboTex->render2Texture(retTex); 
	auto retTexCpu = retTex->dump2Buffer(false);

	auto tCPU = fbo->getRenderTextureCpu(0);

	

	auto cvImg4 = cv::Mat(tCPU->width, tCPU->height, tCPU->nchannel==3 ? CV_8UC3:CV_8UC4, tCPU->getRawData());
	auto cvImg5= cv::Mat(retTexCpu->width, retTexCpu->height, retTexCpu->nchannel==3 ? CV_8UC3:CV_8UC4, retTexCpu->getRawData());
	cv::Mat cvImg6;
	cv::resize(cvImg4, cvImg6, { 512,512 }, 0.0, 0.0, cv::INTER_AREA);
	cv::imwrite("test_tex_render2.png", cvImg5);
	cv::imwrite("test_tex_render.png", cvImg4);
	cv::imwrite("test_tex_render_resize.png", cvImg6);
	return 0;
	fbo->apply0();
	while (true) {
		win->render();
	}
	//render->exportScene(R"(tmp\scene_1.vrscene)");

	return 0;*/
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
