/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-19
 * Description: Entrypoint
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "./alisp.h"

sv_t sv_copy(sv_t old)
{
  char *newstr = calloc(1, old.size * sizeof(*newstr));
  memcpy(newstr, old.data, old.size);
  return SV(newstr, old.size);
}

int main(void)
{
  sys_t sys;
  sys_init(&sys);
  sys_cleanup(&sys);
  return 0;
}
