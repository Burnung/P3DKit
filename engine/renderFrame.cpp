#include "renderFrame.h"
#include "OPenGL/OGLFBO.h"
BEGIN_P3D_NS


std::shared_ptr<RenderFrame> RenderFrame::make(uint type,uint32 w, uint32 h, uint32 ssa) {
	switch (type) {
	case APP_OPENGL:
		return std::make_shared<GLFbo>(w, h, ssa);
		break;
	}
}
END_P3D_NS