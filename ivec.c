/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-19
 * Description: Inline Vector implementation
 */

#include <malloc.h>
#include <string.h>

#include "./alisp.h"

void ivec_make(void **ptr, u64 size)
{
  if (!ptr)
    return;
  ivec_t *ivector   = calloc(1, sizeof(*ivector) + size);
  ivector->size     = 0;
  ivector->capacity = size;
  *ptr              = (ivector + 1);
}

void ivec_free(void **data)
{
  if (!data || !*data)
    return;
  free(IVEC_GET(*data));
  *data = NULL;
}

void ivec_ensure_free(void **ptr, u64 space)
{
  if (!ptr || !*ptr)
    return;
  ivec_t *ivec = IVEC_GET(*ptr);
  if (ivec->capacity - ivec->size < space)
  {
    void *new_ivec = NULL;
    ivec_make(&new_ivec, MAX(ivec->capacity * IVEC_MULT, ivec->size + space));
    IVEC_SIZE(new_ivec) = ivec->size;
    memcpy(new_ivec, *ptr, ivec->size);
    ivec_free(ptr);
    *ptr = new_ivec;
  }
}

void ivec_append_byte(void **ptr, u8 byte)
{
  ivec_ensure_free(ptr, 1);
  ivec_t *ivec              = IVEC_GET(*ptr);
  ivec->bytes[ivec->size++] = byte;
}

void ivec_append(void **ptr, void *data, u64 size)
{
  ivec_ensure_free(ptr, size);
  ivec_t *ivec = IVEC_GET(*ptr);
  memcpy(*ptr + ivec->size, data, size);
  ivec->size += size;
}

void ivec_clone(void **dest, void **src)
{
  if (!dest || !src || !*src)
    return;
  ivec_make(dest, IVEC_SIZE(*src));
  memcpy(*dest, *src, IVEC_SIZE(*src));
  IVEC_SIZE(*dest) = IVEC_SIZE(*src);
}
