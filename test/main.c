/* main.c: Main boot file for unit tests
 * Created: 2025-08-21
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <stdio.h>
#include <string.h>

#include "./data.h"
#include "./test.h"

#include "./test_lisp_api.c"
#include "./test_vec.c"

test_suite_t SUITES[] = {
    LISP_API_SUITE,
    VEC_SUITE,
};

int main(void)
{
  for (u64 i = 0; i < ARRSIZE(SUITES); ++i)
  {
    test_suite_t suite = SUITES[i];
    printf("Suite [%s]\n", suite.name);
    for (u64 j = 0; j < suite.size; ++j)
    {
      printf("[%s]: Running...\n", suite.tests[j].name);
      suite.tests[j].fn();
    }
  }
  return 0;
}

/* Copyright (C) 2025, 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 */
