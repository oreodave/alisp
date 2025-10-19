/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-20
 * Description: Lisp constructors/destructors
 */

#include <malloc.h>

#include "./alisp.h"

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
