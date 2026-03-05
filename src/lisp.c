/* lisp.c: Lisp constructors/destructors
 * Created: 2025-08-20
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <string.h>

#include <alisp/lisp.h>

lisp_t *tag_smi(i64 i)
{
  return TAG(i, SMI);
}

lisp_t *tag_sym(const char *str)
{
  return TAG(str, SYM);
}

lisp_t *tag_vec(const vec_t *vec)
{
  return TAG(vec, VEC);
}

lisp_t *tag_cons(const cons_t *cons)
{
  return TAG(cons, CONS);
}

lisp_t *tag_generic(void *ptr, tag_t type)
{
  switch (type)
  {
  case TAG_NIL:
    return TAG(ptr, NIL);
  case TAG_SMI:
    return tag_smi((i64)ptr);
  case TAG_SYM:
    return tag_sym(ptr);
  case TAG_CONS:
    return tag_cons(ptr);
  case TAG_VEC:
    return tag_vec(ptr);
  default:
    FAIL("Unreachable");
    return NIL;
  }
}

tag_t get_tag(const lisp_t *lisp)
{
  static_assert(NUM_TAGS == 5);
  return GET_TAG(lisp);
}

i64 as_smi(lisp_t *obj)
{
  assert(IS_TAG(obj, SMI));
  u64 raw_obj = UNTAG(obj);
  u64 msb     = (NTH_BYTE(raw_obj, 6) & 0x80) >> 7;
  msb         = ((1LU << 8) - msb) << 56;
  return (i64)(raw_obj | msb);
}

char *as_sym(lisp_t *obj)
{
  assert(IS_TAG(obj, SYM));
  return (char *)UNTAG(obj);
}

cons_t *as_cons(lisp_t *obj)
{
  assert(IS_TAG(obj, CONS));
  return (cons_t *)UNTAG(obj);
}

vec_t *as_vec(lisp_t *obj)
{
  assert(IS_TAG(obj, VEC));
  return (vec_t *)UNTAG(obj);
}

void lisp_print(FILE *fp, lisp_t *lisp)
{
  if (!fp)
    return;
  switch (get_tag(lisp))
  {
  case TAG_NIL:
    fprintf(fp, "NIL");
    break;
  case TAG_SMI:
#if VERBOSE_LOGS == 2
    fprintf(fp, "INT[");
#endif
    fprintf(fp, "%ld", as_smi(lisp));
#if VERBOSE_LOGS == 2
    fprintf(fp, "]");
#endif
    break;
  case TAG_SYM:
#if VERBOSE_LOGS == 2
    fprintf(fp, "SYM[");
#endif
    fprintf(fp, "%s", as_sym(lisp));
#if VERBOSE_LOGS == 2
    fprintf(fp, "]");
#endif
    break;
  case TAG_CONS:
  {
#if VERBOSE_LOGS == 2
    fprintf(fp, "LIST[");
#else
    fprintf(fp, "(");
#endif
    for (; lisp; lisp = CDR(lisp))
    {
      if (IS_TAG(lisp, CONS))
      {
        lisp_t *car = CAR(lisp);
        lisp_t *cdr = CDR(lisp);

        lisp_print(fp, car);
        if (cdr && !IS_TAG(cdr, CONS))
        {
          fprintf(fp, " . ");
        }
        else if (cdr)
        {
          fprintf(fp, " ");
        }
      }
      else
      {
        lisp_print(fp, lisp);
        break;
      }
    }
#if VERBOSE_LOGS == 2
    fprintf(fp, "]");
#else
    fprintf(fp, ")");
#endif
    break;
  }
  case TAG_VEC:
  {
#if VERBOSE_LOGS == 2
    fprintf(fp, "VEC[");
#else
    fprintf(fp, "[");
#endif

    vec_t *vec = as_vec(lisp);
    for (u64 i = 1; i <= VEC_SIZE(vec, lisp_t *); ++i)
    {
      lisp_t *item = VEC_GET(vec, i - 1, lisp_t *);
      lisp_print(fp, item);
      if (i != VEC_SIZE(vec, lisp_t *))
      {
        fprintf(fp, " ");
      }
    }

#if VERBOSE_LOGS == 2
    fprintf(fp, "]");
#else
    fprintf(fp, "]");
#endif
    break;
  }
  case NUM_TAGS:
  default:
    FAIL("Unreachable");
    break;
  }
}

u64 tag_sizeof(tag_t tag)
{
  switch (tag)
  {
  case TAG_NIL:
    return 0;
  case TAG_SMI:
  case TAG_SYM:
    return sizeof(lisp_t *);
  case TAG_CONS:
    return sizeof(cons_t);
  case TAG_VEC:
    return sizeof(vec_t);
  case NUM_TAGS:
  default:
    FAIL("Unreachable");
    return 0;
  }
}

u64 lisp_sizeof(lisp_t *lisp)
{
  return tag_sizeof(get_tag(lisp));
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
