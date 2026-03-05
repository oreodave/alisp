/* sys.h: System context and constructors
 * Created: 2026-02-12
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef SYS_H
#define SYS_H

#include <alisp/allocator.h>
#include <alisp/lisp.h>

/// System context
typedef struct
{
  alloc_t memory;
  sym_table_t symtable;
} sys_t;

void sys_init(sys_t *);
lisp_t *sys_alloc(sys_t *, tag_t type);
void sys_free(sys_t *);

// Debugging function: provides total memory usage from system.
u64 sys_cost(sys_t *);

/// Constructors and general Lisp API
lisp_t *make_int(i64);
lisp_t *intern(sys_t *, sv_t);
lisp_t *cons(sys_t *, lisp_t *, lisp_t *);
lisp_t *make_vec(sys_t *, u64);
lisp_t *make_str(sys_t *, u64);

lisp_t *car(lisp_t *);
lisp_t *cdr(lisp_t *);

void lisp_free(sys_t *, lisp_t *);
void lisp_free_rec(sys_t *, lisp_t *);

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
