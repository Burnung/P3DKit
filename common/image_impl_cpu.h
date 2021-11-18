#pragma once
#include "faceArithBase.h"
BEGIN_P3D_NS
void img_resize_bilinear_grey_float_c(float *dst, int dst_width, int dst_height,
	const float *src, int src_width, int src_height);
void crop_rect(const uint8_t* src, IMAGE_FORMAT src_format, Size src_size,
	Rect src_rect, uint8_t padding, IMAGE_ROTATION rot, IMAGE_MIRROR mirror,
	uint8_t* dst, IMAGE_FORMAT dst_format);
void crop_rect_grey(const uint8_t* src, Size src_size, Rect src_rect, uint8_t padding,
	IMAGE_ROTATION rot, IMAGE_MIRROR mirror, uint8_t* dst);

//src_format in {grey,nv21,nv12}
//dstFormat in {grey_norm_float}
PNdArray crop_to_array_grey_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	RMRect rect, Size dstSize, IMAGE_FORMAT dstFormat,
    double defFillValue, IMAGE_INTERPOLATION interMethod);
PNdArray crop_to_array_grey_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat,
    double defFillValue, IMAGE_INTERPOLATION interMethod);

//src_format in {nv21,nv12}
//dstFormat in {rgb,rgb_norm_float,bgr,bgr_norm_float,yuv0}
PNdArray nv_crop_to_array_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	RMRect rect, Size dstSize, IMAGE_FORMAT dstFormat,
	double defFillValue, IMAGE_INTERPOLATION interMethod);
PNdArray nv_crop_to_array_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat,
	double defFillValue, IMAGE_INTERPOLATION interMethod);

// 参考cv2.warpAffine, padding为0
// transmat为(2, 3)矩阵, 输出ndarray的shape=(dsize.height, dsize.width), 算法会返回新内存对象
// 对应关系为： 设原图点为(x1, y1), x1~[0, arr1.shape[1]-1] y1~[0, arr1.shape[0]-1], 
//            结果图对应点为(x2, y2) ，x2~[0, dsize.width-1] y2~[0, dsize.height-1]
//            x2 = x1 * transmat[0][0] + y1 * transmat[0][1] + transmat[0][2];
//            y2 = x1 * transmat[1][0] + y1 * transmat[1][1] + transmat[1][2];
PNdArray warp_affine_float_general(const float* src_data, Size _src_size, int cn, std::array<float,6> transmat, Size dsize);
END_P3D_NS
