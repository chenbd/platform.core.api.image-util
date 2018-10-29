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

#include <stdio.h>

#include <mm_util_imgp.h>
#include <mm_util_jpeg.h>
#include <mm_util_png.h>
#include <mm_util_gif.h>
#include <mm_util_bmp.h>

#include <image_util.h>
#include <image_util_private.h>

static int _convert_decode_scale_tbl[] = {
	MM_UTIL_JPEG_DECODE_DOWNSCALE_1_1,
	MM_UTIL_JPEG_DECODE_DOWNSCALE_1_2,
	MM_UTIL_JPEG_DECODE_DOWNSCALE_1_4,
	MM_UTIL_JPEG_DECODE_DOWNSCALE_1_8,
};

#define _NUM_OF_SCALE			(sizeof(_convert_decode_scale_tbl)/sizeof(int))
#define _NOT_SUPPORTED_IMAGE_TYPE (-1)

static int _image_util_decode_read_header(const char *path, unsigned char **buffer)
{
#define IMG_HEADER_LENGTH 8

	FILE *fp = NULL;
	unsigned char *read_buffer = NULL;

	image_util_retvm_if(!IMAGE_UTIL_STRING_VALID(path), IMAGE_UTIL_ERROR_NO_SUCH_FILE, "Invalid path");
	image_util_retvm_if(buffer == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid buffer");

	fp = fopen(path, "r");
	image_util_retvm_if(fp == NULL, IMAGE_UTIL_ERROR_NO_SUCH_FILE, "File open failed %s", path);

	read_buffer = (void *)calloc(1, IMG_HEADER_LENGTH + 1);
	if (read_buffer == NULL) {
		image_util_error("Allocation fail");
		fclose(fp);
		fp = NULL;
		return IMAGE_UTIL_ERROR_OUT_OF_MEMORY;
	}

	if (fread(read_buffer, 1, IMG_HEADER_LENGTH, fp) != IMG_HEADER_LENGTH) {
		image_util_error("File read failed");
		fclose(fp);
		fp = NULL;
		IMAGE_UTIL_SAFE_FREE(read_buffer);
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	*buffer = read_buffer;

	fclose(fp);
	fp = NULL;

	return IMAGE_UTIL_ERROR_NONE;
}

static int _image_util_decode_check_image_type(const unsigned char *image_buffer, image_util_type_e *image_type)
{
	static char _JPEG_HEADER[] = { 0xFF, 0xD8 };
	static char _PNG_HEADER[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
	static char _GIF_HEADER[] = { 'G', 'I', 'F' };
	static char _BMP_HEADER[] = { 'B', 'M' };

	static struct {
		char *header;
		int size;
		image_util_type_e image_type;
	} image_header[] = {
		{ _JPEG_HEADER, sizeof(_JPEG_HEADER), IMAGE_UTIL_JPEG },
		{ _PNG_HEADER, sizeof(_PNG_HEADER), IMAGE_UTIL_PNG },
		{ _GIF_HEADER, sizeof(_GIF_HEADER), IMAGE_UTIL_GIF },
		{ _BMP_HEADER, sizeof(_BMP_HEADER), IMAGE_UTIL_BMP }
	,};
	unsigned int i = 0;

	image_util_retvm_if((image_buffer == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid buffer");
	image_util_retvm_if((image_type == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid type");

	*image_type = _NOT_SUPPORTED_IMAGE_TYPE;	/* init */
	for (i = 0; i < sizeof(image_header) / sizeof(image_header[0]); i++) {
		if (strncmp((const char *)image_buffer, image_header[i].header, image_header[i].size) == 0) {
			*image_type = image_header[i].image_type;
			break;
		}
	}

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_decode_create(image_util_decode_h * handle)
{
	image_util_fenter();

	image_util_retvm_if((handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	decode_encode_s *_handle = (decode_encode_s *) calloc(1, sizeof(decode_encode_s));
	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "OUT_OF_MEMORY");

	_handle->src_buffer = NULL;
	_handle->dst_buffer = NULL;
	_handle->path = NULL;
	_handle->mode = MODE_DECODE;
	_handle->image_type = _NOT_SUPPORTED_IMAGE_TYPE;
	_handle->colorspace = IMAGE_UTIL_COLORSPACE_RGBA8888;

	*handle = (image_util_decode_h) _handle;

	return IMAGE_UTIL_ERROR_NONE;
}

static int _image_util_decode_create_image_handle(image_util_decode_h handle)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);

	switch (_handle->image_type) {
	case IMAGE_UTIL_JPEG:
		_handle->down_scale = IMAGE_UTIL_DOWNSCALE_1_1;
		break;
	case IMAGE_UTIL_PNG:
	case IMAGE_UTIL_GIF:
	case IMAGE_UTIL_BMP:
		/* do nothing... */
		break;
	default:
		err = IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT;
		break;
	}

	if (err != IMAGE_UTIL_ERROR_NONE)
		image_util_error("Error - create image handle");

	return err;
}

int image_util_decode_set_input_path(image_util_decode_h handle, const char *path)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;
	unsigned char *image_header = NULL;

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);
	image_util_retvm_if(!IMAGE_UTIL_STRING_VALID(path), IMAGE_UTIL_ERROR_NO_SUCH_FILE, "Invalid path");

	if (_handle->src_buffer)
		_handle->src_buffer = NULL;

	err = _image_util_decode_read_header(path, &image_header);
	image_util_retvm_if((err != IMAGE_UTIL_ERROR_NONE), err, "_image_util_decode_read_header failed");

	err = _image_util_decode_check_image_type(image_header, &_handle->image_type);
	if (err != IMAGE_UTIL_ERROR_NONE) {
		image_util_error("_image_util_decode_check_image_type failed");
		IMAGE_UTIL_SAFE_FREE(image_header);
		return err;
	}

	IMAGE_UTIL_SAFE_FREE(image_header);

	err = _image_util_decode_create_image_handle(_handle);
	image_util_retvm_if((err != IMAGE_UTIL_ERROR_NONE), err, "_image_util_decode_create_image_handle failed");

	_handle->path = g_strndup(path, strlen(path));
	image_util_retvm_if(_handle->path == NULL, IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "OUT_OF_MEMORY");

	return err;
}

int image_util_decode_set_input_buffer(image_util_decode_h handle, const unsigned char *src_buffer, unsigned long long src_size)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);
	image_util_retvm_if((src_buffer == NULL || src_size == 0), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid input buffer");

	IMAGE_UTIL_SAFE_FREE(_handle->path);

	err = _image_util_decode_check_image_type(src_buffer, &_handle->image_type);
	image_util_retvm_if(err != IMAGE_UTIL_ERROR_NONE, err, "_image_util_decode_check_image_type failed");

	err = _image_util_decode_create_image_handle(_handle);
	image_util_retvm_if(err != IMAGE_UTIL_ERROR_NONE, err, "_image_util_decode_create_image_handle failed");

	IMAGE_UTIL_SAFE_FREE(_handle->src_buffer);

	_handle->src_buffer = (void *)calloc(1, sizeof(void *));
	image_util_retvm_if(_handle->src_buffer == NULL, IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "The memory of input buffer was not allocated");

	_handle->src_buffer[0] = (void *)src_buffer;
	_handle->src_size = src_size;

	return err;
}

int image_util_decode_set_output_buffer(image_util_decode_h handle, unsigned char **dst_buffer)
{
	decode_encode_s *_handle = (decode_encode_s *) handle;

	image_util_fenter();

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);
	image_util_retvm_if(dst_buffer == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid parameter");

	_handle->dst_buffer = (void **)dst_buffer;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_decode_set_colorspace(image_util_encode_h handle, image_util_colorspace_e colorspace)
{
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);
	IMAGE_UTIL_TYPE_CHECK(_handle->image_type);

	image_util_retvm_if((is_valid_colorspace(colorspace) == FALSE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid colorspace");
	image_util_retvm_if((is_supported_colorspace(colorspace, _handle->image_type) == FALSE), IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT, "not supported format");

	_handle->colorspace = colorspace;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_decode_set_jpeg_downscale(image_util_encode_h handle, image_util_scale_e down_scale)
{
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);
	IMAGE_UTIL_SUPPORT_TYPE_CHECK(_handle->image_type, IMAGE_UTIL_JPEG);

	image_util_retvm_if((down_scale < 0 || down_scale >= _NUM_OF_SCALE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "downscale is invalid");

	_handle->down_scale = down_scale;

	return IMAGE_UTIL_ERROR_NONE;
}

static int _image_util_decode_internal(decode_encode_s * _handle)
{
	int err = MM_UTIL_ERROR_NONE;

	image_util_fenter();

	image_util_retvm_if((_handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "invalid parameter");
	image_util_retvm_if(_handle->dst_buffer == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid output");

	switch (_handle->image_type) {
	case IMAGE_UTIL_JPEG:
		{
			mm_util_jpeg_yuv_data jpeg_data;
			memset(&jpeg_data, 0, sizeof(mm_util_jpeg_yuv_data));

			if (_handle->path)
					err = mm_util_decode_from_jpeg_file(_handle->path, TYPECAST_COLOR_BY_TYPE(_handle->colorspace, IMAGE_UTIL_JPEG), _convert_decode_scale_tbl[_handle->down_scale], &jpeg_data);
			else
					err = mm_util_decode_from_jpeg_memory(_handle->src_buffer[0], _handle->src_size, TYPECAST_COLOR_BY_TYPE(_handle->colorspace, IMAGE_UTIL_JPEG), _convert_decode_scale_tbl[_handle->down_scale], &jpeg_data);

			if (err == MM_UTIL_ERROR_NONE) {
				*(_handle->dst_buffer) = jpeg_data.data;
				_handle->dst_size = (unsigned long long)jpeg_data.size;
				_handle->width = jpeg_data.width;
				_handle->height = jpeg_data.height;
			} else {
				image_util_error("fail to decode jpeg [%d]", err);
			}
		}
		break;
	case IMAGE_UTIL_PNG:
		{
			mm_util_png_data png_data;
			memset(&png_data, 0, sizeof(mm_util_png_data));

			if (_handle->path)
				err = mm_util_decode_from_png_file(_handle->path, &png_data);
			else
				err = mm_util_decode_from_png_memory(_handle->src_buffer[0], (size_t)_handle->src_size, &png_data);

			if (err == MM_UTIL_ERROR_NONE) {
				*(_handle->dst_buffer) = png_data.data;
				_handle->dst_size = (unsigned long long)png_data.size;
				_handle->width = png_data.width;
				_handle->height = png_data.height;
			} else {
				image_util_error("fail to decode png [%d]", err);
			}
		}
		break;
	case IMAGE_UTIL_GIF:
		{
			mm_util_gif_data gif_data;
			memset(&gif_data, 0, sizeof(mm_util_gif_data));

			if (_handle->path)
				err = mm_util_decode_from_gif_file(&gif_data, _handle->path);
			else
				err = mm_util_decode_from_gif_memory(&gif_data, _handle->src_buffer[0]);

			if (err == MM_UTIL_ERROR_NONE) {
				*(_handle->dst_buffer) = gif_data.data;
				_handle->dst_size = gif_data.size;
				_handle->width = gif_data.width;
				_handle->height = gif_data.height;
			} else {
				image_util_error("fail to decode gif [%d]", err);
			}
		}
		break;
	case IMAGE_UTIL_BMP:
		{
			mm_util_bmp_data bmp_data;
			memset(&bmp_data, 0, sizeof(mm_util_bmp_data));

			if (_handle->path)
				err = mm_util_decode_from_bmp_file(_handle->path, &bmp_data);
			else
				err = mm_util_decode_from_bmp_memory(_handle->src_buffer[0], (size_t)_handle->src_size, &bmp_data);

			if (err == MM_UTIL_ERROR_NONE) {
				*(_handle->dst_buffer) = bmp_data.data;
				_handle->dst_size = (unsigned long long)bmp_data.size;
				_handle->width = bmp_data.width;
				_handle->height = bmp_data.height;
			} else {
				image_util_error("fail to decode bmp [%d]", err);
			}
		}
		break;
	default:
		image_util_error("Not supported format [%d]", _handle->image_type);
		return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		break;
	}

	image_util_debug("dst_buffer(%p) width (%lu) height (%lu) dst_size (%zu)", *(_handle->dst_buffer), _handle->width, _handle->height, _handle->dst_size);

	return _image_error_capi(ERR_TYPE_DECODE, err);
}

int image_util_decode_run(image_util_decode_h handle, unsigned long *width, unsigned long *height, unsigned long long *size)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);
	image_util_retvm_if((_handle->path == NULL && _handle->src_buffer == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid input");
	image_util_retvm_if(_handle->dst_buffer == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid output");

	err = _image_util_decode_internal(_handle);
	image_util_retvm_if(err != IMAGE_UTIL_ERROR_NONE, err, "_image_util_decode_internal failed");

	if (width)
		*width = _handle->width;
	if (height)
		*height = _handle->height;
	if (size)
		*size = _handle->dst_size;

	return err;
}

gpointer _image_util_decode_thread(gpointer data)
{
	decode_encode_s *_handle = (decode_encode_s *) data;
	int err = IMAGE_UTIL_ERROR_NONE;

	image_util_fenter();

	if (!_handle) {
		image_util_error("[ERROR] - handle");
		return NULL;
	}

	err = _image_util_decode_internal(_handle);
	if (err == IMAGE_UTIL_ERROR_NONE)
		image_util_debug("Success - decode_internal");
	else
		image_util_error("Error - decode_internal");

	if (_handle->_decode_cb) {
		image_util_debug("call completed_cb");
		_handle->_decode_cb->image_decode_completed_cb(err, _handle->_decode_cb->user_data, _handle->width, _handle->height, _handle->dst_size);
	} else {
		image_util_error("No callback");
	}

	image_util_fleave();

	return NULL;
}

static int _image_util_decode_create_thread(decode_encode_s * handle)
{
	image_util_retvm_if((handle == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((handle->thread != NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "A thread is alread running");

	/*create threads */
	handle->thread = g_thread_new("decode_thread", (GThreadFunc) _image_util_decode_thread, (gpointer) handle);
	if (!handle->thread) {
		image_util_error("ERROR - create thread");
		return IMAGE_UTIL_ERROR_INVALID_OPERATION;
	}

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_decode_run_async(image_util_decode_h handle, image_util_decode_completed_cb completed_cb, void *user_data)
{
	int err = IMAGE_UTIL_ERROR_NONE;
	decode_encode_s *_handle = (decode_encode_s *) handle;

	image_util_fenter();

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);
	image_util_retvm_if((_handle->path == NULL && _handle->src_buffer == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid input");
	image_util_retvm_if(_handle->dst_buffer == NULL, IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid output");
	image_util_retvm_if((completed_cb == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid callback");
	image_util_retvm_if((_handle->thread != NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "A thread is alread running");

	if (_handle->_decode_cb != NULL) {
		IMAGE_UTIL_SAFE_FREE(_handle->_decode_cb);
		_handle->_decode_cb = NULL;
	}

	_handle->_decode_cb = (decode_cb_s *) calloc(1, sizeof(decode_cb_s));
	image_util_retvm_if((_handle->_decode_cb == NULL), IMAGE_UTIL_ERROR_OUT_OF_MEMORY, "Out of memory");

	_handle->_decode_cb->user_data = user_data;
	_handle->_decode_cb->image_decode_completed_cb = completed_cb;

	err = _image_util_decode_create_thread(_handle);
	if (err != IMAGE_UTIL_ERROR_NONE) {
		IMAGE_UTIL_SAFE_FREE(_handle->_decode_cb);
		_handle->_decode_cb = NULL;
	}

	image_util_fleave();

	return err;
}

int image_util_decode_destroy(image_util_decode_h handle)
{
	decode_encode_s *_handle = (decode_encode_s *) handle;

	image_util_fenter();

	IMAGE_UTIL_DECODE_HANDLE_CHECK(handle);

	/* g_thread_exit(handle->thread); */
	if (_handle->thread) {
		g_thread_join(_handle->thread);
		IMAGE_UTIL_SAFE_FREE(_handle->_decode_cb);
	}
	IMAGE_UTIL_SAFE_FREE(_handle->path);
	IMAGE_UTIL_SAFE_FREE(_handle->src_buffer);
	IMAGE_UTIL_SAFE_FREE(_handle);

	image_util_fleave();

	return IMAGE_UTIL_ERROR_NONE;
}
