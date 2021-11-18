#pragma once
#include "P3D_Utils.h"
#include "faceArithBase.h"
#include "NdArray.h"

BEGIN_P3D_NS
class Image : public P3DObject {
	public:
		static const double DefaultFillValue;

		struct DstImageOpt {
			Size size;
			IMAGE_FORMAT format;
			int batch;
		};
		Image(): _data(nullptr), _format(IMAGE_FORMAT_NONE), _rotation(IMAGE_ROT0), _mirror(IMAGE_MIRROR_NO)
		{}
		Image(void* data, int src_width, int src_height, IMAGE_FORMAT format, 
			IMAGE_ROTATION rotation, IMAGE_MIRROR mirror,
			std::shared_ptr<P3DObject> dataHolder = nullptr):
			_data(data), _src_size(src_width, src_height),
			_format(format), _rotation(rotation), _mirror(mirror), _dataHolder(dataHolder) {
			calcSize();
		}
		Image(const Image& img);
		~Image()
		{}

		static bool isDefaultFillValue(double v) { return v > 1000; }

		bool isEmpty() const { return _data == nullptr; }

        PNdArray cropToArray(RectF rect, Size dstSize, IMAGE_FORMAT dstFormat,
			IMAGE_ROTATION rectRotate = IMAGE_ROT0, IMAGE_MIRROR rectMirror = IMAGE_MIRROR_NO, double defFillValue = DefaultFillValue, IMAGE_INTERPOLATION interMethod = IMAGE_INTERP_BILINEAR) const; //transform after crop

		PNdArrayF cropToArrayF(RectF rect, Size dstSize, IMAGE_FORMAT dstFormat,
			IMAGE_ROTATION rectRotate = IMAGE_ROT0, IMAGE_MIRROR rectMirror = IMAGE_MIRROR_NO, double defFillValue = DefaultFillValue, IMAGE_INTERPOLATION interMethod = IMAGE_INTERP_BILINEAR) const { //transform after crop
			return cropToArray(rect, dstSize, dstFormat, rectRotate, rectMirror, defFillValue, interMethod).asFloat();
		}

		PNdArray cropToArray(RMRect rect, Size dstSize, IMAGE_FORMAT dstFormat, double defFillValue = DefaultFillValue, IMAGE_INTERPOLATION interMethod = IMAGE_INTERP_BILINEAR) const;

		PNdArray cropToArray(Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat, double defFillValue = DefaultFillValue, IMAGE_INTERPOLATION interMethod = IMAGE_INTERP_BILINEAR) const;

		PNdArrayF cropToArrayF(RMRect rect, Size dstSize, IMAGE_FORMAT dstFormat, double defFillValue = DefaultFillValue, IMAGE_INTERPOLATION interMethod = IMAGE_INTERP_BILINEAR) const{
			return cropToArray(rect, dstSize, dstFormat, defFillValue, interMethod).asFloat();
		}

		PNdArrayF cropToArrayF(Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat, double defFillValue = DefaultFillValue, IMAGE_INTERPOLATION interMethod = IMAGE_INTERP_BILINEAR) const {
			return cropToArray(rect, dstSize, dstFormat, defFillValue, interMethod).asFloat();
		}

		PNdArrayF cropToArrayF(RMRect rect, DstImageOpt opt, double defFillValue = DefaultFillValue, IMAGE_INTERPOLATION interMethod = IMAGE_INTERP_BILINEAR) const {
			return cropToArrayF(rect, opt.size, opt.format, defFillValue, interMethod);
		}

		PNdArrayF cropToArrayF(Parallelogram rect, DstImageOpt opt, double defFillValue = DefaultFillValue, IMAGE_INTERPOLATION interMethod = IMAGE_INTERP_BILINEAR) const {
			return cropToArrayF(rect, opt.size, opt.format, defFillValue, interMethod);
		}

		void writeBinFile(cstring fpath);
		void readBinFile(cstring fpath);
		IMAGE_FORMAT getFormat() { return _format; }
	public:
		//debug do not call
		void doRM(IMAGE_ROTATION rot, IMAGE_MIRROR mirror = IMAGE_MIRROR_NO);
		void doRM(ImageRM rm);
		IMAGE_ROTATION rotation() const { return _rotation; }
		void setRM(IMAGE_ROTATION rot, IMAGE_MIRROR mirror) { _rotation = rot; _mirror = mirror; calcSize(); }
		IMAGE_MIRROR mirror() const { return _mirror; }
		Size srcSize() const { return _src_size; }

		int width() const { return _size.width; }
		int height() const { return _size.height; }
		Size size() const { return _size; }

		uint8_t* rawData() { return (uint8_t*)_data; }
		const uint8_t* rawData() const { return (uint8_t*)_data; }
	private:
		void calcSize() {
			_size = ImageRM(_rotation, _mirror).apply(_src_size);
		}
		static int calcDataSize(IMAGE_FORMAT format, int width, int height);
		int calcDataSize();
	private:
		void* _data;
		std::shared_ptr<P3DObject> _dataHolder;
		Size _size;
		Size _src_size;
		IMAGE_FORMAT _format;
		IMAGE_ROTATION _rotation;
		IMAGE_MIRROR _mirror;
};

struct ImageUtil {
	//todo: change to getImageCropOpt
	//static Image::DstImageOpt getInputImageOpt(const PModel& model);
};
typedef  std::shared_ptr<Image> pImage;
END_P3D_NS
