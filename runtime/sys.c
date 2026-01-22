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

#include "../alisp.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

void sys_init(sys_t *sys)
{
  memset(sys, 0, sizeof(*sys));
  vec_init(&sys->conses, 0);
}

void sys_register(sys_t *sys, lisp_t *ptr)
{
  // Simply append it to the list of currently active conses
  vec_append(&sys->conses, &ptr, sizeof(&ptr));
}

void sys_cleanup(sys_t *sys)
{
  static_assert(NUM_TAGS == 5);

  sym_table_cleanup(&sys->symtable);
  if (sys->conses.size == 0)
    return;

  // Iterate through each cons currently allocated and free them
  for (size_t i = 0; i < VEC_SIZE(&sys->conses, lisp_t **); ++i)
  {
    lisp_t *allocated = VEC_GET(&sys->conses, i, lisp_t *);
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
  }

  // Free the container
  vec_free(&sys->conses);

  // Ensure no one treats this as active in any sense
  memset(sys, 0, sizeof(*sys));
}
