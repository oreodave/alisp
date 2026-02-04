/* test.h: Unit test API
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef TEST_H
#define TEST_H

#include <alisp/alisp.h>

#define TEST_PASSED() printf("[%s]: Passed\n", __func__)
#define TEST(COND, ...)      \
  do                         \
  {                          \
    bool cond = (COND);      \
    if (!cond)               \
    {                        \
      printf("\tFAIL: ");    \
    }                        \
    else                     \
    {                        \
      printf("\tPASS: ");    \
    }                        \
    printf("%s => ", #COND); \
    printf(__VA_ARGS__);     \
    printf("\n");            \
    if (!cond)               \
    {                        \
      assert(0);             \
    }                        \
  } while (0)

typedef struct TestFn
{
  const char *name;
  void (*fn)(void);
} test_fn;

#define MAKE_TEST_FN(NAME) {.name = #NAME, .fn = NAME}

typedef struct
{
  const char *name;
  const test_fn *tests;
  const u64 size;
} test_suite_t;

#define MAKE_TEST_SUITE(NAME) \
  {.name = #NAME, .tests = NAME, .size = ARRSIZE(NAME)}

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 */
