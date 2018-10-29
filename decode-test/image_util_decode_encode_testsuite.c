/*
 * image-utility
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Vineeth T M <vineeth.tm@samsung.com>
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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <image_util.h>
#include <image_util_internal.h>
#include <glib.h>
#include <tzplatform_config.h>

#define DECODE_RESULT_PATH tzplatform_mkpath(TZ_USER_CONTENT, "decode_test.")
#define ENCODE_RESULT_PATH tzplatform_mkpath(TZ_USER_CONTENT, "encode_test.")

#define ARGC_MIN 3
#define CMD_MAX 30
#define TEST_FILE_MAX 100

typedef enum {
	TEST_DECODE_FILE = 0,
	TEST_DECODE_MEM,
	TEST_DECODE_ASYNC,
	TEST_DECODE_MEM_ASYNC,
	LAST_DECODE_TEST = TEST_DECODE_MEM_ASYNC,
	FIRST_GIF_TEST,
	GIFTEST_ENCODE_FILE = FIRST_GIF_TEST,
	GIFTEST_ENCODE_MEM,
	GIFTEST_ENCODE_FRAME_FILE,		/* interanl */
	GIFTEST_ENCODE_FRAME_MEM,	/* interanl */
	TEST_COMMAND_NUM,
} test_command_e;

typedef struct {
	test_command_e cmd;
	char *path;
	unsigned int image_type;
	void *buffer;
	unsigned long long buffer_size;
} test_inputs_s;

typedef struct {
	gboolean decode_result;
	char filepath[PATH_MAX];
	unsigned long width;
	unsigned long height;
	unsigned char *decoded;
	unsigned long long decode_size;
} test_decode_s;

typedef struct {
	char out_path[PATH_MAX];
	unsigned char *encoded;
	unsigned long long encode_size;
} test_encode_s;

static unsigned int g_num_of_files;
static unsigned int g_num_of_decoded;

static test_inputs_s g_test_input;
static test_decode_s g_test_decode[TEST_FILE_MAX];
static test_encode_s g_test_encode;

static char TEST_CMD[][CMD_MAX] = {
	"decode",
	"decode-mem",
	"decode-async",
	"decode-mem-async",
	"encode-gif",
	"encode-gif-mem",
	"encode-gif-frame",			/* internal */
	"encode-gif-frame-mem",	/* internal */
};

GCond g_thread_cond;
GMutex g_thread_mutex;

void _wait()
{
	g_mutex_lock(&g_thread_mutex);
	fprintf(stderr, "waiting... until finishing \n");
	g_cond_wait(&g_thread_cond, &g_thread_mutex);
	fprintf(stderr, "<=== get signal from callback \n");
	g_mutex_unlock(&g_thread_mutex);
}

void _signal()
{
	g_mutex_lock(&g_thread_mutex);
	g_cond_signal(&g_thread_cond);
	fprintf(stderr, "===> send signal to test proc \n");
	g_mutex_unlock(&g_thread_mutex);
}

static inline void flush_stdin()
{
	int ch;
	while ((ch = getchar()) != EOF && ch != '\n') ;
}

static gboolean _read_file(const char *file_name, void **data, size_t *data_size)
{
	FILE *fp = NULL;
	long file_size = 0;

	if (!file_name || !data || !data_size) {
		fprintf(stderr, "\tNULL pointer\n");
		return FALSE;
	}

	fprintf(stderr, "\tTry to open %s to read\n", file_name);

	fp = fopen(file_name, "r");
	if (fp == NULL) {
		fprintf(stderr, "\tfile open failed %d\n", errno);
		return FALSE;
	}

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	if (file_size < 0) {
		fprintf(stderr, "\t[JPEG_testsuite] ftell failed \n");
		fclose(fp);
		fp = NULL;
		return FALSE;
	}

	rewind(fp);
	*data = (void *)malloc(file_size);
	if (*data == NULL) {
		fprintf(stderr, "\tmalloc failed %d\n", errno);
		fclose(fp);
		fp = NULL;
		return FALSE;
	}

	*data_size = fread(*data, 1, file_size, fp);
	if (*data_size != (size_t)file_size) {
		fprintf(stderr, "#Error# fread\n");
		fclose(fp);
		fp = NULL;
		free(*data);
		*data = NULL;
		*data_size = 0;
		return FALSE;
	}

	fprintf(stderr, "#Success# fread\n");

	fclose(fp);
	fp = NULL;

	return TRUE;
}

static gboolean _write_file(const char *file_name, void *data, size_t data_size)
{
	FILE *fp = NULL;

	if (!file_name || !data || data_size == 0) {
		fprintf(stderr, "\tinvalid data %s %p size:%zu\n", file_name, data, data_size);
		return FALSE;
	}

	fprintf(stderr, "\tTry to open %s to write\n", file_name);

	fp = fopen(file_name, "w");
	if (fp == NULL) {
		fprintf(stderr, "\tfile open failed %d\n", errno);
		return FALSE;
	}

	fwrite(data, 1, data_size, fp);
	fclose(fp);
	fp = NULL;

	fprintf(stderr, "\tfile [%s] write DONE\n", file_name);

	return TRUE;
}

bool decode_completed_cb(int error, void *user_param, unsigned long width, unsigned long height, unsigned long long size)
{
	test_decode_s *user_data = (test_decode_s *)user_param;
	user_data->width = width;
	user_data->height = height;
	user_data->decode_size = size;
	_signal();

	return TRUE;
}

bool encode_completed_cb(int error, void *user_param, unsigned long long size)
{
	test_encode_s *user_data = (test_encode_s *)user_param;
	user_data->encode_size = size;
	_signal();

	return TRUE;
}

void _free_inputs()
{
	if (g_test_input.path != NULL) {
		g_free(g_test_input.path);
		g_test_input.path = NULL;
	}
	if (g_test_input.buffer != NULL) {
		g_free(g_test_input.buffer);
		g_test_input.buffer = NULL;
	}
}

void _free_decode()
{
	unsigned int i = 0;

	for (i = 0; i < g_num_of_files; i++) {
		if (g_test_decode[i].decoded != NULL) {
			free(g_test_decode[i].decoded);
			g_test_decode[i].decoded = NULL;
		}
	}
}

void _free_encode()
{
	if (g_test_encode.encoded != NULL) {
		free(g_test_encode.encoded);
		g_test_encode.encoded = NULL;
	}
}

void _free_datas()
{
	_free_inputs();
	_free_decode();
	_free_encode();
}

gboolean _init_datas()
{
	memset(&g_test_input, 0, sizeof(test_inputs_s));
	memset(&g_test_decode, 0, sizeof(test_decode_s) * TEST_FILE_MAX);
	memset(&g_test_encode, 0, sizeof(test_encode_s));

	g_num_of_files = 0;
	g_num_of_decoded = 0;

	return TRUE;
}

gboolean _read_dir()
{
	struct dirent *dp = NULL;
	DIR *fd = opendir(g_test_input.
path);
	unsigned int i = 0, j = 0;
	if (fd == NULL) {
		fprintf(stderr, "\tlistdir: can't open %s\n", g_test_input.
path);
		return FALSE;
	}

	while ((dp = readdir(fd)) != NULL) {
		if (strlen(dp->d_name) == 0)
			continue;
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
			continue;	/* skip self and parent */
		size_t nbytes = g_snprintf(g_test_decode[g_num_of_files].filepath, sizeof(g_test_decode[g_num_of_files].filepath), "%s%s", g_test_input.
path, dp->d_name);
		if (nbytes == 0)
			continue;
		g_num_of_files++;
	}

	closedir(fd);

	if (g_num_of_files == 0) {
		fprintf(stderr, "\tNo Test File!\n");
		return FALSE;
	}

	/* sort file_name */
	char temp[PATH_MAX];
	for (i = 0; i < g_num_of_files; i++) {
		for (j = 0; j < g_num_of_files - 1; j++) {
			if ((strlen(g_test_decode[j].filepath) > strlen(g_test_decode[j + 1].filepath)) ||
				(strcmp(g_test_decode[j].filepath, g_test_decode[j + 1].filepath) > 0 && strlen(g_test_decode[j].filepath) == strlen(g_test_decode[j + 1].filepath))) {
				memset(temp, 0, PATH_MAX);
				g_strlcpy(temp, g_test_decode[j].filepath, sizeof(temp));
				g_strlcpy(g_test_decode[j].filepath, g_test_decode[j + 1].filepath, sizeof(g_test_decode[j].filepath));
				g_strlcpy(g_test_decode[j + 1].filepath, temp, sizeof(g_test_decode[j + 1].filepath));
			}
		}
	}

	return TRUE;
}

gboolean _parse_inputs(int argc, char *argv[])
{
	unsigned int i = 0;
	gboolean result = FALSE;

	if (argv[1] == NULL || strlen(argv[1]) == 0) return FALSE;

	for (i = 0; i < TEST_COMMAND_NUM; i++) {
		if (g_strcmp0(argv[1], TEST_CMD[i]) == 0) {
			g_test_input.cmd = i;
			result = TRUE;
		}
	}
	if (!result) return FALSE;

	if (argv[2] == NULL || strlen(argv[2]) == 0) return FALSE;

	g_test_input.path = g_strdup(argv[2]);
	if (g_test_input.path == NULL)
		return FALSE;

	if (g_test_input.cmd >= FIRST_GIF_TEST) {
		g_test_input.image_type = IMAGE_UTIL_GIF;
		size_t nbytes = g_snprintf(g_test_encode.out_path, PATH_MAX, "%s%s", ENCODE_RESULT_PATH, "gif");
		if (nbytes == 0)
			return FALSE;
		return result;
	}

	if (argv[3] == NULL || strlen(argv[3]) == 0) return FALSE;

	long temp = g_ascii_strtoll(argv[3], NULL, 10);
	if (temp < 0 || temp > (long)IMAGE_UTIL_BMP)
		return FALSE;
	g_test_input.image_type  = (unsigned int)temp;

	char ext[4];
	memset(g_test_encode.out_path, 0, PATH_MAX);
	memset(ext, 0, 4);
	switch (g_test_input.image_type) {
	case IMAGE_UTIL_JPEG:
		snprintf(ext, 4, "%s", "jpg");
		break;
	case IMAGE_UTIL_PNG:
		snprintf(ext, 4, "%s", "png");
		break;
	case IMAGE_UTIL_GIF:
		snprintf(ext, 4, "%s", "gif");
		break;
	case IMAGE_UTIL_BMP:
		snprintf(ext, 4, "%s", "bmp");
		break;
	default:
		fprintf(stderr, "\tNot supported image type!\n");
		_free_datas();
		return FALSE;
	}
	snprintf(g_test_encode.out_path, PATH_MAX, "%s%s", ENCODE_RESULT_PATH, ext);

	return result;
}

void _print_help(int argc, char *argv[])
{
	unsigned int i = 0;

	fprintf(stderr, "\t[usage]\n");
	fprintf(stderr, "\t\t1. decode & encode : %s ", argv[0]);
	for (i = 0; i <= LAST_DECODE_TEST; i++) {
		fprintf(stderr, "%s", TEST_CMD[i]);
		if (i != LAST_DECODE_TEST)
			fprintf(stderr, "/");
	}
	fprintf(stderr, " filepath encode_image_type\n");
	fprintf(stderr, "\t\t2. encode animated-gif : %s ", argv[0]);
	for (i = FIRST_GIF_TEST; i < TEST_COMMAND_NUM; i++) {
		fprintf(stderr, "%s", TEST_CMD[i]);
		if (i != TEST_COMMAND_NUM - 1)
			fprintf(stderr, "/");
	}
	fprintf(stderr, " folderpath containing png images named \
				with number prefix according to the animation order'\n");
}

gboolean _read_test_files()
{
	if (g_test_input.cmd >= FIRST_GIF_TEST) {
		if (_read_dir() == FALSE)
			return FALSE;
	} else {
		if ((g_test_input.cmd == TEST_DECODE_MEM) || (g_test_input.cmd == TEST_DECODE_MEM_ASYNC)) {
			size_t read_size = 0;
			if (_read_file(g_test_input.path, &g_test_input.buffer, &read_size) == FALSE) {
				fprintf(stderr, "\tRead test file failed!\n");
				return FALSE;
			}
			g_test_input.buffer_size = (unsigned long long)read_size;
		} else {
			size_t nbytes = g_snprintf(g_test_decode[0].filepath, PATH_MAX, "%s", g_test_input.
path);
			if (nbytes == 0) return FALSE;
		}
		g_num_of_files = 1;
	}

	fprintf(stderr, "\tThe %d files are readed!\n", g_num_of_files);

	return TRUE;
}

gboolean test_decode()
{
	int ret = 0;
	unsigned int i = 0;
	image_util_decode_h decoded = NULL;

	for (i = 0; i < g_num_of_files; i++) {
		g_test_decode[i].decode_result = FALSE;

		ret = image_util_decode_create(&decoded);
		if (ret != IMAGE_UTIL_ERROR_NONE)
			return FALSE;

		if ((g_test_input.cmd == TEST_DECODE_MEM) || (g_test_input.cmd == TEST_DECODE_MEM_ASYNC))
			ret = image_util_decode_set_input_buffer(decoded, (unsigned char *)g_test_input.buffer, g_test_input.buffer_size);
		else
			ret = image_util_decode_set_input_path(decoded, g_test_decode[i].filepath);

		if (ret != IMAGE_UTIL_ERROR_NONE) {
			image_util_decode_destroy(decoded);
			return FALSE;
		}

		ret = image_util_decode_set_output_buffer(decoded, &(g_test_decode[i].decoded));
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			image_util_decode_destroy(decoded);
			return FALSE;
		}

		if ((g_test_input.cmd == TEST_DECODE_ASYNC) || (g_test_input.cmd == TEST_DECODE_MEM_ASYNC)) {
			ret = image_util_decode_run_async(decoded, (image_util_decode_completed_cb) decode_completed_cb, &g_test_decode[i]);
			if (ret == IMAGE_UTIL_ERROR_NONE)
				_wait();
		} else {
			ret = image_util_decode_run(decoded, &g_test_decode[i].width, &g_test_decode[i].height, &g_test_decode[i].decode_size);
		}
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			image_util_decode_destroy(decoded);
			return FALSE;
		}

		image_util_decode_destroy(decoded);
		g_test_decode[i].decode_result = TRUE;
		g_num_of_decoded++;
	}

	if (g_num_of_decoded == 0) {
		fprintf(stderr, "\tNo decoded data!\n");
		return FALSE;
	} else {
		fprintf(stderr, "\tThe %d images are decoded!(0: %p %llu)\n", g_num_of_decoded, g_test_decode[0].decoded, g_test_decode[0].decode_size);
	}

	/* write the decoded result to the file */
	if (g_test_input.cmd <= LAST_DECODE_TEST) {
		char temp[PATH_MAX];
		memset(temp, 0, PATH_MAX);
		snprintf(temp, PATH_MAX, "%s%s", DECODE_RESULT_PATH, "raw");
		if (_write_file(temp, g_test_decode[0].decoded, (size_t)g_test_decode[0].decode_size) == FALSE) {
			fprintf(stderr, "\tWrite the decoded result failed!\n");
			return FALSE;
		}
	}

	return TRUE;
}

gboolean test_encode()
{
	int ret = 0;
	image_util_encode_h encoded = NULL;

	ret = image_util_encode_create(g_test_input.image_type, &encoded);
	if (ret != IMAGE_UTIL_ERROR_NONE)
		return FALSE;

	ret = image_util_encode_set_input_buffer(encoded,  g_test_decode[0].decoded);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	ret = image_util_encode_set_resolution(encoded,  g_test_decode[0].width, g_test_decode[0].height);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	if ((g_test_input.cmd == TEST_DECODE_MEM) || (g_test_input.cmd == TEST_DECODE_MEM_ASYNC))
		ret = image_util_encode_set_output_buffer(encoded, &g_test_encode.encoded);
	else
		ret = image_util_encode_set_output_path(encoded, g_test_encode.out_path);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	if ((g_test_input.cmd == TEST_DECODE_ASYNC) || (g_test_input.cmd == TEST_DECODE_MEM_ASYNC)) {
		ret = image_util_encode_run_async(encoded, (image_util_encode_completed_cb) encode_completed_cb, &g_test_encode);
		if (ret == IMAGE_UTIL_ERROR_NONE)
			_wait();
	} else {
		ret = image_util_encode_run(encoded, &g_test_encode.encode_size);
	}
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	image_util_encode_destroy(encoded);

	if ((g_test_input.cmd == TEST_DECODE_MEM) ||
		(g_test_input.cmd == TEST_DECODE_MEM_ASYNC)) {
		if (_write_file(g_test_encode.out_path, g_test_encode.encoded, (size_t)g_test_encode.encode_size) == FALSE) {
			fprintf(stderr, "\tWrite the encoded result failed!\n");
			return FALSE;
		}
	}

	return TRUE;
}

gboolean test_encode_gif()
{
	int ret = 0;
	unsigned int i = 0;
	image_util_encode_h encoded = NULL;

	ret = image_util_encode_create(g_test_input.image_type, &encoded);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	for (i = 0; i < g_num_of_files; i++) {
		if (g_test_decode[i].decode_result == FALSE)
			continue;

		ret = image_util_encode_set_input_buffer(encoded,  g_test_decode[i].decoded);
		if (ret != IMAGE_UTIL_ERROR_NONE)
			continue;

		ret = image_util_encode_set_resolution(encoded,  g_test_decode[i].width, g_test_decode[i].height);
		if (ret != IMAGE_UTIL_ERROR_NONE)
			continue;

		ret = image_util_encode_set_gif_frame_delay_time(encoded,  50);	/* 500ms */
		if (ret != IMAGE_UTIL_ERROR_NONE)
			continue;
	}

	if (g_test_input.cmd == GIFTEST_ENCODE_MEM)
		ret = image_util_encode_set_output_buffer(encoded, &g_test_encode.encoded);
	else
		ret = image_util_encode_set_output_path(encoded, g_test_encode.out_path);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	ret = image_util_encode_run(encoded, &g_test_encode.encode_size);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	image_util_encode_destroy(encoded);

	if (g_test_input.cmd == GIFTEST_ENCODE_MEM) {
		if (_write_file(g_test_encode.out_path, g_test_encode.encoded, (size_t)g_test_encode.encode_size) == FALSE) {
			fprintf(stderr, "\tWrite the encoded result failed!\n");
			return FALSE;
		}
	}

	return TRUE;
}

gboolean test_encode_gif_frame_by_frame()
{
	int ret = 0;
	unsigned int i = 0;
	image_util_encode_h encoded = NULL;

	ret = image_util_encode_create(g_test_input.image_type, &encoded);
	if (ret != IMAGE_UTIL_ERROR_NONE)
		return FALSE;

	if (g_test_input.cmd == GIFTEST_ENCODE_FRAME_MEM)
		ret = image_util_encode_set_output_buffer(encoded, &g_test_encode.encoded);
	else
		ret = image_util_encode_set_output_path(encoded, g_test_encode.out_path);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	ret = image_util_encode_set_resolution(encoded, g_test_decode[0].width, g_test_decode[0].height);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	for (i = 0; i < g_num_of_files; i++) {
		if (g_test_decode[i].decode_result == FALSE)
			continue;

		image_util_frame_h frame = NULL;
		ret = image_util_frame_create(encoded, &frame);
		if (ret != IMAGE_UTIL_ERROR_NONE)
			continue;

		ret = image_util_frame_set_frame(frame, g_test_decode[i].decoded);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			image_util_frame_destroy(frame);
			continue;
		}

		ret = image_util_frame_set_resolution(frame, g_test_decode[i].width, g_test_decode[i].height);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			image_util_frame_destroy(frame);
			continue;
		}

		ret = image_util_frame_set_gif_delay(frame, 50);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			image_util_frame_destroy(frame);
			continue;
		}

		ret = image_util_encode_add_frame(encoded, frame);
		if (ret != IMAGE_UTIL_ERROR_NONE) {
			image_util_frame_destroy(frame);
			continue;
		}
		image_util_frame_destroy(frame);
	}

	ret = image_util_encode_save(encoded, &g_test_encode.encode_size);
	if (ret != IMAGE_UTIL_ERROR_NONE) {
		image_util_encode_destroy(encoded);
		return FALSE;
	}

	image_util_encode_destroy(encoded);

	if (g_test_input.cmd == GIFTEST_ENCODE_FRAME_MEM) {
		if (_write_file(g_test_encode.out_path, g_test_encode.encoded, (size_t)g_test_encode.encode_size) == FALSE) {
			fprintf(stderr, "\tWrite the encoded result failed!\n");
			return FALSE;
		}
	}

	return TRUE;
}

int main(int argc, char *argv[])
{
	if (argc < ARGC_MIN) {
		_print_help(argc, argv);
		return 0;
	}

	if (_init_datas() == FALSE) {
		fprintf(stderr, "\tInit failed!\n");
		_free_datas();
		return 0;
	}

	if (_parse_inputs(argc, argv) == FALSE) {
		fprintf(stderr, "\tInput was wrong!\n");
		_free_datas();
		return 0;
	}

	if (_read_test_files() == FALSE) {
		fprintf(stderr, "\tCan not read the test files!\n");
		_free_datas();
		return 0;
	}

	fprintf(stderr, "\tTests Start!\n");

	if (test_decode() == FALSE) {
		fprintf(stderr, "\tDecode Tests failed!\n");
		_free_datas();
		return 0;
	}

	fprintf(stderr, "\tDecoding is done!\n");

	if ((g_test_input.cmd == GIFTEST_ENCODE_FILE) || (g_test_input.cmd == GIFTEST_ENCODE_MEM)) {
		if (test_encode_gif() == FALSE) {
			fprintf(stderr, "\tEncode(gif) Tests failed!\n");
			_free_datas();
			return 0;
		}
	} else if ((g_test_input.cmd == GIFTEST_ENCODE_FRAME_FILE) || (g_test_input.cmd == GIFTEST_ENCODE_FRAME_MEM)) {
		if (test_encode_gif_frame_by_frame() == FALSE) {
			fprintf(stderr, "\tEncode(gif frame by frame) Tests failed!\n");
			_free_datas();
			return 0;
		}
	} else {
		if (test_encode() == FALSE) {
			fprintf(stderr, "\tEncode(default) Tests failed!\n");
			_free_datas();
			return 0;
		}
	}

	fprintf(stderr, "\tEncoding is done!\n");

	fprintf(stderr, "\tTests Finished!\n");

	return 0;
}
