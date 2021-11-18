#include "OGLVbo.h"
#include "OGLShader.h"
BEGIN_P3D_NS


void GLVbo::release() {
	vpsBuffer = nullptr;
	uvsBuffer = nullptr;
	vnsBuffer = nullptr;
	if (vao != 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}
GLVbo::~GLVbo() {
	release();
}
void GLVbo::init(pDataBlock vps, pDataBlock tl, pDataBlock uvs, pDataBlock vns) {

	release();
#define VBOINITHELPER(srcData,dataBuffer) \
		if(srcData != nullptr){ \
			dataBuffer = std::make_shared<OGLVertexBuffer>();\
			dataBuffer->init(VERTEX_BUFFER_ARRAY, DYNAMIC_DRAW,srcData->getDataType(), srcData);\
		}
	VBOINITHELPER(vps, vpsBuffer);
	VBOINITHELPER(uvs, uvsBuffer);
	VBOINITHELPER(vns, vnsBuffer);

	tlBuffer = std::make_shared<OGLVertexBuffer>();
	tlBuffer->init(VERTEX_BUFFER_ELEMETN, STATIC_DRAW,tl->getDataType(), tl);
	nTri = tl->getSize() / 3 / DTypeUtil::getSize(tl->getDataType());

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

#define VAOHELPER(vbo,loc,s) \
		if(vbo != 0 && loc != GL_INVALID_INDEX){\
			vbo->applay();\
			glVertexAttribPointer(loc, s, vbo->getGPUBaseDataType(), GL_FALSE,0, nullptr);\
			glEnableVertexAttribArray(loc);\
		}
	VAOHELPER(vpsBuffer, attrLoc[LOC_VPS],3);
	VAOHELPER(uvsBuffer, attrLoc[LOC_UVS], 2);
	VAOHELPER(vnsBuffer, attrLoc[LOC_VNS], 3);
	tlBuffer->applay();
	glBindVertexArray(0);
	elementDataType = tlBuffer->getGPUBaseDataType();
}

void GLVbo::update(pDataBlock vps, pDataBlock uvs /* = nullptr */, pDataBlock vns /* = nullptr */) {

#define VBOUPDATEHELPER(srcData,dataBuffer) \
		if(srcData != nullptr){\
			if(dataBuffer == nullptr){\
				dataBuffer = std::make_shared<OGLVertexBuffer>();\
				dataBuffer->init(VERTEX_BUFFER_ARRAY, DYNAMIC_DRAW,DType::Float32, srcData); \
			}\
			else {\
					dataBuffer->update(srcData);\
			}\
		}
	VBOUPDATEHELPER(vps, vpsBuffer);
	VBOUPDATEHELPER(uvs, uvsBuffer);
	VBOUPDATEHELPER(vns, vnsBuffer);
}

void GLVbo::update(pDataBlock tData, const std::string& attriName) {
	if (tData == nullptr)
		return;
	if (attriName == "InPos") {
		vpsBuffer->update(tData);
	}
	else if (attriName == "InUv")
		uvsBuffer->update(tData);
	else if (attriName == "InVn")
		vnsBuffer->update(tData);
}

 void GLVbo::drawPure() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 3 * nTri,elementDataType , (void*)0);
	glBindVertexArray(0);
}
void GLVbo::draw() {
	mShader->setUniformMat(SHADER_KEY_WORDS::MODEL_MAT, modelMat);
	mShader->applay();
	//glEnable(GL_DEPTH_TEST);
//	glDisable(GL_BLEND);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 3 * nTri,elementDataType , (void*)0);
	glBindVertexArray(0);
//	glDrawElements(GL_TRIANGLES, 3 ,elementDataType , (void*)0);
}

void GLVbo::setShader(pShader tShader) {
	mShader = std::dynamic_pointer_cast<GLShader>(tShader);
	doFor(k, LOC_NUM) {
		attrLoc[k] = mShader->getAttriLoc(ATTRINAME[k]);
	}
}

END_P3D_NS