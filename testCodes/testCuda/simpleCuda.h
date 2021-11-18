#pragma once
#include <GL/glew.h>
#include "engine/P3dEngine.h"
#include "common/NdArray.h"
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <helper/helper_cuda.h>
#include <helper/helper_timer.h>
#include <cudnn.h>


#define FatalError(s) {                                                \
    std::stringstream _where, _message;                                \
    _where << __FILE__ << ':' << __LINE__;                             \
    _message << std::string(s) + "\n" << __FILE__ << ':' << __LINE__;\
    std::cerr << _message.str() << "\nAborting...\n";                  \
    cudaDeviceReset();                                                 \
    exit(EXIT_FAILURE);                                                \
}


#define checkCUDNN(status) {                                           \
    std::stringstream _error;                                          \
    if (status != CUDNN_STATUS_SUCCESS) {                              \
      _error << "CUDNN failure\nError: " << cudnnGetErrorString(status); \
      FatalError(_error.str());                                        \
    }                                                                  \
}

void initCudaGL(int argc,char** argv);
cudaGraphicsResource_t putGLTexture2Cuda(P3D::pTexture pTex);

class CudnnTensor {
public:
	CudnnTensor() = default;
	~CudnnTensor();
	void initfromGLTex(P3D::pTexture pTex,int useChannel,bool isNCWH=true);
	void init(int n, int c, int h, int w,bool isNCWH = true,bool mallocData=true);
	void update();
	bool sizeEqual(std::shared_ptr<CudnnTensor> lp);
	void transform(bool isNCWH);
	P3D::pDataBlock dump();

public:
	static std::shared_ptr<CudnnTensor> concat(const std::vector<std::shared_ptr<CudnnTensor>>& inputs);
	static void concat(const std::vector<std::shared_ptr<CudnnTensor>>& inputs,std::shared_ptr<CudnnTensor> ret);

private:
	void initPbo();

public:
	int n, c, w, h;
	P3D::uint32 pboId = 0;
	P3D::pTexture srcTex;
	size_t dataSize = 0;
	bool isNCWH = false;
	cudnnTensorDescriptor_t des, transDes = nullptr;
	cudaGraphicsResource_t res = nullptr;
	bool holdDev = false, holdtrans = false;
	float* devPtr = nullptr, *transPtr = nullptr;
	cudaArray_t devArrayPtr;
};

typedef std::shared_ptr<CudnnTensor> pTensor;

class CudnnFilter {
public:
	CudnnFilter() = default;
	~CudnnFilter();
	void init(int inChannel,int outChannel,int h,int w,bool isNCWH=true);
	void setValue(ksu::PNdArray arr);

public:
	bool isNCWH = false;
	cudnnFilterDescriptor_t des;
	int outChannel, inChannel, k1, k2;
	size_t dataSize;
	float* devPtr = nullptr;

};
typedef std::shared_ptr<CudnnFilter> pFilter;

class CudnnOpt {
public:
	CudnnOpt() = default;
	virtual ~CudnnOpt() = default;
	virtual pTensor forward(pTensor input) = 0;
	virtual pTensor preRun(pTensor tTensor) = 0;

public:
	float alpha = 1.0;
	float beta = 0.0;

};
typedef std::shared_ptr<CudnnOpt> pOpt;

class CudnnConvOpt:public CudnnOpt {
public:
	CudnnConvOpt() = default;
	virtual ~CudnnConvOpt();
public:
	void init(int inChannel,int outChannel,int kSize,int padding,int stride,int delition,bool isNCWH = false);
	void setValue(ksu::PNdArray pData);
	virtual	pTensor forward(pTensor tensor) override;
	virtual pTensor  preRun(pTensor input) override;
public:
	bool isNCWH = false;
	int inChannel, outChannel, kSize, padding, stride;

	cudnnConvolutionDescriptor_t des ;
	pFilter mFilter;
	pTensor outTensor;
	float* pWorkSpace = nullptr;
	size_t workSize = 0;
};
typedef std::shared_ptr<CudnnConvOpt> pConvOpt;

class CudnnActiveOpt : public CudnnOpt{
public:
	enum {
		ACTIVE_RELUE,
		ACTIVE_LEACK_RELUE,
		ACTIVE_NUM,
	};
public:
	CudnnActiveOpt() = default;
	virtual ~CudnnActiveOpt();
	void init(int type, bool re, float arg1);
	virtual pTensor preRun(pTensor tTensor) override;
	virtual pTensor forward(pTensor input) override;
public:
	int type;
	cudnnActivationDescriptor_t des;
	pTensor outTensor;

};
typedef std::shared_ptr<CudnnActiveOpt> pActiveOpt;

class CudnnNet {
public:
	CudnnNet() = default;
	void addOpt(pOpt tOpt);
	void addOpts(std::vector<pOpt> opts);
	pTensor forward(pTensor input);
	void preRun(pTensor input);
public:
	std::vector<pOpt> opts;
	pTensor outPut;
	pTensor inPut;
};
typedef std::shared_ptr<CudnnNet> pNet;
