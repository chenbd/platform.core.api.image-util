/*
* Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __TIZEN_MULTIMEDIA_IMAGE_UTIL_PRIVATE_H__
#define __TIZEN_MULTIMEDIA_IMAGE_UTIL_PRIVATE_H__

#include <image_util_type.h>
#include <dlog.h>
#include <stdlib.h>
#include <glib.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "CAPI_MEDIA_IMAGE_UTIL"

#define FONT_COLOR_RESET    "\033[0m"
#define FONT_COLOR_RED      "\033[31m"
#define FONT_COLOR_YELLOW   "\033[33m"

#define image_util_debug(fmt, arg...) do { \
		LOGD(FONT_COLOR_RESET""fmt""FONT_COLOR_RESET, ##arg);     \
	} while (0)

#define image_util_error(fmt, arg...) do { \
		LOGE(FONT_COLOR_RED""fmt""FONT_COLOR_RESET, ##arg);     \
	} while (0)

#define image_util_warning(fmt, arg...) do { \
		LOGW(FONT_COLOR_RED""fmt""FONT_COLOR_RESET, ##arg);     \
	} while (0)

#define image_util_fenter() do { \
			LOGD(FONT_COLOR_YELLOW"<ENTER>"FONT_COLOR_RESET); \
		} while (0)

#define image_util_fleave() do { \
			LOGD(FONT_COLOR_YELLOW"<LEAVE>"FONT_COLOR_RESET); \
		} while (0)

#define image_util_retm_if(expr, fmt, arg...) do { \
		if (expr) { \
			LOGE(FONT_COLOR_RED""fmt""FONT_COLOR_RESET, ##arg);     \
			return; \
		} \
	} while (0)

#define image_util_retvm_if(expr, val, fmt, arg...) do { \
		if (expr) { \
			LOGE(FONT_COLOR_RED""fmt""FONT_COLOR_RESET, ##arg);     \
			return (val); \
		} \
	} while (0)


#define IMAGE_UTIL_STRING_VALID(str)	\
	((str != NULL && strlen(str) > 0) ? true : false)

#define IMAGE_UTIL_SAFE_FREE(src)	{ if (src) {free(src); src = NULL; } }

#define _NOT_SUPPORTED_COLORSPACE	(-1)

#define MODE_DECODE		1
#define MODE_ENCODE		2

#define DECODE_ENCODE_CAST(obj)	((decode_encode_s *)(obj))
#define IS_DECODE_MODE(x)		(x == MODE_DECODE)
#define IS_ENCODE_MODE(x)		(x == MODE_ENCODE)

#define IMGCV_FUNC_NAME "mm_util_cv_extract_representative_color"
#define PATH_MMUTIL_IMGCV_LIB PATH_LIBDIR"/libmmutil_imgcv.so"

#define TYPECAST_COLOR(c)			convert_type_of_colorspace(c)
#define TYPECAST_COLOR_BY_TYPE(c, t)	convert_type_of_colorspace_with_image_type(c, t)

#define NUM_OF_COLORSPACE	get_number_of_colorspace()

typedef gboolean(*ModuleFunc)(void *, int, int, unsigned char *, unsigned char *, unsigned char *);

typedef struct {
	void *user_data;
	media_packet_h dst;
	image_util_transform_completed_cb completed_cb;
} image_util_cb_s;

typedef struct {
	media_packet_h src;
	media_packet_h dst;
	image_util_colorspace_e colorspace;
	image_util_rotation_e dest_rotation;
	void *image_h;
	image_util_cb_s *_util_cb;
	unsigned int width;
	unsigned int height;
	image_util_rotation_e rotation;
	unsigned int start_x;
	unsigned int start_y;
	unsigned int end_x;
	unsigned int end_y;
	bool set_convert;
	bool set_resize;
	bool set_rotate;
	bool set_crop;
} transformation_s;

typedef struct {
	void *user_data;
	image_util_decode_completed_cb image_decode_completed_cb;
} decode_cb_s;

typedef struct {
	void *user_data;
	image_util_encode_completed_cb image_encode_completed_cb;
} encode_cb_s;

typedef struct {
	image_util_type_e image_type;
	void **src_buffer;
	size_t src_size;
	void **dst_buffer;
	size_t dst_size;
	size_t gif_encode_size;
	char *path;
	void *image_h;
	unsigned long width;
	unsigned long height;
	unsigned int mode;
	int quality;
	image_util_png_compression_e compression;
	unsigned int current_buffer_count;
	unsigned int current_resolution_count;
	unsigned int current_delay_count;
	image_util_colorspace_e colorspace;
	image_util_scale_e down_scale;
	decode_cb_s *_decode_cb;
	encode_cb_s *_encode_cb;

	/* for async */
	GThread *thread;
} decode_encode_s;

typedef struct {
	void *frame_h;
} frame_s;

typedef enum {
	ERR_TYPE_COMMON,
	ERR_TYPE_TRANSFORM,
	ERR_TYPE_DECODE,
	ERR_TYPE_ENCODE,
} image_util_error_type_e;

#define IMAGE_UTIL_TYPE_CHECK(type) \
	image_util_retvm_if((type < IMAGE_UTIL_JPEG || type > IMAGE_UTIL_BMP), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid parameter")

#define IMAGE_UTIL_SUPPORT_TYPE_CHECK(value, support) \
	image_util_retvm_if((value != support), IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT, "Not supported format")

#define IMAGE_UTIL_DECODE_HANDLE_CHECK(obj) do { \
		image_util_retvm_if(obj == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid decode handle"); \
		image_util_retvm_if(!IS_DECODE_MODE(DECODE_ENCODE_CAST(obj)->mode), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid decode handle"); \
	} while(0)

#define IMAGE_UTIL_ENCODE_HANDLE_CHECK(obj) do { \
		image_util_retvm_if(obj == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid encode handle"); \
		image_util_retvm_if(!IS_ENCODE_MODE(DECODE_ENCODE_CAST(obj)->mode), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid encode handle"); \
	} while(0)

gboolean is_valid_colorspace(image_util_colorspace_e colorspace);
gboolean is_supported_colorspace(image_util_colorspace_e colorspace, image_util_type_e type);

unsigned int get_number_of_colorspace(void);
int convert_type_of_colorspace(const image_util_colorspace_e colorspace);
int convert_type_of_colorspace_with_image_type(const image_util_colorspace_e colorspace, const image_util_type_e type);
int _image_error_capi(image_util_error_type_e error_type, int error_code);
bool _image_util_check_resolution(int width, int height);

/**
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_IMAGE_UTIL_PRIVATE_H__ */
