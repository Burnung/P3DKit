#include "OGLUserMeshGpu.h"
BEGIN_P3D_NS

OGLUserMeshGPU::~OGLUserMeshGPU() {
	release();
}
void OGLUserMeshGPU::release() {
	attriNames.clear();
	vertexBuffers.clear();
	elementBuffer = nullptr;
	if (mVaoId > 0) {
		glDeleteVertexArrays(1, &mVaoId);
		mVaoId = 0;
	}
}
void OGLUserMeshGPU::addVertexData(pVertexBuffer pbuffer, int attriLoc, uint32 stride, uint32 offset ) {
	int i = -1;
	doFor(k, attriLocs.size()) {
		if (attriLocs[k] == attriLoc) {
			logOut.log(Utils::makeStr("Warning : OGLUserMeshGPU has set attribut %d", attriLoc));
			i = k;
			break;
		}
	}
	if (i < 0) {
		vertexBuffers.push_back(pbuffer);
		attriLocs.push_back(attriLoc);
		strides.push_back(stride);
		offsets.push_back(offset);
	}
	else {
		vertexBuffers[i] = pbuffer;
		strides[i] = stride;
		offsets[i] = offset;
	}
}
void OGLUserMeshGPU::addVertexData(pVertexBuffer pbuffer, const std::string& attriName,uint32 stride,uint32 offset) {
	int i = -1;
	doFor(k, attriNames.size()) {
		if (attriNames[k] == attriName) {
			logOut.log(Utils::makeStr("Warning : OGLUserMeshGPU has set attribut %s", attriName.c_str()));
			i = k;
			break;
		}
	}
	if (i < 0) {
		vertexBuffers.push_back(pbuffer);
		attriNames.push_back(attriName);
		strides.push_back(stride);
		offsets.push_back(offset);
	}
	else {
		vertexBuffers[i] = pbuffer;
		strides[i] = stride;
		offsets[i] = offset;
	}
}
void OGLUserMeshGPU::addElementBuffer(pVertexBuffer pbuffer, GeoTYPE type) {
	geoType = type;
	elementBuffer = pbuffer;
}
void OGLUserMeshGPU::setShader(pShader tShader) {
	auto lastShader = mShader;
	if (lastShader == tShader)
		return;
	mShader = std::dynamic_pointer_cast<GLShader>(tShader);
	if (!hasCommit)
		commit();
}
void OGLUserMeshGPU::commitPure() {
	if (mVaoId == 0 || hasCommit) {
		glGenVertexArrays(1, &mVaoId);
	}
	glBindVertexArray(mVaoId);
	int tLoc = 0;
	doFor(k, attriLocs.size()) {
		auto loc = attriLocs[k];// locMaps[attriNames[k]];
		vertexBuffers[k]->applay();
		glVertexAttribPointer(loc, strides[k], vertexBuffers[k]->getGPUBaseDataType(), GL_FALSE,offsets[k], nullptr);
		glEnableVertexAttribArray(loc);
		if (loc == 0)
			drawNum = vertexBuffers[k]->getDataLen() / DTypeUtil::getSize(vertexBuffers[k]->getBaseDataType());
		GLUtils::checkGLError("glSetAttribPoint");
	}
	if (elementBuffer) {
		elementBuffer->applay();
		GLUtils::checkGLError("glSetElement");
		drawNum = elementBuffer->getDataLen() / DTypeUtil::getSize(elementBuffer->getBaseDataType());
	}
	glBindVertexArray(0);
	hasCommit = true;
}
void OGLUserMeshGPU::commit() {
	P3D_ENSURE(mShader, Utils::makeStr("OGLUserMesh commit: should set shader"));
	if (mVaoId == 0 || hasCommit)
		glGenVertexArrays(1, &mVaoId);
	glBindVertexArray(mVaoId);
	doFor(k, attriNames.size()) {
		auto loc = mShader->getAttriLoc(attriNames[k]);
		if (loc >= 0) {
			vertexBuffers[k]->applay();
			glVertexAttribPointer(loc, strides[k], vertexBuffers[k]->getGPUBaseDataType(), GL_FALSE,offsets[k], nullptr);
			glEnableVertexAttribArray(loc);
			if (loc == 0)
				drawNum = vertexBuffers[k]->getDataLen() / DTypeUtil::getSize(vertexBuffers[k]->getBaseDataType());
		}

	}
	if (elementBuffer) {
		elementBuffer->applay();
		drawNum = elementBuffer->getDataLen() / DTypeUtil::getSize(elementBuffer->getBaseDataType());
	}
	glBindVertexArray(0);
	hasCommit = true;

}
void OGLUserMeshGPU::drawPure() {
	glBindVertexArray(mVaoId);
	if (elementBuffer) {
		glDrawElements(OGLGEOTYPE[geoType], drawNum, elementBuffer->getGPUBaseDataType(), (void*)0);
	}
	else {
		glDrawArrays(OGLGEOTYPE[geoType], 0, drawNum);
	}
	glBindVertexArray(0);
}

void OGLUserMeshGPU::draw() {
	if (mShader == nullptr) {
		return drawPure();
	}
	if (skinInfo!=nullptr) {
		mShader->addUniform(std::make_shared<ShaderUniform>(SHADER_KEY_WORDS::U_BONE_MATS, skinInfo->flattenMats));
	}
	if (tMaterial != nullptr) {
		auto tUnis = tMaterial->apply();
		for (auto tu : tUnis)
			mShader->addUniform(tu);
	}

	mShader->setUniformMat(SHADER_KEY_WORDS::MODEL_MAT, modelMat);
	mShader->applay();

	glBindVertexArray(mVaoId);
	if (elementBuffer) {
		glDrawElements(OGLGEOTYPE[geoType], drawNum, elementBuffer->getGPUBaseDataType(), (void*)0);
	}
	else {
		glDrawArrays(OGLGEOTYPE[geoType], 0, drawNum);	}
	glBindVertexArray(0);
	mShader->applay0();
}

void OGLUserMeshGPU::update(pDataBlock pData) {
	for (int i = 0; i < attriNames.size(); ++i) {
		if (attriNames[i] == "InPos") {
			vertexBuffers[i]->update(pData,0);
		}
	}
}

void OGLUserMeshGPU::update(pDataBlock pData, const std::string& attriName) {
	if (pData == nullptr)
		return;
	for (int i = 0; i < attriNames.size(); ++i) {
		if (attriNames[i] == attriName) {
			vertexBuffers[i]->update(pData, 0);
			return;
		}
	}
	for (int i = 0; i < attriLocs.size(); ++i) {
		if (attriLocs[i] == locMaps[attriName])
			vertexBuffers[i]->update(pData, 0); {
			return;
		}
	}
}

END_P3D_NS

