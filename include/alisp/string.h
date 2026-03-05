/* string.h: String library
 * Created: 2026-03-05
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef STRING_H
#define STRING_H

#include <alisp/sv.h>
#include <alisp/vec.h>

typedef struct
{
  vec_t data;
} str_t;

str_t string_make(sv_t sv);
sv_t string_sv(str_t *);

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
