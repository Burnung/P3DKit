#include "userGeoGpu.h"
#include "P3dEngine.h"
BEGIN_P3D_NS

void UserGeoGPU::addVertexData(pDataBlock pData, const std::string& attriName, uint32 stride, uint32 offset) {
	auto pBuffer = P3DEngine::instance().createVertexBuffer();
	//pBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ARRAY, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_FLOAT32, pData);
	pBuffer->init(VERTEX_BUFFER_ARRAY, DYNAMIC_DRAW,pData->getDataType() ,pData);
	addVertexData(pBuffer, attriName, stride, offset);
}

void UserGeoGPU::addVertexData(pDataBlock pData, int attriLoc, uint32 stride, uint32 offset) {
	auto pBuffer = P3DEngine::instance().createVertexBuffer();
	//pBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ARRAY, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_FLOAT32, pData);
	pBuffer->init(VERTEX_BUFFER_ARRAY, DYNAMIC_DRAW,pData->getDataType() ,pData);
	addVertexData(pBuffer, attriLoc, stride, offset);
}
void UserGeoGPU::addElementBuffer(pDataBlock pData, GeoTYPE type) {
	auto pBuffer = P3DEngine::instance().createVertexBuffer();
	pBuffer->init(VERTEX_BUFFER_ELEMETN, STATIC_DRAW,pData->getDataType(),pData);
	addElementBuffer(pBuffer, type);
}

void UserGeoGPU::fromComp(pComp tComp) {
	clear();
	using namespace SHADER_KEY_WORDS;
	int32 locId = 0;

	addVertexData(tComp->cache_vps,locId, 3);
	locId++;
	if (tComp->skinInfo != nullptr) {
		skinInfo = tComp->skinInfo;
		auto boneIdData = std::make_shared<DataBlock>(skinInfo->bId.data(), skinInfo->bId.size()*sizeof(Vec4f), false, DType::Float32);
		addVertexData(boneIdData, locId,4);
		locId++;

		auto boneWData = std::make_shared<DataBlock>(skinInfo->bWs.data(), skinInfo->bWs.size()*sizeof(Vec4f), false, DType::Float32);
		addVertexData(boneWData, locId, 4);
		locId++;
	}

	if (tComp->uvs) {
		addVertexData(tComp->uvs, locId, 2);
		locId++;
	}
	if (tComp->cache_vns) {
		addVertexData(tComp->cache_vns, locId, 3);
		locId++;
	}
	/*if (tComp->vcs) {
		addVertexData(tComp->vcs, locId, 3);
		locId++;
	}*/
	if (tComp->tanget) {
		addVertexData(tComp->tanget, locId, 3);
		locId++;
	}
	if (tComp->bTanget) {
		addVertexData(tComp->bTanget, locId, 3);
		locId++;
	}
	addElementBuffer(tComp->tl, GEO_TYPE_TRIS);
	commitPure();

}
void UserGeoGPU::setMaterial(pMaterial tMat) {
	tMaterial = tMat;
}

END_P3D_NS
