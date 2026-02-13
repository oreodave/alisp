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
  u64 references;
  tag_t tag : 8;
} alloc_metadata_t;

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
  vec_t free_list;
} arena_t;

lisp_t *arena_make(arena_t *, tag_t type);
void arena_delete(arena_t *, lisp_t *);
u64 arena_cost(arena_t *);
void arena_free(arena_t *);

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
