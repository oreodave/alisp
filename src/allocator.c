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
  case TAG_SMI:
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
  case TAG_SMI:
  case TAG_SYM:
  default:
    FAIL("Unreachable");
    return NIL;
  }

  alloc_node_t *node = raw_ptr;
  return &node[-1];
}

lisp_t *alloc_make(alloc_t *alloc, tag_t type)
{
  switch (type)
  {
  case TAG_CONS:
  case TAG_VEC:
    break;
  case TAG_NIL: // These shouldn't be allocated
  case TAG_SMI:
  case TAG_SYM:
  default:
    FAIL("Unreachable");
    return NIL;
  }

  // We want to try to fill this node with an allocation of this type.
  alloc_node_t *node = NULL;

  // Try to get something from the free vector
  u64 free_vec_size = VEC_SIZE(&alloc->free_vec, alloc_node_t *);
  for (u64 i = 0; i < free_vec_size; ++i)
  {
    alloc_node_t **nodeptr = &VEC_GET(&alloc->free_vec, i, alloc_node_t *);

    // Skip any nodes that don't have the right type.
    if (nodeptr[0]->metadata.tag != type)
      continue;

    assert("Expected free node to have no references" &&
           nodeptr[0]->metadata.references == 0);

    // Pop this node off the free vector by swapping it with the last item and
    // decrementing the size of the vector.

    alloc_node_t **lastptr =
        &VEC_GET(&alloc->free_vec, free_vec_size - 1, alloc_node_t *);
    alloc_node_t *val = *nodeptr;
    *nodeptr          = *lastptr;
    *lastptr          = val;

    // Decrement the size of the free vector
    alloc->free_vec.size -= sizeof(val);

    // Then use that valid (and now unused) node as our return.
    node = *lastptr;

    goto end;
  }

  // We couldn't get anything from the free vector, so try to allocate a fresh
  // one against one of the pages.
  for (u64 i = 0; i < VEC_SIZE(&alloc->pages, page_t *); ++i)
  {
    page_t *page = VEC_GET(&alloc->pages, i, page_t *);
    node         = make_node(page, type);
    if (node)
      goto end;
  }

  // There aren't any pages we can allocate against, so we need to make a new
  // page.
  page_t *page = make_page(0);
  vec_append(&alloc->pages, &page, sizeof(page));
  node = make_node(page, type);

end:
  if (!node)
    FAIL("Unexpected issue with allocating to a verifiably good page");

  return tag_generic(node->data, type);
}

void alloc_delete(alloc_t *alloc, lisp_t *lisp)
{
  alloc_node_t *node = lisp_to_node(lisp);
  assert(node && node->metadata.references == 0);
  vec_append(&alloc->free_vec, &node, sizeof(node));
}

u64 alloc_cost(alloc_t *alloc)
{
  u64 total_size = alloc->pages.size;
  for (u64 i = 0; i < VEC_SIZE(&alloc->pages, page_t *); ++i)
  {
    page_t *page = VEC_GET(&alloc->pages, i, page_t *);
    total_size += page->data.size;
  }
  return total_size;
}

void alloc_free(alloc_t *alloc)
{
  for (u64 i = 0; i < VEC_SIZE(&alloc->pages, page_t *); ++i)
  {
    page_t *page = VEC_GET(&alloc->pages, i, page_t *);
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
      case TAG_SMI:
      case TAG_SYM:
      default:
        FAIL("Unreachable");
      }
      j += next;
    }

    // Each page was allocated on the heap.
    vec_free(&page->data);
    free(page);
  }
  vec_free(&alloc->pages);
  vec_free(&alloc->free_vec);
  memset(alloc, 0, sizeof(*alloc));
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
