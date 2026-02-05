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

void stream_test_string(void)
{
  TEST_INIT();
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

  TEST_PASSED();
}

void stream_test_file(void)
{
  TEST_INIT();

  // Create a mock temporary file
  const char *filename = "ahsudhaiusdhasd.txt";
  {
    FILE *fp = fopen(filename, "wb");
    assert(fp);
    fwrite(words_text, ARRSIZE(words_text), 1, fp);
    fclose(fp);
  }

  // Test that initialising works correctly
  {
    FILE *fp = fopen(filename, "rb");
    assert(fp); // Isn't a test
    stream_t stream = {0};
    {
      stream_err_t err = stream_init_file(&stream, filename, fp);
      TEST(err == STREAM_ERR_OK, "Expected initialisating to be okay: %s",
           stream_err_to_cstr(err));
    }
    TEST(stream_size(&stream) == 0, "Stream doesn't read on init: size = %lu",
         stream_size(&stream));
    TEST(!stream_eoc(&stream), "Stream should not be at the EoC from init.");
    fclose(fp);
  }

  // Delete mock file from file system
  assert(remove(filename) == 0);

  // try to initialise the stream again but against a nonexistent file - we're
  // expecting an error.
  {
    FILE *fp        = fopen(filename, "rb");
    stream_t stream = {0};
    {
      stream_err_t err = stream_init_file(&stream, filename, fp);
      TEST(err != STREAM_ERR_OK, "Expected initialisating to not be okay: %s",
           stream_err_to_cstr(err));
    }
  }

  TEST_PASSED();
}

void stream_test_peek_next(void)
{
  TEST_INIT();
  TODO("Not implemented");
}

void stream_test_seek(void)
{
  TEST_INIT();
  TODO("Not implemented");
}

void stream_test_substr(void)
{
  TEST_INIT();
  TODO("Not implemented");
}

void stream_test_till(void)
{
  TEST_INIT();
  TODO("Not implemented");
}

void stream_test_while(void)
{
  TEST_INIT();
  TODO("Not implemented");
}

void stream_test_line_col(void)
{
  TEST_INIT();
  TODO("Not implemented");
}

MAKE_TEST_SUITE(STREAM_SUITE, "Stream Tests",

                MAKE_TEST_FN(stream_test_string),
                MAKE_TEST_FN(stream_test_file));

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
