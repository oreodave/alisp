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
  case STREAM_ERR_FILE_NONEXISTENT:
    return "FILE NONEXISTENT";
  case STREAM_ERR_FILE_READ:
    return "FILE READ";
  case STREAM_ERR_PIPE_NONEXISTENT:
    return "PIPE NONEXISTENT";
  case STREAM_ERR_OK:
    return "OK";
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
  stream->pipe.file = NULL;

  // NOTE: We're reading all the data from the file descriptor now.
  fseek(pipe, 0, SEEK_END);
  long size = ftell(pipe);
  fseek(pipe, 0, SEEK_SET);
  vec_ensure_free(&stream->pipe.cache, size);
  int read = fread(vec_data(&stream->pipe.cache), 1, size, pipe);

  // These must be equivalent for this function.
  assert(read == size);
  stream->pipe.cache.size += size;

  return STREAM_ERR_OK;
}

void stream_reset(stream_t *stream)
{
  if (!stream)
    return;
  stream->position = 0;
}

void stream_free(stream_t *stream)
{
  if (!stream)
    return;
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    free((char *)stream->string.data);
    break;
  case STREAM_TYPE_FILE:
  case STREAM_TYPE_PIPE:
    // Must cleanup caching vector
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

bool stream_eoc(stream_t *stream)
{
  assert(stream);
  switch (stream->type)
  {
  case STREAM_TYPE_FILE:
  case STREAM_TYPE_STRING:
    return stream->position >= stream_size(stream);
  case STREAM_TYPE_PIPE:
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
  switch (stream->type)
  {
  case STREAM_TYPE_FILE:
  case STREAM_TYPE_STRING:
    // nothing to chunk, hence false
    return false;
  case STREAM_TYPE_PIPE:
  {
    if (feof(stream->pipe.file))
      // We can't read anymore.  End of the line
      return false;
    vec_ensure_free(&stream->pipe.cache, STREAM_DEFAULT_CHUNK);
    int read = fread(vec_data(&stream->pipe.cache) + stream->pipe.cache.size, 1,
                     STREAM_DEFAULT_CHUNK, stream->pipe.file);

    // If we read something it's a good thing
    if (read > 0)
    {
      stream->pipe.cache.size += read;
      return true;
    }
    else
    {
      return false;
    }
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
  // End of the line?  We're done.
  if (stream_eoc(stream))
    return '\0';

  switch (stream->type)
  {
  case STREAM_TYPE_FILE:
  case STREAM_TYPE_STRING:
    return stream_sv(stream).data[0];
  case STREAM_TYPE_PIPE:
  {
    // Cached already?  We are done.
    if (stream->position < stream->pipe.cache.size)
      return stream_sv(stream).data[0];

    // Try to read chunks in till we've reached it or we're at the end of the
    // file.
    for (bool read_chunk = stream_chunk(stream);
         read_chunk && stream->position >= stream->pipe.cache.size;
         read_chunk = stream_chunk(stream))
      continue;

    // Same principle as the stream_eos(stream) check.
    if (stream->position >= stream->pipe.cache.size)
      return '\0';
    return stream_sv(stream).data[0];
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
    return 0;
}

u64 stream_seek_forward(stream_t *stream, u64 offset)
{
  if (stream_eoc(stream))
    return 0;
  else if (stream->position + offset < stream_size(stream))
  {
    stream->position += offset;
    return offset;
  }

  // NOTE: The only case not caught by the above branches is exact-to-end
  // movement (i.e. offset puts us exactly at the end of the stream) or movement
  // beyond what we've cached.
  switch (stream->type)
  {
  case STREAM_TYPE_FILE:
  case STREAM_TYPE_STRING:
  {
    // Clamp in the case of FILE and STRING movement since they're already
    // fully cached.
    if (stream->position + offset >= stream_size(stream))
      offset = stream_size(stream) - stream->position;
    stream->position += offset;
    return offset;
  }
  case STREAM_TYPE_PIPE:
  {
    // Pipes may have data remaining that hasn't been cached - we need to chunk
    // before we can be sure to stop.

    // Try to read chunks in till we've reached it or we're at the end of the
    // file.
    for (bool read_chunk = stream_chunk(stream);
         read_chunk && stream->position + offset >= stream->pipe.cache.size;
         read_chunk = stream_chunk(stream))
      continue;

    // NOTE: We've read everything from the pipe, but the offset is greater.  We
    // must clamp here.
    if (stream->position + offset > stream_size(stream))
      offset = stream_size(stream) - stream->position;

    stream->position += offset;
    return offset;
  }
  default:
    FAIL("Unreachable");
  }
  return 0;
}

u64 stream_seek_backward(stream_t *stream, u64 offset)
{
  if (!stream)
    return 0;
  if (stream->position < offset)
    offset = stream->position;

  stream->position -= offset;
  return offset;
}

sv_t stream_sv(stream_t *stream)
{
  sv_t sv = stream_sv_abs(stream);
  sv      = sv_chop_left(sv, stream->position);
  return sv;
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
  case STREAM_TYPE_FILE:
  case STREAM_TYPE_PIPE:
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
  case STREAM_TYPE_PIPE:
  {
    if (index + size > stream_size(stream))
    {
      // => try reading chunks till either we drop or we have enough space
      for (bool read_chunk = stream_chunk(stream);
           read_chunk && index + size >= stream->pipe.cache.size;
           read_chunk = stream_chunk(stream))
        continue;
    }
    break;
  }
  case STREAM_TYPE_STRING:
  case STREAM_TYPE_FILE:
    break;
  default:
    FAIL("Unreachable");
  }

  sv_t sv = stream_sv_abs(stream);
  sv      = sv_chop_left(sv, index);
  sv      = sv_truncate(sv, size);
  return sv;
}

sv_t stream_till(stream_t *stream, const char *str)
{
  if (stream_eoc(stream))
    return SV(NULL, 0);

  sv_t cur_sv = stream_sv(stream);
  sv_t sv     = sv_till(cur_sv, str);
  stream_seek_forward(stream, sv.size);
  switch (stream->type)
  {
  case STREAM_TYPE_FILE:
  case STREAM_TYPE_STRING:
    return sv;
  case STREAM_TYPE_PIPE:
  {
    if (cur_sv.size > sv.size)
      return sv;

    // Build a substring by hand while chunking data.
    u64 index, size;
    for (index = stream->position - sv.size, size = sv.size;
         cur_sv.size == sv.size; size += sv.size)
    {
      cur_sv = stream_sv(stream);
      sv     = sv_till(cur_sv, str);
      stream_seek_forward(stream, sv.size);
      if (sv.size == 0)
        // Must stop if this has happened; nothing else to pick up.
        break;
    }
    return stream_substr_abs(stream, index, size);
  }
  default:
    FAIL("Unreachable");
  }
}

sv_t stream_while(stream_t *stream, const char *str)
{
  if (stream_eoc(stream))
    return SV(NULL, 0);

  sv_t cur_sv = stream_sv(stream);
  sv_t sv     = sv_while(cur_sv, str);
  stream_seek_forward(stream, sv.size);
  switch (stream->type)
  {
  case STREAM_TYPE_FILE:
  case STREAM_TYPE_STRING:
    return sv;
  case STREAM_TYPE_PIPE:
  {
    if (cur_sv.size > sv.size)
      return sv;

    // Build a substring by hand while chunking data.
    u64 index, size;
    for (index = stream->position - sv.size, size = sv.size;
         cur_sv.size == sv.size; size += sv.size)
    {
      cur_sv = stream_sv(stream);
      sv     = sv_while(cur_sv, str);
      stream_seek_forward(stream, sv.size);
      if (sv.size == 0)
        // Must stop if this has happened; nothing else to pick up.
        break;
    }
    return stream_substr_abs(stream, index, size);
  }
  default:
    FAIL("Unreachable");
  }
}

void stream_line_col(stream_t *stream, u64 *line, u64 *col)
{
  if (!stream || !line || !col)
    return;

  // Generate a string view from the stream of exactly the content /upto/
  // stream.postion.
  sv_t sv = stream_sv_abs(stream);
  sv      = sv_truncate(sv, stream->position + 1);

  *line = 1;
  *col  = 0;
  // TODO: Could this be faster?  Does it matter?
  for (u64 i = 0; i < sv.size; ++i)
  {
    char c = sv.data[i];
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
