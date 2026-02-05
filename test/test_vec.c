/* test_vec.c: Vector tests
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include "./data.h"
#include "./test.h"

void vec_test_concat(void)
{
  sys_t system = {0};
  sys_init(&system);

  // Generating a vector word by word
  lisp_t *lvec = make_vec(&system, 0);
  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    const char *word = words[i];
    vec_append(as_vec(lvec), word, strlen(word));
    if (i != ARRSIZE(words) - 1)
      vec_append(as_vec(lvec), " ", 1);
  }
  vec_append(as_vec(lvec), "\0", 1);

  vec_t *vec = as_vec(lvec);

  TEST(vec->size == ARRSIZE(words_text), "%lu == %lu", vec->size,
       ARRSIZE(words_text));
  TEST(strncmp((char *)vec_data(vec), words_text, vec->size) == 0,
       "%p@%lu == %p@%lu", (char *)vec_data(vec), vec->size, words_text,
       strlen(words_text));

  sys_free(&system);
  TEST_PASSED();
}

void vec_test_substr(void)
{
  sys_t system = {0};
  sys_init(&system);
  // Generating substrings
  struct Test
  {
    u64 start, size;
  } tests[] = {
      {0, 16},
      {0, 32},
      {32, 64},
      {0, ARRSIZE(text)},
  };

  for (u64 i = 0; i < ARRSIZE(tests); ++i)
  {
    struct Test test  = tests[i];
    const sv_t substr = SV((char *)text + test.start, test.size);
    const u64 size    = test.size / 2;

    lisp_t *lvec = make_vec(&system, size);
    vec_append(as_vec(lvec), text + test.start, test.size);
    TEST(as_vec(lvec)->size > size, "%lu > %lu", as_vec(lvec)->size, size);
    TEST(strncmp((char *)vec_data(as_vec(lvec)), substr.data, substr.size) == 0,
         "%p@%lu == %p@%lu", (char *)vec_data(as_vec(lvec)), as_vec(lvec)->size,
         substr.data, substr.size);
  }

  sys_free(&system);
  TEST_PASSED();
}

MAKE_TEST_SUITE(VEC_SUITE, "Vector Tests",

                MAKE_TEST_FN(vec_test_concat), MAKE_TEST_FN(vec_test_substr), );

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General *
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
