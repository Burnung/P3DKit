#include "Image.h"
#include "NdArray.h"
//#include "IModel.h"
#include "image_impl_cpu.h"
using namespace std;

BEGIN_P3D_NS
const double Image::DefaultFillValue = HUGE_VAL;

Image::Image(const Image& img) 
	: Image(img._data, img._src_size.width, img._src_size.height, img._format, img._rotation, img._mirror, img._dataHolder)
{
	calcSize();
}

void Image::doRM(ImageRM rm) {
	auto newRM = ImageRM::time(rm, ImageRM(_rotation, _mirror));
	_rotation = newRM.rot;
	_mirror = newRM.mirror;
	calcSize();
}

void Image::doRM(IMAGE_ROTATION rot, IMAGE_MIRROR mirror) {
	auto newRM = ImageRM::time(ImageRM(rot, mirror), ImageRM(_rotation, _mirror));
	_rotation = newRM.rot;
	_mirror = newRM.mirror;
	calcSize();
}

PNdArray Image::cropToArray(RMRect rect, Size dstSize, IMAGE_FORMAT dstFormat, double defFillValue, IMAGE_INTERPOLATION interMethod) const {
	uint8_t* src_data = (uint8_t*)_data;
    switch (dstFormat) {
        case IMAGE_FORMAT_GREY_NORM_FLOAT:
        case IMAGE_FORMAT_GREY:
            return crop_to_array_grey_general(src_data, _format, _src_size,
                                              _rotation, _mirror,
                                              rect, dstSize, dstFormat, defFillValue, interMethod);
        case IMAGE_FORMAT_RGB_NORM_FLOAT:
		case IMAGE_FORMAT_RGB:
        case IMAGE_FORMAT_BGR_NORM_FLOAT:
		case IMAGE_FORMAT_BGR:
		case IMAGE_FORMAT_YUV0:
			return nv_crop_to_array_general(src_data, _format, _src_size,
				_rotation, _mirror,
				rect, dstSize, dstFormat, defFillValue, interMethod);
        default:
            throw std::invalid_argument("unsupported dstFormat");
    }
}

PNdArray Image::cropToArray(Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat, double defFillValue, IMAGE_INTERPOLATION interMethod) const { //todo
	uint8_t* src_data = (uint8_t*)_data;
	switch (dstFormat) {
	case IMAGE_FORMAT_GREY_NORM_FLOAT:
	case IMAGE_FORMAT_GREY:
		return crop_to_array_grey_general(src_data, _format, _src_size,
			_rotation, _mirror,
			rect, dstSize, dstFormat, defFillValue, interMethod);
	case IMAGE_FORMAT_RGB_NORM_FLOAT:
	case IMAGE_FORMAT_RGB:
	case IMAGE_FORMAT_BGR_NORM_FLOAT:
	case IMAGE_FORMAT_BGR:
	case IMAGE_FORMAT_YUV0:
		return nv_crop_to_array_general(src_data, _format, _src_size,
			_rotation, _mirror,
			rect, dstSize, dstFormat, defFillValue, interMethod);
	default:
		throw std::invalid_argument("unsupported dstFormat");
	}
}

//currently, we just use RMRect cropping
PNdArray Image::cropToArray(RectF rect, Size dstSize, IMAGE_FORMAT dstFormat,
	IMAGE_ROTATION rectRotate, IMAGE_MIRROR rectMirror, double defFillValue, IMAGE_INTERPOLATION interMethod) const{
	RMRect rmrect;
	{
		auto invTrans = ImageRM(rectRotate, rectMirror).inv();
		Vec2f center(rect.x + rect.width / 2, rect.y + rect.height / 2);
		rmrect.size = SizeF(rect.width, rect.height).t(rectRotate);
		rmrect.center = center;
		switch (invTrans.rot) {
		case IMAGE_ROT90_CCW:
			rmrect.angle = -C_PI/2;
			break;
		case IMAGE_ROT180:
			rmrect.angle = C_PI;
			break;
		case IMAGE_ROT90_CW:
			rmrect.angle = C_PI/2;
			break;
		default:
		    break;
		}
		rmrect.mirror = invTrans.mirror;
		if (rmrect.mirror == IMAGE_MIRROR_YES) { //note: for RMRect: mirror->rotate, for ImageRM: rotate->mirror
			rmrect.angle *= -1;
		}
	}
	return cropToArray(rmrect, dstSize, dstFormat, defFillValue, interMethod);
}
/*
Image::DstImageOpt ImageUtil::getInputImageOpt(const PModel& model) {
	auto shape = model->getInputShape(0);
	auto type = model->getInputType(0);
	int height, width, cn;
	bool includeBatch;
	if (shape.size() == 4) {
		height = shape[1];
		width = shape[2];
		cn = shape[3];
		includeBatch = true;
	}
	else if (shape.size() == 3) {
		height = shape[0];
		width = shape[1];
		cn = shape[2];
		includeBatch = false;
	}
	else throw invalid_argument("ImgUtil::getDstImageOpt: input image shape must be 3 or 4");
	auto fmt = model->getInputImageFormat(0);
	if (fmt == IMAGE_FORMAT_NONE) {
		if (cn == 3 && type == DType::Float32)
			fmt = IMAGE_FORMAT_RGB_NORM_FLOAT;
		else if (cn == 3 && type == DType::UInt8) {
			fmt = IMAGE_FORMAT_RGB;
		} else if (cn == 4 && type == DType::UInt8) //todo: use metadata instead of guess
			fmt = IMAGE_FORMAT_YUV0;
		else if (cn == 1 && type == DType::Float32)
			fmt = IMAGE_FORMAT_GREY_NORM_FLOAT;
		else if (cn == 1 && type == DType::UInt8)
			fmt = IMAGE_FORMAT_GREY;
		else
			P3D_THROW_RUNTIME("ImgUtil::getDstImageOpt: Invalid input, channel %d, type %s", cn, DTypeUtil::toString(type).c_str());
	}
	return { Size(width,height), fmt, includeBatch ? shape[0] : 0 };
}
*/
END_P3D_NS
