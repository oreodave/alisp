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

void usage(FILE *fp)
{
  fprintf(fp, "Usage: alisp [OPTIONS...] FILE\n"
              "Options:\n"
              "\t--help        Print this usage and exit.\n"
              "File:\n"
              "\t<filename>  Read and interpret this file from filesystem.\n"
              "\t--          Read and interpret from stdin using an EOF.\n");
}

int main(int argc, char *argv[])
{
  int ret = 0;
  if (argc == 1)
  {
    usage(stderr);
    ret = 1;
    goto end;
  }
  else if (argc != 2)
  {
    TODO("alisp doesn't support multiple files currently.");
  }

  FILE *fp        = NULL;
  stream_t stream = {0};
  if (strncmp(argv[1], "--", 2) == 0)
  {
    stream_err_t err = stream_init_pipe(&stream, "stdin", stdin);
    if (err)
    {
      fprintf(stderr, "ERROR: %s from `%s`\n", stream_err_to_cstr(err),
              argv[1]);
      ret = 1;
      goto end;
    }
  }
  else if (strncmp(argv[1], "--help", 6) == 0)
  {
    usage(stdout);
    goto end;
  }
  else
  {
    fp               = fopen(argv[1], "rb");
    stream_err_t err = stream_init_file(&stream, argv[1], fp);
    if (err)
    {
      fprintf(stderr, "ERROR: %s from `%s`\n", stream_err_to_cstr(err),
              argv[1]);
      ret = 1;
      goto end;
    }
  }

  LOG("[INFO]: Initialised stream for `%s`\n", stream.name);
end:
  if (fp)
    fclose(fp);
  stream_stop(&stream);
  return ret;
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
