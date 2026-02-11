/* tag.h: Pointer tagging
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef TAG_H
#define TAG_H

#include <alisp/lisp.h>

typedef enum Tag
{
  TAG_NIL  = 0b00000000, // Start of atomic types
  TAG_INT  = 0b00000001, // Special tag so we can encode 63 bit integers
  TAG_SYM  = 0b00000100,
  TAG_CONS = 0b00000010, // Start of container types
  TAG_VEC  = 0b00000110,
  NUM_TAGS = 5,
} tag_t;

static_assert(NUM_TAGS == 5, "Expected NUM_TAGS == 5 for enum SHIFT");
enum Shift
{
  SHIFT_INT  = 1,
  SHIFT_SYM  = 8,
  SHIFT_CONS = 8,
  SHIFT_VEC  = 8,
};

static_assert(NUM_TAGS == 5, "Expected NUM_TAGS == 5 for enum MASK");
enum Mask
{
  MASK_INT  = 0b00000001,
  MASK_SYM  = 0b11111111,
  MASK_CONS = 0b11111111,
  MASK_VEC  = 0b11111111,
};

// Some helper macros for tagging
#define TAG(PTR, TYPE)    ((lisp_t *)(((PTR) << SHIFT_##TYPE) | TAG_##TYPE))
#define IS_TAG(PTR, TYPE) (((u64)(PTR) & MASK_##TYPE) == TAG_##TYPE)
#define UNTAG(PTR, TYPE)  (((u64)PTR) >> SHIFT_##TYPE)

#define INT_MAX ((((i64)1) << 62) - 1)
#define INT_MIN (-(((i64)1) << 62))

tag_t get_tag(const lisp_t *);
lisp_t *tag_int(const i64);
lisp_t *tag_sym(const char *);
lisp_t *tag_cons(const cons_t *);
lisp_t *tag_vec(const vec_t *);

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
