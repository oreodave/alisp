/* reader.c: Stream reader implementation
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <ctype.h>
#include <string.h>

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
  case READ_ERR_EXPECTED_CLOSED_BRACE:
    return "EXPECTED_CLOSED_BRACE";
  case READ_ERR_EXPECTED_CLOSED_SQUARE_BRACKET:
    return "EXPECTED_CLOSED_SQUARE_BRACKET";
  case READ_ERR_UNEXPECTED_CLOSED_BRACE:
    return "UNEXPECTED_CLOSED_BRACE";
  case READ_ERR_UNEXPECTED_CLOSED_SQUARE_BRACKET:
    return "UNEXPECTED_CLOSED_SQUARE_BRACKET";
  default:
    FAIL("Unreachable");
  }
}

// Accepted characters for symbols.
static const char *SYMBOL_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!$%&*+,-./"
    ":<=>?@\\^_{|}~0123456789";

// Little predicate using SYMBOL_CHARS
bool is_sym(char c)
{
  return strchr(SYMBOL_CHARS, c) != NULL;
}

void skip_comments_and_whitespace(stream_t *stream)
{
  for (char c = stream_peek(stream); c != '\0' && (isspace(c) || c == ';');
       c      = stream_peek(stream))
  {
    stream_while(stream, " \t\n\0");
    if (stream_peek(stream) == ';')
    {
      // Skip till newline
      stream_till(stream, "\n");
    }
  }
}

read_err_t read_sym(sys_t *sys, stream_t *stream, lisp_t **ret)
{
  sv_t sym_sv = stream_while(stream, SYMBOL_CHARS);
  *ret        = intern(sys, sym_sv);
  return READ_ERR_OK;
}

read_err_t read_int(sys_t *sys, stream_t *stream, lisp_t **ret)
{
  sv_t digits_sv = stream_while(stream, "0123456789");
  if (is_sym(stream_peek(stream)))
  {
    // This is actually a symbol
    stream_seek_backward(stream, digits_sv.size);
    return read_sym(sys, stream, ret);
  }

  if (digits_sv.size >= 18)
  {
    TODO("alisp doesn't support big integers (bigger than 56 bits) yet");
  }

  i64 n = 0;
  for (u64 i = 0; i < digits_sv.size; ++i)
  {
    char c   = digits_sv.data[i];
    u8 digit = c - '0';

    // NOTE: 10i + digit > INT_MAX
    // => 10i > INT_MAX - digit
    // => i > (INT_MAX - digit) / 10
    if (n > (INT_MAX - digit) / 10)
    {
      TODO("alisp doesn't support big integers (bigger than 56 bits) yet");
    }

    n *= 10;
    n += digit;
  }

  *ret = make_int(n);
  return READ_ERR_OK;
}

read_err_t read_negative(sys_t *sys, stream_t *stream, lisp_t **ret)
{
  char c = stream_next(stream);
  if (isdigit(c))
  {
    read_err_t err = read_int(sys, stream, ret);
    if (err)
      return err;
    *ret = make_int(as_smi(*ret) * -1);
    return READ_ERR_OK;
  }
  else if (is_sym(c) || isspace(c))
  {
    stream_seek_backward(stream, 1);
    return read_sym(sys, stream, ret);
  }
  else
  {
    return READ_ERR_UNKNOWN_CHAR;
  }
}

read_err_t read_list(sys_t *sys, stream_t *stream, lisp_t **ret)
{
  // skip past the open parentheses '('
  (void)stream_next(stream);

  lisp_t *top = NIL;
  lisp_t *cur = NIL;
  while (!stream_eoc(stream) && stream_peek(stream) != ')')
  {
    lisp_t *item   = NIL;
    read_err_t err = read(sys, stream, &item);
    if (err == READ_ERR_EOF)
    {
      return READ_ERR_EXPECTED_CLOSED_BRACE;
    }
    else if (err)
    {
      return err;
    }
    else if (!top)
    {
      top = cons(sys, item, NIL);
      cur = top;
    }
    else
    {
      as_cons(cur)->cdr = cons(sys, item, NIL);
      cur               = cdr(cur);
    }
  }

  if (stream_peek(stream) != ')')
    return READ_ERR_EXPECTED_CLOSED_BRACE;

  stream_next(stream);
  *ret = top;
  return READ_ERR_OK;
}

read_err_t read_vec(sys_t *sys, stream_t *stream, lisp_t **ret)
{
  (void)stream_next(stream);
  lisp_t *container = make_vec(sys, 0);
  while (!stream_eoc(stream) && stream_peek(stream) != ']')
  {
    lisp_t *item   = NIL;
    read_err_t err = read(sys, stream, &item);
    if (err == READ_ERR_EOF)
    {
      return READ_ERR_EXPECTED_CLOSED_BRACE;
    }
    else if (err)
    {
      return err;
    }
    else
    {
      vec_append(as_vec(container), &item, sizeof(item));
    }
  }

  if (stream_peek(stream) != ']')
    return READ_ERR_EXPECTED_CLOSED_SQUARE_BRACKET;
  stream_next(stream);
  *ret = container;
  return READ_ERR_OK;
}

read_err_t read_quote(sys_t *sys, stream_t *stream, lisp_t **ret)
{
  lisp_t *to_quote = NIL;
  stream_next(stream);
  read_err_t err = read(sys, stream, &to_quote);
  if (err)
    return err;
  lisp_t *items[] = {intern(sys, SV_AUTO("quote")), to_quote};
  *ret            = make_list(sys, items, ARRSIZE(items));
  return READ_ERR_OK;
}

read_err_t read_all(sys_t *sys, stream_t *stream, vec_t *out)
{
  while (!stream_eoc(stream))
  {
    lisp_t *item   = NIL;
    read_err_t err = read(sys, stream, &item);
    if (err)
      return err;
    else
      vec_append(out, &item, sizeof(item));
    skip_comments_and_whitespace(stream);
  }

  return READ_ERR_OK;
}

read_err_t read(sys_t *sys, stream_t *stream, lisp_t **ret)
{
  skip_comments_and_whitespace(stream);
  if (stream_eoc(stream))
    return READ_ERR_EOF;
  char c = stream_peek(stream);
  if (isdigit(c))
    return read_int(sys, stream, ret);
  else if (c == '-')
    return read_negative(sys, stream, ret);
  else if (is_sym(c))
    return read_sym(sys, stream, ret);
  else if (c == '\'')
    return read_quote(sys, stream, ret);
  else if (c == '(')
    return read_list(sys, stream, ret);
  else if (c == ')')
    return READ_ERR_UNEXPECTED_CLOSED_BRACE;
  else if (c == '[')
    return read_vec(sys, stream, ret);
  else if (c == ']')
    return READ_ERR_UNEXPECTED_CLOSED_SQUARE_BRACKET;

  return READ_ERR_UNKNOWN_CHAR;
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
