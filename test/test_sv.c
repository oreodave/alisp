/* test_sv.c: String View tests
 * Created: 2026-02-05
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <assert.h>
#include <malloc.h>

#include "./data.h"
#include "./test.h"

void sv_copy_test(void)
{
  static_assert(ARRSIZE(unique_words) > 3, "Expected at least 3 unique words");
  for (u64 i = 0; i < 3; ++i)
  {
    sv_t word = SV((char *)unique_words[i], strlen(unique_words[i]));
    sv_t copy = sv_copy(word);
    TEST(word.data != copy.data, "%p != %p", word.data, copy.data);
    TEST(word.size == copy.size, "%lu == %lu", word.size, copy.size);
    TEST(strncmp(word.data, copy.data, copy.size) == 0, "`%s` == `%s`",
         word.data, copy.data);

    // Obviously we can't just have this lying around.
    free(copy.data);
  }
}

const test_fn TESTS_SV[] = {
    MAKE_TEST_FN(sv_copy_test),
};

const test_suite_t SV_SUITE = {
    .name  = "String View Tests",
    .tests = TESTS_SV,
    .size  = ARRSIZE(TESTS_SV),
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
