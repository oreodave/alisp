/* test_stream.c: Stream tests
 * Created: 2026-02-05
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include "./data.h"
#include "./test.h"

void stream_test_string(void)
{
  TODO("Not implemented");
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
