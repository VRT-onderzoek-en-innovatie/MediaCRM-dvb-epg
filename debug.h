#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <assert.h>

#ifdef NDEBUG
#define warn_assert(e)   ((void)0)
#else // not defined NDEBUG

#define warn_assert(e)  \
    ((e) ? (void)0 : __warn_assert (#e, __FILE__, __LINE__))
#define __warn_assert(e, file, line)  \
    (void)printf ("%s:%u: failed assertion `%s'\n", file, line, e)

#endif


#endif // __DEBUG_H__
