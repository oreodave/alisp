/* lisp.h: Basic API for the Lisp
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef LISP_H
#define LISP_H

#include <alisp/symtable.h>
#include <alisp/vec.h>

#define NIL 0

// Opaque object for tagged pointers
typedef struct Obj lisp_t;

typedef struct
{
  lisp_t *car, *cdr;
} cons_t;

/// System context
typedef struct
{
  vec_t memory;
  sym_table_t symtable;
} sys_t;

void sys_init(sys_t *);
void sys_register(sys_t *, lisp_t *);
void sys_free(sys_t *);

/// Constructors and destructors
lisp_t *make_int(i64);
lisp_t *make_vec(sys_t *, u64);
lisp_t *intern(sys_t *, sv_t);
lisp_t *cons(sys_t *, lisp_t *, lisp_t *);

i64 as_int(lisp_t *);
char *as_sym(lisp_t *);
cons_t *as_cons(lisp_t *);
vec_t *as_vec(lisp_t *);

#define CAR(L) (as_cons(L)->car)
#define CDR(L) (as_cons(L)->cdr)

lisp_t *car(lisp_t *);
lisp_t *cdr(lisp_t *);

void lisp_free(lisp_t *);

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
