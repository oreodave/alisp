/* test_stream.c: Stream tests
 * Created: 2026-02-05
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>

#include "./data.h"
#include "./test.h"

#include <alisp/stream.h>

void stream_test_string(void)
{
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
  TODO("Not implemented");
}

void stream_test_peek_next(void)
{
  TODO("Not implemented");
}

void stream_test_seek(void)
{
  TODO("Not implemented");
}

void stream_test_substr(void)
{
  TODO("Not implemented");
}

void stream_test_till(void)
{
  TODO("Not implemented");
}

void stream_test_while(void)
{
  TODO("Not implemented");
}

void stream_test_line_col(void)
{
  TODO("Not implemented");
}

const test_suite_t STREAM_SUITE = {
    .name = "Stream Tests",
    .tests =
        (test_fn[]){
            MAKE_TEST_FN(stream_test_string),
            MAKE_TEST_FN(stream_test_file),
            MAKE_TEST_FN(stream_test_peek_next),
            MAKE_TEST_FN(stream_test_seek),
            MAKE_TEST_FN(stream_test_substr),
            MAKE_TEST_FN(stream_test_till),
            MAKE_TEST_FN(stream_test_while),
            MAKE_TEST_FN(stream_test_line_col),
        },
    .size = 8,
};

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
