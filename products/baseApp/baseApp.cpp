#include "baseApp.h"
#include "libs/imgui/myImgui.h"
BEGIN_P3D_NS

BaseApp::BaseApp(uint32 w, uint32 h, const std::string&tittle):winW(w),winH(h),winTittle(tittle) {
}
bool BaseApp::runAFrame() {
	auto nowTime = Utils::getCurrentTime();
	win->beginRender();
	P3DEngine::instance().clearBuffers(0.0, 0.0, 0.0, 0.0f);
	onUpdateRender(nowTime - lastTime);
	if (!updateBaseUI(nowTime - lastTime))
		dExit = true;
	onUpdateUI();
	ImGui::myImGuiRender();
	win->endRender();
	lastTime = nowTime;
	return dExit;
}
void BaseApp::runFrames() {
	lastTime = Utils::getCurrentTime();
	while (!dExit) {
		runAFrame();
	}
	shutDown();
}
void BaseApp::run() {
	init();
	//lastTime = Utils::getCurrentTime();
	runFrames();
/*	while (1) {
		auto nowTime = Utils::getCurrentTime();
		win->beginRender();
		P3DEngine::instance().clearBuffers(0.0, 0.0, 0.0, 0.0f);
		onUpdateRender(nowTime - lastTime);
		if(!updateBaseUI(nowTime - lastTime))
			break;
		onUpdateUI();
		ImGui::myImGuiRender();
		win->endRender();
		lastTime = nowTime;
	}
	shutDown();*/
}

bool BaseApp::updateBaseUI(float deltaTime) {
	ImGui::myImGuiNewFrame();
	ImGui::Begin(winTittle.c_str());
	if (ImGui::Button("exit")) {
		ImGui::End();
		return false;
	}
	ImGui::Text(Utils::makeStr("%.1f fps", 1.0 / deltaTime).c_str());
	ImGui::End();
	return true;
}

void BaseApp::shutDown() {
	ImGui::myImGuiShutdown();
	onShuwdown();
}

void BaseApp::init() {
	Utils::init();
	P3DEngine::instance().init(APP_OPENGL);
	win = P3DEngine::instance().createWindow(winW, winH);
	win->setWinTitle(winTittle);
	ImGui::initImGui(APP_OPENGL, win->getWinHandler());
	onInit();
}
END_P3D_NS