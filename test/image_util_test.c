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

#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <image_util.h>
#include <image_util_type.h>

#define MAX_STRING_LEN 128
#define IMAGE_FORMAT_LABEL_BUFFER_SIZE 4
#define IMAGE_TEST_MAX_REPEAT_COUNT 100

#define IMAGE_UTIL_SAFE_FREE(src)      { if (src) {free(src); src = NULL; } }


GMainLoop *g_loop = NULL;
transformation_h g_handle = NULL;
media_packet_h g_src = NULL;
char *g_path = NULL;
unsigned int g_width = 0;
unsigned int g_height = 0;
image_util_rotation_e g_angle = IMAGE_UTIL_ROTATION_NONE;
int g_format = -1;

GCond g_thread_cond;
GMutex g_thread_mutex;

enum {
	CURRENT_STATE_MAIN_MENU,
	CURRENT_STATE_SET_IMAGE_MENU,
	CURRENT_STATE_SET_ROTATION_PARAMETER_MENU,
};

enum {
	CURRENT_STATE_SET_IMAGE_NONE,
	CURRENT_STATE_SET_IMAGE_PATH,
	CURRENT_STATE_SET_IMAGE_WIDTH,
	CURRENT_STATE_SET_IMAGE_HEIGHT,
	CURRENT_STATE_SET_IMAGE_FORMAT,
};

int g_menu_state = CURRENT_STATE_MAIN_MENU;
int g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_NONE;
int g_menu_rotation_parameter = IMAGE_UTIL_ROTATION_NONE;

void _wait()
{
	g_mutex_lock(&g_thread_mutex);
	g_printf("waiting... untill finishing transform \n");
	g_cond_wait(&g_thread_cond, &g_thread_mutex);
	g_printf("<=== get signal from callback \n");
	g_mutex_unlock(&g_thread_mutex);
}

void _signal()
{
	g_mutex_lock(&g_thread_mutex);
	g_cond_signal(&g_thread_cond);
	g_printf("===> send signal to test proc \n");
	g_mutex_unlock(&g_thread_mutex);
}

media_format_mimetype_e
_image_util_mapping_imgp_format_to_mime(image_util_colorspace_e colorspace)
{
	media_format_mimetype_e mimetype = -1;

	switch (colorspace) {
	case IMAGE_UTIL_COLORSPACE_NV12:
		mimetype = MEDIA_FORMAT_NV12;
		break;
	case IMAGE_UTIL_COLORSPACE_NV16:
		mimetype = MEDIA_FORMAT_NV16;
		break;
	case IMAGE_UTIL_COLORSPACE_YUYV:
		mimetype = MEDIA_FORMAT_YUYV;
		break;
	case IMAGE_UTIL_COLORSPACE_UYVY:
		mimetype = MEDIA_FORMAT_UYVY;
		break;
	case IMAGE_UTIL_COLORSPACE_YUV422:
		mimetype = MEDIA_FORMAT_422P;
		break;
	case IMAGE_UTIL_COLORSPACE_I420:
		mimetype = MEDIA_FORMAT_I420;
		break;
	case IMAGE_UTIL_COLORSPACE_NV21:
		mimetype = MEDIA_FORMAT_YV12;
		break;
	case IMAGE_UTIL_COLORSPACE_RGB565:
		mimetype = MEDIA_FORMAT_RGB565;
		break;
	case IMAGE_UTIL_COLORSPACE_RGB888:
		mimetype = MEDIA_FORMAT_RGB888;
		break;
	case IMAGE_UTIL_COLORSPACE_RGBA8888:
		mimetype = MEDIA_FORMAT_RGBA;
		break;
	case IMAGE_UTIL_COLORSPACE_ARGB8888:
		mimetype = MEDIA_FORMAT_ARGB;
		break;
	case IMAGE_UTIL_COLORSPACE_BGRA8888:
	case IMAGE_UTIL_COLORSPACE_BGRX8888:
	case IMAGE_UTIL_COLORSPACE_NV61:
	default:
		mimetype = -1;
		g_printf("Not Supported Format");
		break;
	}

	g_printf("imgp fmt[%d] mimetype fmt[%d]\n", colorspace, mimetype);

	return mimetype;
}

bool test_transform_completed_cb(media_packet_h *packet, image_util_error_e error, void *user_data)
{
	uint64_t size = 0;
	char output_file[25] = {};

	media_format_h dst_fmt;
	media_format_mimetype_e dst_mimetype;
	int dst_width, dst_height, dst_avg_bps, dst_max_bps;
	char *output_fmt = NULL;

	g_printf("test_transform_completed_cb============= [%d] \n", error);
	if (error == IMAGE_UTIL_ERROR_NONE) {
		g_printf("<<<<< SUCCESS >>>>>\n");
		output_fmt = (char *)malloc(sizeof(char) * IMAGE_FORMAT_LABEL_BUFFER_SIZE);
		if (output_fmt) {
			if (media_packet_get_format(*packet, &dst_fmt) != MEDIA_PACKET_ERROR_NONE) {
				g_printf("Imedia_packet_get_format");
				_signal();
				IMAGE_UTIL_SAFE_FREE(output_fmt);
				return FALSE;
			}

			if (media_format_get_video_info(dst_fmt, &dst_mimetype, &dst_width, &dst_height, &dst_avg_bps, &dst_max_bps) == MEDIA_FORMAT_ERROR_NONE) {
				memset(output_fmt, 0, IMAGE_FORMAT_LABEL_BUFFER_SIZE);
				if (dst_mimetype  == MEDIA_FORMAT_YV12 || dst_mimetype == MEDIA_FORMAT_422P || dst_mimetype == MEDIA_FORMAT_I420
				    || dst_mimetype == MEDIA_FORMAT_NV12 || dst_mimetype == MEDIA_FORMAT_UYVY || dst_mimetype == MEDIA_FORMAT_YUYV) {
					strncpy(output_fmt, "yuv", strlen("yuv"));
				} else {
					strncpy(output_fmt, "rgb", strlen("rgb"));
				}
				g_printf("[mimetype: %d] W x H : %d x %d \n", dst_mimetype, dst_width, dst_height);
				snprintf(output_file, 25, "result_%dx%d.%s", dst_width, dst_height, output_fmt);
			}
		}

		FILE *fpout = fopen(output_file, "w");
		if (fpout) {
			media_packet_get_buffer_size(*packet, &size);
			void *dst = NULL;
			if (media_packet_get_buffer_data_ptr(*packet, &dst) != MEDIA_PACKET_ERROR_NONE) {
				IMAGE_UTIL_SAFE_FREE(dst);
				IMAGE_UTIL_SAFE_FREE(output_fmt);
				fclose(fpout);
				g_printf("[dst] media_packet_get_extra \n");
				_signal();
				return FALSE;
			}
			//g_printf("dst: %p [%llu] \n", dst, size);
			fwrite(dst, 1, size, fpout);
			g_printf("FREE \n");
			fclose(fpout);
		}

		g_printf("Result is written to %s\n", output_file);
		g_printf("Free (output_fmt) \n");
		IMAGE_UTIL_SAFE_FREE(output_fmt);
	} else {
		g_printf("<<<<< ERROR >>>>> complete cb");
		_signal();
		return FALSE;
	}

	media_packet_destroy(*packet);

	_signal();

	return TRUE;
}

static int
create_media_packet()
{
	int ret = 0;
	media_format_h fmt;
	void *src = NULL;
	void *ptr = NULL;
	if (media_format_create(&fmt) == MEDIA_FORMAT_ERROR_NONE) {
		if (media_format_set_video_mime(fmt, _image_util_mapping_imgp_format_to_mime(g_format)) != MEDIA_FORMAT_ERROR_NONE) {
			media_format_unref(fmt);
			g_printf("[Error] Set - video mime\n");
			return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		}

		if (media_format_set_video_width(fmt, g_width) != MEDIA_FORMAT_ERROR_NONE) {
			media_format_unref(fmt);
			g_printf("[Error] Set - video width\n");
			return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		}

		if (media_format_set_video_height(fmt, g_height) != MEDIA_FORMAT_ERROR_NONE) {
			media_format_unref(fmt);
			g_printf("[Error] Set - video heigh\nt");
			return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		}

		if (media_format_set_video_avg_bps(fmt, 2000000) != MEDIA_FORMAT_ERROR_NONE) {
			media_format_unref(fmt);
			g_printf("[Error] Set - video avg bps\n");
			return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		}

		if (media_format_set_video_max_bps(fmt, 15000000) != MEDIA_FORMAT_ERROR_NONE) {
			media_format_unref(fmt);
			g_printf("[Error] Set - video max bps\n");
			return IMAGE_UTIL_ERROR_INVALID_PARAMETER;
		}

		g_printf("media_format_set_video_info success! file[%s] w[%d] h[%d] mime[%d]\n", g_path, g_width, g_height, _image_util_mapping_imgp_format_to_mime(g_format));
	} else {
		g_printf("media_format_create failed...");
	}

	ret = media_packet_create_alloc(fmt, NULL, NULL, &g_src);
	if (ret == MEDIA_PACKET_ERROR_NONE) {
		g_printf("Success - media_packet_create_alloc\n");
		uint64_t size = 0;
		size_t readed = 0;

		if (media_packet_get_buffer_size(g_src, &size) == MEDIA_PACKET_ERROR_NONE) {
			if (media_packet_get_buffer_data_ptr(g_src, &ptr) == MEDIA_PACKET_ERROR_NONE) {
				FILE *fp = fopen(g_path, "r");
				if (fp == NULL) {
					g_printf("\tfile open failed %d\n", errno);
					return IMAGE_UTIL_ERROR_NO_SUCH_FILE;
				}
				src = malloc(size);
				if (src == NULL) {
					g_printf("\tmemory allocation failed\n");
					fclose(fp);
					return IMAGE_UTIL_ERROR_NO_SUCH_FILE;
				}
				readed = fread(src, 1, (size_t)size, fp);
				if (readed <= size) {
					g_printf("#Success# fread\n");
					memcpy(ptr, src, (int)size);
					g_printf("memcpy\n");
				} else {
					g_printf("#Error# fread readed[%zu] size[%" PRIu64 "]\n", readed, size);
				}
				fclose(fp);
				IMAGE_UTIL_SAFE_FREE(src);
			}
		}
	} else {
		media_format_unref(fmt);
		g_printf("ERROR - media_packet_create_alloc");
		return ret;
	}
	media_format_unref(fmt);
	return ret;
}

static void _create()
{
	int ret = 0;
	if (g_handle != NULL) {
		ret = image_util_transform_destroy(g_handle);
		if (ret != IMAGE_UTIL_ERROR_NONE)
			g_printf("[%d]Error image_util_transform_destroy [%d]\n", __LINE__, ret);
		else
			g_printf("[%d]Success image_util_transform_destroy [%d]\n", __LINE__, ret);
	}
	ret = image_util_transform_create(&g_handle);
	if (ret != IMAGE_UTIL_ERROR_NONE)
		g_printf("[%d]Error image_util_transform_create [%d]\n", __LINE__, ret);
	else
		g_printf("[%d]Success image_util_transform_create [%d]\n", __LINE__, ret);
}

static void _set_image()
{
	int ret = 0;
	if (g_src) {
		media_packet_destroy(g_src);
		g_printf("[%d]Success source packet is destroyed \n", __LINE__);
	}
	ret = create_media_packet();
	if (ret == MEDIA_PACKET_ERROR_NONE)
		g_printf("Success - Create_media_packet\n");
	else
		g_printf("Error - Create_media_packet\n");
}

static void _destroy()
{
	int ret = 0;

	if (g_handle) {
		ret = image_util_transform_destroy(g_handle);
		if (ret != IMAGE_UTIL_ERROR_NONE)
			g_printf("[%d]Error image_util_transform_destroy [%d]\n", __LINE__, ret);
		else
			g_printf("[%d]Success image_util_transform_destroy [%d]\n", __LINE__, ret);
	} else {
		g_printf("[%d]Error handle was already destroyed \n", __LINE__);
	}
	g_handle = NULL;
}

static void _transform(const char *cmd)
{
	int ret = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	image_util_colorspace_e colorspace = IMAGE_UTIL_COLORSPACE_ARGB8888;
	int start_x;
	int start_y;
	int end_x;
	int end_y;

	if (!strcmp("convert", cmd)) {
		ret = image_util_transform_set_colorspace(g_handle, colorspace);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			g_printf("[%d]error image_util_transform_set_colorspace [%d]\n", __LINE__, ret);
			return;
		}
	}

	if (!strcmp("resize", cmd)) {
		width = g_width/2;
		height = g_height/2;

		ret = image_util_transform_set_resolution(g_handle, width, height);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			g_printf("[%d]Error image_util_transform_set_resolution [%d]\n", __LINE__, ret);
			return;
		}
	}

	if (!strcmp("rotate", cmd)) {
		ret = image_util_transform_set_rotation(g_handle, g_angle);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			g_printf("[%d]Error image_util_transform_set_rotation [%d]\n", __LINE__, ret);
			return;
		}
	}

	if (!strcmp("crop", cmd)) {
		start_x = g_width/4;
		start_y = g_height/3;
		end_x = g_width*3/4;
		end_y = g_height;

		ret = image_util_transform_set_crop_area(g_handle, start_x, start_y, end_x, end_y);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			g_printf("[%d]Error image_util_transform_set_crop_area [%d]\n", __LINE__, ret);
			return;
		}
	}

	if (!strcmp("run", cmd)) {
		ret = image_util_transform_run(g_handle, g_src, (image_util_transform_completed_cb)test_transform_completed_cb, NULL);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			g_printf("[%d]Error image_util_transform_run [%d]\n", __LINE__, ret);
			return;
		}
		_wait();
	}

}

static void _loop_test(const int count)
{
	int i = 0;
	for (i = 0; i < count; i++) {
		_transform("run");
		_set_image();
		g_printf("<<<<< %03d >>>>>\n", i);
	}
}

void quit(void)
{
	if (g_loop)
		g_main_loop_quit(g_loop);
}

void reset_current_menu_state()
{
	g_menu_state = CURRENT_STATE_MAIN_MENU;
	return;
}

static void display_set_rotation_parameter_menu(void)
{
	g_print("\n");
	g_print("====================================================\n");
	g_print("    Enter number of rotate operation parameter\n");
	g_print("----------------------------------------------------\n");
	g_print("0. None\n");
	g_print("1. Rotation 90 degrees\n");
	g_print("2. Rotation 180 degrees\n");
	g_print("3. Rotation 270 degrees\n");
	g_print("4. Flip horizontal\n");
	g_print("5. Flip vertical\n");
	g_print("====================================================\n");
}

static void display_set_image_menu(void)
{
	g_print("\n");
	g_print("====================================================\n");
	g_print("    image-util Core-API test: Main menu v0.2\n");
	g_print("----------------------------------------------------\n");
	g_print("1. set image path \n");
	g_print("2. set image width \n");
	g_print("3. set image height \n");
	g_print("4. set image format \n");
	g_print("0. back \n");
	g_print("----------------------------------------------------\n");
	g_print("====================================================\n");

}

static void display_menu(void)
{
	g_print("\n");
	g_print("====================================================\n");
	g_print("   image-util Core-API test: Main menu v0.2\n");
	g_print("----------------------------------------------------\n");
	g_print("1. create handle \n");
	g_print("2. set image \n");
	g_print("3. set convert \n");
	g_print("4. set crop \n");
	g_print("5. set resize \n");
	g_print("6. set rotate \n");
	g_print("7. run \n");
	g_print("8. run repeatly \n");
	g_print("9. destroy handle \n");
	g_print("0. quit \n");
	g_print("----------------------------------------------------\n");
	g_print("====================================================\n");

}

static void interpret_rotation_parameter_cmd(char *cmd)
{
	int choice = atoi(cmd);

	if (choice < IMAGE_UTIL_ROTATION_NONE || choice > IMAGE_UTIL_ROTATION_FLIP_VERT) {
		g_print("wrong rotation parameter\n");
		display_set_rotation_parameter_menu();
	}
	g_angle = choice;
}

static void interpret_set_image_cmd(char *cmd)
{
	int len = strlen(cmd);
	if (g_menu_set_image_state == CURRENT_STATE_SET_IMAGE_NONE) {
		if (len == 1) {
			if (!strncmp(cmd, "1", len)) {
				g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_PATH;
				g_print("Path: ");
			} else if (!strncmp(cmd, "2", len)) {
				g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_WIDTH;
				g_print("Width: ");
			} else if (!strncmp(cmd, "3", len)) {
				g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_HEIGHT;
				g_print("height: ");
			} else if (!strncmp(cmd, "4", len)) {
				g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_FORMAT;
				g_print("Format: ");
			} else if (!strncmp(cmd, "0", len)) {
				_set_image();
				reset_current_menu_state();
				display_menu();
			}
		} else {
			g_print("wrong command\n");
			display_set_image_menu();
		}
	} else if (g_menu_set_image_state == CURRENT_STATE_SET_IMAGE_PATH) {
		IMAGE_UTIL_SAFE_FREE(g_path);
		g_path = (char *)g_malloc(MAX_STRING_LEN * sizeof(char));
		if (g_path == NULL) {
			g_printf("memory allocation fail. \n");
			return;
		}
		memset(g_path, 0x00, MAX_STRING_LEN);
		snprintf(g_path, MAX_STRING_LEN, "%s", cmd);
		g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_NONE;
		display_set_image_menu();
	} else if (g_menu_set_image_state == CURRENT_STATE_SET_IMAGE_WIDTH) {
		g_width = atoi(cmd);
		g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_NONE;
		display_set_image_menu();
	} else if (g_menu_set_image_state == CURRENT_STATE_SET_IMAGE_HEIGHT) {
		g_height = atoi(cmd);
		g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_NONE;
		display_set_image_menu();
	} else if (g_menu_set_image_state == CURRENT_STATE_SET_IMAGE_FORMAT) {
		g_format = atoi(cmd);
		g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_NONE;
		display_set_image_menu();
	}
}

static void interpret_cmd(char *cmd)
{
	int len = strlen(cmd);
	if (g_menu_state == CURRENT_STATE_MAIN_MENU) {
		if (len == 1) {
			if (!strncmp(cmd, "1", len)) {
				_create();
			} else if (!strncmp(cmd, "2", len)) {
				g_menu_state = CURRENT_STATE_SET_IMAGE_MENU;
				g_menu_set_image_state = CURRENT_STATE_SET_IMAGE_NONE;
				display_set_image_menu();
				return;
			} else if (!strncmp(cmd, "3", len)) {
				_transform("convert");
			} else if (!strncmp(cmd, "4", len)) {
				_transform("crop");
			} else if (!strncmp(cmd, "5", len)) {
				_transform("resize");
			} else if (!strncmp(cmd, "6", len)) {
				g_menu_state = CURRENT_STATE_SET_ROTATION_PARAMETER_MENU;
				display_set_rotation_parameter_menu();
				return;
			} else if (!strncmp(cmd, "7", len)) {
				_transform("run");
			} else if (!strncmp(cmd, "8", len)) {
				_loop_test(IMAGE_TEST_MAX_REPEAT_COUNT);
			} else if (!strncmp(cmd, "9", len)) {
				_destroy();
			} else if (!strncmp(cmd, "0", len)) {
				quit();
			}
		} else {
			g_print("wrong command\n");
		}

		display_menu();
	} else if (g_menu_state == CURRENT_STATE_SET_IMAGE_MENU) {
		interpret_set_image_cmd(cmd);
	} else if (g_menu_state == CURRENT_STATE_SET_ROTATION_PARAMETER_MENU) {
		interpret_rotation_parameter_cmd(cmd);
		_transform("rotate");
		reset_current_menu_state();
		display_menu();
	} else {
		g_print("wrong menu state\n");
	}
}

gboolean input(GIOChannel *channel)
{
	gchar buf[MAX_STRING_LEN];
	gsize read;
	GError *error = NULL;

	g_io_channel_read_chars(channel, buf, MAX_STRING_LEN, &read, &error);

	buf[read] = '\0';
	g_strstrip(buf);
	interpret_cmd(buf);

	return TRUE;
}

int main(int argc, char **argv)
{
	int ret = IMAGE_UTIL_ERROR_NONE;

	GIOChannel *stdin_channel = NULL;
	stdin_channel = g_io_channel_unix_new(0);
	g_io_channel_set_flags(stdin_channel, G_IO_FLAG_NONBLOCK, NULL);
	g_io_add_watch(stdin_channel, G_IO_IN, (GIOFunc)input, NULL);

	if (argc < 4) {
		g_printf("Do: export XDG_RUNTIME_DIR=/run \n");
		g_printf("Usage: image_utl_test filename width height format \n");
		return ret;
	}

	g_handle = NULL;
	g_src = NULL;
	g_path = (char *)g_malloc(MAX_STRING_LEN * sizeof(char));
	if (g_path == NULL) {
		g_printf("memory allocation fail. \n");
		return ret;
	}

	snprintf(g_path, MAX_STRING_LEN, "%s", argv[1]);
	g_width = atoi(argv[2]);
	g_height = atoi(argv[3]);
	g_format = atoi(argv[4]);

	g_mutex_init(&g_thread_mutex);
	g_cond_init(&g_thread_cond);

	ret = image_util_transform_create(&g_handle);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		g_printf("[%d]Error image_util_transform_create [%d]\n", __LINE__, ret);
		goto Exit;
	}

	ret = create_media_packet();
	if (ret == MEDIA_PACKET_ERROR_NONE) {
		g_printf("Success - Create_media_packet\n");
	} else {
		g_printf("Error - Create_media_packet\n");
		goto Exit;
	}

	display_menu();

	g_loop = g_main_loop_new(NULL, FALSE);

	g_main_loop_run(g_loop);
	g_main_loop_unref(g_loop);

Exit:
	g_mutex_clear(&g_thread_mutex);
	g_cond_clear(&g_thread_cond);
	if (g_path) {
		IMAGE_UTIL_SAFE_FREE(g_path);
		g_printf("[%d]Success file path is destroyed \n", __LINE__);
	} else {
		g_printf("[%d]Error file path was already destroyed \n", __LINE__);
	}
	if (g_src) {
		media_packet_destroy(g_src);
		g_printf("[%d]Success source packet is destroyed \n", __LINE__);
	} else {
		g_printf("[%d]Error source packet was already destroyed \n", __LINE__);
	}
	_destroy();

	return 0;
}
