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

#include "./base.h"

lisp_t *tag_int(i64 i)
{
  return TAG((u64)i, INT);
}

lisp_t *tag_sym(char *str)
{
  return TAG((u64)str, SYM);
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
