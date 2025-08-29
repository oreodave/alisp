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

int main(void)
{
  stream_t stream = {0};
  char _data[]    = "Hello, world!";
  sv_t data       = SV(_data, ARRSIZE(_data) - 1);
  char filename[] = "lorem.txt";

  // stream_init_string(&stream, NULL, data);

  FILE *fp = fopen(filename, "rb");
  stream_init_file(&stream, filename, fp);

  // stream_init_file(&stream, "stdin", stdin);

  /// test 1
  // printf("[debug]: setup stream pipe\n");
  // do
  // {
  //   printf("%s[%lu]: `%c`\n", stream.name, stream.position,
  //          stream_next(&stream));
  // } while (!stream_eoc(&stream));
  // printf("%lu/%lu\n", stream.position, stream_size(&stream));

  /// test 2
  sv_t sv = stream_substr_abs(&stream, 21, 3);
  printf("`" PR_SV "`\n", SV_FMT(sv));

  stream_stop(&stream);
  // fclose(fp);
  return 0;
}
