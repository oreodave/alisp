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
  char *newstr = calloc(1, (old.size + 1) * sizeof(*newstr));
  memcpy(newstr, old.data, old.size);
  newstr[old.size] = '\0';
  return SV(newstr, old.size);
}

char *words[] = {
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

char text[] =
    "Pellentesque dapibus suscipit ligula.  Donec posuere augue in quam.  "
    "Etiam vel tortor sodales tellus ultricies commodo.  Suspendisse potenti.  "
    "Aenean in sem ac leo mollis blandit.  Donec neque quam, dignissim in, "
    "mollis nec, sagittis eu, wisi.  Phasellus lacus.  Etiam laoreet quam sed "
    "arcu.  Phasellus at dui in ligula mollis ultricies.  Integer placerat "
    "tristique nisl.  Praesent augue.  Fusce commodo.  Vestibulum convallis, "
    "lorem a tempus semper, dui dui euismod elit, vitae placerat urna tortor "
    "vitae lacus.  Nullam libero mauris, consequat quis, varius et, dictum id, "
    "arcu.  Mauris mollis tincidunt felis.  Aliquam feugiat tellus ut neque.  "
    "Nulla facilisis, risus a rhoncus fermentum, tellus tellus lacinia purus, "
    "et dictum nunc justo sit amet elit.";

void vec_test(void)
{
  vec_t vec = {0};
  vec_init(&vec, 0);

  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    vec_append(&vec, words[i], strlen(words[i]));
    vec_append(&vec, " ", 1);
    printf("[vec_test]: %lu/%lu, inlined?: %s\n", vec.size, vec.capacity,
           vec.is_inlined ? "yes" : "no");
  }

  printf("[vec_test]: Final: %lu/%lu: %.*s\n", vec.size, vec.capacity,
         (int)vec.size, (char *)vec_data(&vec));

  vec_free(&vec);
}

void symtable_test(void)
{
  sym_table_t table = {0};
  sym_table_init(&table);
  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    char *ptr = sym_table_find(&table, SV(words[i], strlen(words[i])));
    printf("[symtable_test]: %s => %p\n", words[i], ptr);
  }

  printf(
      "[symtable_test]: |words|=%lu, |table|= %lu => Unique word ratio: %lf\n",
      ARRSIZE(words), table.count, table.count / (double)ARRSIZE(words));

  sym_table_cleanup(&table);
}

void make_int_test(void)
{
  i64 ints[] = {
      1,       -1,      (1 << 10) - 1, (-1) * ((1 << 10) - 1),
      INT_MIN, INT_MAX, INT64_MAX,     INT64_MIN,
  };

  for (u64 i = 0; i < ARRSIZE(ints); ++i)
  {
    i64 in       = ints[i];
    lisp_t *lisp = make_int(in);
    i64 out      = as_int(lisp);

    printf("[make_int_test]: %#16lx => %#16lx => %#16lx\n", in, (u64)lisp, out);
  }
}

void intern_test(void)
{
  sys_t system = {0};
  sys_init(&system);
  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    char *in     = words[i];
    lisp_t *lisp = intern(&system, SV(in, strlen(in)));
    char *out    = as_sym(lisp);
    printf("[intern test]: `%s` -> %p -> `%s`\n", in, lisp, out);
  }
  sys_cleanup(&system);
}

void make_vec_test(void)
{
  struct Test
  {
    u64 start, size;
  } tests[] = {
      {0, 16},
      {0, 32},
      {32, 64},
      {0, ARRSIZE(text)},
  };

  sys_t system = {0};
  sys_init(&system);

  for (u64 i = 0; i < ARRSIZE(tests); ++i)
  {
    struct Test test = tests[i];
    // Always initialise below what we expect
    lisp_t *lvec = make_vec(&system, test.size / 2);
    vec_t *vec   = as_vec(lvec);
    printf("[make_vec_test]: %p -> %p[%lu/%lu] -> ", lvec, vec, vec->size,
           vec->capacity);
    vec_append(vec, text + test.start, test.size);
    printf("[%lu/%lu] -> ", vec->size, vec->capacity);
    printf("`%.*s`\n", (int)vec->size, (char *)vec_data(vec));
  }

  sys_cleanup(&system);
}

int main(void)
{
  vec_test();
  printf("\n");
  symtable_test();
  printf("\n");
  make_int_test();
  printf("\n");
  intern_test();
  printf("\n");
  make_vec_test();
  return 0;
}
