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

#include <gmodule.h>
#include <inttypes.h>
#include <mm_util_imgp.h>
#include <mm_util_common.h>

#include <image_util.h>
#include <image_util_private.h>

typedef struct {
	mm_util_color_format_e image_format;
	media_format_mimetype_e mimetype;
	const char *mimetype_name;
} image_format_mimetype_pair_s;

static const image_format_mimetype_pair_s image_format_mimetype_table[MM_UTIL_COLOR_NUM] = {
	{ MM_UTIL_COLOR_YUV420,	MEDIA_FORMAT_YV12,		"MEDIA_FORMAT_YV12" },
	{ MM_UTIL_COLOR_YUV422,	MEDIA_FORMAT_422P,		"MEDIA_FORMAT_422P" },
	{ MM_UTIL_COLOR_I420,		MEDIA_FORMAT_I420,		"MEDIA_FORMAT_I420" },
	{ MM_UTIL_COLOR_NV12,		MEDIA_FORMAT_NV12,						"Not support" },
	{ MM_UTIL_COLOR_UYVY,		MEDIA_FORMAT_UYVY,		"MEDIA_FORMAT_UYVY" },
	{ MM_UTIL_COLOR_YUYV,		MEDIA_FORMAT_YUYV,		"MEDIA_FORMAT_YUYV" },
	{ MM_UTIL_COLOR_RGB16,	MEDIA_FORMAT_RGB565,	"MEDIA_FORMAT_RGB565" },
	{ MM_UTIL_COLOR_RGB24,	MEDIA_FORMAT_RGB888,	"MEDIA_FORMAT_RGB888" },
	{ MM_UTIL_COLOR_ARGB,	MEDIA_FORMAT_ARGB,		"MEDIA_FORMAT_ARGB" },
	{ MM_UTIL_COLOR_BGRA,	MEDIA_FORMAT_BGRA,		"MEDIA_FORMAT_BGRA" },
	{ MM_UTIL_COLOR_RGBA,	MEDIA_FORMAT_RGBA,		"MEDIA_FORMAT_RGBA" },
	{ MM_UTIL_COLOR_BGRX,	-1,						"Not support" },
	{ MM_UTIL_COLOR_NV12_TILED,	MEDIA_FORMAT_NV12T,	"MEDIA_FORMAT_NV12T" },
	{ MM_UTIL_COLOR_NV16,		MEDIA_FORMAT_NV16,		"MEDIA_FORMAT_NV16" },
	{ MM_UTIL_COLOR_NV61,		-1,						"Not support" }
};

static media_format_mimetype_e __image_format_to_mimetype(mm_util_color_format_e format)
{
	unsigned int i = 0;
	media_format_mimetype_e mimetype = -1;

	for (i = 0; i < MM_UTIL_COLOR_NUM; i++) {
		if (image_format_mimetype_table[i].image_format == format) {
			mimetype = image_format_mimetype_table[i].mimetype;
			break;
		}
	}

	image_util_debug("imgp fmt: %d mimetype fmt: %s", format, image_format_mimetype_table[i].mimetype_name);

	return mimetype;
}

static mm_util_color_format_e __mimetype_to_image_format(media_format_mimetype_e mimetype)
{
	unsigned int i = 0;
	mm_util_color_format_e format = -1;

	for (i = 0; i < MM_UTIL_COLOR_NUM; i++) {
		if (image_format_mimetype_table[i].mimetype == mimetype) {
			format = image_format_mimetype_table[i].image_format;
			break;
		}
	}

	image_util_debug("mimetype: %s imgp fmt: %d", image_format_mimetype_table[i].mimetype_name, format);

	return format;
}

static int __create_media_format(media_format_mimetype_e mimetype, unsigned int width, unsigned int height, media_format_h *new_fmt)
{
	int err = MEDIA_FORMAT_ERROR_NONE;

	image_util_retvm_if((new_fmt == NULL) || (width == 0) || (height == 0), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid parameter");

	err = media_format_create(new_fmt);
	image_util_retvm_if((err != MEDIA_FORMAT_ERROR_NONE), IMAGE_UTIL_ERROR_INVALID_OPERATION, "media_format_make_writable failed (%d)", err);

	err = media_format_set_video_mime(*new_fmt, mimetype);
	if (err != MEDIA_FORMAT_ERROR_NONE) {
		media_format_unref(*new_fmt);
		image_util_error("media_format_set_video_mime failed (%d)", err);
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	err = media_format_set_video_width(*new_fmt, width);
	if (err != MEDIA_FORMAT_ERROR_NONE) {
		media_format_unref(*new_fmt);
		image_util_error("media_format_set_video_width failed (%d)", err);
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	err = media_format_set_video_height(*new_fmt, height);
	if (err != MEDIA_FORMAT_ERROR_NONE) {
		media_format_unref(*new_fmt);
		image_util_error("media_format_set_video_height failed (%d)", err);
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	return IMAGE_UTIL_ERROR_NONE;
}

static int _image_util_packet_to_image(media_packet_h packet, mm_util_color_image_h *color_image)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	media_format_mimetype_e mimetype = 0;
	int width = 0, height = 0;
	uint64_t size = 0;
	void *ptr = NULL;
	media_format_h fmt = NULL;

	image_util_retvm_if(((packet == NULL) || (color_image == NULL)), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid parameter");

	err = media_packet_get_format(packet, &fmt);
	image_util_retvm_if((err != MEDIA_PACKET_ERROR_NONE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "media_packet_get_format failed (%d)", err);

	err = media_format_get_video_info(fmt, &mimetype, &width, &height, NULL, NULL);
	if (err != MEDIA_FORMAT_ERROR_NONE) {
		image_util_error("media_packet_get_format failed (%d)", err);
		media_format_unref(fmt);
		return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
	}
	media_format_unref(fmt);

	err = media_packet_get_buffer_size(packet, &size);
	image_util_retvm_if((err != MEDIA_PACKET_ERROR_NONE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "media_packet_get_buffer_size failed (%d)", err);

	if (size) {
		err = media_packet_get_buffer_data_ptr(packet, &ptr);
		image_util_retvm_if((err != MEDIA_PACKET_ERROR_NONE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "media_packet_get_buffer_data_ptr failed (%d)", err);
	}

	image_util_debug("[Fotmat: %u] W x H : %d x %d", mimetype, width, height);
	image_util_retvm_if(((width == 0) || (height == 0) || (size == 0) || (ptr == NULL)), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid source packet");

	err = mm_util_create_color_image(color_image, (unsigned long)width, (unsigned long)height, __mimetype_to_image_format(mimetype), ptr, (size_t)size);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_TRANSFORM, err), "mm_util_create_color_image failed (%d)", err);

	image_util_debug("_image_util_packet_to_image succeed");

	return IMAGE_UTIL_ERROR_NONE;
}

static int _image_util_image_to_packet(mm_util_color_image_h image, media_packet_h *packet)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	mm_util_color_format_e format = 0;
	unsigned long width = 0, height = 0;
	void *buffer = NULL;
	size_t buffer_size = 0;
	media_format_h fmt = NULL;
	void *packet_ptr = NULL;
	uint64_t packet_size = 0;
	size_t size = 0;

	err = mm_util_get_color_image(image, &width, &height, &format, &buffer, &buffer_size);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_TRANSFORM, err), "mm_util_get_color_image failed (%d)", err);

	err = __create_media_format(__image_format_to_mimetype(format), (unsigned int)width, (unsigned int)height, &fmt);
	image_util_retvm_if((err != IMAGE_UTIL_ERROR_NONE), err, "__create_media_format failed (%d)", err);

	err = media_packet_create_alloc(fmt, NULL, NULL, packet);
	if (err != MEDIA_PACKET_ERROR_NONE) {
		image_util_error("media_packet_create_alloc failed (%d)", err);
		media_format_unref(fmt);
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	err = media_packet_get_buffer_size(*packet, &packet_size);
	if (err != MEDIA_PACKET_ERROR_NONE) {
		image_util_error("media_packet_get_buffer_size failed (%d)", err);
		media_packet_destroy(*packet);
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	err = media_packet_get_buffer_data_ptr(*packet, &packet_ptr);
	if (err != MEDIA_PACKET_ERROR_NONE) {
		image_util_error("media_packet_get_buffer_data_ptr failed");
		media_packet_destroy(*packet);
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	if (packet_ptr == NULL || packet_size == 0) {
		image_util_error("media_packet creation failed (%p, %" PRIu64 ")", packet_ptr, packet_size);
		media_packet_destroy(*packet);
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}
	image_util_debug("Success - media_packet is created (%p, %" PRIu64 ")", packet_ptr, packet_size);

	if ((uint64_t)buffer_size < packet_size) {
		size = (size_t)buffer_size;
	} else {
		size = (size_t)packet_size;
	}

	image_util_debug("Size: result(%u) media_packet(%" PRIu64 ") copied(%zu)", buffer_size, packet_size, size);
	memcpy(packet_ptr, buffer, size);

	image_util_debug("_image_util_image_to_packet succeed");

	return IMAGE_UTIL_ERROR_NONE;
}

static void _image_util_transform_completed_cb(mm_util_color_image_h raw_image, int error, void *user_data)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	image_util_cb_s *_util_cb = (image_util_cb_s *) user_data;
	media_packet_h packet = NULL;

	if ((_util_cb != NULL) && (_util_cb->completed_cb != NULL)) {
		err = _image_util_image_to_packet(raw_image, &packet);
		if (err != MM_UTIL_ERROR_NONE) {
			image_util_error("_image_util_image_to_packet failed (%d)", err);
			_util_cb->completed_cb(NULL, err, _util_cb->user_data);
		} else {
			_util_cb->completed_cb(&packet, _image_error_capi(ERR_TYPE_TRANSFORM, error), _util_cb->user_data);
		}
	}

	return;
}

static int _image_util_create_transform_handle(transformation_s * handle)
{
	int err = MM_UTIL_ERROR_NONE;
	mm_util_imgp_h image_h;

	err = mm_util_create(&image_h);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_TRANSFORM, err), "Error - mm_util_create");

	handle->image_h = image_h;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_create(transformation_h * handle)
{
	int err = MM_UTIL_ERROR_NONE;

	image_util_fenter();

	image_util_retvm_if((handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	transformation_s *_handle = (transformation_s *) calloc(1, sizeof(transformation_s));
	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "OUT_OF_MEMORY");

	_handle->colorspace = _NOT_SUPPORTED_COLORSPACE;
	_handle->_util_cb = NULL;
	_handle->image_h = NULL;
	_handle->set_convert = false;
	_handle->set_resize = false;
	_handle->set_rotate = false;
	_handle->set_crop = false;

	err = _image_util_create_transform_handle(_handle);
	if (err != MM_UTIL_ERROR_NONE) {
		image_util_error("Error - create transform handle");
		IMAGE_UTIL_SAFE_FREE(_handle);
		return _image_error_capi(ERR_TYPE_TRANSFORM, err);
	}

	*handle = (transformation_h) _handle;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_set_hardware_acceleration(transformation_h handle, bool mode)
{
	transformation_s *_handle = (transformation_s *) handle;

	image_util_warning("DEPRECATION WARNING: image_util_transform_set_hardware_acceleration() is deprecated and will be removed from next release.");
	image_util_debug("Set hardware_acceleration %d", mode);

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
#ifndef ENABLE_HW_ACCELERATION
	image_util_retvm_if((mode == true), IMAGE_UTIL_ERROR_NOT_SUPPORTED, "hardware acceleration is not supported");
#endif

	image_util_debug("Set hardware_acceleration %d", mode);

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_set_colorspace(transformation_h handle, image_util_colorspace_e colorspace)
{
	int err = MM_UTIL_ERROR_NONE;
	transformation_s *_handle = (transformation_s *) handle;

	image_util_debug("Set colorspace_convert_info [%d]", colorspace);

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	err = mm_util_set_colorspace_convert(_handle->image_h, colorspace);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_TRANSFORM, err), "Error - Set colorspace convert");

	_handle->colorspace = colorspace;
	_handle->set_convert = true;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_set_resolution(transformation_h handle, unsigned int width, unsigned int height)
{
	int err = MM_UTIL_ERROR_NONE;
	transformation_s *_handle = (transformation_s *) handle;

	image_util_debug("Set resize_info w[%d] h[%d]", width, height);

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((_handle->set_crop), IMAGE_UTIL_ERROR_INVALID_OPERATION, "Crop and Resize can't do at the same time");
	image_util_retvm_if((_image_util_check_resolution(width, height) == false), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid resolution");

	err = mm_util_set_resolution(_handle->image_h, width, height);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_TRANSFORM, err), "Error - Set resolution");

	_handle->width = width;
	_handle->height = height;
	_handle->set_resize = true;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_set_rotation(transformation_h handle, image_util_rotation_e rotation)
{
	int err = MM_UTIL_ERROR_NONE;
	transformation_s *_handle = (transformation_s *) handle;

	image_util_debug("Set rotate_info [%d]", rotation);

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	err = mm_util_set_rotation(_handle->image_h, rotation);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_TRANSFORM, err), "Error - Set rotation");

	_handle->rotation = rotation;
	_handle->set_rotate = true;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_set_crop_area(transformation_h handle, unsigned int start_x, unsigned int start_y, unsigned int end_x, unsigned int end_y)
{
	int err = MM_UTIL_ERROR_NONE;
	transformation_s *_handle = (transformation_s *) handle;
	int dest_width;
	int dest_height;

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((_handle->set_resize), IMAGE_UTIL_ERROR_INVALID_OPERATION, "Crop and Resize can't do at the same time");

	dest_width = end_x - start_x;
	dest_height = end_y - start_y;

	image_util_debug("Set crop_info x[%d] y[%d] w[%d] h[%d]", start_x, start_y, dest_width, dest_height);

	image_util_retvm_if((_image_util_check_resolution(dest_width, dest_height) == false), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid dest resolution");
	err = mm_util_set_crop_area(_handle->image_h, start_x, start_y, end_x, end_y);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_TRANSFORM, err), "Error - Set crop area");

	_handle->start_x = start_x;
	_handle->start_y = start_y;
	_handle->end_x = end_x;
	_handle->end_y = end_y;
	_handle->set_crop = true;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_get_colorspace(transformation_h handle, image_util_colorspace_e * colorspace)
{
	transformation_s *_handle = (transformation_s *) handle;

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((colorspace == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "colorspace parameter error");

	if (!_handle->set_convert) {
		image_util_error("Did not set colorspace before");
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	image_util_debug("Get colorspace_convert_info [%d]", _handle->colorspace);

	*colorspace = _handle->colorspace;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_get_resolution(transformation_h handle, unsigned int *width, unsigned int *height)
{
	transformation_s *_handle = (transformation_s *) handle;

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((width == NULL || height == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "width or height parameter error");

	if (!_handle->set_resize) {
		image_util_error("Did not set resolution before");
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	image_util_debug("Get resize_info w[%ui] h[%ui]", _handle->width, _handle->height);

	*width = _handle->width;
	*height = _handle->height;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_get_rotation(transformation_h handle, image_util_rotation_e * rotation)
{
	transformation_s *_handle = (transformation_s *) handle;

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((rotation == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "rotation parameter error");

	if (!_handle->set_rotate) {
		image_util_error("Did not set rotation before");
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	image_util_debug("Get rotate_info [%d]", _handle->rotation);

	*rotation = _handle->rotation;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_get_crop_area(transformation_h handle, unsigned int *start_x, unsigned int *start_y, unsigned int *end_x, unsigned int *end_y)
{
	transformation_s *_handle = (transformation_s *) handle;

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((start_x == NULL || start_y == NULL || end_x == NULL || end_y == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "crop area parameter error");

	if (!_handle->set_crop) {
		image_util_error("Did not set crop area before");
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	*start_x = _handle->start_x;
	*start_y = _handle->start_y;
	*end_x = _handle->end_x;
	*end_y = _handle->end_y;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_run(transformation_h handle, media_packet_h src, image_util_transform_completed_cb completed_cb, void *user_data)
{
	int err = MM_UTIL_ERROR_NONE;
	transformation_s *_handle = (transformation_s *) handle;

	image_util_fenter();

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((completed_cb == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid callback");
	image_util_retvm_if((src == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid source");
	image_util_retvm_if((!_handle->set_convert && !_handle->set_resize && !_handle->set_rotate && !_handle->set_crop), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid transform");

	mm_util_color_image_h color_image = NULL;

	err = _image_util_packet_to_image(src, &color_image);
	image_util_retvm_if((err != IMAGE_UTIL_ERROR_NONE), err, "_image_util_packet_to_image failed");

	_handle->_util_cb = (image_util_cb_s *) calloc(1, sizeof(image_util_cb_s));
	if (_handle->_util_cb == NULL) {
		image_util_error("Memory allocation failed");
		mm_util_destroy_color_image(color_image);
		return IMAGE_UTIL_ERROR_OUT_OF_MEMORY;
	}
	_handle->_util_cb->user_data = user_data;
	_handle->_util_cb->completed_cb = completed_cb;

	err = mm_util_transform(_handle->image_h, color_image, (mm_util_completed_callback) _image_util_transform_completed_cb, (void *)_handle->_util_cb);
	if (err != MM_UTIL_ERROR_NONE) {
		image_util_error("Error - Run transform (%d)", err);
		mm_util_destroy_color_image(color_image);
		return _image_error_capi(ERR_TYPE_TRANSFORM, err);
	}

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_transform_destroy(transformation_h handle)
{
	int err = MM_UTIL_ERROR_NONE;
	transformation_s *_handle = (transformation_s *) handle;

	image_util_fenter();

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	err = mm_util_destroy(_handle->image_h);

	IMAGE_UTIL_SAFE_FREE(_handle->_util_cb);
	IMAGE_UTIL_SAFE_FREE(_handle);

	return _image_error_capi(ERR_TYPE_TRANSFORM, err);
}

int image_util_calculate_buffer_size(int width, int height, image_util_colorspace_e colorspace, unsigned int *size)
{
	int err = MM_UTIL_ERROR_NONE;
	size_t size_ptr = 0;

	image_util_retvm_if((is_valid_colorspace(colorspace) == FALSE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid colorspace");
	image_util_retvm_if((width <= 0 || height <= 0), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid width or Invalid height");
	image_util_retvm_if((size == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "size is null");

	err = mm_util_get_image_size(TYPECAST_COLOR(colorspace), width, height, &size_ptr);

	*size = (unsigned int)size_ptr;

	return _image_error_capi(ERR_TYPE_COMMON, err);
}

int image_util_extract_color_from_memory(const unsigned char *image_buffer, int width, int height, unsigned char *rgb_r, unsigned char *rgb_g, unsigned char *rgb_b)
{
	int ret = MM_UTIL_ERROR_NONE;

	image_util_retvm_if((image_buffer == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "image_buffer is null");

	GModule *module = NULL;
	ModuleFunc mmutil_imgcv_module_func = NULL;
	module = g_module_open(PATH_MMUTIL_IMGCV_LIB, G_MODULE_BIND_LAZY);
	image_util_retvm_if((module == NULL), IMAGE_UTIL_ERROR_NO_SUCH_FILE, "fail to open module");

	if (!g_module_symbol(module, IMGCV_FUNC_NAME, (gpointer *)&mmutil_imgcv_module_func)) {
		image_util_error("fail to g_module_symbol");
		g_module_close(module);

		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	if (!mmutil_imgcv_module_func)
		g_module_close(module);

	image_util_retvm_if((mmutil_imgcv_module_func == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "fail to get symbol");

	unsigned char r_color, g_color, b_color;
	ret = mmutil_imgcv_module_func((void *)image_buffer, width, height, &r_color, &g_color, &b_color);

	*rgb_r = r_color;
	*rgb_g = g_color;
	*rgb_b = b_color;

	if (module) {
		g_module_close(module);
		module = NULL;
	}

	return _image_error_capi(ERR_TYPE_COMMON, ret);
}

int image_util_foreach_supported_colorspace(image_util_type_e image_type, image_util_supported_colorspace_cb callback, void *user_data)
{
	int idx = 0;

	IMAGE_UTIL_TYPE_CHECK(image_type);
	image_util_retvm_if((callback == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "callback is null");

	for (idx = (int)(NUM_OF_COLORSPACE - 1); idx >= 0; idx--) {
		if (is_supported_colorspace(idx, image_type))
			if (false == callback(idx, user_data))
				return IMAGE_UTIL_ERROR_NONE;

	}

	return IMAGE_UTIL_ERROR_NONE;
}
