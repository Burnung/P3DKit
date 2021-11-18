#include"P3DLoader.h"
#include "assimp/Importer.hpp"
#include "common/NdArray.h"

#include <FreeImage.h>
BEGIN_P3D_NS


std::vector<pComp> P3DLoader::loadComps(const std::string& fPath, bool calcTangent) {
	std::cout << "loading mesh " << fPath << std::endl;
	auto loadFlags = importFlags;
	if (calcTangent)
		loadFlags |= aiProcess_CalcTangentSpace;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fPath, loadFlags);
	std::cout << scene->mNumAnimations << std::endl;
	P3D_ENSURE((scene && (scene->HasMeshes())), Utils::makeStr("load file %s error!", fPath));
	std::vector<pComp> ret;
	for (int i = 0; i < scene->mNumMeshes; ++i) {
		std::string tName = scene->mMeshes[i]->mName.C_Str();

		auto tComp = convertAiMesh2Comp(scene->mMeshes[i]);
		tComp->name = tName;
		ret.push_back(tComp);
	}
	return ret;

}

pComp P3DLoader::convertAiMesh2Comp(const aiMesh* tMesh) {
	P3D_ENSURE(tMesh->HasPositions(), "assimp mesh no vps");
	std::cout << tMesh->mNumAnimMeshes << std::endl;
	std::cout << tMesh->mNumBones << std::endl;
	//auto tComp = std::make_shared(P3DComp)
	auto nVert = tMesh->mNumVertices;
	auto vps = std::make_shared<DataBlock>(nVert * sizeof(Vec3f),DType::Float32);
	vps->copyData((uint8*)&(tMesh->mVertices[0]), vps->getSize(), 0);
	pDataBlock vns = nullptr;
	pDataBlock uvs = nullptr;
	pDataBlock vcs = nullptr;
	pDataBlock tangets = nullptr;
	pDataBlock biTangets = nullptr;
	if (tMesh->HasNormals()) {
		vns = std::make_shared<DataBlock>(nVert * sizeof(Vec3f), DType::Float32);
		vns->copyData((uint8*)&(tMesh->mNormals[0]), vns->getSize(), 0);
	}

	if (tMesh->HasTextureCoords(0)) {
		uvs = std::make_shared<DataBlock>(nVert * sizeof(Vec2f), DType::Float32);
		auto uvData = uvs->rawData<float32>();
		for (int i = 0; i < nVert; ++i) {
			uvData[i * 2] = tMesh->mTextureCoords[0][i].x;
			uvData[i * 2 + 1] = tMesh->mTextureCoords[0][i].y;
		}
	}

	if (tMesh->HasTangentsAndBitangents()) {
		tangets = std::make_shared<DataBlock>(nVert * sizeof(Vec3f), DType::Float32);
		tangets->copyData((uint8*)&(tMesh->mTangents[0]), tangets->getSize(), 0);

		biTangets = std::make_shared<DataBlock>(nVert * sizeof(Vec3f), DType::Float32);
		biTangets->copyData((uint8*)&(tMesh->mBitangents[0]), biTangets->getSize(), 0);
	}

	auto tl = std::make_shared<DataBlock>(tMesh->mNumFaces*sizeof(uint32)*3,DType::Uint32);
	uint32* tlData = tl->rawData<uint32>();
	for (uint32 i = 0; i < tMesh->mNumFaces; ++i) {
		tlData[i * 3] = tMesh->mFaces[i].mIndices[0];
		tlData[i * 3+1] = tMesh->mFaces[i].mIndices[1];
		tlData[i * 3+2] = tMesh->mFaces[i].mIndices[2];
	}

	auto retComp = std::make_shared<P3DComp>(vps, tl, uvs, vns, vcs, tangets, biTangets);
	return retComp;
}

pCPuImage P3DLoader::loadCpuImg(const std::string& fPath) {
	auto fif = FreeImage_GetFileType(fPath.c_str());
	P3D_ENSURE((fif != FIF_UNKNOWN), Utils::makeStr("free image not suport %s", fPath));
	auto dib = FreeImage_Load(fif,fPath.c_str());
	if (!dib) {
		P3D_THROW_RUNTIME("load file %s failed", fPath);
		return nullptr;
	}
	auto imgType = FreeImage_GetImageType(dib);
	int w = FreeImage_GetWidth(dib);
	int h = FreeImage_GetHeight(dib);
	int c = 1;
	DType tType = DType::UInt8;
	switch (imgType)
	{
	case FIT_BITMAP:
	{
		auto cType = FreeImage_GetColorType(dib);
		if (cType == FIC_RGB)
			c = 3;
		else if (cType == FIC_RGBALPHA)
			c = 4;
		break;
	}
	case FIT_UINT16:
		tType = DType::Uint16;
		break;
	case FIT_INT16:
		tType = DType::Int16;
		break;
	case FIT_UINT32:
		tType = DType::Uint32;
		break;
	case FIT_INT32:
		tType = DType::Int32;
		break;
	case FIT_FLOAT:
		tType = DType::Float32;
		break;
	case FIT_DOUBLE:
		tType = DType::Float64;
		break;
	case FIT_RGB16:
		tType = DType::Uint16;
		c = 3;
		break;
	case FIT_RGBA16:
		tType = DType::Uint16;
		c = 4;
		break;
	case FIT_RGBF:
		tType = DType::Float32;
		c = 3;
		break;
	case FIT_RGBAF:
		tType = DType::Float32;
		c = 4;
		break;
	default:
		break;
	}
	auto srcData = FreeImage_GetBits(dib);
	if (c == 3) {
	// 	dib = FreeImage_
		auto a = 0;
	}

	PNdArray imgArr(tType, { h,w,c }, true);
	auto tBlock = imgArr.getDataBlock();
	tBlock->copyData((uint8*)srcData, tBlock->getSize(), 0);
	if (c >= 3) {
		switch (tType)
		{
		case P3D::DType::Float32: {
			auto tmpArray = imgArr.asFloat();
			swapRedBlue<float32>(tmpArray);
			break;
		}

		case P3D::DType::Int32:
			break;
		case P3D::DType::UInt8: {
			auto tmpArray = imgArr.asType<uint8>();
			swapRedBlue<uint8>(tmpArray);
			//imgArr = tmpArray;

			break;
		}
		case P3D::DType::Int64:
			break;
		case P3D::DType::String:
			break;
		case P3D::DType::Bool:
			break;
		case P3D::DType::Int16:
			break;
		case P3D::DType::Float64:
			break;
		case P3D::DType::Uint16:
			break;
		case P3D::DType::Uint32:
			break;
		}


	}
	auto ret = std::make_shared<CpuImage>(imgArr);


	return ret;


	/*
	auto  tMat = cv::imread(fPath, cv::IMREAD_UNCHANGED);
	int w = tMat.rows;
	int h = tMat.cols;
	int c = 3;
	P3D::DType tType = DType::UInt8;
	switch (tMat.type())
	{
	case CV_8U:
	{
		c = 1;
		break;
	}
	case CV_8UC3:
	{
		c = 3;
		cv::cvtColor(tMat, tMat,cv::COLOR_BGR2RGB);
		break;

	}
	case CV_8UC4:
	{
		c = 4;
		cv::cvtColor(tMat, tMat,cv::COLOR_BGRA2RGBA);
		break;

	}
	case CV_32FC3:
	{
		c = 3;
		cv::cvtColor(tMat, tMat, cv::COLOR_BGR2RGB);
		tType = DType::Float32;
		break;
	}
	default:
		break;
	}
	
	PNdArray imgArray(tType, { h,w,c },true);
	auto dataBlock = imgArray.getDataBlock();
	dataBlock->copyData((uint8*)(tMat.data), dataBlock->getSize(),0);
	auto ret = std::make_shared<CpuImage>(imgArray);

	return ret;*/
}

END_P3D_NS