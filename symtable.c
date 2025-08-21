/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-19
 * Description: Symbol Table implementation
 */

#include "./alisp.h"

#include <malloc.h>
#include <string.h>

u64 djb2(sv_t string)
{
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

  // WIP: Deal with resizing this when table->count > table->size / 2
  u64 index = djb2(sv) & (table->capacity - 1);

  for (sv_t comp = VEC_GET(&table->entries, sv_t)[index]; comp.data; index += 1,
            index = index & (table->capacity - 1),
            comp  = VEC_GET(&table->entries, sv_t)[index])
    // Is it present in the table?
    if (sv.size == comp.size && strncmp(sv.data, comp.data, sv.size) == 0)
      break;

  // we couldn't find it in our linear search (worst case scenario)
  if (!VEC_GET(&table->entries, sv_t)[index].data)
  {
    sv_t newsv                            = sv_copy(sv);
    VEC_GET(&table->entries, sv_t)[index] = newsv;
    ++table->count;
  }

  return VEC_GET(&table->entries, sv_t)[index].data;
}

void sym_table_cleanup(sym_table_t *table)
{
  // kill the data
  sv_t current = {0};
  for (u64 i = 0; i < table->capacity; ++i)
  {
    current = VEC_GET(&table->entries, sv_t)[i];
    if (current.data)
      free(current.data);
  }
  // kill the container
  vec_free(&table->entries);
  memset(table, 0, sizeof(*table));
}
