/* main.c: Entrypoint
 * Created: 2025-08-19
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <alisp/alisp.h>

void usage(FILE *fp);
int init_stream_on_args(int argc, char *argv[], FILE **pipe, stream_t *stream);

int main(int argc, char *argv[])
{
  int ret         = 0;
  FILE *pipe      = NULL;
  stream_t stream = {0};
  vec_t ast       = {0};
  sys_t sys       = {0};

  ret = init_stream_on_args(argc, argv, &pipe, &stream);
  if (ret)
    goto end;

  LOG("[INFO]: Initialised stream for `%s`\n", stream.name);
  {
    read_err_t err = read_all(&sys, &stream, &ast);
    if (err)
    {
      u64 line = 0, col = 0;
      stream_line_col(&stream, &line, &col);
      fprintf(stderr, "%s:%lu:%lu: ERROR: %s\n", stream.name, line, col,
              read_err_to_cstr(err));
      ret = 1;
      goto end;
    }
  }

  LOG("[INFO]: Utilised %lu bytes in parsing\n", sys_cost(&sys));
  LOG("[INFO]: Parsed %lu %s\n", VEC_SIZE(&ast, lisp_t *),
      VEC_SIZE(&ast, lisp_t *) == 1 ? "expr" : "exprs");

  {
    FOR_VEC(i, &ast, lisp_t *)
    {
#if VERBOSE_LOGS
      lisp_t *expr = VEC_GET(&ast, i, lisp_t *);
      printf("\t[%lu]: ", i);
      lisp_print(stdout, expr);
      printf("\n");
#endif
    }
  }

end:
  sys_free(&sys);
  vec_free(&ast);
  stream_free(&stream);
  if (pipe)
    fclose(pipe);
  return ret;
}

int init_stream_on_args(int argc, char *argv[], FILE **pipe, stream_t *stream)
{
  if (argc == 1)
  {
    usage(stderr);
    return 1;
  }
  else if (argc != 2)
  {
    TODO("alisp doesn't support multiple files currently.");
  }

  if (strncmp(argv[1], "--", 2) == 0)
  {
    stream_err_t err = stream_init_pipe(stream, "stdin", stdin);
    if (err)
    {
      fprintf(stderr, "ERROR: %s from `%s`\n", stream_err_to_cstr(err),
              argv[1]);
      return 1;
    }
  }
  else if (strncmp(argv[1], "--help", 6) == 0)
  {
    usage(stdout);
    return 0;
  }
  else
  {
    *pipe            = fopen(argv[1], "rb");
    stream_err_t err = stream_init_file(stream, argv[1], *pipe);
    if (err)
    {
      fprintf(stderr, "ERROR: %s from `%s`\n", stream_err_to_cstr(err),
              argv[1]);
      return 1;
    }
  }

  return 0;
}

void usage(FILE *fp)
{
  fprintf(fp, "Usage: alisp [OPTIONS...] FILE\n"
              "Options:\n"
              "\t--help        Print this usage and exit.\n"
              "File:\n"
              "\t<filename>  Read and interpret this file from filesystem.\n"
              "\t--          Read and interpret from stdin using an EOF.\n");
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
