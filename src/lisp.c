/* lisp.c: Lisp constructors/destructors
 * Created: 2025-08-20
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <string.h>

#include <alisp/lisp.h>
#include <alisp/tag.h>

void sys_init(sys_t *sys)
{
  memset(sys, 0, sizeof(*sys));
}

void sys_register(sys_t *sys, lisp_t *ptr)
{
  // Simply append it to the list of currently active conses
  vec_append(&sys->memory, &ptr, sizeof(&ptr));
}

void sys_free(sys_t *sys)
{
  static_assert(NUM_TAGS == 5);

  sym_table_free(&sys->symtable);
  if (sys->memory.size == 0)
    return;

  // Iterate through each cell of memory currently allocated and free them
  for (size_t i = 0; i < VEC_SIZE(&sys->memory, lisp_t **); ++i)
  {
    lisp_t *allocated = VEC_GET(&sys->memory, i, lisp_t *);
    lisp_free(allocated);
  }

  // Free the container
  vec_free(&sys->memory);

  // Ensure no one treats this as active in any sense
  memset(sys, 0, sizeof(*sys));
}

lisp_t *make_int(i64 i)
{
  return tag_int(i);
}

lisp_t *cons(sys_t *sys, lisp_t *car, lisp_t *cdr)
{
  cons_t *cons = calloc(1, sizeof(*cons));
  cons->car    = car;
  cons->cdr    = cdr;

  lisp_t *lcons = tag_cons(cons);
  sys_register(sys, lcons);
  return lcons;
}

lisp_t *make_vec(sys_t *sys, u64 capacity)
{
  vec_t *vec = calloc(1, sizeof(*vec));
  vec_init(vec, capacity);
  lisp_t *ptr = tag_vec(vec);
  sys_register(sys, ptr);
  return ptr;
}

lisp_t *intern(sys_t *sys, sv_t sv)
{
  char *str = sym_table_find(&sys->symtable, sv);
  return tag_sym(str);
}

lisp_t *car(lisp_t *lsp)
{
  if (!IS_TAG(lsp, CONS))
    return NIL;
  else
    return CAR(lsp);
}

lisp_t *cdr(lisp_t *lsp)
{
  if (!IS_TAG(lsp, CONS))
    return NIL;
  else
    return CDR(lsp);
}

void lisp_free(lisp_t *item)
{
  switch (get_tag(item))
  {
  case TAG_CONS:
    // Delete the cons
    free(as_cons(item));
    break;
  case TAG_VEC:
  {
    vec_t *vec = as_vec(item);
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

/* Copyright (C) 2025, 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
