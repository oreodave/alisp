/* vec.c: Stable Vector implementation
 * Created: 2025-08-20
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <string.h>

#include <alisp/vec.h>

void vec_init(vec_t *vec, u64 size)
{
  memset(vec, 0, sizeof(*vec));
  if (!vec)
    return;
  else if (size <= VEC_INLINE_CAPACITY)
  {
    vec->not_inlined = 0;
    vec->capacity    = VEC_INLINE_CAPACITY;
    vec->ptr         = NULL;
  }
  else
  {
    vec->not_inlined = 1;
    vec->capacity    = size;
    vec->ptr         = calloc(1, vec->capacity);
  }
}

void vec_free(vec_t *vec)
{
  if (!vec)
    return;
  if (vec->not_inlined && vec->ptr)
    free(vec->ptr);
  memset(vec, 0, sizeof(*vec));
}

u8 *vec_data(vec_t *vec)
{
  return vec->not_inlined ? vec->ptr : vec->inlined;
}

void vec_ensure_free(vec_t *vec, u64 size)
{
  if (!vec)
    return;
  if (vec->capacity == 0)
  {
    // We need to initialise this ourselves.
    vec->capacity = VEC_INLINE_CAPACITY;
    vec->size     = 0;
    memset(vec->inlined, 0, sizeof(vec->inlined));
  }

  if (vec->capacity - vec->size < size)
  {
    vec->capacity = MAX(vec->capacity * VEC_MULT, vec->size + size);
    if (!vec->not_inlined)
    {
      // If we're inlined, we need to allocate on the heap now.  So let's copy
      // vec->inlined over to vec->ptr, then turn off inlining.

      // We need to do a two-way swap since vec->ptr and vec->inlined are taking
      // up the same space.
      u8 buffer[VEC_INLINE_CAPACITY];
      memcpy(buffer, vec->inlined, vec->size);
      vec->ptr = calloc(1, vec->capacity);
      memcpy(vec->ptr, buffer, vec->size);
      vec->not_inlined = 1;
    }
    else
      vec->ptr = realloc(vec->ptr, vec->capacity);
  }
}

void vec_append(vec_t *vec, const void *const ptr, u64 size)
{
  if (!vec)
    return;
  vec_ensure_free(vec, size);
  if (ptr)
    memcpy(vec_data(vec) + vec->size, ptr, size);
  vec->size += size;
}

bool vec_try_append(vec_t *vec, const void *const ptr, u64 size)
{
  if (!vec || vec->capacity - vec->size < size)
    return false;
  if (ptr)
  {
    void *newptr = vec_data(vec) + vec->size;
    memcpy(newptr, ptr, size);
  }
  vec->size += size;
  return true;
}

void vec_clone(vec_t *dest, vec_t *src)
{
  if (!src || !dest)
    return;
  vec_init(dest, src->capacity);
  memcpy(vec_data(dest), vec_data(src), src->size);
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
