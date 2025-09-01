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

#include <alisp.h>

int main(void)
{
  stream_t stream = {0};
  // const char data[] = "Hello, world!";
  // const sv_t sv     = SV(data, ARRSIZE(data) - 1);
  // stream_init_string(&stream, NULL, sv);

  // stream_init_file(&stream, "test.txt");

  stream_init_file(&stream, "stdin", stdin);
  printf("[debug]: setup stream pipe\n");
  do
  {
    printf("%s[%lu]: `%c`\n", stream.name, stream.position,
           stream_next(&stream));
  } while (!stream_eoc(&stream));
  printf("%lu/%lu\n", stream.position, stream_size(&stream));
  stream_stop(&stream);
  return 0;
}
