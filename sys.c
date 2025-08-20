/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-20
 * Description: System management
 */

#include "./alisp.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

void sys_init(sys_t *sys)
{
  sys->memory = NIL;
  sym_table_init(&sys->symtable);
}

void sys_register(sys_t *sys, lisp_t *ptr)
{
  // Generate an unmanaged cons
  cons_t *cons = calloc(1, sizeof(*cons));
  cons->car    = ptr;
  cons->cdr    = sys->memory;
  sys->memory  = tag_cons(cons);
}

void sys_cleanup(sys_t *sys)
{
  static_assert(NUM_TAGS == 5);

  sym_table_cleanup(&sys->symtable);

  if (!sys->memory)
    return;

  // Iterate through each element of memory
  for (lisp_t *cell = sys->memory, *next = CDR(cell); cell;
       cell = next, next = CDR(cell))
  {
    // Only reason allocated exists is because we had to allocate memory on the
    // heap for it.  It's therefore
    lisp_t *allocated = CAR(cell);
    switch (get_tag(allocated))
    {
    case TAG_CONS:
      // Delete the cons
      free(as_cons(allocated));
      break;
    case TAG_VEC:
    {
      vec_t *vec = as_vec(allocated);
      vec_free(vec);
      free(vec);
      break;
    }
    case TAG_NIL:
    case TAG_INT:
    case TAG_SYM:
    case NUM_TAGS:
      // shouldn't be dealt with (either constant or dealt with elsewhere)
      break;
    }

    // Then free the current cell
    free(as_cons(cell));
  }
  memset(sys, 0, sizeof(*sys));
}
