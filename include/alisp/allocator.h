/* allocator.h: Lisp Allocator
 * Created: 2026-02-12
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <alisp/lisp.h>
#include <alisp/vec.h>

#define ALLOC_PAGE_DEFAULT_SIZE 512

typedef struct
{
  u64 padding : 56;
  tag_t tag   : 8;
  u64 references;
} alloc_metadata_t;

static_assert(sizeof(alloc_metadata_t) == 16,
              "16 byte metadata required for alignment purposes");

typedef struct
{
  alloc_metadata_t metadata;
  u8 data[];
} alloc_node_t;

typedef struct
{
  vec_t data;
} page_t;

typedef struct
{
  vec_t pages;
  vec_t free_vec;
} alloc_t;

lisp_t *alloc_make(alloc_t *, tag_t type);
void alloc_delete(alloc_t *, lisp_t *);
u64 alloc_cost(alloc_t *);
void alloc_free(alloc_t *);

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
