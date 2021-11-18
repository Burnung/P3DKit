#pragma once

#include"common/P3D_Utils.h"
#include "common/RenderCommon.h"
#include "common/mathVector.hpp"
#include "Shader.h"
#include "OPenGL/OGLShader.h"
BEGIN_P3D_NS
pShader Shader::make(uint t) {
	switch (t)
	{
	case APP_OPENGL:
		return std::make_shared<GLShader>();
	}
}
END_P3D_NS