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

#include <image_util.h>
#include <image_util_internal.h>
#include <image_util_private.h>

#include <mm_util_imgp.h>
#include <mm_util_gif.h>

int image_util_convert_colorspace(unsigned char *dest, image_util_colorspace_e dest_colorspace, const unsigned char *src, int width, int height, image_util_colorspace_e src_colorspace)
{
	int err = MM_UTIL_ERROR_NONE;
	unsigned int res_w = 0;
	unsigned int res_h = 0;
	unsigned char *res_buffer = NULL;
	size_t res_buffer_size = 0;

	image_util_retvm_if((dest == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "dest is null");
	image_util_retvm_if((src == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "src is null");
	image_util_retvm_if((is_valid_colorspace(dest_colorspace) == FALSE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid dst_colorspace");
	image_util_retvm_if((is_valid_colorspace(src_colorspace) == FALSE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid src_colorspace");

	err = mm_util_convert_colorspace(src, width, height, TYPECAST_COLOR(src_colorspace), TYPECAST_COLOR(dest_colorspace), &res_buffer, &res_w, &res_h, &res_buffer_size);
	if (err == MM_UTIL_ERROR_NONE)
		memcpy(dest, res_buffer, res_buffer_size);

	IMAGE_UTIL_SAFE_FREE(res_buffer);

	return _image_error_capi(ERR_TYPE_TRANSFORM, err);
}

int image_util_resize(unsigned char *dest, int *dest_width, int *dest_height, const unsigned char *src, int src_width, int src_height, image_util_colorspace_e colorspace)
{
	int err = MM_UTIL_ERROR_NONE;
	unsigned int res_w = 0;
	unsigned int res_h = 0;
	unsigned char *res_buffer = NULL;
	size_t res_buffer_size = 0;

	image_util_retvm_if((dest == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "dest is null");
	image_util_retvm_if((src == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "src is null");
	image_util_retvm_if((is_valid_colorspace(colorspace) == FALSE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid colorspace");
	image_util_retvm_if((dest_width == NULL || dest_height == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "dest_width or dest_height is null");
	image_util_retvm_if((*dest_width <= 0 || *dest_height <= 0), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid dest_width or Invalid dest_height");

	err = mm_util_resize_image(src, src_width, src_height, TYPECAST_COLOR(colorspace), *dest_width, *dest_height, &res_buffer, &res_w, &res_h, &res_buffer_size);
	if (err == MM_UTIL_ERROR_NONE) {
		memcpy(dest, res_buffer, res_buffer_size);
		*dest_width = (int)res_w;
		*dest_height = (int)res_h;
	}

	return _image_error_capi(ERR_TYPE_TRANSFORM, err);
}

int image_util_rotate(unsigned char *dest, int *dest_width, int *dest_height, image_util_rotation_e dest_rotation, const unsigned char *src, int src_width, int src_height, image_util_colorspace_e colorspace)
{
	int err = MM_UTIL_ERROR_NONE;
	unsigned int res_w = 0;
	unsigned int res_h = 0;
	unsigned char *res_buffer = NULL;
	size_t res_buffer_size = 0;

	image_util_retvm_if((dest == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "dest is null");
	image_util_retvm_if((src == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "src is null");
	image_util_retvm_if((is_valid_colorspace(colorspace) == FALSE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid colorspace");
	image_util_retvm_if((dest_rotation < 0 || dest_rotation > IMAGE_UTIL_ROTATION_FLIP_VERT), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid rotation");
	image_util_retvm_if((dest_width == NULL || dest_height == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "dest_width or dest_height is null");

	err = mm_util_rotate_image(src, src_width, src_height, TYPECAST_COLOR(colorspace), dest_rotation, &res_buffer, &res_w, &res_h, &res_buffer_size);
	if (err == MM_UTIL_ERROR_NONE) {
		memcpy(dest, res_buffer, res_buffer_size);
		*dest_width = (int)res_w;
		*dest_height = (int)res_h;
	}
	return _image_error_capi(ERR_TYPE_TRANSFORM, err);
}

int image_util_crop(unsigned char *dest, int x, int y, int *width, int *height, const unsigned char *src, int src_width, int src_height, image_util_colorspace_e colorspace)
{
	int err = MM_UTIL_ERROR_NONE;
	unsigned int res_w = 0;
	unsigned int res_h = 0;
	unsigned char *res_buffer = NULL;
	size_t res_buffer_size = 0;

	image_util_retvm_if((dest == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "dest is null");
	image_util_retvm_if((src == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "src is null");
	image_util_retvm_if((is_valid_colorspace(colorspace) == FALSE), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid colorspace");
	image_util_retvm_if((width == NULL || height == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "width or height is null");
	image_util_retvm_if((src_width <= x || src_height <= y || src_width < x + *width || src_height < y + *height), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid crop area");

	err = mm_util_crop_image(src, src_width, src_height, TYPECAST_COLOR(colorspace), x, y, *width, *height, &res_buffer, &res_w, &res_h, &res_buffer_size);
	if (err == MM_UTIL_ERROR_NONE) {
		memcpy(dest, res_buffer, res_buffer_size);
		*width = (int)res_w;
		*height = (int)res_h;
	}

	IMAGE_UTIL_SAFE_FREE(res_buffer);

	return _image_error_capi(ERR_TYPE_TRANSFORM, err);
}

int image_util_frame_create(void *decode_encode_h, image_util_frame_h *frame_h)
{
	int ret = MM_UTIL_ERROR_NONE;

	image_util_retvm_if((decode_encode_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((frame_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	decode_encode_s *image = (decode_encode_s *)decode_encode_h;
	image_util_retvm_if((image->image_h == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "The image handle is wrong");

	frame_s *frame = calloc(1, sizeof(frame_s));
	if (frame == NULL) {
		image_util_error("Memory allocation is failed.");
		return IMAGE_UTIL_ERROR_OUT_OF_MEMORY;
	}

	if (image->image_type == IMAGE_UTIL_GIF) {
		mm_gif_file_h gif_data = (mm_gif_file_h)image->image_h;
		mm_gif_image_h gif_frame = NULL;

		ret = mm_util_gif_image_create(gif_data, &gif_frame);
		if (ret != MM_UTIL_ERROR_NONE) {
			image_util_error("mm_util_gif_image_create is failed(%d).", ret);
			IMAGE_UTIL_SAFE_FREE(frame);
			return _image_error_capi(ERR_TYPE_ENCODE, ret);
		}
		frame->frame_h = gif_frame;
	} else {
		image_util_error("The image type(%d) is not supported.", image->image_type);
		IMAGE_UTIL_SAFE_FREE(frame);
		return IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT;
	}

	*frame_h = frame;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_frame_set_resolution(image_util_frame_h frame_h, const int width, const int height)
{
	int ret = MM_UTIL_ERROR_NONE;

	image_util_retvm_if((frame_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((width <= 0) || (height <= 0), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Width or Height");

	frame_s *frame = (frame_s *)frame_h;

	ret = mm_util_gif_image_set_position(frame->frame_h, 0, 0, width, height);
	if (ret != MM_UTIL_ERROR_NONE) {
		image_util_error("mm_util_gif_image_set_position is failed(%d).", ret);
		return _image_error_capi(ERR_TYPE_ENCODE, ret);
	}

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_frame_set_gif_delay(image_util_frame_h frame_h, const int delay_time)
{
	int ret = MM_UTIL_ERROR_NONE;

	image_util_retvm_if((frame_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((delay_time <= 0), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Delay Time");

	frame_s *frame = (frame_s *)frame_h;

	ret = mm_util_gif_image_set_delay_time(frame->frame_h, delay_time);
	if (ret != MM_UTIL_ERROR_NONE) {
		image_util_error("mm_util_gif_image_set_delay_time is failed(%d).", ret);
		return _image_error_capi(ERR_TYPE_ENCODE, ret);
	}

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_frame_set_frame(image_util_frame_h frame_h, unsigned char *buffer)
{
	int ret = MM_UTIL_ERROR_NONE;

	image_util_retvm_if((frame_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((buffer == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Buffer");

	frame_s *frame = (frame_s *)frame_h;

	ret = mm_util_gif_image_set_image(frame->frame_h, buffer);
	if (ret != MM_UTIL_ERROR_NONE) {
		image_util_error("mm_util_gif_image_set_image is failed(%d).", ret);
		return _image_error_capi(ERR_TYPE_ENCODE, ret);
	}

	return IMAGE_UTIL_ERROR_NONE;
}

void image_util_frame_destroy(image_util_frame_h frame_h)
{
	image_util_retm_if((frame_h == NULL), "Invalid Handle");

	frame_s *frame = (frame_s *)frame_h;
	mm_util_gif_image_destory(frame->frame_h);

	IMAGE_UTIL_SAFE_FREE(frame);
}

int image_util_encode_add_frame(image_util_encode_h encode_h, image_util_frame_h frame_h)
{
	int ret = MM_UTIL_ERROR_NONE;

	image_util_retvm_if((encode_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");
	image_util_retvm_if((frame_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	decode_encode_s *encode = (decode_encode_s *)encode_h;
	frame_s *frame = (frame_s *)frame_h;
	image_util_retvm_if((encode->image_h == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "The image handle is wrong");
	image_util_retvm_if((encode->image_type != IMAGE_UTIL_GIF), IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT, "The image type(%d) is not supported.", encode->image_type);
	image_util_retvm_if((frame->frame_h == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "The frame handle is wrong");

	mm_gif_file_h gif_data = (mm_gif_file_h)encode->image_h;

	if (encode->current_buffer_count == 0) {
		if (encode->path)
			ret = mm_util_gif_encode_set_file(gif_data, encode->path);
		else
			ret = mm_util_gif_encode_set_mem(gif_data, encode->dst_buffer, &encode->gif_encode_size);
	}
	if (ret != MM_UTIL_ERROR_NONE) {
		image_util_error("mm_util_gif_encode_add_image is failed(%d).", ret);
		return _image_error_capi(ERR_TYPE_ENCODE, ret);
	}

	ret = mm_util_gif_encode_add_image(gif_data, (mm_gif_image_h)frame->frame_h);
	if (ret != MM_UTIL_ERROR_NONE) {
		image_util_error("mm_util_gif_encode_add_image is failed(%d).", ret);
		return _image_error_capi(ERR_TYPE_ENCODE, ret);
	}
	encode->current_buffer_count++;

	return IMAGE_UTIL_ERROR_NONE;
}

int image_util_encode_save(image_util_encode_h encode_h, unsigned long long *size)
{
	int ret = MM_UTIL_ERROR_NONE;

	image_util_retvm_if((encode_h == NULL), IMAGE_UTIL_ERROR_INVALID_PARAMETER, "Invalid Handle");

	decode_encode_s *encode = (decode_encode_s *)encode_h;
	image_util_retvm_if((encode->image_h == NULL), IMAGE_UTIL_ERROR_INVALID_OPERATION, "The image handle is wrong");
	image_util_retvm_if((encode->image_type != IMAGE_UTIL_GIF), IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT, "The image type(%d) is not supported.", encode->image_type);

	mm_gif_file_h gif_data = (mm_gif_file_h)encode->image_h;

	ret = mm_util_gif_encode_save(gif_data);
	if (ret != MM_UTIL_ERROR_NONE) {
		image_util_error("mm_util_gif_encode_save is failed(%d).", ret);
		mm_util_gif_encode_destroy(gif_data);
		return _image_error_capi(ERR_TYPE_ENCODE, ret);
	}

	*size = (unsigned long long)encode->gif_encode_size;
	encode->current_buffer_count = 0;

	return IMAGE_UTIL_ERROR_NONE;
}
