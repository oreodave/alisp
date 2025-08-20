/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-19
 * Description: Pointer tagging
 */

#include <assert.h>
#include <stdlib.h>

#include "./base.h"

lisp_t *tag_int(i64 i)
{
  return TAG((u64)i, INT);
}

lisp_t *tag_sym(char *str)
{
  return TAG((u64)str, SYM);
}

lisp_t *tag_vec(lvec_t *lvec)
{
  return TAG((u64)lvec, VEC);
}

lisp_t *tag_cons(cons_t *cons)
{
  return TAG((u64)cons, CONS);
}

tag_t get_tag(lisp_t *lisp)
{
  static_assert(NUM_TAGS == 5);
  if (!lisp)
    return TAG_NIL;
  else if (IS_TAG(lisp, INT))
    return TAG_INT;

  return (u64)lisp & 0xFF;
}

i64 as_int(lisp_t *obj)
{
  assert(IS_TAG(obj, INT));
  u64 p_obj = (u64)obj;
  return UNTAG(p_obj, INT) |               // Delete the tag
         (NTH_BYTE(p_obj, 7) & 0x80) << 56 // duplicate the MSB (preserve sign)
      ;
}

char *as_sym(lisp_t *obj)
{
  assert(IS_TAG(obj, SYM));
  return (char *)UNTAG(obj, SYM);
}

cons_t *as_cons(lisp_t *obj)
{
  assert(IS_TAG(obj, CONS));
  return (cons_t *)UNTAG(obj, CONS);
}

void *as_vec(lisp_t *obj)
{
  assert(IS_TAG(obj, VEC));
  lvec_t *vec = (lvec_t *)UNTAG(obj, VEC);
  if (vec)
    return vec->data;
  else
    return NULL;
}
