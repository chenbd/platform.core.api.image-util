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

#ifndef __TIZEN_MULTIMEDIA_IMAGE_UTIL_ENCODE_H__
#define __TIZEN_MULTIMEDIA_IMAGE_UTIL_ENCODE_H__

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
* @brief Creates a handle to image util encoding.
* @details This function creates a handle to image util encoding.
* @since_tizen 3.0
*
* @remarks The @a handle should be released using image_util_encode_destroy().
*
* @param[in] image_type The type of output image for which to create encode handle.
* @param[out] handle A handle to image util encoding
*
* @return @c 0 on success,
*            otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_OUT_OF_MEMORY Out of memory
*
* @see image_util_encode_destroy()
*
*/
int image_util_encode_create(image_util_type_e image_type, image_util_encode_h *handle);

/**
* @brief Sets the resolution of the encoded image.
* @since_tizen 3.0
*
* @remarks This should be called before calling image_util_encode_run().\n
*               While encoding animated gif image, resolution should be set for each frame.
*
* @param[in] handle The handle to image util encoding
* @param[in] width Width of the original image
* @param[in] height Height of the original image
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_encode_create()
*
* @post image_util_encode_run() / image_util_encode_run_async()
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_set_output_path()
* @see image_util_encode_set_output_buffer()
* @see image_util_encode_run()
* @see image_util_encode_run_async()
* @see image_util_encode_destroy()
*/
int image_util_encode_set_resolution(image_util_encode_h handle, unsigned long width, unsigned long height);

/**
* @brief Sets the colorspace format for image encoding.
* @since_tizen 3.0
*
* @remarks The default colorspace is IMAGE_UTIL_COLORSPACE_RGBA8888.\n
*             Use image_util_foreach_supported_colorspace to get supported colorspaces for each image format.\n
*             Errors would be returned if not supported.
*
* @param[in] handle The handle to image util encoding
* @param[in] colorspace The colorspace of the input image to encode
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
*
* @pre image_util_encode_create()
*
* @post image_util_encode_run() / image_util_encode_run_async()
* @post image_util_encode_destroy()
*
* @see image_util_supported_colorspace_cb()
* @see image_util_foreach_supported_colorspace()
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_set_output_path()
* @see image_util_encode_set_output_buffer()
* @see image_util_encode_run()
* @see image_util_encode_run_async()
* @see image_util_encode_destroy()
*/
int image_util_encode_set_colorspace(image_util_encode_h handle, image_util_colorspace_e colorspace);

/**
* @brief Sets the quality for image encoding.
* @since_tizen 3.0
*
* @remarks If application does not set this, then by default quality of 75 is set.\n
*              Quality is supported for JPEG format. IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT will be returned for other formats.
*
* @param[in] handle The handle to image util encoding
* @param[in] quality Encoding quality from 1~100
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
*
* @pre image_util_encode_create()
*
* @post image_util_encode_run() / image_util_encode_run_async()
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_set_output_path()
* @see image_util_encode_set_output_buffer()
* @see image_util_encode_run()
* @see image_util_encode_run_async()
* @see image_util_encode_destroy()
*/
int image_util_encode_set_quality(image_util_encode_h handle, int quality);

/**
* @brief Sets the compression value of png image encoding(0~9).
* @since_tizen 3.0
*
* @remarks If application does not set this, then default compression of 6 is set.
*
* @param[in] handle The handle to image util encoding
* @param[in] compression The compression value valid from 0~9
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_encode_create()
*
* @post image_util_encode_run() / image_util_encode_run_async()
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_set_output_path()
* @see image_util_encode_set_output_buffer()
* @see image_util_encode_run()
* @see image_util_encode_run_async()
* @see image_util_encode_destroy()
*/
int image_util_encode_set_png_compression(image_util_encode_h handle, image_util_png_compression_e compression);

/**
* @brief Sets the time delay between each frame in the encoded animated gif image.
* @since_tizen 3.0
*
* @remarks In case animated gif image if this is not set then there will be no delay between each frame.\n
*             This should be set for each frame in the animated gif image.\n
*             This can be set a different value for each frame, which results in different delay time between different frames.
*
* @param[in] handle The handle to image util encoding
* @param[in] delay_time Time delay between each frame in the encoded image, in 0.01sec units.
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
*
* @pre image_util_encode_create()
*
* @post image_util_encode_run() / image_util_encode_run_async()
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_set_output_path()
* @see image_util_encode_set_output_buffer()
* @see image_util_encode_run()
* @see image_util_encode_run_async()
* @see image_util_encode_destroy()
*/
int image_util_encode_set_gif_frame_delay_time(image_util_encode_h handle, unsigned long long delay_time);

/**
* @brief Sets the input buffer from which to encode.
* @since_tizen 3.0
*
* @remarks Either image_util_encode_set_output_path() or image_util_encode_set_output_buffer() should be set.\n
*             By default the input buffer colorspace will be considered as IMAGE_UTIL_COLORSPACE_RGBA8888.\n
*             Use image_util_encode_set_colorspace to change the colorspace.\n
*             While encoding animated gif image, input buffer should be set for each frame.
*
* @param[in] handle The handle to image util decoding
* @param[in] src_buffer The input image buffer
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_encode_create()
*
* @post image_util_encode_run() / image_util_encode_run_async()
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_set_output_path()
* @see image_util_encode_set_output_buffer()
* @see image_util_encode_run()
* @see image_util_encode_run_async()
* @see image_util_encode_destroy()
*/
int image_util_encode_set_input_buffer(image_util_encode_h handle, const unsigned char *src_buffer);

/**
* @brief Sets the output path to which to encoded buffer will be written to.
* @since_tizen 3.0
*
* @remarks One of image_util_encode_set_output_path() or image_util_encode_set_output_buffer() should be set.\n
*             If both are set then the latest output set is considered.\n
*             http://tizen.org/privilege/mediastorage is needed if input or output path are relevant to media storage.\n
*             http://tizen.org/privilege/externalstorage is needed if input or output path are relevant to external storage.
*
* @param[in] handle The handle to image util encoding
* @param[in] path The output file path
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_NO_SUCH_FILE No such file
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
* @retval #IMAGE_UTIL_ERROR_PERMISSION_DENIED The application does not have the privilege to call this function
*
* @pre image_util_encode_create()
*
* @post image_util_encode_run() / image_util_encode_run_async()
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_run()
* @see image_util_encode_run_async()
* @see image_util_encode_destroy()
*/
int image_util_encode_set_output_path(image_util_encode_h handle, const char *path);

/**
* @brief Sets the output buffer to which to encoded buffer will be written to.
* @since_tizen 3.0
*
* @remarks One of image_util_encode_set_output_path() or image_util_encode_set_output_buffer() should be set.\n
*           If both are set then the latest output set is considered.\n
*           Before 4.0, output buffer setting is not supported for bmp. IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT will be returned for bmp.\n
*           Since 4.0, output buffer setting has been supported for bmp. Applications can set the output buffer to write encoded bmp.\n
*           In case of gif encoding, the output buffer will be completely available only after image_util_encode_destroy().
*           @a dst_buffer should be released after @c image_util_encode_run() or @c image_util_encode_run_async().
*
* @param[in] handle The handle to image util encoding
* @param[in] dst_buffer The output image buffer
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Format not supported
*
* @pre image_util_encode_create()
*
* @post image_util_encode_run() / image_util_encode_run_async()
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_run()
* @see image_util_encode_run_async()
* @see image_util_encode_destroy()
*/
int image_util_encode_set_output_buffer(image_util_encode_h handle, unsigned char **dst_buffer);

/**
* @brief Starts encoding of the image and fills the output buffer, set using image_util_encode_set_output_buffer() or image_util_encode_set_output_path().
* @since_tizen 3.0
*
* @remarks The output will be stored in the pointer set to image_util_encode_set_output_buffer() or image_util_encode_set_output_path().\n
*          The function executes synchronously.\n
*          When any of the @pre functions are not called, IMAGE_UTIL_ERROR_INVALID_PARAMETER is returned.\n
*          In case of animated gif encoding, image_util_encode_set_resolution(), image_util_encode_set_input_buffer() and\n
*          image_util_encode_set_gif_frame_delay_time() MUST be called for each frame.
*          In case of animated gif encoding, call image_util_encode_run() for each frame to encode progressively.
*
* @param[in] handle The handle to image util encoding
* @param[out] size Size of the encoded image
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_encode_create()
* @pre image_util_encode_set_resolution()
* @pre image_util_encode_set_input_buffer()
* @pre image_util_encode_set_output_buffer() / image_util_encode_set_output_path()
*
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_set_gif_frame_delay_time()
* @see image_util_encode_set_output_path()
* @see image_util_encode_set_output_buffer()
* @see image_util_encode_destroy()
*/
int image_util_encode_run(image_util_encode_h handle, unsigned long long *size);

/**
* @brief Starts encoding of the image and fills the output buffer, set using image_util_encode_set_output_buffer() or image_util_encode_set_output_path().
* @since_tizen 3.0
*
* @remarks The output will be stored in the pointer set to image_util_encode_set_output_buffer() or image_util_encode_set_output_path().\n
*          The function executes asynchronously, which contains complete callback.\n
*          When any of the @pre functions are not called, IMAGE_UTIL_ERROR_INVALID_PARAMETER is returned.\n
*          In case of animated gif encoding, image_util_encode_set_resolution(), image_util_encode_set_input_buffer() and\n
*          image_util_encode_set_gif_frame_delay_time() MUST be called for each frame.
*          In case of animated gif encoding, call image_util_encode_run_async() for each frame to encode progressively.
*
* @param[in] handle The handle to image util encoding
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
* @pre image_util_encode_create()
* @pre image_util_encode_set_resolution()
* @pre image_util_encode_set_input_buffer()
* @pre image_util_encode_set_output_buffer() / image_util_encode_set_output_path()
*
* @post image_util_encode_destroy()
*
* @see image_util_encode_create()
* @see image_util_encode_set_resolution()
* @see image_util_encode_set_input_buffer()
* @see image_util_encode_set_gif_frame_delay_time()
* @see image_util_encode_set_output_path()
* @see image_util_encode_set_output_buffer()
* @see image_util_encode_destroy()
*/
int image_util_encode_run_async(image_util_encode_h handle, image_util_encode_completed_cb callback, void *user_data);

/**
* @brief Destroys the image encoding handle.
* @since_tizen 3.0
*
* @remarks Any image handle created should be destroyed.
*
* @param[in] handle The handle to image util encoding
*
* @return @c 0 on success,
*                 otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_encode_create()
*
* @see image_util_encode_create()
*/
int image_util_encode_destroy(image_util_encode_h handle);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_IMAGE_UTIL_ENCODE_H__ */