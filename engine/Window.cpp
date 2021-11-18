#include "Window.h"
#if IS_WINDOWS || IS_MACOS
#include "OPenGL/OGLWIidow.h"
#endif
#if IS_LINUX
#include "OPenGL/OGLWindowLinux.h"
#endif
#if VULKAN_ENABLE
#include "Vul/VulWindow.h"
#endif
#include "common/RenderCommon.h"
BEGIN_P3D_NS
std::shared_ptr<P3DWindow> P3DWindow::make(uint8 type, uint32 w, uint32 h){
#if !IS_IOS
	switch (type)
	{
	case APP_OPENGL:
		return std::shared_ptr<P3DWindow>(new GLWindow(w, h));
#if VULKAN_ENABLE
	case APP_VULKAN:
		return std::shared_ptr<VulWindow>(new VulWindow(w, h));
		break;
#endif
	default:
		break;
	}
#endif
	return nullptr;
}

void P3DWindow::addComp(pComp comp, pShader shader) {
	scene->addComp(comp, shader);
}

END_P3D_NS
