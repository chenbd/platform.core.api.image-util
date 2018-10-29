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

#include <mm_util_imgp.h>
#include <mm_util_jpeg.h>
#include <mm_util_png.h>
#include <mm_util_gif.h>
#include <mm_util_bmp.h>

#include <image_util.h>
#include <image_util_private.h>

static int _image_util_encode_get_gif_frame(mm_gif_file_h gif_data, unsigned int index, mm_gif_image_h *frame)
{
	int err = MM_UTIL_ERROR_NONE;
	image_util_retvm_if((gif_data == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((frame == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	err = mm_util_gif_enocde_get_image_handle(gif_data, (int)index, frame);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_ENCODE, err), "mm_util_gif_enocde_get_image_handle failed %d", err);

	if (*frame == NULL) {
		err = mm_util_gif_image_create(gif_data, frame);
		image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_ENCODE, err), "mm_util_gif_image_create failed %d", err);

		err = mm_util_gif_enocde_set_image_handle(gif_data, *frame);
		if (err != MM_UTIL_ERROR_NONE) {
			image_util_error("mm_util_gif_enocde_set_image_handle is failed %d", err);
			mm_util_gif_image_destory(*frame);
			return _image_error_capi(ERR_TYPE_ENCODE, err);
		}
	}
	return _image_error_capi(ERR_TYPE_ENCODE, err);
}

static void _image_util_encode_destroy_image_handle(decode_encode_s * handle)
{
	image_util_retm_if((handle == NULL), "Invalid Handle");
	void *image_handle = (void *)(handle->image_h);
	image_util_retm_if((image_handle == NULL), "Invalid image handle");

	if (handle->image_type == IMAGE_UTIL_GIF)
		mm_util_gif_encode_destroy(image_handle);

	IMAGE_UTIL_SAFE_FREE(image_handle);
}

static int _image_util_encode_create_jpeg_handle(decode_encode_s * handle)
{
	image_util_retvm_if((handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	mm_util_jpeg_yuv_data *_handle = (mm_util_jpeg_yuv_data *) calloc(1, sizeof(mm_util_jpeg_yuv_data));
	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "OUT_OF_MEMORY");

	handle->image_h = (mm_util_imgp_h) _handle;
	handle->colorspace = IMAGE_UTIL_COLORSPACE_RGBA8888;
	handle->quality = 75;

	return IMAGE_UTIL_ERROR_NONE;
}

static int _image_util_encode_create_png_handle(decode_encode_s * handle)
{
	image_util_retvm_if((handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	mm_util_png_data *_handle = (mm_util_png_data *) calloc(1, sizeof(mm_util_png_data));
	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "OUT_OF_MEMORY");

	handle->image_h = (mm_util_imgp_h) _handle;
	handle->compression = IMAGE_UTIL_PNG_COMPRESSION_6;

	return IMAGE_UTIL_ERROR_NONE;
}

static int _image_util_encode_create_gif_handle(decode_encode_s * handle)
{
	int err = MM_UTIL_ERROR_NONE;
	mm_gif_file_h _handle = NULL;

	image_util_retvm_if((handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	err = mm_util_gif_encode_create(&_handle);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE),  _image_error_capi(ERR_TYPE_ENCODE, err), "Error - mm_util_gif_encode_create is failed (%d)", err);

	handle->image_h = (mm_util_imgp_h) _handle;

	return IMAGE_UTIL_ERROR_NONE;
}

static int _image_util_encode_create_bmp_handle(decode_encode_s * handle)
{
	image_util_retvm_if((handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	mm_util_bmp_data *_handle = (mm_util_bmp_data *) calloc(1, sizeof(mm_util_bmp_data));
	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "OUT_OF_MEMORY");

	handle->image_h = (mm_util_imgp_h) _handle;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_encode_create(image_util_type_e image_type, image_util_encode_h * handle)
{
	int err = IMAGE_UTIL_ERROR_NONE;

	image_util_debug("image_util_encode_create");
	image_util_retvm_if(handle == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid parameter");

	decode_encode_s *_handle = (decode_encode_s *) calloc(1, sizeof(decode_encode_s));
	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "OUT_OF_MEMORY");

	_handle->image_type = image_type;
	_handle->src_buffer = NULL;
	_handle->dst_buffer = NULL;
	_handle->path = NULL;
	_handle->image_h = NULL;
	_handle->mode = MODE_ENCODE;
	_handle->current_buffer_count = 0;
	_handle->current_resolution_count = 0;
	_handle->current_delay_count = 0;

	switch (image_type) {
	case IMAGE_UTIL_JPEG:
		err = _image_util_encode_create_jpeg_handle(_handle);
		break;
	case IMAGE_UTIL_PNG:
		err = _image_util_encode_create_png_handle(_handle);
		break;
	case IMAGE_UTIL_GIF:
		err = _image_util_encode_create_gif_handle(_handle);
		break;
	case IMAGE_UTIL_BMP:
		err = _image_util_encode_create_bmp_handle(_handle);
		break;
	default:
		err = IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		break;
	}

	if (err != IMAGE_UTIL_ERROR_NONE) {
		image_util_error("Error - create image handle");
		IMAGE_UTIL_SAFE_FREE(_handle);
		return err;
	}

	*handle = (image_util_encode_h) _handle;

	return err;
}

int image_util_encode_set_resolution(image_util_encode_h handle, unsigned long width, unsigned long height)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);
	image_util_retvm_if(_handle->image_h == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid parameter");
	image_util_retvm_if((width == 0 || height == 0), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid resolution");

	switch (_handle->image_type) {
	case IMAGE_UTIL_JPEG:
		{
			mm_util_jpeg_yuv_data *jpeg_data = (mm_util_jpeg_yuv_data *) _handle->image_h;
			jpeg_data->width = width;
			jpeg_data->height = height;
		}
		break;
	case IMAGE_UTIL_PNG:
		{
			mm_util_png_data *png_data = (mm_util_png_data *) _handle->image_h;
			png_data->width = width;
			png_data->height = height;
		}
		break;
	case IMAGE_UTIL_GIF:
		{
			mm_gif_file_h gif_data = (mm_gif_file_h)_handle->image_h;

			image_util_retvm_if((width > INT_MAX) || (height > INT_MAX), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid resolution");

			if (_handle->current_resolution_count == 0) {
				_handle->width = width;
				_handle->height = height;
				mm_util_gif_encode_set_resolution(gif_data, width, height);
			}
			mm_gif_image_h frame = NULL;
			err = _image_util_encode_get_gif_frame(gif_data, _handle->current_resolution_count, &frame);
			image_util_retvm_if((err != IMAGE_UTIL_ERROR_NONE), err, "_image_util_encode_get_gif_frame failed %d", err);

			err = mm_util_gif_image_set_position(frame, 0, 0, (int)width, (int)height);
			image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_ENCODE, err), "mm_util_gif_image_set_position failed %d", err);

			_handle->current_resolution_count++;

			return err;
		}
		break;
	case IMAGE_UTIL_BMP:
		{
			mm_util_bmp_data *bmp_data = (mm_util_bmp_data *) _handle->image_h;
			bmp_data->width = width;
			bmp_data->height = height;
		}
		break;
	default:
		err = IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		break;
	}

	_handle->width = width;
	_handle->height = height;

	return err;
}

int image_util_encode_set_colorspace(image_util_encode_h handle, image_util_colorspace_e colorspace)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);
	IMAGE_UTIL_TYPE_CHECK(_handle->image_type);

	image_util_retvm_if((is_valid_colorspace(colorspace) == FALSE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid colorspace");
	image_util_retvm_if((is_supported_colorspace(colorspace, _handle->image_type) == FALSE), IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT, "not supported format");

	_handle->colorspace = colorspace;

	return err;
}

int image_util_encode_set_quality(image_util_encode_h handle, int quality)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);
	IMAGE_UTIL_SUPPORT_TYPE_CHECK(_handle->image_type, IMAGE_UTIL_JPEG);

	image_util_retvm_if((quality <= 0 || quality > 100), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid quality");

	_handle->quality = quality;

	return err;
}

int image_util_encode_set_png_compression(image_util_encode_h handle, image_util_png_compression_e compression)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);
	IMAGE_UTIL_SUPPORT_TYPE_CHECK(_handle->image_type, IMAGE_UTIL_PNG);

	image_util_retvm_if((compression < IMAGE_UTIL_PNG_COMPRESSION_0 || compression > IMAGE_UTIL_PNG_COMPRESSION_9), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid compression value");

	_handle->compression = compression;

	return err;
}

int image_util_encode_set_gif_frame_delay_time(image_util_encode_h handle, unsigned long long delay_time)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;
	mm_gif_file_h gif_data = NULL;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);
	IMAGE_UTIL_SUPPORT_TYPE_CHECK(_handle->image_type, IMAGE_UTIL_GIF);

	gif_data = (mm_gif_file_h) _handle->image_h;
	image_util_retvm_if(gif_data == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid gif data");

	image_util_retvm_if((delay_time > INT_MAX), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid delay time");

	mm_gif_image_h frame = NULL;
	err = _image_util_encode_get_gif_frame(gif_data, _handle->current_delay_count, &frame);
	image_util_retvm_if((err != IMAGE_UTIL_ERROR_NONE), err, "_image_util_encode_get_gif_frame failed %d", err);

	err = mm_util_gif_image_set_delay_time(frame, (int)delay_time);
	image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_ENCODE, err), "mm_util_gif_image_set_delay_time failed %d", err);

	_handle->current_delay_count++;

	return err;
}

int image_util_encode_set_input_buffer(image_util_encode_h handle, const unsigned char *src_buffer)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);

	if (src_buffer == NULL) {
		image_util_error("Invalid input buffer");
		return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
	}

	/* initialize buffer and value for source buffer */
	if (_handle->image_type == IMAGE_UTIL_GIF) {
		mm_gif_file_h gif_data = (mm_gif_file_h) _handle->image_h;
		mm_gif_image_h frame = NULL;
		image_util_retvm_if(gif_data == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid gif data");

		err = _image_util_encode_get_gif_frame(gif_data, _handle->current_buffer_count, &frame);
		image_util_retvm_if((err != IMAGE_UTIL_ERROR_NONE), err, "_image_util_encode_get_gif_frame failed %d", err);

		err = mm_util_gif_image_set_image(frame, src_buffer);
		image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_ENCODE, err), "mm_util_gif_image_set_image failed %d", err);

		_handle->current_buffer_count++;
	} else {
		if (_handle->src_buffer == NULL)
			_handle->src_buffer = (void *)calloc(1, sizeof(void *));
		image_util_retvm_if(_handle->src_buffer == NULL, IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "calloc fail");

		_handle->src_buffer[_handle->current_buffer_count] = (void *)src_buffer;
	}

	return err;
}

int image_util_encode_set_output_path(image_util_encode_h handle, const char *path)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);

	image_util_retvm_if(!IMAGE_UTIL_STRING_VALID(path), IMAGE_UTIL_ERROR_NO_SUCH_FILE, "Invalid path");

	if (_handle->dst_buffer)
		_handle->dst_buffer = NULL;

	IMAGE_UTIL_SAFE_FREE(_handle->path);

	_handle->path = g_strndup(path, strlen(path));
	image_util_retvm_if(_handle->path == NULL, IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "OUT_OF_MEMORY");

	return err;
}

int image_util_encode_set_output_buffer(image_util_encode_h handle, unsigned char **dst_buffer)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);

	image_util_retvm_if(dst_buffer == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid output buffer");

	IMAGE_UTIL_SAFE_FREE(_handle->path);

	_handle->dst_buffer = (void **)dst_buffer;

	return err;
}

static int _image_util_encode_internal(decode_encode_s * _handle)
{
	int err = MM_UTIL_ERROR_NONE;

	image_util_retvm_if((_handle == NULL || _handle->image_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid parameter");
	switch (_handle->image_type) {
	case IMAGE_UTIL_JPEG:
		{
			mm_util_jpeg_yuv_data *jpeg_data = (mm_util_jpeg_yuv_data *) _handle->image_h;
			size_t size = 0;

			jpeg_data->data = _handle->src_buffer[0];
			jpeg_data->format = TYPECAST_COLOR_BY_TYPE(_handle->colorspace, IMAGE_UTIL_JPEG);

			if (_handle->path)
				err = mm_util_jpeg_encode_to_file(jpeg_data, _handle->quality, _handle->path);
			else
				err = mm_util_encode_to_jpeg_memory(jpeg_data, _handle->quality, _handle->dst_buffer, &size);

			if (err == MM_UTIL_ERROR_NONE)
				_handle->dst_size = (unsigned long long)size;
		}
		break;
	case IMAGE_UTIL_PNG:
		{
			mm_util_png_data *png_data = (mm_util_png_data *) _handle->image_h;
			size_t size = 0;

			png_data->data = _handle->src_buffer[0];

			if (_handle->path)
				err = mm_util_encode_to_png_file(png_data, _handle->compression, _handle->path);
			else
				err = mm_util_encode_to_png_memory(png_data, _handle->compression, _handle->dst_buffer, &size);

			if (err == MM_UTIL_ERROR_NONE)
				_handle->dst_size = (unsigned long long)size;
		}
		break;
	case IMAGE_UTIL_GIF:
		{
			mm_gif_file_h gif_data = (mm_gif_file_h)_handle->image_h;
			size_t encoded_buffer_size = 0;

			image_util_debug("[Count] buffer:%d, resolution:%d, delay:%d", _handle->current_buffer_count, _handle->current_resolution_count, _handle->current_delay_count);
			if ((_handle->current_buffer_count > 1) && ((_handle->current_buffer_count != _handle->current_resolution_count) || (_handle->current_buffer_count != _handle->current_delay_count))) {
				image_util_error("Total frame count does not match with the data set, for animated gif encoding");
				return IMAGE_UTIL_ERROR_INVALID_OPERATION;
			} else if ((_handle->current_buffer_count > 0) && ((_handle->current_buffer_count != _handle->current_resolution_count))) {
				image_util_error("Total frame count does not match with the data set, for gif encoding");
				return IMAGE_UTIL_ERROR_INVALID_OPERATION;
			}
			if (_handle->path)
				err = mm_util_gif_encode_set_file(gif_data, _handle->path);
			else
				err = mm_util_gif_encode_set_mem(gif_data, _handle->dst_buffer, &encoded_buffer_size);
			image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_ENCODE, err), "mm_util_gif_encode_set_file | mm_util_gif_encode_set_mem failed %d", err);

			err = mm_util_gif_encode(gif_data);
			image_util_retvm_if((err != MM_UTIL_ERROR_NONE), _image_error_capi(ERR_TYPE_ENCODE, err), "mm_util_gif_encode failed %d", err);

			if (encoded_buffer_size != 0)
				_handle->dst_size = (unsigned long long)encoded_buffer_size;
		}
		break;
	case IMAGE_UTIL_BMP:
		{
			mm_util_bmp_data *bmp_data = (mm_util_bmp_data *) _handle->image_h;
			size_t size = 0;

			bmp_data->data = _handle->src_buffer[0];

			if (_handle->path)
				err = mm_util_encode_bmp_to_file(bmp_data, _handle->path);
			else
				err = mm_util_encode_bmp_to_memory(bmp_data, _handle->dst_buffer, &size);

			if (err == MM_UTIL_ERROR_NONE)
				_handle->dst_size = size;
		}
		break;
	default:
		return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		break;
	}

	return _image_error_capi(ERR_TYPE_ENCODE, err);
}

int image_util_encode_run(image_util_encode_h handle, unsigned long long *size)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);
	image_util_retvm_if((_handle->path == NULL && _handle->dst_buffer == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid output");
	image_util_retvm_if(_handle->image_type != IMAGE_UTIL_GIF && _handle->src_buffer == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid input");
	image_util_retvm_if((_image_util_check_resolution(_handle->width, _handle->height) == false), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid resolution");

	err = _image_util_encode_internal(_handle);
	image_util_retvm_if((err != IMAGE_UTIL_ERROR_NONE), err, "_image_util_encode_internal failed");

	if (size)
		*size = _handle->dst_size;

	return err;
}

gpointer _image_util_encode_thread(gpointer data)
{
	decode_encode_s *_handle = (decode_encode_s *) data;
	int err = IMAGE_UTIL_ERROR_NONE;

	if (!_handle) {
		image_util_error("[ERROR] - handle");
		return NULL;
	}

	err = _image_util_encode_internal(_handle);
	if (err == IMAGE_UTIL_ERROR_NONE)
		image_util_debug("Success - encode_internal");
	else
		image_util_error("Error - encode_internal");

	if (_handle->_encode_cb) {
		image_util_debug("completed_cb");
		_handle->_encode_cb->image_encode_completed_cb(err, _handle->_encode_cb->user_data, _handle->dst_size);
	}

	IMAGE_UTIL_SAFE_FREE(_handle->_encode_cb);
	_handle->thread = NULL;
	image_util_debug("exit thread");

	return NULL;
}

static int _image_util_encode_create_thread(decode_encode_s * handle)
{
	image_util_retvm_if((handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((handle->thread != NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "The thread is alread running");

	/*create threads */
	handle->thread = g_thread_new("encode_thread", (GThreadFunc) _image_util_encode_thread, (gpointer) handle);
	if (!handle->thread) {
		image_util_error("ERROR - create thread");

		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_encode_run_async(image_util_encode_h handle, image_util_encode_completed_cb completed_cb, void *user_data)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);
	image_util_retvm_if((_handle->path == NULL && _handle->dst_buffer == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid output");
	image_util_retvm_if(_handle->image_type != IMAGE_UTIL_GIF && _handle->src_buffer == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid input");
	image_util_retvm_if((_image_util_check_resolution(_handle->width, _handle->height) == false), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid resolution");
	image_util_retvm_if((completed_cb == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid callback");
	image_util_retvm_if((_handle->thread != NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "The thread is alread running");

	if (_handle->_encode_cb != NULL) {
		IMAGE_UTIL_SAFE_FREE(_handle->_encode_cb);
		_handle->_encode_cb = NULL;
	}
	_handle->_encode_cb = (encode_cb_s *) calloc(1, sizeof(encode_cb_s));
	image_util_retvm_if((_handle->_encode_cb == NULL), IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "Out of memory");

	_handle->_encode_cb->user_data = user_data;
	_handle->_encode_cb->image_encode_completed_cb = completed_cb;

	err = _image_util_encode_create_thread(_handle);
	if (err != IMAGE_UTIL_ERROR_NONE) {
		IMAGE_UTIL_SAFE_FREE(_handle->_encode_cb);
		_handle->_encode_cb = NULL;
	}

	return err;
}

int image_util_encode_destroy(image_util_encode_h handle)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	image_util_debug("image_util_encode_destroy");

	IMAGE_UTIL_ENCODE_HANDLE_CHECK(_handle);

	_image_util_encode_destroy_image_handle(_handle);

	/* g_thread_exit(handle->thread); */
	if (_handle->thread) {
		g_thread_join(_handle->thread);
		IMAGE_UTIL_SAFE_FREE(_handle->_encode_cb);
	}

	IMAGE_UTIL_SAFE_FREE(_handle->path);
	IMAGE_UTIL_SAFE_FREE(_handle->src_buffer);
	IMAGE_UTIL_SAFE_FREE(_handle);

	return err;
}
