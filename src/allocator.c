/* allocator.c: Allocator implementations
 * Created: 2026-02-12
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#include <stdlib.h>

#include <alisp/allocator.h>
#include <alisp/lisp.h>
#include <alisp/vec.h>
#include <string.h>

page_t *make_page(u64 size)
{
  page_t *page = calloc(1, sizeof(*page));
  vec_init(&page->data, MAX(size, ALLOC_PAGE_DEFAULT_SIZE));
  return page;
}

alloc_node_t *make_node(page_t *page, tag_t type)
{
  alloc_node_t *node = NULL;
  u64 size           = sizeof(*node);
  switch (type)
  {
  case TAG_CONS:
    size += sizeof(cons_t);
    break;
  case TAG_VEC:
    size += sizeof(vec_t);
    break;
  case TAG_NIL:
  case TAG_INT:
  case TAG_SYM:
  default:
    FAIL("Unreachable");
    return node;
  }

  // We must ensure size is a multiple of 8 for alignment purposes
  size = (size & 0b111) == 0 ? size : size + (8 - (size & 0b111));

  if (!vec_try_append(&page->data, NULL, size))
    return NULL;
  node = (alloc_node_t *)(vec_data(&page->data) + page->data.size - size);
  node->metadata = (alloc_metadata_t){.references = 0, .tag = type};
  return node;
}

alloc_node_t *lisp_to_node(lisp_t *lisp)
{
  void *raw_ptr = NULL;
  switch (get_tag(lisp))
  {
  case TAG_CONS:
    raw_ptr = as_cons(lisp);
    break;
  case TAG_VEC:
    raw_ptr = as_vec(lisp);
    break;
  case TAG_NIL: // These shouldn't be allocated
  case TAG_INT:
  case TAG_SYM:
  default:
    FAIL("Unreachable");
    return NIL;
  }

  alloc_metadata_t *data = raw_ptr;
  return (alloc_node_t *)(&data[-1]);
}

lisp_t *arena_make(arena_t *arena, tag_t type)
{
  switch (type)
  {
  case TAG_CONS:
  case TAG_VEC:
    break;
  case TAG_NIL: // These shouldn't be allocated
  case TAG_INT:
  case TAG_SYM:
  default:
    FAIL("Unreachable");
    return NIL;
  }

  for (u64 i = 0; i < VEC_SIZE(&arena->pages, page_t *); ++i)
  {
    page_t *page       = VEC_GET(&arena->pages, i, page_t *);
    alloc_node_t *node = make_node(page, type);
    if (node)
    {
      return tag_generic(node->data, type);
    }
  }

  // We'll have to make a new page to satisfy the size requirements.
  page_t *page = make_page(0);
  vec_append(&arena->pages, &page, sizeof(page));
  alloc_node_t *node = make_node(page, type);

  if (!node)
  {
    FAIL("Unexpected issue with allocating to a verifiably good page");
  }
  return tag_generic(node->data, type);
}

void arena_delete(arena_t *arena, lisp_t *lisp)
{
  alloc_node_t *node = lisp_to_node(lisp);
  assert(node && node->metadata.references == 0);
  vec_append(&arena->free_list, &node, sizeof(node));
}

u64 arena_cost(arena_t *arena)
{
  u64 total_size = arena->pages.size;
  for (u64 i = 0; i < VEC_SIZE(&arena->pages, page_t *); ++i)
  {
    page_t *page = VEC_GET(&arena->pages, i, page_t *);
    total_size += page->data.size;
  }
  return total_size;
}

void arena_free(arena_t *arena)
{
  for (u64 i = 0; i < VEC_SIZE(&arena->pages, page_t *); ++i)
  {
    page_t *page = VEC_GET(&arena->pages, i, page_t *);
    // Iterate through every alloc_node in this page
    for (u64 j = 0; j < VEC_SIZE(&page->data, u8);)
    {
      alloc_node_t *node = (alloc_node_t *)(vec_data(&page->data) + j);
      u64 next           = sizeof(*node) + tag_sizeof(node->metadata.tag);
      switch (node->metadata.tag)
      {
      case TAG_CONS:
        // Do nothing - will be cleaned by overall vec free anyway
        break;
      case TAG_VEC:
        vec_free((vec_t *)node->data);
        break;
      case TAG_NIL:
      case TAG_INT:
      case TAG_SYM:
      default:
        FAIL("Unreachable");
      }
      j += next;
    }
    vec_free(&page->data);
    free(page);
  }
  vec_free(&arena->pages);
  vec_free(&arena->free_list);
  memset(arena, 0, sizeof(*arena));
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
