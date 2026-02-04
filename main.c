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

const char *TOKEN_DELIM = "\n ";

int main(void)
{
  sym_table_t table = {0};
  sym_table_init(&table);

  char filename[] = "./lorem.txt";
  FILE *fp        = fopen(filename, "r");
  stream_t stream = {0};
  stream_init_file(&stream, filename, fp);

  for (u64 token_no = 1; !stream_eoc(&stream); ++token_no)
  {
    // Skip forward any delimiters
    stream_while(&stream, TOKEN_DELIM);
    // Get the token (up until delimiter)
    sv_t token     = stream_till(&stream, TOKEN_DELIM);
    char *interned = sym_table_find(&table, token);
    printf("%s[%lu] => `%s`\n", stream.name, token_no, interned);
  }

  printf("\nTable count=%lu\n", table.count);
  for (u64 i = 0, j = 0; i < table.capacity; ++i)
  {
    sv_t token = VEC_GET(&table.entries, i, sv_t);
    if (!token.data)
      continue;
    printf("[%lu]@[%lu] => `" PR_SV "`\n", j, i, SV_FMT(token));
    ++j;
  }

  stream_stop(&stream);
  fclose(fp);
  sym_table_cleanup(&table);
  return 0;
}

/* Copyright (C) 2025, 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 */
