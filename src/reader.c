/* reader.c: Stream reader implementation
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <ctype.h>
#include <string.h>

#include <alisp/base.h>
#include <alisp/reader.h>

const char *read_err_to_cstr(read_err_t err)
{
  switch (err)
  {
  case READ_ERR_OK:
    return "OK";
  case READ_ERR_EOF:
    return "EOF";
  case READ_ERR_UNKNOWN_CHAR:
    return "UNKNOWN_CHAR";
  default:
    FAIL("Unreachable");
  }
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
