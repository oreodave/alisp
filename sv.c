/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the Unlicense
 * for details.

 * You may distribute and modify this code under the terms of the
 * Unlicense, which you should have received a copy of along with this
 * program.  If not, please go to <https://unlicense.org/>.

 * Created: 2025-08-21
 * Description: String views
 */

#include <malloc.h>
#include <string.h>

#include "./alisp.h"

sv_t sv_copy(sv_t old)
{
  char *newstr = calloc(1, (old.size + 1) * sizeof(*newstr));
  memcpy(newstr, old.data, old.size);
  newstr[old.size] = '\0';
  return SV(newstr, old.size);
}
