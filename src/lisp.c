/* lisp.c: Lisp constructors/destructors
 * Created: 2025-08-20
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <string.h>

#include <alisp/lisp.h>

lisp_t *tag_int(i64 i)
{
  return TAG((u64)i, INT);
}

lisp_t *tag_sym(const char *str)
{
  return TAG((u64)str, SYM);
}

lisp_t *tag_vec(const vec_t *vec)
{
  return TAG((u64)vec, VEC);
}

lisp_t *tag_cons(const cons_t *cons)
{
  return TAG((u64)cons, CONS);
}

tag_t get_tag(const lisp_t *lisp)
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

vec_t *as_vec(lisp_t *obj)
{
  assert(IS_TAG(obj, VEC));
  return (vec_t *)UNTAG(obj, VEC);
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
  case TAG_INT:
#if VERBOSE_LOGS
    fprintf(fp, "INT[");
#endif
    fprintf(fp, "%ld", as_int(lisp));
#if VERBOSE_LOGS
    fprintf(fp, "]");
#endif
    break;
  case TAG_SYM:
#if VERBOSE_LOGS
    fprintf(fp, "SYM[");
#endif
    fprintf(fp, "%s", as_sym(lisp));
#if VERBOSE_LOGS
    fprintf(fp, "]");
#endif
    break;
  case TAG_CONS:
  {
#if VERBOSE_LOGS
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
#if VERBOSE_LOGS
    fprintf(fp, "]");
#else
    fprintf(fp, ")");
#endif
    break;
  }
  case TAG_VEC:
  {
#if VERBOSE_LOGS
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

#if VERBOSE_LOGS
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

/* Copyright (C) 2025, 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
