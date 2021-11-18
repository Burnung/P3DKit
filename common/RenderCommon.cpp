//
// Created by bunring on 19-3-12.
//

#include "RenderCommon.h"

BEGIN_P3D_NS
CpuImage::CpuImage(PNdArray arrData,bool isbgr) {
	isBGR = isbgr;
	uint32 nchannel = 1;
	if (arrData.shape().size() == 3) {
		nchannel = arrData.shape()[2];
	}
	switch (nchannel)
	{
	case 1:
	{
		if (arrData.dtype() == DType::UInt8) {
			pixelFormat = PIXEL_R8;

		}
		else if (arrData.dtype() == DType::Float32)
			pixelFormat = PIXEL_DEPTHF;
		break;
	}
	case 3:
	{
		if (arrData.dtype() == DType::UInt8)
			pixelFormat = isBGR? PIXEL_BGR8 : PIXEL_RGB8;
		else if (arrData.dtype() == DType::Float32)
			pixelFormat = isBGR? PIXEL_BGRF: PIXEL_RGBF;
		break;
	}
	case 4:
	{
		if (arrData.dtype() == DType::UInt8)
			pixelFormat = isBGR? PIXEL_BGRA8 : PIXEL_RGBA8;
		else if (arrData.dtype() ==  DType::Float32)
			pixelFormat = isBGR? PIXEL_BGRAF: PIXEL_RGBAF;
		break;
	}
	default:
		P3D_THROW_ARGUMENT("invalid ndarry format");
	}
	width = arrData.shape()[1];
	height = arrData.shape()[0];
	depth = 1;
    buffers.push_back(arrData);
}

CpuImage::CpuImage(TextureType t,PixelFormat format, uint32 w,uint32 h,uint32 d,bool useuint8):width(w),height(h),\
                                        depth(d),type(t),pixelFormat(format){
    uint32 unitSize = 1;

    if(useuint8){
        if(format == PIXEL_RGBAF)
            pixelFormat = PIXEL_RGBA8;
        else if(format == PIXEL_RGBF)
            pixelFormat = PIXEL_RGB8;
    }
    else{
        if(pixelFormat == PIXEL_RGBAF || pixelFormat == PIXEL_RGBF || pixelFormat == PIXEL_BGRAF || pixelFormat == PIXEL_BGRF)
            unitSize = 4;
		//else(pixelFormat == PIXEL_BGR8 || pixelFormat == )
    }
    if(pixelFormat == PIXEL_DEPTHF){
        unitSize = 4;
        nchannel = 1;
    }
    else if(pixelFormat == PIXEL_RGBA8 || pixelFormat == PIXEL_RGBAF)
        nchannel = 4;

    bufferSize = unitSize * nchannel* w * h * d;
	auto dataType = DType::UInt8;
	if (unitSize == 4)
		dataType = DType::Float32;
	else if (unitSize == 8)
		dataType = DType::Float64;
    
    P3D_ENSURE(type != TEXTURE_CUBE,"tex cube type not support");

	auto tbuffer = PNdArray(dataType, {int(h),int(w),int(nchannel)},false);
    buffers.push_back(tbuffer);
}
    

uint8* CpuImage::getRawData(int index){
    P3D_ENSURE(index < buffers.size(),"cpuImage:: invalid index");
    return (uint8*)(buffers[index].rawData());
}

void CpuImage::setFromBuffer(uint8* data,uint32 index) {
	buffers[index].getDataBlock()->setFromMem(data, bufferSize);
}
END_P3D_NS


