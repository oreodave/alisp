/* stream.h: Read/Write streams
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef STREAM_H
#define STREAM_H

#include <stdio.h>

#include <alisp/sv.h>
#include <alisp/vec.h>

typedef enum
{
  STREAM_TYPE_STRING = 0,
  STREAM_TYPE_PIPE,
  STREAM_TYPE_FILE,
} stream_type_t;

typedef enum
{
  STREAM_ERR_OK = 0,
  STREAM_ERR_INVALID_PTR,
  STREAM_ERR_FILE_NONEXISTENT,
  STREAM_ERR_FILE_READ,
  STREAM_ERR_PIPE_NONEXISTENT,
} stream_err_t;

const char *stream_err_to_cstr(stream_err_t);

typedef struct
{
  vec_t cache;
  FILE *file;
} stream_pipe_t;

typedef struct
{
  stream_type_t type;
  const char *name;
  u64 position;
  union
  {
    sv_t string;
    stream_pipe_t pipe;
  };
} stream_t;

#define STREAM_DEFAULT_CHUNK 64

stream_err_t stream_init_string(stream_t *, const char *, sv_t);
stream_err_t stream_init_pipe(stream_t *, const char *, FILE *);
stream_err_t stream_init_file(stream_t *, const char *, FILE *);
void stream_stop(stream_t *);

// End of Content (i.e. we've consumed all cached content/file)
bool stream_eoc(stream_t *);
// size of immediately accessible content
u64 stream_size(stream_t *);

// Return current character, push position by 1
char stream_next(stream_t *);
// Peek current character, do not push position
char stream_peek(stream_t *);
// Move forward or backward in the stream, return success of operation
bool stream_seek(stream_t *, i64);
bool stream_seek_forward(stream_t *, u64);
bool stream_seek_backward(stream_t *, u64);

// Return a relative substring of a given size
sv_t stream_substr(stream_t *, u64);
// Return an absolute substring at given index and of given size.
sv_t stream_substr_abs(stream_t *, u64, u64);

// Skip forward in stream till one of the characters in the given C string is
// encountered.
sv_t stream_till(stream_t *, const char *);
// Skip forward in stream while one of the characters in the given C string is
// present.
sv_t stream_while(stream_t *, const char *);

// Get the line and column of the stream at its current position.
void stream_line_col(stream_t *, u64 *line, u64 *col);

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
