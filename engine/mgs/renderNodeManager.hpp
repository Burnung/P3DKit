#pragma once
#include "renderNodeBase.hpp"
#include "renderNodeImgFilter.hpp"
#include "sceneManager.hpp"
#include "renderNodeCustom.hpp"
#include "renderNode3D.hpp"
#include "renderNodeSky.hpp"
BEGIN_P3D_NS

class RenderNodeManager :public P3DObject{
public:
	RenderNodeManager() = default;
	virtual ~RenderNodeManager() = default;
public:
	void setRenderSort(int rank, const std::string& tName) {
		if (renderNodes.find(tName) == renderNodes.end())
			return;
		renderSort[rank].push_back(tName);
		maxRank = max(rank, maxRank);
		minRank = min(rank, minRank);
	}
	std::unordered_map<std::string, pRenderNode> getAllRenderNodes() {
		return renderNodes;
	}
public:
	pRenderNode createRenderNode(const RENDERNODE_TYPE& tType, const std::string& tName, const std::string& vsPath, const std::string& fsPath, const std::string&renderFrameName, const std::vector<float>& winRatio = { 1.0,1.0 }) {
		pRenderNode ret = nullptr;
		if (tType == RENDERNODE_IMG_FILTER)
			ret = std::make_shared<RenderNodeImgFilter>(tName,vsPath, fsPath, renderFrameName, winRatio);
		else if (tType == RENDERNODE_CUSTOM)
			ret = std::make_shared<RenderNodeCustom>(tName, vsPath, fsPath, renderFrameName, winRatio);
		else if(tType == RENDERNODE_3D)
			ret = std::make_shared<RenderNode3D>(tName, vsPath, fsPath, renderFrameName, winRatio);
		else if(tType == RENDERNODE_SKY_SPHERE)
			ret =  std::make_shared<RenderNodeImgSkysphere>(tName, vsPath, fsPath, renderFrameName, winRatio);

		if (ret)
			renderNodes[tName] = ret;
		return ret;
	}
	pRenderNode createRenderNode(RENDERNODE_TYPE tType, const std::string& tName, const std::string& shaderName, std::string renderFrameName, const std::vector<float32>& winRatio = { 1.0,1.0f }) {
		pRenderNode ret = nullptr;
		if (tType == RENDERNODE_IMG_FILTER)
			ret = std::make_shared<RenderNodeImgFilter>(tName, shaderName, renderFrameName, winRatio);
		else if (tType == RENDERNODE_CUSTOM)
			ret = std::make_shared<RenderNodeCustom>(tName, shaderName, renderFrameName, winRatio);
		else if (tType == RENDERNODE_3D)
			ret = std::make_shared<RenderNode3D>(tName, shaderName, renderFrameName, winRatio);
		else if (tType == RENDERNODE_SKY_SPHERE)
			ret = std::make_shared<RenderNodeImgSkysphere>(tName, shaderName, renderFrameName, winRatio);
		else if (ret)
			renderNodes[tName] = ret;

		if (ret)
			renderNodes[tName] = ret;
		return ret;
	}
	void paraseFromeFile(const std::string& fPath);
	pRenderNode getRenderNode(const std::string& tName) {
		return renderNodes[tName];
	}

	void renderAll() {
		for (int i = minRank; i <= maxRank;++i) {
			if (renderSort.find(i) == renderSort.end())
				continue;
			for (const auto& nodeName : renderSort[i]) {
				auto tNode = renderNodes[nodeName];
				if (tNode)
					tNode->render();
			}
		}
	}
private:
	std::unordered_map<std::string, pRenderNode> renderNodes;
	std::unordered_map<int, std::list<std::string>> renderSort;
	int maxRank = 0;
	int minRank = 100;
};

typedef std::shared_ptr<RenderNodeManager> pRenderNodeManager;
END_P3D_NS
