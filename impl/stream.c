/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 * Created: 2025-08-26
 * Description: Stream implementation
 */

#include <string.h>

#include <alisp.h>

stream_err_t stream_init_string(stream_t *stream, char *name, sv_t contents)
{
  if (!stream)
    return STREAM_ERR_INVALID_PTR;
  name = name ? name : "<stream>";
  memset(stream, 0, sizeof(*stream));

  stream->type   = STREAM_TYPE_STRING;
  stream->name   = name;
  stream->string = contents;

  return STREAM_ERR_OK;
}

stream_err_t stream_init_file(stream_t *stream, char *name, FILE *pipe)
{
  if (!stream || !pipe)
    return STREAM_ERR_INVALID_PTR;
  name = name ? name : "<stream>";
  memset(stream, 0, sizeof(*stream));

  stream->type      = STREAM_TYPE_FILE;
  stream->name      = name;
  stream->pipe.file = pipe;

  vec_init(&stream->pipe.cache, STREAM_DEFAULT_CHUNK);
  // try to read an initial chunk
  stream_chunk(stream);

  return STREAM_ERR_OK;
}

void stream_stop(stream_t *stream)
{
  if (!stream)
    return;
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    // Nothing to do, all dealt with outside of stream
    break;
  case STREAM_TYPE_FILE:
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
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    // vacuously true
    return true;
  case STREAM_TYPE_FILE:
  {
    if (feof(stream->pipe.file))
      return false;
    vec_ensure_free(&stream->pipe.cache, STREAM_DEFAULT_CHUNK);
    int read = fread(vec_data(&stream->pipe.cache) + stream->pipe.cache.size, 1,
                     STREAM_DEFAULT_CHUNK, stream->pipe.file);
    stream->pipe.cache.size += read;
    return true;
  }
  default:
    FAIL("Unreachable");
    return 0;
  }
}

char stream_next(stream_t *stream)
{
  char c = stream_peek(stream);
  if (c != '\0')
    ++stream->position;
  return c;
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
  case STREAM_TYPE_FILE:
  {
    // Cached already?  We are done.
    if (stream->position < stream->pipe.cache.size)
      return ((char *)vec_data(&stream->pipe.cache))[stream->position];

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

bool stream_seek(stream_t *stream, i64 offset)
{
  if (offset < 0)
    return stream_seek_backward(stream, offset * -1);
  else if (offset > 0)
    return stream_seek_forward(stream, offset);
  else
    // vacuously successful
    return true;
}

bool stream_seek_forward(stream_t *stream, u64 offset)
{
  if (stream_eoc(stream))
    return false;

  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
  {
    if (stream->position + offset >= stream->string.size)
      return false;

    stream->position += offset;
    return true;
  }
  case STREAM_TYPE_FILE:
  {
    // Similar principle as stream_peek really...

    // Cached already?  We are done.
    if (stream->position + offset < stream->pipe.cache.size)
    {
      stream->position += offset;
      return true;
    }

    // Try to read chunks in till we've reached it or we're at the end of the
    // file.
    for (bool read_chunk = stream_chunk(stream);
         read_chunk && stream->position + offset >= stream->pipe.cache.size;
         read_chunk = stream_chunk(stream))
      continue;

    // Same principle as the stream_eoc(stream) check.
    if (stream->position + offset >= stream->pipe.cache.size)
      return false;
    stream->position += offset;
    return true;
  }
  default:
    FAIL("Unreachable");
    return 0;
  }
}

bool stream_seek_backward(stream_t *stream, u64 offset)
{
  assert(stream);
  if (stream->position < offset)
    return false;
  stream->position -= offset;
  return true;
}

sv_t stream_substr(stream_t *stream, u64 size)
{
  if (stream_eoc(stream))
    return SV(NULL, 0);

  // TODO: this is kinda disgusting, any better way of doing this
  u64 current_position = stream->position;
  bool successful      = stream_seek_forward(stream, size);
  // Reset the position in either situation
  stream->position = current_position;

  if (!successful)
    return SV(NULL, 0);

  char *ptr = NULL;
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    ptr = stream->string.data;
    break;
  case STREAM_TYPE_FILE:
    ptr = vec_data(&stream->pipe.cache);
    break;
  default:
    FAIL("Unreachable");
    return SV(NULL, 0);
  }

  return SV(ptr + stream->position, size);
}

sv_t stream_substr_abs(stream_t *stream, u64 index, u64 size)
{
  switch (stream->type)
  {
  case STREAM_TYPE_STRING:
    if (index + size <= stream_size(stream))
      return SV(stream->string.data + index, size);
    return SV(NULL, 0);
  case STREAM_TYPE_FILE:
  {
    if (index + size <= stream_size(stream))
      return SV(vec_data(&stream->pipe.cache) + index, size);
    // stream_size(stream) <= index + size => try reading chunks
    for (bool read_chunk = stream_chunk(stream);
         read_chunk && index + size >= stream->pipe.cache.size;
         read_chunk = stream_chunk(stream))
      continue;

    if (index + size > stream_size(stream))
      return SV(NULL, 0);
    return SV(vec_data(&stream->pipe.cache) + index, size);
  }
  default:
    assert("Unreachable");
    return SV(NULL, 0);
  }
}
