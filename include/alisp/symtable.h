/* symtable.h: Symbol tables
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <alisp/sv.h>
#include <alisp/vec.h>

typedef struct
{
  u64 count;    // How many strings?
  u64 capacity; // How many entry buckets?
  vec_t entries;
} sym_table_t;

#define SYM_TABLE_INIT_SIZE (1 << 10)

void sym_table_init(sym_table_t *);
char *sym_table_find(sym_table_t *, sv_t);
void sym_table_free(sym_table_t *);

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General *
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
