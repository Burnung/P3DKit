#include "faceArithBase.h"
#include <math.h>
#include "Image.h"
#include "NdArray.h"
#include "imagerm_coord_utils.h"
#include "image_impl_cpu.h"

using namespace std;
BEGIN_P3D_NS

void img_resize_bilinear_grey_float_c(float *dst, int dst_width, int dst_height, 
	const float *src, int src_width, int src_height)
{
	//int dst_stride = dst_width * 4;
	int max_src_x = src_width - 1;
	int max_dst_x = dst_width - 1;
	int max_src_y = src_height - 1;
	int max_dst_y = dst_height - 1;
	float scale_x = float(max_src_x) / float(max_dst_x);
	float scale_y = float(max_src_y) / float(max_dst_y);
	for (int dst_y = 0; dst_y < dst_height; dst_y++) for (int dst_x = 0; dst_x < dst_width; dst_x++) {
		float src_x = dst_x * scale_x;
		float src_y = dst_y * scale_y;
		int x0 = (int)floorf(src_x);
		int y0 = (int)floorf(src_y);
		int x1 = min(x0 + 1, max_src_x);
		int y1 = min(y0 + 1, max_src_y);
		/*00 10
		  01 11*/
		float v00 = src[y0*src_width + x0];
		float v10 = src[y0*src_width + x1];
		float v01 = src[y1*src_width + x0];
		float v11 = src[y1*src_width + x1];
		
		float a = src_x - x0;
		float b = src_y - y0;
		float a1 = 1 - a;
		float b1 = 1 - b;
		*dst = v00*a1*b1 + v10*a*b1 + v01*a1*b + v11*a*b;
		dst++;
	}
}

#define DO_CROP_LOOP(cond) DO_CROP_LOOP_T(cond, uint8_t)

//rect: on transformed src image
void crop_rect_grey(const uint8_t* src, Size src_size, Rect src_rect, uint8_t padding,
	IMAGE_ROTATION rot, IMAGE_MIRROR mirror, uint8_t* pdst) {
	//dst.resize(src_rect.width*src_rect.height, padding);
	//if (dst.empty()) return;
	auto dst_rect = ImageRM(rot, mirror).apply(src_size, src_rect);
	int dst_width = dst_rect.width;
	int dst_height = dst_rect.height;
	int srect_width = src_rect.width;
	int srect_height = src_rect.height;
	int srect_width1 = srect_width - 1;
	int srect_height1 = srect_height - 1;
	int srect_x_begin = max(0, src_rect.x)-src_rect.x;
	int srect_x_end = min(src_rect.x+src_rect.width, src_size.width)-src_rect.x;
	int srect_y_begin = max(0, src_rect.y)-src_rect.y;
	int srect_y_end = min(src_rect.y + src_rect.height, src_size.height)-src_rect.y;
	switch ((((int)rot) << 1) + (int)mirror) {
	case 0: //r0m0
		DO_CROP_LOOP(0_0)
		break;
	case 1: //r0m1
		DO_CROP_LOOP(0_1)
		break;
	case 2: //r1m0
		DO_CROP_LOOP(1_0)
		break;
	case 3: //r1m1
		DO_CROP_LOOP(1_1)
		break;
	case 4: //r2m0
		DO_CROP_LOOP(2_0)
		break;
	case 5: //r2m1
		DO_CROP_LOOP(2_1)
		break;
	case 6: //r3m0
		DO_CROP_LOOP(3_0)
		break;
	case 7: //r3m1
		DO_CROP_LOOP(3_1)
		break;
	}
}

void crop_rect(const uint8_t* src, IMAGE_FORMAT src_format, Size src_size,
	Rect src_rect, uint8_t padding, IMAGE_ROTATION rot, IMAGE_MIRROR mirror, 
	uint8_t* pdst, IMAGE_FORMAT dst_format) {
	if (dst_format != IMAGE_FORMAT_GREY) {
		throw invalid_argument("unsupported dst format");
	}
	crop_rect_grey(src, src_size, src_rect, padding, rot, mirror, pdst);
}

template<typename trait>
inline typename trait::PixelBlock _get_img_color(uint8_t* ptr, int width, int height, int x, int y,
                                                 typename trait::TDst defval) {
    typename trait::PixelBlock block = trait::init_pixelblock(defval);
    if (x < 0 || x >= width || y < 0 || y >= height)
        return block;
    trait::store_pixel_block(block, ptr[y*width + x]);
    return block;
}

template<typename trait>
PNdArray crop_to_array_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	Parallelogram rect1, Size dstSize, IMAGE_FORMAT dstFormat,
    double defFillValue, IMAGE_INTERPOLATION interMethod) {
	typedef typename trait::TDst TDst;
	if (src_format != IMAGE_FORMAT_GREY && src_format != IMAGE_FORMAT_NV21 && src_format != IMAGE_FORMAT_NV12) {
		throw invalid_argument("only support grey/nv21/nv12 for grey crop");
	}
	if (dstFormat != IMAGE_FORMAT_GREY_NORM_FLOAT && dstFormat != IMAGE_FORMAT_GREY) {
		throw invalid_argument("currently only support grey/norm_grey");
	}
	//full length
	auto lvx = rect1.lvx;
	auto lvy = rect1.lvy;
	//step length
	int dwidth = dstSize.width;
	int dheight = dstSize.height;
	auto vx = lvx / float(dwidth);
	auto vy = lvy / float(dheight);
	//start point
	auto sp = rect1.startPos + (vx + vy) / 2.0f;
	PNdArrayT<TDst> rlt({ dheight, dwidth, 1 }, false);
	auto prlt = rlt.data();
	//cache
	int _src_width = _src_size.width;
	int _src_height = _src_size.height;
    TDst defval;
    if (Image::isDefaultFillValue(defFillValue))
        defval = trait::get_defval();
    else
        defval = (TDst)defFillValue;
	if (interMethod == IMAGE_INTERP_BILINEAR) {
		for (int y = 0; y < dheight; y++) {
			Vec2f cp = sp + vy * float(y);
			for (int x = 0; x < dwidth; x++) {
				int x0 = (int)floorf(cp.x);
				int y0 = (int)floorf(cp.y);
				int x1 = x0 + 1;
				int y1 = y0 + 1;
				/*00 10
				01 11*/
				typename trait::PixelBlock c00 = _get_img_color<trait>(src_data, _src_width, _src_height, x0, y0,
                                                                       defval);
				typename trait::PixelBlock c10 = _get_img_color<trait>(src_data, _src_width, _src_height, x1, y0,
                                                                       defval);
				typename trait::PixelBlock c01 = _get_img_color<trait>(src_data, _src_width, _src_height, x0, y1,
                                                                       defval);
				typename trait::PixelBlock c11 = _get_img_color<trait>(src_data, _src_width, _src_height, x1, y1,
                                                                       defval);
				float a = cp.x - x0;
				float b = cp.y - y0;
				float a1 = 1 - a;
				float b1 = 1 - b;
				*(prlt++) = TDst(c00 * a1*b1 + c10 * a*b1 + c01 * a1*b + c11 * a*b);
				cp += vx;
			}
		}
	}
	else if (interMethod == IMAGE_INTERP_NN) {
		for (int y = 0; y < dheight; y++) {
			Vec2f cp = sp + vy * float(y);
			for (int x = 0; x < dwidth; x++) {
				int x0 = roundi(cp.x);
				int y0 = roundi(cp.y);
				typename trait::PixelBlock c00 = _get_img_color<trait>(src_data, _src_width, _src_height, x0, y0,
                                                                       defval);
				*(prlt++) = TDst(c00);
				cp += vx;
			}
		}
	}
	else {
		throw invalid_argument("unknown interpolation method");
	}
	return rlt;
}

template<typename trait>
PNdArray crop_to_array_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size, 
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	RMRect rect, Size dstSize, IMAGE_FORMAT dstFormat,
    double defFillValue, IMAGE_INTERPOLATION interMethod) {
	auto _size = _src_size.t(_rotation);
	auto rect1 = ImageRM(_rotation, _mirror).inv().apply(_size, rect);
	return crop_to_array_general<trait>(src_data, src_format, _src_size, rect1.toParallelogram(),
		dstSize, dstFormat, defFillValue, interMethod);
}

template<typename trait>
PNdArray crop_to_array_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat,
    double defFillValue, IMAGE_INTERPOLATION interMethod) {
	auto _size = _src_size.t(_rotation);
	auto rect1 = ImageRM(_rotation, _mirror).inv().apply(_size, rect);
	return crop_to_array_general<trait>(src_data, src_format, _src_size, rect1,
		dstSize, dstFormat, defFillValue, interMethod);
}

struct NV21toGREYNorm {
public:
    typedef float TDst;
    typedef float PixelBlock;
    static TDst get_defval() {
        return 0.0;
    }
    static PixelBlock const init_pixelblock(float defval = 0.0) {
        return defval;
    }
    static void store_pixel_block(PixelBlock& block, uint8_t data) {
        block = ((float)data - 127.0f) / 128.0f;
    }
};

struct NV21toGREY {
public:
    typedef uint8_t TDst;
    typedef uint8_t PixelBlock;
    static TDst get_defval() {
        return 127;
    }
    static PixelBlock const init_pixelblock(uint8_t defval = 127) {
        return defval;
    }
    static void store_pixel_block(PixelBlock& block, uint8_t data) {
        block = data;
    }
};

PNdArray crop_to_array_grey_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
                                    IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
                                    RMRect rect, Size dstSize, IMAGE_FORMAT dstFormat,
                                    double defFillValue,
                                    IMAGE_INTERPOLATION interMethod) {
    if (dstFormat == IMAGE_FORMAT_GREY_NORM_FLOAT)
        return crop_to_array_general<NV21toGREYNorm>(src_data, src_format, _src_size, _rotation, _mirror,
                                                    rect, dstSize, dstFormat, defFillValue, interMethod);
    else if (dstFormat == IMAGE_FORMAT_GREY)
        return crop_to_array_general<NV21toGREY>(src_data, src_format, _src_size, _rotation, _mirror,
                                                rect, dstSize, dstFormat, defFillValue, interMethod);
    else
        throw invalid_argument("unsupported dstFormat");
}

PNdArray crop_to_array_grey_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat,
    double defFillValue, IMAGE_INTERPOLATION interMethod) {
	if (dstFormat == IMAGE_FORMAT_GREY_NORM_FLOAT)
		return crop_to_array_general<NV21toGREYNorm>(src_data, src_format, _src_size, _rotation, _mirror,
			rect, dstSize, dstFormat, defFillValue, interMethod);
	else if (dstFormat == IMAGE_FORMAT_GREY)
		return crop_to_array_general<NV21toGREY>(src_data, src_format, _src_size, _rotation, _mirror,
			rect, dstSize, dstFormat, defFillValue, interMethod);
	else
		throw invalid_argument("unsupported dstFormat");
}
END_P3D_NS
