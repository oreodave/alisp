/* sv.c: String views
 * Created: 2025-08-21
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <string.h>

#include <alisp/sv.h>

sv_t sv_copy(sv_t old)
{
  if (old.size == 0)
    return SV(old.data, 0);
  else if (old.data == NULL)
    return SV(NULL, old.size);
  char *newstr = calloc(1, (old.size + 1) * sizeof(*newstr));
  memcpy(newstr, old.data, old.size);
  newstr[old.size] = '\0';
  return SV(newstr, old.size);
}

sv_t sv_chop_left(sv_t sv, u64 size)
{
  if (sv.size <= size)
    return SV(NULL, 0);
  return SV(sv.data + size, sv.size - size);
}

sv_t sv_chop_right(sv_t sv, u64 size)
{
  if (sv.size <= size)
    return SV(NULL, 0);
  return SV(sv.data, sv.size - size);
}

sv_t sv_substr(sv_t sv, u64 position, u64 size)
{
  return sv_chop_right(sv_chop_left(sv, position), size);
}

sv_t sv_truncate(sv_t sv, u64 newsize)
{
  if (newsize >= sv.size)
    return sv;
  return SV(sv.data, newsize);
}

sv_t sv_till(sv_t sv, const char *reject)
{
  if (sv.size == 0 || !sv.data)
    return SV(NULL, 0);

  u64 offset;
  for (offset = 0; offset < sv.size && strchr(reject, sv.data[offset]) == NULL;
       ++offset)
    continue;

  if (offset == sv.size)
    return sv;
  return sv_chop_right(sv, sv.size - offset);
}

sv_t sv_while(sv_t sv, const char *accept)
{
  if (sv.size == 0 || !sv.data)
    return SV(NULL, 0);

  u64 offset;
  for (offset = 0; offset < sv.size && strchr(accept, sv.data[offset]) != NULL;
       ++offset)
    continue;

  if (offset == sv.size)
    return sv;
  return sv_chop_right(sv, sv.size - offset);
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
