/* lisp.c: Lisp constructors/destructors
 * Created: 2025-08-20
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <string.h>

#include <alisp/lisp.h>

void sys_init(sys_t *sys)
{
  memset(sys, 0, sizeof(*sys));
}

lisp_t *sys_alloc(sys_t *sys, tag_t type)
{
  switch (type)
  {
  case TAG_CONS:
  {
    cons_t *cons = calloc(1, sizeof(*cons));
    lisp_t *lisp = tag_cons(cons);
    vec_append(&sys->conses, &lisp, sizeof(&lisp));
    return lisp;
  }
  case TAG_VEC:
  {
    vec_t *vec   = calloc(1, sizeof(*vec));
    lisp_t *lisp = tag_vec(vec);
    vec_append(&sys->vectors, &lisp, sizeof(&lisp));
    return lisp;
  }
  // Shouldn't be registered
  case TAG_NIL:
  case TAG_INT:
  case TAG_SYM:
  case NUM_TAGS:
  default:
    FAIL("Unreachable");
  }
  return NIL;
}

u64 sys_cost(sys_t *sys)
{
  u64 vec_capacity = 0;
  for (u64 i = 0; i < VEC_SIZE(&sys->vectors, lisp_t *); ++i)
  {
    lisp_t *vec = VEC_GET(&sys->vectors, i, lisp_t *);
    vec_capacity += as_vec(vec)->capacity;
  }
  return sym_table_cost(&sys->symtable) + sys->conses.capacity + vec_capacity;
}

void sys_free(sys_t *sys)
{
  sym_table_free(&sys->symtable);

  // Iterate through each cell of memory currently allocated and free them
  for (size_t i = 0; i < VEC_SIZE(&sys->conses, lisp_t **); ++i)
  {
    lisp_t *allocated = VEC_GET(&sys->conses, i, lisp_t *);
    lisp_free(allocated);
  }

  // Iterate through each cell of memory currently allocated and free them
  for (size_t i = 0; i < VEC_SIZE(&sys->vectors, lisp_t **); ++i)
  {
    lisp_t *allocated = VEC_GET(&sys->vectors, i, lisp_t *);
    lisp_free(allocated);
  }

  // Free the containers
  vec_free(&sys->conses);
  vec_free(&sys->vectors);

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
