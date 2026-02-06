/* test_lisp_api.c: Testing of constructors/destructors of Lisp expressions
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include "./data.h"
#include "./test.h"

#include <alisp/lisp.h>

void smi_test(void)
{
  TEST_START();
  // Standard old testing, checking both sides of the number line and our set
  // bounds.
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

  TEST_END();
}

void smi_oob_test(void)
{
  TEST_START();
  // These are integers that are completely out of the bounds of our standard
  // tagging system due to their size.  We need to use big integers for this.
  i64 ints[] = {
      INT_MIN - 1,
      INT_MAX + 1,
      INT64_MIN,
      INT64_MAX,
  };

  for (u64 i = 0; i < ARRSIZE(ints); ++i)
  {
    i64 in       = ints[i];
    lisp_t *lisp = make_int(in);
    i64 out      = as_int(lisp);

    TEST(in != out, "%ld != %ld", in, out);
  }

  TEST_END();
}

void sym_fresh_test(void)
{
  TEST_START();
  sys_t system = {0};
  sys_init(&system);

  // We expect every interned symbol to get a fresh allocation, but still be a
  // valid representation of the original symbol.
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
  TEST_END();
}

void sym_unique_test(void)
{
  TEST_START();
  sys_t system = {0};
  sys_init(&system);

  sv_t symbols[] = {
      SV("hello", 5),
      SV("goodbye", 7),
      SV("display", 7),
      SV("@xs'a_sh;d::a-h]", 16),
  };

  lisp_t *ptrs[ARRSIZE(symbols)];
  for (u64 i = 0; i < ARRSIZE(symbols); ++i)
  {
    ptrs[i] = intern(&system, symbols[i]);
    TEST(ptrs[i] != 0, "%p (derived from `" PR_SV "`) is not NIL",
         (void *)ptrs[i], SV_FMT(symbols[i]));
  }

  for (u64 i = 0; i < ARRSIZE(symbols); ++i)
  {
    lisp_t *newptr = intern(&system, symbols[i]);
    TEST(newptr == ptrs[i], "interning again (%p) gives us the same (%p)",
         (void *)newptr, (void *)ptrs[i]);
  }

  sys_free(&system);
  TEST_END();
}

void cons_test(void)
{
  TEST_START();
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

  /*
    As we've cons'd each word, we'd expect the order to be reversed.  This test
    will allow us to verify:
        1) words have actually been added to the linked list.
        2) words are in the order we expect.
    in one go.
   */
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
  TEST_END();
}

void sys_test(void)
{
  TEST_START();
  sys_t sys = {0};
  sys_init(&sys);
  u64 old_memory_size = sys.memory.size;

  // Creating integers doesn't affect memory size
  (void)make_int(2000);
  TEST(sys.memory.size == old_memory_size,
       "Making integers doesn't affect system memory size");

  // Creating symbols won't affect memory size, but does affect the symbol table
  (void)intern(&sys, SV("hello world!", 12));
  TEST(sys.memory.size == old_memory_size,
       "Interning doesn't affect system memory size");
  TEST(sys.symtable.count > 0, "Interning affects symbol table");

  // Creating conses do affect memory size
  (void)cons(&sys, make_int(1), make_int(2));
  TEST(sys.memory.size > 0, "Creating conses affects memory size");
  old_memory_size = sys.memory.size;

  (void)cons(&sys, intern(&sys, SV("test", 4)), NIL);
  TEST(sys.memory.size > old_memory_size,
       "Creating conses back to back affects memory size");
  old_memory_size = sys.memory.size;

  // Creating vectors does affect memory size
  (void)make_vec(&sys, 8);
  TEST(sys.memory.size > old_memory_size,
       "Creating vectors (size 8) affects memory size");
  old_memory_size = sys.memory.size;

  (void)make_vec(&sys, 1000);
  TEST(sys.memory.size > old_memory_size,
       "Creating vectors (size 1000) affects memory size");
  old_memory_size = sys.memory.size;

  sys_free(&sys);
  TEST(sys.memory.size == 0, "sys_free cleans up memory (shallow check)");
  TEST(sys.symtable.count == 0, "sys_free cleans up symtable (shallow check)");

  TEST_END();
}

MAKE_TEST_SUITE(LISP_API_SUITE, "LISP API Tests",

                MAKE_TEST_FN(smi_test), MAKE_TEST_FN(smi_oob_test),
                MAKE_TEST_FN(sym_fresh_test), MAKE_TEST_FN(sym_unique_test),
                MAKE_TEST_FN(cons_test), MAKE_TEST_FN(sys_test), );

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General *
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
