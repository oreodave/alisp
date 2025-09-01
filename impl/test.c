/* Copyright (C) 2025 Aryadev Chavali

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the Unlicense
 * for details.

 * You may distribute and modify this code under the terms of the
 * Unlicense, which you should have received a copy of along with this
 * program.  If not, please go to <https://unlicense.org/>.

 * Created: 2025-08-21
 * Description: Tests
 */

#include <stdio.h>
#include <string.h>

#include <alisp.h>

#define TEST_PASSED() printf("[%s]: Pass\n", __func__)
#define TEST(NAME, COND)                                \
  do                                                    \
  {                                                     \
    if (!(COND))                                        \
    {                                                   \
      printf("[%s]: `%s` failed!\n", __func__, (NAME)); \
      assert(0);                                        \
    }                                                   \
  } while (0)

// Sample data
const char *unique_words[] = {
    "bibendum",     "etiam",    "gravida",   "dui",       "cursus",
    "purus",        "diam",     "phasellus", "nam",       "fermentum",
    "leo",          "enim",     "ac",        "semper",    "non",
    "mauris",       "proin",    "tellus",    "vivamus",   "lobortis",
    "lacus",        "neque",    "in",        "nullam",    "felis",
    "orci",         "pede",     "tempus",    "nec",       "at",
    "tortor",       "massa",    "sed",       "magna",     "eget",
    "tempor",       "velit",    "imperdiet", "praesent",  "volutpat",
    "tristique",    "id",       "commodo",   "aliquet",   "quis",
    "pellentesque", "eleifend", "porta",     "nunc",      "euismod",
    "aliquam",      "a",        "erat",      "dignissim", "ut",
    "vitae",        "vel",      "donec",
};

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

char words_text[] =
    "aliquam erat volutpat nunc eleifend leo vitae magna in id erat non orci "
    "commodo lobortis proin neque massa cursus ut gravida ut lobortis eget "
    "lacus sed diam praesent fermentum tempor tellus nullam tempus mauris ac "
    "felis vel velit tristique imperdiet donec at pede etiam vel neque nec dui "
    "dignissim bibendum vivamus id enim phasellus neque orci porta a aliquet "
    "quis semper a massa phasellus purus pellentesque tristique imperdiet "
    "tortor nam euismod tellus id erat";

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

void symtable_test(void)
{
  sym_table_t table = {0};
  sym_table_init(&table);
  for (u64 i = 0; i < ARRSIZE(words); ++i)
    sym_table_find(&table, SV(words[i], strlen(words[i])));

  TEST("|table|=|set(words)|", table.count == ARRSIZE(unique_words));
  TEST("|table| < |words|", table.count < ARRSIZE(words));

  TEST_PASSED();

  sym_table_cleanup(&table);
}

void int_test(void)
{
  i64 ints[] = {
      1, -1, (1 << 10) - 1, (-1) * ((1 << 10) - 1), INT_MIN, INT_MAX,
  };

  for (u64 i = 0; i < ARRSIZE(ints); ++i)
  {
    i64 in       = ints[i];
    lisp_t *lisp = make_int(in);
    i64 out      = as_int(lisp);

    TEST("in == out", in == out);
  }

  TEST_PASSED();
}

void sym_test(void)
{
  sys_t system = {0};
  sys_init(&system);
  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    char *in     = words[i];
    lisp_t *lisp = intern(&system, SV(in, strlen(in)));
    char *out    = as_sym(lisp);
    TEST("unique pointers when interning a symbol", in != out);
    TEST("same size", strlen(in) == strlen(out));
    TEST("same string", strncmp(in, out, strlen(in)) == 0);
  }
  TEST_PASSED();
  sys_cleanup(&system);
}

void vec_test1(void)
{
  sys_t system = {0};
  sys_init(&system);

  // Generating a vector word by word
  lisp_t *lvec = make_vec(&system, 0);
  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    char *word = words[i];
    vec_append(as_vec(lvec), word, strlen(word));
    if (i != ARRSIZE(words) - 1)
      vec_append(as_vec(lvec), " ", 1);
  }
  vec_append(as_vec(lvec), "\0", 1);

  vec_t *vec = as_vec(lvec);

  TEST("same size vector", vec->size == ARRSIZE(words_text));
  TEST("same as actually concatenated string",
       strncmp(vec_data(vec), words_text, vec->size) == 0);

  TEST_PASSED();
  sys_cleanup(&system);
}

void vec_test2(void)
{
  sys_t system = {0};
  sys_init(&system);
  // Generating substrings
  struct Test
  {
    u64 start, size;
  } tests[] = {
      {0, 16},
      {0, 32},
      {32, 64},
      {0, ARRSIZE(text)},
  };

  for (u64 i = 0; i < ARRSIZE(tests); ++i)
  {
    struct Test test  = tests[i];
    const sv_t substr = SV(text + test.start, test.size);
    const u64 size    = test.size / 2;

    lisp_t *lvec = make_vec(&system, size);
    vec_append(as_vec(lvec), text + test.start, test.size);
    TEST("Vector grew", as_vec(lvec)->size > size);
    TEST("Vector's substring is equivalent to the actual substring",
         strncmp(vec_data(as_vec(lvec)), substr.data, substr.size) == 0);
  }

  printf("[vec_test2]: Pass\n");
  sys_cleanup(&system);
}

void cons_test(void)
{
  sys_t system = {0};
  sys_init(&system);

  // Let's make a list of words using `cons`
  lisp_t *lisp = NIL;
  for (u64 i = 0; i < ARRSIZE(words); ++i)
  {
    char *word    = words[i];
    lisp_t *lword = intern(&system, SV(word, strlen(word)));
    lisp          = cons(&system, lword, lisp);
  }

  // Make sure we've essentially reversed the `words` array
  u64 i = ARRSIZE(words);
  for (lisp_t *iter = lisp; iter; iter = cdr(iter))
  {
    lisp_t *item = car(iter);
    TEST("we've reversed the array",
         strncmp(words[i - 1], as_sym(item), strlen(words[i - 1])) == 0);
    i -= 1;
  }

  TEST_PASSED();

  sys_cleanup(&system);
}

typedef void (*test_fn)(void);

const test_fn TESTS[] = {
    int_test, sym_test, vec_test1, vec_test2, cons_test,
};

int main(void)
{
  for (u64 i = 0; i < ARRSIZE(TESTS); ++i)
  {
    TESTS[i]();
  }
  return 0;
}
