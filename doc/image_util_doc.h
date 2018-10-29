/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __TIZEN_MEDIA_IMAGE_UTIL_DOC_H__
#define __TIZEN_MEDIA_IMAGE_UTIL_DOC_H__

/**
 * @file image_util_doc.h
 * @brief This file contains high level documentation of the Multimedia Service.
 */

/**
 * @defgroup CAPI_MEDIA_IMAGE_UTIL_MODULE Image Util
 * @brief  The @ref CAPI_MEDIA_IMAGE_UTIL_MODULE  APIs provides functions for common of images .
 *
 * @ingroup CAPI_MEDIA_FRAMEWORK
 *
 * @section CAPI_MEDIA_IMAGE_UTIL_MODULE_HEADER Required Header
 *   \#include <image_util.h>
 *
 * @section CAPI_MEDIA_IMAGE_UTIL_MODULE_OVERVIEW Overview
 *
 * Image Util API provides support for common functions of images. The API allows :
 *- Calculate the size of the image buffer for the specified resolution and colorspace to be used in the image util module.
 *- Extracts representative color from an image buffer.
 *
 * The API consists of @ref CAPI_MEDIA_IMAGE_UTIL_TRANSFORM_MODULE,@ref CAPI_MEDIA_IMAGE_UTIL_ENCODE_DECODE_MODULE.
 */

/**
 * @defgroup CAPI_MEDIA_IMAGE_UTIL_TRANSFORM_MODULE Image Util Transform
 * @brief  The @ref CAPI_MEDIA_IMAGE_UTIL_TRANSFORM_MODULE  APIs provides functions for transforming of images .
 *
 * @ingroup CAPI_MEDIA_IMAGE_UTIL_MODULE
 *
 * @section CAPI_MEDIA_IMAGE_UTIL_MODULE_TRANSFORM_HEADER Required Header
 *   \#include <image_util.h>
 *
 * @section CAPI_MEDIA_IMAGE_UTIL_MODULE_TRANSFORM_OVERVIEW Overview
 *
 * Image Util API provides support for transforming of images. The API allows :
 *- Image transformation(Colorspace conversion/Resize/Rotate/Crop) .
 *
 * The API allows to transform(Colorspace conversion/Resize/Rotate/Crop) including convert the colorspace(#image_util_colorspace_e) of an image by calling
 * #image_util_transform_run() function.
 */

/**
 * @defgroup CAPI_MEDIA_IMAGE_UTIL_ENCODE_DECODE_MODULE Image Util Encode/Decode
 * @brief  The @ref CAPI_MEDIA_IMAGE_UTIL_ENCODE_DECODE_MODULE  APIs provides functions for encoding and decoding of images .
 *
 * @ingroup CAPI_MEDIA_IMAGE_UTIL_MODULE
 *
 * @section CAPI_MEDIA_IMAGE_UTIL_MODULE_ENCODE_DECODE_HEADER Required Header
 *   \#include <image_util.h>
 *
 * @section CAPI_MEDIA_IMAGE_UTIL_MODULE_ENCODE_DECODE_OVERVIEW Overview
 *
 * Image Util API provides support for encoding and decoding of images. The API allows :
 *- Encoding of a JPEG/PNG/GIF/BMP image.
 *- Decoding of a JPEG/PNG/GIF/BMP image.
 * Encoding supports gif images including the animated, but decoding gif does not support the animated.
 *
 * The decoding of a jpeg/png/gif/bmp image located at a given path, to a buffer can be done by calling #image_util_decode_run() function and decoding of a jpeg image on memory
 * can be done by calling #image_util_decode_set_output_buffer(). Similarly, for encoding #image_util_encode_run() and #image_util_encode_set_output_buffer() functions can be called
 * to encode an image buffer to a jpeg/png/gif/bmp image and a buffer respectively.
 *
 * @subsection CAPI_MEDIA_IMAGE_UTIL_ENCODE_DECODE_FOREACH_OPERATIONS Foreach Operations
 * <div><table class="doxtable" >
 *     <tr>
 *        <th><b>FOREACH</b></th>
 *        <th><b>CALLBACK</b></th>
 *        <th><b>DESCRIPTION</b></th>
 *     </tr>
 *     <tr>
 *        <td>image_util_foreach_supported_colorspace()</td>
 *        <td>image_util_supported_colorspace_cb()</td>
 *        <td> Supported JPEG/PNG/GIF/BMP encoding/decoding colorspace</td>
 *     </tr>
 *</table></div>
 */

#endif /* __TIZEN_MEDIA_IMAGE_UTIL_DOC_H__ */
