/* base.h: Basic macros and type definitions
 * Created: 2026-02-04
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef BASE_H
#define BASE_H

#include <assert.h>
#include <stdint.h>

/// Essential macros
#define MAX(A, B)      ((A) > (B) ? (A) : (B))
#define MIN(A, B)      ((A) < (B) ? (A) : (B))
#define ARRSIZE(A)     (sizeof(A) / sizeof((A)[0]))
#define NTH_BYTE(X, N) (((X) >> (8 * N)) & ((1 << 8) - 1))
#define FAIL(MSG)      assert(false && "FAIL: " #MSG)
#define TODO(MSG)      assert(false && "TODO: " #MSG)

/// Numeric aliases
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General *
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
