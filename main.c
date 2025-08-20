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

sv_t sv_copy(sv_t old)
{
  char *newstr = calloc(1, old.size * sizeof(*newstr));
  memcpy(newstr, old.data, old.size);
  return SV(newstr, old.size);
}

int main(void)
{
  const char *words[] = {
      "aliquam",      "erat",      "volutpat",  "nunc",      "eleifend",
      "leo",          "vitae",     "magna",     "in",        "id",
      "erat",         "non",       "orci",      "commodo",   "lobortis",
      "proin",        "neque",     "massa",     "cursus",    "ut",
      "gravida",      "ut",        "lobortis",  "eget",      "lacus",
      "sed",          "diam",      "praesent",  "fermentum", "tempor",
      "tellus",       "nullam",    "tempus",    "mauris",    "ac",
      "felis",        "vel",       "velit",     "tristique", "imperdiet",
      "donec",        "at",        "pede",      "etiam",     "vel",
      "neque",        "nec",       "dui",       "dignissim", "bibendum",
      "vivamus",      "id",        "enim",      "phasellus", "neque",
      "orci",         "porta",     "a",         "aliquet",   "quis",
      "semper",       "a",         "massa",     "phasellus", "purus",
      "pellentesque", "tristique", "imperdiet", "tortor",    "nam",
      "euismod",      "tellus",    "id",        "erat",
  };

  vec_t vec = {0};
  vec_init(&vec, 0);

  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    vec_append(&vec, words[i], strlen(words[i]));
    vec_append(&vec, "\n", 1);
    printf("%lu/%lu, inlined?: %s\n", vec.size, vec.capacity,
           vec.is_inlined ? "yes" : "no");
  }

  printf("%lu/%lu: %.*s\n", vec.size, vec.capacity, (int)vec.size,
         vec_data(&vec));

  vec_free(&vec);

  // sym_table_t table = {0};
  // sym_table_init(&table);
  // // Let's hash the words of lorem ipsum
  // for (u64 i = 0; i < ARRSIZE(words); ++i)
  // {
  //   char *ptr = sym_table_find(&table, SV(words[i], strlen(words[i])));
  //   printf("%s => %p\n", words[i], ptr);
  // }

  // sym_table_cleanup(&table);
  return 0;
}
