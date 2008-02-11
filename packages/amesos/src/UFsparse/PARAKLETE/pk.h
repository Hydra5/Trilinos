/*
 * Include file for demo main program.
 *
 * PARAKLETE version 0.3: parallel sparse LU factorization.  Nov 13, 2007
 * Copyright (C) 2007, Univ. of Florida.  Author: Timothy A. Davis
 * See License.txt for the Version 2.1 of the GNU Lesser General Public License
 * http://www.cise.ufl.edu/research/sparse
 */

#include "paraklete.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

cholmod_triplet *read_triplet (FILE *, cholmod_common *) ;
void my_handler (int status, char *file, int line, char *msg) ;

void *my_malloc2 (size_t size) ;
void *my_calloc2 (size_t n, size_t size) ;
void *my_realloc2 (void *p, size_t size) ;
void my_free2 (void *p) ;
void normal_memory_handler ( cholmod_common *cm ) ;
void test_memory_handler ( cholmod_common *cm ) ;

