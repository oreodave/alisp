/* sv.h: String views
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef SV_H
#define SV_H

#include <alisp/base.h>

/// String Views
typedef struct
{
  u64 size;
  char *data;
} sv_t;

// String view macro constructor
#define SV(DATA, SIZE) ((sv_t){.data = (DATA), .size = (SIZE)})
// Pretty printers
#define SV_FMT(SV) (int)(SV).size, (SV).data
#define PR_SV      "%.*s"
#define PRD_SV     "%d@%p"

// String view functions
sv_t sv_copy(sv_t);

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
