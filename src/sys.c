/* sys.c: System implementation
 * Created: 2026-02-12
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <stdlib.h>
#include <string.h>

#include <alisp/sys.h>

void sys_init(sys_t *sys)
{
  memset(sys, 0, sizeof(*sys));
}

lisp_t *sys_alloc(sys_t *sys, tag_t type)
{
  switch (type)
  {
  case TAG_CONS:
  case TAG_VEC:
    return arena_make(&sys->memory, type);
  // Shouldn't be registered
  case TAG_NIL:
  case TAG_INT:
  case TAG_SYM:
  default:
    FAIL("Unreachable");
  }
  return NIL;
}

u64 sys_cost(sys_t *sys)
{
  return arena_cost(&sys->memory) + sym_table_cost(&sys->symtable);
}

void sys_free(sys_t *sys)
{
  sym_table_free(&sys->symtable);
  arena_free(&sys->memory);
  // Ensure no one treats this as active in any sense
  memset(sys, 0, sizeof(*sys));
}

lisp_t *make_int(i64 i)
{
  return tag_int(i);
}

lisp_t *cons(sys_t *sys, lisp_t *car, lisp_t *cdr)
{
  lisp_t *cons = sys_alloc(sys, TAG_CONS);
  CAR(cons)    = car;
  CDR(cons)    = cdr;
  return cons;
}

lisp_t *make_vec(sys_t *sys, u64 capacity)
{
  lisp_t *vec = sys_alloc(sys, TAG_VEC);
  vec_init(as_vec(vec), capacity);
  return vec;
}

lisp_t *intern(sys_t *sys, sv_t sv)
{
  const char *str = sym_table_find(&sys->symtable, sv);
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

void lisp_free_rec(lisp_t *item)
{
  switch (get_tag(item))
  {
  case TAG_CONS:
  {
    lisp_free_rec(car(item));
    lisp_free_rec(cdr(item));
    free(as_cons(item));
    break;
  }
  case TAG_VEC:
  {
    vec_t *vec = as_vec(item);
    for (size_t i = 0; i < VEC_SIZE(vec, lisp_t **); ++i)
    {
      lisp_t *allocated = VEC_GET(vec, i, lisp_t *);
      lisp_free_rec(allocated);
    }
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

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
