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

#ifndef __TIZEN_MULTIMEDIA_IMAGE_UTIL_H__
#define __TIZEN_MULTIMEDIA_IMAGE_UTIL_H__

#include <image_util_type.h>
#include <image_util_decode.h>
#include <image_util_encode.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file image_util.h
 * @brief This file contains the image util API.
 */

/**
 * @addtogroup CAPI_MEDIA_IMAGE_UTIL_MODULE
 * @{
 */

/**
* @brief Calculates the size of the image buffer for the specified resolution and colorspace.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @param[in] width The image width
* @param[in] height The image height
* @param[in] colorspace The image colorspace
* @param[out] size The Calculated buffer size
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
*
* @see image_util_transform_run()
*/
int image_util_calculate_buffer_size(int width, int height, image_util_colorspace_e colorspace , unsigned int *size);

/**
* @brief Extracts representative color from an image buffer.
* @since_tizen 3.0
* @remarks @a image_buffer should be RGB888 colorspace.
*
* @param[in] image_buffer The original image buffer
* @param[in] width The image width
* @param[in] height The image height
* @param[out] rgb_r The red color in RGB color space
* @param[out] rgb_g The green color in RGB color space
* @param[out] rgb_b The blue color in RGB color space
*
* @return 0 on success,
*           otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_OUT_OF_MEMORY Out of memory
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
*/
int image_util_extract_color_from_memory(const unsigned char *image_buffer, int width, int height, unsigned char *rgb_r, unsigned char *rgb_g, unsigned char *rgb_b);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_IMAGE_UTIL_TRANSFORM_MODULE
 * @{
 */

/**
* @brief Creates a handle to image util transform.
* @details This function creates a handle to image util transform.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @remarks The @a handle should be released using image_util_transform_destroy().
*
* @param[out] handle A handle to image util transform
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_OUT_OF_MEMORY Out of memory
*
* @see image_util_transform_destroy()
*
*/
int image_util_transform_create(transformation_h *handle);

/**
* @deprecated Deprecated since 5.0.
* @brief Sets the image util's accurate mode.
* @details This function set if you use hardware acceleration or not.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
*
* @remarks The value returned will be IMAGE_UTIL_ERROR_NOT_SUPPORTED, if H/W acceleration doesn't support on the device.
*
* @param[in] handle The handle to image util
* @param[in] mode Set @c true, user can use the hardware acceleration\n
*                               otherwise set @c false if user can only software image processing
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED The application does not have the hardware acceleration
*
* @pre image_util_transform_create().
*
* @see image_util_transform_create()
* @see image_util_transform_destroy()
*/
int image_util_transform_set_hardware_acceleration(transformation_h handle, bool mode) TIZEN_DEPRECATED_API;

/**
* @brief Sets the information of the converting.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @param[in] handle The handle to image util transform
* @param[in] colorspace The colorspace of the image buffer
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Not supported format
*
* @pre image_util_transform_create().
*
* @see image_util_transform_create()
* @see image_util_transform_run()
* @see image_util_transform_destroy()
*/
int image_util_transform_set_colorspace(transformation_h handle, image_util_colorspace_e colorspace);

/**
* @brief Sets the information of the resizing.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @remarks image_util_transform_set_resolution and image_util_transform_set_crop_area can't do that at the same time.
*
* @param[in] handle The handle to image util transform
* @param[in] width The width of image buffer
* @param[in] height The height of image buffer
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_transform_create().
*
* @see image_util_transform_create()
* @see image_util_transform_run()
* @see image_util_transform_destroy()
*/
int image_util_transform_set_resolution(transformation_h handle, unsigned int width, unsigned int height);

/**
* @brief Sets the information of the rotating.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @param[in] handle The handle to image util transform
* @param[in] rotation The rotation value of image buffer
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_transform_create().
*
* @see image_util_transform_create()
* @see image_util_transform_run()
* @see image_util_transform_destroy()
*/
int image_util_transform_set_rotation(transformation_h handle, image_util_rotation_e rotation);

/**
* @brief Sets the information of the cropping.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @remarks image_util_transform_set_resolution and image_util_transform_set_crop_area can't do that at the same time.
*
* @param[in] handle The handle to image util transform
* @param[in] start_x The start x position of cropped image buffer
* @param[in] start_y The start y position of cropped image buffer
* @param[in] end_x The end x position of cropped image buffer
* @param[in] end_y The end y position of cropped image buffer
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_transform_create().
*
* @see image_util_transform_create()
* @see image_util_transform_run()
* @see image_util_transform_destroy()
*/
int image_util_transform_set_crop_area(transformation_h handle, unsigned int start_x, unsigned int start_y, unsigned int end_x, unsigned int end_y);

/**
* @brief Gets the colorspace of the image buffer.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @param[in] handle The handle to image util transform
* @param[in,out] colorspace The colorspace of the image buffer
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
* @retval #IMAGE_UTIL_ERROR_NOT_SUPPORTED_FORMAT Not supported format
*
* @pre image_util_transform_create().\n
*            image_util_transform_set_crop_area().
*
* @see image_util_transform_create()
* @see image_util_transform_destroy()
*/
int image_util_transform_get_colorspace(transformation_h handle, image_util_colorspace_e *colorspace);

/**
* @brief Gets the resolution of the image buffer.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @param[in] handle The handle to image util transform
* @param[in,out] width The width of source image buffer
* @param[in,out] height The height of source image buffer
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_transform_create().\n
*            image_util_transform_set_resolution().
*
* @see image_util_transform_create()
* @see image_util_transform_destroy()
*/
int image_util_transform_get_resolution(transformation_h handle, unsigned int *width, unsigned int *height);

/**
* @brief Gets the information of the rotating.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @param[in] handle The handle to image util transform
* @param[in,out] rotation The rotation value of image buffer
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_transform_create().\n
*            image_util_transform_set_rotation().
*
* @see image_util_transform_create()
* @see image_util_transform_destroy()
*/
int image_util_transform_get_rotation(transformation_h handle, image_util_rotation_e *rotation);

/**
* @brief Gets the information of the cropping.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @param[in] handle The handle to image util transform
* @param[in,out] start_x The start x position of cropped source image buffer
* @param[in,out] start_y The start y position of cropped source image buffer
* @param[in,out] end_x The end x position of cropped source image buffer
* @param[in,out] end_y The end y position of cropped source image buffer
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @pre image_util_transform_create().\n
*            image_util_transform_set_crop_area().
*
* @see image_util_transform_create()
* @see image_util_transform_destroy()
*/
int image_util_transform_get_crop_area(transformation_h handle, unsigned int *start_x, unsigned int *start_y, unsigned int *end_x, unsigned int *end_y);

/**
* @brief Transforms the image for given image util handle.
* @details The function execute asynchronously, which contains complete callback \n
*          If you set more than two transforming, the order of running is crop or resolution, colorspace converting, rotation. \n
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @remarks If H/W acceleration is not set, transformation is done via S/W acceleration.
*
* @param[in] handle The handle of transform
* @param[in] src The handle to image util transform
* @param[in] callback The callback function to be invoked
* @param[in] user_data The user data to be passed to the callback function
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
* @retval #IMAGE_UTIL_ERROR_INVALID_OPERATION Invalid operation
*
* @see image_util_transform_create()
* @see image_util_transform_destroy()
*/
int image_util_transform_run(transformation_h handle, media_packet_h src, image_util_transform_completed_cb callback, void *user_data);

/**
* @brief Destroys a handle to image util.
* @details The function frees all resources related to the image util handle. The image util
*               handle no longer can be used to perform any operations. A new image util handle
*               has to be created before the next usage.
* @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*
* @param[in] handle The handle to image util transform
*
* @return @c 0 on success,
*               otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
*
* @see image_util_transform_create()
*
*/
int image_util_transform_destroy(transformation_h handle);

/**
 * @}
 */

/**
* @addtogroup CAPI_MEDIA_IMAGE_UTIL_ENCODE_DECODE_MODULE
* @{
*/

/**
* @brief Called once for each supported image encode/decode colorspace.
* @since_tizen 3.0
*
* @param[in] colorspace The colorspace
* @param[in] user_data The user data passed from the foreach function
*
* @return @c true to continue with the next iteration of the loop, \n
*         otherwise @c false to break out of the loop
*
* @pre image_util_foreach_supported_colorspace() invokes this callback.
*
* @see image_util_foreach_supported_colorspace()
* @see image_util_encode_create()
* @see image_util_decode_create()
*/
typedef bool(*image_util_supported_colorspace_cb)(image_util_colorspace_e colorspace, void *user_data);

/**
* @brief Retrieves all supported image encoding/decoding colorspace by invoking a callback function once for each one.
* @since_tizen 3.0
*
* @param[in] image_type The type of supported image for which to create encode/decode handle
* @param[in] callback The callback function to invoke
* @param[in] user_data The user data to be passed to the callback function
* @return 0 on success,
*         otherwise a negative error value
*
* @retval #IMAGE_UTIL_ERROR_NONE Successful
* @retval #IMAGE_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
*
* @post This function invokes image_util_supported_colorspace_cb() repeatedly to retrieve each supported image encoding/decoding colorspace.
*
* @see image_util_supported_colorspace_cb()
* @see image_util_encode_create()
* @see image_util_decode_create()
*/
int image_util_foreach_supported_colorspace(image_util_type_e image_type, image_util_supported_colorspace_cb callback, void *user_data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_IMAGE_UTIL_H__ */
