/* -*- mode: C; c-basic-offset: 4 -*- */
#ident "Copyright (c) 2007-2010 Tokutek Inc.  All rights reserved."

#include <toku_portability.h>
#include "toku_assert.h"

#include <stdlib.h>
#include <stdio.h>
#if !TOKU_WINDOWS
#include <execinfo.h>
#endif


#if !TOKU_WINDOWS
#define N_POINTERS 1000
// These are statically allocated so that the backtrace can run without any calls to malloc()
static void *backtrace_pointers[N_POINTERS];
#endif

void (*do_assert_hook)(void) = NULL;

void toku_do_assert_fail (const char* expr_as_string,const char *function,const char*file,int line, int caller_errno)
{
        fprintf(stderr, "%s:%d %s: Assertion `%s' failed (errno=%d)\n", file,line,function,expr_as_string, caller_errno);

	// backtrace
#if !TOKU_WINDOWS
	int n = backtrace(backtrace_pointers, N_POINTERS);
	fprintf(stderr, "Backtrace: (Note: toku_do_assert=0x%p)\n", toku_do_assert); fflush(stderr);
	backtrace_symbols_fd(backtrace_pointers, n, fileno(stderr));
#endif

	fflush(stderr);

#if TOKU_WINDOWS
	//Following commented methods will not always end the process (could hang).
	//They could be unacceptable for other reasons as well (popups,
	//flush buffers before quitting, etc)
	//  abort()
	//  assert(FALSE) (assert.h assert)
	//  raise(SIGABRT)
	//  divide by 0
	//  null dereference
	//  _exit
	//  exit
	//  ExitProcess
	TerminateProcess(GetCurrentProcess(), 134); //Only way found so far to unconditionally
	//Terminate the process
#endif

	if (do_assert_hook) do_assert_hook();

	abort();
}

void toku_do_assert(int expr,const char* expr_as_string,const char *function,const char*file,int line, int caller_errno) {
    if (expr==0) {
      toku_do_assert_fail(expr_as_string, function, file, line, caller_errno);
    }
}
