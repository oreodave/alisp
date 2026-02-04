/* vec.h: Dynamic Arrays with SBO
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef VEC_H
#define VEC_H

#include <stddef.h>

#include <alisp/base.h>

#define VEC_INLINE_CAPACITY 32
#define VEC_MULT            2

typedef struct Vector
{
  u64 size, capacity;
  // Small buffer optimisation
  u8 not_inlined;
  union
  {
    void *ptr;
    alignas(max_align_t) u8 inlined[VEC_INLINE_CAPACITY];
  };
} vec_t;

static_assert(sizeof(vec_t) == 64, "vec_t has to be 64 bytes as part of SBO");

#define VEC_GET(V, I, T) (((T *)vec_data(V))[I])
#define VEC_SIZE(V, T)   ((V)->size / (sizeof(T)))

void vec_init(vec_t *, u64);
void vec_free(vec_t *);
u8 *vec_data(vec_t *);

void vec_append(vec_t *, const void *const, u64);
void vec_ensure_free(vec_t *, u64);
void vec_clone(vec_t *, vec_t *);

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General *
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
