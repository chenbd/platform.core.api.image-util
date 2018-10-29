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

#include <image_util_private.h>

#include <mm_util_imgp.h>
#include <mm_util_jpeg.h>
#include <mm_util_png.h>
#include <mm_util_gif.h>
#include <mm_util_bmp.h>

static int _convert_colorspace_tbl[] = {
	MM_UTIL_COLOR_YUV420,		/* IMAGE_UTIL_COLORSPACE_YUV420 */
	MM_UTIL_COLOR_YUV422,		/* IMAGE_UTIL_COLORSPACE_YUV422 */
	MM_UTIL_COLOR_I420,			/* IMAGE_UTIL_COLORSPACE_I420 */
	MM_UTIL_COLOR_NV12,			/* IMAGE_UTIL_COLORSPACE_NV12 */
	MM_UTIL_COLOR_UYVY,			/* IMAGE_UTIL_COLORSPACE_UYVY */
	MM_UTIL_COLOR_YUYV,			/* IMAGE_UTIL_COLORSPACE_YUYV */
	MM_UTIL_COLOR_RGB16,		/* IMAGE_UTIL_COLORSPACE_RGB565 */
	MM_UTIL_COLOR_RGB24,		/* IMAGE_UTIL_COLORSPACE_RGB888 */
	MM_UTIL_COLOR_ARGB,			/* IMAGE_UTIL_COLORSPACE_ARGB8888 */
	MM_UTIL_COLOR_BGRA,			/* IMAGE_UTIL_COLORSPACE_BGRA8888 */
	MM_UTIL_COLOR_RGBA,			/* IMAGE_UTIL_COLORSPACE_RGBA8888 */
	MM_UTIL_COLOR_BGRX,			/* IMAGE_UTIL_COLORSPACE_BGRX8888 */
	MM_UTIL_COLOR_NV21,			/* IMAGE_UTIL_COLORSPACE_NV12 */
	MM_UTIL_COLOR_NV16,			/* IMAGE_UTIL_COLORSPACE_NV16 */
	MM_UTIL_COLOR_NV61,			/* IMAGE_UTIL_COLORSPACE_NV61 */
};

static int _convert_jpeg_colorspace_tbl[] = {
	MM_UTIL_COLOR_YUV420,		/* IMAGE_UTIL_COLORSPACE_YUV420 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUV422 */
	MM_UTIL_COLOR_YUV420,		/* IMAGE_UTIL_COLORSPACE_I420 */
	MM_UTIL_COLOR_NV12,			/* IMAGE_UTIL_COLORSPACE_NV12 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_UYVY */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUYV */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_RGB565 */
	MM_UTIL_COLOR_RGB24,		/* IMAGE_UTIL_COLORSPACE_RGB888 */
	MM_UTIL_COLOR_ARGB,			/* IMAGE_UTIL_COLORSPACE_ARGB8888 */
	MM_UTIL_COLOR_BGRA,			/* IMAGE_UTIL_COLORSPACE_BGRA8888 */
	MM_UTIL_COLOR_RGBA,			/* IMAGE_UTIL_COLORSPACE_RGBA8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_BGRX8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV21 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV16 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV61 */
};

static int _convert_png_colorspace_tbl[] = {
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUV420 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUV422 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_I420 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV12 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_UYVY */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUYV */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_RGB565 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_RGB888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_ARGB8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_BGRA8888 */
	MM_UTIL_COLOR_RGBA,			/* IMAGE_UTIL_COLORSPACE_RGBA8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_BGRX8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV21 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV16 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV61 */
};

static int _convert_gif_colorspace_tbl[] = {
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUV420 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUV422 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_I420 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV12 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_UYVY */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUYV */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_RGB565 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_RGB888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_ARGB8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_BGRA8888 */
	MM_UTIL_COLOR_RGBA,			/* IMAGE_UTIL_COLORSPACE_RGBA8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_BGRX8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV21 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV16 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV61 */
};

static int _convert_bmp_colorspace_tbl[] = {
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUV420 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUV422 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_I420 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV12 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_UYVY */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_YUYV */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_RGB565 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_RGB888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_ARGB8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_BGRA8888 */
	MM_UTIL_COLOR_RGBA,			/* IMAGE_UTIL_COLORSPACE_RGBA8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_BGRX8888 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV21 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV16 */
	_NOT_SUPPORTED_COLORSPACE,	/* IMAGE_UTIL_COLORSPACE_NV61 */
};

unsigned int get_number_of_colorspace(void)
{
	return (unsigned int)(sizeof(_convert_colorspace_tbl)/sizeof(int));
}

gboolean is_valid_colorspace(image_util_colorspace_e colorspace)
{
	if ((colorspace < IMAGE_UTIL_COLORSPACE_YV12) || (colorspace >= NUM_OF_COLORSPACE))
		return FALSE;

	return TRUE;
}

gboolean is_supported_colorspace(image_util_colorspace_e colorspace, image_util_type_e type)
{
	switch (type) {
	case IMAGE_UTIL_JPEG:
		image_util_retvm_if((_convert_jpeg_colorspace_tbl[colorspace] == _NOT_SUPPORTED_COLORSPACE), FALSE, "[jpeg] %d not supported format", colorspace);
		break;
	case IMAGE_UTIL_PNG:
		image_util_retvm_if((_convert_png_colorspace_tbl[colorspace] == _NOT_SUPPORTED_COLORSPACE), FALSE, "[png] %d not supported format", colorspace);
		break;
	case IMAGE_UTIL_GIF:
		image_util_retvm_if((_convert_gif_colorspace_tbl[colorspace] == _NOT_SUPPORTED_COLORSPACE), FALSE, "[gif] %d not supported format", colorspace);
		break;
	case IMAGE_UTIL_BMP:
		image_util_retvm_if((_convert_bmp_colorspace_tbl[colorspace] == _NOT_SUPPORTED_COLORSPACE), FALSE, "[bmp] %d not supported format", colorspace);
		break;
	default:
		image_util_retvm_if(TRUE, FALSE, "Invalid image type");
	}
	return TRUE;
}

int convert_type_of_colorspace(const image_util_colorspace_e colorspace)
{
	if (is_valid_colorspace(colorspace) == FALSE)
		return _NOT_SUPPORTED_COLORSPACE;

	return _convert_colorspace_tbl[colorspace];
}

int convert_type_of_colorspace_with_image_type(const image_util_colorspace_e colorspace, const image_util_type_e type)
{
	int new_colorspace = _NOT_SUPPORTED_COLORSPACE;

	if (is_valid_colorspace(colorspace) == FALSE)
		return _NOT_SUPPORTED_COLORSPACE;

	if (is_supported_colorspace(colorspace, type) == FALSE)
		return _NOT_SUPPORTED_COLORSPACE;

	switch (type) {
	case IMAGE_UTIL_JPEG:
		new_colorspace = _convert_jpeg_colorspace_tbl[colorspace];
		break;
	case IMAGE_UTIL_PNG:
		new_colorspace = _convert_png_colorspace_tbl[colorspace];
		break;
	case IMAGE_UTIL_GIF:
		new_colorspace = _convert_gif_colorspace_tbl[colorspace];
		break;
	case IMAGE_UTIL_BMP:
		new_colorspace = _convert_bmp_colorspace_tbl[colorspace];
		break;
	default:
		image_util_error("Invalid image type");
	}

	return new_colorspace;
}

int _image_error_capi(image_util_error_type_e error_type, int error_code)
{
	if (error_code != MM_UTIL_ERROR_NONE)
		image_util_error("[type : %d] error : %d ", error_type, error_code);

	/*Error None*/
	if (error_code == MM_UTIL_ERROR_NONE)
		return IMAGE_UTIL_ERROR_NONE;

	/* Invalid parameter error*/
	else if (error_code == MM_UTIL_ERROR_INVALID_PARAMETER)
		return IMAGE_UTIL_ERROR_INVALID_PARAMETER;

	else if (error_code == MM_UTIL_ERROR_OUT_OF_MEMORY)
		return IMAGE_UTIL_ERROR_OUT_OF_MEMORY;

	/* No such file  error*/
	else if (error_code == MM_UTIL_ERROR_NO_SUCH_FILE)
		return IMAGE_UTIL_ERROR_NO_SUCH_FILE;

	/* Internal operation error*/
	else if (error_code == MM_UTIL_ERROR_INVALID_OPERATION)
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;

	/* Not supported format error*/
	else if (error_code == MM_UTIL_ERROR_NOT_SUPPORTED_FORMAT)
		return IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT;

	/*ETC*/
	return IMAGE_UTIL_ERROR_INVALID_OPERATION;

}

bool _image_util_check_resolution(int width, int height)
{
	if (width <= 0) {
		image_util_error("invalid width [%d]", width);
		return false;
	}

	if (height <= 0) {
		image_util_error("invalid height [%d]", height);
		return false;
	}

	return true;
}
