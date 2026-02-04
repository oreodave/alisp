/* symtable.c: Symbol Table implementation
 * Created: 2025-08-19
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <alisp/symtable.h>

#include <malloc.h>
#include <string.h>

#define ENTRY_GET(TABLE, INDEX) (VEC_GET(&(TABLE)->entries, (INDEX), sv_t))

u64 djb2(sv_t string)
{
  // magic number (see
  // https://stackoverflow.com/questions/10696223/reason-for-the-number-5381-in-the-djb-hash-function)
  u64 hash = 5381;
  for (u64 i = 0; i < string.size; ++i)
    hash = string.data[i] + (hash + (hash << 5));
  return hash;
}

void sym_table_init(sym_table_t *table)
{
  table->capacity = MAX(table->capacity, SYM_TABLE_INIT_SIZE);
  table->count    = 0;
  vec_init(&table->entries, table->capacity * sizeof(sv_t));
}

char *sym_table_find(sym_table_t *table, sv_t sv)
{
  // Initialise the table if it's not done already
  if (table->entries.capacity == 0)
    sym_table_init(table);

  // TODO: Consider resizing table when count >= capacity / 2
  u64 index = djb2(sv) & (table->capacity - 1);

  // FIXME: Since we don't implement resizing currently, this will infinite loop
  // when the table capacity SYM_TABLE_INIT_SIZE=(1 << 10)=1024 is filled.
  for (sv_t comp = ENTRY_GET(table, index); comp.data;
       index     = (index + 1) & (table->capacity - 1),
            comp = ENTRY_GET(table, index))
    // If present in the table already, stop.
    if (sv.size == comp.size && strncmp(sv.data, comp.data, sv.size) == 0)
      return comp.data;

  // Worst case: not present in the table already.  Since index is certainly
  // free (based on loop condition before) we can fill it.
  ENTRY_GET(table, index) = sv_copy(sv);
  ++table->count;
  return ENTRY_GET(table, index).data;
}

void sym_table_cleanup(sym_table_t *table)
{
  // Iterate through the strings and free each of them.
  sv_t current = {0};
  for (u64 i = 0; i < table->capacity; ++i)
  {
    current = ENTRY_GET(table, i);
    if (current.data)
      free(current.data);
  }
  // Free the underlying container
  vec_free(&table->entries);
  memset(table, 0, sizeof(*table));
}

/* Copyright (C) 2025, 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
