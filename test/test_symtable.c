/* test_symtable.c: Symbol table tests
 * Created: 2026-02-05
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include "./data.h"
#include "./test.h"

void symtable_test(void)
{
  TEST_START();
  sym_table_t table = {0};
  sym_table_init(&table);
  for (u64 i = 0; i < ARRSIZE(words); ++i)
    sym_table_find(&table, SV((char *)words[i], strlen(words[i])));

  TEST(table.count == ARRSIZE(unique_words), "%lu == %lu", table.count,
       ARRSIZE(unique_words));

  sym_table_free(&table);
  TEST_END();
}

MAKE_TEST_SUITE(SYMTABLE_SUITE, "Symbol Table Tests",

                MAKE_TEST_FN(symtable_test), );

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
