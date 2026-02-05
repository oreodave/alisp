/* test_lisp_api.c: Testing of constructors/destructors of Lisp expressions
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include "./data.h"
#include "./test.h"

#include <alisp/lisp.h>

void int_test(void)
{
  i64 ints[] = {
      1, -1, (1 << 10) - 1, (-1) * ((1 << 10) - 1), INT_MIN, INT_MAX,
  };

  for (u64 i = 0; i < ARRSIZE(ints); ++i)
  {
    i64 in       = ints[i];
    lisp_t *lisp = make_int(in);
    i64 out      = as_int(lisp);

    TEST(in == out, "%ld == %ld", in, out);
  }

  TEST_PASSED();
}

void sym_test(void)
{
  sys_t system = {0};
  sys_init(&system);

  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    const char *in = words[i];
    lisp_t *lisp   = intern(&system, SV((char *)in, strlen(in)));
    char *out      = as_sym(lisp);
    TEST(in != out, "%p != %p", in, out);
    TEST(strlen(in) == strlen(out), "%zu == %zu", strlen(in), strlen(out));
    TEST(strncmp(in, out, strlen(in)) == 0, "`%s` == `%s`", in, out);
  }

  sys_free(&system);
  TEST_PASSED();
}

void cons_test(void)
{
  sys_t system = {0};
  sys_init(&system);

  // Let's make a list of words using `cons`
  lisp_t *lisp = NIL;
  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    const char *word = words[i];
    lisp_t *lword    = intern(&system, SV((char *)word, strlen(word)));
    lisp             = cons(&system, lword, lisp);
  }

  // Make sure we've essentially reversed the `words` array
  u64 i = ARRSIZE(words);
  for (lisp_t *iter = lisp; iter; iter = cdr(iter), --i)
  {
    const char *expected = words[i - 1];
    lisp_t *item         = car(iter);
    char *got            = as_sym(item);
    size_t size          = MIN(strlen(expected), strlen(got));

    TEST(strncmp(expected, got, size) == 0, "%s == %s", expected, got);
  }

  sys_free(&system);
  TEST_PASSED();
}

const test_suite_t LISP_API_SUITE = {
    .name = "Lisp API Tests",
    .tests =
        (test_fn[]){
            MAKE_TEST_FN(int_test),
            MAKE_TEST_FN(sym_test),
            MAKE_TEST_FN(cons_test),
        },
    .size = 4,
};

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General *
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
