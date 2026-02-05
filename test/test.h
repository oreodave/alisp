/* test.h: Unit test API
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef TEST_H
#define TEST_H

#include <alisp/alisp.h>

#ifndef TEST_VERBOSE
#define TEST_VERBOSE 0
#endif

#define TEST_START() printf("\t[%s]: Running...\n", __func__)
#define TEST_END()   printf("\t[%s]: Passed\n", __func__)
#define TEST_INFO(...)   \
  do                     \
  {                      \
    printf("\tINFO: ");  \
    printf(__VA_ARGS__); \
  } while (0);

#if TEST_VERBOSE
#define TEST(COND, ...)                  \
  do                                     \
  {                                      \
    bool cond = (COND);                  \
    if (!cond)                           \
    {                                    \
      printf("\t\tFAIL: ");              \
    }                                    \
    else                                 \
    {                                    \
      printf("\t\tPASS: ");              \
    }                                    \
    printf(__VA_ARGS__);                 \
    printf("\n\t\t      [%s]\n", #COND); \
    if (!cond)                           \
    {                                    \
      assert(0);                         \
    }                                    \
  } while (0)
#else
#define TEST(COND, ...) \
  do                    \
  {                     \
    if (!(COND))        \
    {                   \
      assert(0);        \
    }                   \
  } while (0)
#endif

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

#define MAKE_TEST_SUITE(NAME, DESC, ...)        \
  const test_fn NAME##_TESTS[] = {__VA_ARGS__}; \
  const test_suite_t NAME      = {              \
           .name  = DESC,                       \
           .tests = NAME##_TESTS,               \
           .size  = ARRSIZE(NAME##_TESTS),      \
  }

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
