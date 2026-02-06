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

/* Copyright (C) 2025, 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
