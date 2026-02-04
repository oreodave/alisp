/* reader.h: Stream reader
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef READER_H
#define READER_H

#include <alisp/stream.h>

typedef enum
{
  READ_OK = 0,
} read_err_t;

// Attempt to read an expression from the stream, storing it in a pointer,
// returning any errors if failed.
read_err_t read(sys_t *, stream_t *, lisp_t **);

// Attempt to read all expressions from a stream till end of content, storing
// them in the given vector.  Return any error at any point during the read.
read_err_t read_all(sys_t *, stream_t *, vec_t *);

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
