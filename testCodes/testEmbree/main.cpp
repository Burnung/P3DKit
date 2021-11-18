#include<iostream>
#include <GL/glew.h>
#include <string>
#include "engine/P3dEngine.h"
#include "geo/meshDataIO.h"
#include "engine/Camera.h"
#include "renderTech/hairAttris.h"
#include "renderTech/hairImage.h"
#include "renderTech/simpleRender.h"
#include "geo/meshDataIO.h"
#include "loader/P3DLoader.h"
#include "engine/extern/mediaCenter.h"
#include "engine/extern/cameraControllerPro.h"
#include "engine/OPenGL/OGLCommon.h"
#include "renderTech/blendShapeRender.h"
#include "tools/deformEyes/deformEyes.h"
#include "common/NdArray.h"

using namespace P3D;
void testEyeDeform() {

	auto eyeDeformer = std::make_shared<EyeDeformer>();

	auto eyePath = R"(D:\Work\data\ScanData\Jinbo\alain_head\eye_mouth\eyes.obj)";
	auto headPath = R"(D:\Work\data\ScanData\Jinbo\alain_head\del_mouth_objs\obj_0000.obj)";

	PNdArrayI lHardIdx = NdUtils::createArray<int>({ 4084,4085,4086 });
	PNdArrayI rHardIdx = NdUtils::createArray<int>({ 1134,1135,1136 });
	PNdArrayI lExtraIdx = NdUtils::createArray<int>({ 2392, 3371, 3803, 3804 });
	PNdArrayI rExtraIdx = NdUtils::createArray<int>({ 11, 14, 20, 178, 183 });

	PNdArrayF lCenter = NdUtils::createArray<float>({-2.5178, 16.9891, 6.3813});
	PNdArrayF rCenter = NdUtils::createArray<float>({ 3.6559, 17.1028, 6.3082 });
	eyeDeformer->init(eyePath, lHardIdx, lExtraIdx, rHardIdx, rExtraIdx, lCenter,rCenter);
	eyeDeformer->setHeadObj(headPath);
	auto retPos = eyeDeformer->solve(0.01, 10);

}
int main() {
	testEyeDeform();
}