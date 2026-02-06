/* stream.c: Stream implementation
 * Created: 2025-08-26
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <alisp/base.h>
#include <alisp/stream.h>
#include <alisp/sv.h>
#include <alisp/vec.h>

const char *stream_err_to_cstr(stream_err_t err)
{
  switch (err)
  {
  case STREAM_ERR_INVALID_PTR:
    return "INVALID PTR";
    break;
  case STREAM_ERR_FILE_NONEXISTENT:
    return "FILE NONEXISTENT";
    break;
  case STREAM_ERR_FILE_READ:
    return "FILE READ";
    break;
  case STREAM_ERR_PIPE_NONEXISTENT:
    return "PIPE NONEXISTENT";
    break;
  case STREAM_ERR_OK:
    return "OK";
    break;
  default:
    FAIL("Unreachable");
  }
}

stream_err_t stream_init_string(stream_t *stream, const char *name,
                                sv_t contents)
{
  if (!stream)
    return STREAM_ERR_INVALID_PTR;
  name = name ? name : "<stream>";
  memset(stream, 0, sizeof(*stream));

  stream->type   = STREAM_TYPE_STRING;
  stream->name   = name;
  stream->string = sv_copy(contents);

  return STREAM_ERR_OK;
}

stream_err_t stream_init_pipe(stream_t *stream, const char *name, FILE *pipe)
{
  if (!stream)
    return STREAM_ERR_INVALID_PTR;
  else if (!pipe)
    return STREAM_ERR_PIPE_NONEXISTENT;

  name = name ? name : "<stream>";
  memset(stream, 0, sizeof(*stream));

  stream->type      = STREAM_TYPE_PIPE;
  stream->name      = name;
  stream->pipe.file = pipe;

  vec_init(&stream->pipe.cache, STREAM_DEFAULT_CHUNK);

  return STREAM_ERR_OK;
}

stream_err_t stream_init_file(stream_t *stream, const char *name, FILE *pipe)
{
  if (!stream)
    return STREAM_ERR_INVALID_PTR;
  else if (!pipe)
    return STREAM_ERR_FILE_NONEXISTENT;

  name = name ? name : "<stream>";
  memset(stream, 0, sizeof(*stream));

  stream->type      = STREAM_TYPE_FILE;
  stream->name      = name;
  stream->pipe.file = pipe;

  return STREAM_ERR_OK;
}

void stream_stop(stream_t *stream)
{
  if (!stream)
    return;
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    free(stream->string.data);
    break;
  case STREAM_TYPE_FILE:
    // ensure we reset the FILE pointer to the start
    fseek(stream->pipe.file, 0, SEEK_SET);
    // fallthrough
  case STREAM_TYPE_PIPE:
    // Must cleanup vector
    vec_free(&stream->pipe.cache);
    break;
  }
  memset(stream, 0, sizeof(*stream));
}

u64 stream_size(stream_t *stream)
{
  assert(stream);
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    return stream->string.size;
  case STREAM_TYPE_PIPE:
  case STREAM_TYPE_FILE:
    return stream->pipe.cache.size;
  default:
    FAIL("Unreachable");
    return 0;
  }
}

bool stream_eos(stream_t *stream)
{
  assert(stream);
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    return stream->position >= stream->string.size;
  case STREAM_TYPE_PIPE:
  case STREAM_TYPE_FILE:
    return feof(stream->pipe.file);
  default:
    FAIL("Unreachable");
    return 0;
  }
}

bool stream_eoc(stream_t *stream)
{
  assert(stream);
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    return stream->position >= stream->string.size;
  case STREAM_TYPE_PIPE:
  case STREAM_TYPE_FILE:
    return feof(stream->pipe.file) &&
           stream->position >= stream->pipe.cache.size;
  default:
    FAIL("Unreachable");
    return 0;
  }
}

bool stream_chunk(stream_t *stream)
{
  assert(stream);
  u64 to_read = STREAM_DEFAULT_CHUNK;
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    // vacuously true
    return true;
  case STREAM_TYPE_PIPE:
    to_read = 1;
    // fallthrough
  case STREAM_TYPE_FILE:
  {
    if (feof(stream->pipe.file))
      // We can't read anymore.  End of the line
      return false;
    vec_ensure_free(&stream->pipe.cache, to_read);
    int read = fread(vec_data(&stream->pipe.cache) + stream->pipe.cache.size, 1,
                     to_read, stream->pipe.file);

    // If we read something it's a good thing
    if (read > 0)
    {
      stream->pipe.cache.size += read;
      return true;
    }
    else
      return false;
  }
  default:
    FAIL("Unreachable");
    return 0;
  }
}

char stream_next(stream_t *stream)
{
  if (stream_peek(stream) != '\0')
    ++stream->position;
  return stream_peek(stream);
}

char stream_peek(stream_t *stream)
{
  // If we've reached end of stream, and end of content, there's really nothing
  // to check here.
  if (stream_eoc(stream))
    return '\0';

  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    return stream->string.data[stream->position];
  case STREAM_TYPE_PIPE:
  case STREAM_TYPE_FILE:
  {
    // Cached already?  We are done.
    if (stream->position < stream->pipe.cache.size)
    {
      const char *const str = (char *)vec_data(&stream->pipe.cache);
      return str[stream->position];
    }

    // Try to read chunks in till we've reached it or we're at the end of the
    // file.
    for (bool read_chunk = stream_chunk(stream);
         read_chunk && stream->position >= stream->pipe.cache.size;
         read_chunk = stream_chunk(stream))
      continue;

    // Same principle as the stream_eos(stream) check.
    if (stream->position >= stream->pipe.cache.size)
      return '\0';
    return ((char *)vec_data(&stream->pipe.cache))[stream->position];
  }
  default:
    FAIL("Unreachable");
    return 0;
  }
}

u64 stream_seek(stream_t *stream, i64 offset)
{
  if (offset < 0)
    return stream_seek_backward(stream, offset * -1);
  else if (offset > 0)
    return stream_seek_forward(stream, offset);
  else
    // vacuously successful
    return true;
}

u64 stream_seek_forward(stream_t *stream, u64 offset)
{
  if (stream_eoc(stream))
    return 0;

  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
  {
    if (stream->position + offset >= stream->string.size)
      return 0;

    stream->position += offset;
    return offset;
  }
  case STREAM_TYPE_PIPE:
  case STREAM_TYPE_FILE:
  {
    // Similar principle as stream_peek really...

    // Cached already?  We are done.
    if (stream->position + offset < stream->pipe.cache.size)
    {
      stream->position += offset;
      return offset;
    }

    // Try to read chunks in till we've reached it or we're at the end of the
    // file.
    for (bool read_chunk = stream_chunk(stream);
         read_chunk && stream->position + offset >= stream->pipe.cache.size;
         read_chunk = stream_chunk(stream))
      continue;

    // Same principle as the stream_eoc(stream) check.
    if (stream->position + offset > stream->pipe.cache.size)
    {
      offset = stream->pipe.cache.size - stream->position;
    }
    stream->position += offset;
    return offset;
  }
  default:
    FAIL("Unreachable");
    return 0;
  }
}

u64 stream_seek_backward(stream_t *stream, u64 offset)
{
  assert(stream);
  if (stream->position < offset)
  {
    offset = stream->position;
  }

  stream->position -= offset;
  return offset;
}

sv_t stream_sv(stream_t *stream)
{
  return sv_chop_left(stream_sv_abs(stream), stream->position);
}

sv_t stream_sv_abs(stream_t *stream)
{
  if (!stream)
    return SV(NULL, 0);
  sv_t sv = {0};
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    sv = stream->string;
    break;
  case STREAM_TYPE_PIPE:
  case STREAM_TYPE_FILE:
    sv = SV((char *)vec_data(&stream->pipe.cache), stream_size(stream));
    break;
  default:
    FAIL("Unreachable");
    return SV(NULL, 0);
  }
  return sv;
}

sv_t stream_substr(stream_t *stream, u64 size)
{
  if (stream_eoc(stream))
    return SV(NULL, 0);

  // See if I can go forward enough to make this substring
  u64 current_position = stream->position;
  u64 successful       = stream_seek_forward(stream, size);
  // Reset the position in either situation
  stream->position = current_position;

  if (successful != size)
    return SV(NULL, 0);

  sv_t sv = stream_sv(stream);
  sv      = sv_truncate(sv, size);
  return sv;
}

sv_t stream_substr_abs(stream_t *stream, u64 index, u64 size)
{
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    return sv_truncate(sv_chop_left(stream_sv_abs(stream), index), size);
  case STREAM_TYPE_PIPE:
  case STREAM_TYPE_FILE:
  {
    if (index + size > stream_size(stream))
      // => try reading chunks till either we drop or we have enough space
      for (bool read_chunk = stream_chunk(stream);
           read_chunk && index + size >= stream->pipe.cache.size;
           read_chunk = stream_chunk(stream))
        continue;

    sv_t sv = stream_sv_abs(stream);
    sv      = sv_chop_left(sv, index);
    sv      = sv_truncate(sv, size);
    return sv;
  }
  default:
    assert("Unreachable");
    return SV(NULL, 0);
  }
}

sv_t stream_till(stream_t *stream, const char *str)
{
  if (stream_eoc(stream))
    return SV(NULL, 0);
  u64 current_position = stream->position;
  for (char c = stream_peek(stream); c != '\0' && strchr(str, c) == NULL;
       c      = stream_next(stream))
    continue;
  u64 size = stream->position - current_position;
  if (size == 0)
    return SV(NULL, 0);
  return stream_substr_abs(stream, current_position, size - 1);
}

sv_t stream_while(stream_t *stream, const char *str)
{
  if (stream_eoc(stream))
    return SV(NULL, 0);
  u64 current_position = stream->position;
  for (char c = stream_peek(stream); c != '\0' && strchr(str, c);
       c      = stream_next(stream))
    continue;
  u64 size = stream->position - current_position;
  if (size == 0)
    return SV(NULL, 0);
  return stream_substr_abs(stream, current_position, size - 1);
}

void stream_line_col(stream_t *stream, u64 *line, u64 *col)
{
  if (!stream || !line || !col)
    return;
  // Go through the cache, byte by byte.
  char *cache = NULL;
  u64 size    = 0;
  if (stream->type == STREAM_TYPE_STRING)
  {
    cache = stream->string.data;
    size  = stream->string.size;
  }
  else
  {
    cache = (char *)vec_data(&stream->pipe.cache);
    size  = stream->pipe.cache.size;
  }

  *line = 1;
  *col  = 0;
  for (u64 i = 0; i < size; ++i)
  {
    char c = cache[i];
    if (c == '\n')
    {
      *line += 1;
      *col = 0;
    }
    else
    {
      *col += 1;
    }
  }
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
