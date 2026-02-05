/* test_stream.c: Stream tests
 * Created: 2026-02-05
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <stdio.h>

#include "./data.h"
#include "./test.h"

#include <alisp/stream.h>

const char *valid_filename = "build/stream_test_artifact";
FILE *valid_fp             = NULL;
FILE *invalid_fp           = NULL;

void stream_test_prologue(void)
{
  TEST_INFO("Creating file named `%s`\n", valid_filename);
  valid_fp = fopen(valid_filename, "wb");
  // This should do a few things for us
  // 1) Create a file, or clear the contents of it if it exists already.
  // 2) Write some content to it.
  assert(valid_fp);
  fwrite(words_text, ARRSIZE(words_text), 1, valid_fp);
  fclose(valid_fp);
  valid_fp = fopen(valid_filename, "rb");
  assert(valid_fp);

  invalid_fp = NULL;
}

void stream_test_epilogue(void)
{
  TEST_INFO("Freeing resources and deleting file `%s`\n", valid_filename);
  assert(valid_fp);
  fclose(valid_fp);
  remove(valid_filename);
}

void stream_test_string(void)
{
  TEST_START();
  sv_t test_strings[] = {
      SV("hello, world!", 13),
      SV("another string", 14),
      SV((char *)text, ARRSIZE(text) / 2),
  };

  for (u64 i = 0; i < ARRSIZE(test_strings); ++i)
  {
    sv_t copy = sv_copy(test_strings[i]);

    stream_t stream  = {0};
    stream_err_t err = stream_init_string(&stream, NULL, test_strings[i]);
    TEST(err == STREAM_ERR_OK, "Stream initialising did not fail: %s",
         stream_err_to_cstr(err));
    TEST(stream_size(&stream) == test_strings[i].size,
         "Stream size is always string size (%lu == %lu)", stream_size(&stream),
         test_strings[i].size);
    TEST(!stream_eoc(&stream), "Not end of content already");

    stream_stop(&stream);
    TEST(strncmp(copy.data, test_strings[i].data, copy.size) == 0,
         "Freeing a stream does not free the underlying memory it was derived "
         "from");

    free(copy.data);
  }

  stream_t stream  = {0};
  stream_err_t err = stream_init_string(&stream, NULL, SV(NULL, 0));
  TEST(err == STREAM_ERR_OK, "NULL stream initialising did not fail: %s",
       stream_err_to_cstr(err));
  TEST(stream_size(&stream) == 0, "NULL stream size is 0");
  TEST(stream_eoc(&stream), "NULL stream is always at end of content");
  stream_stop(&stream);

  TEST_END();
}

void stream_test_file(void)
{
  TEST_START();

  // Test that initialising works correctly
  {
    stream_t stream = {0};
    {
      stream_err_t err = stream_init_file(&stream, valid_filename, valid_fp);
      TEST(err == STREAM_ERR_OK, "Expected initialisating to be okay: %s",
           stream_err_to_cstr(err));
    }
    TEST(stream_size(&stream) == 0, "Stream doesn't read on init: size = %lu",
         stream_size(&stream));
    TEST(!stream_eoc(&stream), "Stream should not be at the EoC from init.");
  }

  // try to initialise the stream again but against a nonexistent file - we're
  // expecting an error.
  {
    stream_t stream = {0};
    {
      stream_err_t err = stream_init_file(&stream, NULL, invalid_fp);
      TEST(err != STREAM_ERR_OK, "Expected initialisating to not be okay: %s",
           stream_err_to_cstr(err));
    }
  }

  TEST_END();
}

void stream_test_peek_next(void)
{
  TEST_START();

  // Valid streams
  {
    stream_t stream = {0};
    stream_init_file(&stream, valid_filename, valid_fp);

    u64 old_position = stream.position;
    char c1          = stream_peek(&stream);
    TEST(c1 != '\0', "Peek should provide a normal character (%c)", c1);
    TEST(stream.position == old_position,
         "Peek should not shift the position (%lu -> %lu)", old_position,
         stream.position);

    char c2 = stream_next(&stream);
    TEST(c2 != '\0', "Next should provide a normal character (%c)", c2);
    TEST(stream.position > old_position,
         "Next should shift the position (%lu -> %lu)", old_position,
         stream.position);
    TEST(c2 != c1,
         "Next should yield a different character (%c) to the previous peek "
         "(%c)",
         c2, c1);

    char c3 = stream_peek(&stream);
    TEST(c3 == c2,
         "Peeking should yield the same character (%c) as the previous next "
         "(%c)",
         c3, c2);

    stream_stop(&stream);
  }

  // Invalid streams
  {
    stream_t stream = {0};
    stream_init_file(&stream, NULL, invalid_fp);
    char c = stream_peek(&stream);
    TEST(c == '\0', "Invalid streams should have an invalid peek (%c)", c);

    u64 old_position = stream.position;
    c                = stream_next(&stream);
    TEST(c == '\0', "Invalid streams should have an invalid next (%c)", c);
    TEST(old_position == stream.position,
         "Next on an invalid stream should not affect position (%lu -> %lu)",
         old_position, stream.position);
  }
  TEST_END();
}

void stream_test_seek(void)
{
  TEST_START();
  TODO("Not implemented");
}

void stream_test_substr(void)
{
  TEST_START();
  TODO("Not implemented");
}

void stream_test_till(void)
{
  TEST_START();
  TODO("Not implemented");
}

void stream_test_while(void)
{
  TEST_START();
  TODO("Not implemented");
}

void stream_test_line_col(void)
{
  TEST_START();
  TODO("Not implemented");
}

MAKE_TEST_SUITE(STREAM_SUITE, "Stream Tests",

                MAKE_TEST_FN(stream_test_prologue),
                MAKE_TEST_FN(stream_test_string),
                MAKE_TEST_FN(stream_test_file),
                MAKE_TEST_FN(stream_test_peek_next),
                MAKE_TEST_FN(stream_test_epilogue), );

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
