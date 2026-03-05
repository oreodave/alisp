/* lisp.h: Basic API for the Lisp
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef LISP_H
#define LISP_H

#include <stdio.h>

#include <alisp/symtable.h>
#include <alisp/vec.h>

#define NIL 0

// Opaque object for tagged pointers
typedef struct Obj lisp_t;

typedef struct
{
  lisp_t *car, *cdr;
} cons_t;

/// Tagging system
typedef enum Tag
{
  TAG_SMI  = 0b00000001, // Atomic types
  TAG_SYM  = 0b00000011,
  TAG_NIL  = 0b00000000, // Container types (0 LSB)
  TAG_CONS = 0b00000010,
  TAG_VEC  = 0b00000100,
  NUM_TAGS = 5,
} tag_t;

// Some helper macros for tagging
#define SHIFT_TAG (8)
#define MASK_TAG  ((1 << SHIFT_TAG) - 1)

#define TAG(PTR, TYPE)    ((lisp_t *)((((u64)(PTR)) << SHIFT_TAG) | TAG_##TYPE))
#define UNTAG(PTR)        (((u64)PTR) >> SHIFT_TAG)
#define GET_TAG(PTR)      ((tag_t)(((u64)(PTR)) & MASK_TAG))
#define IS_TAG(PTR, TYPE) (GET_TAG(PTR) == TAG_##TYPE)

#define INT_BITS ((sizeof(i64) * 8) - SHIFT_TAG)
#define INT_MAX  ((((i64)1) << (INT_BITS - 1)) - 1)
#define INT_MIN  (-(INT_MAX + 1))

tag_t get_tag(const lisp_t *);
lisp_t *tag_smi(const i64);
lisp_t *tag_sym(const char *);
lisp_t *tag_cons(const cons_t *);
lisp_t *tag_vec(const vec_t *);
lisp_t *tag_generic(void *, tag_t);

i64 as_smi(lisp_t *);
char *as_sym(lisp_t *);
cons_t *as_cons(lisp_t *);
vec_t *as_vec(lisp_t *);

#define CAR(L) (as_cons(L)->car)
#define CDR(L) (as_cons(L)->cdr)

void lisp_print(FILE *, lisp_t *);
u64 tag_sizeof(tag_t);
u64 lisp_sizeof(lisp_t *);

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
