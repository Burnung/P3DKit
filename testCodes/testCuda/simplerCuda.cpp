#include "simpleCuda.h"
#include <cudnn.h>
#include "common/P3D_Utils.h"
static cudnnHandle_t mCunnHandel;

void initCudaGL(int argc,char** argv) {
	auto devId = findCudaDevice(argc,(const char**)argv);
	checkCudaErrors(cudaGLSetGLDevice(devId));
	checkCUDNN(cudnnCreate(&mCunnHandel));
}

cudaGraphicsResource_t putGLTexture2Cuda(P3D::pTexture pTex) {
	cudaGraphicsResource_t ret;
	checkCudaErrors(cudaGraphicsGLRegisterImage(&ret, pTex->getHandler(), GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone));
	return ret;
}

void CudnnTensor::initPbo() {
	if (pboId)
		glDeleteBuffers(1, &pboId);
	glGenBuffers(1, &pboId);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboId);
	glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, nullptr, GL_DYNAMIC_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}
void CudnnTensor::initfromGLTex(P3D::pTexture pTex,int useChannle,bool isNCWH) {
	n = 1, c = useChannle, h = pTex->Height(), w = pTex->Width();

	srcTex = pTex;
	init(n, c, h, w, false, false);
	initPbo();

	checkCudaErrors(cudaGraphicsGLRegisterBuffer(&res,pboId, cudaGraphicsRegisterFlagsReadOnly));

	this->isNCWH = isNCWH;
	if (this->isNCWH) {
		std::swap( transDes,des);
		if (!des) {
			checkCUDNN(cudnnCreateTensorDescriptor(&des));
			checkCUDNN(cudnnSetTensor4dDescriptor(des, CUDNN_TENSOR_NCHW, CUDNN_DATA_FLOAT, n, c, h, w));
		}
	}
	update();
}
void CudnnTensor::init(int n, int c, int h, int w,bool isNCWH,bool isMalloc) {
	this->isNCWH = isNCWH;
	this->n = n, this->c = c, this->h = h, this->w = w;
	checkCUDNN(cudnnCreateTensorDescriptor(&des));
	checkCUDNN(cudnnSetTensor4dDescriptor(des, isNCWH?CUDNN_TENSOR_NCHW:CUDNN_TENSOR_NHWC,CUDNN_DATA_FLOAT,n,c,h,w));
	dataSize = n * c * w * h * sizeof(float);
	if (isMalloc) {
		if (devPtr != nullptr)
			checkCudaErrors(cudaFree(devPtr));
		checkCudaErrors(cudaMalloc((void**)&devPtr, dataSize));
		holdDev = true;
	}

}
void CudnnTensor::transform(bool isNCWH) {
	if (isNCWH == this->isNCWH)
		return;
	this->isNCWH = isNCWH;
	if (!transDes) {
		checkCUDNN(cudnnCreateTensorDescriptor(&transDes));
		checkCUDNN(cudnnSetTensor4dDescriptor(transDes, isNCWH ? CUDNN_TENSOR_NCHW : CUDNN_TENSOR_NHWC, CUDNN_DATA_FLOAT, n, c, h, w));
	}

	if (transPtr && !holdtrans && srcTex && !this->isNCWH) {
		std::swap(transPtr, devPtr);
		std::swap(holdDev, holdtrans);
		std::swap(transDes, des);
		return;
	}

	if(!holdtrans)
		checkCudaErrors(cudaMalloc((void**)&transPtr, dataSize));

	float alpha = 1.0, beta = 0.0;
	checkCUDNN(cudnnTransformTensor(mCunnHandel, &alpha, des, (const void*)devPtr, &beta, transDes, (void*)transPtr));

	std::swap(transDes, des);
	std::swap(transPtr, devPtr);
	holdtrans = holdDev;
	holdDev = true;

}

void CudnnTensor::update() {
	if (!srcTex || ! pboId)
		return;
	srcTex->applyShader(0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboId);
	glGetTexImage(GL_TEXTURE_2D, 0,c==4?GL_RGBA : GL_RGB , GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);

	checkCudaErrors(cudaGraphicsMapResources(1, &res, 0));
	size_t tmpSize;
	float*tmpPtr;
	checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void**)&tmpPtr, &tmpSize, res));
	if (!isNCWH) {
		devPtr = tmpPtr;
		holdDev = false;
	}
	else {
		float alpha = 1.0, beta = 0.0;
		if (!devPtr && !holdDev) {
			checkCudaErrors(cudaMalloc((void**)&devPtr, dataSize));
		}
		if (!transDes) {
			checkCUDNN(cudnnCreateTensorDescriptor(&transDes));
			checkCUDNN(cudnnSetTensor4dDescriptor(transDes, CUDNN_TENSOR_NHWC, CUDNN_DATA_FLOAT, n, c, h, w));
		}
		checkCUDNN(cudnnTransformTensor(mCunnHandel, &alpha, transDes, (const void*)tmpPtr, &beta, des, (void*)devPtr));
		holdDev = true;
		transPtr = tmpPtr;
		holdtrans = false;
	}
//	checkCudaErrors(cudaGraphicsUnmapResources(1, &res, 0));

}

P3D::pDataBlock CudnnTensor::dump() {
	if (!devPtr)
		return nullptr;

	auto ret = std::make_shared<P3D::DataBlock>(dataSize);
	checkCudaErrors(cudaMemcpy((void*)(ret->rawData()),(const void*)devPtr,dataSize,cudaMemcpyDeviceToHost));
	return ret;
}
CudnnTensor::~CudnnTensor() {
	if (des)
		checkCUDNN(cudnnDestroyTensorDescriptor(des));
	if(transDes)
		checkCUDNN(cudnnDestroyTensorDescriptor(transDes));
	if (holdDev && devPtr)
		checkCudaErrors(cudaFree(devPtr));
	if (holdtrans && transPtr)
		checkCudaErrors(cudaFree(transPtr));
}


//static void concat(const std::vector<std::shared_ptr<CudnnTensor>>& inputs,std::shared_ptr<CudnnTensor> ret);
void CudnnTensor::concat(const std::vector<std::shared_ptr<CudnnTensor>>& inputs,std::shared_ptr<CudnnTensor> ret) {
	if (ret == nullptr) {
		P3D_THROW_ARGUMENT("CudnnTensor concat error");
		return;
	}
	int srcW = inputs[0]->w, srcH = inputs[0]->h;
	int allC = 0;
	for (auto t : inputs) {
		if (t->n != 1 || t->h != srcH || t->w != srcW ) {
			P3D_THROW_ARGUMENT("CudnnTensor concat error");
			break;
		}
		allC += t->c;
	}
	if (ret->n != 1 || ret->h != srcH || ret->w != srcW || ret->c != allC) {
		P3D_THROW_ARGUMENT("CudnnTensor concat error");
		return;
	}

	size_t offset = 0;
	for (auto t : inputs) {
		checkCudaErrors(cudaMemcpy((void*)((char*)(ret->devPtr) + offset), t->devPtr, t->dataSize, cudaMemcpyDeviceToDevice));
		offset += t->dataSize;
	}
}

std::shared_ptr<CudnnTensor> CudnnTensor::concat(const std::vector<std::shared_ptr<CudnnTensor>>& inputs) {
	int srcW = inputs[0]->w, srcH = inputs[0]->h;
	int allC = 0;
	for (auto t : inputs) {
		if (t->n != 1 || t->h != srcH || t->w != srcW) {
			P3D_THROW_ARGUMENT("CudnnTensor concat error");
			break;
		}
		allC += t->c;
	}
	auto ret = std::make_shared<CudnnTensor>();
	ret->init(1, allC, srcH, srcW, true, true);

	size_t offset = 0;
	for (auto t : inputs) {
		checkCudaErrors(cudaMemcpy((void*)((char*)(ret->devPtr) + offset), t->devPtr, t->dataSize, cudaMemcpyDeviceToDevice));
		offset += t->dataSize;
	}
	return ret;
}

bool CudnnTensor::sizeEqual(std::shared_ptr<CudnnTensor> lp) {
	if (lp == nullptr)
		return false;
	return (lp->w == w) && (lp->h == h) && (lp->c == c) && (lp->n == n) && (lp->isNCWH == isNCWH);
}
void CudnnFilter::init(int inChannel,int outChannel,int h,int w, bool isNCWH) {
	this->isNCWH = isNCWH;
	this->outChannel = outChannel, this->inChannel = inChannel, k1 = h, k2 = w;
	checkCUDNN(cudnnCreateFilterDescriptor(&des));
	checkCUDNN(cudnnSetFilter4dDescriptor(des, CUDNN_DATA_FLOAT, isNCWH? CUDNN_TENSOR_NCHW:CUDNN_TENSOR_NHWC, outChannel, inChannel, h, w));

	dataSize = k1 * k2 *inChannel * outChannel * sizeof(float);
	checkCudaErrors(cudaMalloc((void**)(&devPtr), dataSize));
}

void CudnnFilter::setValue(ksu::PNdArray pArray) {
	if (devPtr == nullptr) {
		checkCudaErrors(cudaMalloc((void**)(&devPtr), this->dataSize));
	}
	checkCudaErrors(cudaMemcpy((void*)devPtr, (const void*)pArray.rawData(),this->dataSize, cudaMemcpyHostToDevice));
}

CudnnFilter::~CudnnFilter() {
	if (devPtr)
		checkCudaErrors(cudaFree(devPtr));
	if (des)
		checkCUDNN(cudnnDestroyFilterDescriptor(des));
}

//CudnnCOnv
void CudnnConvOpt::init(int inChannel,int outChannel,int k,int padding,int stride,int delition,bool isNCWH){
	this->inChannel = inChannel, this->outChannel = outChannel, kSize = k, this->padding = padding, this->stride = stride;
	checkCUDNN(cudnnCreateConvolutionDescriptor(&des));
	checkCUDNN(cudnnSetConvolution2dDescriptor(des, padding, padding, stride, stride, delition,delition, CUDNN_CONVOLUTION, CUDNN_DATA_FLOAT));
	checkCUDNN(cudnnSetConvolutionMathType(des, CUDNN_TENSOR_OP_MATH));
	mFilter = std::make_shared<CudnnFilter>();
	mFilter->init(inChannel, outChannel, k, k, isNCWH);
	this->isNCWH = isNCWH;
	outTensor = std::make_shared<CudnnTensor>();
}

void CudnnConvOpt::setValue(ksu::PNdArray pData) {
	if (mFilter)
		mFilter->setValue(pData);
}

pTensor CudnnConvOpt::preRun(pTensor tensor) {

	int ow = (tensor->w + 2 * padding - kSize) / stride + 1;
	int oh = (tensor->h + 2 * padding - kSize) / stride + 1;
	if (outTensor == nullptr || !((ow == outTensor->w) && (oh = outTensor->h) && (tensor->n == outTensor->n) && (tensor->isNCWH == outTensor->isNCWH))) {
		outTensor = std::make_shared<CudnnTensor>();
		outTensor->init(tensor->n, outChannel, oh, ow, tensor->isNCWH, true);
	}
	size_t tWorkSize = 0;
	checkCUDNN(cudnnGetConvolutionForwardWorkspaceSize(mCunnHandel, tensor->des, mFilter->des, des,
		outTensor->des, CUDNN_CONVOLUTION_FWD_ALGO_IMPLICIT_GEMM, &tWorkSize));
	if (tWorkSize != workSize || pWorkSpace == nullptr) {
		checkCudaErrors(cudaMalloc((void**)(&pWorkSpace), tWorkSize));
		workSize = tWorkSize;
	}
	return outTensor;
}

CudnnConvOpt::~CudnnConvOpt() {
	outTensor = nullptr;
	mFilter = nullptr;
	if (des)
		checkCUDNN(cudnnDestroyConvolutionDescriptor(des));

	workSize = 0;
	if (pWorkSpace)
		checkCudaErrors(cudaFree(pWorkSpace));
}
pTensor CudnnConvOpt::forward(pTensor tensor) {
	
	checkCUDNN(cudnnConvolutionForward(mCunnHandel, (const void*)&alpha, tensor->des, tensor->devPtr, mFilter->des, mFilter->devPtr, 
		des, CUDNN_CONVOLUTION_FWD_ALGO_IMPLICIT_GEMM,(void*)pWorkSpace,workSize,(const void*)&beta,outTensor->des,outTensor->devPtr));

	return outTensor;
}

// active opt
CudnnActiveOpt::~CudnnActiveOpt() {
	if (des != nullptr)
		cudnnDestroyActivationDescriptor(des);
	outTensor = nullptr;
}

void CudnnActiveOpt::init(int type, bool re, float arg1) {
	this->type = type;
	checkCUDNN(cudnnCreateActivationDescriptor(&des));
	checkCUDNN(cudnnSetActivationDescriptor(des, (cudnnActivationMode_t)type, CUDNN_NOT_PROPAGATE_NAN, arg1));
}

pTensor CudnnActiveOpt::preRun(pTensor input) {
	if (outTensor == nullptr || !input->sizeEqual(outTensor)) {
		outTensor = std::make_shared<CudnnTensor>();
		outTensor->init(input->n, input->c, input->h, input->w, input->isNCWH, true);
	}
	return outTensor;
}

pTensor CudnnActiveOpt::forward(pTensor input) {
	checkCUDNN(cudnnActivationForward(mCunnHandel, des, (const void*)&alpha, input->des, input->devPtr,
		(const void*)&beta, outTensor->des, outTensor->devPtr));
	return outTensor;
}

void CudnnNet::addOpt(pOpt opt) {
	opts.push_back(opt);
}

void CudnnNet::preRun(pTensor input) {
	auto tmp = input;
	for (auto opt : opts) {
		tmp = opt->preRun(tmp);
	}
}

void CudnnNet::addOpts(std::vector<pOpt> topts) {
	opts.insert(opts.end(), topts.begin(), topts.end());
}

pTensor CudnnNet::forward(pTensor input) {
	this->inPut = inPut;
	auto tmpData = input;
	for (auto opt : opts) {
		tmpData = opt->forward(tmpData);
	}
	outPut = tmpData;
	return outPut;
}
