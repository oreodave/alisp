/* string.c: String library implementation
 * Created: 2026-03-05
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <string.h>

#include <alisp/string.h>

str_t string_make(sv_t sv)
{
  str_t string = {0};
  if (sv.size)
  {
    vec_init(&string.data, sv.size);
    if (sv.data)
    {
      memcpy(vec_data(&string.data), sv.data, sv.size);
    }
  }
  return string;
}

sv_t string_sv(str_t *str)
{
  if (!str)
    return SV(NULL, 0);
  return SV((char *)vec_data(&str->data), str->data.size);
}

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
