#pragma once
//r0-m0
#define CROP_COORD_0_0() \
	int x1 = x; int y1 = y;
#define CROP_STEP_0_0() int step = 1;

//r0-m1
#define CROP_COORD_0_1() \
	int x1 = srect_width1-x; int y1 = y;
#define CROP_STEP_0_1() int step = -1;

//r90-m0
#define CROP_COORD_1_0() \
	int x1 = y; int y1 = srect_width1-x;
#define CROP_STEP_1_0() int step = -dst_width;

//r90-m1
#define CROP_COORD_1_1() \
	int x1 = srect_height1-y; int y1 = srect_width1-x;
#define CROP_STEP_1_1() int step = -dst_width;

//r180-m0
#define CROP_COORD_2_0() \
	int x1 = srect_width1-x; int y1 = srect_height1-y;
#define CROP_STEP_2_0() int step = -1;

//r180-m1
#define CROP_COORD_2_1() \
	int x1 = x; int y1 = srect_height1-y;
#define CROP_STEP_2_1() int step = 1;

//r270-m0
#define CROP_COORD_3_0() \
	int x1 = srect_height1-y; int y1 = x;
#define CROP_STEP_3_0() int step = dst_width;

//r270-m1
#define CROP_COORD_3_1() \
	int x1 = y; int y1 = x;
#define CROP_STEP_3_1() int step = dst_width;

#define DO_CROP_LOOP_T(cond, TP) \
{\
	CROP_STEP_ ## cond(); \
	for (int y = srect_y_begin; y < srect_y_end; y++) { \
		const TP* p_src = (src_rect.y+y)*src_size.width+(src_rect.x+srect_x_begin)+src; \
		int x = srect_x_begin; \
		CROP_COORD_ ## cond(); \
		TP* p_dst1 = pdst + y1*dst_width + x1; \
		for (x = srect_x_begin; x < srect_x_end; x++) { \
			*p_dst1 = *(p_src++); \
			p_dst1 += step; \
		} \
	}\
}
