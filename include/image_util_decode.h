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

#ifndef __TIZEN_MULTIMEDIA_IMAGE_UTIL_DECODE_H__
#define __TIZEN_MULTIMEDIA_IMAGE_UTIL_DECODE_H__

#include <image_util_type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @addtogroup CAPI_MEDIA_IMAGE_UTIL_ENCODE_DECODE_MODULE
* @{
*/

/**
* @brief Creates a handle to image util decoding.
* @details This function creates a handle to image util decoding.
* @since_tizen 3.0
*
* @remarks The @a handle should be released using image_util_decode_destroy().
*
* @param[out] handle A handle to image util decoding
*
* @return @c 0 on success,
*             otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_OUT_OF_MEMORY Out of memory
*
* @see image_util_decode_destroy()
*
*/
int image_util_decode_create(image_util_decode_h *handle);

/**
* @brief Sets the input file path from which to decode.
* @since_tizen 3.0
*
* @remarks One of image_util_decode_set_input_path() or image_util_decode_set_input_buffer() should be set.\n
*             If both are set then the latest input set, is considered.\n
*             http://tizen.org/privilege/mediastorage is needed if input or output path are relevant to media storage.\n
*             http://tizen.org/privilege/externalstorage is needed if input or output path are relevant to external storage.\n
*             Finds out image type by reading the header of the image provided in input path.
*
* @param[in] handle The handle to image util decoding
* @param[in] path The path to input image
*
* @return @c 0 on success,
*            otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_NO_SUCH_FILE No such file
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
* @retval #IMAGE_UTIL_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
* @retval #IMAGE_UTIL_ERROR_OUT_OF_MEMORY Out of memory
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
*
* @pre image_util_decode_create()
*
* @post image_util_decode_run() / image_util_decode_run_async()
* @post image_util_decode_destroy()
*
* @see image_util_decode_create()
* @see image_util_decode_set_output_buffer()
* @see image_util_decode_run()
* @see image_util_decode_run_async()
* @see image_util_decode_destroy()
*/
int image_util_decode_set_input_path(image_util_decode_h handle, const char *path);

/**
* @brief Sets the input buffer from which to decode.
* @since_tizen 3.0
*
* @remarks One of image_util_decode_set_input_path() or image_util_decode_set_input_buffer() should be set.\n
*              If both are set then the latest input set, is considered.\n
*              Finds out image type by reading the header of the image provided in input buffer.
*
* @param[in] handle The handle to image util decoding
* @param[in] src_buffer The input image buffer
* @param[in] src_size The input image buffer size
*
* @return @c 0 on success,
*             otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
* @retval #IMAGE_UTIL_ERROR_OUT_OF_MEMORY Out of memory
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
*
* @pre image_util_decode_create()
*
* @post image_util_decode_run() / image_util_decode_run_async()
* @post image_util_decode_destroy()
*
* @see image_util_decode_create()
* @see image_util_decode_set_output_buffer()
* @see image_util_decode_run()
* @see image_util_decode_run_async()
* @see image_util_decode_destroy()
*/
int image_util_decode_set_input_buffer(image_util_decode_h handle, const unsigned char *src_buffer, unsigned long long src_size);

/**
* @brief Sets the output buffer to which the decoded buffer will be written to.
* @since_tizen 3.0
*
* @remarks Either image_util_decode_set_input_path() or image_util_decode_set_input_buffer() should be set.\n
*             By default the decoded output buffer colorspace will be IMAGE_UTIL_COLORSPACE_RGBA8888.
*             Use image_util_decode_set_colorspace to change the colorspace.
*             @a dst_buffer should be released after @c image_util_decode_run() or @c image_util_decode_run_async().
*
* @param[in] handle The handle to image util decoding
* @param[in] dst_buffer The decoded output buffer
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_decode_create()
*
* @post image_util_decode_run() / image_util_decode_run_async()
* @post image_util_decode_destroy()
*
* @see image_util_decode_create()
* @see image_util_decode_set_input_path()
* @see image_util_decode_set_input_buffer()
* @see image_util_decode_run()
* @see image_util_decode_run_async()
* @see image_util_decode_destroy()
*/
int image_util_decode_set_output_buffer(image_util_decode_h handle, unsigned char **dst_buffer);

/**
* @brief Sets the decoded image colorspace format.
* @since_tizen 3.0
*
* @remarks The default colorspace is IMAGE_UTIL_COLORSPACE_RGBA8888.\n
*             Use image_util_foreach_supported_colorspace to get supported colorspaces for each image format.\n
*             Errors would be returned if not supported.
*
* @param[in] handle The handle to image util decoding
* @param[in] colorspace The decoded image colorspace
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
*
* @pre image_util_decode_create()
* @pre image_util_decode_set_input_path() / image_util_decode_set_input_buffer()
*
* @post image_util_decode_run() / image_util_decode_run_async()
* @post image_util_decode_destroy()
*
* @see image_util_supported_colorspace_cb()
* @see image_util_foreach_supported_colorspace()
* @see image_util_decode_create()
* @see image_util_decode_set_input_path()
* @see image_util_decode_set_input_buffer()
* @see image_util_decode_set_output_buffer()
* @see image_util_decode_run()
* @see image_util_decode_run_async()
* @see image_util_decode_destroy()
*/
int image_util_decode_set_colorspace(image_util_encode_h handle, image_util_colorspace_e colorspace);

/**
* @brief Sets the downscale value at which JPEG image should be decoded.
* @since_tizen 3.0
*
* @remarks This is API is supported only for JPEG decoding.
*
* @param[in] handle The handle to image util decoding
* @param[in] down_scale The downscale at which image is to be decoded
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
*
* @pre image_util_decode_create()
* @pre image_util_decode_set_input_path() / image_util_decode_set_input_buffer()
*
* @post image_util_decode_run() / image_util_decode_run_async()
* @post image_util_decode_destroy()
*
* @see image_util_decode_create()
* @see image_util_decode_set_input_path()
* @see image_util_decode_set_input_buffer()
* @see image_util_decode_set_output_buffer()
* @see image_util_decode_run()
* @see image_util_decode_run_async()
* @see image_util_decode_destroy()
*/
int image_util_decode_set_jpeg_downscale(image_util_encode_h handle, image_util_scale_e down_scale);

/**
* @brief Starts decoding of the image and fills the output buffer set using image_util_decode_set_output_buffer().
* @since_tizen 3.0
*
* @remarks The output will be stored in the pointer set using image_util_decode_set_output_buffer().\n
*          The function executes synchronously.\n
*          When any of the @pre functions are not called, IMAGE_UTIL_ERROR_INVALID_PARAMETER is returned.
*
* @param[in] handle The handle to image util decoding
* @param[out] width Width of the decoded image
* @param[out] height Height of the decoded image
* @param[out] size Size of the decoded image
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_decode_create()
* @pre image_util_decode_set_input_buffer() / image_util_decode_set_input_path().
* @pre image_util_decode_set_output_buffer()
*
* @post image_util_decode_destroy()
*
* @see image_util_decode_create()
* @see image_util_decode_set_input_path()
* @see image_util_decode_set_input_buffer()
* @see image_util_decode_set_output_buffer()
* @see image_util_decode_destroy()
*/
int image_util_decode_run(image_util_decode_h handle, unsigned long *width, unsigned long *height, unsigned long long *size);

/**
* @brief Starts decoding of the image and fills the output buffer set using image_util_decode_set_output_buffer().
* @since_tizen 3.0
*
* @remarks The output will be stored in the pointer set using image_util_decode_set_output_buffer().\n
*          The function executes asynchronously, which contains complete callback.\n
*          When any of the @pre functions are not called, IMAGE_UTIL_ERROR_INVALID_PARAMETER is returned.
*
* @param[in] handle The handle to image util decoding
* @param[in] callback The callback function to be invoked
* @param[in] user_data The user data to be passed to the callback function
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_decode_create()
* @pre image_util_decode_set_input_buffer() / image_util_decode_set_input_path().
* @pre image_util_decode_set_output_buffer()
*
* @post image_util_decode_destroy()
*
* @see image_util_decode_create()
* @see image_util_decode_set_input_path()
* @see image_util_decode_set_input_buffer()
* @see image_util_decode_set_output_buffer()
* @see image_util_decode_destroy()
*/
int image_util_decode_run_async(image_util_decode_h handle, image_util_decode_completed_cb callback, void *user_data);

/**
* @brief Destroys the image decoding handle.
* @since_tizen 3.0
*
* @remarks Any image handle created should be destroyed.
*
* @param[in] handle The handle to image util decoding
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_decode_create()
*
* @see image_util_decode_create()
*/
int image_util_decode_destroy(image_util_decode_h handle);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_IMAGE_UTIL_DECODE_H__ */