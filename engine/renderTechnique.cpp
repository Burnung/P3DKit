#include "renderTechnique.h"
BEGIN_P3D_NS

void RenderTechnique::addRenderPass(pRenderable renderable, uint32 renderNum,bool lastPass,uint32 nSample) {
	pRenderFrame tFbo = nullptr;
	if (lastPass && !passes.empty())
		tFbo = passes.back().fbo;
	
	auto useW = winW, useH = winH;
	//if()
	RenderPass tPass({ renderable }, renderNum, winW, winH,tFbo);
	if (!passes.empty()&&!lastPass) {
		const auto& lastPass = passes.back();
		auto tShader = renderable->getShader();
		for (auto i = 0; i < lastPass.num; ++i) {
			auto keyName = Utils::makeStr("passTex%d", i);
			tShader->setUniformTex(keyName, lastPass.fbo->getRenderTexture(i));
		}
	}
	passes.push_back(tPass);
}

void RenderTechnique::addRenderPass(std::vector<pRenderable> renderables, uint32 renderNum) {
	RenderPass tPass(renderables, renderNum, winW, winH);
	if (!passes.empty()) {
		const auto& lastPass = passes.back();
		for (auto renderable : renderables) {
			auto tShader = renderable->getShader();
			for (auto i = 0; i < lastPass.num; ++i) {
				auto keyName = Utils::makeStr("passTex%d", i);
				tShader->setUniformTex(keyName, lastPass.fbo->getRenderTexture(i));
			}
		}
	}
	passes.push_back(tPass);

}

pRenderFrame RenderTechnique::render() {
	for (auto& rt : passes) {
		rt.setCamera(mCamera);
		rt.draw();
	}
	return passes.back().fbo;
}



pRenderFrame RenderTechnique::getPassFrame(int idx) {
	if (idx < passes.size())
		return passes[idx].fbo;
	return nullptr;
}



END_P3D_NS