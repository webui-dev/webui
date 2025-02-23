/* Copyright (c) 2013-2024 the Civetweb developers
 * Copyright (c) 2004-2013 Sergey Lyubka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if defined(__GNUC__) || defined(__MINGW32__)
#ifndef GCC_VERSION
#define GCC_VERSION                                                            \
	(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif
#if GCC_VERSION >= 40500
/* gcc diagnostic pragmas available */
#define GCC_DIAGNOSTIC
#endif
#endif

#if defined(GCC_DIAGNOSTIC)
/* Disable unused macros warnings - not all defines are required
 * for all systems and all compilers. */
#pragma GCC diagnostic ignored "-Wunused-macros"
/* A padding warning is just plain useless */
#pragma GCC diagnostic ignored "-Wpadded"
#endif

#if defined(__clang__) /* GCC does not (yet) support this pragma */
/* We must set some flags for the headers we include. These flags
 * are reserved ids according to C99, so we need to disable a
 * warning for that. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#endif

#if defined(_WIN32)
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS /* Disable deprecation warning in VS2005 */
#endif
#if !defined(_WIN32_WINNT) /* Minimum API version */
#define _WIN32_WINNT 0x0601
#endif
#else
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE /* for setgroups(), pthread_setname_np() */
#endif
#if defined(__linux__) && !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 600 /* For flockfile() on Linux */
#endif
#if defined(__LSB_VERSION__) || defined(__sun)
#define NEED_TIMEGM
#define NO_THREAD_NAME
#endif
#if !defined(_LARGEFILE_SOURCE)
#define _LARGEFILE_SOURCE /* For fseeko(), ftello() */
#endif
#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64 /* Use 64-bit file offsets by default */
#endif
#if !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS /* <inttypes.h> wants this for C++ */
#endif
#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS /* C++ wants that for INT64_MAX */
#endif
#if !defined(_DARWIN_UNLIMITED_SELECT)
#define _DARWIN_UNLIMITED_SELECT
#endif
#if defined(__sun)
#define __EXTENSIONS__  /* to expose flockfile and friends in stdio.h */
#define __inline inline /* not recognized on older compiler versions */
#endif
#endif

#if defined(__clang__)
/* Enable reserved-id-macro warning again. */
#pragma GCC diagnostic pop
#endif


#if defined(USE_LUA)
#define USE_TIMERS
#endif

#if defined(_MSC_VER)
/* 'type cast' : conversion from 'int' to 'HANDLE' of greater size */
#pragma warning(disable : 4306)
/* conditional expression is constant: introduced by FD_SET(..) */
#pragma warning(disable : 4127)
/* non-constant aggregate initializer: issued due to missing C99 support */
#pragma warning(disable : 4204)
/* padding added after data member */
#pragma warning(disable : 4820)
/* not defined as a preprocessor macro, replacing with '0' for '#if/#elif' */
#pragma warning(disable : 4668)
/* no function prototype given: converting '()' to '(void)' */
#pragma warning(disable : 4255)
/* function has been selected for automatic inline expansion */
#pragma warning(disable : 4711)
#endif


/* This code uses static_assert to check some conditions.
 * Unfortunately some compilers still do not support it, so we have a
 * replacement function here. */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ > 201100L
#define mg_static_assert _Static_assert
#elif defined(__cplusplus) && __cplusplus >= 201103L
#define mg_static_assert static_assert
#else
char static_assert_replacement[1];
#define mg_static_assert(cond, txt)                                            \
	extern char static_assert_replacement[(cond) ? 1 : -1]
#endif

mg_static_assert(sizeof(int) == 4 || sizeof(int) == 8,
                 "int data type size check");
mg_static_assert(sizeof(void *) == 4 || sizeof(void *) == 8,
                 "pointer data type size check");
mg_static_assert(sizeof(void *) >= sizeof(int), "data type size check");


/* Select queue implementation. Diagnosis features originally only implemented
 * for the "ALTERNATIVE_QUEUE" have been ported to the previous queue
 * implementation (NO_ALTERNATIVE_QUEUE) as well. The new configuration value
 * "CONNECTION_QUEUE_SIZE" is only available for the previous queue
 * implementation, since the queue length is independent from the number of
 * worker threads there, while the new queue is one element per worker thread.
 *
 */
#if defined(NO_ALTERNATIVE_QUEUE) && defined(ALTERNATIVE_QUEUE)
/* The queues are exclusive or - only one can be used. */
#error                                                                         \
    "Define ALTERNATIVE_QUEUE or NO_ALTERNATIVE_QUEUE (or none of them), but not both"
#endif
#if !defined(NO_ALTERNATIVE_QUEUE) && !defined(ALTERNATIVE_QUEUE)
/* Use a default implementation */
#define NO_ALTERNATIVE_QUEUE
#endif

#if defined(NO_FILESYSTEMS) && !defined(NO_FILES)
/* File system access:
 * NO_FILES = do not serve any files from the file system automatically.
 * However, with NO_FILES CivetWeb may still write log files, read access
 * control files, default error page files or use API functions like
 * mg_send_file in callbacks to send files from the server local
 * file system.
 * NO_FILES only disables the automatic mapping between URLs and local
 * file names.
 * NO_FILESYSTEM = do not access any file at all. Useful for embedded
 * devices without file system. Logging to files in not available
 * (use callbacks instead) and API functions like mg_send_file are not
 * available.
 * If NO_FILESYSTEM is set, NO_FILES must be set as well.
 */
#error "Inconsistent build flags, NO_FILESYSTEMS requires NO_FILES"
#endif

/* DTL -- including winsock2.h works better if lean and mean */
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(__SYMBIAN32__)
/* According to https://en.wikipedia.org/wiki/Symbian#History,
 * Symbian is no longer maintained since 2014-01-01.
 * Support for Symbian has been removed from CivetWeb
 */
#error "Symbian is no longer maintained. CivetWeb no longer supports Symbian."
#endif /* __SYMBIAN32__ */

#if defined(__rtems__)
#include <rtems/version.h>
#endif

#if defined(__ZEPHYR__)
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include <zephyr/kernel.h>

/* Max worker threads is the max of pthreads minus the main application thread
 * and minus the main civetweb thread, thus -2
 */
#define MAX_WORKER_THREADS (CONFIG_MAX_PTHREAD_COUNT - 2)

#if defined(USE_STACK_SIZE) && (USE_STACK_SIZE > 1)
#define ZEPHYR_STACK_SIZE USE_STACK_SIZE
#else
#define ZEPHYR_STACK_SIZE (1024 * 16)
#endif

K_THREAD_STACK_DEFINE(civetweb_main_stack, ZEPHYR_STACK_SIZE);
K_THREAD_STACK_ARRAY_DEFINE(civetweb_worker_stacks,
                            MAX_WORKER_THREADS,
                            ZEPHYR_STACK_SIZE);

static int zephyr_worker_stack_index;

#endif

#if !defined(CIVETWEB_HEADER_INCLUDED)
/* Include the header file here, so the CivetWeb interface is defined for the
 * entire implementation, including the following forward definitions. */
#include "civetweb.h"
#endif

#if !defined(DEBUG_TRACE)
#if defined(DEBUG)
static void DEBUG_TRACE_FUNC(const char *func,
                             unsigned line,
                             PRINTF_FORMAT_STRING(const char *fmt),
                             ...) PRINTF_ARGS(3, 4);

#define DEBUG_TRACE(fmt, ...)                                                  \
	DEBUG_TRACE_FUNC(__func__, __LINE__, fmt, __VA_ARGS__)

#define NEED_DEBUG_TRACE_FUNC
#if !defined(DEBUG_TRACE_STREAM)
#define DEBUG_TRACE_STREAM stdout
#endif

#else
#define DEBUG_TRACE(fmt, ...)                                                  \
	do {                                                                       \
	} while (0)
#endif /* DEBUG */
#endif /* DEBUG_TRACE */


#if !defined(DEBUG_ASSERT)
#if defined(DEBUG)
#include <stdlib.h>
#define DEBUG_ASSERT(cond)                                                     \
	do {                                                                       \
		if (!(cond)) {                                                         \
			DEBUG_TRACE("ASSERTION FAILED: %s", #cond);                        \
			exit(2); /* Exit with error */                                     \
		}                                                                      \
	} while (0)
#else
#define DEBUG_ASSERT(cond)
#endif /* DEBUG */
#endif


#if defined(__GNUC__) && defined(GCC_INSTRUMENTATION)
void __cyg_profile_func_enter(void *this_fn, void *call_site)
    __attribute__((no_instrument_function));

void __cyg_profile_func_exit(void *this_fn, void *call_site)
    __attribute__((no_instrument_function));

void
__cyg_profile_func_enter(void *this_fn, void *call_site)
{
	if ((void *)this_fn != (void *)printf) {
		printf("E %p %p\n", this_fn, call_site);
	}
}

void
__cyg_profile_func_exit(void *this_fn, void *call_site)
{
	if ((void *)this_fn != (void *)printf) {
		printf("X %p %p\n", this_fn, call_site);
	}
}
#endif


#if !defined(IGNORE_UNUSED_RESULT)
#define IGNORE_UNUSED_RESULT(a) ((void)((a) && 1))
#endif


#if defined(__GNUC__) || defined(__MINGW32__)

/* GCC unused function attribute seems fundamentally broken.
 * Several attempts to tell the compiler "THIS FUNCTION MAY BE USED
 * OR UNUSED" for individual functions failed.
 * Either the compiler creates an "unused-function" warning if a
 * function is not marked with __attribute__((unused)).
 * On the other hand, if the function is marked with this attribute,
 * but is used, the compiler raises a completely idiotic
 * "used-but-marked-unused" warning - and
 *   #pragma GCC diagnostic ignored "-Wused-but-marked-unused"
 * raises error: unknown option after "#pragma GCC diagnostic".
 * Disable this warning completely, until the GCC guys sober up
 * again.
 */

#pragma GCC diagnostic ignored "-Wunused-function"

#define FUNCTION_MAY_BE_UNUSED /* __attribute__((unused)) */

#else
#define FUNCTION_MAY_BE_UNUSED
#endif


/* Some ANSI #includes are not available on Windows CE and Zephyr */
#if !defined(_WIN32_WCE) && !defined(__ZEPHYR__)
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif /* !_WIN32_WCE */


#if defined(__clang__)
/* When using -Weverything, clang does not accept it's own headers
 * in a release build configuration. Disable what is too much in
 * -Weverything. */
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif

#if defined(__GNUC__) || defined(__MINGW32__)
/* Who on earth came to the conclusion, using __DATE__ should rise
 * an "expansion of date or time macro is not reproducible"
 * warning. That's exactly what was intended by using this macro.
 * Just disable this nonsense warning. */

/* And disabling them does not work either:
 * #pragma clang diagnostic ignored "-Wno-error=date-time"
 * #pragma clang diagnostic ignored "-Wdate-time"
 * So we just have to disable ALL warnings for some lines
 * of code.
 * This seems to be a known GCC bug, not resolved since 2012:
 * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431
 */
#endif


#if defined(__MACH__) && defined(__APPLE__) /* Apple OSX section */

#if defined(__clang__)
#if (__clang_major__ == 3) && ((__clang_minor__ == 7) || (__clang_minor__ == 8))
/* Avoid warnings for Xcode 7. It seems it does no longer exist in Xcode 8 */
#pragma clang diagnostic ignored "-Wno-reserved-id-macro"
#pragma clang diagnostic ignored "-Wno-keyword-macro"
#endif
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC (1)
#endif
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME (2)
#endif

#include <mach/clock.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/errno.h>
#include <sys/time.h>

/* clock_gettime is not implemented on OSX prior to 10.12 */
static int
_civet_clock_gettime(int clk_id, struct timespec *t)
{
	memset(t, 0, sizeof(*t));
	if (clk_id == CLOCK_REALTIME) {
		struct timeval now;
		int rv = gettimeofday(&now, NULL);
		if (rv) {
			return rv;
		}
		t->tv_sec = now.tv_sec;
		t->tv_nsec = now.tv_usec * 1000;
		return 0;

	} else if (clk_id == CLOCK_MONOTONIC) {
		static uint64_t clock_start_time = 0;
		static mach_timebase_info_data_t timebase_ifo = {0, 0};

		uint64_t now = mach_absolute_time();

		if (clock_start_time == 0) {
			kern_return_t mach_status = mach_timebase_info(&timebase_ifo);
			DEBUG_ASSERT(mach_status == KERN_SUCCESS);

			/* appease "unused variable" warning for release builds */
			(void)mach_status;

			clock_start_time = now;
		}

		now = (uint64_t)((double)(now - clock_start_time)
		                 * (double)timebase_ifo.numer
		                 / (double)timebase_ifo.denom);

		t->tv_sec = now / 1000000000;
		t->tv_nsec = now % 1000000000;
		return 0;
	}
	return -1; /* EINVAL - Clock ID is unknown */
}

/* if clock_gettime is declared, then __CLOCK_AVAILABILITY will be defined */
#if defined(__CLOCK_AVAILABILITY)
/* If we compiled with Mac OSX 10.12 or later, then clock_gettime will be
 * declared but it may be NULL at runtime. So we need to check before using
 * it. */
static int
_civet_safe_clock_gettime(int clk_id, struct timespec *t)
{
	if (clock_gettime) {
		return clock_gettime(clk_id, t);
	}
	return _civet_clock_gettime(clk_id, t);
}
#define clock_gettime _civet_safe_clock_gettime
#else
#define clock_gettime _civet_clock_gettime
#endif

#endif


#if defined(_WIN32)
#define ERROR_TRY_AGAIN(err) ((err) == WSAEWOULDBLOCK)
#else
/* Unix might return different error codes indicating to try again.
 * For Linux EAGAIN==EWOULDBLOCK, maybe EAGAIN!=EWOULDBLOCK is history from
 * decades ago, but better check both and let the compiler optimize it. */
#define ERROR_TRY_AGAIN(err)                                                   \
	(((err) == EAGAIN) || ((err) == EWOULDBLOCK) || ((err) == EINTR))
#endif

#if defined(USE_ZLIB)
#include "zconf.h"
#include "zlib.h"
#endif


/********************************************************************/
/* CivetWeb configuration defines */
/********************************************************************/

/* Maximum number of threads that can be configured.
 * The number of threads actually created depends on the "num_threads"
 * configuration parameter, but this is the upper limit. */
#if !defined(MAX_WORKER_THREADS)
#define MAX_WORKER_THREADS (1024 * 64) /* in threads (count) */
#endif

/* Timeout interval for select/poll calls.
 * The timeouts depend on "*_timeout_ms" configuration values, but long
 * timeouts are split into timouts as small as SOCKET_TIMEOUT_QUANTUM.
 * This reduces the time required to stop the server. */
#if !defined(SOCKET_TIMEOUT_QUANTUM)
#define SOCKET_TIMEOUT_QUANTUM (2000) /* in ms */
#endif

/* Do not try to compress files smaller than this limit. */
#if !defined(MG_FILE_COMPRESSION_SIZE_LIMIT)
#define MG_FILE_COMPRESSION_SIZE_LIMIT (1024) /* in bytes */
#endif

#if !defined(PASSWORDS_FILE_NAME)
#define PASSWORDS_FILE_NAME ".htpasswd"
#endif

/* Initial buffer size for all CGI environment variables. In case there is
 * not enough space, another block is allocated. */
#if !defined(CGI_ENVIRONMENT_SIZE)
#define CGI_ENVIRONMENT_SIZE (4096) /* in bytes */
#endif

/* Maximum number of environment variables. */
#if !defined(MAX_CGI_ENVIR_VARS)
#define MAX_CGI_ENVIR_VARS (256) /* in variables (count) */
#endif

/* General purpose buffer size. */
#if !defined(MG_BUF_LEN) /* in bytes */
#define MG_BUF_LEN (1024 * 8)
#endif


/********************************************************************/

/* Helper macros */
#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

#include <stdint.h>

/* Standard defines */
#if !defined(INT64_MAX)
#define INT64_MAX (9223372036854775807)
#endif

#define SHUTDOWN_RD (0)
#define SHUTDOWN_WR (1)
#define SHUTDOWN_BOTH (2)

mg_static_assert(MAX_WORKER_THREADS >= 1,
                 "worker threads must be a positive number");

mg_static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8,
                 "size_t data type size check");


#if defined(_WIN32) /* WINDOWS include block */
#include <malloc.h> /* *alloc( */
#include <stdlib.h> /* *alloc( */
#include <time.h>   /* struct timespec */
#include <windows.h>
#include <winsock2.h> /* DTL add for SO_EXCLUSIVE */
#include <ws2tcpip.h>

typedef const char *SOCK_OPT_TYPE;

/* For a detailed description of these *_PATH_MAX defines, see
 * https://github.com/civetweb/civetweb/issues/937. */

/* UTF8_PATH_MAX is a char buffer size for 259 BMP characters in UTF-8 plus
 * null termination, rounded up to the next 4 bytes boundary */
#define UTF8_PATH_MAX (3 * 260)
/* UTF16_PATH_MAX is the 16-bit wchar_t buffer size required for 259 BMP
 * characters plus termination. (Note: wchar_t is 16 bit on Windows) */
#define UTF16_PATH_MAX (260)

#if !defined(_IN_PORT_T)
#if !defined(in_port_t)
#define in_port_t u_short
#endif
#endif

#if defined(_WIN32_WCE)
#error "WinCE support has ended"
#endif

#include <direct.h>
#include <io.h>
#include <process.h>


#define MAKEUQUAD(lo, hi)                                                      \
	((uint64_t)(((uint32_t)(lo)) | ((uint64_t)((uint32_t)(hi))) << 32))
#define RATE_DIFF (10000000) /* 100 nsecs */
#define EPOCH_DIFF (MAKEUQUAD(0xd53e8000, 0x019db1de))
#define SYS2UNIX_TIME(lo, hi)                                                  \
	((time_t)((MAKEUQUAD((lo), (hi)) - EPOCH_DIFF) / RATE_DIFF))

/* Visual Studio 6 does not know __func__ or __FUNCTION__
 * The rest of MS compilers use __FUNCTION__, not C99 __func__
 * Also use _strtoui64 on modern M$ compilers */
#if defined(_MSC_VER)
#if (_MSC_VER < 1300)
#define STRX(x) #x
#define STR(x) STRX(x)
#define __func__ __FILE__ ":" STR(__LINE__)
#define strtoull(x, y, z) ((unsigned __int64)_atoi64(x))
#define strtoll(x, y, z) (_atoi64(x))
#else
#define __func__ __FUNCTION__
#define strtoull(x, y, z) (_strtoui64(x, y, z))
#define strtoll(x, y, z) (_strtoi64(x, y, z))
#endif
#endif /* _MSC_VER */


#define ERRNO ((int)(GetLastError()))
#define NO_SOCKLEN_T


#if defined(_WIN64) || defined(__MINGW64__)
#if !defined(SSL_LIB)

#if defined(OPENSSL_API_3_0)
#define SSL_LIB "libssl-3-x64.dll"
#define CRYPTO_LIB "libcrypto-3-x64.dll"
#endif

#if defined(OPENSSL_API_1_1)
#define SSL_LIB "libssl-1_1-x64.dll"
#define CRYPTO_LIB "libcrypto-1_1-x64.dll"
#endif /* OPENSSL_API_1_1 */

#if defined(OPENSSL_API_1_0)
#define SSL_LIB "ssleay64.dll"
#define CRYPTO_LIB "libeay64.dll"
#endif /* OPENSSL_API_1_0 */

#endif
#else /* defined(_WIN64) || defined(__MINGW64__) */
#if !defined(SSL_LIB)

#if defined(OPENSSL_API_3_0)
#define SSL_LIB "libssl-3.dll"
#define CRYPTO_LIB "libcrypto-3.dll"
#endif

#if defined(OPENSSL_API_1_1)
#define SSL_LIB "libssl-1_1.dll"
#define CRYPTO_LIB "libcrypto-1_1.dll"
#endif /* OPENSSL_API_1_1 */

#if defined(OPENSSL_API_1_0)
#define SSL_LIB "ssleay32.dll"
#define CRYPTO_LIB "libeay32.dll"
#endif /* OPENSSL_API_1_0 */

#endif /* SSL_LIB */
#endif /* defined(_WIN64) || defined(__MINGW64__) */


#define O_NONBLOCK (0)
#if !defined(W_OK)
#define W_OK (2) /* http://msdn.microsoft.com/en-us/library/1w06ktdy.aspx */
#endif
#define _POSIX_
#define INT64_FMT "I64d"
#define UINT64_FMT "I64u"

#define WINCDECL __cdecl
#define vsnprintf_impl _vsnprintf
#define access _access
#define mg_sleep(x) (Sleep(x))

#define pipe(x) _pipe(x, MG_BUF_LEN, _O_BINARY)
#if !defined(popen)
#define popen(x, y) (_popen(x, y))
#endif
#if !defined(pclose)
#define pclose(x) (_pclose(x))
#endif
#define close(x) (_close(x))
#define dlsym(x, y) (GetProcAddress((HINSTANCE)(x), (y)))
#define RTLD_LAZY (0)
#define fseeko(x, y, z) ((_lseeki64(_fileno(x), (y), (z)) == -1) ? -1 : 0)
#define fdopen(x, y) (_fdopen((x), (y)))
#define write(x, y, z) (_write((x), (y), (unsigned)z))
#define read(x, y, z) (_read((x), (y), (unsigned)z))
#define flockfile(x) ((void)pthread_mutex_lock(&global_log_file_lock))
#define funlockfile(x) ((void)pthread_mutex_unlock(&global_log_file_lock))
#define sleep(x) (Sleep((x)*1000))
#define rmdir(x) (_rmdir(x))
#if defined(_WIN64) || !defined(__MINGW32__)
/* Only MinGW 32 bit is missing this function */
#define timegm(x) (_mkgmtime(x))
#else
time_t timegm(struct tm *tm);
#define NEED_TIMEGM
#endif


#if !defined(fileno)
#define fileno(x) (_fileno(x))
#endif /* !fileno MINGW #defines fileno */

typedef struct {
	CRITICAL_SECTION sec; /* Immovable */
} pthread_mutex_t;
typedef DWORD pthread_key_t;
typedef HANDLE pthread_t;
typedef struct {
	pthread_mutex_t threadIdSec;
	struct mg_workerTLS *waiting_thread; /* The chain of threads */
} pthread_cond_t;

#if !defined(__clockid_t_defined)
typedef DWORD clockid_t;
#endif
#if !defined(CLOCK_MONOTONIC)
#define CLOCK_MONOTONIC (1)
#endif
#if !defined(CLOCK_REALTIME)
#define CLOCK_REALTIME (2)
#endif
#if !defined(CLOCK_THREAD)
#define CLOCK_THREAD (3)
#endif
#if !defined(CLOCK_PROCESS)
#define CLOCK_PROCESS (4)
#endif


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#define _TIMESPEC_DEFINED
#endif
#if !defined(_TIMESPEC_DEFINED)
struct timespec {
	time_t tv_sec; /* seconds */
	long tv_nsec;  /* nanoseconds */
};
#endif

#if !defined(WIN_PTHREADS_TIME_H)
#define MUST_IMPLEMENT_CLOCK_GETTIME
#endif

#if defined(MUST_IMPLEMENT_CLOCK_GETTIME)
#define clock_gettime mg_clock_gettime
static int
clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	FILETIME ft;
	ULARGE_INTEGER li, li2;
	BOOL ok = FALSE;
	double d;
	static double perfcnt_per_sec = 0.0;
	static BOOL initialized = FALSE;

	if (!initialized) {
		QueryPerformanceFrequency((LARGE_INTEGER *)&li);
		perfcnt_per_sec = 1.0 / li.QuadPart;
		initialized = TRUE;
	}

	if (tp) {
		memset(tp, 0, sizeof(*tp));

		if (clk_id == CLOCK_REALTIME) {

			/* BEGIN: CLOCK_REALTIME = wall clock (date and time) */
			GetSystemTimeAsFileTime(&ft);
			li.LowPart = ft.dwLowDateTime;
			li.HighPart = ft.dwHighDateTime;
			li.QuadPart -= 116444736000000000; /* 1.1.1970 in filedate */
			tp->tv_sec = (time_t)(li.QuadPart / 10000000);
			tp->tv_nsec = (long)(li.QuadPart % 10000000) * 100;
			ok = TRUE;
			/* END: CLOCK_REALTIME */

		} else if (clk_id == CLOCK_MONOTONIC) {

			/* BEGIN: CLOCK_MONOTONIC = stopwatch (time differences) */
			QueryPerformanceCounter((LARGE_INTEGER *)&li);
			d = li.QuadPart * perfcnt_per_sec;
			tp->tv_sec = (time_t)d;
			d -= (double)tp->tv_sec;
			tp->tv_nsec = (long)(d * 1.0E9);
			ok = TRUE;
			/* END: CLOCK_MONOTONIC */

		} else if (clk_id == CLOCK_THREAD) {

			/* BEGIN: CLOCK_THREAD = CPU usage of thread */
			FILETIME t_create, t_exit, t_kernel, t_user;
			if (GetThreadTimes(GetCurrentThread(),
			                   &t_create,
			                   &t_exit,
			                   &t_kernel,
			                   &t_user)) {
				li.LowPart = t_user.dwLowDateTime;
				li.HighPart = t_user.dwHighDateTime;
				li2.LowPart = t_kernel.dwLowDateTime;
				li2.HighPart = t_kernel.dwHighDateTime;
				li.QuadPart += li2.QuadPart;
				tp->tv_sec = (time_t)(li.QuadPart / 10000000);
				tp->tv_nsec = (long)(li.QuadPart % 10000000) * 100;
				ok = TRUE;
			}
			/* END: CLOCK_THREAD */

		} else if (clk_id == CLOCK_PROCESS) {

			/* BEGIN: CLOCK_PROCESS = CPU usage of process */
			FILETIME t_create, t_exit, t_kernel, t_user;
			if (GetProcessTimes(GetCurrentProcess(),
			                    &t_create,
			                    &t_exit,
			                    &t_kernel,
			                    &t_user)) {
				li.LowPart = t_user.dwLowDateTime;
				li.HighPart = t_user.dwHighDateTime;
				li2.LowPart = t_kernel.dwLowDateTime;
				li2.HighPart = t_kernel.dwHighDateTime;
				li.QuadPart += li2.QuadPart;
				tp->tv_sec = (time_t)(li.QuadPart / 10000000);
				tp->tv_nsec = (long)(li.QuadPart % 10000000) * 100;
				ok = TRUE;
			}
			/* END: CLOCK_PROCESS */

		} else {

			/* BEGIN: unknown clock */
			/* ok = FALSE; already set by init */
			/* END: unknown clock */
		}
	}

	return ok ? 0 : -1;
}
#endif


#define pid_t HANDLE /* MINGW typedefs pid_t to int. Using #define here. */

static int pthread_mutex_lock(pthread_mutex_t *);
static int pthread_mutex_unlock(pthread_mutex_t *);
static void path_to_unicode(const struct mg_connection *conn,
                            const char *path,
                            wchar_t *wbuf,
                            size_t wbuf_len);

/* All file operations need to be rewritten to solve #246. */

struct mg_file;

static const char *mg_fgets(char *buf, size_t size, struct mg_file *filep);


/* POSIX dirent interface */
struct dirent {
	char d_name[UTF8_PATH_MAX];
};

typedef struct DIR {
	HANDLE handle;
	WIN32_FIND_DATAW info;
	struct dirent result;
} DIR;

#if defined(HAVE_POLL)
#define mg_pollfd pollfd
#else
struct mg_pollfd {
	SOCKET fd;
	short events;
	short revents;
};
#endif

/* Mark required libraries */
#if defined(_MSC_VER)
#pragma comment(lib, "Ws2_32.lib")
#endif

#else /* defined(_WIN32) - WINDOWS vs UNIX include block */

#include <inttypes.h>

/* Linux & co. internally use UTF8 */
#define UTF8_PATH_MAX (PATH_MAX)

typedef const void *SOCK_OPT_TYPE;

#if defined(ANDROID)
typedef unsigned short int in_port_t;
#endif

#if !defined(__ZEPHYR__)
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <grp.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <pthread.h>
#include <pwd.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#if !defined(__rtems__)
#include <sys/utsname.h>
#endif
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#if defined(USE_X_DOM_SOCKET)
#include <sys/un.h>
#endif
#endif

#define vsnprintf_impl vsnprintf

#if !defined(NO_SSL_DL) && !defined(NO_SSL)
#include <dlfcn.h>
#endif

#if defined(__MACH__) && defined(__APPLE__)

#if defined(OPENSSL_API_3_0)
#define SSL_LIB "libssl.3.dylib"
#define CRYPTO_LIB "libcrypto.3.dylib"
#endif

#if defined(OPENSSL_API_1_1)
#define SSL_LIB "libssl.1.1.dylib"
#define CRYPTO_LIB "libcrypto.1.1.dylib"
#endif /* OPENSSL_API_1_1 */

#if defined(OPENSSL_API_1_0)
#define SSL_LIB "libssl.1.0.dylib"
#define CRYPTO_LIB "libcrypto.1.0.dylib"
#endif /* OPENSSL_API_1_0 */

#else
#if !defined(SSL_LIB)
#define SSL_LIB "libssl.so"
#endif
#if !defined(CRYPTO_LIB)
#define CRYPTO_LIB "libcrypto.so"
#endif
#endif
#if !defined(O_BINARY)
#define O_BINARY (0)
#endif /* O_BINARY */
#define closesocket(a) (close(a))
#define mg_mkdir(conn, path, mode) (mkdir(path, mode))
#define mg_remove(conn, x) (remove(x))
#define mg_sleep(x) (usleep((x)*1000))
#define mg_opendir(conn, x) (opendir(x))
#define mg_closedir(x) (closedir(x))
#define mg_readdir(x) (readdir(x))
#define ERRNO (errno)
#define INVALID_SOCKET (-1)
#define INT64_FMT PRId64
#define UINT64_FMT PRIu64
typedef int SOCKET;
#define WINCDECL

#if defined(__hpux)
/* HPUX 11 does not have monotonic, fall back to realtime */
#if !defined(CLOCK_MONOTONIC)
#define CLOCK_MONOTONIC CLOCK_REALTIME
#endif

/* HPUX defines socklen_t incorrectly as size_t which is 64bit on
 * Itanium.  Without defining _XOPEN_SOURCE or _XOPEN_SOURCE_EXTENDED
 * the prototypes use int* rather than socklen_t* which matches the
 * actual library expectation.  When called with the wrong size arg
 * accept() returns a zero client inet addr and check_acl() always
 * fails.  Since socklen_t is widely used below, just force replace
 * their typedef with int. - DTL
 */
#define socklen_t int
#endif /* hpux */

#define mg_pollfd pollfd

#endif /* defined(_WIN32) - WINDOWS vs UNIX include block */

/* In case our C library is missing "timegm", provide an implementation */
#if defined(NEED_TIMEGM)
static inline int
is_leap(int y)
{
	return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

static inline int
count_leap(int y)
{
	return (y - 1969) / 4 - (y - 1901) / 100 + (y - 1601) / 400;
}

static time_t
timegm(struct tm *tm)
{
	static const unsigned short ydays[] = {
	    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
	int year = tm->tm_year + 1900;
	int mon = tm->tm_mon;
	int mday = tm->tm_mday - 1;
	int hour = tm->tm_hour;
	int min = tm->tm_min;
	int sec = tm->tm_sec;

	if (year < 1970 || mon < 0 || mon > 11 || mday < 0
	    || (mday >= ydays[mon + 1] - ydays[mon]
	                    + (mon == 1 && is_leap(year) ? 1 : 0))
	    || hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 60)
		return -1;

	time_t res = year - 1970;
	res *= 365;
	res += mday;
	res += ydays[mon] + (mon > 1 && is_leap(year) ? 1 : 0);
	res += count_leap(year);

	res *= 24;
	res += hour;
	res *= 60;
	res += min;
	res *= 60;
	res += sec;
	return res;
}
#endif /* NEED_TIMEGM */


/* va_copy should always be a macro, C99 and C++11 - DTL */
#if !defined(va_copy)
#define va_copy(x, y) ((x) = (y))
#endif


#if defined(_WIN32)
/* Create substitutes for POSIX functions in Win32. */

#if defined(GCC_DIAGNOSTIC)
/* Show no warning in case system functions are not used. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif


static pthread_mutex_t global_log_file_lock;

FUNCTION_MAY_BE_UNUSED
static DWORD
pthread_self(void)
{
	return GetCurrentThreadId();
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_key_create(
    pthread_key_t *key,
    void (*_ignored)(void *) /* destructor not supported for Windows */
)
{
	(void)_ignored;

	if ((key != 0)) {
		*key = TlsAlloc();
		return (*key != TLS_OUT_OF_INDEXES) ? 0 : -1;
	}
	return -2;
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_key_delete(pthread_key_t key)
{
	return TlsFree(key) ? 0 : 1;
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_setspecific(pthread_key_t key, void *value)
{
	return TlsSetValue(key, value) ? 0 : 1;
}


FUNCTION_MAY_BE_UNUSED
static void *
pthread_getspecific(pthread_key_t key)
{
	return TlsGetValue(key);
}

#if defined(GCC_DIAGNOSTIC)
/* Enable unused function warning again */
#pragma GCC diagnostic pop
#endif

static struct pthread_mutex_undefined_struct *pthread_mutex_attr = NULL;
#else
static pthread_mutexattr_t pthread_mutex_attr;
#endif /* _WIN32 */


#if defined(GCC_DIAGNOSTIC)
/* Show no warning in case system functions are not used. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif /* defined(GCC_DIAGNOSTIC) */
#if defined(__clang__)
/* Show no warning in case system functions are not used. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

static pthread_mutex_t global_lock_mutex;


FUNCTION_MAY_BE_UNUSED
static void
mg_global_lock(void)
{
	(void)pthread_mutex_lock(&global_lock_mutex);
}


FUNCTION_MAY_BE_UNUSED
static void
mg_global_unlock(void)
{
	(void)pthread_mutex_unlock(&global_lock_mutex);
}


#if defined(_WIN64)
mg_static_assert(SIZE_MAX == 0xFFFFFFFFFFFFFFFFu, "Mismatch for atomic types");
#elif defined(_WIN32)
mg_static_assert(SIZE_MAX == 0xFFFFFFFFu, "Mismatch for atomic types");
#endif


/* Atomic functions working on ptrdiff_t ("signed size_t").
 * Operations: Increment, Decrement, Add, Maximum.
 * Up to size_t, they do not an atomic "load" operation.
 */
FUNCTION_MAY_BE_UNUSED
static ptrdiff_t
mg_atomic_inc(volatile ptrdiff_t *addr)
{
	ptrdiff_t ret;

#if defined(_WIN64) && !defined(NO_ATOMICS)
	ret = InterlockedIncrement64(addr);
#elif defined(_WIN32) && !defined(NO_ATOMICS)
#ifdef __cplusplus
	/* For C++ the Microsoft Visual Studio compiler can not decide what
	 * overloaded function prototpye in the SDC corresponds to "ptrdiff_t". */
	static_assert(sizeof(ptrdiff_t) == sizeof(LONG), "Size mismatch");
	static_assert(sizeof(ptrdiff_t) == sizeof(int32_t), "Size mismatch");
	ret = InterlockedIncrement((LONG *)addr);
#else
	ret = InterlockedIncrement(addr);
#endif
#elif defined(__GNUC__)                                                        \
    && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 0)))           \
    && !defined(NO_ATOMICS)
	ret = __sync_add_and_fetch(addr, 1);
#else
	mg_global_lock();
	ret = (++(*addr));
	mg_global_unlock();
#endif
	return ret;
}


FUNCTION_MAY_BE_UNUSED
static ptrdiff_t
mg_atomic_dec(volatile ptrdiff_t *addr)
{
	ptrdiff_t ret;

#if defined(_WIN64) && !defined(NO_ATOMICS)
	ret = InterlockedDecrement64(addr);
#elif defined(_WIN32) && !defined(NO_ATOMICS)
#ifdef __cplusplus
	/* see mg_atomic_inc */
	static_assert(sizeof(ptrdiff_t) == sizeof(LONG), "Size mismatch");
	static_assert(sizeof(ptrdiff_t) == sizeof(int32_t), "Size mismatch");
	ret = InterlockedDecrement((LONG *)addr);
#else
	ret = InterlockedDecrement(addr);
#endif
#elif defined(__GNUC__)                                                        \
    && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 0)))           \
    && !defined(NO_ATOMICS)
	ret = __sync_sub_and_fetch(addr, 1);
#else
	mg_global_lock();
	ret = (--(*addr));
	mg_global_unlock();
#endif
	return ret;
}


#if defined(USE_SERVER_STATS) || defined(STOP_FLAG_NEEDS_LOCK)
static ptrdiff_t
mg_atomic_add(volatile ptrdiff_t *addr, ptrdiff_t value)
{
	ptrdiff_t ret;

#if defined(_WIN64) && !defined(NO_ATOMICS)
	ret = InterlockedAdd64(addr, value);
#elif defined(_WIN32) && !defined(NO_ATOMICS)
	ret = InterlockedExchangeAdd(addr, value) + value;
#elif defined(__GNUC__)                                                        \
    && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 0)))           \
    && !defined(NO_ATOMICS)
	ret = __sync_add_and_fetch(addr, value);
#else
	mg_global_lock();
	*addr += value;
	ret = (*addr);
	mg_global_unlock();
#endif
	return ret;
}


FUNCTION_MAY_BE_UNUSED
static ptrdiff_t
mg_atomic_compare_and_swap(volatile ptrdiff_t *addr,
                           ptrdiff_t oldval,
                           ptrdiff_t newval)
{
	ptrdiff_t ret;

#if defined(_WIN64) && !defined(NO_ATOMICS)
	ret = InterlockedCompareExchange64(addr, newval, oldval);
#elif defined(_WIN32) && !defined(NO_ATOMICS)
	ret = InterlockedCompareExchange(addr, newval, oldval);
#elif defined(__GNUC__)                                                        \
    && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 0)))           \
    && !defined(NO_ATOMICS)
	ret = __sync_val_compare_and_swap(addr, oldval, newval);
#else
	mg_global_lock();
	ret = *addr;
	if ((ret != newval) && (ret == oldval)) {
		*addr = newval;
	}
	mg_global_unlock();
#endif
	return ret;
}


static void
mg_atomic_max(volatile ptrdiff_t *addr, ptrdiff_t value)
{
	register ptrdiff_t tmp = *addr;

#if defined(_WIN64) && !defined(NO_ATOMICS)
	while (tmp < value) {
		tmp = InterlockedCompareExchange64(addr, value, tmp);
	}
#elif defined(_WIN32) && !defined(NO_ATOMICS)
	while (tmp < value) {
		tmp = InterlockedCompareExchange(addr, value, tmp);
	}
#elif defined(__GNUC__)                                                        \
    && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 0)))           \
    && !defined(NO_ATOMICS)
	while (tmp < value) {
		tmp = __sync_val_compare_and_swap(addr, tmp, value);
	}
#else
	mg_global_lock();
	if (*addr < value) {
		*addr = value;
	}
	mg_global_unlock();
#endif
}


static int64_t
mg_atomic_add64(volatile int64_t *addr, int64_t value)
{
	int64_t ret;

#if defined(_WIN64) && !defined(NO_ATOMICS)
	ret = InterlockedAdd64(addr, value);
#elif defined(_WIN32) && !defined(NO_ATOMICS)
	ret = InterlockedExchangeAdd64(addr, value) + value;
#elif defined(__GNUC__)                                                        \
    && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 0)))           \
    && !defined(NO_ATOMICS)
	ret = __sync_add_and_fetch(addr, value);
#else
	mg_global_lock();
	*addr += value;
	ret = (*addr);
	mg_global_unlock();
#endif
	return ret;
}
#endif


#if defined(GCC_DIAGNOSTIC)
/* Show no warning in case system functions are not used. */
#pragma GCC diagnostic pop
#endif /* defined(GCC_DIAGNOSTIC) */
#if defined(__clang__)
/* Show no warning in case system functions are not used. */
#pragma clang diagnostic pop
#endif


#if defined(USE_SERVER_STATS)

struct mg_memory_stat {
	volatile ptrdiff_t totalMemUsed;
	volatile ptrdiff_t maxMemUsed;
	volatile ptrdiff_t blockCount;
};


static struct mg_memory_stat *get_memory_stat(struct mg_context *ctx);


static void *
mg_malloc_ex(size_t size,
             struct mg_context *ctx,
             const char *file,
             unsigned line)
{
	void *data = malloc(size + 2 * sizeof(uintptr_t));
	void *memory = 0;
	struct mg_memory_stat *mstat = get_memory_stat(ctx);

#if defined(MEMORY_DEBUGGING)
	char mallocStr[256];
#else
	(void)file;
	(void)line;
#endif

	if (data) {
		uintptr_t *tmp = (uintptr_t *)data;
		ptrdiff_t mmem = mg_atomic_add(&mstat->totalMemUsed, (ptrdiff_t)size);
		mg_atomic_max(&mstat->maxMemUsed, mmem);
		mg_atomic_inc(&mstat->blockCount);
		tmp[0] = size;
		tmp[1] = (uintptr_t)mstat;
		memory = (void *)&tmp[2];
	}

#if defined(MEMORY_DEBUGGING)
	sprintf(mallocStr,
	        "MEM: %p %5lu alloc   %7lu %4lu --- %s:%u\n",
	        memory,
	        (unsigned long)size,
	        (unsigned long)mstat->totalMemUsed,
	        (unsigned long)mstat->blockCount,
	        file,
	        line);
	DEBUG_TRACE("%s", mallocStr);
#endif

	return memory;
}


static void *
mg_calloc_ex(size_t count,
             size_t size,
             struct mg_context *ctx,
             const char *file,
             unsigned line)
{
	void *data = mg_malloc_ex(size * count, ctx, file, line);

	if (data) {
		memset(data, 0, size * count);
	}
	return data;
}


static void
mg_free_ex(void *memory, const char *file, unsigned line)
{
#if defined(MEMORY_DEBUGGING)
	char mallocStr[256];
#else
	(void)file;
	(void)line;
#endif

	if (memory) {
		void *data = (void *)(((char *)memory) - 2 * sizeof(uintptr_t));
		uintptr_t size = ((uintptr_t *)data)[0];
		struct mg_memory_stat *mstat =
		    (struct mg_memory_stat *)(((uintptr_t *)data)[1]);
		mg_atomic_add(&mstat->totalMemUsed, -(ptrdiff_t)size);
		mg_atomic_dec(&mstat->blockCount);

#if defined(MEMORY_DEBUGGING)
		sprintf(mallocStr,
		        "MEM: %p %5lu free    %7lu %4lu --- %s:%u\n",
		        memory,
		        (unsigned long)size,
		        (unsigned long)mstat->totalMemUsed,
		        (unsigned long)mstat->blockCount,
		        file,
		        line);
		DEBUG_TRACE("%s", mallocStr);
#endif
		free(data);
	}
}


static void *
mg_realloc_ex(void *memory,
              size_t newsize,
              struct mg_context *ctx,
              const char *file,
              unsigned line)
{
	void *data;
	void *_realloc;
	uintptr_t oldsize;

#if defined(MEMORY_DEBUGGING)
	char mallocStr[256];
#else
	(void)file;
	(void)line;
#endif

	if (newsize) {
		if (memory) {
			/* Reallocate existing block */
			struct mg_memory_stat *mstat;
			data = (void *)(((char *)memory) - 2 * sizeof(uintptr_t));
			oldsize = ((uintptr_t *)data)[0];
			mstat = (struct mg_memory_stat *)((uintptr_t *)data)[1];
			_realloc = realloc(data, newsize + 2 * sizeof(uintptr_t));
			if (_realloc) {
				data = _realloc;
				mg_atomic_add(&mstat->totalMemUsed, -(ptrdiff_t)oldsize);
#if defined(MEMORY_DEBUGGING)
				sprintf(mallocStr,
				        "MEM: %p %5lu r-free  %7lu %4lu --- %s:%u\n",
				        memory,
				        (unsigned long)oldsize,
				        (unsigned long)mstat->totalMemUsed,
				        (unsigned long)mstat->blockCount,
				        file,
				        line);
				DEBUG_TRACE("%s", mallocStr);
#endif
				mg_atomic_add(&mstat->totalMemUsed, (ptrdiff_t)newsize);

#if defined(MEMORY_DEBUGGING)
				sprintf(mallocStr,
				        "MEM: %p %5lu r-alloc %7lu %4lu --- %s:%u\n",
				        memory,
				        (unsigned long)newsize,
				        (unsigned long)mstat->totalMemUsed,
				        (unsigned long)mstat->blockCount,
				        file,
				        line);
				DEBUG_TRACE("%s", mallocStr);
#endif
				*(uintptr_t *)data = newsize;
				data = (void *)(((char *)data) + 2 * sizeof(uintptr_t));
			} else {
#if defined(MEMORY_DEBUGGING)
				DEBUG_TRACE("%s", "MEM: realloc failed\n");
#endif
				return _realloc;
			}
		} else {
			/* Allocate new block */
			data = mg_malloc_ex(newsize, ctx, file, line);
		}
	} else {
		/* Free existing block */
		data = 0;
		mg_free_ex(memory, file, line);
	}

	return data;
}


#define mg_malloc(a) mg_malloc_ex(a, NULL, __FILE__, __LINE__)
#define mg_calloc(a, b) mg_calloc_ex(a, b, NULL, __FILE__, __LINE__)
#define mg_realloc(a, b) mg_realloc_ex(a, b, NULL, __FILE__, __LINE__)
#define mg_free(a) mg_free_ex(a, __FILE__, __LINE__)

#define mg_malloc_ctx(a, c) mg_malloc_ex(a, c, __FILE__, __LINE__)
#define mg_calloc_ctx(a, b, c) mg_calloc_ex(a, b, c, __FILE__, __LINE__)
#define mg_realloc_ctx(a, b, c) mg_realloc_ex(a, b, c, __FILE__, __LINE__)


#else /* USE_SERVER_STATS */


static __inline void *
mg_malloc(size_t a)
{
	return malloc(a);
}

static __inline void *
mg_calloc(size_t a, size_t b)
{
	return calloc(a, b);
}

static __inline void *
mg_realloc(void *a, size_t b)
{
	return realloc(a, b);
}

static __inline void
mg_free(void *a)
{
	free(a);
}

#define mg_malloc_ctx(a, c) mg_malloc(a)
#define mg_calloc_ctx(a, b, c) mg_calloc(a, b)
#define mg_realloc_ctx(a, b, c) mg_realloc(a, b)
#define mg_free_ctx(a, c) mg_free(a)

#endif /* USE_SERVER_STATS */


static void mg_vsnprintf(const struct mg_connection *conn,
                         int *truncated,
                         char *buf,
                         size_t buflen,
                         const char *fmt,
                         va_list ap);

static void mg_snprintf(const struct mg_connection *conn,
                        int *truncated,
                        char *buf,
                        size_t buflen,
                        PRINTF_FORMAT_STRING(const char *fmt),
                        ...) PRINTF_ARGS(5, 6);

/* This following lines are just meant as a reminder to use the mg-functions
 * for memory management */
#if defined(malloc)
#undef malloc
#endif
#if defined(calloc)
#undef calloc
#endif
#if defined(realloc)
#undef realloc
#endif
#if defined(free)
#undef free
#endif
#if defined(snprintf)
#undef snprintf
#endif
#if defined(vsnprintf)
#undef vsnprintf
#endif
#if !defined(NDEBUG)
#define malloc DO_NOT_USE_THIS_FUNCTION__USE_mg_malloc
#define calloc DO_NOT_USE_THIS_FUNCTION__USE_mg_calloc
#define realloc DO_NOT_USE_THIS_FUNCTION__USE_mg_realloc
#define free DO_NOT_USE_THIS_FUNCTION__USE_mg_free
#define snprintf DO_NOT_USE_THIS_FUNCTION__USE_mg_snprintf
#endif
#if defined(_WIN32)
/* vsnprintf must not be used in any system,
 * but this define only works well for Windows. */
#define vsnprintf DO_NOT_USE_THIS_FUNCTION__USE_mg_vsnprintf
#endif


/* mg_init_library counter */
static int mg_init_library_called = 0;

#if !defined(NO_SSL)
#if defined(OPENSSL_API_1_0) || defined(OPENSSL_API_1_1)                       \
    || defined(OPENSSL_API_3_0)
static int mg_openssl_initialized = 0;
#endif
#if !defined(OPENSSL_API_1_0) && !defined(OPENSSL_API_1_1)                     \
    && !defined(OPENSSL_API_3_0) && !defined(USE_MBEDTLS)                      \
	&& !defined(USE_GNUTLS)
#error "Please define OPENSSL_API_#_# or USE_MBEDTLS or USE_GNUTLS"
#endif
#if defined(OPENSSL_API_1_0) && defined(OPENSSL_API_1_1)
#error "Multiple OPENSSL_API versions defined"
#endif
#if defined(OPENSSL_API_1_1) && defined(OPENSSL_API_3_0)
#error "Multiple OPENSSL_API versions defined"
#endif
#if defined(OPENSSL_API_1_0) && defined(OPENSSL_API_3_0)
#error "Multiple OPENSSL_API versions defined"
#endif
#if (defined(OPENSSL_API_1_0) || defined(OPENSSL_API_1_1)                      \
     || defined(OPENSSL_API_3_0))                                              \
    && (defined(USE_MBEDTLS) || defined(USE_GNUTLS))
#error "Multiple SSL libraries defined"
#endif
#if defined(USE_MBEDTLS) && defined(USE_GNUTLS)
#error "Multiple SSL libraries defined"
#endif
#endif


static pthread_key_t sTlsKey; /* Thread local storage index */
static volatile ptrdiff_t thread_idx_max = 0;

#if defined(MG_LEGACY_INTERFACE)
#define MG_ALLOW_USING_GET_REQUEST_INFO_FOR_RESPONSE
#endif

struct mg_workerTLS {
	int is_master;
	unsigned long thread_idx;
	void *user_ptr;
#if defined(_WIN32)
	HANDLE pthread_cond_helper_mutex;
	struct mg_workerTLS *next_waiting_thread;
#endif
	const char *alpn_proto;
#if defined(MG_ALLOW_USING_GET_REQUEST_INFO_FOR_RESPONSE)
	char txtbuf[4];
#endif
};


#if defined(GCC_DIAGNOSTIC)
/* Show no warning in case system functions are not used. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif /* defined(GCC_DIAGNOSTIC) */
#if defined(__clang__)
/* Show no warning in case system functions are not used. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif


/* Get a unique thread ID as unsigned long, independent from the data type
 * of thread IDs defined by the operating system API.
 * If two calls to mg_current_thread_id  return the same value, they calls
 * are done from the same thread. If they return different values, they are
 * done from different threads. (Provided this function is used in the same
 * process context and threads are not repeatedly created and deleted, but
 * CivetWeb does not do that).
 * This function must match the signature required for SSL id callbacks:
 * CRYPTO_set_id_callback
 */
FUNCTION_MAY_BE_UNUSED
static unsigned long
mg_current_thread_id(void)
{
#if defined(_WIN32)
	return GetCurrentThreadId();
#else

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
	/* For every compiler, either "sizeof(pthread_t) > sizeof(unsigned long)"
	 * or not, so one of the two conditions will be unreachable by construction.
	 * Unfortunately the C standard does not define a way to check this at
	 * compile time, since the #if preprocessor conditions can not use the
	 * sizeof operator as an argument. */
#endif

	if (sizeof(pthread_t) > sizeof(unsigned long)) {
		/* This is the problematic case for CRYPTO_set_id_callback:
		 * The OS pthread_t can not be cast to unsigned long. */
		struct mg_workerTLS *tls =
		    (struct mg_workerTLS *)pthread_getspecific(sTlsKey);
		if (tls == NULL) {
			/* SSL called from an unknown thread: Create some thread index.
			 */
			tls = (struct mg_workerTLS *)mg_malloc(sizeof(struct mg_workerTLS));
			tls->is_master = -2; /* -2 means "3rd party thread" */
			tls->thread_idx = (unsigned)mg_atomic_inc(&thread_idx_max);
			pthread_setspecific(sTlsKey, tls);
		}
		return tls->thread_idx;
	} else {
		/* pthread_t may be any data type, so a simple cast to unsigned long
		 * can rise a warning/error, depending on the platform.
		 * Here memcpy is used as an anything-to-anything cast. */
		unsigned long ret = 0;
		pthread_t t = pthread_self();
		memcpy(&ret, &t, sizeof(pthread_t));
		return ret;
	}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
}


FUNCTION_MAY_BE_UNUSED
static uint64_t
mg_get_current_time_ns(void)
{
	struct timespec tsnow;
	clock_gettime(CLOCK_REALTIME, &tsnow);
	return (((uint64_t)tsnow.tv_sec) * 1000000000) + (uint64_t)tsnow.tv_nsec;
}


#if defined(GCC_DIAGNOSTIC)
/* Show no warning in case system functions are not used. */
#pragma GCC diagnostic pop
#endif /* defined(GCC_DIAGNOSTIC) */
#if defined(__clang__)
/* Show no warning in case system functions are not used. */
#pragma clang diagnostic pop
#endif


#if defined(NEED_DEBUG_TRACE_FUNC)
static void
DEBUG_TRACE_FUNC(const char *func, unsigned line, const char *fmt, ...)
{
	va_list args;
	struct timespec tsnow;

	/* Get some operating system independent thread id */
	unsigned long thread_id = mg_current_thread_id();

	clock_gettime(CLOCK_REALTIME, &tsnow);

	flockfile(DEBUG_TRACE_STREAM);
	fprintf(DEBUG_TRACE_STREAM,
	        "*** %lu.%09lu %lu %s:%u: ",
	        (unsigned long)tsnow.tv_sec,
	        (unsigned long)tsnow.tv_nsec,
	        thread_id,
	        func,
	        line);
	va_start(args, fmt);
	vfprintf(DEBUG_TRACE_STREAM, fmt, args);
	va_end(args);
	putc('\n', DEBUG_TRACE_STREAM);
	fflush(DEBUG_TRACE_STREAM);
	funlockfile(DEBUG_TRACE_STREAM);
}
#endif /* NEED_DEBUG_TRACE_FUNC */


#define MD5_STATIC static
#include "md5.inl"

/* Darwin prior to 7.0 and Win32 do not have socklen_t */
#if defined(NO_SOCKLEN_T)
typedef int socklen_t;
#endif /* NO_SOCKLEN_T */

#define IP_ADDR_STR_LEN (50) /* IPv6 hex string is 46 chars */

#if !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL (0)
#endif


/* SSL: mbedTLS vs. GnuTLS vs. no-ssl vs. OpenSSL */
#if defined(USE_MBEDTLS)
/* mbedTLS */
#include "mod_mbedtls.inl"

#elif defined(USE_GNUTLS)
/* GnuTLS */
#include "mod_gnutls.inl"

#elif defined(NO_SSL)
/* no SSL */
typedef struct SSL SSL; /* dummy for SSL argument to push/pull */
typedef struct SSL_CTX SSL_CTX;

#elif defined(NO_SSL_DL)
/* OpenSSL without dynamic loading */
#include <openssl/bn.h>
#include <openssl/conf.h>
#include <openssl/crypto.h>
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <openssl/x509.h>

#if defined(WOLFSSL_VERSION)
/* Additional defines for WolfSSL, see
 * https://github.com/civetweb/civetweb/issues/583 */
#include "wolfssl_extras.inl"
#endif

#if defined(OPENSSL_IS_BORINGSSL)
/* From boringssl/src/include/openssl/mem.h:
 *
 * OpenSSL has, historically, had a complex set of malloc debugging options.
 * However, that was written in a time before Valgrind and ASAN. Since we now
 * have those tools, the OpenSSL allocation functions are simply macros around
 * the standard memory functions.
 *
 * #define OPENSSL_free free */
#define free free
// disable for boringssl
#define CONF_modules_unload(a) ((void)0)
#define ENGINE_cleanup() ((void)0)
#endif

/* If OpenSSL headers are included, automatically select the API version */
#if (OPENSSL_VERSION_NUMBER >= 0x30000000L)
#if !defined(OPENSSL_API_3_0)
#define OPENSSL_API_3_0
#endif
#define OPENSSL_REMOVE_THREAD_STATE()
#else
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L)
#if !defined(OPENSSL_API_1_1)
#define OPENSSL_API_1_1
#endif
#define OPENSSL_REMOVE_THREAD_STATE()
#else
#if !defined(OPENSSL_API_1_0)
#define OPENSSL_API_1_0
#endif
#define OPENSSL_REMOVE_THREAD_STATE() ERR_remove_thread_state(NULL)
#endif
#endif


#else
/* SSL loaded dynamically from DLL / shared object */
/* Add all prototypes here, to be independent from OpenSSL source
 * installation. */
#include "openssl_dl.inl"

#endif /* Various SSL bindings */


#if !defined(NO_CACHING)
static const char month_names[][4] = {"Jan",
                                      "Feb",
                                      "Mar",
                                      "Apr",
                                      "May",
                                      "Jun",
                                      "Jul",
                                      "Aug",
                                      "Sep",
                                      "Oct",
                                      "Nov",
                                      "Dec"};
#endif /* !NO_CACHING */


/* Unified socket address. For IPv6 support, add IPv6 address structure in
 * the union u. */
union usa {
	struct sockaddr sa;
	struct sockaddr_in sin;
#if defined(USE_IPV6)
	struct sockaddr_in6 sin6;
#endif
#if defined(USE_X_DOM_SOCKET)
	struct sockaddr_un sun;
#endif
};

#if defined(USE_X_DOM_SOCKET)
static unsigned short
USA_IN_PORT_UNSAFE(union usa *s)
{
	if (s->sa.sa_family == AF_INET)
		return s->sin.sin_port;
#if defined(USE_IPV6)
	if (s->sa.sa_family == AF_INET6)
		return s->sin6.sin6_port;
#endif
	return 0;
}
#endif
#if defined(USE_IPV6)
#define USA_IN_PORT_UNSAFE(s)                                                  \
	(((s)->sa.sa_family == AF_INET6) ? (s)->sin6.sin6_port : (s)->sin.sin_port)
#else
#define USA_IN_PORT_UNSAFE(s) ((s)->sin.sin_port)
#endif

/* Describes a string (chunk of memory). */
struct vec {
	const char *ptr;
	size_t len;
};

struct mg_file_stat {
	/* File properties filled by mg_stat: */
	uint64_t size;
	time_t last_modified;
	int is_directory; /* Set to 1 if mg_stat is called for a directory */
	int is_gzipped;   /* Set to 1 if the content is gzipped, in which
	                   * case we need a "Content-Eencoding: gzip" header */
	int location;     /* 0 = nowhere, 1 = on disk, 2 = in memory */
};


struct mg_file_access {
	/* File properties filled by mg_fopen: */
	FILE *fp;
};

struct mg_file {
	struct mg_file_stat stat;
	struct mg_file_access access;
};


#define STRUCT_FILE_INITIALIZER                                                \
	{                                                                          \
		{(uint64_t)0, (time_t)0, 0, 0, 0},                                     \
		{                                                                      \
			(FILE *)NULL                                                       \
		}                                                                      \
	}


/* Describes listening socket, or socket which was accept()-ed by the master
 * thread and queued for future handling by the worker thread. */
struct socket {
	SOCKET sock;             /* Listening socket */
	union usa lsa;           /* Local socket address */
	union usa rsa;           /* Remote socket address */
	unsigned char is_ssl;    /* Is port SSL-ed */
	unsigned char ssl_redir; /* Is port supposed to redirect everything to SSL
	                          * port */
	unsigned char
	    is_optional; /* Shouldn't cause us to exit if we can't bind to it */
	unsigned char in_use; /* 0: invalid, 1: valid, 2: free */
};


/* Enum const for all options must be in sync with
 * static struct mg_option config_options[]
 * This is tested in the unit test (test/private.c)
 * "Private Config Options"
 */
enum {
	/* Once for each server */
	LISTENING_PORTS,
	NUM_THREADS,
	PRESPAWN_THREADS,
	RUN_AS_USER,
	CONFIG_TCP_NODELAY, /* Prepended CONFIG_ to avoid conflict with the
	                     * socket option typedef TCP_NODELAY. */
	MAX_REQUEST_SIZE,
	LINGER_TIMEOUT,
	CONNECTION_QUEUE_SIZE,
	LISTEN_BACKLOG_SIZE,
#if defined(__linux__)
	ALLOW_SENDFILE_CALL,
#endif
#if defined(_WIN32)
	CASE_SENSITIVE_FILES,
#endif
	THROTTLE,
	ENABLE_KEEP_ALIVE,
	REQUEST_TIMEOUT,
	KEEP_ALIVE_TIMEOUT,
#if defined(USE_WEBSOCKET)
	WEBSOCKET_TIMEOUT,
	ENABLE_WEBSOCKET_PING_PONG,
#endif
	DECODE_URL,
	DECODE_QUERY_STRING,
#if defined(USE_LUA)
	LUA_BACKGROUND_SCRIPT,
	LUA_BACKGROUND_SCRIPT_PARAMS,
#endif
#if defined(USE_HTTP2)
	ENABLE_HTTP2,
#endif

	/* Once for each domain */
	DOCUMENT_ROOT,
	FALLBACK_DOCUMENT_ROOT,

	ACCESS_LOG_FILE,
	ERROR_LOG_FILE,

	CGI_EXTENSIONS,
	CGI_ENVIRONMENT,
	CGI_INTERPRETER,
	CGI_INTERPRETER_ARGS,
#if defined(USE_TIMERS)
	CGI_TIMEOUT,
#endif
	CGI_BUFFERING,

	CGI2_EXTENSIONS,
	CGI2_ENVIRONMENT,
	CGI2_INTERPRETER,
	CGI2_INTERPRETER_ARGS,
#if defined(USE_TIMERS)
	CGI2_TIMEOUT,
#endif
	CGI2_BUFFERING,

#if defined(USE_4_CGI)
	CGI3_EXTENSIONS,
	CGI3_ENVIRONMENT,
	CGI3_INTERPRETER,
	CGI3_INTERPRETER_ARGS,
#if defined(USE_TIMERS)
	CGI3_TIMEOUT,
#endif
	CGI3_BUFFERING,

	CGI4_EXTENSIONS,
	CGI4_ENVIRONMENT,
	CGI4_INTERPRETER,
	CGI4_INTERPRETER_ARGS,
#if defined(USE_TIMERS)
	CGI4_TIMEOUT,
#endif
	CGI4_BUFFERING,
#endif

	PUT_DELETE_PASSWORDS_FILE, /* must follow CGI_* */
	PROTECT_URI,
	AUTHENTICATION_DOMAIN,
	ENABLE_AUTH_DOMAIN_CHECK,
	SSI_EXTENSIONS,
	ENABLE_DIRECTORY_LISTING,
	ENABLE_WEBDAV,
	GLOBAL_PASSWORDS_FILE,
	INDEX_FILES,
	ACCESS_CONTROL_LIST,
	EXTRA_MIME_TYPES,
	SSL_CERTIFICATE,
	SSL_CERTIFICATE_CHAIN,
	URL_REWRITE_PATTERN,
	HIDE_FILES,
	SSL_DO_VERIFY_PEER,
	SSL_CACHE_TIMEOUT,
	SSL_CA_PATH,
	SSL_CA_FILE,
	SSL_VERIFY_DEPTH,
	SSL_DEFAULT_VERIFY_PATHS,
	SSL_CIPHER_LIST,
	SSL_PROTOCOL_VERSION,
	SSL_SHORT_TRUST,

#if defined(USE_LUA)
	LUA_PRELOAD_FILE,
	LUA_SCRIPT_EXTENSIONS,
	LUA_SERVER_PAGE_EXTENSIONS,
#if defined(MG_EXPERIMENTAL_INTERFACES)
	LUA_DEBUG_PARAMS,
#endif
#endif
#if defined(USE_DUKTAPE)
	DUKTAPE_SCRIPT_EXTENSIONS,
#endif

#if defined(USE_WEBSOCKET)
	WEBSOCKET_ROOT,
	FALLBACK_WEBSOCKET_ROOT,
#endif
#if defined(USE_LUA) && defined(USE_WEBSOCKET)
	LUA_WEBSOCKET_EXTENSIONS,
#endif

	ACCESS_CONTROL_ALLOW_ORIGIN,
	ACCESS_CONTROL_ALLOW_METHODS,
	ACCESS_CONTROL_ALLOW_HEADERS,
	ACCESS_CONTROL_EXPOSE_HEADERS,
	ACCESS_CONTROL_ALLOW_CREDENTIALS,
	ERROR_PAGES,
#if !defined(NO_CACHING)
	STATIC_FILE_MAX_AGE,
	STATIC_FILE_CACHE_CONTROL,
#endif
#if !defined(NO_SSL)
	STRICT_HTTPS_MAX_AGE,
#endif
	ADDITIONAL_HEADER,
	ALLOW_INDEX_SCRIPT_SUB_RES,

	NUM_OPTIONS
};


/* Config option name, config types, default value.
 * Must be in the same order as the enum const above.
 */
static const struct mg_option config_options[] = {

    /* Once for each server */
    {"listening_ports", MG_CONFIG_TYPE_STRING_LIST, "8080"},
    {"num_threads", MG_CONFIG_TYPE_NUMBER, "50"},
    {"prespawn_threads", MG_CONFIG_TYPE_NUMBER, "0"},
    {"run_as_user", MG_CONFIG_TYPE_STRING, NULL},
    {"tcp_nodelay", MG_CONFIG_TYPE_NUMBER, "0"},
    {"max_request_size", MG_CONFIG_TYPE_NUMBER, "16384"},
    {"linger_timeout_ms", MG_CONFIG_TYPE_NUMBER, NULL},
    {"connection_queue", MG_CONFIG_TYPE_NUMBER, "20"},
    {"listen_backlog", MG_CONFIG_TYPE_NUMBER, "200"},
#if defined(__linux__)
    {"allow_sendfile_call", MG_CONFIG_TYPE_BOOLEAN, "yes"},
#endif
#if defined(_WIN32)
    {"case_sensitive", MG_CONFIG_TYPE_BOOLEAN, "no"},
#endif
    {"throttle", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"enable_keep_alive", MG_CONFIG_TYPE_BOOLEAN, "no"},
    {"request_timeout_ms", MG_CONFIG_TYPE_NUMBER, "30000"},
    {"keep_alive_timeout_ms", MG_CONFIG_TYPE_NUMBER, "500"},
#if defined(USE_WEBSOCKET)
    {"websocket_timeout_ms", MG_CONFIG_TYPE_NUMBER, NULL},
    {"enable_websocket_ping_pong", MG_CONFIG_TYPE_BOOLEAN, "no"},
#endif
    {"decode_url", MG_CONFIG_TYPE_BOOLEAN, "yes"},
    {"decode_query_string", MG_CONFIG_TYPE_BOOLEAN, "no"},
#if defined(USE_LUA)
    {"lua_background_script", MG_CONFIG_TYPE_FILE, NULL},
    {"lua_background_script_params", MG_CONFIG_TYPE_STRING_LIST, NULL},
#endif
#if defined(USE_HTTP2)
    {"enable_http2", MG_CONFIG_TYPE_BOOLEAN, "no"},
#endif

    /* Once for each domain */
    {"document_root", MG_CONFIG_TYPE_DIRECTORY, NULL},
    {"fallback_document_root", MG_CONFIG_TYPE_DIRECTORY, NULL},

    {"access_log_file", MG_CONFIG_TYPE_FILE, NULL},
    {"error_log_file", MG_CONFIG_TYPE_FILE, NULL},

    {"cgi_pattern", MG_CONFIG_TYPE_EXT_PATTERN, "**.cgi$|**.pl$|**.php$"},
    {"cgi_environment", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"cgi_interpreter", MG_CONFIG_TYPE_FILE, NULL},
    {"cgi_interpreter_args", MG_CONFIG_TYPE_STRING, NULL},
#if defined(USE_TIMERS)
    {"cgi_timeout_ms", MG_CONFIG_TYPE_NUMBER, NULL},
#endif
    {"cgi_buffering", MG_CONFIG_TYPE_BOOLEAN, "yes"},

    {"cgi2_pattern", MG_CONFIG_TYPE_EXT_PATTERN, NULL},
    {"cgi2_environment", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"cgi2_interpreter", MG_CONFIG_TYPE_FILE, NULL},
    {"cgi2_interpreter_args", MG_CONFIG_TYPE_STRING, NULL},
#if defined(USE_TIMERS)
    {"cgi2_timeout_ms", MG_CONFIG_TYPE_NUMBER, NULL},
#endif
    {"cgi2_buffering", MG_CONFIG_TYPE_BOOLEAN, "yes"},

#if defined(USE_4_CGI)
    {"cgi3_pattern", MG_CONFIG_TYPE_EXT_PATTERN, NULL},
    {"cgi3_environment", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"cgi3_interpreter", MG_CONFIG_TYPE_FILE, NULL},
    {"cgi3_interpreter_args", MG_CONFIG_TYPE_STRING, NULL},
#if defined(USE_TIMERS)
    {"cgi3_timeout_ms", MG_CONFIG_TYPE_NUMBER, NULL},
#endif
    {"cgi3_buffering", MG_CONFIG_TYPE_BOOLEAN, "yes"},

    {"cgi4_pattern", MG_CONFIG_TYPE_EXT_PATTERN, NULL},
    {"cgi4_environment", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"cgi4_interpreter", MG_CONFIG_TYPE_FILE, NULL},
    {"cgi4_interpreter_args", MG_CONFIG_TYPE_STRING, NULL},
#if defined(USE_TIMERS)
    {"cgi4_timeout_ms", MG_CONFIG_TYPE_NUMBER, NULL},
#endif
    {"cgi4_buffering", MG_CONFIG_TYPE_BOOLEAN, "yes"},

#endif

    {"put_delete_auth_file", MG_CONFIG_TYPE_FILE, NULL},
    {"protect_uri", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"authentication_domain", MG_CONFIG_TYPE_STRING, "mydomain.com"},
    {"enable_auth_domain_check", MG_CONFIG_TYPE_BOOLEAN, "yes"},
    {"ssi_pattern", MG_CONFIG_TYPE_EXT_PATTERN, "**.shtml$|**.shtm$"},
    {"enable_directory_listing", MG_CONFIG_TYPE_BOOLEAN, "yes"},
    {"enable_webdav", MG_CONFIG_TYPE_BOOLEAN, "no"},
    {"global_auth_file", MG_CONFIG_TYPE_FILE, NULL},
    {"index_files",
     MG_CONFIG_TYPE_STRING_LIST,
#if defined(USE_LUA)
     "index.xhtml,index.html,index.htm,"
     "index.lp,index.lsp,index.lua,index.cgi,"
     "index.shtml,index.php"},
#else
     "index.xhtml,index.html,index.htm,index.cgi,index.shtml,index.php"},
#endif
    {"access_control_list", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"extra_mime_types", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"ssl_certificate", MG_CONFIG_TYPE_FILE, NULL},
    {"ssl_certificate_chain", MG_CONFIG_TYPE_FILE, NULL},
    {"url_rewrite_patterns", MG_CONFIG_TYPE_STRING_LIST, NULL},
    {"hide_files_patterns", MG_CONFIG_TYPE_EXT_PATTERN, NULL},

    {"ssl_verify_peer", MG_CONFIG_TYPE_YES_NO_OPTIONAL, "no"},
    {"ssl_cache_timeout", MG_CONFIG_TYPE_NUMBER, "-1"},

    {"ssl_ca_path", MG_CONFIG_TYPE_DIRECTORY, NULL},
    {"ssl_ca_file", MG_CONFIG_TYPE_FILE, NULL},
    {"ssl_verify_depth", MG_CONFIG_TYPE_NUMBER, "9"},
    {"ssl_default_verify_paths", MG_CONFIG_TYPE_BOOLEAN, "yes"},
    {"ssl_cipher_list", MG_CONFIG_TYPE_STRING, NULL},

    /* HTTP2 requires ALPN, and anyway TLS1.2 should be considered
     * as a minimum in 2020 */
    {"ssl_protocol_version", MG_CONFIG_TYPE_NUMBER, "4"},

    {"ssl_short_trust", MG_CONFIG_TYPE_BOOLEAN, "no"},

#if defined(USE_LUA)
    {"lua_preload_file", MG_CONFIG_TYPE_FILE, NULL},
    {"lua_script_pattern", MG_CONFIG_TYPE_EXT_PATTERN, "**.lua$"},
    {"lua_server_page_pattern", MG_CONFIG_TYPE_EXT_PATTERN, "**.lp$|**.lsp$"},
#if defined(MG_EXPERIMENTAL_INTERFACES)
    {"lua_debug", MG_CONFIG_TYPE_STRING, NULL},
#endif
#endif
#if defined(USE_DUKTAPE)
    /* The support for duktape is still in alpha version state.
     * The name of this config option might change. */
    {"duktape_script_pattern", MG_CONFIG_TYPE_EXT_PATTERN, "**.ssjs$"},
#endif

#if defined(USE_WEBSOCKET)
    {"websocket_root", MG_CONFIG_TYPE_DIRECTORY, NULL},
    {"fallback_websocket_root", MG_CONFIG_TYPE_DIRECTORY, NULL},
#endif
#if defined(USE_LUA) && defined(USE_WEBSOCKET)
    {"lua_websocket_pattern", MG_CONFIG_TYPE_EXT_PATTERN, "**.lua$"},
#endif
    {"access_control_allow_origin", MG_CONFIG_TYPE_STRING, "*"},
    {"access_control_allow_methods", MG_CONFIG_TYPE_STRING, "*"},
    {"access_control_allow_headers", MG_CONFIG_TYPE_STRING, "*"},
    {"access_control_expose_headers", MG_CONFIG_TYPE_STRING, ""},
    {"access_control_allow_credentials", MG_CONFIG_TYPE_STRING, ""},
    {"error_pages", MG_CONFIG_TYPE_DIRECTORY, NULL},
#if !defined(NO_CACHING)
    {"static_file_max_age", MG_CONFIG_TYPE_NUMBER, "3600"},
    {"static_file_cache_control", MG_CONFIG_TYPE_STRING, NULL},
#endif
#if !defined(NO_SSL)
    {"strict_transport_security_max_age", MG_CONFIG_TYPE_NUMBER, NULL},
#endif
    {"additional_header", MG_CONFIG_TYPE_STRING_MULTILINE, NULL},
    {"allow_index_script_resource", MG_CONFIG_TYPE_BOOLEAN, "no"},

    {NULL, MG_CONFIG_TYPE_UNKNOWN, NULL}};


/* Check if the config_options and the corresponding enum have compatible
 * sizes. */
mg_static_assert((sizeof(config_options) / sizeof(config_options[0]))
                     == (NUM_OPTIONS + 1),
                 "config_options and enum not sync");


enum { REQUEST_HANDLER, WEBSOCKET_HANDLER, AUTH_HANDLER };


struct mg_handler_info {
	/* Name/Pattern of the URI. */
	char *uri;
	size_t uri_len;

	/* handler type */
	int handler_type;

	/* Handler for http/https or requests. */
	mg_request_handler handler;
	unsigned int refcount;
	int removing;

	/* Handler for ws/wss (websocket) requests. */
	mg_websocket_connect_handler connect_handler;
	mg_websocket_ready_handler ready_handler;
	mg_websocket_data_handler data_handler;
	mg_websocket_close_handler close_handler;

	/* accepted subprotocols for ws/wss requests. */
	struct mg_websocket_subprotocols *subprotocols;

	/* Handler for authorization requests */
	mg_authorization_handler auth_handler;

	/* User supplied argument for the handler function. */
	void *cbdata;

	/* next handler in a linked list */
	struct mg_handler_info *next;
};


enum {
	CONTEXT_INVALID,
	CONTEXT_SERVER,
	CONTEXT_HTTP_CLIENT,
	CONTEXT_WS_CLIENT
};


struct mg_domain_context {
	SSL_CTX *ssl_ctx;                 /* SSL context */
	char *config[NUM_OPTIONS];        /* Civetweb configuration parameters */
	struct mg_handler_info *handlers; /* linked list of uri handlers */
	int64_t ssl_cert_last_mtime;

	/* Server nonce */
	uint64_t auth_nonce_mask;  /* Mask for all nonce values */
	unsigned long nonce_count; /* Used nonces, used for authentication */

#if defined(USE_LUA) && defined(USE_WEBSOCKET)
	/* linked list of shared lua websockets */
	struct mg_shared_lua_websocket_list *shared_lua_websockets;
#endif

	/* Linked list of domains */
	struct mg_domain_context *next;
};


/* Stop flag can be "volatile" or require a lock.
 * MSDN uses volatile for "Interlocked" operations, but also explicitly
 * states a read operation for int is always atomic. */
#if defined(STOP_FLAG_NEEDS_LOCK)

typedef ptrdiff_t volatile stop_flag_t;

static int
STOP_FLAG_IS_ZERO(const stop_flag_t *f)
{
	stop_flag_t sf = mg_atomic_add((stop_flag_t *)f, 0);
	return (sf == 0);
}

static int
STOP_FLAG_IS_TWO(stop_flag_t *f)
{
	stop_flag_t sf = mg_atomic_add(f, 0);
	return (sf == 2);
}

static void
STOP_FLAG_ASSIGN(stop_flag_t *f, stop_flag_t v)
{
	stop_flag_t sf = 0;
	do {
		sf = mg_atomic_compare_and_swap(f, *f, v);
	} while (sf != v);
}

#else /* STOP_FLAG_NEEDS_LOCK */

typedef int volatile stop_flag_t;
#define STOP_FLAG_IS_ZERO(f) ((*(f)) == 0)
#define STOP_FLAG_IS_TWO(f) ((*(f)) == 2)
#define STOP_FLAG_ASSIGN(f, v) ((*(f)) = (v))

#endif /* STOP_FLAG_NEEDS_LOCK */


#if !defined(NUM_WEBDAV_LOCKS)
#define NUM_WEBDAV_LOCKS 10
#endif
#if !defined(LOCK_DURATION_S)
#define LOCK_DURATION_S 60
#endif


struct twebdav_lock {
	uint64_t locktime;
	char token[33];
	char path[UTF8_PATH_MAX * 2];
	char user[UTF8_PATH_MAX * 2];
};


struct mg_context {

	/* Part 1 - Physical context:
	 * This holds threads, ports, timeouts, ...
	 * set for the entire server, independent from the
	 * addressed hostname.
	 */

	/* Connection related */
	int context_type; /* See CONTEXT_* above */

	struct socket *listening_sockets;
	struct mg_pollfd *listening_socket_fds;
	unsigned int num_listening_sockets;

	struct mg_connection *worker_connections; /* The connection struct, pre-
	                                           * allocated for each worker */

#if defined(USE_SERVER_STATS)
	volatile ptrdiff_t active_connections;
	volatile ptrdiff_t max_active_connections;
	volatile ptrdiff_t total_connections;
	volatile ptrdiff_t total_requests;
	volatile int64_t total_data_read;
	volatile int64_t total_data_written;
#endif

	/* Thread related */
	stop_flag_t stop_flag;        /* Should we stop event loop */
	pthread_mutex_t thread_mutex; /* Protects client_socks or queue */

	pthread_t masterthreadid;            /* The master thread ID */
	unsigned int cfg_max_worker_threads; /* How many worker-threads we are
	                                        allowed to create, total */

	unsigned int spawned_worker_threads; /* How many worker-threads currently
	                                        exist (modified by master thread) */
	unsigned int
	    idle_worker_thread_count; /* How many worker-threads are currently
	                                 sitting around with nothing to do */
	/* Access to this value MUST be synchronized by thread_mutex */

	pthread_t *worker_threadids;      /* The worker thread IDs */
	unsigned long starter_thread_idx; /* thread index which called mg_start */

	/* Connection to thread dispatching */
#if defined(ALTERNATIVE_QUEUE)
	struct socket *client_socks;
	void **client_wait_events;
#else
	struct socket *squeue; /* Socket queue (sq) : accepted sockets waiting for a
	                       worker thread */
	volatile int sq_head;  /* Head of the socket queue */
	volatile int sq_tail;  /* Tail of the socket queue */
	pthread_cond_t sq_full;  /* Signaled when socket is produced */
	pthread_cond_t sq_empty; /* Signaled when socket is consumed */
	volatile int sq_blocked; /* Status information: sq is full */
	int sq_size;             /* No of elements in socket queue */
#if defined(USE_SERVER_STATS)
	int sq_max_fill;
#endif /* USE_SERVER_STATS */
#endif /* ALTERNATIVE_QUEUE */

	/* Memory related */
	unsigned int max_request_size; /* The max request size */

#if defined(USE_SERVER_STATS)
	struct mg_memory_stat ctx_memory;
#endif

	/* WebDAV lock structures */
	struct twebdav_lock webdav_lock[NUM_WEBDAV_LOCKS];

	/* Operating system related */
	char *systemName;  /* What operating system is running */
	time_t start_time; /* Server start time, used for authentication
	                    * and for diagnstics. */

#if defined(USE_TIMERS)
	struct ttimers *timers;
#endif

	/* Lua specific: Background operations and shared websockets */
#if defined(USE_LUA)
	void *lua_background_state;   /* lua_State (here as void *) */
	pthread_mutex_t lua_bg_mutex; /* Protect background state */
	int lua_bg_log_available;     /* Use Lua background state for access log */
#endif

	int user_shutdown_notification_socket;   /* mg_stop() will close this
	                                            socket... */
	int thread_shutdown_notification_socket; /* to cause poll() in all threads
	                                            to return immediately */

	/* Server nonce */
	pthread_mutex_t nonce_mutex; /* Protects ssl_ctx, handlers,
	                              * ssl_cert_last_mtime, nonce_count, and
	                              * next (linked list) */

	/* Server callbacks */
	struct mg_callbacks callbacks; /* User-defined callback function */
	void *user_data;               /* User-defined data */

	/* Part 2 - Logical domain:
	 * This holds hostname, TLS certificate, document root, ...
	 * set for a domain hosted at the server.
	 * There may be multiple domains hosted at one physical server.
	 * The default domain "dd" is the first element of a list of
	 * domains.
	 */
	struct mg_domain_context dd; /* default domain */
};


#if defined(USE_SERVER_STATS)
static struct mg_memory_stat mg_common_memory = {0, 0, 0};

static struct mg_memory_stat *
get_memory_stat(struct mg_context *ctx)
{
	if (ctx) {
		return &(ctx->ctx_memory);
	}
	return &mg_common_memory;
}
#endif

enum {
	CONNECTION_TYPE_INVALID = 0,
	CONNECTION_TYPE_REQUEST = 1,
	CONNECTION_TYPE_RESPONSE = 2
};

enum {
	PROTOCOL_TYPE_HTTP1 = 0,
	PROTOCOL_TYPE_WEBSOCKET = 1,
	PROTOCOL_TYPE_HTTP2 = 2
};


#if defined(USE_HTTP2)
#if !defined(HTTP2_DYN_TABLE_SIZE)
#define HTTP2_DYN_TABLE_SIZE (256)
#endif

struct mg_http2_connection {
	uint32_t stream_id;
	uint32_t dyn_table_size;
	struct mg_header dyn_table[HTTP2_DYN_TABLE_SIZE];
};
#endif


struct mg_connection {
	int connection_type; /* see CONNECTION_TYPE_* above */
	int protocol_type;   /* see PROTOCOL_TYPE_*: 0=http/1.x, 1=ws, 2=http/2 */
	int request_state;   /* 0: nothing sent, 1: header partially sent, 2: header
	                     fully sent */
#if defined(USE_HTTP2)
	struct mg_http2_connection http2;
#endif

	struct mg_request_info request_info;
	struct mg_response_info response_info;

	struct mg_context *phys_ctx;
	struct mg_domain_context *dom_ctx;

#if defined(USE_SERVER_STATS)
	int conn_state; /* 0 = undef, numerical value may change in different
	                 * versions. For the current definition, see
	                 * mg_get_connection_info_impl */
#endif
	SSL *ssl;               /* SSL descriptor */
	struct socket client;   /* Connected client */
	time_t conn_birth_time; /* Time (wall clock) when connection was
	                         * established */
#if defined(USE_SERVER_STATS)
	time_t conn_close_time; /* Time (wall clock) when connection was
	                         * closed (or 0 if still open) */
	double processing_time; /* Processing time for one request. */
#endif
	struct timespec req_time; /* Time (since system start) when the request
	                           * was received */
	int64_t num_bytes_sent;   /* Total bytes sent to client */
	int64_t content_len;      /* How many bytes of content can be read
	                           * !is_chunked: Content-Length header value
	                           *              or -1 (until connection closed,
	                           *                     not allowed for a request)
	                           * is_chunked: >= 0, appended gradually
	                           */
	int64_t consumed_content; /* How many bytes of content have been read */
	int is_chunked;           /* Transfer-Encoding is chunked:
	                           * 0 = not chunked,
	                           * 1 = chunked, not yet, or some data read,
	                           * 2 = chunked, has error,
	                           * 3 = chunked, all data read except trailer,
	                           * 4 = chunked, all data read
	                           */
	char *buf;                /* Buffer for received data */
	char *path_info;          /* PATH_INFO part of the URL */

	int must_close;       /* 1 if connection must be closed */
	int accept_gzip;      /* 1 if gzip encoding is accepted */
	int in_error_handler; /* 1 if in handler for user defined error
	                       * pages */
#if defined(USE_WEBSOCKET)
	int in_websocket_handling; /* 1 if in read_websocket */
#endif
#if defined(USE_ZLIB) && defined(USE_WEBSOCKET)                                \
    && defined(MG_EXPERIMENTAL_INTERFACES)
	/* Parameters for websocket data compression according to rfc7692 */
	int websocket_deflate_server_max_windows_bits;
	int websocket_deflate_client_max_windows_bits;
	int websocket_deflate_server_no_context_takeover;
	int websocket_deflate_client_no_context_takeover;
	int websocket_deflate_initialized;
	int websocket_deflate_flush;
	z_stream websocket_deflate_state;
	z_stream websocket_inflate_state;
#endif
	int handled_requests; /* Number of requests handled by this connection
	                       */
	int buf_size;         /* Buffer size */
	int request_len;      /* Size of the request + headers in a buffer */
	int data_len;         /* Total size of data in a buffer */
	int status_code;      /* HTTP reply status code, e.g. 200 */
	int throttle;         /* Throttling, bytes/sec. <= 0 means no
	                       * throttle */

	time_t last_throttle_time; /* Last time throttled data was sent */
	int last_throttle_bytes;   /* Bytes sent this second */
	pthread_mutex_t mutex;     /* Used by mg_(un)lock_connection to ensure
	                            * atomic transmissions for websockets */
#if defined(USE_LUA) && defined(USE_WEBSOCKET)
	void *lua_websocket_state; /* Lua_State for a websocket connection */
#endif

	void *tls_user_ptr; /* User defined pointer in thread local storage,
	                     * for quick access */
};


/* Directory entry */
struct de {
	char *file_name;
	struct mg_file_stat file;
};


#define mg_cry_internal(conn, fmt, ...)                                        \
	mg_cry_internal_wrap(conn, NULL, __func__, __LINE__, fmt, __VA_ARGS__)

#define mg_cry_ctx_internal(ctx, fmt, ...)                                     \
	mg_cry_internal_wrap(NULL, ctx, __func__, __LINE__, fmt, __VA_ARGS__)

static void mg_cry_internal_wrap(const struct mg_connection *conn,
                                 struct mg_context *ctx,
                                 const char *func,
                                 unsigned line,
                                 const char *fmt,
                                 ...) PRINTF_ARGS(5, 6);


#if !defined(NO_THREAD_NAME)
#if defined(_WIN32) && defined(_MSC_VER)
/* Set the thread name for debugging purposes in Visual Studio
 * http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
 */
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
	DWORD dwType;     /* Must be 0x1000. */
	LPCSTR szName;    /* Pointer to name (in user addr space). */
	DWORD dwThreadID; /* Thread ID (-1=caller thread). */
	DWORD dwFlags;    /* Reserved for future use, must be zero. */
} THREADNAME_INFO;
#pragma pack(pop)

#elif defined(__linux__)

#include <sys/prctl.h>
#include <sys/sendfile.h>
#if defined(ALTERNATIVE_QUEUE)
#include <sys/eventfd.h>
#endif /* ALTERNATIVE_QUEUE */


#if defined(ALTERNATIVE_QUEUE)

static void *
event_create(void)
{
	int evhdl = eventfd(0, EFD_CLOEXEC);
	int *ret;

	if (evhdl == -1) {
		/* Linux uses -1 on error, Windows NULL. */
		/* However, Linux does not return 0 on success either. */
		return 0;
	}

	ret = (int *)mg_malloc(sizeof(int));
	if (ret) {
		*ret = evhdl;
	} else {
		(void)close(evhdl);
	}

	return (void *)ret;
}


static int
event_wait(void *eventhdl)
{
	uint64_t u;
	int evhdl, s;

	if (!eventhdl) {
		/* error */
		return 0;
	}
	evhdl = *(int *)eventhdl;

	s = (int)read(evhdl, &u, sizeof(u));
	if (s != sizeof(u)) {
		/* error */
		return 0;
	}
	(void)u; /* the value is not required */
	return 1;
}


static int
event_signal(void *eventhdl)
{
	uint64_t u = 1;
	int evhdl, s;

	if (!eventhdl) {
		/* error */
		return 0;
	}
	evhdl = *(int *)eventhdl;

	s = (int)write(evhdl, &u, sizeof(u));
	if (s != sizeof(u)) {
		/* error */
		return 0;
	}
	return 1;
}


static void
event_destroy(void *eventhdl)
{
	int evhdl;

	if (!eventhdl) {
		/* error */
		return;
	}
	evhdl = *(int *)eventhdl;

	close(evhdl);
	mg_free(eventhdl);
}


#endif

#endif


#if !defined(__linux__) && !defined(_WIN32) && defined(ALTERNATIVE_QUEUE)

struct posix_event {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int signaled;
};


static void *
event_create(void)
{
	struct posix_event *ret = mg_malloc(sizeof(struct posix_event));
	if (ret == 0) {
		/* out of memory */
		return 0;
	}
	if (0 != pthread_mutex_init(&(ret->mutex), NULL)) {
		/* pthread mutex not available */
		mg_free(ret);
		return 0;
	}
	if (0 != pthread_cond_init(&(ret->cond), NULL)) {
		/* pthread cond not available */
		pthread_mutex_destroy(&(ret->mutex));
		mg_free(ret);
		return 0;
	}
	ret->signaled = 0;
	return (void *)ret;
}


static int
event_wait(void *eventhdl)
{
	struct posix_event *ev = (struct posix_event *)eventhdl;
	pthread_mutex_lock(&(ev->mutex));
	while (!ev->signaled) {
		pthread_cond_wait(&(ev->cond), &(ev->mutex));
	}
	ev->signaled = 0;
	pthread_mutex_unlock(&(ev->mutex));
	return 1;
}


static int
event_signal(void *eventhdl)
{
	struct posix_event *ev = (struct posix_event *)eventhdl;
	pthread_mutex_lock(&(ev->mutex));
	pthread_cond_signal(&(ev->cond));
	ev->signaled = 1;
	pthread_mutex_unlock(&(ev->mutex));
	return 1;
}


static void
event_destroy(void *eventhdl)
{
	struct posix_event *ev = (struct posix_event *)eventhdl;
	pthread_cond_destroy(&(ev->cond));
	pthread_mutex_destroy(&(ev->mutex));
	mg_free(ev);
}
#endif


static void
mg_set_thread_name(const char *name)
{
	char threadName[16 + 1]; /* 16 = Max. thread length in Linux/OSX/.. */

	mg_snprintf(
	    NULL, NULL, threadName, sizeof(threadName), "civetweb-%s", name);

#if defined(_WIN32)
#if defined(_MSC_VER)
	/* Windows and Visual Studio Compiler */
	__try {
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = ~0U;
		info.dwFlags = 0;

		RaiseException(0x406D1388,
		               0,
		               sizeof(info) / sizeof(ULONG_PTR),
		               (ULONG_PTR *)&info);
	} __except (EXCEPTION_EXECUTE_HANDLER) {
	}
#elif defined(__MINGW32__)
	/* No option known to set thread name for MinGW known */
#endif
#elif defined(_GNU_SOURCE) && defined(__GLIBC__)                               \
    && ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))
	/* pthread_setname_np first appeared in glibc in version 2.12 */
#if defined(__MACH__) && defined(__APPLE__)
	/* OS X only current thread name can be changed */
	(void)pthread_setname_np(threadName);
#else
	(void)pthread_setname_np(pthread_self(), threadName);
#endif
#elif defined(__linux__)
	/* On Linux we can use the prctl function.
	 * When building for Linux Standard Base (LSB) use
	 * NO_THREAD_NAME. However, thread names are a big
	 * help for debugging, so the stadard is to set them.
	 */
	(void)prctl(PR_SET_NAME, threadName, 0, 0, 0);
#endif
}
#else /* !defined(NO_THREAD_NAME) */
static void
mg_set_thread_name(const char *threadName)
{
}
#endif


CIVETWEB_API const struct mg_option *
mg_get_valid_options(void)
{
	return config_options;
}


/* Do not open file (unused) */
#define MG_FOPEN_MODE_NONE (0)

/* Open file for read only access */
#define MG_FOPEN_MODE_READ (1)

/* Open file for writing, create and overwrite */
#define MG_FOPEN_MODE_WRITE (2)

/* Open file for writing, create and append */
#define MG_FOPEN_MODE_APPEND (4)


static int
is_file_opened(const struct mg_file_access *fileacc)
{
	if (!fileacc) {
		return 0;
	}

	return (fileacc->fp != NULL);
}


#if !defined(NO_FILESYSTEMS)
static int mg_stat(const struct mg_connection *conn,
                   const char *path,
                   struct mg_file_stat *filep);


/* Reject files with special characters (for Windows) */
static int
mg_path_suspicious(const struct mg_connection *conn, const char *path)
{
	const uint8_t *c = (const uint8_t *)path;
	(void)conn; /* not used */

	if ((c == NULL) || (c[0] == 0)) {
		/* Null pointer or empty path --> suspicious */
		return 1;
	}

#if defined(_WIN32)
	while (*c) {
		if (*c < 32) {
			/* Control character */
			return 1;
		}
		if ((*c == '>') || (*c == '<') || (*c == '|')) {
			/* stdin/stdout redirection character */
			return 1;
		}
		if ((*c == '*') || (*c == '?')) {
			/* Wildcard character */
			return 1;
		}
		if (*c == '"') {
			/* Windows quotation */
			return 1;
		}
		c++;
	}
#endif

	/* Nothing suspicious found */
	return 0;
}


/* mg_fopen will open a file either in memory or on the disk.
 * The input parameter path is a string in UTF-8 encoding.
 * The input parameter mode is MG_FOPEN_MODE_*
 * On success, fp will be set in the output struct mg_file.
 * All status members will also be set.
 * The function returns 1 on success, 0 on error. */
static int
mg_fopen(const struct mg_connection *conn,
         const char *path,
         int mode,
         struct mg_file *filep)
{
	int found;

	if (!filep) {
		return 0;
	}
	filep->access.fp = NULL;

	if (mg_path_suspicious(conn, path)) {
		return 0;
	}

	/* filep is initialized in mg_stat: all fields with memset to,
	 * some fields like size and modification date with values */
	found = mg_stat(conn, path, &(filep->stat));

	if ((mode == MG_FOPEN_MODE_READ) && (!found)) {
		/* file does not exist and will not be created */
		return 0;
	}

#if defined(_WIN32)
	{
		wchar_t wbuf[UTF16_PATH_MAX];
		path_to_unicode(conn, path, wbuf, ARRAY_SIZE(wbuf));
		switch (mode) {
		case MG_FOPEN_MODE_READ:
			filep->access.fp = _wfopen(wbuf, L"rb");
			break;
		case MG_FOPEN_MODE_WRITE:
			filep->access.fp = _wfopen(wbuf, L"wb");
			break;
		case MG_FOPEN_MODE_APPEND:
			filep->access.fp = _wfopen(wbuf, L"ab");
			break;
		}
	}
#else
	/* Linux et al already use unicode. No need to convert. */
	switch (mode) {
	case MG_FOPEN_MODE_READ:
		filep->access.fp = fopen(path, "r");
		break;
	case MG_FOPEN_MODE_WRITE:
		filep->access.fp = fopen(path, "w");
		break;
	case MG_FOPEN_MODE_APPEND:
		filep->access.fp = fopen(path, "a");
		break;
	}

#endif
	if (!found) {
		/* File did not exist before fopen was called.
		 * Maybe it has been created now. Get stat info
		 * like creation time now. */
		found = mg_stat(conn, path, &(filep->stat));
		(void)found;
	}

	/* return OK if file is opened */
	return (filep->access.fp != NULL);
}


/* return 0 on success, just like fclose */
static int
mg_fclose(struct mg_file_access *fileacc)
{
	int ret = -1;
	if (fileacc != NULL) {
		if (fileacc->fp != NULL) {
			ret = fclose(fileacc->fp);
		}
		/* reset all members of fileacc */
		memset(fileacc, 0, sizeof(*fileacc));
	}
	return ret;
}
#endif /* NO_FILESYSTEMS */


static void
mg_strlcpy(char *dst, const char *src, size_t n)
{
	for (; *src != '\0' && n > 1; n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
}


static int
lowercase(const char *s)
{
	return tolower((unsigned char)*s);
}


CIVETWEB_API int
mg_strncasecmp(const char *s1, const char *s2, size_t len)
{
	int diff = 0;

	if (len > 0) {
		do {
			diff = lowercase(s1++) - lowercase(s2++);
		} while (diff == 0 && s1[-1] != '\0' && --len > 0);
	}

	return diff;
}


CIVETWEB_API int
mg_strcasecmp(const char *s1, const char *s2)
{
	int diff;

	do {
		diff = lowercase(s1++) - lowercase(s2++);
	} while (diff == 0 && s1[-1] != '\0');

	return diff;
}


static char *
mg_strndup_ctx(const char *ptr, size_t len, struct mg_context *ctx)
{
	char *p;
	(void)ctx; /* Avoid Visual Studio warning if USE_SERVER_STATS is not
	            * defined */

	if ((p = (char *)mg_malloc_ctx(len + 1, ctx)) != NULL) {
		mg_strlcpy(p, ptr, len + 1);
	}

	return p;
}


static char *
mg_strdup_ctx(const char *str, struct mg_context *ctx)
{
	return mg_strndup_ctx(str, strlen(str), ctx);
}

static char *
mg_strdup(const char *str)
{
	return mg_strndup_ctx(str, strlen(str), NULL);
}


static const char *
mg_strcasestr(const char *big_str, const char *small_str)
{
	size_t i, big_len = strlen(big_str), small_len = strlen(small_str);

	if (big_len >= small_len) {
		for (i = 0; i <= (big_len - small_len); i++) {
			if (mg_strncasecmp(big_str + i, small_str, small_len) == 0) {
				return big_str + i;
			}
		}
	}

	return NULL;
}


/* Return null terminated string of given maximum length.
 * Report errors if length is exceeded. */
static void
mg_vsnprintf(const struct mg_connection *conn,
             int *truncated,
             char *buf,
             size_t buflen,
             const char *fmt,
             va_list ap)
{
	int n, ok;

	if (buflen == 0) {
		if (truncated) {
			*truncated = 1;
		}
		return;
	}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
	/* Using fmt as a non-literal is intended here, since it is mostly called
	 * indirectly by mg_snprintf */
#endif

	n = (int)vsnprintf_impl(buf, buflen, fmt, ap);
	ok = (n >= 0) && ((size_t)n < buflen);

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

	if (ok) {
		if (truncated) {
			*truncated = 0;
		}
	} else {
		if (truncated) {
			*truncated = 1;
		}
		mg_cry_internal(conn,
		                "truncating vsnprintf buffer: [%.*s]",
		                (int)((buflen > 200) ? 200 : (buflen - 1)),
		                buf);
		n = (int)buflen - 1;
	}
	buf[n] = '\0';
}


static void
mg_snprintf(const struct mg_connection *conn,
            int *truncated,
            char *buf,
            size_t buflen,
            const char *fmt,
            ...)
{
	va_list ap;

	va_start(ap, fmt);
	mg_vsnprintf(conn, truncated, buf, buflen, fmt, ap);
	va_end(ap);
}


static int
get_option_index(const char *name)
{
	int i;

	for (i = 0; config_options[i].name != NULL; i++) {
		if (strcmp(config_options[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}


CIVETWEB_API const char *
mg_get_option(const struct mg_context *ctx, const char *name)
{
	int i;
	if ((i = get_option_index(name)) == -1) {
		return NULL;
	} else if (!ctx || ctx->dd.config[i] == NULL) {
		return "";
	} else {
		return ctx->dd.config[i];
	}
}

#define mg_get_option DO_NOT_USE_THIS_FUNCTION_INTERNALLY__access_directly

CIVETWEB_API struct mg_context *
mg_get_context(const struct mg_connection *conn)
{
	return (conn == NULL) ? (struct mg_context *)NULL : (conn->phys_ctx);
}


CIVETWEB_API void *
mg_get_user_data(const struct mg_context *ctx)
{
	return (ctx == NULL) ? NULL : ctx->user_data;
}


CIVETWEB_API void *
mg_get_user_context_data(const struct mg_connection *conn)
{
	return mg_get_user_data(mg_get_context(conn));
}


CIVETWEB_API void *
mg_get_thread_pointer(const struct mg_connection *conn)
{
	/* both methods should return the same pointer */
	if (conn) {
		/* quick access, in case conn is known */
		return conn->tls_user_ptr;
	} else {
		/* otherwise get pointer from thread local storage (TLS) */
		struct mg_workerTLS *tls =
		    (struct mg_workerTLS *)pthread_getspecific(sTlsKey);
		return tls->user_ptr;
	}
}


CIVETWEB_API void
mg_set_user_connection_data(const struct mg_connection *const_conn, void *data)
{
	if (const_conn != NULL) {
		/* Const cast, since "const struct mg_connection *" does not mean
		 * the connection object is not modified. Here "const" is used,
		 * to indicate mg_read/mg_write/mg_send/.. must not be called. */
		struct mg_connection *conn = (struct mg_connection *)const_conn;
		conn->request_info.conn_data = data;
	}
}


CIVETWEB_API void *
mg_get_user_connection_data(const struct mg_connection *conn)
{
	if (conn != NULL) {
		return conn->request_info.conn_data;
	}
	return NULL;
}


CIVETWEB_API int
mg_get_server_ports(const struct mg_context *ctx,
                    int size,
                    struct mg_server_port *ports)
{
	int i, cnt = 0;

	if (size <= 0) {
		return -1;
	}
	memset(ports, 0, sizeof(*ports) * (size_t)size);
	if (!ctx) {
		return -1;
	}
	if (!ctx->listening_sockets) {
		return -1;
	}

	for (i = 0; (i < size) && (i < (int)ctx->num_listening_sockets); i++) {

		ports[cnt].port =
		    ntohs(USA_IN_PORT_UNSAFE(&(ctx->listening_sockets[i].lsa)));
		ports[cnt].is_ssl = ctx->listening_sockets[i].is_ssl;
		ports[cnt].is_redirect = ctx->listening_sockets[i].ssl_redir;
		ports[cnt].is_optional = ctx->listening_sockets[i].is_optional;

		if (ctx->listening_sockets[i].lsa.sa.sa_family == AF_INET) {
			/* IPv4 */
			ports[cnt].protocol = 1;
			cnt++;
		} else if (ctx->listening_sockets[i].lsa.sa.sa_family == AF_INET6) {
			/* IPv6 */
			ports[cnt].protocol = 3;
			cnt++;
		}
	}

	return cnt;
}


#if defined(USE_X_DOM_SOCKET) && !defined(UNIX_DOMAIN_SOCKET_SERVER_NAME)
#define UNIX_DOMAIN_SOCKET_SERVER_NAME "*"
#endif

static void
sockaddr_to_string(char *buf, size_t len, const union usa *usa)
{
	buf[0] = '\0';

	if (!usa) {
		return;
	}

	if (usa->sa.sa_family == AF_INET) {
		getnameinfo(&usa->sa,
		            sizeof(usa->sin),
		            buf,
		            (unsigned)len,
		            NULL,
		            0,
		            NI_NUMERICHOST);
	}
#if defined(USE_IPV6)
	else if (usa->sa.sa_family == AF_INET6) {
		getnameinfo(&usa->sa,
		            sizeof(usa->sin6),
		            buf,
		            (unsigned)len,
		            NULL,
		            0,
		            NI_NUMERICHOST);
	}
#endif
#if defined(USE_X_DOM_SOCKET)
	else if (usa->sa.sa_family == AF_UNIX) {
		/* TODO: Define a remote address for unix domain sockets.
		* This code will always return "localhost", identical to http+tcp:
		getnameinfo(&usa->sa,
		sizeof(usa->sun),
		buf,
		(unsigned)len,
		NULL,
		0,
		NI_NUMERICHOST);
		*/
		mg_strlcpy(buf, UNIX_DOMAIN_SOCKET_SERVER_NAME, len);
	}
#endif
}


/* Convert time_t to a string. According to RFC2616, Sec 14.18, this must be
 * included in all responses other than 100, 101, 5xx. */
static void
gmt_time_string(char *buf, size_t buf_len, time_t *t)
{
#if !defined(REENTRANT_TIME)
	struct tm *tm;

	tm = ((t != NULL) ? gmtime(t) : NULL);
	if (tm != NULL) {
#else
	struct tm _tm;
	struct tm *tm = &_tm;

	if (t != NULL) {
		gmtime_r(t, tm);
#endif
		strftime(buf, buf_len, "%a, %d %b %Y %H:%M:%S GMT", tm);
	} else {
		mg_strlcpy(buf, "Thu, 01 Jan 1970 00:00:00 GMT", buf_len);
	}
}


/* difftime for struct timespec. Return value is in seconds. */
static double
mg_difftimespec(const struct timespec *ts_now, const struct timespec *ts_before)
{
	return (double)(ts_now->tv_nsec - ts_before->tv_nsec) * 1.0E-9
	       + (double)(ts_now->tv_sec - ts_before->tv_sec);
}


#if defined(MG_EXTERNAL_FUNCTION_mg_cry_internal_impl)
static void mg_cry_internal_impl(const struct mg_connection *conn,
                                 const char *func,
                                 unsigned line,
                                 const char *fmt,
                                 va_list ap);
#include "external_mg_cry_internal_impl.inl"
#elif !defined(NO_FILESYSTEMS)

/* Print error message to the opened error log stream. */
static void
mg_cry_internal_impl(const struct mg_connection *conn,
                     const char *func,
                     unsigned line,
                     const char *fmt,
                     va_list ap)
{
	char buf[MG_BUF_LEN], src_addr[IP_ADDR_STR_LEN];
	struct mg_file fi;
	time_t timestamp;

	/* Unused, in the RELEASE build */
	(void)func;
	(void)line;

#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

	IGNORE_UNUSED_RESULT(vsnprintf_impl(buf, sizeof(buf), fmt, ap));

#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic pop
#endif

	buf[sizeof(buf) - 1] = 0;

	DEBUG_TRACE("mg_cry called from %s:%u: %s", func, line, buf);

	if (!conn) {
		puts(buf);
		return;
	}

	/* Do not lock when getting the callback value, here and below.
	 * I suppose this is fine, since function cannot disappear in the
	 * same way string option can. */
	if ((conn->phys_ctx->callbacks.log_message == NULL)
	    || (conn->phys_ctx->callbacks.log_message(conn, buf) == 0)) {

		if (conn->dom_ctx->config[ERROR_LOG_FILE] != NULL) {
			if (mg_fopen(conn,
			             conn->dom_ctx->config[ERROR_LOG_FILE],
			             MG_FOPEN_MODE_APPEND,
			             &fi)
			    == 0) {
				fi.access.fp = NULL;
			}
		} else {
			fi.access.fp = NULL;
		}

		if (fi.access.fp != NULL) {
			flockfile(fi.access.fp);
			timestamp = time(NULL);

			sockaddr_to_string(src_addr, sizeof(src_addr), &conn->client.rsa);
			fprintf(fi.access.fp,
			        "[%010lu] [error] [client %s] ",
			        (unsigned long)timestamp,
			        src_addr);

			if (conn->request_info.request_method != NULL) {
				fprintf(fi.access.fp,
				        "%s %s: ",
				        conn->request_info.request_method,
				        conn->request_info.request_uri
				            ? conn->request_info.request_uri
				            : "");
			}

			fprintf(fi.access.fp, "%s", buf);
			fputc('\n', fi.access.fp);
			fflush(fi.access.fp);
			funlockfile(fi.access.fp);
			(void)mg_fclose(&fi.access); /* Ignore errors. We can't call
			                              * mg_cry here anyway ;-) */
		}
	}
}
#else
#error Must either enable filesystems or provide a custom mg_cry_internal_impl implementation
#endif /* Externally provided function */


/* Construct fake connection structure. Used for logging, if connection
 * is not applicable at the moment of logging. */
static struct mg_connection *
fake_connection(struct mg_connection *fc, struct mg_context *ctx)
{
	static const struct mg_connection conn_zero = {0};
	*fc = conn_zero;
	fc->phys_ctx = ctx;
	fc->dom_ctx = &(ctx->dd);
	return fc;
}


static void
mg_cry_internal_wrap(const struct mg_connection *conn,
                     struct mg_context *ctx,
                     const char *func,
                     unsigned line,
                     const char *fmt,
                     ...)
{
	va_list ap;
	va_start(ap, fmt);
	if (!conn && ctx) {
		struct mg_connection fc;
		mg_cry_internal_impl(fake_connection(&fc, ctx), func, line, fmt, ap);
	} else {
		mg_cry_internal_impl(conn, func, line, fmt, ap);
	}
	va_end(ap);
}


CIVETWEB_API void
mg_cry(const struct mg_connection *conn, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	mg_cry_internal_impl(conn, "user", 0, fmt, ap);
	va_end(ap);
}


#define mg_cry DO_NOT_USE_THIS_FUNCTION__USE_mg_cry_internal


CIVETWEB_API const char *
mg_version(void)
{
	return CIVETWEB_VERSION;
}


CIVETWEB_API const struct mg_request_info *
mg_get_request_info(const struct mg_connection *conn)
{
	if (!conn) {
		return NULL;
	}
#if defined(MG_ALLOW_USING_GET_REQUEST_INFO_FOR_RESPONSE)
	if (conn->connection_type == CONNECTION_TYPE_RESPONSE) {
		char txt[16];
		struct mg_workerTLS *tls =
		    (struct mg_workerTLS *)pthread_getspecific(sTlsKey);

		sprintf(txt, "%03i", conn->response_info.status_code);
		if (strlen(txt) == 3) {
			memcpy(tls->txtbuf, txt, 4);
		} else {
			strcpy(tls->txtbuf, "ERR");
		}

		((struct mg_connection *)conn)->request_info.local_uri =
		    tls->txtbuf; /* use thread safe buffer */
		((struct mg_connection *)conn)->request_info.local_uri_raw =
		    tls->txtbuf; /* use the same thread safe buffer */
		((struct mg_connection *)conn)->request_info.request_uri =
		    tls->txtbuf; /* use  the same thread safe buffer */

		((struct mg_connection *)conn)->request_info.num_headers =
		    conn->response_info.num_headers;
		memcpy(((struct mg_connection *)conn)->request_info.http_headers,
		       conn->response_info.http_headers,
		       sizeof(conn->response_info.http_headers));
	} else
#endif
	    if (conn->connection_type != CONNECTION_TYPE_REQUEST) {
		return NULL;
	}
	return &conn->request_info;
}


CIVETWEB_API const struct mg_response_info *
mg_get_response_info(const struct mg_connection *conn)
{
	if (!conn) {
		return NULL;
	}
	if (conn->connection_type != CONNECTION_TYPE_RESPONSE) {
		return NULL;
	}
	return &conn->response_info;
}


static const char *
get_proto_name(const struct mg_connection *conn)
{
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
	/* Depending on USE_WEBSOCKET and NO_SSL, some oft the protocols might be
	 * not supported. Clang raises an "unreachable code" warning for parts of ?:
	 * unreachable, but splitting into four different #ifdef clauses here is
	 * more complicated.
	 */
#endif

	const struct mg_request_info *ri = &conn->request_info;

	const char *proto = ((conn->protocol_type == PROTOCOL_TYPE_WEBSOCKET)
	                         ? (ri->is_ssl ? "wss" : "ws")
	                         : (ri->is_ssl ? "https" : "http"));

	return proto;

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}


static int
mg_construct_local_link(const struct mg_connection *conn,
                        char *buf,
                        size_t buflen,
                        const char *define_proto,
                        int define_port,
                        const char *define_uri)
{
	if ((buflen < 1) || (buf == 0) || (conn == 0)) {
		return -1;
	} else {
		int i, j;
		int truncated = 0;
		const struct mg_request_info *ri = &conn->request_info;

		const char *proto =
		    (define_proto != NULL) ? define_proto : get_proto_name(conn);
		const char *uri =
		    (define_uri != NULL)
		        ? define_uri
		        : ((ri->request_uri != NULL) ? ri->request_uri : ri->local_uri);
		int port = (define_port > 0) ? define_port : ri->server_port;
		int default_port = 80;
		char *uri_encoded;
		size_t uri_encoded_len;

		if (uri == NULL) {
			return -1;
		}

		uri_encoded_len = strlen(uri) * 3 + 1;
		uri_encoded = (char *)mg_malloc_ctx(uri_encoded_len, conn->phys_ctx);
		if (uri_encoded == NULL) {
			return -1;
		}
		mg_url_encode(uri, uri_encoded, uri_encoded_len);

		/* Directory separator should be preserved. */
		for (i = j = 0; uri_encoded[i]; j++) {
			if (!strncmp(uri_encoded + i, "%2f", 3)) {
				uri_encoded[j] = '/';
				i += 3;
			} else {
				uri_encoded[j] = uri_encoded[i++];
			}
		}
		uri_encoded[j] = '\0';

#if defined(USE_X_DOM_SOCKET)
		if (conn->client.lsa.sa.sa_family == AF_UNIX) {
			/* TODO: Define and document a link for UNIX domain sockets. */
			/* There seems to be no official standard for this.
			 * Common uses seem to be "httpunix://", "http.unix://" or
			 * "http+unix://" as a protocol definition string, followed by
			 * "localhost" or "127.0.0.1" or "/tmp/unix/path" or
			 * "%2Ftmp%2Funix%2Fpath" (url % encoded) or
			 * "localhost:%2Ftmp%2Funix%2Fpath" (domain socket path as port) or
			 * "" (completely skipping the server name part). In any case, the
			 * last part is the server local path. */
			const char *server_name = UNIX_DOMAIN_SOCKET_SERVER_NAME;
			mg_snprintf(conn,
			            &truncated,
			            buf,
			            buflen,
			            "%s.unix://%s%s",
			            proto,
			            server_name,
			            ri->local_uri);
			default_port = 0;
			mg_free(uri_encoded);
			return 0;
		}
#endif

		if (define_proto) {
			/* If we got a protocol name, use the default port accordingly. */
			if ((0 == strcmp(define_proto, "https"))
			    || (0 == strcmp(define_proto, "wss"))) {
				default_port = 443;
			}
		} else if (ri->is_ssl) {
			/* If we did not get a protocol name, use TLS as default if it is
			 * already used. */
			default_port = 443;
		}

		{
#if defined(USE_IPV6)
			int is_ipv6 = (conn->client.lsa.sa.sa_family == AF_INET6);
#endif
			int auth_domain_check_enabled =
			    conn->dom_ctx->config[ENABLE_AUTH_DOMAIN_CHECK]
			    && (!mg_strcasecmp(
			           conn->dom_ctx->config[ENABLE_AUTH_DOMAIN_CHECK], "yes"));

			const char *server_domain =
			    conn->dom_ctx->config[AUTHENTICATION_DOMAIN];

			char portstr[16];
			char server_ip[48];

			if (port != default_port) {
				sprintf(portstr, ":%u", (unsigned)port);
			} else {
				portstr[0] = 0;
			}

			if (!auth_domain_check_enabled || !server_domain) {

				sockaddr_to_string(server_ip,
				                   sizeof(server_ip),
				                   &conn->client.lsa);

				server_domain = server_ip;
			}

			mg_snprintf(conn,
			            &truncated,
			            buf,
			            buflen,
#if defined(USE_IPV6)
			            "%s://%s%s%s%s%s",
			            proto,
			            (is_ipv6 && (server_domain == server_ip)) ? "[" : "",
			            server_domain,
			            (is_ipv6 && (server_domain == server_ip)) ? "]" : "",
#else
			            "%s://%s%s%s",
			            proto,
			            server_domain,
#endif
			            portstr,
			            uri_encoded);

			mg_free(uri_encoded);
			if (truncated) {
				return -1;
			}
			return 0;
		}
	}
}


CIVETWEB_API int
mg_get_request_link(const struct mg_connection *conn, char *buf, size_t buflen)
{
	return mg_construct_local_link(conn, buf, buflen, NULL, -1, NULL);
}


/* Skip the characters until one of the delimiters characters found.
 * 0-terminate resulting word. Skip the delimiter and following whitespaces.
 * Advance pointer to buffer to the next word. Return found 0-terminated
 * word.
 * Delimiters can be quoted with quotechar. */
static char *
skip_quoted(char **buf,
            const char *delimiters,
            const char *whitespace,
            char quotechar)
{
	char *p, *begin_word, *end_word, *end_whitespace;

	begin_word = *buf;
	end_word = begin_word + strcspn(begin_word, delimiters);

	/* Check for quotechar */
	if (end_word > begin_word) {
		p = end_word - 1;
		while (*p == quotechar) {
			/* While the delimiter is quoted, look for the next delimiter. */
			/* This happens, e.g., in calls from parse_auth_header,
			 * if the user name contains a " character. */

			/* If there is anything beyond end_word, copy it. */
			if (*end_word != '\0') {
				size_t end_off = strcspn(end_word + 1, delimiters);
				memmove(p, end_word, end_off + 1);
				p += end_off; /* p must correspond to end_word - 1 */
				end_word += end_off + 1;
			} else {
				*p = '\0';
				break;
			}
		}
		for (p++; p < end_word; p++) {
			*p = '\0';
		}
	}

	if (*end_word == '\0') {
		*buf = end_word;
	} else {

#if defined(GCC_DIAGNOSTIC)
		/* Disable spurious conversion warning for GCC */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif /* defined(GCC_DIAGNOSTIC) */

		end_whitespace = end_word + strspn(&end_word[1], whitespace) + 1;

#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic pop
#endif /* defined(GCC_DIAGNOSTIC) */

		for (p = end_word; p < end_whitespace; p++) {
			*p = '\0';
		}

		*buf = end_whitespace;
	}

	return begin_word;
}


/* Return HTTP header value, or NULL if not found. */
static const char *
get_header(const struct mg_header *hdr, int num_hdr, const char *name)
{
	int i;
	for (i = 0; i < num_hdr; i++) {
		if (!mg_strcasecmp(name, hdr[i].name)) {
			return hdr[i].value;
		}
	}

	return NULL;
}


/* Retrieve requested HTTP header multiple values, and return the number of
 * found occurrences */
static int
get_req_headers(const struct mg_request_info *ri,
                const char *name,
                const char **output,
                int output_max_size)
{
	int i;
	int cnt = 0;
	if (ri) {
		for (i = 0; i < ri->num_headers && cnt < output_max_size; i++) {
			if (!mg_strcasecmp(name, ri->http_headers[i].name)) {
				output[cnt++] = ri->http_headers[i].value;
			}
		}
	}
	return cnt;
}


CIVETWEB_API const char *
mg_get_header(const struct mg_connection *conn, const char *name)
{
	if (!conn) {
		return NULL;
	}

	if (conn->connection_type == CONNECTION_TYPE_REQUEST) {
		return get_header(conn->request_info.http_headers,
		                  conn->request_info.num_headers,
		                  name);
	}
	if (conn->connection_type == CONNECTION_TYPE_RESPONSE) {
		return get_header(conn->response_info.http_headers,
		                  conn->response_info.num_headers,
		                  name);
	}
	return NULL;
}


static const char *
get_http_version(const struct mg_connection *conn)
{
	if (!conn) {
		return NULL;
	}

	if (conn->connection_type == CONNECTION_TYPE_REQUEST) {
		return conn->request_info.http_version;
	}
	if (conn->connection_type == CONNECTION_TYPE_RESPONSE) {
		return conn->response_info.http_version;
	}
	return NULL;
}


/* A helper function for traversing a comma separated list of values.
 * It returns a list pointer shifted to the next value, or NULL if the end
 * of the list found.
 * Value is stored in val vector. If value has form "x=y", then eq_val
 * vector is initialized to point to the "y" part, and val vector length
 * is adjusted to point only to "x". */
static const char *
next_option(const char *list, struct vec *val, struct vec *eq_val)
{
	int end;

reparse:
	if (val == NULL || list == NULL || *list == '\0') {
		/* End of the list */
		return NULL;
	}

	/* Skip over leading LWS */
	while (*list == ' ' || *list == '\t')
		list++;

	val->ptr = list;
	if ((list = strchr(val->ptr, ',')) != NULL) {
		/* Comma found. Store length and shift the list ptr */
		val->len = ((size_t)(list - val->ptr));
		list++;
	} else {
		/* This value is the last one */
		list = val->ptr + strlen(val->ptr);
		val->len = ((size_t)(list - val->ptr));
	}

	/* Adjust length for trailing LWS */
	end = (int)val->len - 1;
	while (end >= 0 && ((val->ptr[end] == ' ') || (val->ptr[end] == '\t')))
		end--;
	val->len = (size_t)(end) + (size_t)(1);

	if (val->len == 0) {
		/* Ignore any empty entries. */
		goto reparse;
	}

	if (eq_val != NULL) {
		/* Value has form "x=y", adjust pointers and lengths
		 * so that val points to "x", and eq_val points to "y". */
		eq_val->len = 0;
		eq_val->ptr = (const char *)memchr(val->ptr, '=', val->len);
		if (eq_val->ptr != NULL) {
			eq_val->ptr++; /* Skip over '=' character */
			eq_val->len = ((size_t)(val->ptr - eq_val->ptr)) + val->len;
			val->len = ((size_t)(eq_val->ptr - val->ptr)) - 1;
		}
	}

	return list;
}


/* A helper function for checking if a comma separated list of values
 * contains
 * the given option (case insensitvely).
 * 'header' can be NULL, in which case false is returned. */
static int
header_has_option(const char *header, const char *option)
{
	struct vec opt_vec;
	struct vec eq_vec;

	DEBUG_ASSERT(option != NULL);
	DEBUG_ASSERT(option[0] != '\0');

	while ((header = next_option(header, &opt_vec, &eq_vec)) != NULL) {
		if (mg_strncasecmp(option, opt_vec.ptr, opt_vec.len) == 0)
			return 1;
	}

	return 0;
}


/* Sorting function implemented in a separate file */
#include "sort.inl"

/* Pattern matching has been reimplemented in a new file */
#include "match.inl"


/* HTTP 1.1 assumes keep alive if "Connection:" header is not set
 * This function must tolerate situations when connection info is not
 * set up, for example if request parsing failed. */
static int
should_keep_alive(const struct mg_connection *conn)
{
	const char *http_version;
	const char *header;

	/* First satisfy needs of the server */
	if ((conn == NULL) || conn->must_close) {
		/* Close, if civetweb framework needs to close */
		return 0;
	}

	if (mg_strcasecmp(conn->dom_ctx->config[ENABLE_KEEP_ALIVE], "yes") != 0) {
		/* Close, if keep alive is not enabled */
		return 0;
	}

	/* Check explicit wish of the client */
	header = mg_get_header(conn, "Connection");
	if (header) {
		/* If there is a connection header from the client, obey */
		if (header_has_option(header, "keep-alive")) {
			return 1;
		}
		return 0;
	}

	/* Use default of the standard */
	http_version = get_http_version(conn);
	if (http_version && (0 == strcmp(http_version, "1.1"))) {
		/* HTTP 1.1 default is keep alive */
		return 1;
	}

	/* HTTP 1.0 (and earlier) default is to close the connection */
	return 0;
}


static int
should_decode_url(const struct mg_connection *conn)
{
	if (!conn || !conn->dom_ctx) {
		return 0;
	}

	return (mg_strcasecmp(conn->dom_ctx->config[DECODE_URL], "yes") == 0);
}


static int
should_decode_query_string(const struct mg_connection *conn)
{
	if (!conn || !conn->dom_ctx) {
		return 0;
	}

	return (mg_strcasecmp(conn->dom_ctx->config[DECODE_QUERY_STRING], "yes")
	        == 0);
}


static const char *
suggest_connection_header(const struct mg_connection *conn)
{
	return should_keep_alive(conn) ? "keep-alive" : "close";
}


#include "response.inl"


static void
send_no_cache_header(struct mg_connection *conn)
{
	/* Send all current and obsolete cache opt-out directives. */
	mg_response_header_add(conn,
	                       "Cache-Control",
	                       "no-cache, no-store, "
	                       "must-revalidate, private, max-age=0",
	                       -1);
	mg_response_header_add(conn, "Expires", "0", -1);

	if (conn->protocol_type == PROTOCOL_TYPE_HTTP1) {
		/* Obsolete, but still send it for HTTP/1.0 */
		mg_response_header_add(conn, "Pragma", "no-cache", -1);
	}
}


static void
send_static_cache_header(struct mg_connection *conn)
{
#if !defined(NO_CACHING)
	int max_age;
	char val[64];

	const char *cache_control =
	    conn->dom_ctx->config[STATIC_FILE_CACHE_CONTROL];

	/* If there is a full cache-control option configured,0 use it */
	if (cache_control != NULL) {
		mg_response_header_add(conn, "Cache-Control", cache_control, -1);
		return;
	}

	/* Read the server config to check how long a file may be cached.
	 * The configuration is in seconds. */
	max_age = atoi(conn->dom_ctx->config[STATIC_FILE_MAX_AGE]);
	if (max_age <= 0) {
		/* 0 means "do not cache". All values <0 are reserved
		 * and may be used differently in the future. */
		/* If a file should not be cached, do not only send
		 * max-age=0, but also pragmas and Expires headers. */
		send_no_cache_header(conn);
		return;
	}

	/* Use "Cache-Control: max-age" instead of "Expires" header.
	 * Reason: see https://www.mnot.net/blog/2007/05/15/expires_max-age */
	/* See also https://www.mnot.net/cache_docs/ */
	/* According to RFC 2616, Section 14.21, caching times should not exceed
	 * one year. A year with 365 days corresponds to 31536000 seconds, a
	 * leap
	 * year to 31622400 seconds. For the moment, we just send whatever has
	 * been configured, still the behavior for >1 year should be considered
	 * as undefined. */
	mg_snprintf(
	    conn, NULL, val, sizeof(val), "max-age=%lu", (unsigned long)max_age);
	mg_response_header_add(conn, "Cache-Control", val, -1);

#else  /* NO_CACHING */

	send_no_cache_header(conn);
#endif /* !NO_CACHING */
}


static void
send_additional_header(struct mg_connection *conn)
{
	const char *header = conn->dom_ctx->config[ADDITIONAL_HEADER];

#if !defined(NO_SSL)
	if (conn->dom_ctx->config[STRICT_HTTPS_MAX_AGE]) {
		long max_age = atol(conn->dom_ctx->config[STRICT_HTTPS_MAX_AGE]);
		if (max_age >= 0) {
			char val[64];
			mg_snprintf(conn,
			            NULL,
			            val,
			            sizeof(val),
			            "max-age=%lu",
			            (unsigned long)max_age);
			mg_response_header_add(conn, "Strict-Transport-Security", val, -1);
		}
	}
#endif

	// Content-Security-Policy

	if (header && header[0]) {
		mg_response_header_add_lines(conn, header);
	}
}


static void
send_cors_header(struct mg_connection *conn)
{
	const char *origin_hdr = mg_get_header(conn, "Origin");
	const char *cors_orig_cfg =
	    conn->dom_ctx->config[ACCESS_CONTROL_ALLOW_ORIGIN];
	const char *cors_cred_cfg =
	    conn->dom_ctx->config[ACCESS_CONTROL_ALLOW_CREDENTIALS];
	const char *cors_hdr_cfg =
	    conn->dom_ctx->config[ACCESS_CONTROL_ALLOW_HEADERS];
	const char *cors_exphdr_cfg =
	    conn->dom_ctx->config[ACCESS_CONTROL_EXPOSE_HEADERS];
	const char *cors_meth_cfg =
	    conn->dom_ctx->config[ACCESS_CONTROL_ALLOW_METHODS];

	if (cors_orig_cfg && *cors_orig_cfg && origin_hdr && *origin_hdr) {
		/* Cross-origin resource sharing (CORS), see
		 * http://www.html5rocks.com/en/tutorials/cors/,
		 * http://www.html5rocks.com/static/images/cors_server_flowchart.png
		 * CORS preflight is not supported for files. */
		mg_response_header_add(conn,
		                       "Access-Control-Allow-Origin",
		                       cors_orig_cfg,
		                       -1);
	}

	if (cors_cred_cfg && *cors_cred_cfg && origin_hdr && *origin_hdr) {
		/* Cross-origin resource sharing (CORS), see
		 * https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Access-Control-Allow-Credentials
		 */
		mg_response_header_add(conn,
		                       "Access-Control-Allow-Credentials",
		                       cors_cred_cfg,
		                       -1);
	}

	if (cors_hdr_cfg && *cors_hdr_cfg) {
		mg_response_header_add(conn,
		                       "Access-Control-Allow-Headers",
		                       cors_hdr_cfg,
		                       -1);
	}

	if (cors_exphdr_cfg && *cors_exphdr_cfg) {
		mg_response_header_add(conn,
		                       "Access-Control-Expose-Headers",
		                       cors_exphdr_cfg,
		                       -1);
	}

	if (cors_meth_cfg && *cors_meth_cfg) {
		mg_response_header_add(conn,
		                       "Access-Control-Allow-Methods",
		                       cors_meth_cfg,
		                       -1);
	}
}


#if !defined(NO_FILESYSTEMS)
static void handle_file_based_request(struct mg_connection *conn,
                                      const char *path,
                                      struct mg_file *filep);
#endif /* NO_FILESYSTEMS */


CIVETWEB_API const char *
mg_get_response_code_text(const struct mg_connection *conn, int response_code)
{
	/* See IANA HTTP status code assignment:
	 * http://www.iana.org/assignments/http-status-codes/http-status-codes.xhtml
	 */

	switch (response_code) {
		/* RFC2616 Section 10.1 - Informational 1xx */
	case 100:
		return "Continue"; /* RFC2616 Section 10.1.1 */
	case 101:
		return "Switching Protocols"; /* RFC2616 Section 10.1.2 */
	case 102:
		return "Processing"; /* RFC2518 Section 10.1 */

		/* RFC2616 Section 10.2 - Successful 2xx */
	case 200:
		return "OK"; /* RFC2616 Section 10.2.1 */
	case 201:
		return "Created"; /* RFC2616 Section 10.2.2 */
	case 202:
		return "Accepted"; /* RFC2616 Section 10.2.3 */
	case 203:
		return "Non-Authoritative Information"; /* RFC2616 Section 10.2.4 */
	case 204:
		return "No Content"; /* RFC2616 Section 10.2.5 */
	case 205:
		return "Reset Content"; /* RFC2616 Section 10.2.6 */
	case 206:
		return "Partial Content"; /* RFC2616 Section 10.2.7 */
	case 207:
		return "Multi-Status"; /* RFC2518 Section 10.2, RFC4918 Section 11.1
		                        */
	case 208:
		return "Already Reported"; /* RFC5842 Section 7.1 */

	case 226:
		return "IM used"; /* RFC3229 Section 10.4.1 */

		/* RFC2616 Section 10.3 - Redirection 3xx */
	case 300:
		return "Multiple Choices"; /* RFC2616 Section 10.3.1 */
	case 301:
		return "Moved Permanently"; /* RFC2616 Section 10.3.2 */
	case 302:
		return "Found"; /* RFC2616 Section 10.3.3 */
	case 303:
		return "See Other"; /* RFC2616 Section 10.3.4 */
	case 304:
		return "Not Modified"; /* RFC2616 Section 10.3.5 */
	case 305:
		return "Use Proxy"; /* RFC2616 Section 10.3.6 */
	case 307:
		return "Temporary Redirect"; /* RFC2616 Section 10.3.8 */
	case 308:
		return "Permanent Redirect"; /* RFC7238 Section 3 */

		/* RFC2616 Section 10.4 - Client Error 4xx */
	case 400:
		return "Bad Request"; /* RFC2616 Section 10.4.1 */
	case 401:
		return "Unauthorized"; /* RFC2616 Section 10.4.2 */
	case 402:
		return "Payment Required"; /* RFC2616 Section 10.4.3 */
	case 403:
		return "Forbidden"; /* RFC2616 Section 10.4.4 */
	case 404:
		return "Not Found"; /* RFC2616 Section 10.4.5 */
	case 405:
		return "Method Not Allowed"; /* RFC2616 Section 10.4.6 */
	case 406:
		return "Not Acceptable"; /* RFC2616 Section 10.4.7 */
	case 407:
		return "Proxy Authentication Required"; /* RFC2616 Section 10.4.8 */
	case 408:
		return "Request Time-out"; /* RFC2616 Section 10.4.9 */
	case 409:
		return "Conflict"; /* RFC2616 Section 10.4.10 */
	case 410:
		return "Gone"; /* RFC2616 Section 10.4.11 */
	case 411:
		return "Length Required"; /* RFC2616 Section 10.4.12 */
	case 412:
		return "Precondition Failed"; /* RFC2616 Section 10.4.13 */
	case 413:
		return "Request Entity Too Large"; /* RFC2616 Section 10.4.14 */
	case 414:
		return "Request-URI Too Large"; /* RFC2616 Section 10.4.15 */
	case 415:
		return "Unsupported Media Type"; /* RFC2616 Section 10.4.16 */
	case 416:
		return "Requested range not satisfiable"; /* RFC2616 Section 10.4.17
		                                           */
	case 417:
		return "Expectation Failed"; /* RFC2616 Section 10.4.18 */

	case 421:
		return "Misdirected Request"; /* RFC7540 Section 9.1.2 */
	case 422:
		return "Unproccessable entity"; /* RFC2518 Section 10.3, RFC4918
		                                 * Section 11.2 */
	case 423:
		return "Locked"; /* RFC2518 Section 10.4, RFC4918 Section 11.3 */
	case 424:
		return "Failed Dependency"; /* RFC2518 Section 10.5, RFC4918
		                             * Section 11.4 */

	case 426:
		return "Upgrade Required"; /* RFC 2817 Section 4 */

	case 428:
		return "Precondition Required"; /* RFC 6585, Section 3 */
	case 429:
		return "Too Many Requests"; /* RFC 6585, Section 4 */

	case 431:
		return "Request Header Fields Too Large"; /* RFC 6585, Section 5 */

	case 451:
		return "Unavailable For Legal Reasons"; /* draft-tbray-http-legally-restricted-status-05,
		                                         * Section 3 */

		/* RFC2616 Section 10.5 - Server Error 5xx */
	case 500:
		return "Internal Server Error"; /* RFC2616 Section 10.5.1 */
	case 501:
		return "Not Implemented"; /* RFC2616 Section 10.5.2 */
	case 502:
		return "Bad Gateway"; /* RFC2616 Section 10.5.3 */
	case 503:
		return "Service Unavailable"; /* RFC2616 Section 10.5.4 */
	case 504:
		return "Gateway Time-out"; /* RFC2616 Section 10.5.5 */
	case 505:
		return "HTTP Version not supported"; /* RFC2616 Section 10.5.6 */
	case 506:
		return "Variant Also Negotiates"; /* RFC 2295, Section 8.1 */
	case 507:
		return "Insufficient Storage"; /* RFC2518 Section 10.6, RFC4918
		                                * Section 11.5 */
	case 508:
		return "Loop Detected"; /* RFC5842 Section 7.1 */

	case 510:
		return "Not Extended"; /* RFC 2774, Section 7 */
	case 511:
		return "Network Authentication Required"; /* RFC 6585, Section 6 */

		/* Other status codes, not shown in the IANA HTTP status code
		 * assignment.
		 * E.g., "de facto" standards due to common use, ... */
	case 418:
		return "I am a teapot"; /* RFC2324 Section 2.3.2 */
	case 419:
		return "Authentication Timeout"; /* common use */
	case 420:
		return "Enhance Your Calm"; /* common use */
	case 440:
		return "Login Timeout"; /* common use */
	case 509:
		return "Bandwidth Limit Exceeded"; /* common use */

	default:
		/* This error code is unknown. This should not happen. */
		if (conn) {
			mg_cry_internal(conn,
			                "Unknown HTTP response code: %u",
			                response_code);
		}

		/* Return at least a category according to RFC 2616 Section 10. */
		if (response_code >= 100 && response_code < 200) {
			/* Unknown informational status code */
			return "Information";
		}
		if (response_code >= 200 && response_code < 300) {
			/* Unknown success code */
			return "Success";
		}
		if (response_code >= 300 && response_code < 400) {
			/* Unknown redirection code */
			return "Redirection";
		}
		if (response_code >= 400 && response_code < 500) {
			/* Unknown request error code */
			return "Client Error";
		}
		if (response_code >= 500 && response_code < 600) {
			/* Unknown server error code */
			return "Server Error";
		}

		/* Response code not even within reasonable range */
		return "";
	}
}


static int
mg_send_http_error_impl(struct mg_connection *conn,
                        int status,
                        const char *fmt,
                        va_list args)
{
	char errmsg_buf[MG_BUF_LEN];
	va_list ap;
	int has_body;

#if !defined(NO_FILESYSTEMS)
	char path_buf[UTF8_PATH_MAX];
	int len, i, page_handler_found, scope, truncated;
	const char *error_handler = NULL;
	struct mg_file error_page_file = STRUCT_FILE_INITIALIZER;
	const char *error_page_file_ext, *tstr;
#endif /* NO_FILESYSTEMS */
	int handled_by_callback = 0;

	if ((conn == NULL) || (fmt == NULL)) {
		return -2;
	}

	/* Set status (for log) */
	conn->status_code = status;

	/* Errors 1xx, 204 and 304 MUST NOT send a body */
	has_body = ((status > 199) && (status != 204) && (status != 304));

	/* Prepare message in buf, if required */
	if (has_body
	    || (!conn->in_error_handler
	        && (conn->phys_ctx->callbacks.http_error != NULL))) {
		/* Store error message in errmsg_buf */
		va_copy(ap, args);
		mg_vsnprintf(conn, NULL, errmsg_buf, sizeof(errmsg_buf), fmt, ap);
		va_end(ap);
		/* In a debug build, print all html errors */
		DEBUG_TRACE("Error %i - [%s]", status, errmsg_buf);
	}

	/* If there is a http_error callback, call it.
	 * But don't do it recursively, if callback calls mg_send_http_error again.
	 */
	if (!conn->in_error_handler
	    && (conn->phys_ctx->callbacks.http_error != NULL)) {
		/* Mark in_error_handler to avoid recursion and call user callback. */
		conn->in_error_handler = 1;
		handled_by_callback =
		    (conn->phys_ctx->callbacks.http_error(conn, status, errmsg_buf)
		     == 0);
		conn->in_error_handler = 0;
	}

	if (!handled_by_callback) {
		/* Check for recursion */
		if (conn->in_error_handler) {
			DEBUG_TRACE(
			    "Recursion when handling error %u - fall back to default",
			    status);
#if !defined(NO_FILESYSTEMS)
		} else {
			/* Send user defined error pages, if defined */
			error_handler = conn->dom_ctx->config[ERROR_PAGES];
			error_page_file_ext = conn->dom_ctx->config[INDEX_FILES];
			page_handler_found = 0;

			if (error_handler != NULL) {
				for (scope = 1; (scope <= 3) && !page_handler_found; scope++) {
					switch (scope) {
					case 1: /* Handler for specific error, e.g. 404 error */
						mg_snprintf(conn,
						            &truncated,
						            path_buf,
						            sizeof(path_buf) - 32,
						            "%serror%03u.",
						            error_handler,
						            status);
						break;
					case 2: /* Handler for error group, e.g., 5xx error
					         * handler
					         * for all server errors (500-599) */
						mg_snprintf(conn,
						            &truncated,
						            path_buf,
						            sizeof(path_buf) - 32,
						            "%serror%01uxx.",
						            error_handler,
						            status / 100);
						break;
					default: /* Handler for all errors */
						mg_snprintf(conn,
						            &truncated,
						            path_buf,
						            sizeof(path_buf) - 32,
						            "%serror.",
						            error_handler);
						break;
					}

					/* String truncation in buf may only occur if
					 * error_handler is too long. This string is
					 * from the config, not from a client. */
					(void)truncated;

					/* The following code is redundant, but it should avoid
					 * false positives in static source code analyzers and
					 * vulnerability scanners.
					 */
					path_buf[sizeof(path_buf) - 32] = 0;
					len = (int)strlen(path_buf);
					if (len > (int)sizeof(path_buf) - 32) {
						len = (int)sizeof(path_buf) - 32;
					}

					/* Start with the file extension from the configuration. */
					tstr = strchr(error_page_file_ext, '.');

					while (tstr) {
						for (i = 1;
						     (i < 32) && (tstr[i] != 0) && (tstr[i] != ',');
						     i++) {
							/* buffer overrun is not possible here, since
							 * (i < 32) && (len < sizeof(path_buf) - 32)
							 * ==> (i + len) < sizeof(path_buf) */
							path_buf[len + i - 1] = tstr[i];
						}
						/* buffer overrun is not possible here, since
						 * (i <= 32) && (len < sizeof(path_buf) - 32)
						 * ==> (i + len) <= sizeof(path_buf) */
						path_buf[len + i - 1] = 0;

						if (mg_stat(conn, path_buf, &error_page_file.stat)) {
							DEBUG_TRACE("Check error page %s - found",
							            path_buf);
							page_handler_found = 1;
							break;
						}
						DEBUG_TRACE("Check error page %s - not found",
						            path_buf);

						/* Continue with the next file extension from the
						 * configuration (if there is a next one). */
						tstr = strchr(tstr + i, '.');
					}
				}
			}

			if (page_handler_found) {
				conn->in_error_handler = 1;
				handle_file_based_request(conn, path_buf, &error_page_file);
				conn->in_error_handler = 0;
				return 0;
			}
#endif /* NO_FILESYSTEMS */
		}

		/* No custom error page. Send default error page. */
		conn->must_close = 1;
		mg_response_header_start(conn, status);
		send_no_cache_header(conn);
		send_additional_header(conn);
		send_cors_header(conn);
		if (has_body) {
			mg_response_header_add(conn,
			                       "Content-Type",
			                       "text/plain; charset=utf-8",
			                       -1);
		}
		mg_response_header_send(conn);

		/* HTTP responses 1xx, 204 and 304 MUST NOT send a body */
		if (has_body) {
			/* For other errors, send a generic error message. */
			const char *status_text = mg_get_response_code_text(conn, status);
			mg_printf(conn, "Error %d: %s\n", status, status_text);
			mg_write(conn, errmsg_buf, strlen(errmsg_buf));

		} else {
			/* No body allowed. Close the connection. */
			DEBUG_TRACE("Error %i", status);
		}
	}
	return 0;
}


CIVETWEB_API int
mg_send_http_error(struct mg_connection *conn, int status, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = mg_send_http_error_impl(conn, status, fmt, ap);
	va_end(ap);

	return ret;
}


CIVETWEB_API int
mg_send_http_ok(struct mg_connection *conn,
                const char *mime_type,
                long long content_length)
{
	if ((mime_type == NULL) || (*mime_type == 0)) {
		/* No content type defined: default to text/html */
		mime_type = "text/html";
	}

	mg_response_header_start(conn, 200);
	send_no_cache_header(conn);
	send_additional_header(conn);
	send_cors_header(conn);
	mg_response_header_add(conn, "Content-Type", mime_type, -1);
	if (content_length < 0) {
		/* Size not known. Use chunked encoding (HTTP/1.x) */
		if (conn->protocol_type == PROTOCOL_TYPE_HTTP1) {
			/* Only HTTP/1.x defines "chunked" encoding, HTTP/2 does not*/
			mg_response_header_add(conn, "Transfer-Encoding", "chunked", -1);
		}
	} else {
		char len[32];
		int trunc = 0;
		mg_snprintf(conn,
		            &trunc,
		            len,
		            sizeof(len),
		            "%" UINT64_FMT,
		            (uint64_t)content_length);
		if (!trunc) {
			/* Since 32 bytes is enough to hold any 64 bit decimal number,
			 * !trunc is always true */
			mg_response_header_add(conn, "Content-Length", len, -1);
		}
	}
	mg_response_header_send(conn);

	return 0;
}


CIVETWEB_API int
mg_send_http_redirect(struct mg_connection *conn,
                      const char *target_url,
                      int redirect_code)
{
	/* Send a 30x redirect response.
	 *
	 * Redirect types (status codes):
	 *
	 * Status | Perm/Temp | Method              | Version
	 *   301  | permanent | POST->GET undefined | HTTP/1.0
	 *   302  | temporary | POST->GET undefined | HTTP/1.0
	 *   303  | temporary | always use GET      | HTTP/1.1
	 *   307  | temporary | always keep method  | HTTP/1.1
	 *   308  | permanent | always keep method  | HTTP/1.1
	 */

#if defined(MG_SEND_REDIRECT_BODY)
	char redirect_body[MG_BUF_LEN];
	size_t content_len = 0;
	char content_len_text[32];
#endif

	/* In case redirect_code=0, use 307. */
	if (redirect_code == 0) {
		redirect_code = 307;
	}

	/* In case redirect_code is none of the above, return error. */
	if ((redirect_code != 301) && (redirect_code != 302)
	    && (redirect_code != 303) && (redirect_code != 307)
	    && (redirect_code != 308)) {
		/* Parameter error */
		return -2;
	}

	/* If target_url is not defined, redirect to "/". */
	if ((target_url == NULL) || (*target_url == 0)) {
		target_url = "/";
	}

#if defined(MG_SEND_REDIRECT_BODY)
	/* TODO: condition name? */

	/* Prepare a response body with a hyperlink.
	 *
	 * According to RFC2616 (and RFC1945 before):
	 * Unless the request method was HEAD, the entity of the
	 * response SHOULD contain a short hypertext note with a hyperlink to
	 * the new URI(s).
	 *
	 * However, this response body is not useful in M2M communication.
	 * Probably the original reason in the RFC was, clients not supporting
	 * a 30x HTTP redirect could still show the HTML page and let the user
	 * press the link. Since current browsers support 30x HTTP, the additional
	 * HTML body does not seem to make sense anymore.
	 *
	 * The new RFC7231 (Section 6.4) does no longer recommend it ("SHOULD"),
	 * but it only notes:
	 * The server's response payload usually contains a short
	 * hypertext note with a hyperlink to the new URI(s).
	 *
	 * Deactivated by default. If you need the 30x body, set the define.
	 */
	mg_snprintf(
	    conn,
	    NULL /* ignore truncation */,
	    redirect_body,
	    sizeof(redirect_body),
	    "<html><head>%s</head><body><a href=\"%s\">%s</a></body></html>",
	    redirect_text,
	    target_url,
	    target_url);
	content_len = strlen(reply);
	sprintf(content_len_text, "%lu", (unsigned long)content_len);
#endif

	/* Send all required headers */
	mg_response_header_start(conn, redirect_code);
	mg_response_header_add(conn, "Location", target_url, -1);
	if ((redirect_code == 301) || (redirect_code == 308)) {
		/* Permanent redirect */
		send_static_cache_header(conn);
	} else {
		/* Temporary redirect */
		send_no_cache_header(conn);
	}
	send_additional_header(conn);
	send_cors_header(conn);
#if defined(MG_SEND_REDIRECT_BODY)
	mg_response_header_add(conn, "Content-Type", "text/html", -1);
	mg_response_header_add(conn, "Content-Length", content_len_text, -1);
#else
	mg_response_header_add(conn, "Content-Length", "0", 1);
#endif
	mg_response_header_send(conn);

#if defined(MG_SEND_REDIRECT_BODY)
	/* Send response body */
	/* ... unless it is a HEAD request */
	if (0 != strcmp(conn->request_info.request_method, "HEAD")) {
		ret = mg_write(conn, redirect_body, content_len);
	}
#endif

	return 1;
}


#if defined(_WIN32)
/* Create substitutes for POSIX functions in Win32. */

#if defined(GCC_DIAGNOSTIC)
/* Show no warning in case system functions are not used. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif


static int
pthread_mutex_init(pthread_mutex_t *mutex, void *unused)
{
	(void)unused;
	/* Always initialize as PTHREAD_MUTEX_RECURSIVE */
	InitializeCriticalSection(&mutex->sec);
	return 0;
}


static int
pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	DeleteCriticalSection(&mutex->sec);
	return 0;
}


static int
pthread_mutex_lock(pthread_mutex_t *mutex)
{
	EnterCriticalSection(&mutex->sec);
	return 0;
}


static int
pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	LeaveCriticalSection(&mutex->sec);
	return 0;
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_cond_init(pthread_cond_t *cv, const void *unused)
{
	(void)unused;
	(void)pthread_mutex_init(&cv->threadIdSec, &pthread_mutex_attr);
	cv->waiting_thread = NULL;
	return 0;
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_cond_timedwait(pthread_cond_t *cv,
                       pthread_mutex_t *mutex,
                       FUNCTION_MAY_BE_UNUSED const struct timespec *abstime)
{
	struct mg_workerTLS **ptls,
	    *tls = (struct mg_workerTLS *)pthread_getspecific(sTlsKey);
	int ok;
	uint64_t nsnow, nswaitabs;
	int64_t nswaitrel;
	DWORD mswaitrel;

	pthread_mutex_lock(&cv->threadIdSec);
	/* Add this thread to cv's waiting list */
	ptls = &cv->waiting_thread;
	for (; *ptls != NULL; ptls = &(*ptls)->next_waiting_thread)
		;
	tls->next_waiting_thread = NULL;
	*ptls = tls;
	pthread_mutex_unlock(&cv->threadIdSec);

	if (abstime) {
		nsnow = mg_get_current_time_ns();
		nswaitabs =
		    (((uint64_t)abstime->tv_sec) * 1000000000) + abstime->tv_nsec;
		nswaitrel = nswaitabs - nsnow;
		if (nswaitrel < 0) {
			nswaitrel = 0;
		}
		mswaitrel = (DWORD)(nswaitrel / 1000000);
	} else {
		mswaitrel = (DWORD)INFINITE;
	}

	pthread_mutex_unlock(mutex);
	ok = (WAIT_OBJECT_0
	      == WaitForSingleObject(tls->pthread_cond_helper_mutex, mswaitrel));
	if (!ok) {
		ok = 1;
		pthread_mutex_lock(&cv->threadIdSec);
		ptls = &cv->waiting_thread;
		for (; *ptls != NULL; ptls = &(*ptls)->next_waiting_thread) {
			if (*ptls == tls) {
				*ptls = tls->next_waiting_thread;
				ok = 0;
				break;
			}
		}
		pthread_mutex_unlock(&cv->threadIdSec);
		if (ok) {
			WaitForSingleObject(tls->pthread_cond_helper_mutex,
			                    (DWORD)INFINITE);
		}
	}
	/* This thread has been removed from cv's waiting list */
	pthread_mutex_lock(mutex);

	return ok ? 0 : -1;
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex)
{
	return pthread_cond_timedwait(cv, mutex, NULL);
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_cond_signal(pthread_cond_t *cv)
{
	HANDLE wkup = NULL;
	BOOL ok = FALSE;

	pthread_mutex_lock(&cv->threadIdSec);
	if (cv->waiting_thread) {
		wkup = cv->waiting_thread->pthread_cond_helper_mutex;
		cv->waiting_thread = cv->waiting_thread->next_waiting_thread;

		ok = SetEvent(wkup);
		DEBUG_ASSERT(ok);
	}
	pthread_mutex_unlock(&cv->threadIdSec);

	return ok ? 0 : 1;
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_cond_broadcast(pthread_cond_t *cv)
{
	pthread_mutex_lock(&cv->threadIdSec);
	while (cv->waiting_thread) {
		pthread_cond_signal(cv);
	}
	pthread_mutex_unlock(&cv->threadIdSec);

	return 0;
}


FUNCTION_MAY_BE_UNUSED
static int
pthread_cond_destroy(pthread_cond_t *cv)
{
	pthread_mutex_lock(&cv->threadIdSec);
	DEBUG_ASSERT(cv->waiting_thread == NULL);
	pthread_mutex_unlock(&cv->threadIdSec);
	pthread_mutex_destroy(&cv->threadIdSec);

	return 0;
}


#if defined(ALTERNATIVE_QUEUE)
FUNCTION_MAY_BE_UNUSED
static void *
event_create(void)
{
	return (void *)CreateEvent(NULL, FALSE, FALSE, NULL);
}


FUNCTION_MAY_BE_UNUSED
static int
event_wait(void *eventhdl)
{
	int res = WaitForSingleObject((HANDLE)eventhdl, (DWORD)INFINITE);
	return (res == WAIT_OBJECT_0);
}


FUNCTION_MAY_BE_UNUSED
static int
event_signal(void *eventhdl)
{
	return (int)SetEvent((HANDLE)eventhdl);
}


FUNCTION_MAY_BE_UNUSED
static void
event_destroy(void *eventhdl)
{
	CloseHandle((HANDLE)eventhdl);
}
#endif


#if defined(GCC_DIAGNOSTIC)
/* Enable unused function warning again */
#pragma GCC diagnostic pop
#endif


/* For Windows, change all slashes to backslashes in path names. */
static void
change_slashes_to_backslashes(char *path)
{
	int i;

	for (i = 0; path[i] != '\0'; i++) {
		if (path[i] == '/') {
			path[i] = '\\';
		}

		/* remove double backslash (check i > 0 to preserve UNC paths,
		 * like \\server\file.txt) */
		if ((i > 0) && (path[i] == '\\')) {
			while ((path[i + 1] == '\\') || (path[i + 1] == '/')) {
				(void)memmove(path + i + 1, path + i + 2, strlen(path + i + 1));
			}
		}
	}
}


static int
mg_wcscasecmp(const wchar_t *s1, const wchar_t *s2)
{
	int diff;

	do {
		diff = ((*s1 >= L'A') && (*s1 <= L'Z') ? (*s1 - L'A' + L'a') : *s1)
		       - ((*s2 >= L'A') && (*s2 <= L'Z') ? (*s2 - L'A' + L'a') : *s2);
		s1++;
		s2++;
	} while ((diff == 0) && (s1[-1] != L'\0'));

	return diff;
}


/* Encode 'path' which is assumed UTF-8 string, into UNICODE string.
 * wbuf and wbuf_len is a target buffer and its length. */
static void
path_to_unicode(const struct mg_connection *conn,
                const char *path,
                wchar_t *wbuf,
                size_t wbuf_len)
{
	char buf[UTF8_PATH_MAX], buf2[UTF8_PATH_MAX];
	wchar_t wbuf2[UTF16_PATH_MAX + 1];
	DWORD long_len, err;
	int (*fcompare)(const wchar_t *, const wchar_t *) = mg_wcscasecmp;

	mg_strlcpy(buf, path, sizeof(buf));
	change_slashes_to_backslashes(buf);

	/* Convert to Unicode and back. If doubly-converted string does not
	 * match the original, something is fishy, reject. */
	memset(wbuf, 0, wbuf_len * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, (int)wbuf_len);
	WideCharToMultiByte(
	    CP_UTF8, 0, wbuf, (int)wbuf_len, buf2, sizeof(buf2), NULL, NULL);
	if (strcmp(buf, buf2) != 0) {
		wbuf[0] = L'\0';
	}

	/* Windows file systems are not case sensitive, but you can still use
	 * uppercase and lowercase letters (on all modern file systems).
	 * The server can check if the URI uses the same upper/lowercase
	 * letters an the file system, effectively making Windows servers
	 * case sensitive (like Linux servers are). It is still not possible
	 * to use two files with the same name in different cases on Windows
	 * (like /a and /A) - this would be possible in Linux.
	 * As a default, Windows is not case sensitive, but the case sensitive
	 * file name check can be activated by an additional configuration. */
	if (conn) {
		if (conn->dom_ctx->config[CASE_SENSITIVE_FILES]
		    && !mg_strcasecmp(conn->dom_ctx->config[CASE_SENSITIVE_FILES],
		                      "yes")) {
			/* Use case sensitive compare function */
			fcompare = wcscmp;
		}
	}
	(void)conn; /* conn is currently unused */

	/* Only accept a full file path, not a Windows short (8.3) path. */
	memset(wbuf2, 0, ARRAY_SIZE(wbuf2) * sizeof(wchar_t));
	long_len = GetLongPathNameW(wbuf, wbuf2, ARRAY_SIZE(wbuf2) - 1);
	if (long_len == 0) {
		err = GetLastError();
		if (err == ERROR_FILE_NOT_FOUND) {
			/* File does not exist. This is not always a problem here. */
			return;
		}
	}
	if ((long_len >= ARRAY_SIZE(wbuf2)) || (fcompare(wbuf, wbuf2) != 0)) {
		/* Short name is used. */
		wbuf[0] = L'\0';
	}
}


#if !defined(NO_FILESYSTEMS)
/* Get file information, return 1 if file exists, 0 if not */
static int
mg_stat(const struct mg_connection *conn,
        const char *path,
        struct mg_file_stat *filep)
{
	wchar_t wbuf[UTF16_PATH_MAX];
	WIN32_FILE_ATTRIBUTE_DATA info;
	time_t creation_time;
	size_t len;

	if (!filep) {
		return 0;
	}
	memset(filep, 0, sizeof(*filep));

	if (mg_path_suspicious(conn, path)) {
		return 0;
	}

	path_to_unicode(conn, path, wbuf, ARRAY_SIZE(wbuf));
	/* Windows happily opens files with some garbage at the end of file name.
	 * For example, fopen("a.cgi    ", "r") on Windows successfully opens
	 * "a.cgi", despite one would expect an error back. */
	len = strlen(path);
	if ((len > 0) && (path[len - 1] != ' ') && (path[len - 1] != '.')
	    && (GetFileAttributesExW(wbuf, GetFileExInfoStandard, &info) != 0)) {
		filep->size = MAKEUQUAD(info.nFileSizeLow, info.nFileSizeHigh);
		filep->last_modified =
		    SYS2UNIX_TIME(info.ftLastWriteTime.dwLowDateTime,
		                  info.ftLastWriteTime.dwHighDateTime);

		/* On Windows, the file creation time can be higher than the
		 * modification time, e.g. when a file is copied.
		 * Since the Last-Modified timestamp is used for caching
		 * it should be based on the most recent timestamp. */
		creation_time = SYS2UNIX_TIME(info.ftCreationTime.dwLowDateTime,
		                              info.ftCreationTime.dwHighDateTime);
		if (creation_time > filep->last_modified) {
			filep->last_modified = creation_time;
		}

		filep->is_directory = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		return 1;
	}

	return 0;
}
#endif


static int
mg_remove(const struct mg_connection *conn, const char *path)
{
	wchar_t wbuf[UTF16_PATH_MAX];
	path_to_unicode(conn, path, wbuf, ARRAY_SIZE(wbuf));
	return DeleteFileW(wbuf) ? 0 : -1;
}


static int
mg_mkdir(const struct mg_connection *conn, const char *path, int mode)
{
	wchar_t wbuf[UTF16_PATH_MAX];
	(void)mode;
	path_to_unicode(conn, path, wbuf, ARRAY_SIZE(wbuf));
	return CreateDirectoryW(wbuf, NULL) ? 0 : -1;
}


/* Create substitutes for POSIX functions in Win32. */

#if defined(GCC_DIAGNOSTIC)
/* Show no warning in case system functions are not used. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif


/* Implementation of POSIX opendir/closedir/readdir for Windows. */
FUNCTION_MAY_BE_UNUSED
static DIR *
mg_opendir(const struct mg_connection *conn, const char *name)
{
	DIR *dir = NULL;
	wchar_t wpath[UTF16_PATH_MAX];
	DWORD attrs;

	if (name == NULL) {
		SetLastError(ERROR_BAD_ARGUMENTS);
	} else if ((dir = (DIR *)mg_malloc(sizeof(*dir))) == NULL) {
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	} else {
		path_to_unicode(conn, name, wpath, ARRAY_SIZE(wpath));
		attrs = GetFileAttributesW(wpath);
		if ((wcslen(wpath) + 2 < ARRAY_SIZE(wpath)) && (attrs != 0xFFFFFFFF)
		    && ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0)) {
			(void)wcscat(wpath, L"\\*");
			dir->handle = FindFirstFileW(wpath, &dir->info);
			dir->result.d_name[0] = '\0';
		} else {
			mg_free(dir);
			dir = NULL;
		}
	}

	return dir;
}


FUNCTION_MAY_BE_UNUSED
static int
mg_closedir(DIR *dir)
{
	int result = 0;

	if (dir != NULL) {
		if (dir->handle != INVALID_HANDLE_VALUE)
			result = FindClose(dir->handle) ? 0 : -1;

		mg_free(dir);
	} else {
		result = -1;
		SetLastError(ERROR_BAD_ARGUMENTS);
	}

	return result;
}


FUNCTION_MAY_BE_UNUSED
static struct dirent *
mg_readdir(DIR *dir)
{
	struct dirent *result = 0;

	if (dir) {
		if (dir->handle != INVALID_HANDLE_VALUE) {
			result = &dir->result;
			(void)WideCharToMultiByte(CP_UTF8,
			                          0,
			                          dir->info.cFileName,
			                          -1,
			                          result->d_name,
			                          sizeof(result->d_name),
			                          NULL,
			                          NULL);

			if (!FindNextFileW(dir->handle, &dir->info)) {
				(void)FindClose(dir->handle);
				dir->handle = INVALID_HANDLE_VALUE;
			}

		} else {
			SetLastError(ERROR_FILE_NOT_FOUND);
		}
	} else {
		SetLastError(ERROR_BAD_ARGUMENTS);
	}

	return result;
}


#if !defined(HAVE_POLL)
#undef POLLIN
#undef POLLPRI
#undef POLLOUT
#undef POLLERR
#define POLLIN (1)  /* Data ready - read will not block. */
#define POLLPRI (2) /* Priority data ready. */
#define POLLOUT (4) /* Send queue not full - write will not block. */
#define POLLERR (8) /* Error event */

FUNCTION_MAY_BE_UNUSED
static int
poll(struct mg_pollfd *pfd, unsigned int n, int milliseconds)
{
	struct timeval tv;
	fd_set rset;
	fd_set wset;
	fd_set eset;
	unsigned int i;
	int result;
	SOCKET maxfd = 0;

	memset(&tv, 0, sizeof(tv));
	tv.tv_sec = milliseconds / 1000;
	tv.tv_usec = (milliseconds % 1000) * 1000;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_ZERO(&eset);

	for (i = 0; i < n; i++) {
		if (pfd[i].events & (POLLIN | POLLOUT | POLLERR)) {
			if (pfd[i].events & POLLIN) {
				FD_SET(pfd[i].fd, &rset);
			}
			if (pfd[i].events & POLLOUT) {
				FD_SET(pfd[i].fd, &wset);
			}
			/* Check for errors for any FD in the set */
			FD_SET(pfd[i].fd, &eset);
		}
		pfd[i].revents = 0;

		if (pfd[i].fd > maxfd) {
			maxfd = pfd[i].fd;
		}
	}

	if ((result = select((int)maxfd + 1, &rset, &wset, &eset, &tv)) > 0) {
		for (i = 0; i < n; i++) {
			if (FD_ISSET(pfd[i].fd, &rset)) {
				pfd[i].revents |= POLLIN;
			}
			if (FD_ISSET(pfd[i].fd, &wset)) {
				pfd[i].revents |= POLLOUT;
			}
			if (FD_ISSET(pfd[i].fd, &eset)) {
				pfd[i].revents |= POLLERR;
			}
		}
	}

	/* We should subtract the time used in select from remaining
	 * "milliseconds", in particular if called from mg_poll with a
	 * timeout quantum.
	 * Unfortunately, the remaining time is not stored in "tv" in all
	 * implementations, so the result in "tv" must be considered undefined.
	 * See http://man7.org/linux/man-pages/man2/select.2.html */

	return result;
}
#endif /* HAVE_POLL */


#if defined(GCC_DIAGNOSTIC)
/* Enable unused function warning again */
#pragma GCC diagnostic pop
#endif


static void
set_close_on_exec(SOCKET sock,
                  const struct mg_connection *conn /* may be null */,
                  struct mg_context *ctx /* may be null */)
{
	(void)conn; /* Unused. */
	(void)ctx;

	(void)SetHandleInformation((HANDLE)(intptr_t)sock, HANDLE_FLAG_INHERIT, 0);
}


CIVETWEB_API int
mg_start_thread(mg_thread_func_t f, void *p)
{
#if defined(USE_STACK_SIZE) && (USE_STACK_SIZE > 1)
	/* Compile-time option to control stack size, e.g.
	 * -DUSE_STACK_SIZE=16384
	 */
	return ((_beginthread((void(__cdecl *)(void *))f, USE_STACK_SIZE, p)
	         == ((uintptr_t)(-1L)))
	            ? -1
	            : 0);
#else
	return (
	    (_beginthread((void(__cdecl *)(void *))f, 0, p) == ((uintptr_t)(-1L)))
	        ? -1
	        : 0);
#endif /* defined(USE_STACK_SIZE) && (USE_STACK_SIZE > 1) */
}


/* Start a thread storing the thread context. */
static int
mg_start_thread_with_id(unsigned(__stdcall *f)(void *),
                        void *p,
                        pthread_t *threadidptr)
{
	uintptr_t uip;
	HANDLE threadhandle;
	int result = -1;

	uip = _beginthreadex(NULL, 0, f, p, 0, NULL);
	threadhandle = (HANDLE)uip;
	if ((uip != 0) && (threadidptr != NULL)) {
		*threadidptr = threadhandle;
		result = 0;
	}

	return result;
}


/* Wait for a thread to finish. */
static int
mg_join_thread(pthread_t threadid)
{
	int result;
	DWORD dwevent;

	result = -1;
	dwevent = WaitForSingleObject(threadid, (DWORD)INFINITE);
	if (dwevent == WAIT_FAILED) {
		DEBUG_TRACE("WaitForSingleObject() failed, error %d", ERRNO);
	} else {
		if (dwevent == WAIT_OBJECT_0) {
			CloseHandle(threadid);
			result = 0;
		}
	}

	return result;
}

#if !defined(NO_SSL_DL) && !defined(NO_SSL)
/* If SSL is loaded dynamically, dlopen/dlclose is required. */
/* Create substitutes for POSIX functions in Win32. */

#if defined(GCC_DIAGNOSTIC)
/* Show no warning in case system functions are not used. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif


FUNCTION_MAY_BE_UNUSED
static HANDLE
dlopen(const char *dll_name, int flags)
{
	wchar_t wbuf[UTF16_PATH_MAX];
	(void)flags;
	path_to_unicode(NULL, dll_name, wbuf, ARRAY_SIZE(wbuf));
	return LoadLibraryW(wbuf);
}


FUNCTION_MAY_BE_UNUSED
static int
dlclose(void *handle)
{
	int result;

	if (FreeLibrary((HMODULE)handle) != 0) {
		result = 0;
	} else {
		result = -1;
	}

	return result;
}


#if defined(GCC_DIAGNOSTIC)
/* Enable unused function warning again */
#pragma GCC diagnostic pop
#endif

#endif


#if !defined(NO_CGI)
#define SIGKILL (0)


static int
kill(pid_t pid, int sig_num)
{
	(void)TerminateProcess((HANDLE)pid, (UINT)sig_num);
	(void)CloseHandle((HANDLE)pid);
	return 0;
}


#if !defined(WNOHANG)
#define WNOHANG (1)
#endif


static pid_t
waitpid(pid_t pid, int *status, int flags)
{
	DWORD timeout = INFINITE;
	DWORD waitres;

	(void)status; /* Currently not used by any client here */

	if ((flags | WNOHANG) == WNOHANG) {
		timeout = 0;
	}

	waitres = WaitForSingleObject((HANDLE)pid, timeout);
	if (waitres == WAIT_OBJECT_0) {
		return pid;
	}
	if (waitres == WAIT_TIMEOUT) {
		return 0;
	}
	return (pid_t)-1;
}


static void
trim_trailing_whitespaces(char *s)
{
	char *e = s + strlen(s);
	while ((e > s) && isspace((unsigned char)e[-1])) {
		*(--e) = '\0';
	}
}


static pid_t
spawn_process(struct mg_connection *conn,
              const char *prog,
              char *envblk,
              char *envp[],
              int fdin[2],
              int fdout[2],
              int fderr[2],
              const char *dir,
              int cgi_config_idx)
{
	HANDLE me;
	char *interp;
	char *interp_arg = 0;
	char full_dir[UTF8_PATH_MAX], cmdline[UTF8_PATH_MAX], buf[UTF8_PATH_MAX];
	int truncated;
	struct mg_file file = STRUCT_FILE_INITIALIZER;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi = {0};

	(void)envp;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	me = GetCurrentProcess();
	DuplicateHandle(me,
	                (HANDLE)_get_osfhandle(fdin[0]),
	                me,
	                &si.hStdInput,
	                0,
	                TRUE,
	                DUPLICATE_SAME_ACCESS);
	DuplicateHandle(me,
	                (HANDLE)_get_osfhandle(fdout[1]),
	                me,
	                &si.hStdOutput,
	                0,
	                TRUE,
	                DUPLICATE_SAME_ACCESS);
	DuplicateHandle(me,
	                (HANDLE)_get_osfhandle(fderr[1]),
	                me,
	                &si.hStdError,
	                0,
	                TRUE,
	                DUPLICATE_SAME_ACCESS);

	/* Mark handles that should not be inherited. See
	 * https://msdn.microsoft.com/en-us/library/windows/desktop/ms682499%28v=vs.85%29.aspx
	 */
	SetHandleInformation((HANDLE)_get_osfhandle(fdin[1]),
	                     HANDLE_FLAG_INHERIT,
	                     0);
	SetHandleInformation((HANDLE)_get_osfhandle(fdout[0]),
	                     HANDLE_FLAG_INHERIT,
	                     0);
	SetHandleInformation((HANDLE)_get_osfhandle(fderr[0]),
	                     HANDLE_FLAG_INHERIT,
	                     0);

	/* First check, if there is a CGI interpreter configured for all CGI
	 * scripts. */
	interp = conn->dom_ctx->config[CGI_INTERPRETER + cgi_config_idx];
	if (interp != NULL) {
		/* If there is a configured interpreter, check for additional arguments
		 */
		interp_arg =
		    conn->dom_ctx->config[CGI_INTERPRETER_ARGS + cgi_config_idx];
	} else {
		/* Otherwise, the interpreter must be stated in the first line of the
		 * CGI script file, after a #! (shebang) mark. */
		buf[0] = buf[1] = '\0';

		/* Get the full script path */
		mg_snprintf(
		    conn, &truncated, cmdline, sizeof(cmdline), "%s/%s", dir, prog);

		if (truncated) {
			pi.hProcess = (pid_t)-1;
			goto spawn_cleanup;
		}

		/* Open the script file, to read the first line */
		if (mg_fopen(conn, cmdline, MG_FOPEN_MODE_READ, &file)) {

			/* Read the first line of the script into the buffer */
			mg_fgets(buf, sizeof(buf), &file);
			(void)mg_fclose(&file.access); /* ignore error on read only file */
			buf[sizeof(buf) - 1] = '\0';
		}

		if ((buf[0] == '#') && (buf[1] == '!')) {
			trim_trailing_whitespaces(buf + 2);
		} else {
			buf[2] = '\0';
		}
		interp = buf + 2;
	}

	GetFullPathNameA(dir, sizeof(full_dir), full_dir, NULL);

	if (interp[0] != '\0') {
		/* This is an interpreted script file. We must call the interpreter. */
		if ((interp_arg != 0) && (interp_arg[0] != 0)) {
			mg_snprintf(conn,
			            &truncated,
			            cmdline,
			            sizeof(cmdline),
			            "\"%s\" %s \"%s\\%s\"",
			            interp,
			            interp_arg,
			            full_dir,
			            prog);
		} else {
			mg_snprintf(conn,
			            &truncated,
			            cmdline,
			            sizeof(cmdline),
			            "\"%s\" \"%s\\%s\"",
			            interp,
			            full_dir,
			            prog);
		}
	} else {
		/* This is (probably) a compiled program. We call it directly. */
		mg_snprintf(conn,
		            &truncated,
		            cmdline,
		            sizeof(cmdline),
		            "\"%s\\%s\"",
		            full_dir,
		            prog);
	}

	if (truncated) {
		pi.hProcess = (pid_t)-1;
		goto spawn_cleanup;
	}

	DEBUG_TRACE("Running [%s]", cmdline);
	if (CreateProcessA(NULL,
	                   cmdline,
	                   NULL,
	                   NULL,
	                   TRUE,
	                   CREATE_NEW_PROCESS_GROUP,
	                   envblk,
	                   NULL,
	                   &si,
	                   &pi)
	    == 0) {
		mg_cry_internal(
		    conn, "%s: CreateProcess(%s): %ld", __func__, cmdline, (long)ERRNO);
		pi.hProcess = (pid_t)-1;
		/* goto spawn_cleanup; */
	}

spawn_cleanup:
	(void)CloseHandle(si.hStdOutput);
	(void)CloseHandle(si.hStdError);
	(void)CloseHandle(si.hStdInput);
	if (pi.hThread != NULL) {
		(void)CloseHandle(pi.hThread);
	}

	return (pid_t)pi.hProcess;
}
#endif /* !NO_CGI */


static int
set_blocking_mode(SOCKET sock)
{
	unsigned long non_blocking = 0;
	return ioctlsocket(sock, (long)FIONBIO, &non_blocking);
}


static int
set_non_blocking_mode(SOCKET sock)
{
	unsigned long non_blocking = 1;
	return ioctlsocket(sock, (long)FIONBIO, &non_blocking);
}


#else


#if !defined(NO_FILESYSTEMS)
static int
mg_stat(const struct mg_connection *conn,
        const char *path,
        struct mg_file_stat *filep)
{
	struct stat st;
	if (!filep) {
		return 0;
	}
	memset(filep, 0, sizeof(*filep));

	if (mg_path_suspicious(conn, path)) {
		return 0;
	}

	if (0 == stat(path, &st)) {
		filep->size = (uint64_t)(st.st_size);
		filep->last_modified = st.st_mtime;
		filep->is_directory = S_ISDIR(st.st_mode);
		return 1;
	}

	return 0;
}
#endif /* NO_FILESYSTEMS */


static void
set_close_on_exec(int fd,
                  const struct mg_connection *conn /* may be null */,
                  struct mg_context *ctx /* may be null */)
{
#if defined(__ZEPHYR__)
	(void)fd;
	(void)conn;
	(void)ctx;
#else
	if (fcntl(fd, F_SETFD, FD_CLOEXEC) != 0) {
		if (conn || ctx) {
			struct mg_connection fc;
			mg_cry_internal((conn ? conn : fake_connection(&fc, ctx)),
			                "%s: fcntl(F_SETFD FD_CLOEXEC) failed: %s",
			                __func__,
			                strerror(ERRNO));
		}
	}
#endif
}


CIVETWEB_API int
mg_start_thread(mg_thread_func_t func, void *param)
{
	pthread_t thread_id;
	pthread_attr_t attr;
	int result;

	(void)pthread_attr_init(&attr);
	(void)pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

#if defined(__ZEPHYR__)
	pthread_attr_setstack(&attr, &civetweb_main_stack, ZEPHYR_STACK_SIZE);
#elif defined(USE_STACK_SIZE) && (USE_STACK_SIZE > 1)
	/* Compile-time option to control stack size,
	 * e.g. -DUSE_STACK_SIZE=16384 */
	(void)pthread_attr_setstacksize(&attr, USE_STACK_SIZE);
#endif /* defined(USE_STACK_SIZE) && (USE_STACK_SIZE > 1) */

	result = pthread_create(&thread_id, &attr, func, param);
	pthread_attr_destroy(&attr);

	return result;
}


/* Start a thread storing the thread context. */
static int
mg_start_thread_with_id(mg_thread_func_t func,
                        void *param,
                        pthread_t *threadidptr)
{
	pthread_t thread_id;
	pthread_attr_t attr;
	int result;

	(void)pthread_attr_init(&attr);

#if defined(__ZEPHYR__)
	pthread_attr_setstack(&attr,
	                      &civetweb_worker_stacks[zephyr_worker_stack_index++],
	                      ZEPHYR_STACK_SIZE);
#elif defined(USE_STACK_SIZE) && (USE_STACK_SIZE > 1)
	/* Compile-time option to control stack size,
	 * e.g. -DUSE_STACK_SIZE=16384 */
	(void)pthread_attr_setstacksize(&attr, USE_STACK_SIZE);
#endif /* defined(USE_STACK_SIZE) && USE_STACK_SIZE > 1 */

	result = pthread_create(&thread_id, &attr, func, param);
	pthread_attr_destroy(&attr);
	if ((result == 0) && (threadidptr != NULL)) {
		*threadidptr = thread_id;
	}
	return result;
}


/* Wait for a thread to finish. */
static int
mg_join_thread(pthread_t threadid)
{
	int result;

	result = pthread_join(threadid, NULL);
	return result;
}


#if !defined(NO_CGI)
static pid_t
spawn_process(struct mg_connection *conn,
              const char *prog,
              char *envblk,
              char *envp[],
              int fdin[2],
              int fdout[2],
              int fderr[2],
              const char *dir,
              int cgi_config_idx)
{
	pid_t pid;
	const char *interp;

	(void)envblk;

	if ((pid = fork()) == -1) {
		/* Parent */
		mg_cry_internal(conn, "%s: fork(): %s", __func__, strerror(ERRNO));
	} else if (pid != 0) {
		/* Make sure children close parent-side descriptors.
		 * The caller will close the child-side immediately. */
		set_close_on_exec(fdin[1], conn, NULL);  /* stdin write */
		set_close_on_exec(fdout[0], conn, NULL); /* stdout read */
		set_close_on_exec(fderr[0], conn, NULL); /* stderr read */
	} else {
		/* Child */
		if (chdir(dir) != 0) {
			mg_cry_internal(
			    conn, "%s: chdir(%s): %s", __func__, dir, strerror(ERRNO));
		} else if (dup2(fdin[0], 0) == -1) {
			mg_cry_internal(conn,
			                "%s: dup2(%d, 0): %s",
			                __func__,
			                fdin[0],
			                strerror(ERRNO));
		} else if (dup2(fdout[1], 1) == -1) {
			mg_cry_internal(conn,
			                "%s: dup2(%d, 1): %s",
			                __func__,
			                fdout[1],
			                strerror(ERRNO));
		} else if (dup2(fderr[1], 2) == -1) {
			mg_cry_internal(conn,
			                "%s: dup2(%d, 2): %s",
			                __func__,
			                fderr[1],
			                strerror(ERRNO));
		} else {
			struct sigaction sa;

			/* Keep stderr and stdout in two different pipes.
			 * Stdout will be sent back to the client,
			 * stderr should go into a server error log. */
			(void)close(fdin[0]);
			(void)close(fdout[1]);
			(void)close(fderr[1]);

			/* Close write end fdin and read end fdout and fderr */
			(void)close(fdin[1]);
			(void)close(fdout[0]);
			(void)close(fderr[0]);

			/* After exec, all signal handlers are restored to their default
			 * values, with one exception of SIGCHLD. According to
			 * POSIX.1-2001 and Linux's implementation, SIGCHLD's handler
			 * will leave unchanged after exec if it was set to be ignored.
			 * Restore it to default action. */
			memset(&sa, 0, sizeof(sa));
			sa.sa_handler = SIG_DFL;
			sigaction(SIGCHLD, &sa, NULL);

			interp = conn->dom_ctx->config[CGI_INTERPRETER + cgi_config_idx];
			if (interp == NULL) {
				/* no interpreter configured, call the program directly */
				(void)execle(prog, prog, NULL, envp);
				mg_cry_internal(conn,
				                "%s: execle(%s): %s",
				                __func__,
				                prog,
				                strerror(ERRNO));
			} else {
				/* call the configured interpreter */
				const char *interp_args =
				    conn->dom_ctx
				        ->config[CGI_INTERPRETER_ARGS + cgi_config_idx];

				if ((interp_args != NULL) && (interp_args[0] != 0)) {
					(void)execle(interp, interp, interp_args, prog, NULL, envp);
				} else {
					(void)execle(interp, interp, prog, NULL, envp);
				}
				mg_cry_internal(conn,
				                "%s: execle(%s %s): %s",
				                __func__,
				                interp,
				                prog,
				                strerror(ERRNO));
			}
		}
		exit(EXIT_FAILURE);
	}

	return pid;
}
#endif /* !NO_CGI */


static int
set_non_blocking_mode(SOCKET sock)
{
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		return -1;
	}

	if (fcntl(sock, F_SETFL, (flags | O_NONBLOCK)) < 0) {
		return -1;
	}
	return 0;
}

static int
set_blocking_mode(SOCKET sock)
{
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		return -1;
	}

	if (fcntl(sock, F_SETFL, flags & (~(int)(O_NONBLOCK))) < 0) {
		return -1;
	}
	return 0;
}
#endif /* _WIN32 / else */

/* End of initial operating system specific define block. */


/* Get a random number (independent of C rand function) */
static uint64_t
get_random(void)
{
	static uint64_t lfsr = 0; /* Linear feedback shift register */
	static uint64_t lcg = 0;  /* Linear congruential generator */
	uint64_t now = mg_get_current_time_ns();

	if (lfsr == 0) {
		/* lfsr will be only 0 if has not been initialized,
		 * so this code is called only once. */
		lfsr = mg_get_current_time_ns();
		lcg = mg_get_current_time_ns();
	} else {
		/* Get the next step of both random number generators. */
		lfsr = (lfsr >> 1)
		       | ((((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 1)
		          << 63);
		lcg = lcg * 6364136223846793005LL + 1442695040888963407LL;
	}

	/* Combining two pseudo-random number generators and a high resolution
	 * part
	 * of the current server time will make it hard (impossible?) to guess
	 * the
	 * next number. */
	return (lfsr ^ lcg ^ now);
}


static int
mg_poll(struct mg_pollfd *pfd,
        unsigned int n,
        int milliseconds,
        const stop_flag_t *stop_flag)
{
	/* Call poll, but only for a maximum time of a few seconds.
	 * This will allow to stop the server after some seconds, instead
	 * of having to wait for a long socket timeout. */
	int ms_now = SOCKET_TIMEOUT_QUANTUM; /* Sleep quantum in ms */

	int check_pollerr = 0;
	if ((n == 1) && ((pfd[0].events & POLLERR) == 0)) {
		/* If we wait for only one file descriptor, wait on error as well */
		pfd[0].events |= POLLERR;
		check_pollerr = 1;
	}

	do {
		int result;

		if (!STOP_FLAG_IS_ZERO(&*stop_flag)) {
			/* Shut down signal */
			return -2;
		}

		if ((milliseconds >= 0) && (milliseconds < ms_now)) {
			ms_now = milliseconds;
		}

		result = poll(pfd, n, ms_now);
		if (result != 0) {
			int err = ERRNO;
			if ((result == 1) || (!ERROR_TRY_AGAIN(err))) {
				/* Poll returned either success (1) or error (-1).
				 * Forward both to the caller. */
				if ((check_pollerr)
				    && ((pfd[0].revents & (POLLIN | POLLOUT | POLLERR))
				        == POLLERR)) {
					/* One and only file descriptor returned error */
					return -1;
				}
				return result;
			}
		}

		/* Poll returned timeout (0). */
		if (milliseconds > 0) {
			milliseconds -= ms_now;
		}

	} while (milliseconds > 0);

	/* timeout: return 0 */
	return 0;
}


/* Write data to the IO channel - opened file descriptor, socket or SSL
 * descriptor.
 * Return value:
 *  >=0 .. number of bytes successfully written
 *   -1 .. timeout
 *   -2 .. error
 */
static int
push_inner(struct mg_context *ctx,
           FILE *fp,
           SOCKET sock,
           SSL *ssl,
           const char *buf,
           int len,
           double timeout)
{
	uint64_t start = 0, now = 0, timeout_ns = 0;
	int n, err;
	unsigned ms_wait = SOCKET_TIMEOUT_QUANTUM; /* Sleep quantum in ms */

#if defined(_WIN32)
	typedef int len_t;
#else
	typedef size_t len_t;
#endif

	if (timeout > 0) {
		now = mg_get_current_time_ns();
		start = now;
		timeout_ns = (uint64_t)(timeout * 1.0E9);
	}

	if (ctx == NULL) {
		return -2;
	}

#if defined(NO_SSL) && !defined(USE_MBEDTLS) && !defined(USE_GNUTLS)
	if (ssl) {
		return -2;
	}
#endif

	/* Try to read until it succeeds, fails, times out, or the server
	 * shuts down. */
	for (;;) {

#if defined(USE_MBEDTLS)
		if (ssl != NULL) {
			n = mbed_ssl_write(ssl, (const unsigned char *)buf, len);
			if (n <= 0) {
				if ((n == MBEDTLS_ERR_SSL_WANT_READ)
				    || (n == MBEDTLS_ERR_SSL_WANT_WRITE)
				    || n == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS) {
					n = 0;
				} else {
					fprintf(stderr, "SSL write failed, error %d\n", n);
					return -2;
				}
			} else {
				err = 0;
			}
		} else
#elif defined(USE_GNUTLS)
		if (ssl != NULL) {
			n = gtls_ssl_write(ssl, (const unsigned char *)buf, (size_t) len);
			if (n < 0) {
				fprintf(stderr, "SSL write failed (%d): %s", n, gnutls_strerror(n));
				return -2;
			} else {
				err = 0;
			}
		} else
#elif !defined(NO_SSL)
		if (ssl != NULL) {
			ERR_clear_error();
			n = SSL_write(ssl, buf, len);
			if (n <= 0) {
				err = SSL_get_error(ssl, n);
				if ((err == SSL_ERROR_SYSCALL) && (n == -1)) {
					err = ERRNO;
				} else if ((err == SSL_ERROR_WANT_READ)
				           || (err == SSL_ERROR_WANT_WRITE)) {
					n = 0;
				} else {
					DEBUG_TRACE("SSL_write() failed, error %d", err);
					ERR_clear_error();
					return -2;
				}
				ERR_clear_error();
			} else {
				err = 0;
			}
		} else
#endif

		    if (fp != NULL) {
			n = (int)fwrite(buf, 1, (size_t)len, fp);
			if (ferror(fp)) {
				n = -1;
				err = ERRNO;
			} else {
				err = 0;
			}
		} else {
			n = (int)send(sock, buf, (len_t)len, MSG_NOSIGNAL);
			err = (n < 0) ? ERRNO : 0;
			if (ERROR_TRY_AGAIN(err)) {
				err = 0;
				n = 0;
			}
			if (n < 0) {
				/* shutdown of the socket at client side */
				return -2;
			}
		}

		if (!STOP_FLAG_IS_ZERO(&ctx->stop_flag)) {
			return -2;
		}

		if ((n > 0) || ((n == 0) && (len == 0))) {
			/* some data has been read, or no data was requested */
			return n;
		}
		if (n < 0) {
			/* socket error - check errno */
			DEBUG_TRACE("send() failed, error %d", err);

			/* TODO (mid): error handling depending on the error code.
			 * These codes are different between Windows and Linux.
			 * Currently there is no problem with failing send calls,
			 * if there is a reproducible situation, it should be
			 * investigated in detail.
			 */
			return -2;
		}

		/* Only in case n=0 (timeout), repeat calling the write function */

		/* If send failed, wait before retry */
		if (fp != NULL) {
			/* For files, just wait a fixed time.
			 * Maybe it helps, maybe not. */
			mg_sleep(5);
		} else {
			/* For sockets, wait for the socket using poll */
			struct mg_pollfd pfd[2];
			int pollres;
			unsigned int num_sock = 1;

			pfd[0].fd = sock;
			pfd[0].events = POLLOUT;

			if (ctx->context_type == CONTEXT_SERVER) {
				pfd[num_sock].fd = ctx->thread_shutdown_notification_socket;
				pfd[num_sock].events = POLLIN;
				num_sock++;
			}

			pollres = mg_poll(pfd, num_sock, (int)(ms_wait), &(ctx->stop_flag));
			if (!STOP_FLAG_IS_ZERO(&ctx->stop_flag)) {
				return -2;
			}
			if (pollres > 0) {
				continue;
			}
		}

		if (timeout > 0) {
			now = mg_get_current_time_ns();
			if ((now - start) > timeout_ns) {
				/* Timeout */
				break;
			}
		}
	}

	(void)err; /* Avoid unused warning if NO_SSL is set and DEBUG_TRACE is not
	           used */

	return -1;
}


static int
push_all(struct mg_context *ctx,
         FILE *fp,
         SOCKET sock,
         SSL *ssl,
         const char *buf,
         int len)
{
	double timeout = -1.0;
	int n, nwritten = 0;

	if (ctx == NULL) {
		return -1;
	}

	if (ctx->dd.config[REQUEST_TIMEOUT]) {
		timeout = atoi(ctx->dd.config[REQUEST_TIMEOUT]) / 1000.0;
	}
	if (timeout <= 0.0) {
		timeout = strtod(config_options[REQUEST_TIMEOUT].default_value, NULL)
		          / 1000.0;
	}

	while ((len > 0) && STOP_FLAG_IS_ZERO(&ctx->stop_flag)) {
		n = push_inner(ctx, fp, sock, ssl, buf + nwritten, len, timeout);
		if (n < 0) {
			if (nwritten == 0) {
				nwritten = -1; /* Propagate the error */
			}
			break;
		} else if (n == 0) {
			break; /* No more data to write */
		} else {
			nwritten += n;
			len -= n;
		}
	}

	return nwritten;
}


/* Read from IO channel - opened file descriptor, socket, or SSL descriptor.
 * Return value:
 *  >=0 .. number of bytes successfully read
 *   -1 .. timeout
 *   -2 .. error
 */
static int
pull_inner(FILE *fp,
           struct mg_connection *conn,
           char *buf,
           int len,
           double timeout)
{
	int nread, err = 0;

#if defined(_WIN32)
	typedef int len_t;
#else
	typedef size_t len_t;
#endif

	/* We need an additional wait loop around this, because in some cases
	 * with TLSwe may get data from the socket but not from SSL_read.
	 * In this case we need to repeat at least once.
	 */

	if (fp != NULL) {
		/* Use read() instead of fread(), because if we're reading from the
		 * CGI pipe, fread() may block until IO buffer is filled up. We
		 * cannot afford to block and must pass all read bytes immediately
		 * to the client. */
		nread = (int)read(fileno(fp), buf, (size_t)len);

		err = (nread < 0) ? ERRNO : 0;
		if ((nread == 0) && (len > 0)) {
			/* Should get data, but got EOL */
			return -2;
		}

#if defined(USE_MBEDTLS)
	} else if (conn->ssl != NULL) {
		struct mg_pollfd pfd[2];
		int to_read;
		int pollres;
		unsigned int num_sock = 1;

		to_read = mbedtls_ssl_get_bytes_avail(conn->ssl);

		if (to_read > 0) {
			/* We already know there is no more data buffered in conn->buf
			 * but there is more available in the SSL layer. So don't poll
			 * conn->client.sock yet. */

			pollres = 1;
			if (to_read > len)
				to_read = len;
		} else {
			pfd[0].fd = conn->client.sock;
			pfd[0].events = POLLIN;

			if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
				pfd[num_sock].fd =
				    conn->phys_ctx->thread_shutdown_notification_socket;
				pfd[num_sock].events = POLLIN;
				num_sock++;
			}

			to_read = len;

			pollres = mg_poll(pfd,
			                  num_sock,
			                  (int)(timeout * 1000.0),
			                  &(conn->phys_ctx->stop_flag));

			if (!STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
				return -2;
			}
		}

		if (pollres > 0) {
			nread = mbed_ssl_read(conn->ssl, (unsigned char *)buf, to_read);
			if (nread <= 0) {
				if ((nread == MBEDTLS_ERR_SSL_WANT_READ)
				    || (nread == MBEDTLS_ERR_SSL_WANT_WRITE)
				    || nread == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS) {
					nread = 0;
				} else {
					fprintf(stderr, "SSL read failed, error %d\n", nread);
					return -2;
				}
			} else {
				err = 0;
			}

		} else if (pollres < 0) {
			/* Error */
			return -2;
		} else {
			/* pollres = 0 means timeout */
			nread = 0;
		}

#elif defined(USE_GNUTLS)
	} else if (conn->ssl != NULL) {
		struct mg_pollfd pfd[2];
		size_t to_read;
		int pollres;
		unsigned int num_sock = 1;

		to_read = gnutls_record_check_pending(conn->ssl->sess);

		if (to_read > 0) {
			/* We already know there is no more data buffered in conn->buf
			 * but there is more available in the SSL layer. So don't poll
			 * conn->client.sock yet. */

			pollres = 1;
			if (to_read > (size_t)len)
				to_read = (size_t)len;
		} else {
			pfd[0].fd = conn->client.sock;
			pfd[0].events = POLLIN;

			if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
				pfd[num_sock].fd =
				    conn->phys_ctx->thread_shutdown_notification_socket;
				pfd[num_sock].events = POLLIN;
				num_sock++;
			}

			to_read = (size_t)len;

			pollres = mg_poll(pfd,
			                  num_sock,
			                  (int)(timeout * 1000.0),
			                  &(conn->phys_ctx->stop_flag));

			if (!STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
				return -2;
			}
		}

		if (pollres > 0) {
			nread = gtls_ssl_read(conn->ssl, (unsigned char *)buf, to_read);
			if (nread < 0) {
				fprintf(stderr, "SSL read failed (%d): %s", nread, gnutls_strerror(nread));
				return -2;
			} else {
				err = 0;
			}
		} else if (pollres < 0) {
			/* Error */
			return -2;
		} else {
			/* pollres = 0 means timeout */
			nread = 0;
		}

#elif !defined(NO_SSL)
	} else if (conn->ssl != NULL) {
		int ssl_pending;
		struct mg_pollfd pfd[2];
		int pollres;
		unsigned int num_sock = 1;

		if ((ssl_pending = SSL_pending(conn->ssl)) > 0) {
			/* We already know there is no more data buffered in conn->buf
			 * but there is more available in the SSL layer. So don't poll
			 * conn->client.sock yet. */
			if (ssl_pending > len) {
				ssl_pending = len;
			}
			pollres = 1;
		} else {
			pfd[0].fd = conn->client.sock;
			pfd[0].events = POLLIN;

			if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
				pfd[num_sock].fd =
				    conn->phys_ctx->thread_shutdown_notification_socket;
				pfd[num_sock].events = POLLIN;
				num_sock++;
			}

			pollres = mg_poll(pfd,
			                  num_sock,
			                  (int)(timeout * 1000.0),
			                  &(conn->phys_ctx->stop_flag));
			if (!STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
				return -2;
			}
		}
		if (pollres > 0) {
			ERR_clear_error();
			nread =
			    SSL_read(conn->ssl, buf, (ssl_pending > 0) ? ssl_pending : len);
			if (nread <= 0) {
				err = SSL_get_error(conn->ssl, nread);
				if ((err == SSL_ERROR_SYSCALL) && (nread == -1)) {
					err = ERRNO;
				} else if ((err == SSL_ERROR_WANT_READ)
				           || (err == SSL_ERROR_WANT_WRITE)) {
					nread = 0;
				} else {
					/* All errors should return -2 */
					DEBUG_TRACE("SSL_read() failed, error %d", err);
					ERR_clear_error();
					return -2;
				}
				ERR_clear_error();
			} else {
				err = 0;
			}
		} else if (pollres < 0) {
			/* Error */
			return -2;
		} else {
			/* pollres = 0 means timeout */
			nread = 0;
		}
#endif

	} else {
		struct mg_pollfd pfd[2];
		int pollres;
		unsigned int num_sock = 1;

		pfd[0].fd = conn->client.sock;
		pfd[0].events = POLLIN;

		if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
			pfd[num_sock].fd =
			    conn->phys_ctx->thread_shutdown_notification_socket;
			pfd[num_sock].events = POLLIN;
			num_sock++;
		}

		pollres = mg_poll(pfd,
		                  num_sock,
		                  (int)(timeout * 1000.0),
		                  &(conn->phys_ctx->stop_flag));
		if (!STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
			return -2;
		}
		if (pollres > 0) {
			nread = (int)recv(conn->client.sock, buf, (len_t)len, 0);
			err = (nread < 0) ? ERRNO : 0;
			if (nread <= 0) {
				/* shutdown of the socket at client side */
				return -2;
			}
		} else if (pollres < 0) {
			/* error calling poll */
			return -2;
		} else {
			/* pollres = 0 means timeout */
			nread = 0;
		}
	}

	if (conn != NULL && !STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
		return -2;
	}

	if ((nread > 0) || ((nread == 0) && (len == 0))) {
		/* some data has been read, or no data was requested */
		return nread;
	}

	if (nread < 0) {
		/* socket error - check errno */
#if defined(_WIN32)
		if (err == WSAEWOULDBLOCK) {
			/* TODO (low): check if this is still required */
			/* standard case if called from close_socket_gracefully */
			return -2;
		} else if (err == WSAETIMEDOUT) {
			/* TODO (low): check if this is still required */
			/* timeout is handled by the while loop  */
			return 0;
		} else if (err == WSAECONNABORTED) {
			/* See https://www.chilkatsoft.com/p/p_299.asp */
			return -2;
		} else {
			DEBUG_TRACE("read()/recv() failed, error %d", err);
			return -2;
		}
#else
		/* TODO: POSIX returns either EAGAIN or EWOULDBLOCK in both cases,
		 * if the timeout is reached and if the socket was set to non-
		 * blocking in close_socket_gracefully, so we can not distinguish
		 * here. We have to wait for the timeout in both cases for now.
		 */
		if (ERROR_TRY_AGAIN(err)) {
			/* TODO (low): check if this is still required */
			/* EAGAIN/EWOULDBLOCK:
			 * standard case if called from close_socket_gracefully
			 * => should return -1 */
			/* or timeout occurred
			 * => the code must stay in the while loop */

			/* EINTR can be generated on a socket with a timeout set even
			 * when SA_RESTART is effective for all relevant signals
			 * (see signal(7)).
			 * => stay in the while loop */
		} else {
			DEBUG_TRACE("read()/recv() failed, error %d", err);
			return -2;
		}
#endif
	}

	/* Timeout occurred, but no data available. */
	return -1;
}


static int
pull_all(FILE *fp, struct mg_connection *conn, char *buf, int len)
{
	int n, nread = 0;
	double timeout = -1.0;
	uint64_t start_time = 0, now = 0, timeout_ns = 0;

	if (conn->dom_ctx->config[REQUEST_TIMEOUT]) {
		timeout = atoi(conn->dom_ctx->config[REQUEST_TIMEOUT]) / 1000.0;
	}
	if (timeout <= 0.0) {
		timeout = strtod(config_options[REQUEST_TIMEOUT].default_value, NULL)
		          / 1000.0;
	}
	start_time = mg_get_current_time_ns();
	timeout_ns = (uint64_t)(timeout * 1.0E9);

	while ((len > 0) && STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
		n = pull_inner(fp, conn, buf + nread, len, timeout);
		if (n == -2) {
			if (nread == 0) {
				nread = -1; /* Propagate the error */
			}
			break;
		} else if (n == -1) {
			/* timeout */
			if (timeout >= 0.0) {
				now = mg_get_current_time_ns();
				if ((now - start_time) <= timeout_ns) {
					continue;
				}
			}
			break;
		} else if (n == 0) {
			break; /* No more data to read */
		} else {
			nread += n;
			len -= n;
		}
	}

	return nread;
}


static void
discard_unread_request_data(struct mg_connection *conn)
{
	char buf[MG_BUF_LEN];

	while (mg_read(conn, buf, sizeof(buf)) > 0)
		;
}


static int
mg_read_inner(struct mg_connection *conn, void *buf, size_t len)
{
	int64_t content_len, n, buffered_len, nread;
	int64_t len64 =
	    (int64_t)((len > INT_MAX) ? INT_MAX : len); /* since the return value is
	                                                 * int, we may not read more
	                                                 * bytes */
	const char *body;

	if (conn == NULL) {
		return 0;
	}

	/* If Content-Length is not set for a response with body data,
	 * we do not know in advance how much data should be read. */
	content_len = conn->content_len;
	if (content_len < 0) {
		/* The body data is completed when the connection is closed. */
		content_len = INT64_MAX;
	}

	nread = 0;
	if (conn->consumed_content < content_len) {
		/* Adjust number of bytes to read. */
		int64_t left_to_read = content_len - conn->consumed_content;
		if (left_to_read < len64) {
			/* Do not read more than the total content length of the
			 * request.
			 */
			len64 = left_to_read;
		}

		/* Return buffered data */
		buffered_len = (int64_t)(conn->data_len) - (int64_t)conn->request_len
		               - conn->consumed_content;
		if (buffered_len > 0) {
			if (len64 < buffered_len) {
				buffered_len = len64;
			}
			body = conn->buf + conn->request_len + conn->consumed_content;
			memcpy(buf, body, (size_t)buffered_len);
			len64 -= buffered_len;
			conn->consumed_content += buffered_len;
			nread += buffered_len;
			buf = (char *)buf + buffered_len;
		}

		/* We have returned all buffered data. Read new data from the remote
		 * socket.
		 */
		if ((n = pull_all(NULL, conn, (char *)buf, (int)len64)) >= 0) {
			conn->consumed_content += n;
			nread += n;
		} else {
			nread = ((nread > 0) ? nread : n);
		}
	}
	return (int)nread;
}


/* Forward declarations */
static void handle_request(struct mg_connection *);
static void log_access(const struct mg_connection *);


/* Handle request, update statistics and call access log */
static void
handle_request_stat_log(struct mg_connection *conn)
{
#if defined(USE_SERVER_STATS)
	struct timespec tnow;
	conn->conn_state = 4; /* processing */
#endif

	handle_request(conn);


#if defined(USE_SERVER_STATS)
	conn->conn_state = 5; /* processed */

	clock_gettime(CLOCK_MONOTONIC, &tnow);
	conn->processing_time = mg_difftimespec(&tnow, &(conn->req_time));

	mg_atomic_add64(&(conn->phys_ctx->total_data_read), conn->consumed_content);
	mg_atomic_add64(&(conn->phys_ctx->total_data_written),
	                conn->num_bytes_sent);
#endif

	DEBUG_TRACE("%s", "handle_request done");

	if (conn->phys_ctx->callbacks.end_request != NULL) {
		conn->phys_ctx->callbacks.end_request(conn, conn->status_code);
		DEBUG_TRACE("%s", "end_request callback done");
	}
	log_access(conn);
}


#if defined(USE_HTTP2)
#if defined(NO_SSL)
#error "HTTP2 requires ALPN, ALPN requires SSL/TLS"
#endif
#define USE_ALPN
#include "http2.inl"
/* Not supported with HTTP/2 */
#define HTTP1_only                                                             \
	{                                                                          \
		if (conn->protocol_type == PROTOCOL_TYPE_HTTP2) {                      \
			http2_must_use_http1(conn);                                        \
			DEBUG_TRACE("%s", "must use HTTP/1.x");                            \
			return;                                                            \
		}                                                                      \
	}
#else
#define HTTP1_only
#endif


CIVETWEB_API int
mg_read(struct mg_connection *conn, void *buf, size_t len)
{
	if (len > INT_MAX) {
		len = INT_MAX;
	}

	if (conn == NULL) {
		return 0;
	}

	if (conn->is_chunked) {
		size_t all_read = 0;

		while (len > 0) {
			if (conn->is_chunked >= 3) {
				/* No more data left to read */
				return 0;
			}
			if (conn->is_chunked != 1) {
				/* Has error */
				return -1;
			}

			if (conn->consumed_content != conn->content_len) {
				/* copy from the current chunk */
				int read_ret = mg_read_inner(conn, (char *)buf + all_read, len);

				if (read_ret < 1) {
					/* read error */
					conn->is_chunked = 2;
					return -1;
				}

				all_read += (size_t)read_ret;
				len -= (size_t)read_ret;

				if (conn->consumed_content == conn->content_len) {
					/* Add data bytes in the current chunk have been read,
					 * so we are expecting \r\n now. */
					char x[2];
					conn->content_len += 2;
					if ((mg_read_inner(conn, x, 2) != 2) || (x[0] != '\r')
					    || (x[1] != '\n')) {
						/* Protocol violation */
						conn->is_chunked = 2;
						return -1;
					}
				}

			} else {
				/* fetch a new chunk */
				size_t i;
				char lenbuf[64];
				char *end = NULL;
				unsigned long chunkSize = 0;

				for (i = 0; i < (sizeof(lenbuf) - 1); i++) {
					conn->content_len++;
					if (mg_read_inner(conn, lenbuf + i, 1) != 1) {
						lenbuf[i] = 0;
					}
					if ((i > 0) && (lenbuf[i] == ';')) {
						// chunk extension --> skip chars until next CR
						//
						// RFC 2616, 3.6.1 Chunked Transfer Coding
						// (https://www.rfc-editor.org/rfc/rfc2616#page-25)
						//
						// chunk          = chunk-size [ chunk-extension ] CRLF
						//                  chunk-data CRLF
						// ...
						// chunk-extension= *( ";" chunk-ext-name [ "="
						// chunk-ext-val ] )
						do
							++conn->content_len;
						while (mg_read_inner(conn, lenbuf + i, 1) == 1
						       && lenbuf[i] != '\r');
					}
					if ((i > 0) && (lenbuf[i] == '\r')
					    && (lenbuf[i - 1] != '\r')) {
						continue;
					}
					if ((i > 1) && (lenbuf[i] == '\n')
					    && (lenbuf[i - 1] == '\r')) {
						lenbuf[i + 1] = 0;
						chunkSize = strtoul(lenbuf, &end, 16);
						if (chunkSize == 0) {
							/* regular end of content */
							conn->is_chunked = 3;
						}
						break;
					}
					if (!isxdigit((unsigned char)lenbuf[i])) {
						/* illegal character for chunk length */
						conn->is_chunked = 2;
						return -1;
					}
				}
				if ((end == NULL) || (*end != '\r')) {
					/* chunksize not set correctly */
					conn->is_chunked = 2;
					return -1;
				}
				if (conn->is_chunked == 3) {
					/* try discarding trailer for keep-alive */

					// We found the last chunk (length 0) including the
					// CRLF that terminates that chunk. Now follows a possibly
					// empty trailer and a final CRLF.
					//
					// see RFC 2616, 3.6.1 Chunked Transfer Coding
					// (https://www.rfc-editor.org/rfc/rfc2616#page-25)
					//
					// Chunked-Body   = *chunk
					// 	                last-chunk
					// 	                trailer
					// 	                CRLF
					// ...
					// last-chunk     = 1*("0") [ chunk-extension ] CRLF
					// ...
					// trailer        = *(entity-header CRLF)

					int crlf_count = 2; // one CRLF already determined

					while (crlf_count < 4 && conn->is_chunked == 3) {
						++conn->content_len;
						if (mg_read_inner(conn, lenbuf, 1) == 1) {
							if ((crlf_count == 0 || crlf_count == 2)) {
								if (lenbuf[0] == '\r')
									++crlf_count;
								else
									crlf_count = 0;
							} else {
								// previous character was a CR
								// --> next character must be LF

								if (lenbuf[0] == '\n')
									++crlf_count;
								else
									conn->is_chunked = 2;
							}
						} else
							// premature end of trailer
							conn->is_chunked = 2;
					}

					if (conn->is_chunked == 2)
						return -1;
					else
						conn->is_chunked = 4;

					break;
				}

				/* append a new chunk */
				conn->content_len += (int64_t)chunkSize;
			}
		}

		return (int)all_read;
	}
	return mg_read_inner(conn, buf, len);
}


CIVETWEB_API int
mg_write(struct mg_connection *conn, const void *buf, size_t len)
{
	time_t now;
	int n, total, allowed;

	if (conn == NULL) {
		return 0;
	}
	if (len > INT_MAX) {
		return -1;
	}

	/* Mark connection as "data sent" */
	conn->request_state = 10;
#if defined(USE_HTTP2)
	if (conn->protocol_type == PROTOCOL_TYPE_HTTP2) {
		http2_data_frame_head(conn, len, 0);
	}
#endif

	if (conn->throttle > 0) {
		if ((now = time(NULL)) != conn->last_throttle_time) {
			conn->last_throttle_time = now;
			conn->last_throttle_bytes = 0;
		}
		allowed = conn->throttle - conn->last_throttle_bytes;
		if (allowed > (int)len) {
			allowed = (int)len;
		}

		total = push_all(conn->phys_ctx,
		                 NULL,
		                 conn->client.sock,
		                 conn->ssl,
		                 (const char *)buf,
		                 allowed);

		if (total == allowed) {

			buf = (const char *)buf + total;
			conn->last_throttle_bytes += total;
			while ((total < (int)len)
			       && STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
				allowed = (conn->throttle > ((int)len - total))
				              ? (int)len - total
				              : conn->throttle;

				n = push_all(conn->phys_ctx,
				             NULL,
				             conn->client.sock,
				             conn->ssl,
				             (const char *)buf,
				             allowed);

				if (n != allowed) {
					break;
				}
				sleep(1);
				conn->last_throttle_bytes = allowed;
				conn->last_throttle_time = time(NULL);
				buf = (const char *)buf + n;
				total += n;
			}
		}
	} else {
		total = push_all(conn->phys_ctx,
		                 NULL,
		                 conn->client.sock,
		                 conn->ssl,
		                 (const char *)buf,
		                 (int)len);
	}
	if (total > 0) {
		conn->num_bytes_sent += total;
	}
	return total;
}


/* Send a chunk, if "Transfer-Encoding: chunked" is used */
CIVETWEB_API int
mg_send_chunk(struct mg_connection *conn,
              const char *chunk,
              unsigned int chunk_len)
{
	char lenbuf[16];
	size_t lenbuf_len;
	int ret;
	int t;

	/* First store the length information in a text buffer. */
	sprintf(lenbuf, "%x\r\n", chunk_len);
	lenbuf_len = strlen(lenbuf);

	/* Then send length information, chunk and terminating \r\n. */
	ret = mg_write(conn, lenbuf, lenbuf_len);
	if (ret != (int)lenbuf_len) {
		return -1;
	}
	t = ret;

	ret = mg_write(conn, chunk, chunk_len);
	if (ret != (int)chunk_len) {
		return -1;
	}
	t += ret;

	ret = mg_write(conn, "\r\n", 2);
	if (ret != 2) {
		return -1;
	}
	t += ret;

	return t;
}


#if defined(GCC_DIAGNOSTIC)
/* This block forwards format strings to printf implementations,
 * so we need to disable the format-nonliteral warning. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif


/* Alternative alloc_vprintf() for non-compliant C runtimes */
static int
alloc_vprintf2(char **buf, const char *fmt, va_list ap)
{
	va_list ap_copy;
	size_t size = MG_BUF_LEN / 4;
	int len = -1;

	*buf = NULL;
	while (len < 0) {
		if (*buf) {
			mg_free(*buf);
		}

		size *= 4;
		*buf = (char *)mg_malloc(size);
		if (!*buf) {
			break;
		}

		va_copy(ap_copy, ap);
		len = vsnprintf_impl(*buf, size - 1, fmt, ap_copy);
		va_end(ap_copy);
		(*buf)[size - 1] = 0;
	}

	return len;
}


/* Print message to buffer. If buffer is large enough to hold the message,
 * return buffer. If buffer is to small, allocate large enough buffer on
 * heap,
 * and return allocated buffer. */
static int
alloc_vprintf(char **out_buf,
              char *prealloc_buf,
              size_t prealloc_size,
              const char *fmt,
              va_list ap)
{
	va_list ap_copy;
	int len;

	/* Windows is not standard-compliant, and vsnprintf() returns -1 if
	 * buffer is too small. Also, older versions of msvcrt.dll do not have
	 * _vscprintf().  However, if size is 0, vsnprintf() behaves correctly.
	 * Therefore, we make two passes: on first pass, get required message
	 * length.
	 * On second pass, actually print the message. */
	va_copy(ap_copy, ap);
	len = vsnprintf_impl(NULL, 0, fmt, ap_copy);
	va_end(ap_copy);

	if (len < 0) {
		/* C runtime is not standard compliant, vsnprintf() returned -1.
		 * Switch to alternative code path that uses incremental
		 * allocations.
		 */
		va_copy(ap_copy, ap);
		len = alloc_vprintf2(out_buf, fmt, ap_copy);
		va_end(ap_copy);

	} else if ((size_t)(len) >= prealloc_size) {
		/* The pre-allocated buffer not large enough. */
		/* Allocate a new buffer. */
		*out_buf = (char *)mg_malloc((size_t)(len) + 1);
		if (!*out_buf) {
			/* Allocation failed. Return -1 as "out of memory" error. */
			return -1;
		}
		/* Buffer allocation successful. Store the string there. */
		va_copy(ap_copy, ap);
		IGNORE_UNUSED_RESULT(
		    vsnprintf_impl(*out_buf, (size_t)(len) + 1, fmt, ap_copy));
		va_end(ap_copy);

	} else {
		/* The pre-allocated buffer is large enough.
		 * Use it to store the string and return the address. */
		va_copy(ap_copy, ap);
		IGNORE_UNUSED_RESULT(
		    vsnprintf_impl(prealloc_buf, prealloc_size, fmt, ap_copy));
		va_end(ap_copy);
		*out_buf = prealloc_buf;
	}

	return len;
}


static int
alloc_printf(char **out_buf, const char *fmt, ...)
{
	va_list ap;
	int result;

	va_start(ap, fmt);
	result = alloc_vprintf(out_buf, NULL, 0, fmt, ap);
	va_end(ap);

	return result;
}


#if defined(GCC_DIAGNOSTIC)
/* Enable format-nonliteral warning again. */
#pragma GCC diagnostic pop
#endif


static int
mg_vprintf(struct mg_connection *conn, const char *fmt, va_list ap)
{
	char mem[MG_BUF_LEN];
	char *buf = NULL;
	int len;

	if ((len = alloc_vprintf(&buf, mem, sizeof(mem), fmt, ap)) > 0) {
		len = mg_write(conn, buf, (size_t)len);
	}
	if (buf != mem) {
		mg_free(buf);
	}

	return len;
}


CIVETWEB_API int
mg_printf(struct mg_connection *conn, const char *fmt, ...)
{
	va_list ap;
	int result;

	va_start(ap, fmt);
	result = mg_vprintf(conn, fmt, ap);
	va_end(ap);

	return result;
}


CIVETWEB_API int
mg_url_decode(const char *src,
              int src_len,
              char *dst,
              int dst_len,
              int is_form_url_encoded)
{
	int i, j, a, b;
#define HEXTOI(x) (isdigit(x) ? (x - '0') : (x - 'W'))

	for (i = j = 0; (i < src_len) && (j < (dst_len - 1)); i++, j++) {
		if ((i < src_len - 2) && (src[i] == '%')
		    && isxdigit((unsigned char)src[i + 1])
		    && isxdigit((unsigned char)src[i + 2])) {
			a = tolower((unsigned char)src[i + 1]);
			b = tolower((unsigned char)src[i + 2]);
			dst[j] = (char)((HEXTOI(a) << 4) | HEXTOI(b));
			i += 2;
		} else if (is_form_url_encoded && (src[i] == '+')) {
			dst[j] = ' ';
		} else {
			dst[j] = src[i];
		}
	}

	dst[j] = '\0'; /* Null-terminate the destination */

	return (i >= src_len) ? j : -1;
}


/* form url decoding of an entire string */
static void
url_decode_in_place(char *buf)
{
	int len = (int)strlen(buf);
	(void)mg_url_decode(buf, len, buf, len + 1, 1);
}


CIVETWEB_API int
mg_get_var(const char *data,
           size_t data_len,
           const char *name,
           char *dst,
           size_t dst_len)
{
	return mg_get_var2(data, data_len, name, dst, dst_len, 0);
}


CIVETWEB_API int
mg_get_var2(const char *data,
            size_t data_len,
            const char *name,
            char *dst,
            size_t dst_len,
            size_t occurrence)
{
	const char *p, *e, *s;
	size_t name_len;
	int len;

	if ((dst == NULL) || (dst_len == 0)) {
		len = -2;
	} else if ((data == NULL) || (name == NULL) || (data_len == 0)) {
		len = -1;
		dst[0] = '\0';
	} else {
		name_len = strlen(name);
		e = data + data_len;
		len = -1;
		dst[0] = '\0';

		/* data is "var1=val1&var2=val2...". Find variable first */
		for (p = data; p + name_len < e; p++) {
			if (((p == data) || (p[-1] == '&')) && (p[name_len] == '=')
			    && !mg_strncasecmp(name, p, name_len) && 0 == occurrence--) {
				/* Point p to variable value */
				p += name_len + 1;

				/* Point s to the end of the value */
				s = (const char *)memchr(p, '&', (size_t)(e - p));
				if (s == NULL) {
					s = e;
				}
				DEBUG_ASSERT(s >= p);
				if (s < p) {
					return -3;
				}

				/* Decode variable into destination buffer */
				len = mg_url_decode(p, (int)(s - p), dst, (int)dst_len, 1);

				/* Redirect error code from -1 to -2 (destination buffer too
				 * small). */
				if (len == -1) {
					len = -2;
				}
				break;
			}
		}
	}

	return len;
}


/* split a string "key1=val1&key2=val2" into key/value pairs */
CIVETWEB_API int
mg_split_form_urlencoded(char *data,
                         struct mg_header *form_fields,
                         unsigned num_form_fields)
{
	char *b;
	int i;
	int num = 0;

	if (data == NULL) {
		/* parameter error */
		return -1;
	}

	if ((form_fields == NULL) && (num_form_fields == 0)) {
		/* determine the number of expected fields */
		if (data[0] == 0) {
			return 0;
		}
		/* count number of & to return the number of key-value-pairs */
		num = 1;
		while (*data) {
			if (*data == '&') {
				num++;
			}
			data++;
		}
		return num;
	}

	if ((form_fields == NULL) || ((int)num_form_fields <= 0)) {
		/* parameter error */
		return -1;
	}

	for (i = 0; i < (int)num_form_fields; i++) {
		/* extract key-value pairs from input data */
		while ((*data == ' ') || (*data == '\t')) {
			/* skip initial spaces */
			data++;
		}
		if (*data == 0) {
			/* end of string reached */
			break;
		}
		form_fields[num].name = data;

		/* find & or = */
		b = data;
		while ((*b != 0) && (*b != '&') && (*b != '=')) {
			b++;
		}

		if (*b == 0) {
			/* last key without value */
			form_fields[num].value = NULL;
		} else if (*b == '&') {
			/* mid key without value */
			form_fields[num].value = NULL;
		} else {
			/* terminate string */
			*b = 0;
			/* value starts after '=' */
			data = b + 1;
			form_fields[num].value = data;
		}

		/* new field is stored */
		num++;

		/* find a next key */
		b = strchr(data, '&');
		if (b == 0) {
			/* no more data */
			break;
		} else {
			/* terminate value of last field at '&' */
			*b = 0;
			/* next key-value-pairs starts after '&' */
			data = b + 1;
		}
	}

	/* Decode all values */
	for (i = 0; i < num; i++) {
		if (form_fields[i].name) {
			url_decode_in_place((char *)form_fields[i].name);
		}
		if (form_fields[i].value) {
			url_decode_in_place((char *)form_fields[i].value);
		}
	}

	/* return number of fields found */
	return num;
}


/* HCP24: some changes to compare whole var_name */
CIVETWEB_API int
mg_get_cookie(const char *cookie_header,
              const char *var_name,
              char *dst,
              size_t dst_size)
{
	const char *s, *p, *end;
	int name_len, len = -1;

	if ((dst == NULL) || (dst_size == 0)) {
		return -2;
	}

	dst[0] = '\0';
	if ((var_name == NULL) || ((s = cookie_header) == NULL)) {
		return -1;
	}

	name_len = (int)strlen(var_name);
	end = s + strlen(s);
	for (; (s = mg_strcasestr(s, var_name)) != NULL; s += name_len) {
		if (s[name_len] == '=') {
			/* HCP24: now check is it a substring or a full cookie name */
			if ((s == cookie_header) || (s[-1] == ' ')) {
				s += name_len + 1;
				if ((p = strchr(s, ' ')) == NULL) {
					p = end;
				}
				if (p[-1] == ';') {
					p--;
				}
				if ((*s == '"') && (p[-1] == '"') && (p > s + 1)) {
					s++;
					p--;
				}
				if ((size_t)(p - s) < dst_size) {
					len = (int)(p - s);
					mg_strlcpy(dst, s, (size_t)len + 1);
				} else {
					len = -3;
				}
				break;
			}
		}
	}
	return len;
}


CIVETWEB_API int
mg_base64_encode(const unsigned char *src,
                 size_t src_len,
                 char *dst,
                 size_t *dst_len)
{
	static const char *b64 =
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	size_t i, j;
	int a, b, c;

	if (dst_len != NULL) {
		/* Expected length including 0 termination: */
		/* IN 1 -> OUT 5, IN 2 -> OUT 5, IN 3 -> OUT 5, IN 4 -> OUT 9,
		 * IN 5 -> OUT 9, IN 6 -> OUT 9, IN 7 -> OUT 13, etc. */
		size_t expected_len = ((src_len + 2) / 3) * 4 + 1;
		if (*dst_len < expected_len) {
			if (*dst_len > 0) {
				dst[0] = '\0';
			}
			*dst_len = expected_len;
			return 0;
		}
	}

	for (i = j = 0; i < src_len; i += 3) {
		a = src[i];
		b = ((i + 1) >= src_len) ? 0 : src[i + 1];
		c = ((i + 2) >= src_len) ? 0 : src[i + 2];

		dst[j++] = b64[a >> 2];
		dst[j++] = b64[((a & 3) << 4) | (b >> 4)];
		if (i + 1 < src_len) {
			dst[j++] = b64[(b & 15) << 2 | (c >> 6)];
		}
		if (i + 2 < src_len) {
			dst[j++] = b64[c & 63];
		}
	}
	while (j % 4 != 0) {
		dst[j++] = '=';
	}
	dst[j++] = '\0';

	if (dst_len != NULL) {
		*dst_len = (size_t)j;
	}

	/* Return -1 for "OK" */
	return -1;
}


static unsigned char
b64reverse(char letter)
{
	if ((letter >= 'A') && (letter <= 'Z')) {
		return (unsigned char)(letter - 'A');
	}
	if ((letter >= 'a') && (letter <= 'z')) {
		return (unsigned char)(letter - 'a' + 26);
	}
	if ((letter >= '0') && (letter <= '9')) {
		return (unsigned char)(letter - '0' + 52);
	}
	if (letter == '+') {
		return 62;
	}
	if (letter == '/') {
		return 63;
	}
	if (letter == '=') {
		return 255; /* normal end */
	}
	return 254; /* error */
}


CIVETWEB_API int
mg_base64_decode(const char *src,
                 size_t src_len,
                 unsigned char *dst,
                 size_t *dst_len)
{
	size_t i;
	unsigned char a, b, c, d;
	size_t dst_len_limit = (size_t)-1;
	size_t dst_len_used = 0;

	if (dst_len != NULL) {
		dst_len_limit = *dst_len;
		*dst_len = 0;
	}

	for (i = 0; i < src_len; i += 4) {
		/* Read 4 characters from BASE64 string */
		a = b64reverse(src[i]);
		if (a >= 254) {
			return (int)i;
		}

		b = b64reverse(((i + 1) >= src_len) ? 0 : src[i + 1]);
		if (b >= 254) {
			return (int)i + 1;
		}

		c = b64reverse(((i + 2) >= src_len) ? 0 : src[i + 2]);
		if (c == 254) {
			return (int)i + 2;
		}

		d = b64reverse(((i + 3) >= src_len) ? 0 : src[i + 3]);
		if (d == 254) {
			return (int)i + 3;
		}

		/* Add first (of 3) decoded character */
		if (dst_len_used < dst_len_limit) {
			dst[dst_len_used] = (unsigned char)((unsigned char)(a << 2)
			                                    + (unsigned char)(b >> 4));
		}
		dst_len_used++;

		if (c != 255) {
			if (dst_len_used < dst_len_limit) {

				dst[dst_len_used] = (unsigned char)((unsigned char)(b << 4)
				                                    + (unsigned char)(c >> 2));
			}
			dst_len_used++;
			if (d != 255) {
				if (dst_len_used < dst_len_limit) {
					dst[dst_len_used] =
					    (unsigned char)((unsigned char)(c << 6) + d);
				}
				dst_len_used++;
			}
		}
	}

	/* Add terminating zero */
	if (dst_len_used < dst_len_limit) {
		dst[dst_len_used] = '\0';
	}
	dst_len_used++;
	if (dst_len != NULL) {
		*dst_len = dst_len_used;
	}

	if (dst_len_used > dst_len_limit) {
		/* Out of memory */
		return 0;
	}

	/* Return -1 for "OK" */
	return -1;
}


static int
is_put_or_delete_method(const struct mg_connection *conn)
{
	if (conn) {
		const char *s = conn->request_info.request_method;
		if (s != NULL) {
			/* PUT, DELETE, MKCOL, PATCH, LOCK, UNLOCK, PROPPATCH, MOVE, COPY */
			return (!strcmp(s, "PUT") || !strcmp(s, "DELETE")
			        || !strcmp(s, "MKCOL") || !strcmp(s, "PATCH")
			        || !strcmp(s, "LOCK") || !strcmp(s, "UNLOCK")
			        || !strcmp(s, "PROPPATCH") || !strcmp(s, "MOVE")
			        || !strcmp(s, "COPY"));
		}
	}
	return 0;
}


static int
is_civetweb_webdav_method(const struct mg_connection *conn)
{
	/* Note: Here we only have to identify the WebDav methods that need special
	 * handling in the CivetWeb code - not all methods used in WebDav. In
	 * particular, methods used on directories (when using Windows Explorer as
	 * WebDav client).
	 */
	if (conn) {
		const char *s = conn->request_info.request_method;
		if (s != NULL) {
			/* These are the civetweb builtin DAV methods */
			return (!strcmp(s, "PROPFIND") || !strcmp(s, "PROPPATCH")
			        || !strcmp(s, "LOCK") || !strcmp(s, "UNLOCK")
			        || !strcmp(s, "MOVE") || !strcmp(s, "COPY"));
		}
	}
	return 0;
}


#if !defined(NO_FILES)
static int
extention_matches_script(
    struct mg_connection *conn, /* in: request (must be valid) */
    const char *filename        /* in: filename  (must be valid) */
)
{
#if !defined(NO_CGI)
	int cgi_config_idx, inc, max;
#endif

#if defined(USE_LUA)
	if (match_prefix_strlen(conn->dom_ctx->config[LUA_SCRIPT_EXTENSIONS],
	                        filename)
	    > 0) {
		return 1;
	}
#endif
#if defined(USE_DUKTAPE)
	if (match_prefix_strlen(conn->dom_ctx->config[DUKTAPE_SCRIPT_EXTENSIONS],
	                        filename)
	    > 0) {
		return 1;
	}
#endif
#if !defined(NO_CGI)
	inc = CGI2_EXTENSIONS - CGI_EXTENSIONS;
	max = PUT_DELETE_PASSWORDS_FILE - CGI_EXTENSIONS;
	for (cgi_config_idx = 0; cgi_config_idx < max; cgi_config_idx += inc) {
		if ((conn->dom_ctx->config[CGI_EXTENSIONS + cgi_config_idx] != NULL)
		    && (match_prefix_strlen(
		            conn->dom_ctx->config[CGI_EXTENSIONS + cgi_config_idx],
		            filename)
		        > 0)) {
			return 1;
		}
	}
#endif
	/* filename and conn could be unused, if all preocessor conditions
	 * are false (no script language supported). */
	(void)filename;
	(void)conn;

	return 0;
}


static int
extention_matches_template_text(
    struct mg_connection *conn, /* in: request (must be valid) */
    const char *filename        /* in: filename  (must be valid) */
)
{
#if defined(USE_LUA)
	if (match_prefix_strlen(conn->dom_ctx->config[LUA_SERVER_PAGE_EXTENSIONS],
	                        filename)
	    > 0) {
		return 1;
	}
#endif
	if (match_prefix_strlen(conn->dom_ctx->config[SSI_EXTENSIONS], filename)
	    > 0) {
		return 1;
	}
	return 0;
}


/* For given directory path, substitute it to valid index file.
 * Return 1 if index file has been found, 0 if not found.
 * If the file is found, it's stats is returned in stp. */
static int
substitute_index_file_aux(struct mg_connection *conn,
                          char *path,
                          size_t path_len,
                          struct mg_file_stat *filestat)
{
	const char *list = conn->dom_ctx->config[INDEX_FILES];
	struct vec filename_vec;
	size_t n = strlen(path);
	int found = 0;

	/* The 'path' given to us points to the directory. Remove all trailing
	 * directory separator characters from the end of the path, and
	 * then append single directory separator character. */
	while ((n > 0) && (path[n - 1] == '/')) {
		n--;
	}
	path[n] = '/';

	/* Traverse index files list. For each entry, append it to the given
	 * path and see if the file exists. If it exists, break the loop */
	while ((list = next_option(list, &filename_vec, NULL)) != NULL) {
		/* Ignore too long entries that may overflow path buffer */
		if ((filename_vec.len + 1) > (path_len - (n + 1))) {
			continue;
		}

		/* Prepare full path to the index file */
		mg_strlcpy(path + n + 1, filename_vec.ptr, filename_vec.len + 1);

		/* Does it exist? */
		if (mg_stat(conn, path, filestat)) {
			/* Yes it does, break the loop */
			found = 1;
			break;
		}
	}

	/* If no index file exists, restore directory path */
	if (!found) {
		path[n] = '\0';
	}

	return found;
}

/* Same as above, except if the first try fails and a fallback-root is
 * configured, we'll try there also */
static int
substitute_index_file(struct mg_connection *conn,
                      char *path,
                      size_t path_len,
                      struct mg_file_stat *filestat)
{
	int ret = substitute_index_file_aux(conn, path, path_len, filestat);
	if (ret == 0) {
		const char *root_prefix = conn->dom_ctx->config[DOCUMENT_ROOT];
		const char *fallback_root_prefix =
		    conn->dom_ctx->config[FALLBACK_DOCUMENT_ROOT];
		if ((root_prefix) && (fallback_root_prefix)) {
			const size_t root_prefix_len = strlen(root_prefix);
			if ((strncmp(path, root_prefix, root_prefix_len) == 0)) {
				char scratch_path[UTF8_PATH_MAX]; /* separate storage, to avoid
				                                  side effects if we fail */
				size_t sub_path_len;

				const size_t fallback_root_prefix_len =
				    strlen(fallback_root_prefix);
				const char *sub_path = path + root_prefix_len;
				while (*sub_path == '/') {
					sub_path++;
				}
				sub_path_len = strlen(sub_path);

				if (((fallback_root_prefix_len + 1 + sub_path_len + 1)
				     < sizeof(scratch_path))) {
					/* The concatenations below are all safe because we
					 * pre-verified string lengths above */
					char *nul;
					strcpy(scratch_path, fallback_root_prefix);
					nul = strchr(scratch_path, '\0');
					if ((nul > scratch_path) && (*(nul - 1) != '/')) {
						*nul++ = '/';
						*nul = '\0';
					}
					strcat(scratch_path, sub_path);
					if (substitute_index_file_aux(conn,
					                              scratch_path,
					                              sizeof(scratch_path),
					                              filestat)) {
						mg_strlcpy(path, scratch_path, path_len);
						return 1;
					}
				}
			}
		}
	}
	return ret;
}

#endif


static void
interpret_uri(struct mg_connection *conn, /* in/out: request (must be valid) */
              char *filename,             /* out: filename */
              size_t filename_buf_len,    /* in: size of filename buffer */
              struct mg_file_stat *filestat, /* out: file status structure */
              int *is_found,                 /* out: file found (directly) */
              int *is_script_resource,       /* out: handled by a script? */
              int *is_websocket_request,     /* out: websocket connection? */
              int *is_put_or_delete_request, /* out: put/delete a file? */
              int *is_webdav_request,        /* out: webdav request? */
              int *is_template_text          /* out: SSI file or LSP file? */
)
{
	char const *accept_encoding;

#if !defined(NO_FILES)
	const char *uri = conn->request_info.local_uri;
	const char *roots[] = {conn->dom_ctx->config[DOCUMENT_ROOT],
	                       conn->dom_ctx->config[FALLBACK_DOCUMENT_ROOT],
	                       NULL};
	int fileExists = 0;
	const char *rewrite;
	struct vec a, b;
	ptrdiff_t match_len;
	char gz_path[UTF8_PATH_MAX];
	int truncated;
	int i;
#if !defined(NO_CGI) || defined(USE_LUA) || defined(USE_DUKTAPE)
	char *tmp_str;
	size_t tmp_str_len, sep_pos;
	int allow_substitute_script_subresources;
#endif
#else
	(void)filename_buf_len; /* unused if NO_FILES is defined */
#endif

	/* Step 1: Set all initially unknown outputs to zero */
	memset(filestat, 0, sizeof(*filestat));
	*filename = 0;
	*is_found = 0;
	*is_script_resource = 0;
	*is_template_text = 0;

	/* Step 2: Classify the request method */
	/* Step 2a: Check if the request attempts to modify the file system */
	*is_put_or_delete_request = is_put_or_delete_method(conn);
	/* Step 2b: Check if the request uses WebDav method that requires special
	 * handling */
	*is_webdav_request = is_civetweb_webdav_method(conn);

	/* Step 3: Check if it is a websocket request, and modify the document
	 * root if required */
#if defined(USE_WEBSOCKET)
	*is_websocket_request = (conn->protocol_type == PROTOCOL_TYPE_WEBSOCKET);
#if !defined(NO_FILES)
	if ((*is_websocket_request) && conn->dom_ctx->config[WEBSOCKET_ROOT]) {
		roots[0] = conn->dom_ctx->config[WEBSOCKET_ROOT];
		roots[1] = conn->dom_ctx->config[FALLBACK_WEBSOCKET_ROOT];
	}
#endif /* !NO_FILES */
#else  /* USE_WEBSOCKET */
	*is_websocket_request = 0;
#endif /* USE_WEBSOCKET */

	/* Step 4: Check if gzip encoded response is allowed */
	conn->accept_gzip = 0;
	if ((accept_encoding = mg_get_header(conn, "Accept-Encoding")) != NULL) {
		if (strstr(accept_encoding, "gzip") != NULL) {
			conn->accept_gzip = 1;
		}
	}

#if !defined(NO_FILES)
	/* Step 5: If there is no root directory, don't look for files. */
	/* Note that roots[0] == NULL is a regular use case here. This occurs,
	 * if all requests are handled by callbacks, so the WEBSOCKET_ROOT
	 * config is not required. */
	if (roots[0] == NULL) {
		/* all file related outputs have already been set to 0, just return
		 */
		return;
	}

	for (i = 0; roots[i] != NULL; i++) {
		/* Step 6: Determine the local file path from the root path and the
		 * request uri. */
		/* Using filename_buf_len - 1 because memmove() for PATH_INFO may shift
		 * part of the path one byte on the right. */
		truncated = 0;
		mg_snprintf(conn,
		            &truncated,
		            filename,
		            filename_buf_len - 1,
		            "%s%s",
		            roots[i],
		            uri);

		if (truncated) {
			goto interpret_cleanup;
		}

		/* Step 7: URI rewriting */
		rewrite = conn->dom_ctx->config[URL_REWRITE_PATTERN];
		while ((rewrite = next_option(rewrite, &a, &b)) != NULL) {
			if ((match_len = match_prefix(a.ptr, a.len, uri)) > 0) {
				mg_snprintf(conn,
				            &truncated,
				            filename,
				            filename_buf_len - 1,
				            "%.*s%s",
				            (int)b.len,
				            b.ptr,
				            uri + match_len);
				break;
			}
		}

		if (truncated) {
			goto interpret_cleanup;
		}

		/* Step 8: Check if the file exists at the server */
		/* Local file path and name, corresponding to requested URI
		 * is now stored in "filename" variable. */
		if (mg_stat(conn, filename, filestat)) {
			fileExists = 1;
			break;
		}
	}

	if (fileExists) {
		int uri_len = (int)strlen(uri);
		int is_uri_end_slash = (uri_len > 0) && (uri[uri_len - 1] == '/');

		/* 8.1: File exists. */
		*is_found = 1;

		/* 8.2: Check if it is a script type. */
		if (extention_matches_script(conn, filename)) {
			/* The request addresses a CGI resource, Lua script or
			 * server-side javascript.
			 * The URI corresponds to the script itself (like
			 * /path/script.cgi), and there is no additional resource
			 * path (like /path/script.cgi/something).
			 * Requests that modify (replace or delete) a resource, like
			 * PUT and DELETE requests, should replace/delete the script
			 * file.
			 * Requests that read or write from/to a resource, like GET and
			 * POST requests, should call the script and return the
			 * generated response. */
			*is_script_resource = (!*is_put_or_delete_request);
		}

		/* 8.3: Check for SSI and LSP files */
		if (extention_matches_template_text(conn, filename)) {
			/* Same as above, but for *.lsp and *.shtml files. */
			/* A "template text" is a file delivered directly to the client,
			 * but with some text tags replaced by dynamic content.
			 * E.g. a Server Side Include (SSI) or Lua Page/Lua Server Page
			 * (LP, LSP) file. */
			*is_template_text = (!*is_put_or_delete_request);
		}

		/* 8.4: If the request target is a directory, there could be
		 * a substitute file (index.html, index.cgi, ...). */
		/* But do not substitute a directory for a WebDav request */
		if (filestat->is_directory && is_uri_end_slash
		    && (!*is_webdav_request)) {
			/* Use a local copy here, since substitute_index_file will
			 * change the content of the file status */
			struct mg_file_stat tmp_filestat;
			memset(&tmp_filestat, 0, sizeof(tmp_filestat));

			if (substitute_index_file(
			        conn, filename, filename_buf_len, &tmp_filestat)) {

				/* Substitute file found. Copy stat to the output, then
				 * check if the file is a script file */
				*filestat = tmp_filestat;

				if (extention_matches_script(conn, filename)) {
					/* Substitute file is a script file */
					*is_script_resource = 1;
				} else if (extention_matches_template_text(conn, filename)) {
					/* Substitute file is a LSP or SSI file */
					*is_template_text = 1;
				} else {
					/* Substitute file is a regular file */
					*is_script_resource = 0;
					*is_found = (mg_stat(conn, filename, filestat) ? 1 : 0);
				}
			}
			/* If there is no substitute file, the server could return
			 * a directory listing in a later step */
		}
		return;
	}

	/* Step 9: Check for zipped files: */
	/* If we can't find the actual file, look for the file
	 * with the same name but a .gz extension. If we find it,
	 * use that and set the gzipped flag in the file struct
	 * to indicate that the response need to have the content-
	 * encoding: gzip header.
	 * We can only do this if the browser declares support. */
	if (conn->accept_gzip) {
		mg_snprintf(
		    conn, &truncated, gz_path, sizeof(gz_path), "%s.gz", filename);

		if (truncated) {
			goto interpret_cleanup;
		}

		if (mg_stat(conn, gz_path, filestat)) {
			if (filestat) {
				filestat->is_gzipped = 1;
				*is_found = 1;
			}
			/* Currently gz files can not be scripts. */
			return;
		}
	}

#if !defined(NO_CGI) || defined(USE_LUA) || defined(USE_DUKTAPE)
	/* Step 10: Script resources may handle sub-resources */
	/* Support PATH_INFO for CGI scripts. */
	tmp_str_len = strlen(filename);
	tmp_str =
	    (char *)mg_malloc_ctx(tmp_str_len + UTF8_PATH_MAX + 1, conn->phys_ctx);
	if (!tmp_str) {
		/* Out of memory */
		goto interpret_cleanup;
	}
	memcpy(tmp_str, filename, tmp_str_len + 1);

	/* Check config, if index scripts may have sub-resources */
	allow_substitute_script_subresources =
	    !mg_strcasecmp(conn->dom_ctx->config[ALLOW_INDEX_SCRIPT_SUB_RES],
	                   "yes");
	if (*is_webdav_request) {
		/* TO BE DEFINED: Should scripts handle special WebDAV methods lile
		 * PROPFIND for their subresources? */
		/* allow_substitute_script_subresources = 0; */
	}

	sep_pos = tmp_str_len;
	while (sep_pos > 0) {
		sep_pos--;
		if (tmp_str[sep_pos] == '/') {
			int is_script = 0, does_exist = 0;

			tmp_str[sep_pos] = 0;
			if (tmp_str[0]) {
				is_script = extention_matches_script(conn, tmp_str);
				does_exist = mg_stat(conn, tmp_str, filestat);
			}

			if (does_exist && is_script) {
				filename[sep_pos] = 0;
				memmove(filename + sep_pos + 2,
				        filename + sep_pos + 1,
				        strlen(filename + sep_pos + 1) + 1);
				conn->path_info = filename + sep_pos + 1;
				filename[sep_pos + 1] = '/';
				*is_script_resource = 1;
				*is_found = 1;
				break;
			}

			if (allow_substitute_script_subresources) {
				if (substitute_index_file(
				        conn, tmp_str, tmp_str_len + UTF8_PATH_MAX, filestat)) {

					/* some intermediate directory has an index file */
					if (extention_matches_script(conn, tmp_str)) {

						size_t script_name_len = strlen(tmp_str);

						/* subres_name read before this memory locatio will be
						overwritten */
						char *subres_name = filename + sep_pos;
						size_t subres_name_len = strlen(subres_name);

						DEBUG_TRACE("Substitute script %s serving path %s",
						            tmp_str,
						            filename);

						/* this index file is a script */
						if ((script_name_len + subres_name_len + 2)
						    >= filename_buf_len) {
							mg_free(tmp_str);
							goto interpret_cleanup;
						}

						conn->path_info =
						    filename + script_name_len + 1; /* new target */
						memmove(conn->path_info, subres_name, subres_name_len);
						conn->path_info[subres_name_len] = 0;
						memcpy(filename, tmp_str, script_name_len + 1);

						*is_script_resource = 1;
						*is_found = 1;
						break;

					} else {

						DEBUG_TRACE("Substitute file %s serving path %s",
						            tmp_str,
						            filename);

						/* non-script files will not have sub-resources */
						filename[sep_pos] = 0;
						conn->path_info = 0;
						*is_script_resource = 0;
						*is_found = 0;
						break;
					}
				}
			}

			tmp_str[sep_pos] = '/';
		}
	}

	mg_free(tmp_str);

#endif /* !defined(NO_CGI) || defined(USE_LUA) || defined(USE_DUKTAPE) */
#endif /* !defined(NO_FILES) */
	return;

#if !defined(NO_FILES)
/* Reset all outputs */
interpret_cleanup:
	memset(filestat, 0, sizeof(*filestat));
	*filename = 0;
	*is_found = 0;
	*is_script_resource = 0;
	*is_websocket_request = 0;
	*is_put_or_delete_request = 0;
#endif /* !defined(NO_FILES) */
}


/* Check whether full request is buffered. Return:
 * -1  if request or response is malformed
 *  0  if request or response is not yet fully buffered
 * >0  actual request length, including last \r\n\r\n */
static int
get_http_header_len(const char *buf, int buflen)
{
	int i;
	for (i = 0; i < buflen; i++) {
		/* Do an unsigned comparison in some conditions below */
		const unsigned char c = (unsigned char)buf[i];

		if ((c < 128) && ((char)c != '\r') && ((char)c != '\n')
		    && !isprint(c)) {
			/* abort scan as soon as one malformed character is found */
			return -1;
		}

		if (i < buflen - 1) {
			if ((buf[i] == '\n') && (buf[i + 1] == '\n')) {
				/* Two newline, no carriage return - not standard compliant,
				 * but it should be accepted */
				return i + 2;
			}
		}

		if (i < buflen - 3) {
			if ((buf[i] == '\r') && (buf[i + 1] == '\n') && (buf[i + 2] == '\r')
			    && (buf[i + 3] == '\n')) {
				/* Two \r\n - standard compliant */
				return i + 4;
			}
		}
	}

	return 0;
}


#if !defined(NO_CACHING)
/* Convert month to the month number. Return -1 on error, or month number */
static int
get_month_index(const char *s)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(month_names); i++) {
		if (!strcmp(s, month_names[i])) {
			return (int)i;
		}
	}

	return -1;
}


/* Parse UTC date-time string, and return the corresponding time_t value. */
static time_t
parse_date_string(const char *datetime)
{
	char month_str[32] = {0};
	int second, minute, hour, day, month, year;
	time_t result = (time_t)0;
	struct tm tm;

	if ((sscanf(datetime,
	            "%d/%3s/%d %d:%d:%d",
	            &day,
	            month_str,
	            &year,
	            &hour,
	            &minute,
	            &second)
	     == 6)
	    || (sscanf(datetime,
	               "%d %3s %d %d:%d:%d",
	               &day,
	               month_str,
	               &year,
	               &hour,
	               &minute,
	               &second)
	        == 6)
	    || (sscanf(datetime,
	               "%*3s, %d %3s %d %d:%d:%d",
	               &day,
	               month_str,
	               &year,
	               &hour,
	               &minute,
	               &second)
	        == 6)
	    || (sscanf(datetime,
	               "%d-%3s-%d %d:%d:%d",
	               &day,
	               month_str,
	               &year,
	               &hour,
	               &minute,
	               &second)
	        == 6)) {
		month = get_month_index(month_str);
		if ((month >= 0) && (year >= 1970)) {
			memset(&tm, 0, sizeof(tm));
			tm.tm_year = year - 1900;
			tm.tm_mon = month;
			tm.tm_mday = day;
			tm.tm_hour = hour;
			tm.tm_min = minute;
			tm.tm_sec = second;
			result = timegm(&tm);
		}
	}

	return result;
}
#endif /* !NO_CACHING */


/* Pre-process URIs according to RFC + protect against directory disclosure
 * attacks by removing '..', excessive '/' and '\' characters */
static void
remove_dot_segments(char *inout)
{
	/* Windows backend protection
	 * (https://tools.ietf.org/html/rfc3986#section-7.3): Replace backslash
	 * in URI by slash */
	char *out_end = inout;
	char *in = inout;

	if (!in) {
		/* Param error. */
		return;
	}

	while (*in) {
		if (*in == '\\') {
			*in = '/';
		}
		in++;
	}

	/* Algorithm "remove_dot_segments" from
	 * https://tools.ietf.org/html/rfc3986#section-5.2.4 */
	/* Step 1:
	 * The input buffer is initialized.
	 * The output buffer is initialized to the empty string.
	 */
	in = inout;

	/* Step 2:
	 * While the input buffer is not empty, loop as follows:
	 */
	/* Less than out_end of the inout buffer is used as output, so keep
	 * condition: out_end <= in */
	while (*in) {
		/* Step 2a:
		 * If the input buffer begins with a prefix of "../" or "./",
		 * then remove that prefix from the input buffer;
		 */
		if (!strncmp(in, "../", 3)) {
			in += 3;
		} else if (!strncmp(in, "./", 2)) {
			in += 2;
		}
		/* otherwise */
		/* Step 2b:
		 * if the input buffer begins with a prefix of "/./" or "/.",
		 * where "." is a complete path segment, then replace that
		 * prefix with "/" in the input buffer;
		 */
		else if (!strncmp(in, "/./", 3)) {
			in += 2;
		} else if (!strcmp(in, "/.")) {
			in[1] = 0;
		}
		/* otherwise */
		/* Step 2c:
		 * if the input buffer begins with a prefix of "/../" or "/..",
		 * where ".." is a complete path segment, then replace that
		 * prefix with "/" in the input buffer and remove the last
		 * segment and its preceding "/" (if any) from the output
		 * buffer;
		 */
		else if (!strncmp(in, "/../", 4)) {
			in += 3;
			if (inout != out_end) {
				/* remove last segment */
				do {
					out_end--;
				} while ((inout != out_end) && (*out_end != '/'));
			}
		} else if (!strcmp(in, "/..")) {
			in[1] = 0;
			if (inout != out_end) {
				/* remove last segment */
				do {
					out_end--;
				} while ((inout != out_end) && (*out_end != '/'));
			}
		}
		/* otherwise */
		/* Step 2d:
		 * if the input buffer consists only of "." or "..", then remove
		 * that from the input buffer;
		 */
		else if (!strcmp(in, ".") || !strcmp(in, "..")) {
			*in = 0;
		}
		/* otherwise */
		/* Step 2e:
		 * move the first path segment in the input buffer to the end of
		 * the output buffer, including the initial "/" character (if
		 * any) and any subsequent characters up to, but not including,
		 * the next "/" character or the end of the input buffer.
		 */
		else {
			do {
				*out_end = *in;
				out_end++;
				in++;
			} while ((*in != 0) && (*in != '/'));
		}
	}

	/* Step 3:
	 * Finally, the output buffer is returned as the result of
	 * remove_dot_segments.
	 */
	/* Terminate output */
	*out_end = 0;

	/* For Windows, the files/folders "x" and "x." (with a dot but without
	 * extension) are identical. Replace all "./" by "/" and remove a "." at
	 * the end. Also replace all "//" by "/". Repeat until there is no "./"
	 * or "//" anymore.
	 */
	out_end = in = inout;
	while (*in) {
		if (*in == '.') {
			/* remove . at the end or preceding of / */
			char *in_ahead = in;
			do {
				in_ahead++;
			} while (*in_ahead == '.');
			if (*in_ahead == '/') {
				in = in_ahead;
				if ((out_end != inout) && (out_end[-1] == '/')) {
					/* remove generated // */
					out_end--;
				}
			} else if (*in_ahead == 0) {
				in = in_ahead;
			} else {
				do {
					*out_end++ = '.';
					in++;
				} while (in != in_ahead);
			}
		} else if (*in == '/') {
			/* replace // by / */
			*out_end++ = '/';
			do {
				in++;
			} while (*in == '/');
		} else {
			*out_end++ = *in;
			in++;
		}
	}
	*out_end = 0;
}


static const struct {
	const char *extension;
	size_t ext_len;
	const char *mime_type;
} builtin_mime_types[] = {
    /* IANA registered MIME types
     * (http://www.iana.org/assignments/media-types)
     * application types */
    {".bin", 4, "application/octet-stream"},
    {".deb", 4, "application/octet-stream"},
    {".dmg", 4, "application/octet-stream"},
    {".dll", 4, "application/octet-stream"},
    {".doc", 4, "application/msword"},
    {".eps", 4, "application/postscript"},
    {".exe", 4, "application/octet-stream"},
    {".iso", 4, "application/octet-stream"},
    {".js", 3, "application/javascript"},
    {".json", 5, "application/json"},
    {".mjs", 4, "application/javascript"},
    {".msi", 4, "application/octet-stream"},
    {".pdf", 4, "application/pdf"},
    {".ps", 3, "application/postscript"},
    {".rtf", 4, "application/rtf"},
    {".wasm", 5, "application/wasm"},
    {".xhtml", 6, "application/xhtml+xml"},
    {".xsl", 4, "application/xml"},
    {".xslt", 5, "application/xml"},

    /* fonts */
    {".ttf", 4, "application/font-sfnt"},
    {".cff", 4, "application/font-sfnt"},
    {".otf", 4, "application/font-sfnt"},
    {".aat", 4, "application/font-sfnt"},
    {".sil", 4, "application/font-sfnt"},
    {".pfr", 4, "application/font-tdpfr"},
    {".woff", 5, "application/font-woff"},
    {".woff2", 6, "application/font-woff2"},

    /* audio */
    {".mp3", 4, "audio/mpeg"},
    {".oga", 4, "audio/ogg"},
    {".ogg", 4, "audio/ogg"},

    /* image */
    {".gif", 4, "image/gif"},
    {".ief", 4, "image/ief"},
    {".jpeg", 5, "image/jpeg"},
    {".jpg", 4, "image/jpeg"},
    {".jpm", 4, "image/jpm"},
    {".jpx", 4, "image/jpx"},
    {".png", 4, "image/png"},
    {".svg", 4, "image/svg+xml"},
    {".tif", 4, "image/tiff"},
    {".tiff", 5, "image/tiff"},

    /* model */
    {".wrl", 4, "model/vrml"},

    /* text */
    {".css", 4, "text/css"},
    {".csv", 4, "text/csv"},
    {".htm", 4, "text/html"},
    {".html", 5, "text/html"},
    {".sgm", 4, "text/sgml"},
    {".shtm", 5, "text/html"},
    {".shtml", 6, "text/html"},
    {".txt", 4, "text/plain"},
    {".xml", 4, "text/xml"},

    /* video */
    {".mov", 4, "video/quicktime"},
    {".mp4", 4, "video/mp4"},
    {".mpeg", 5, "video/mpeg"},
    {".mpg", 4, "video/mpeg"},
    {".ogv", 4, "video/ogg"},
    {".qt", 3, "video/quicktime"},

    /* not registered types
     * (http://reference.sitepoint.com/html/mime-types-full,
     * http://www.hansenb.pdx.edu/DMKB/dict/tutorials/mime_typ.php, ..) */
    {".arj", 4, "application/x-arj-compressed"},
    {".gz", 3, "application/x-gunzip"},
    {".rar", 4, "application/x-arj-compressed"},
    {".swf", 4, "application/x-shockwave-flash"},
    {".tar", 4, "application/x-tar"},
    {".tgz", 4, "application/x-tar-gz"},
    {".torrent", 8, "application/x-bittorrent"},
    {".ppt", 4, "application/x-mspowerpoint"},
    {".xls", 4, "application/x-msexcel"},
    {".zip", 4, "application/x-zip-compressed"},
    {".aac",
     4,
     "audio/aac"}, /* http://en.wikipedia.org/wiki/Advanced_Audio_Coding */
    {".flac", 5, "audio/flac"},
    {".aif", 4, "audio/x-aif"},
    {".m3u", 4, "audio/x-mpegurl"},
    {".mid", 4, "audio/x-midi"},
    {".ra", 3, "audio/x-pn-realaudio"},
    {".ram", 4, "audio/x-pn-realaudio"},
    {".wav", 4, "audio/x-wav"},
    {".bmp", 4, "image/bmp"},
    {".ico", 4, "image/x-icon"},
    {".pct", 4, "image/x-pct"},
    {".pict", 5, "image/pict"},
    {".rgb", 4, "image/x-rgb"},
    {".webm", 5, "video/webm"}, /* http://en.wikipedia.org/wiki/WebM */
    {".asf", 4, "video/x-ms-asf"},
    {".avi", 4, "video/x-msvideo"},
    {".m4v", 4, "video/x-m4v"},
    {NULL, 0, NULL}};


CIVETWEB_API const char *
mg_get_builtin_mime_type(const char *path)
{
	const char *ext;
	size_t i, path_len;

	path_len = strlen(path);

	for (i = 0; builtin_mime_types[i].extension != NULL; i++) {
		ext = path + (path_len - builtin_mime_types[i].ext_len);
		if ((path_len > builtin_mime_types[i].ext_len)
		    && (mg_strcasecmp(ext, builtin_mime_types[i].extension) == 0)) {
			return builtin_mime_types[i].mime_type;
		}
	}

	return "text/plain";
}


/* Look at the "path" extension and figure what mime type it has.
 * Store mime type in the vector. */
static void
get_mime_type(struct mg_connection *conn, const char *path, struct vec *vec)
{
	struct vec ext_vec, mime_vec;
	const char *list, *ext;
	size_t path_len;

	path_len = strlen(path);

	if ((conn == NULL) || (vec == NULL)) {
		if (vec != NULL) {
			memset(vec, '\0', sizeof(struct vec));
		}
		return;
	}

	/* Scan user-defined mime types first, in case user wants to
	 * override default mime types. */
	list = conn->dom_ctx->config[EXTRA_MIME_TYPES];
	while ((list = next_option(list, &ext_vec, &mime_vec)) != NULL) {
		/* ext now points to the path suffix */
		ext = path + path_len - ext_vec.len;
		if (mg_strncasecmp(ext, ext_vec.ptr, ext_vec.len) == 0) {
			*vec = mime_vec;
			return;
		}
	}

	vec->ptr = mg_get_builtin_mime_type(path);
	vec->len = strlen(vec->ptr);
}


/* Stringify binary data. Output buffer must be twice as big as input,
 * because each byte takes 2 bytes in string representation */
static void
bin2str(char *to, const unsigned char *p, size_t len)
{
	static const char *hex = "0123456789abcdef";

	for (; len--; p++) {
		*to++ = hex[p[0] >> 4];
		*to++ = hex[p[0] & 0x0f];
	}
	*to = '\0';
}


/* Return stringified MD5 hash for list of strings. Buffer must be 33 bytes.
 */
CIVETWEB_API char *
mg_md5(char buf[33], ...)
{
	md5_byte_t hash[16];
	const char *p;
	va_list ap;
	md5_state_t ctx;

	md5_init(&ctx);

	va_start(ap, buf);
	while ((p = va_arg(ap, const char *)) != NULL) {
		md5_append(&ctx, (const md5_byte_t *)p, strlen(p));
	}
	va_end(ap);

	md5_finish(&ctx, hash);
	bin2str(buf, hash, sizeof(hash));
	return buf;
}


/* Check the user's password, return 1 if OK */
static int
check_password_digest(const char *method,
                      const char *ha1,
                      const char *uri,
                      const char *nonce,
                      const char *nc,
                      const char *cnonce,
                      const char *qop,
                      const char *response)
{
	char ha2[32 + 1], expected_response[32 + 1];

	/* Some of the parameters may be NULL */
	if ((method == NULL) || (nonce == NULL) || (nc == NULL) || (cnonce == NULL)
	    || (qop == NULL) || (response == NULL)) {
		return 0;
	}

	/* NOTE(lsm): due to a bug in MSIE, we do not compare the URI */
	if (strlen(response) != 32) {
		return 0;
	}

	mg_md5(ha2, method, ":", uri, NULL);
	mg_md5(expected_response,
	       ha1,
	       ":",
	       nonce,
	       ":",
	       nc,
	       ":",
	       cnonce,
	       ":",
	       qop,
	       ":",
	       ha2,
	       NULL);

	return mg_strcasecmp(response, expected_response) == 0;
}


#if !defined(NO_FILESYSTEMS)
/* Use the global passwords file, if specified by auth_gpass option,
 * or search for .htpasswd in the requested directory. */
static void
open_auth_file(struct mg_connection *conn,
               const char *path,
               struct mg_file *filep)
{
	if ((conn != NULL) && (conn->dom_ctx != NULL)) {
		char name[UTF8_PATH_MAX];
		const char *p, *e,
		    *gpass = conn->dom_ctx->config[GLOBAL_PASSWORDS_FILE];
		int truncated;

		if (gpass != NULL) {
			/* Use global passwords file */
			if (!mg_fopen(conn, gpass, MG_FOPEN_MODE_READ, filep)) {
#if defined(DEBUG)
				/* Use mg_cry_internal here, since gpass has been
				 * configured. */
				mg_cry_internal(conn, "fopen(%s): %s", gpass, strerror(ERRNO));
#endif
			}
			/* Important: using local struct mg_file to test path for
			 * is_directory flag. If filep is used, mg_stat() makes it
			 * appear as if auth file was opened.
			 * TODO(mid): Check if this is still required after rewriting
			 * mg_stat */
		} else if (mg_stat(conn, path, &filep->stat)
		           && filep->stat.is_directory) {
			mg_snprintf(conn,
			            &truncated,
			            name,
			            sizeof(name),
			            "%s/%s",
			            path,
			            PASSWORDS_FILE_NAME);

			if (truncated || !mg_fopen(conn, name, MG_FOPEN_MODE_READ, filep)) {
#if defined(DEBUG)
				/* Don't use mg_cry_internal here, but only a trace, since
				 * this is a typical case. It will occur for every directory
				 * without a password file. */
				DEBUG_TRACE("fopen(%s): %s", name, strerror(ERRNO));
#endif
			}
		} else {
			/* Try to find .htpasswd in requested directory. */
			for (p = path, e = p + strlen(p) - 1; e > p; e--) {
				if (e[0] == '/') {
					break;
				}
			}
			mg_snprintf(conn,
			            &truncated,
			            name,
			            sizeof(name),
			            "%.*s/%s",
			            (int)(e - p),
			            p,
			            PASSWORDS_FILE_NAME);

			if (truncated || !mg_fopen(conn, name, MG_FOPEN_MODE_READ, filep)) {
#if defined(DEBUG)
				/* Don't use mg_cry_internal here, but only a trace, since
				 * this is a typical case. It will occur for every directory
				 * without a password file. */
				DEBUG_TRACE("fopen(%s): %s", name, strerror(ERRNO));
#endif
			}
		}
	}
}
#endif /* NO_FILESYSTEMS */


/* Parsed Authorization header */
struct auth_header {
	char *user;
	int type;             /* 1 = basic, 2 = digest */
	char *plain_password; /* Basic only */
	char *uri, *cnonce, *response, *qop, *nc, *nonce; /* Digest only */
};


/* Return 1 on success. Always initializes the auth_header structure. */
static int
parse_auth_header(struct mg_connection *conn,
                  char *buf,
                  size_t buf_size,
                  struct auth_header *auth_header)
{
	char *name, *value, *s;
	const char *ah;
	uint64_t nonce;

	if (!auth_header || !conn) {
		return 0;
	}

	(void)memset(auth_header, 0, sizeof(*auth_header));
	ah = mg_get_header(conn, "Authorization");

	if (ah == NULL) {
		/* No Authorization header at all */
		return 0;
	}
	if (0 == mg_strncasecmp(ah, "Basic ", 6)) {
		/* Basic Auth (we never asked for this, but some client may send it) */
		char *split;
		const char *userpw_b64 = ah + 6;
		size_t userpw_b64_len = strlen(userpw_b64);
		size_t buf_len_r = buf_size;
		if (mg_base64_decode(
		        userpw_b64, userpw_b64_len, (unsigned char *)buf, &buf_len_r)
		    != -1) {
			return 0; /* decode error */
		}
		split = strchr(buf, ':');
		if (!split) {
			return 0; /* Format error */
		}

		/* Separate string at ':' */
		*split = 0;

		/* User name is before ':', Password is after ':'  */
		auth_header->user = buf;
		auth_header->type = 1;
		auth_header->plain_password = split + 1;

		return 1;

	} else if (0 == mg_strncasecmp(ah, "Digest ", 7)) {
		/* Digest Auth ... implemented below */
		auth_header->type = 2;

	} else {
		/* Unknown or invalid Auth method */
		return 0;
	}

	/* Make modifiable copy of the auth header */
	(void)mg_strlcpy(buf, ah + 7, buf_size);
	s = buf;

	/* Parse authorization header */
	for (;;) {
		/* Gobble initial spaces */
		while (isspace((unsigned char)*s)) {
			s++;
		}
		name = skip_quoted(&s, "=", " ", 0);
		/* Value is either quote-delimited, or ends at first comma or space.
		 */
		if (s[0] == '\"') {
			s++;
			value = skip_quoted(&s, "\"", " ", '\\');
			if (s[0] == ',') {
				s++;
			}
		} else {
			value = skip_quoted(&s, ", ", " ", 0); /* IE uses commas, FF
			                                        * uses spaces */
		}
		if (*name == '\0') {
			break;
		}

		if (!strcmp(name, "username")) {
			auth_header->user = value;
		} else if (!strcmp(name, "cnonce")) {
			auth_header->cnonce = value;
		} else if (!strcmp(name, "response")) {
			auth_header->response = value;
		} else if (!strcmp(name, "uri")) {
			auth_header->uri = value;
		} else if (!strcmp(name, "qop")) {
			auth_header->qop = value;
		} else if (!strcmp(name, "nc")) {
			auth_header->nc = value;
		} else if (!strcmp(name, "nonce")) {
			auth_header->nonce = value;
		}
	}

#if !defined(NO_NONCE_CHECK)
	/* Read the nonce from the response. */
	if (auth_header->nonce == NULL) {
		return 0;
	}
	s = NULL;
	nonce = strtoull(auth_header->nonce, &s, 10);
	if ((s == NULL) || (*s != 0)) {
		return 0;
	}

	/* Convert the nonce from the client to a number. */
	nonce ^= conn->dom_ctx->auth_nonce_mask;

	/* The converted number corresponds to the time the nounce has been
	 * created. This should not be earlier than the server start. */
	/* Server side nonce check is valuable in all situations but one:
	 * if the server restarts frequently, but the client should not see
	 * that, so the server should accept nonces from previous starts. */
	/* However, the reasonable default is to not accept a nonce from a
	 * previous start, so if anyone changed the access rights between
	 * two restarts, a new login is required. */
	if (nonce < (uint64_t)conn->phys_ctx->start_time) {
		/* nonce is from a previous start of the server and no longer valid
		 * (replay attack?) */
		return 0;
	}
	/* Check if the nonce is too high, so it has not (yet) been used by the
	 * server. */
	if (nonce >= ((uint64_t)conn->phys_ctx->start_time
	              + conn->dom_ctx->nonce_count)) {
		return 0;
	}
#else
	(void)nonce;
#endif

	return (auth_header->user != NULL);
}


static const char *
mg_fgets(char *buf, size_t size, struct mg_file *filep)
{
	if (!filep) {
		return NULL;
	}

	if (filep->access.fp != NULL) {
		return fgets(buf, (int)size, filep->access.fp);
	} else {
		return NULL;
	}
}

/* Define the initial recursion depth for procesesing htpasswd files that
 * include other htpasswd
 * (or even the same) files.  It is not difficult to provide a file or files
 * s.t. they force civetweb
 * to infinitely recurse and then crash.
 */
#define INITIAL_DEPTH 9
#if INITIAL_DEPTH <= 0
#error Bad INITIAL_DEPTH for recursion, set to at least 1
#endif

#if !defined(NO_FILESYSTEMS)
struct read_auth_file_struct {
	struct mg_connection *conn;
	struct auth_header auth_header;
	const char *domain;
	char buf[256 + 256 + 40];
	const char *f_user;
	const char *f_domain;
	const char *f_ha1;
};


static int
read_auth_file(struct mg_file *filep,
               struct read_auth_file_struct *workdata,
               int depth)
{
	int is_authorized = 0;
	struct mg_file fp;
	size_t l;

	if (!filep || !workdata || (0 == depth)) {
		return 0;
	}

	/* Loop over passwords file */
	while (mg_fgets(workdata->buf, sizeof(workdata->buf), filep) != NULL) {
		l = strlen(workdata->buf);
		while (l > 0) {
			if (isspace((unsigned char)workdata->buf[l - 1])
			    || iscntrl((unsigned char)workdata->buf[l - 1])) {
				l--;
				workdata->buf[l] = 0;
			} else
				break;
		}
		if (l < 1) {
			continue;
		}

		workdata->f_user = workdata->buf;

		if (workdata->f_user[0] == ':') {
			/* user names may not contain a ':' and may not be empty,
			 * so lines starting with ':' may be used for a special purpose
			 */
			if (workdata->f_user[1] == '#') {
				/* :# is a comment */
				continue;
			} else if (!strncmp(workdata->f_user + 1, "include=", 8)) {
				if (mg_fopen(workdata->conn,
				             workdata->f_user + 9,
				             MG_FOPEN_MODE_READ,
				             &fp)) {
					is_authorized = read_auth_file(&fp, workdata, depth - 1);
					(void)mg_fclose(
					    &fp.access); /* ignore error on read only file */

					/* No need to continue processing files once we have a
					 * match, since nothing will reset it back
					 * to 0.
					 */
					if (is_authorized) {
						return is_authorized;
					}
				} else {
					mg_cry_internal(workdata->conn,
					                "%s: cannot open authorization file: %s",
					                __func__,
					                workdata->buf);
				}
				continue;
			}
			/* everything is invalid for the moment (might change in the
			 * future) */
			mg_cry_internal(workdata->conn,
			                "%s: syntax error in authorization file: %s",
			                __func__,
			                workdata->buf);
			continue;
		}

		workdata->f_domain = strchr(workdata->f_user, ':');
		if (workdata->f_domain == NULL) {
			mg_cry_internal(workdata->conn,
			                "%s: syntax error in authorization file: %s",
			                __func__,
			                workdata->buf);
			continue;
		}
		*(char *)(workdata->f_domain) = 0;
		(workdata->f_domain)++;

		workdata->f_ha1 = strchr(workdata->f_domain, ':');
		if (workdata->f_ha1 == NULL) {
			mg_cry_internal(workdata->conn,
			                "%s: syntax error in authorization file: %s",
			                __func__,
			                workdata->buf);
			continue;
		}
		*(char *)(workdata->f_ha1) = 0;
		(workdata->f_ha1)++;

		if (!strcmp(workdata->auth_header.user, workdata->f_user)
		    && !strcmp(workdata->domain, workdata->f_domain)) {
			switch (workdata->auth_header.type) {
			case 1: /* Basic */
			{
				char md5[33];
				mg_md5(md5,
				       workdata->f_user,
				       ":",
				       workdata->domain,
				       ":",
				       workdata->auth_header.plain_password,
				       NULL);
				return 0 == memcmp(workdata->f_ha1, md5, 33);
			}
			case 2: /* Digest */
				return check_password_digest(
				    workdata->conn->request_info.request_method,
				    workdata->f_ha1,
				    workdata->auth_header.uri,
				    workdata->auth_header.nonce,
				    workdata->auth_header.nc,
				    workdata->auth_header.cnonce,
				    workdata->auth_header.qop,
				    workdata->auth_header.response);
			default: /* None/Other/Unknown */
				return 0;
			}
		}
	}

	return is_authorized;
}


/* Authorize against the opened passwords file. Return 1 if authorized. */
static int
authorize(struct mg_connection *conn, struct mg_file *filep, const char *realm)
{
	struct read_auth_file_struct workdata;
	char buf[MG_BUF_LEN];

	if (!conn || !conn->dom_ctx) {
		return 0;
	}

	memset(&workdata, 0, sizeof(workdata));
	workdata.conn = conn;

	if (!parse_auth_header(conn, buf, sizeof(buf), &workdata.auth_header)) {
		return 0;
	}

	/* CGI needs it as REMOTE_USER */
	conn->request_info.remote_user =
	    mg_strdup_ctx(workdata.auth_header.user, conn->phys_ctx);

	if (realm) {
		workdata.domain = realm;
	} else {
		workdata.domain = conn->dom_ctx->config[AUTHENTICATION_DOMAIN];
	}

	return read_auth_file(filep, &workdata, INITIAL_DEPTH);
}


/* Public function to check http digest authentication header */
CIVETWEB_API int
mg_check_digest_access_authentication(struct mg_connection *conn,
                                      const char *realm,
                                      const char *filename)
{
	struct mg_file file = STRUCT_FILE_INITIALIZER;
	int auth;

	if (!conn || !filename) {
		return -1;
	}
	if (!mg_fopen(conn, filename, MG_FOPEN_MODE_READ, &file)) {
		return -2;
	}

	auth = authorize(conn, &file, realm);

	mg_fclose(&file.access);

	return auth;
}
#endif /* NO_FILESYSTEMS */


/* Return 1 if request is authorised, 0 otherwise. */
static int
check_authorization(struct mg_connection *conn, const char *path)
{
#if !defined(NO_FILESYSTEMS)
	char fname[UTF8_PATH_MAX];
	struct vec uri_vec, filename_vec;
	const char *list;
	struct mg_file file = STRUCT_FILE_INITIALIZER;
	int authorized = 1, truncated;

	if (!conn || !conn->dom_ctx) {
		return 0;
	}

	list = conn->dom_ctx->config[PROTECT_URI];
	while ((list = next_option(list, &uri_vec, &filename_vec)) != NULL) {
		if (!memcmp(conn->request_info.local_uri, uri_vec.ptr, uri_vec.len)) {
			mg_snprintf(conn,
			            &truncated,
			            fname,
			            sizeof(fname),
			            "%.*s",
			            (int)filename_vec.len,
			            filename_vec.ptr);

			if (truncated
			    || !mg_fopen(conn, fname, MG_FOPEN_MODE_READ, &file)) {
				mg_cry_internal(conn,
				                "%s: cannot open %s: %s",
				                __func__,
				                fname,
				                strerror(errno));
			}
			break;
		}
	}

	if (!is_file_opened(&file.access)) {
		open_auth_file(conn, path, &file);
	}

	if (is_file_opened(&file.access)) {
		authorized = authorize(conn, &file, NULL);
		(void)mg_fclose(&file.access); /* ignore error on read only file */
	}

	return authorized;
#else
	(void)conn;
	(void)path;
	return 1;
#endif /* NO_FILESYSTEMS */
}


/* Internal function. Assumes conn is valid */
static void
send_authorization_request(struct mg_connection *conn, const char *realm)
{
	uint64_t nonce = (uint64_t)(conn->phys_ctx->start_time);
	int trunc = 0;
	char buf[128];

	if (!realm) {
		realm = conn->dom_ctx->config[AUTHENTICATION_DOMAIN];
	}

	mg_lock_context(conn->phys_ctx);
	nonce += conn->dom_ctx->nonce_count;
	++conn->dom_ctx->nonce_count;
	mg_unlock_context(conn->phys_ctx);

	nonce ^= conn->dom_ctx->auth_nonce_mask;
	conn->must_close = 1;

	/* Create 401 response */
	mg_response_header_start(conn, 401);
	send_no_cache_header(conn);
	send_additional_header(conn);
	mg_response_header_add(conn, "Content-Length", "0", -1);

	/* Content for "WWW-Authenticate" header */
	mg_snprintf(conn,
	            &trunc,
	            buf,
	            sizeof(buf),
	            "Digest qop=\"auth\", realm=\"%s\", "
	            "nonce=\"%" UINT64_FMT "\"",
	            realm,
	            nonce);

	if (!trunc) {
		/* !trunc should always be true */
		mg_response_header_add(conn, "WWW-Authenticate", buf, -1);
	}

	/* Send all headers */
	mg_response_header_send(conn);
}


/* Interface function. Parameters are provided by the user, so do
 * at least some basic checks.
 */
CIVETWEB_API int
mg_send_digest_access_authentication_request(struct mg_connection *conn,
                                             const char *realm)
{
	if (conn && conn->dom_ctx) {
		send_authorization_request(conn, realm);
		return 0;
	}
	return -1;
}


#if !defined(NO_FILES)
static int
is_authorized_for_put(struct mg_connection *conn)
{
	int ret = 0;

	if (conn) {
		struct mg_file file = STRUCT_FILE_INITIALIZER;
		const char *passfile = conn->dom_ctx->config[PUT_DELETE_PASSWORDS_FILE];

		if (passfile != NULL
		    && mg_fopen(conn, passfile, MG_FOPEN_MODE_READ, &file)) {
			ret = authorize(conn, &file, NULL);
			(void)mg_fclose(&file.access); /* ignore error on read only file */
		}
	}

	DEBUG_TRACE("file write authorization: %i", ret);
	return ret;
}
#endif


CIVETWEB_API int
mg_modify_passwords_file_ha1(const char *fname,
                             const char *domain,
                             const char *user,
                             const char *ha1)
{
	int found = 0, i, result = 1;
	char line[512], u[256], d[256], h[256];
	struct stat st = {0};
	FILE *fp = NULL;
	char *temp_file = NULL;
	int temp_file_offs = 0;

	/* Regard empty password as no password - remove user record. */
	if ((ha1 != NULL) && (ha1[0] == '\0')) {
		ha1 = NULL;
	}

	/* Other arguments must not be empty */
	if ((fname == NULL) || (domain == NULL) || (user == NULL)) {
		return 0;
	}

	/* Using the given file format, user name and domain must not contain
	 * the ':' character */
	if (strchr(user, ':') != NULL) {
		return 0;
	}
	if (strchr(domain, ':') != NULL) {
		return 0;
	}

	/* Do not allow control characters like newline in user name and domain.
	 * Do not allow excessively long names either. */
	for (i = 0; ((i < 255) && (user[i] != 0)); i++) {
		if (iscntrl((unsigned char)user[i])) {
			return 0;
		}
	}
	if (user[i]) {
		return 0; /* user name too long */
	}
	for (i = 0; ((i < 255) && (domain[i] != 0)); i++) {
		if (iscntrl((unsigned char)domain[i])) {
			return 0;
		}
	}
	if (domain[i]) {
		return 0; /* domain name too long */
	}

	/* The maximum length of the path to the password file is limited */
	if (strlen(fname) >= UTF8_PATH_MAX) {
		return 0;
	}

	/* Check if the file exists, and get file size */
	if (0 == stat(fname, &st)) {
		int temp_buf_len;
		if (st.st_size > 10485760) {
			/* Some funster provided a >10 MB text file */
			return 0;
		}

		/* Add enough space for one more line */
		temp_buf_len = (int)st.st_size + 1024;

		/* Allocate memory (instead of using a temporary file) */
		temp_file = (char *)mg_calloc((size_t)temp_buf_len, 1);
		if (!temp_file) {
			/* Out of memory */
			return 0;
		}

		/* File exists. Read it into a memory buffer. */
		fp = fopen(fname, "r");
		if (fp == NULL) {
			/* Cannot read file. No permission? */
			mg_free(temp_file);
			return 0;
		}

		/* Read content and store in memory */
		while ((fgets(line, sizeof(line), fp) != NULL)
		       && ((temp_file_offs + 600) < temp_buf_len)) {
			/* file format is "user:domain:hash\n" */
			if (sscanf(line, "%255[^:]:%255[^:]:%255s", u, d, h) != 3) {
				continue;
			}
			u[255] = 0;
			d[255] = 0;
			h[255] = 0;

			if (!strcmp(u, user) && !strcmp(d, domain)) {
				/* Found the user: change the password hash or drop the user
				 */
				if ((ha1 != NULL) && (!found)) {
					i = sprintf(temp_file + temp_file_offs,
					            "%s:%s:%s\n",
					            user,
					            domain,
					            ha1);
					if (i < 1) {
						fclose(fp);
						mg_free(temp_file);
						return 0;
					}
					temp_file_offs += i;
				}
				found = 1;
			} else {
				/* Copy existing user, including password hash */
				i = sprintf(temp_file + temp_file_offs, "%s:%s:%s\n", u, d, h);
				if (i < 1) {
					fclose(fp);
					mg_free(temp_file);
					return 0;
				}
				temp_file_offs += i;
			}
		}
		fclose(fp);
	}

	/* Create new file */
	fp = fopen(fname, "w");
	if (!fp) {
		mg_free(temp_file);
		return 0;
	}

#if !defined(_WIN32)
	/* On Linux & co., restrict file read/write permissions to the owner */
	if (fchmod(fileno(fp), S_IRUSR | S_IWUSR) != 0) {
		result = 0;
	}
#endif

	if ((temp_file != NULL) && (temp_file_offs > 0)) {
		/* Store buffered content of old file */
		if (fwrite(temp_file, 1, (size_t)temp_file_offs, fp)
		    != (size_t)temp_file_offs) {
			result = 0;
		}
	}

	/* If new user, just add it */
	if ((ha1 != NULL) && (!found)) {
		if (fprintf(fp, "%s:%s:%s\n", user, domain, ha1) < 6) {
			result = 0;
		}
	}

	/* All data written */
	if (fclose(fp) != 0) {
		result = 0;
	}

	mg_free(temp_file);
	return result;
}


CIVETWEB_API int
mg_modify_passwords_file(const char *fname,
                         const char *domain,
                         const char *user,
                         const char *pass)
{
	char ha1buf[33];
	if ((fname == NULL) || (domain == NULL) || (user == NULL)) {
		return 0;
	}
	if ((pass == NULL) || (pass[0] == 0)) {
		return mg_modify_passwords_file_ha1(fname, domain, user, NULL);
	}

	mg_md5(ha1buf, user, ":", domain, ":", pass, NULL);
	return mg_modify_passwords_file_ha1(fname, domain, user, ha1buf);
}


static int
is_valid_port(unsigned long port)
{
	return (port <= 0xffff);
}


static int
mg_inet_pton(int af, const char *src, void *dst, size_t dstlen, int resolve_src)
{
	struct addrinfo hints, *res, *ressave;
	int func_ret = 0;
	int gai_ret;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = af;
	if (!resolve_src) {
		hints.ai_flags = AI_NUMERICHOST;
	}

	gai_ret = getaddrinfo(src, NULL, &hints, &res);
	if (gai_ret != 0) {
		/* gai_strerror could be used to convert gai_ret to a string */
		/* POSIX return values: see
		 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/freeaddrinfo.html
		 */
		/* Windows return values: see
		 * https://msdn.microsoft.com/en-us/library/windows/desktop/ms738520%28v=vs.85%29.aspx
		 */
		return 0;
	}

	ressave = res;

	while (res) {
		if ((dstlen >= (size_t)res->ai_addrlen)
		    && (res->ai_addr->sa_family == af)) {
			memcpy(dst, res->ai_addr, res->ai_addrlen);
			func_ret = 1;
		}
		res = res->ai_next;
	}

	freeaddrinfo(ressave);
	return func_ret;
}


static int
connect_socket(
    struct mg_context *ctx /* may be NULL */,
    const char *host,
    int port,    /* 1..65535, or -99 for domain sockets (may be changed) */
    int use_ssl, /* 0 or 1 */
    struct mg_error_data *error,
    SOCKET *sock /* output: socket, must not be NULL */,
    union usa *sa /* output: socket address, must not be NULL  */
)
{
	int ip_ver = 0;
	int conn_ret = -1;
	int sockerr = 0;
	*sock = INVALID_SOCKET;
	memset(sa, 0, sizeof(*sa));

	if (host == NULL) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_PARAM;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "NULL host");
		}
		return 0;
	}

#if defined(USE_X_DOM_SOCKET)
	if (port == -99) {
		/* Unix domain socket */
		size_t hostlen = strlen(host);
		if (hostlen >= sizeof(sa->sun.sun_path)) {
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_INVALID_PARAM;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for ebuf */
				            error->text,
				            error->text_buffer_size,
				            "%s",
				            "host length exceeds limit");
			}
			return 0;
		}
	} else
#endif
	    if ((port <= 0) || !is_valid_port((unsigned)port)) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_PARAM;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "invalid port");
		}
		return 0;
	}

#if !defined(NO_SSL) && !defined(USE_MBEDTLS) && !defined(USE_GNUTLS) && !defined(NO_SSL_DL)
#if defined(OPENSSL_API_1_1) || defined(OPENSSL_API_3_0)
	if (use_ssl && (TLS_client_method == NULL)) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_LIBRARY_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "SSL is not initialized");
		}
		return 0;
	}
#else
	if (use_ssl && (SSLv23_client_method == NULL)) {
		if (error != 0) {
			error->code = MG_ERROR_DATA_CODE_INIT_LIBRARY_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "SSL is not initialized");
		}
		return 0;
	}
#endif /* OPENSSL_API_1_1 || OPENSSL_API_3_0*/
#else
	(void)use_ssl;
#endif /* NO SSL */

#if defined(USE_X_DOM_SOCKET)
	if (port == -99) {
		size_t hostlen = strlen(host);
		/* check (hostlen < sizeof(sun.sun_path)) already passed above */
		ip_ver = -99;
		sa->sun.sun_family = AF_UNIX;
		memset(sa->sun.sun_path, 0, sizeof(sa->sun.sun_path));
		memcpy(sa->sun.sun_path, host, hostlen);
	} else
#endif
	    if (mg_inet_pton(AF_INET, host, &sa->sin, sizeof(sa->sin), 1)) {
		sa->sin.sin_port = htons((uint16_t)port);
		ip_ver = 4;
#if defined(USE_IPV6)
	} else if (mg_inet_pton(AF_INET6, host, &sa->sin6, sizeof(sa->sin6), 1)) {
		sa->sin6.sin6_port = htons((uint16_t)port);
		ip_ver = 6;
	} else if (host[0] == '[') {
		/* While getaddrinfo on Windows will work with [::1],
		 * getaddrinfo on Linux only works with ::1 (without []). */
		size_t l = strlen(host + 1);
		char *h = (l > 1) ? mg_strdup_ctx(host + 1, ctx) : NULL;
		if (h) {
			h[l - 1] = 0;
			if (mg_inet_pton(AF_INET6, h, &sa->sin6, sizeof(sa->sin6), 0)) {
				sa->sin6.sin6_port = htons((uint16_t)port);
				ip_ver = 6;
			}
			mg_free(h);
		}
#endif
	}

	if (ip_ver == 0) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_HOST_NOT_FOUND;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "host not found");
		}
		return 0;
	}

	if (ip_ver == 4) {
		*sock = socket(PF_INET, SOCK_STREAM, 0);
	}
#if defined(USE_IPV6)
	else if (ip_ver == 6) {
		*sock = socket(PF_INET6, SOCK_STREAM, 0);
	}
#endif
#if defined(USE_X_DOM_SOCKET)
	else if (ip_ver == -99) {
		*sock = socket(AF_UNIX, SOCK_STREAM, 0);
	}
#endif

	if (*sock == INVALID_SOCKET) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OS_ERROR;
			error->code_sub = (unsigned)ERRNO;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "socket(): %s",
			            strerror(ERRNO));
		}
		return 0;
	}

	if (0 != set_non_blocking_mode(*sock)) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OS_ERROR;
			error->code_sub = (unsigned)ERRNO;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "Cannot set socket to non-blocking: %s",
			            strerror(ERRNO));
		}
		closesocket(*sock);
		*sock = INVALID_SOCKET;
		return 0;
	}

	set_close_on_exec(*sock, NULL, ctx);

	if (ip_ver == 4) {
		/* connected with IPv4 */
		conn_ret = connect(*sock,
		                   (struct sockaddr *)((void *)&sa->sin),
		                   sizeof(sa->sin));
	}
#if defined(USE_IPV6)
	else if (ip_ver == 6) {
		/* connected with IPv6 */
		conn_ret = connect(*sock,
		                   (struct sockaddr *)((void *)&sa->sin6),
		                   sizeof(sa->sin6));
	}
#endif
#if defined(USE_X_DOM_SOCKET)
	else if (ip_ver == -99) {
		/* connected to domain socket */
		conn_ret = connect(*sock,
		                   (struct sockaddr *)((void *)&sa->sun),
		                   sizeof(sa->sun));
	}
#endif

	if (conn_ret != 0) {
		sockerr = ERRNO;
	}

#if defined(_WIN32)
	if ((conn_ret != 0) && (sockerr == WSAEWOULDBLOCK)) {
#else
	if ((conn_ret != 0) && (sockerr == EINPROGRESS)) {
#endif
		/* Data for getsockopt */
		void *psockerr = &sockerr;
		int ret;

#if defined(_WIN32)
		int len = (int)sizeof(sockerr);
#else
		socklen_t len = (socklen_t)sizeof(sockerr);
#endif

		/* Data for poll */
		struct mg_pollfd pfd[2];
		int pollres;
		int ms_wait = 10000;       /* 10 second timeout */
		stop_flag_t nonstop = 0;   /* STOP_FLAG_ASSIGN(&nonstop, 0); */
		unsigned int num_sock = 1; /* use one or two sockets */

		/* For a non-blocking socket, the connect sequence is:
		 * 1) call connect (will not block)
		 * 2) wait until the socket is ready for writing (select or poll)
		 * 3) check connection state with getsockopt
		 */
		pfd[0].fd = *sock;
		pfd[0].events = POLLOUT;

		if (ctx && (ctx->context_type == CONTEXT_SERVER)) {
			pfd[num_sock].fd = ctx->thread_shutdown_notification_socket;
			pfd[num_sock].events = POLLIN;
			num_sock++;
		}

		pollres =
		    mg_poll(pfd, num_sock, ms_wait, ctx ? &(ctx->stop_flag) : &nonstop);

		if (pollres != 1) {
			/* Not connected */
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_CONNECT_TIMEOUT;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for ebuf */
				            error->text,
				            error->text_buffer_size,
				            "connect(%s:%d): timeout",
				            host,
				            port);
			}
			closesocket(*sock);
			*sock = INVALID_SOCKET;
			return 0;
		}

#if defined(_WIN32)
		ret = getsockopt(*sock, SOL_SOCKET, SO_ERROR, (char *)psockerr, &len);
#else
		ret = getsockopt(*sock, SOL_SOCKET, SO_ERROR, psockerr, &len);
#endif

		if ((ret == 0) && (sockerr == 0)) {
			conn_ret = 0;
		}
	}

	if (conn_ret != 0) {
		/* Not connected */
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_CONNECT_FAILED;
			error->code_sub = (unsigned)ERRNO;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "connect(%s:%d): error %s",
			            host,
			            port,
			            strerror(sockerr));
		}
		closesocket(*sock);
		*sock = INVALID_SOCKET;
		return 0;
	}

	return 1;
}


CIVETWEB_API int
mg_url_encode(const char *src, char *dst, size_t dst_len)
{
	static const char *dont_escape = "._-$,;~()";
	static const char *hex = "0123456789abcdef";
	char *pos = dst;
	const char *end = dst + dst_len - 1;

	for (; ((*src != '\0') && (pos < end)); src++, pos++) {
		if (isalnum((unsigned char)*src)
		    || (strchr(dont_escape, *src) != NULL)) {
			*pos = *src;
		} else if (pos + 2 < end) {
			pos[0] = '%';
			pos[1] = hex[(unsigned char)*src >> 4];
			pos[2] = hex[(unsigned char)*src & 0xf];
			pos += 2;
		} else {
			break;
		}
	}

	*pos = '\0';
	return (*src == '\0') ? (int)(pos - dst) : -1;
}

/* Return 0 on success, non-zero if an error occurs. */

static int
print_dir_entry(struct mg_connection *conn, struct de *de)
{
	size_t namesize, escsize, i;
	char *href, *esc, *p;
	char size[64], mod[64];
#if defined(REENTRANT_TIME)
	struct tm _tm;
	struct tm *tm = &_tm;
#else
	struct tm *tm;
#endif

	/* Estimate worst case size for encoding and escaping */
	namesize = strlen(de->file_name) + 1;
	escsize = de->file_name[strcspn(de->file_name, "&<>")] ? namesize * 5 : 0;
	href = (char *)mg_malloc(namesize * 3 + escsize);
	if (href == NULL) {
		return -1;
	}
	mg_url_encode(de->file_name, href, namesize * 3);
	esc = NULL;
	if (escsize > 0) {
		/* HTML escaping needed */
		esc = href + namesize * 3;
		for (i = 0, p = esc; de->file_name[i]; i++, p += strlen(p)) {
			mg_strlcpy(p, de->file_name + i, 2);
			if (*p == '&') {
				strcpy(p, "&amp;");
			} else if (*p == '<') {
				strcpy(p, "&lt;");
			} else if (*p == '>') {
				strcpy(p, "&gt;");
			}
		}
	}

	if (de->file.is_directory) {
		mg_snprintf(conn,
		            NULL, /* Buffer is big enough */
		            size,
		            sizeof(size),
		            "%s",
		            "[DIRECTORY]");
	} else {
		/* We use (signed) cast below because MSVC 6 compiler cannot
		 * convert unsigned __int64 to double. Sigh. */
		if (de->file.size < 1024) {
			mg_snprintf(conn,
			            NULL, /* Buffer is big enough */
			            size,
			            sizeof(size),
			            "%d",
			            (int)de->file.size);
		} else if (de->file.size < 0x100000) {
			mg_snprintf(conn,
			            NULL, /* Buffer is big enough */
			            size,
			            sizeof(size),
			            "%.1fk",
			            (double)de->file.size / 1024.0);
		} else if (de->file.size < 0x40000000) {
			mg_snprintf(conn,
			            NULL, /* Buffer is big enough */
			            size,
			            sizeof(size),
			            "%.1fM",
			            (double)de->file.size / 1048576);
		} else {
			mg_snprintf(conn,
			            NULL, /* Buffer is big enough */
			            size,
			            sizeof(size),
			            "%.1fG",
			            (double)de->file.size / 1073741824);
		}
	}

	/* Note: mg_snprintf will not cause a buffer overflow above.
	 * So, string truncation checks are not required here. */

#if defined(REENTRANT_TIME)
	localtime_r(&de->file.last_modified, tm);
#else
	tm = localtime(&de->file.last_modified);
#endif
	if (tm != NULL) {
		strftime(mod, sizeof(mod), "%d-%b-%Y %H:%M", tm);
	} else {
		mg_strlcpy(mod, "01-Jan-1970 00:00", sizeof(mod));
	}
	mg_printf(conn,
	          "<tr><td><a href=\"%s%s\">%s%s</a></td>"
	          "<td>&nbsp;%s</td><td>&nbsp;&nbsp;%s</td></tr>\n",
	          href,
	          de->file.is_directory ? "/" : "",
	          esc ? esc : de->file_name,
	          de->file.is_directory ? "/" : "",
	          mod,
	          size);
	mg_free(href);
	return 0;
}


/* This function is called from send_directory() and used for
 * sorting directory entries by size, name, or modification time. */
static int
compare_dir_entries(const void *p1, const void *p2, void *arg)
{
	const char *query_string = (const char *)(arg != NULL ? arg : "");
	if (p1 && p2) {
		const struct de *a = (const struct de *)p1, *b = (const struct de *)p2;
		int cmp_result = 0;

		if ((query_string == NULL) || (query_string[0] == '\0')) {
			query_string = "n";
		}

		/* Sort Directories vs Files */
		if (a->file.is_directory && !b->file.is_directory) {
			return -1; /* Always put directories on top */
		} else if (!a->file.is_directory && b->file.is_directory) {
			return 1; /* Always put directories on top */
		}

		/* Sort by size or date */
		if (*query_string == 's') {
			cmp_result = (a->file.size == b->file.size)
			                 ? 0
			                 : ((a->file.size > b->file.size) ? 1 : -1);
		} else if (*query_string == 'd') {
			cmp_result =
			    (a->file.last_modified == b->file.last_modified)
			        ? 0
			        : ((a->file.last_modified > b->file.last_modified) ? 1
			                                                           : -1);
		}

		/* Sort by name:
		 * if (*query_string == 'n')  ...
		 * but also sort files of same size/date by name as secondary criterion.
		 */
		if (cmp_result == 0) {
			cmp_result = strcmp(a->file_name, b->file_name);
		}

		/* For descending order, invert result */
		return (query_string[1] == 'd') ? -cmp_result : cmp_result;
	}
	return 0;
}


static int
must_hide_file(struct mg_connection *conn, const char *path)
{
	if (conn && conn->dom_ctx) {
		const char *pw_pattern = "**" PASSWORDS_FILE_NAME "$";
		const char *pattern = conn->dom_ctx->config[HIDE_FILES];
		return (match_prefix_strlen(pw_pattern, path) > 0)
		       || (match_prefix_strlen(pattern, path) > 0);
	}
	return 0;
}


#if !defined(NO_FILESYSTEMS)
static int
scan_directory(struct mg_connection *conn,
               const char *dir,
               void *data,
               int (*cb)(struct de *, void *))
{
	char path[UTF8_PATH_MAX];
	struct dirent *dp;
	DIR *dirp;
	struct de de;
	int truncated;

	if ((dirp = mg_opendir(conn, dir)) == NULL) {
		return 0;
	} else {

		while ((dp = mg_readdir(dirp)) != NULL) {
			/* Do not show current dir and hidden files */
			if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")
			    || must_hide_file(conn, dp->d_name)) {
				continue;
			}

			mg_snprintf(
			    conn, &truncated, path, sizeof(path), "%s/%s", dir, dp->d_name);

			/* If we don't memset stat structure to zero, mtime will have
			 * garbage and strftime() will segfault later on in
			 * print_dir_entry(). memset is required only if mg_stat()
			 * fails. For more details, see
			 * http://code.google.com/p/mongoose/issues/detail?id=79 */
			memset(&de.file, 0, sizeof(de.file));

			if (truncated) {
				/* If the path is not complete, skip processing. */
				continue;
			}

			if (!mg_stat(conn, path, &de.file)) {
				mg_cry_internal(conn,
				                "%s: mg_stat(%s) failed: %s",
				                __func__,
				                path,
				                strerror(ERRNO));
			}
			de.file_name = dp->d_name;
			if (cb(&de, data)) {
				/* stopped */
				break;
			}
		}
		(void)mg_closedir(dirp);
	}
	return 1;
}
#endif /* NO_FILESYSTEMS */


#if !defined(NO_FILES)
static int
remove_directory(struct mg_connection *conn, const char *dir)
{
	char path[UTF8_PATH_MAX];
	struct dirent *dp;
	DIR *dirp;
	struct de de;
	int truncated;
	int ok = 1;

	if ((dirp = mg_opendir(conn, dir)) == NULL) {
		return 0;
	} else {

		while ((dp = mg_readdir(dirp)) != NULL) {
			/* Do not show current dir (but show hidden files as they will
			 * also be removed) */
			if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
				continue;
			}

			mg_snprintf(
			    conn, &truncated, path, sizeof(path), "%s/%s", dir, dp->d_name);

			/* If we don't memset stat structure to zero, mtime will have
			 * garbage and strftime() will segfault later on in
			 * print_dir_entry(). memset is required only if mg_stat()
			 * fails. For more details, see
			 * http://code.google.com/p/mongoose/issues/detail?id=79 */
			memset(&de.file, 0, sizeof(de.file));

			if (truncated) {
				/* Do not delete anything shorter */
				ok = 0;
				continue;
			}

			if (!mg_stat(conn, path, &de.file)) {
				mg_cry_internal(conn,
				                "%s: mg_stat(%s) failed: %s",
				                __func__,
				                path,
				                strerror(ERRNO));
				ok = 0;
			}

			if (de.file.is_directory) {
				if (remove_directory(conn, path) == 0) {
					ok = 0;
				}
			} else {
				/* This will fail file is the file is in memory */
				if (mg_remove(conn, path) == 0) {
					ok = 0;
				}
			}
		}
		(void)mg_closedir(dirp);

		IGNORE_UNUSED_RESULT(rmdir(dir));
	}

	return ok;
}
#endif


struct dir_scan_data {
	struct de *entries;
	size_t num_entries;
	size_t arr_size;
};


#if !defined(NO_FILESYSTEMS)
static int
dir_scan_callback(struct de *de, void *data)
{
	struct dir_scan_data *dsd = (struct dir_scan_data *)data;
	struct de *entries = dsd->entries;

	if ((entries == NULL) || (dsd->num_entries >= dsd->arr_size)) {
		/* Here "entries" is a temporary pointer and can be replaced,
		 * "dsd->entries" is the original pointer */
		entries =
		    (struct de *)mg_realloc(entries,
		                            dsd->arr_size * 2 * sizeof(entries[0]));
		if (entries == NULL) {
			/* stop scan */
			return 1;
		}
		dsd->entries = entries;
		dsd->arr_size *= 2;
	}
	entries[dsd->num_entries].file_name = mg_strdup(de->file_name);
	if (entries[dsd->num_entries].file_name == NULL) {
		/* stop scan */
		return 1;
	}
	entries[dsd->num_entries].file = de->file;
	dsd->num_entries++;

	return 0;
}


static void
handle_directory_request(struct mg_connection *conn, const char *dir)
{
	size_t i;
	int sort_direction;
	struct dir_scan_data data = {NULL, 0, 128};
	char date[64], *esc, *p;
	const char *title;
	time_t curtime = time(NULL);

	if (!conn) {
		return;
	}

	if (!scan_directory(conn, dir, &data, dir_scan_callback)) {
		mg_send_http_error(conn,
		                   500,
		                   "Error: Cannot open directory\nopendir(%s): %s",
		                   dir,
		                   strerror(ERRNO));
		return;
	}

	gmt_time_string(date, sizeof(date), &curtime);

	esc = NULL;
	title = conn->request_info.local_uri;
	if (title[strcspn(title, "&<>")]) {
		/* HTML escaping needed */
		esc = (char *)mg_malloc(strlen(title) * 5 + 1);
		if (esc) {
			for (i = 0, p = esc; title[i]; i++, p += strlen(p)) {
				mg_strlcpy(p, title + i, 2);
				if (*p == '&') {
					strcpy(p, "&amp;");
				} else if (*p == '<') {
					strcpy(p, "&lt;");
				} else if (*p == '>') {
					strcpy(p, "&gt;");
				}
			}
		} else {
			title = "";
		}
	}

	sort_direction = ((conn->request_info.query_string != NULL)
	                  && (conn->request_info.query_string[0] != '\0')
	                  && (conn->request_info.query_string[1] == 'd'))
	                     ? 'a'
	                     : 'd';

	conn->must_close = 1;

	/* Create 200 OK response */
	mg_response_header_start(conn, 200);
	send_static_cache_header(conn);
	send_additional_header(conn);
	mg_response_header_add(conn,
	                       "Content-Type",
	                       "text/html; charset=utf-8",
	                       -1);

	/* Send all headers */
	mg_response_header_send(conn);

	/* Body */
	mg_printf(conn,
	          "<!DOCTYPE html>"
	          "<html><head><title>Index of %s</title>"
	          "<style>th {text-align: left;}</style></head>"
	          "<body><h1>Index of %s</h1><pre><table cellpadding=\"0\">"
	          "<tr><th><a href=\"?n%c\">Name</a></th>"
	          "<th><a href=\"?d%c\">Modified</a></th>"
	          "<th><a href=\"?s%c\">Size</a></th></tr>"
	          "<tr><td colspan=\"3\"><hr></td></tr>",
	          esc ? esc : title,
	          esc ? esc : title,
	          sort_direction,
	          sort_direction,
	          sort_direction);
	mg_free(esc);

	/* Print first entry - link to a parent directory */
	mg_printf(conn,
	          "<tr><td><a href=\"%s\">%s</a></td>"
	          "<td>&nbsp;%s</td><td>&nbsp;&nbsp;%s</td></tr>\n",
	          "..",
	          "Parent directory",
	          "-",
	          "-");

	/* Sort and print directory entries */
	if (data.entries != NULL) {
		mg_sort(data.entries,
		        data.num_entries,
		        sizeof(data.entries[0]),
		        compare_dir_entries,
		        (void *)conn->request_info.query_string);
		for (i = 0; i < data.num_entries; i++) {
			print_dir_entry(conn, &data.entries[i]);
			mg_free(data.entries[i].file_name);
		}
		mg_free(data.entries);
	}

	mg_printf(conn, "%s", "</table></pre></body></html>");
	conn->status_code = 200;
}
#endif /* NO_FILESYSTEMS */


/* Send len bytes from the opened file to the client. */
static void
send_file_data(struct mg_connection *conn,
               struct mg_file *filep,
               int64_t offset,
               int64_t len,
               int no_buffering)
{
	char buf[MG_BUF_LEN];
	int to_read, num_read, num_written;
	int64_t size;

	if (!filep || !conn) {
		return;
	}

	/* Sanity check the offset */
	size = (filep->stat.size > INT64_MAX) ? INT64_MAX
	                                      : (int64_t)(filep->stat.size);
	offset = (offset < 0) ? 0 : ((offset > size) ? size : offset);

	if (len > 0 && filep->access.fp != NULL) {
		/* file stored on disk */
#if defined(__linux__)
		/* sendfile is only available for Linux */
		if ((conn->ssl == 0) && (conn->throttle == 0)
		    && (!mg_strcasecmp(conn->dom_ctx->config[ALLOW_SENDFILE_CALL],
		                       "yes"))) {
			off_t sf_offs = (off_t)offset;
			ssize_t sf_sent;
			int sf_file = fileno(filep->access.fp);
			int loop_cnt = 0;

			do {
				/* 2147479552 (0x7FFFF000) is a limit found by experiment on
				 * 64 bit Linux (2^31 minus one memory page of 4k?). */
				size_t sf_tosend =
				    (size_t)((len < 0x7FFFF000) ? len : 0x7FFFF000);
				sf_sent =
				    sendfile(conn->client.sock, sf_file, &sf_offs, sf_tosend);
				if (sf_sent > 0) {
					len -= sf_sent;
					offset += sf_sent;
				} else if (loop_cnt == 0) {
					/* This file can not be sent using sendfile.
					 * This might be the case for pseudo-files in the
					 * /sys/ and /proc/ file system.
					 * Use the regular user mode copy code instead. */
					break;
				} else if (sf_sent == 0) {
					/* No error, but 0 bytes sent. May be EOF? */
					return;
				}
				loop_cnt++;

			} while ((len > 0) && (sf_sent >= 0));

			if (sf_sent > 0) {
				return; /* OK */
			}

			/* sf_sent<0 means error, thus fall back to the classic way */
			/* This is always the case, if sf_file is not a "normal" file,
			 * e.g., for sending data from the output of a CGI process. */
			offset = (int64_t)sf_offs;
		}
#endif
		if ((offset > 0) && (fseeko(filep->access.fp, offset, SEEK_SET) != 0)) {
			mg_cry_internal(conn,
			                "%s: fseeko() failed: %s",
			                __func__,
			                strerror(ERRNO));
			mg_send_http_error(
			    conn,
			    500,
			    "%s",
			    "Error: Unable to access file at requested position.");
		} else {
			while (len > 0) {
				/* Calculate how much to read from the file into the buffer. */
				/* If no_buffering is set, we should not wait until the
				 * CGI->Server buffer is filled, but send everything
				 * immediately. In theory buffering could be turned off using
				 * setbuf(filep->access.fp, NULL);
				 * setvbuf(filep->access.fp, NULL, _IONBF, 0);
				 * but in practice this does not work. A "Linux only" solution
				 * may be to use select(). The only portable way is to read byte
				 * by byte, but this is quite inefficient from a performance
				 * point of view. */
				to_read = no_buffering ? 1 : sizeof(buf);
				if ((int64_t)to_read > len) {
					to_read = (int)len;
				}

				/* Read from file, exit the loop on error */
				if ((num_read = pull_inner(filep->access.fp,
				                           NULL,
				                           buf,
				                           to_read,
				                           /* unused */ 0.0))
				    <= 0) {
					break;
				}

				/* Send read bytes to the client, exit the loop on error */
				if ((num_written = mg_write(conn, buf, (size_t)num_read))
				    != num_read) {
					break;
				}

				/* Both read and were successful, adjust counters */
				len -= num_written;
			}
		}
	}
}


static int
parse_range_header(const char *header, int64_t *a, int64_t *b)
{
	return sscanf(header,
	              "bytes=%" INT64_FMT "-%" INT64_FMT,
	              a,
	              b); // NOLINT(cert-err34-c) 'sscanf' used to convert a string
	                  // to an integer value, but function will not report
	                  // conversion errors; consider using 'strtol' instead
}


static void
construct_etag(char *buf, size_t buf_len, const struct mg_file_stat *filestat)
{
	if ((filestat != NULL) && (buf != NULL)) {
		mg_snprintf(NULL,
		            NULL, /* All calls to construct_etag use 64 byte buffer */
		            buf,
		            buf_len,
		            "\"%lx.%" INT64_FMT "\"",
		            (unsigned long)filestat->last_modified,
		            filestat->size);
	}
}


static void
fclose_on_exec(struct mg_file_access *filep, struct mg_connection *conn)
{
	if (filep != NULL && filep->fp != NULL) {
#if defined(_WIN32)
		(void)conn; /* Unused. */
#else
		if (fcntl(fileno(filep->fp), F_SETFD, FD_CLOEXEC) != 0) {
			mg_cry_internal(conn,
			                "%s: fcntl(F_SETFD FD_CLOEXEC) failed: %s",
			                __func__,
			                strerror(ERRNO));
		}
#endif
	}
}


#if defined(USE_ZLIB)
#include "mod_zlib.inl"
#endif


#if !defined(NO_FILESYSTEMS)
static void
handle_static_file_request(struct mg_connection *conn,
                           const char *path,
                           struct mg_file *filep,
                           const char *mime_type,
                           const char *additional_headers)
{
	char lm[64], etag[64];
	char range[128]; /* large enough, so there will be no overflow */
	const char *range_hdr;
	int64_t cl, r1, r2;
	struct vec mime_vec;
	int n, truncated;
	char gz_path[UTF8_PATH_MAX];
	const char *encoding = 0;
	int is_head_request;

#if defined(USE_ZLIB)
	/* Compression is allowed, unless there is a reason not to use
	 * compression. If the file is already compressed, too small or a
	 * "range" request was made, on the fly compression is not possible. */
	int allow_on_the_fly_compression = 1;
#endif

	if ((conn == NULL) || (conn->dom_ctx == NULL) || (filep == NULL)) {
		return;
	}

	is_head_request = !strcmp(conn->request_info.request_method, "HEAD");

	if (mime_type == NULL) {
		get_mime_type(conn, path, &mime_vec);
	} else {
		mime_vec.ptr = mime_type;
		mime_vec.len = strlen(mime_type);
	}
	if (filep->stat.size > INT64_MAX) {
		mg_send_http_error(conn,
		                   500,
		                   "Error: File size is too large to send\n%" INT64_FMT,
		                   filep->stat.size);
		return;
	}
	cl = (int64_t)filep->stat.size;
	conn->status_code = 200;
	range[0] = '\0';

#if defined(USE_ZLIB)
	/* if this file is in fact a pre-gzipped file, rewrite its filename
	 * it's important to rewrite the filename after resolving
	 * the mime type from it, to preserve the actual file's type */
	if (!conn->accept_gzip) {
		allow_on_the_fly_compression = 0;
	}
#endif

	/* Check if there is a range header */
	range_hdr = mg_get_header(conn, "Range");

	/* For gzipped files, add *.gz */
	if (filep->stat.is_gzipped) {
		mg_snprintf(conn, &truncated, gz_path, sizeof(gz_path), "%s.gz", path);

		if (truncated) {
			mg_send_http_error(conn,
			                   500,
			                   "Error: Path of zipped file too long (%s)",
			                   path);
			return;
		}

		path = gz_path;
		encoding = "gzip";

#if defined(USE_ZLIB)
		/* File is already compressed. No "on the fly" compression. */
		allow_on_the_fly_compression = 0;
#endif
	} else if ((conn->accept_gzip) && (range_hdr == NULL)
	           && (filep->stat.size >= MG_FILE_COMPRESSION_SIZE_LIMIT)) {
		struct mg_file_stat file_stat;

		mg_snprintf(conn, &truncated, gz_path, sizeof(gz_path), "%s.gz", path);

		if (!truncated && mg_stat(conn, gz_path, &file_stat)
		    && !file_stat.is_directory) {
			file_stat.is_gzipped = 1;
			filep->stat = file_stat;
			cl = (int64_t)filep->stat.size;
			path = gz_path;
			encoding = "gzip";

#if defined(USE_ZLIB)
			/* File is already compressed. No "on the fly" compression. */
			allow_on_the_fly_compression = 0;
#endif
		}
	}

	if (!mg_fopen(conn, path, MG_FOPEN_MODE_READ, filep)) {
		mg_send_http_error(conn,
		                   500,
		                   "Error: Cannot open file\nfopen(%s): %s",
		                   path,
		                   strerror(ERRNO));
		return;
	}

	fclose_on_exec(&filep->access, conn);

	/* If "Range" request was made: parse header, send only selected part
	 * of the file. */
	r1 = r2 = 0;
	if ((range_hdr != NULL)
	    && ((n = parse_range_header(range_hdr, &r1, &r2)) > 0) && (r1 >= 0)
	    && (r2 >= 0)) {
		/* actually, range requests don't play well with a pre-gzipped
		 * file (since the range is specified in the uncompressed space) */
		if (filep->stat.is_gzipped) {
			mg_send_http_error(
			    conn,
			    416, /* 416 = Range Not Satisfiable */
			    "%s",
			    "Error: Range requests in gzipped files are not supported");
			(void)mg_fclose(
			    &filep->access); /* ignore error on read only file */
			return;
		}
		conn->status_code = 206;
		cl = (n == 2) ? (((r2 > cl) ? cl : r2) - r1 + 1) : (cl - r1);
		mg_snprintf(conn,
		            NULL, /* range buffer is big enough */
		            range,
		            sizeof(range),
		            "bytes "
		            "%" INT64_FMT "-%" INT64_FMT "/%" INT64_FMT,
		            r1,
		            r1 + cl - 1,
		            filep->stat.size);

#if defined(USE_ZLIB)
		/* Do not compress ranges. */
		allow_on_the_fly_compression = 0;
#endif
	}

	/* Do not compress small files. Small files do not benefit from file
	 * compression, but there is still some overhead. */
#if defined(USE_ZLIB)
	if (filep->stat.size < MG_FILE_COMPRESSION_SIZE_LIMIT) {
		/* File is below the size limit. */
		allow_on_the_fly_compression = 0;
	}
#endif

	/* Prepare Etag, and Last-Modified headers. */
	gmt_time_string(lm, sizeof(lm), &filep->stat.last_modified);
	construct_etag(etag, sizeof(etag), &filep->stat);

	/* Create 2xx (200, 206) response */
	mg_response_header_start(conn, conn->status_code);
	send_static_cache_header(conn);
	send_additional_header(conn);
	send_cors_header(conn);
	mg_response_header_add(conn,
	                       "Content-Type",
	                       mime_vec.ptr,
	                       (int)mime_vec.len);
	mg_response_header_add(conn, "Last-Modified", lm, -1);
	mg_response_header_add(conn, "Etag", etag, -1);

#if defined(USE_ZLIB)
	/* On the fly compression allowed */
	if (allow_on_the_fly_compression) {
		/* For on the fly compression, we don't know the content size in
		 * advance, so we have to use chunked encoding */
		encoding = "gzip";
		if (conn->protocol_type == PROTOCOL_TYPE_HTTP1) {
			/* HTTP/2 is always using "chunks" (frames) */
			mg_response_header_add(conn, "Transfer-Encoding", "chunked", -1);
		}

	} else
#endif
	{
		/* Without on-the-fly compression, we know the content-length
		 * and we can use ranges (with on-the-fly compression we cannot).
		 * So we send these response headers only in this case. */
		char len[32];
		int trunc = 0;
		mg_snprintf(conn, &trunc, len, sizeof(len), "%" INT64_FMT, cl);

		if (!trunc) {
			mg_response_header_add(conn, "Content-Length", len, -1);
		}

		mg_response_header_add(conn, "Accept-Ranges", "bytes", -1);
	}

	if (encoding) {
		mg_response_header_add(conn, "Content-Encoding", encoding, -1);
	}
	if (range[0] != 0) {
		mg_response_header_add(conn, "Content-Range", range, -1);
	}

	/* The code above does not add any header starting with X- to make
	 * sure no one of the additional_headers is included twice */
	if ((additional_headers != NULL) && (*additional_headers != 0)) {
		mg_response_header_add_lines(conn, additional_headers);
	}

	/* Send all headers */
	mg_response_header_send(conn);

	if (!is_head_request) {
#if defined(USE_ZLIB)
		if (allow_on_the_fly_compression) {
			/* Compress and send */
			send_compressed_data(conn, filep);
		} else
#endif
		{
			/* Send file directly */
			send_file_data(conn, filep, r1, cl, 0); /* send static file */
		}
	}
	(void)mg_fclose(&filep->access); /* ignore error on read only file */
}


CIVETWEB_API int
mg_send_file_body(struct mg_connection *conn, const char *path)
{
	struct mg_file file = STRUCT_FILE_INITIALIZER;
	if (!mg_fopen(conn, path, MG_FOPEN_MODE_READ, &file)) {
		return -1;
	}
	fclose_on_exec(&file.access, conn);
	send_file_data(conn, &file, 0, INT64_MAX, 0); /* send static file */
	(void)mg_fclose(&file.access); /* Ignore errors for readonly files */
	return 0;                      /* >= 0 for OK */
}
#endif /* NO_FILESYSTEMS */


#if !defined(NO_CACHING)
/* Return True if we should reply 304 Not Modified. */
static int
is_not_modified(const struct mg_connection *conn,
                const struct mg_file_stat *filestat)
{
	char etag[64];
	const char *ims = mg_get_header(conn, "If-Modified-Since");
	const char *inm = mg_get_header(conn, "If-None-Match");
	construct_etag(etag, sizeof(etag), filestat);

	return ((inm != NULL) && !mg_strcasecmp(etag, inm))
	       || ((ims != NULL)
	           && (filestat->last_modified <= parse_date_string(ims)));
}


static void
handle_not_modified_static_file_request(struct mg_connection *conn,
                                        struct mg_file *filep)
{
	char lm[64], etag[64];

	if ((conn == NULL) || (filep == NULL)) {
		return;
	}

	gmt_time_string(lm, sizeof(lm), &filep->stat.last_modified);
	construct_etag(etag, sizeof(etag), &filep->stat);

	/* Create 304 "not modified" response */
	mg_response_header_start(conn, 304);
	send_static_cache_header(conn);
	send_additional_header(conn);
	mg_response_header_add(conn, "Last-Modified", lm, -1);
	mg_response_header_add(conn, "Etag", etag, -1);

	/* Send all headers */
	mg_response_header_send(conn);
}
#endif


#if !defined(NO_FILESYSTEMS)
CIVETWEB_API void
mg_send_file(struct mg_connection *conn, const char *path)
{
	mg_send_mime_file2(conn, path, NULL, NULL);
}


CIVETWEB_API void
mg_send_mime_file(struct mg_connection *conn,
                  const char *path,
                  const char *mime_type)
{
	mg_send_mime_file2(conn, path, mime_type, NULL);
}


CIVETWEB_API void
mg_send_mime_file2(struct mg_connection *conn,
                   const char *path,
                   const char *mime_type,
                   const char *additional_headers)
{
	struct mg_file file = STRUCT_FILE_INITIALIZER;

	if (!conn) {
		/* No conn */
		return;
	}

	if (mg_stat(conn, path, &file.stat)) {
#if !defined(NO_CACHING)
		if (is_not_modified(conn, &file.stat)) {
			/* Send 304 "Not Modified" - this must not send any body data */
			handle_not_modified_static_file_request(conn, &file);
		} else
#endif /* NO_CACHING */
		    if (file.stat.is_directory) {
			if (!mg_strcasecmp(conn->dom_ctx->config[ENABLE_DIRECTORY_LISTING],
			                   "yes")) {
				handle_directory_request(conn, path);
			} else {
				mg_send_http_error(conn,
				                   403,
				                   "%s",
				                   "Error: Directory listing denied");
			}
		} else {
			handle_static_file_request(
			    conn, path, &file, mime_type, additional_headers);
		}
	} else {
		mg_send_http_error(conn, 404, "%s", "Error: File not found");
	}
}


/* For a given PUT path, create all intermediate subdirectories.
 * Return  0  if the path itself is a directory.
 * Return  1  if the path leads to a file.
 * Return -1  for if the path is too long.
 * Return -2  if path can not be created.
 */
static int
put_dir(struct mg_connection *conn, const char *path)
{
	char buf[UTF8_PATH_MAX];
	const char *s, *p;
	struct mg_file file = STRUCT_FILE_INITIALIZER;
	size_t len;
	int res = 1;

	for (s = p = path + 2; (p = strchr(s, '/')) != NULL; s = ++p) {
		len = (size_t)(p - path);
		if (len >= sizeof(buf)) {
			/* path too long */
			res = -1;
			break;
		}
		memcpy(buf, path, len);
		buf[len] = '\0';

		/* Try to create intermediate directory */
		DEBUG_TRACE("mkdir(%s)", buf);
		if (!mg_stat(conn, buf, &file.stat) && mg_mkdir(conn, buf, 0755) != 0) {
			/* path does not exist and can not be created */
			res = -2;
			break;
		}

		/* Is path itself a directory? */
		if (p[1] == '\0') {
			res = 0;
		}
	}

	return res;
}


static void
remove_bad_file(const struct mg_connection *conn, const char *path)
{
	int r = mg_remove(conn, path);
	if (r != 0) {
		mg_cry_internal(conn,
		                "%s: Cannot remove invalid file %s",
		                __func__,
		                path);
	}
}


CIVETWEB_API long long
mg_store_body(struct mg_connection *conn, const char *path)
{
	char buf[MG_BUF_LEN];
	long long len = 0;
	int ret, n;
	struct mg_file fi;

	if (conn->consumed_content != 0) {
		mg_cry_internal(conn, "%s: Contents already consumed", __func__);
		return -11;
	}

	ret = put_dir(conn, path);
	if (ret < 0) {
		/* -1 for path too long,
		 * -2 for path can not be created. */
		return ret;
	}
	if (ret != 1) {
		/* Return 0 means, path itself is a directory. */
		return 0;
	}

	if (mg_fopen(conn, path, MG_FOPEN_MODE_WRITE, &fi) == 0) {
		return -12;
	}

	ret = mg_read(conn, buf, sizeof(buf));
	while (ret > 0) {
		n = (int)fwrite(buf, 1, (size_t)ret, fi.access.fp);
		if (n != ret) {
			(void)mg_fclose(
			    &fi.access); /* File is bad and will be removed anyway. */
			remove_bad_file(conn, path);
			return -13;
		}
		len += ret;
		ret = mg_read(conn, buf, sizeof(buf));
	}

	/* File is open for writing. If fclose fails, there was probably an
	 * error flushing the buffer to disk, so the file on disk might be
	 * broken. Delete it and return an error to the caller. */
	if (mg_fclose(&fi.access) != 0) {
		remove_bad_file(conn, path);
		return -14;
	}

	return len;
}
#endif /* NO_FILESYSTEMS */


/* Parse a buffer:
 * Forward the string pointer till the end of a word, then
 * terminate it and forward till the begin of the next word.
 */
static int
skip_to_end_of_word_and_terminate(char **ppw, int eol)
{
	/* Forward until a space is found - use isgraph here */
	/* Extended ASCII characters are also treated as word characters. */
	/* See http://www.cplusplus.com/reference/cctype/ */
	while ((unsigned char)**ppw > 127 || isgraph((unsigned char)**ppw)) {
		(*ppw)++;
	}

	/* Check end of word */
	if (eol) {
		/* must be a end of line */
		if ((**ppw != '\r') && (**ppw != '\n')) {
			return -1;
		}
	} else {
		/* must be a end of a word, but not a line */
		if (**ppw != ' ') {
			return -1;
		}
	}

	/* Terminate and forward to the next word */
	do {
		**ppw = 0;
		(*ppw)++;
	} while (isspace((unsigned char)**ppw));

	/* Check after term */
	if (!eol) {
		/* if it's not the end of line, there must be a next word */
		if (!isgraph((unsigned char)**ppw)) {
			return -1;
		}
	}

	/* ok */
	return 1;
}


/* Parse HTTP headers from the given buffer, advance buf pointer
 * to the point where parsing stopped.
 * All parameters must be valid pointers (not NULL).
 * Return <0 on error. */
static int
parse_http_headers(char **buf, struct mg_header hdr[MG_MAX_HEADERS])
{
	int i;
	int num_headers = 0;

	for (i = 0; i < (int)MG_MAX_HEADERS; i++) {
		char *dp = *buf;

		/* Skip all ASCII characters (>SPACE, <127), to find a ':' */
		while ((*dp != ':') && (*dp >= 33) && (*dp <= 126)) {
			dp++;
		}
		if (dp == *buf) {
			/* End of headers reached. */
			break;
		}

		/* Drop all spaces after header name before : */
		while (*dp == ' ') {
			*dp = 0;
			dp++;
		}
		if (*dp != ':') {
			/* This is not a valid field. */
			return -1;
		}

		/* End of header key (*dp == ':') */
		/* Truncate here and set the key name */
		*dp = 0;
		hdr[i].name = *buf;

		/* Skip all spaces */
		do {
			dp++;
		} while ((*dp == ' ') || (*dp == '\t'));

		/* The rest of the line is the value */
		hdr[i].value = dp;

		/* Find end of line */
		while ((*dp != 0) && (*dp != '\r') && (*dp != '\n')) {
			dp++;
		};

		/* eliminate \r */
		if (*dp == '\r') {
			*dp = 0;
			dp++;
			if (*dp != '\n') {
				/* This is not a valid line. */
				return -1;
			}
		}

		/* here *dp is either 0 or '\n' */
		/* in any case, we have found a complete header */
		num_headers = i + 1;

		if (*dp) {
			*dp = 0;
			dp++;
			*buf = dp;

			if ((dp[0] == '\r') || (dp[0] == '\n')) {
				/* We've had CRLF twice in a row
				 * This is the end of the headers */
				break;
			}
			/* continue within the loop, find the next header */
		} else {
			*buf = dp;
			break;
		}
	}
	return num_headers;
}


struct mg_http_method_info {
	const char *name;
	int request_has_body;
	int response_has_body;
	int is_safe;
	int is_idempotent;
	int is_cacheable;
};


/* https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods */
static const struct mg_http_method_info http_methods[] = {
    /* HTTP (RFC 2616) */
    {"GET", 0, 1, 1, 1, 1},
    {"POST", 1, 1, 0, 0, 0},
    {"PUT", 1, 0, 0, 1, 0},
    {"DELETE", 0, 0, 0, 1, 0},
    {"HEAD", 0, 0, 1, 1, 1},
    {"OPTIONS", 0, 0, 1, 1, 0},
    {"CONNECT", 1, 1, 0, 0, 0},
    /* TRACE method (RFC 2616) is not supported for security reasons */

    /* PATCH method (RFC 5789) */
    {"PATCH", 1, 0, 0, 0, 0},
    /* PATCH method only allowed for CGI/Lua/LSP and callbacks. */

    /* WEBDAV (RFC 2518) */
    {"PROPFIND", 0, 1, 1, 1, 0},
    /* http://www.webdav.org/specs/rfc4918.html, 9.1:
     * Some PROPFIND results MAY be cached, with care,
     * as there is no cache validation mechanism for
     * most properties. This method is both safe and
     * idempotent (see Section 9.1 of [RFC2616]). */
    {"MKCOL", 0, 0, 0, 1, 0},
    /* http://www.webdav.org/specs/rfc4918.html, 9.1:
     * When MKCOL is invoked without a request body,
     * the newly created collection SHOULD have no
     * members. A MKCOL request message may contain
     * a message body. The precise behavior of a MKCOL
     * request when the body is present is undefined,
     * ... ==> We do not support MKCOL with body data.
     * This method is idempotent, but not safe (see
     * Section 9.1 of [RFC2616]). Responses to this
     * method MUST NOT be cached. */

    /* Methods for write access to files on WEBDAV (RFC 2518) */
    {"LOCK", 1, 1, 0, 0, 0},
    {"UNLOCK", 1, 0, 0, 0, 0},
    {"PROPPATCH", 1, 1, 0, 0, 0},
    {"COPY", 1, 0, 0, 0, 0},
    {"MOVE", 1, 1, 0, 0, 0},

    /* Unsupported WEBDAV Methods: */
    /* + 11 methods from RFC 3253 */
    /* ORDERPATCH (RFC 3648) */
    /* ACL (RFC 3744) */
    /* SEARCH (RFC 5323) */
    /* + MicroSoft extensions
     * https://msdn.microsoft.com/en-us/library/aa142917.aspx */

    /* REPORT method (RFC 3253) */
    {"REPORT", 1, 1, 1, 1, 1},
    /* REPORT method only allowed for CGI/Lua/LSP and callbacks. */
    /* It was defined for WEBDAV in RFC 3253, Sec. 3.6
     * (https://tools.ietf.org/html/rfc3253#section-3.6), but seems
     * to be useful for REST in case a "GET request with body" is
     * required. */

    {NULL, 0, 0, 0, 0, 0}
    /* end of list */
};


/* All method names */
static char *all_methods = NULL; /* Built by mg_init_library */


static const struct mg_http_method_info *
get_http_method_info(const char *method)
{
	/* Check if the method is known to the server. The list of all known
	 * HTTP methods can be found here at
	 * http://www.iana.org/assignments/http-methods/http-methods.xhtml
	 */
	const struct mg_http_method_info *m = http_methods;

	while (m->name) {
		if (!strcmp(m->name, method)) {
			return m;
		}
		m++;
	}
	return NULL;
}


static int
is_valid_http_method(const char *method)
{
	return (get_http_method_info(method) != NULL);
}


/* Parse HTTP request, fill in mg_request_info structure.
 * This function modifies the buffer by NUL-terminating
 * HTTP request components, header names and header values.
 * Parameters:
 *   buf (in/out): pointer to the HTTP header to parse and split
 *   len (in): length of HTTP header buffer
 *   re (out): parsed header as mg_request_info
 * buf and ri must be valid pointers (not NULL), len>0.
 * Returns <0 on error. */
static int
parse_http_request(char *buf, int len, struct mg_request_info *ri)
{
	int request_length;
	int init_skip = 0;

	/* Reset attributes. DO NOT TOUCH is_ssl, remote_addr,
	 * remote_port */
	ri->remote_user = ri->request_method = ri->request_uri = ri->http_version =
	    NULL;
	ri->num_headers = 0;

	/* RFC says that all initial whitespaces should be ignored */
	/* This included all leading \r and \n (isspace) */
	/* See table: http://www.cplusplus.com/reference/cctype/ */
	while ((len > 0) && isspace((unsigned char)*buf)) {
		buf++;
		len--;
		init_skip++;
	}

	if (len == 0) {
		/* Incomplete request */
		return 0;
	}

	/* Control characters are not allowed, including zero */
	if (iscntrl((unsigned char)*buf)) {
		return -1;
	}

	/* Find end of HTTP header */
	request_length = get_http_header_len(buf, len);
	if (request_length <= 0) {
		return request_length;
	}
	buf[request_length - 1] = '\0';

	if ((*buf == 0) || (*buf == '\r') || (*buf == '\n')) {
		return -1;
	}

	/* The first word has to be the HTTP method */
	ri->request_method = buf;

	if (skip_to_end_of_word_and_terminate(&buf, 0) <= 0) {
		return -1;
	}

	/* The second word is the URI */
	ri->request_uri = buf;

	if (skip_to_end_of_word_and_terminate(&buf, 0) <= 0) {
		return -1;
	}

	/* Next would be the HTTP version */
	ri->http_version = buf;

	if (skip_to_end_of_word_and_terminate(&buf, 1) <= 0) {
		return -1;
	}

	/* Check for a valid HTTP version key */
	if (strncmp(ri->http_version, "HTTP/", 5) != 0) {
		/* Invalid request */
		return -1;
	}
	ri->http_version += 5;

	/* Check for a valid http method */
	if (!is_valid_http_method(ri->request_method)) {
		return -1;
	}

	/* Parse all HTTP headers */
	ri->num_headers = parse_http_headers(&buf, ri->http_headers);
	if (ri->num_headers < 0) {
		/* Error while parsing headers */
		return -1;
	}

	return request_length + init_skip;
}


static int
parse_http_response(char *buf, int len, struct mg_response_info *ri)
{
	int response_length;
	int init_skip = 0;
	char *tmp, *tmp2;
	long l;

	/* Initialize elements. */
	ri->http_version = ri->status_text = NULL;
	ri->num_headers = ri->status_code = 0;

	/* RFC says that all initial whitespaces should be ignored */
	/* This included all leading \r and \n (isspace) */
	/* See table: http://www.cplusplus.com/reference/cctype/ */
	while ((len > 0) && isspace((unsigned char)*buf)) {
		buf++;
		len--;
		init_skip++;
	}

	if (len == 0) {
		/* Incomplete request */
		return 0;
	}

	/* Control characters are not allowed, including zero */
	if (iscntrl((unsigned char)*buf)) {
		return -1;
	}

	/* Find end of HTTP header */
	response_length = get_http_header_len(buf, len);
	if (response_length <= 0) {
		return response_length;
	}
	buf[response_length - 1] = '\0';

	if ((*buf == 0) || (*buf == '\r') || (*buf == '\n')) {
		return -1;
	}

	/* The first word is the HTTP version */
	/* Check for a valid HTTP version key */
	if (strncmp(buf, "HTTP/", 5) != 0) {
		/* Invalid request */
		return -1;
	}
	buf += 5;
	if (!isgraph((unsigned char)buf[0])) {
		/* Invalid request */
		return -1;
	}
	ri->http_version = buf;

	if (skip_to_end_of_word_and_terminate(&buf, 0) <= 0) {
		return -1;
	}

	/* The second word is the status as a number */
	tmp = buf;

	if (skip_to_end_of_word_and_terminate(&buf, 0) <= 0) {
		return -1;
	}

	l = strtol(tmp, &tmp2, 10);
	if ((l < 100) || (l >= 1000) || ((tmp2 - tmp) != 3) || (*tmp2 != 0)) {
		/* Everything else but a 3 digit code is invalid */
		return -1;
	}
	ri->status_code = (int)l;

	/* The rest of the line is the status text */
	ri->status_text = buf;

	/* Find end of status text */
	/* isgraph or isspace = isprint */
	while (isprint((unsigned char)*buf)) {
		buf++;
	}
	if ((*buf != '\r') && (*buf != '\n')) {
		return -1;
	}
	/* Terminate string and forward buf to next line */
	do {
		*buf = 0;
		buf++;
	} while (isspace((unsigned char)*buf));

	/* Parse all HTTP headers */
	ri->num_headers = parse_http_headers(&buf, ri->http_headers);
	if (ri->num_headers < 0) {
		/* Error while parsing headers */
		return -1;
	}

	return response_length + init_skip;
}


/* Keep reading the input (either opened file descriptor fd, or socket sock,
 * or SSL descriptor ssl) into buffer buf, until \r\n\r\n appears in the
 * buffer (which marks the end of HTTP request). Buffer buf may already
 * have some data. The length of the data is stored in nread.
 * Upon every read operation, increase nread by the number of bytes read. */
static int
read_message(FILE *fp,
             struct mg_connection *conn,
             char *buf,
             int bufsiz,
             int *nread)
{
	int request_len, n = 0;
	struct timespec last_action_time;
	double request_timeout;

	if (!conn) {
		return 0;
	}

	memset(&last_action_time, 0, sizeof(last_action_time));

	if (conn->dom_ctx->config[REQUEST_TIMEOUT]) {
		/* value of request_timeout is in seconds, config in milliseconds */
		request_timeout =
		    strtod(conn->dom_ctx->config[REQUEST_TIMEOUT], NULL) / 1000.0;
	} else {
		request_timeout =
		    strtod(config_options[REQUEST_TIMEOUT].default_value, NULL)
		    / 1000.0;
	}
	if (conn->handled_requests > 0) {
		if (conn->dom_ctx->config[KEEP_ALIVE_TIMEOUT]) {
			request_timeout =
			    strtod(conn->dom_ctx->config[KEEP_ALIVE_TIMEOUT], NULL)
			    / 1000.0;
		}
	}

	request_len = get_http_header_len(buf, *nread);

	while (request_len == 0) {
		/* Full request not yet received */
		if (!STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
			/* Server is to be stopped. */
			return -1;
		}

		if (*nread >= bufsiz) {
			/* Request too long */
			return -2;
		}

		n = pull_inner(
		    fp, conn, buf + *nread, bufsiz - *nread, request_timeout);
		if (n == -2) {
			/* Receive error */
			return -1;
		}

		/* update clock after every read request */
		clock_gettime(CLOCK_MONOTONIC, &last_action_time);

		if (n > 0) {
			*nread += n;
			request_len = get_http_header_len(buf, *nread);
		}

		if ((n <= 0) && (request_timeout >= 0)) {
			if (mg_difftimespec(&last_action_time, &(conn->req_time))
			    > request_timeout) {
				/* Timeout */
				return -3;
			}
		}
	}

	return request_len;
}


#if !defined(NO_CGI) || !defined(NO_FILES)
static int
forward_body_data(struct mg_connection *conn, FILE *fp, SOCKET sock, SSL *ssl)
{
	const char *expect;
	char buf[MG_BUF_LEN];
	int success = 0;

	if (!conn) {
		return 0;
	}

	expect = mg_get_header(conn, "Expect");
	DEBUG_ASSERT(fp != NULL);
	if (!fp) {
		mg_send_http_error(conn, 500, "%s", "Error: NULL File");
		return 0;
	}

	if ((expect != NULL) && (mg_strcasecmp(expect, "100-continue") != 0)) {
		/* Client sent an "Expect: xyz" header and xyz is not 100-continue.
		 */
		mg_send_http_error(conn, 417, "Error: Can not fulfill expectation");
	} else {
		if (expect != NULL) {
			(void)mg_printf(conn, "%s", "HTTP/1.1 100 Continue\r\n\r\n");
			conn->status_code = 100;
		} else {
			conn->status_code = 200;
		}

		DEBUG_ASSERT(conn->consumed_content == 0);

		if (conn->consumed_content != 0) {
			mg_send_http_error(conn, 500, "%s", "Error: Size mismatch");
			return 0;
		}

		for (;;) {
			int nread = mg_read(conn, buf, sizeof(buf));
			if (nread <= 0) {
				success = (nread == 0);
				break;
			}
			if (push_all(conn->phys_ctx, fp, sock, ssl, buf, nread) != nread) {
				break;
			}
		}

		/* Each error code path in this function must send an error */
		if (!success) {
			/* NOTE: Maybe some data has already been sent. */
			/* TODO (low): If some data has been sent, a correct error
			 * reply can no longer be sent, so just close the connection */
			mg_send_http_error(conn, 500, "%s", "");
		}
	}

	return success;
}
#endif


#if defined(USE_TIMERS)

#define TIMER_API static
#include "timer.inl"

#endif /* USE_TIMERS */


#if !defined(NO_CGI)
/* This structure helps to create an environment for the spawned CGI
 * program.
 * Environment is an array of "VARIABLE=VALUE\0" ASCII strings,
 * last element must be NULL.
 * However, on Windows there is a requirement that all these
 * VARIABLE=VALUE\0
 * strings must reside in a contiguous buffer. The end of the buffer is
 * marked by two '\0' characters.
 * We satisfy both worlds: we create an envp array (which is vars), all
 * entries are actually pointers inside buf. */
struct cgi_environment {
	struct mg_connection *conn;
	/* Data block */
	char *buf;      /* Environment buffer */
	size_t buflen;  /* Space available in buf */
	size_t bufused; /* Space taken in buf */
	/* Index block */
	char **var;     /* char **envp */
	size_t varlen;  /* Number of variables available in var */
	size_t varused; /* Number of variables stored in var */
};


static void addenv(struct cgi_environment *env,
                   PRINTF_FORMAT_STRING(const char *fmt),
                   ...) PRINTF_ARGS(2, 3);

/* Append VARIABLE=VALUE\0 string to the buffer, and add a respective
 * pointer into the vars array. Assumes env != NULL and fmt != NULL. */
static void
addenv(struct cgi_environment *env, const char *fmt, ...)
{
	size_t i, n, space;
	int truncated = 0;
	char *added;
	va_list ap;

	if ((env->varlen - env->varused) < 2) {
		mg_cry_internal(env->conn,
		                "%s: Cannot register CGI variable [%s]",
		                __func__,
		                fmt);
		return;
	}

	/* Calculate how much space is left in the buffer */
	space = (env->buflen - env->bufused);

	do {
		/* Space for "\0\0" is always needed. */
		if (space <= 2) {
			/* Allocate new buffer */
			n = env->buflen + CGI_ENVIRONMENT_SIZE;
			added = (char *)mg_realloc_ctx(env->buf, n, env->conn->phys_ctx);
			if (!added) {
				/* Out of memory */
				mg_cry_internal(
				    env->conn,
				    "%s: Cannot allocate memory for CGI variable [%s]",
				    __func__,
				    fmt);
				return;
			}
			/* Retarget pointers */
			env->buf = added;
			env->buflen = n;
			for (i = 0, n = 0; i < env->varused; i++) {
				env->var[i] = added + n;
				n += strlen(added + n) + 1;
			}
			space = (env->buflen - env->bufused);
		}

		/* Make a pointer to the free space int the buffer */
		added = env->buf + env->bufused;

		/* Copy VARIABLE=VALUE\0 string into the free space */
		va_start(ap, fmt);
		mg_vsnprintf(env->conn, &truncated, added, space - 1, fmt, ap);
		va_end(ap);

		/* Do not add truncated strings to the environment */
		if (truncated) {
			/* Reallocate the buffer */
			space = 0;
		}
	} while (truncated);

	/* Calculate number of bytes added to the environment */
	n = strlen(added) + 1;
	env->bufused += n;

	/* Append a pointer to the added string into the envp array */
	env->var[env->varused] = added;
	env->varused++;
}

/* Return 0 on success, non-zero if an error occurs. */

static int
prepare_cgi_environment(struct mg_connection *conn,
                        const char *prog,
                        struct cgi_environment *env,
                        int cgi_config_idx)
{
	const char *s;
	struct vec var_vec;
	char *p, src_addr[IP_ADDR_STR_LEN], http_var_name[128];
	int i, truncated, uri_len;

	if ((conn == NULL) || (prog == NULL) || (env == NULL)) {
		return -1;
	}

	env->conn = conn;
	env->buflen = CGI_ENVIRONMENT_SIZE;
	env->bufused = 0;
	env->buf = (char *)mg_malloc_ctx(env->buflen, conn->phys_ctx);
	if (env->buf == NULL) {
		mg_cry_internal(conn,
		                "%s: Not enough memory for environmental buffer",
		                __func__);
		return -1;
	}
	env->varlen = MAX_CGI_ENVIR_VARS;
	env->varused = 0;
	env->var =
	    (char **)mg_malloc_ctx(env->varlen * sizeof(char *), conn->phys_ctx);
	if (env->var == NULL) {
		mg_cry_internal(conn,
		                "%s: Not enough memory for environmental variables",
		                __func__);
		mg_free(env->buf);
		return -1;
	}

	addenv(env, "SERVER_NAME=%s", conn->dom_ctx->config[AUTHENTICATION_DOMAIN]);
	addenv(env, "SERVER_ROOT=%s", conn->dom_ctx->config[DOCUMENT_ROOT]);
	addenv(env, "DOCUMENT_ROOT=%s", conn->dom_ctx->config[DOCUMENT_ROOT]);
	if (conn->dom_ctx->config[FALLBACK_DOCUMENT_ROOT]) {
		addenv(env,
		       "FALLBACK_DOCUMENT_ROOT=%s",
		       conn->dom_ctx->config[FALLBACK_DOCUMENT_ROOT]);
	}
	addenv(env, "SERVER_SOFTWARE=CivetWeb/%s", mg_version());

	/* Prepare the environment block */
	addenv(env, "%s", "GATEWAY_INTERFACE=CGI/1.1");
	addenv(env, "%s", "SERVER_PROTOCOL=HTTP/1.1");
	addenv(env, "%s", "REDIRECT_STATUS=200"); /* For PHP */

	addenv(env, "SERVER_PORT=%d", conn->request_info.server_port);

	sockaddr_to_string(src_addr, sizeof(src_addr), &conn->client.rsa);
	addenv(env, "REMOTE_ADDR=%s", src_addr);

	addenv(env, "REQUEST_METHOD=%s", conn->request_info.request_method);
	addenv(env, "REMOTE_PORT=%d", conn->request_info.remote_port);

	addenv(env, "REQUEST_URI=%s", conn->request_info.request_uri);
	addenv(env, "LOCAL_URI=%s", conn->request_info.local_uri);
	addenv(env, "LOCAL_URI_RAW=%s", conn->request_info.local_uri_raw);

	/* SCRIPT_NAME */
	uri_len = (int)strlen(conn->request_info.local_uri);
	if (conn->path_info == NULL) {
		if (conn->request_info.local_uri[uri_len - 1] != '/') {
			/* URI: /path_to_script/script.cgi */
			addenv(env, "SCRIPT_NAME=%s", conn->request_info.local_uri);
		} else {
			/* URI: /path_to_script/ ... using index.cgi */
			const char *index_file = strrchr(prog, '/');
			if (index_file) {
				addenv(env,
				       "SCRIPT_NAME=%s%s",
				       conn->request_info.local_uri,
				       index_file + 1);
			}
		}
	} else {
		/* URI: /path_to_script/script.cgi/path_info */
		addenv(env,
		       "SCRIPT_NAME=%.*s",
		       uri_len - (int)strlen(conn->path_info),
		       conn->request_info.local_uri);
	}

	addenv(env, "SCRIPT_FILENAME=%s", prog);
	if (conn->path_info == NULL) {
		addenv(env, "PATH_TRANSLATED=%s", conn->dom_ctx->config[DOCUMENT_ROOT]);
	} else {
		addenv(env,
		       "PATH_TRANSLATED=%s%s",
		       conn->dom_ctx->config[DOCUMENT_ROOT],
		       conn->path_info);
	}

	addenv(env, "HTTPS=%s", (conn->ssl == NULL) ? "off" : "on");

	if ((s = mg_get_header(conn, "Content-Type")) != NULL) {
		addenv(env, "CONTENT_TYPE=%s", s);
	}
	if (conn->request_info.query_string != NULL) {
		addenv(env, "QUERY_STRING=%s", conn->request_info.query_string);
	}
	if ((s = mg_get_header(conn, "Content-Length")) != NULL) {
		addenv(env, "CONTENT_LENGTH=%s", s);
	}
	if ((s = getenv("PATH")) != NULL) {
		addenv(env, "PATH=%s", s);
	}
	if (conn->path_info != NULL) {
		addenv(env, "PATH_INFO=%s", conn->path_info);
	}

	if (conn->status_code > 0) {
		/* CGI error handler should show the status code */
		addenv(env, "STATUS=%d", conn->status_code);
	}

#if defined(_WIN32)
	if ((s = getenv("COMSPEC")) != NULL) {
		addenv(env, "COMSPEC=%s", s);
	}
	if ((s = getenv("SYSTEMROOT")) != NULL) {
		addenv(env, "SYSTEMROOT=%s", s);
	}
	if ((s = getenv("SystemDrive")) != NULL) {
		addenv(env, "SystemDrive=%s", s);
	}
	if ((s = getenv("ProgramFiles")) != NULL) {
		addenv(env, "ProgramFiles=%s", s);
	}
	if ((s = getenv("ProgramFiles(x86)")) != NULL) {
		addenv(env, "ProgramFiles(x86)=%s", s);
	}
#else
	if ((s = getenv("LD_LIBRARY_PATH")) != NULL) {
		addenv(env, "LD_LIBRARY_PATH=%s", s);
	}
#endif /* _WIN32 */

	if ((s = getenv("PERLLIB")) != NULL) {
		addenv(env, "PERLLIB=%s", s);
	}

	if (conn->request_info.remote_user != NULL) {
		addenv(env, "REMOTE_USER=%s", conn->request_info.remote_user);
		addenv(env, "%s", "AUTH_TYPE=Digest");
	}

	/* Add all headers as HTTP_* variables */
	for (i = 0; i < conn->request_info.num_headers; i++) {

		(void)mg_snprintf(conn,
		                  &truncated,
		                  http_var_name,
		                  sizeof(http_var_name),
		                  "HTTP_%s",
		                  conn->request_info.http_headers[i].name);

		if (truncated) {
			mg_cry_internal(conn,
			                "%s: HTTP header variable too long [%s]",
			                __func__,
			                conn->request_info.http_headers[i].name);
			continue;
		}

		/* Convert variable name into uppercase, and change - to _ */
		for (p = http_var_name; *p != '\0'; p++) {
			if (*p == '-') {
				*p = '_';
			}
			*p = (char)toupper((unsigned char)*p);
		}

		addenv(env,
		       "%s=%s",
		       http_var_name,
		       conn->request_info.http_headers[i].value);
	}

	/* Add user-specified variables */
	s = conn->dom_ctx->config[CGI_ENVIRONMENT + cgi_config_idx];
	while ((s = next_option(s, &var_vec, NULL)) != NULL) {
		addenv(env, "%.*s", (int)var_vec.len, var_vec.ptr);
	}

	env->var[env->varused] = NULL;
	env->buf[env->bufused] = '\0';

	return 0;
}


/* Data for CGI process control: PID and number of references */
struct process_control_data {
	pid_t pid;
	ptrdiff_t references;
};

static int
abort_cgi_process(void *data)
{
	/* Waitpid checks for child status and won't work for a pid that does
	 * not identify a child of the current process. Thus, if the pid is
	 * reused, we will not affect a different process. */
	struct process_control_data *proc = (struct process_control_data *)data;
	int status = 0;
	ptrdiff_t refs;
	pid_t ret_pid;

	ret_pid = waitpid(proc->pid, &status, WNOHANG);
	if ((ret_pid != (pid_t)-1) && (status == 0)) {
		/* Stop child process */
		DEBUG_TRACE("CGI timer: Stop child process %d\n", proc->pid);
		kill(proc->pid, SIGABRT);

		/* Wait until process is terminated (don't leave zombies) */
		while (waitpid(proc->pid, &status, 0) != (pid_t)-1) /* nop */
			;
	} else {
		DEBUG_TRACE("CGI timer: Child process %d already stopped\n", proc->pid);
	}
	/* Dec reference counter */
	refs = mg_atomic_dec(&proc->references);
	if (refs == 0) {
		/* no more references - free data */
		mg_free(data);
	}

	return 0;
}


/* Local (static) function assumes all arguments are valid. */
static void
handle_cgi_request(struct mg_connection *conn,
                   const char *prog,
                   int cgi_config_idx)
{
	char *buf;
	size_t buflen;
	int headers_len, data_len, i, truncated;
	int fdin[2] = {-1, -1}, fdout[2] = {-1, -1}, fderr[2] = {-1, -1};
	const char *status, *status_text;
	char *pbuf, dir[UTF8_PATH_MAX], *p;
	struct mg_request_info ri;
	struct cgi_environment blk;
	FILE *in = NULL, *out = NULL, *err = NULL;
	struct mg_file fout = STRUCT_FILE_INITIALIZER;
	pid_t pid = (pid_t)-1;
	struct process_control_data *proc = NULL;
	char *cfg_buffering = conn->dom_ctx->config[CGI_BUFFERING + cgi_config_idx];
	int no_buffering = 0;

#if defined(USE_TIMERS)
	double cgi_timeout;
	if (conn->dom_ctx->config[CGI_TIMEOUT + cgi_config_idx]) {
		/* Get timeout in seconds */
		cgi_timeout =
		    atof(conn->dom_ctx->config[CGI_TIMEOUT + cgi_config_idx]) * 0.001;
	} else {
		cgi_timeout =
		    atof(config_options[REQUEST_TIMEOUT].default_value) * 0.001;
	}
#endif
	if (cfg_buffering != NULL) {
		if (!mg_strcasecmp(cfg_buffering, "no")) {
			no_buffering = 1;
		}
	}

	buf = NULL;
	buflen = conn->phys_ctx->max_request_size;
	i = prepare_cgi_environment(conn, prog, &blk, cgi_config_idx);
	if (i != 0) {
		blk.buf = NULL;
		blk.var = NULL;
		goto done;
	}

	/* CGI must be executed in its own directory. 'dir' must point to the
	 * directory containing executable program, 'p' must point to the
	 * executable program name relative to 'dir'. */
	(void)mg_snprintf(conn, &truncated, dir, sizeof(dir), "%s", prog);

	if (truncated) {
		mg_cry_internal(conn, "Error: CGI program \"%s\": Path too long", prog);
		mg_send_http_error(conn, 500, "Error: %s", "CGI path too long");
		goto done;
	}

	if ((p = strrchr(dir, '/')) != NULL) {
		*p++ = '\0';
	} else {
		dir[0] = '.';
		dir[1] = '\0';
		p = (char *)prog;
	}

	if ((pipe(fdin) != 0) || (pipe(fdout) != 0) || (pipe(fderr) != 0)) {
		status = strerror(ERRNO);
		mg_cry_internal(
		    conn,
		    "Error: CGI program \"%s\": Can not create CGI pipes: %s",
		    prog,
		    status);
		mg_send_http_error(conn,
		                   500,
		                   "Error: Cannot create CGI pipe: %s",
		                   status);
		goto done;
	}

	proc = (struct process_control_data *)
	    mg_malloc_ctx(sizeof(struct process_control_data), conn->phys_ctx);
	if (proc == NULL) {
		mg_cry_internal(conn, "Error: CGI program \"%s\": Out or memory", prog);
		mg_send_http_error(conn, 500, "Error: Out of memory [%s]", prog);
		goto done;
	}

	DEBUG_TRACE("CGI: spawn %s %s\n", dir, p);
	pid = spawn_process(
	    conn, p, blk.buf, blk.var, fdin, fdout, fderr, dir, cgi_config_idx);

	if (pid == (pid_t)-1) {
		status = strerror(ERRNO);
		mg_cry_internal(
		    conn,
		    "Error: CGI program \"%s\": Can not spawn CGI process: %s",
		    prog,
		    status);
		mg_send_http_error(conn, 500, "Error: Cannot spawn CGI process");
		mg_free(proc);
		proc = NULL;
		goto done;
	}

	/* Store data in shared process_control_data */
	proc->pid = pid;
	proc->references = 1;

#if defined(USE_TIMERS)
	if (cgi_timeout > 0.0) {
		proc->references = 2;

		// Start a timer for CGI
		timer_add(conn->phys_ctx,
		          cgi_timeout /* in seconds */,
		          0.0,
		          1,
		          abort_cgi_process,
		          (void *)proc,
		          NULL);
	}
#endif

	/* Parent closes only one side of the pipes.
	 * If we don't mark them as closed, close() attempt before
	 * return from this function throws an exception on Windows.
	 * Windows does not like when closed descriptor is closed again. */
	(void)close(fdin[0]);
	(void)close(fdout[1]);
	(void)close(fderr[1]);
	fdin[0] = fdout[1] = fderr[1] = -1;

	if (((in = fdopen(fdin[1], "wb")) == NULL)
	    || ((out = fdopen(fdout[0], "rb")) == NULL)
	    || ((err = fdopen(fderr[0], "rb")) == NULL)) {
		status = strerror(ERRNO);
		mg_cry_internal(conn,
		                "Error: CGI program \"%s\": Can not open fd: %s",
		                prog,
		                status);
		mg_send_http_error(conn,
		                   500,
		                   "Error: CGI can not open fd\nfdopen: %s",
		                   status);
		goto done;
	}

	setbuf(in, NULL);
	setbuf(out, NULL);
	setbuf(err, NULL);
	fout.access.fp = out;

	if ((conn->content_len != 0) || (conn->is_chunked)) {
		DEBUG_TRACE("CGI: send body data (%" INT64_FMT ")\n",
		            conn->content_len);

		/* This is a POST/PUT request, or another request with body data. */
		if (!forward_body_data(conn, in, INVALID_SOCKET, NULL)) {
			/* Error sending the body data */
			mg_cry_internal(
			    conn,
			    "Error: CGI program \"%s\": Forward body data failed",
			    prog);
			goto done;
		}
	}

	/* Close so child gets an EOF. */
	fclose(in);
	in = NULL;
	fdin[1] = -1;

	/* Now read CGI reply into a buffer. We need to set correct
	 * status code, thus we need to see all HTTP headers first.
	 * Do not send anything back to client, until we buffer in all
	 * HTTP headers. */
	data_len = 0;
	buf = (char *)mg_malloc_ctx(buflen, conn->phys_ctx);
	if (buf == NULL) {
		mg_send_http_error(conn,
		                   500,
		                   "Error: Not enough memory for CGI buffer (%u bytes)",
		                   (unsigned int)buflen);
		mg_cry_internal(
		    conn,
		    "Error: CGI program \"%s\": Not enough memory for buffer (%u "
		    "bytes)",
		    prog,
		    (unsigned int)buflen);
		goto done;
	}

	DEBUG_TRACE("CGI: %s", "wait for response");
	headers_len = read_message(out, conn, buf, (int)buflen, &data_len);
	DEBUG_TRACE("CGI: response: %li", (signed long)headers_len);

	if (headers_len <= 0) {

		/* Could not parse the CGI response. Check if some error message on
		 * stderr. */
		i = pull_all(err, conn, buf, (int)buflen);
		if (i > 0) {
			/* CGI program explicitly sent an error */
			/* Write the error message to the internal log */
			mg_cry_internal(conn,
			                "Error: CGI program \"%s\" sent error "
			                "message: [%.*s]",
			                prog,
			                i,
			                buf);
			/* Don't send the error message back to the client */
			mg_send_http_error(conn,
			                   500,
			                   "Error: CGI program \"%s\" failed.",
			                   prog);
		} else {
			/* CGI program did not explicitly send an error, but a broken
			 * respon header */
			mg_cry_internal(conn,
			                "Error: CGI program sent malformed or too big "
			                "(>%u bytes) HTTP headers: [%.*s]",
			                (unsigned)buflen,
			                data_len,
			                buf);

			mg_send_http_error(conn,
			                   500,
			                   "Error: CGI program sent malformed or too big "
			                   "(>%u bytes) HTTP headers: [%.*s]",
			                   (unsigned)buflen,
			                   data_len,
			                   buf);
		}

		/* in both cases, abort processing CGI */
		goto done;
	}

	pbuf = buf;
	buf[headers_len - 1] = '\0';
	ri.num_headers = parse_http_headers(&pbuf, ri.http_headers);

	/* Make up and send the status line */
	status_text = "OK";
	if ((status = get_header(ri.http_headers, ri.num_headers, "Status"))
	    != NULL) {
		conn->status_code = atoi(status);
		status_text = status;
		while (isdigit((unsigned char)*status_text) || *status_text == ' ') {
			status_text++;
		}
	} else if (get_header(ri.http_headers, ri.num_headers, "Location")
	           != NULL) {
		conn->status_code = 307;
	} else {
		conn->status_code = 200;
	}

	if (!should_keep_alive(conn)) {
		conn->must_close = 1;
	}

	DEBUG_TRACE("CGI: response %u %s", conn->status_code, status_text);

	(void)mg_printf(conn, "HTTP/1.1 %d %s\r\n", conn->status_code, status_text);

	/* Send headers */
	for (i = 0; i < ri.num_headers; i++) {
		DEBUG_TRACE("CGI header: %s: %s",
		            ri.http_headers[i].name,
		            ri.http_headers[i].value);
		mg_printf(conn,
		          "%s: %s\r\n",
		          ri.http_headers[i].name,
		          ri.http_headers[i].value);
	}
	mg_write(conn, "\r\n", 2);

	/* Send chunk of data that may have been read after the headers */
	mg_write(conn, buf + headers_len, (size_t)(data_len - headers_len));

	/* Read the rest of CGI output and send to the client */
	DEBUG_TRACE("CGI: %s", "forward all data");
	send_file_data(conn, &fout, 0, INT64_MAX, no_buffering); /* send CGI data */
	DEBUG_TRACE("CGI: %s", "all data sent");

done:
	mg_free(blk.var);
	mg_free(blk.buf);

	if (pid != (pid_t)-1) {
		abort_cgi_process((void *)proc);
	}

	if (fdin[0] != -1) {
		close(fdin[0]);
	}
	if (fdout[1] != -1) {
		close(fdout[1]);
	}
	if (fderr[1] != -1) {
		close(fderr[1]);
	}

	if (in != NULL) {
		fclose(in);
	} else if (fdin[1] != -1) {
		close(fdin[1]);
	}

	if (out != NULL) {
		fclose(out);
	} else if (fdout[0] != -1) {
		close(fdout[0]);
	}

	if (err != NULL) {
		fclose(err);
	} else if (fderr[0] != -1) {
		close(fderr[0]);
	}

	mg_free(buf);
}
#endif /* !NO_CGI */


#if !defined(NO_FILES)
static void
dav_mkcol(struct mg_connection *conn, const char *path)
{
	int rc, body_len;
	struct de de;

	if (conn == NULL) {
		return;
	}

	/* TODO (mid): Check the mg_send_http_error situations in this function
	 */

	memset(&de.file, 0, sizeof(de.file));
	if (!mg_stat(conn, path, &de.file)) {
		mg_cry_internal(conn,
		                "%s: mg_stat(%s) failed: %s",
		                __func__,
		                path,
		                strerror(ERRNO));
	}

	if (de.file.last_modified) {
		/* TODO (mid): This check does not seem to make any sense ! */
		/* TODO (mid): Add a webdav unit test first, before changing
		 * anything here. */
		mg_send_http_error(
		    conn, 405, "Error: mkcol(%s): %s", path, strerror(ERRNO));
		return;
	}

	body_len = conn->data_len - conn->request_len;
	if (body_len > 0) {
		mg_send_http_error(
		    conn, 415, "Error: mkcol(%s): %s", path, strerror(ERRNO));
		return;
	}

	rc = mg_mkdir(conn, path, 0755);
	DEBUG_TRACE("mkdir %s: %i", path, rc);
	if (rc == 0) {
		/* Create 201 "Created" response */
		mg_response_header_start(conn, 201);
		send_static_cache_header(conn);
		send_additional_header(conn);
		mg_response_header_add(conn, "Content-Length", "0", -1);

		/* Send all headers - there is no body */
		mg_response_header_send(conn);
	} else {
		int http_status = 500;
		switch (errno) {
		case EEXIST:
			http_status = 405;
			break;
		case EACCES:
			http_status = 403;
			break;
		case ENOENT:
			http_status = 409;
			break;
		}

		mg_send_http_error(conn,
		                   http_status,
		                   "Error processing %s: %s",
		                   path,
		                   strerror(ERRNO));
	}
}


/* Forward decrlaration */
static int get_uri_type(const char *uri);
static const char *
get_rel_url_at_current_server(const char *uri,
                              const struct mg_connection *conn);


static void
dav_move_file(struct mg_connection *conn, const char *path, int do_copy)
{
	const char *overwrite_hdr;
	const char *destination_hdr;
	const char *root;
	int rc, dest_uri_type;
	int http_status = 400;
	int do_overwrite = 0;
	int destination_ok = 0;
	char dest_path[UTF8_PATH_MAX];
	struct mg_file_stat ignored;

	if (conn == NULL) {
		return;
	}

	root = conn->dom_ctx->config[DOCUMENT_ROOT];
	overwrite_hdr = mg_get_header(conn, "Overwrite");
	destination_hdr = mg_get_header(conn, "Destination");
	if ((overwrite_hdr != NULL) && (toupper(overwrite_hdr[0]) == 'T')) {
		do_overwrite = 1;
	}

	if ((destination_hdr == NULL) || (destination_hdr[0] == 0)) {
		mg_send_http_error(conn, 400, "%s", "Missing destination");
		return;
	}

	if (root != NULL) {
		char *local_dest = NULL;
		dest_uri_type = get_uri_type(destination_hdr);
		if (dest_uri_type == 2) {
			local_dest = mg_strdup_ctx(destination_hdr, conn->phys_ctx);
		} else if ((dest_uri_type == 3) || (dest_uri_type == 4)) {
			const char *h =
			    get_rel_url_at_current_server(destination_hdr, conn);
			if (h) {
				size_t len = strlen(h);
				local_dest = mg_malloc_ctx(len + 1, conn->phys_ctx);
				mg_url_decode(h, (int)len, local_dest, (int)len + 1, 0);
			}
		}
		if (local_dest != NULL) {
			remove_dot_segments(local_dest);
			if (local_dest[0] == '/') {
				int trunc_check = 0;
				mg_snprintf(conn,
				            &trunc_check,
				            dest_path,
				            sizeof(dest_path),
				            "%s/%s",
				            root,
				            local_dest);
				if (trunc_check == 0) {
					destination_ok = 1;
				}
			}
			mg_free(local_dest);
		}
	}

	if (!destination_ok) {
		mg_send_http_error(conn, 502, "%s", "Illegal destination");
		return;
	}

	/* Check now if this file exists */
	if (mg_stat(conn, dest_path, &ignored)) {
		/* File exists */
		if (do_overwrite) {
			/* Overwrite allowed: delete the file first */
			if (0 != remove(dest_path)) {
				/* No overwrite: return error */
				mg_send_http_error(conn,
				                   403,
				                   "Cannot overwrite file: %s",
				                   dest_path);
				return;
			}
		} else {
			/* No overwrite: return error */
			mg_send_http_error(conn,
			                   412,
			                   "Destination already exists: %s",
			                   dest_path);
			return;
		}
	}

	/* Copy / Move / Rename operation. */
	DEBUG_TRACE("%s %s to %s", (do_copy ? "copy" : "move"), path, dest_path);
#if defined(_WIN32)
	{
		/* For Windows, we need to convert from UTF-8 to UTF-16 first. */
		wchar_t wSource[UTF16_PATH_MAX];
		wchar_t wDest[UTF16_PATH_MAX];
		BOOL ok;

		path_to_unicode(conn, path, wSource, ARRAY_SIZE(wSource));
		path_to_unicode(conn, dest_path, wDest, ARRAY_SIZE(wDest));
		if (do_copy) {
			ok = CopyFileW(wSource, wDest, do_overwrite ? FALSE : TRUE);
		} else {
			ok = MoveFileExW(wSource,
			                 wDest,
			                 do_overwrite ? MOVEFILE_REPLACE_EXISTING : 0);
		}
		if (ok) {
			rc = 0;
		} else {
			DWORD lastErr = GetLastError();
			if (lastErr == ERROR_ALREADY_EXISTS) {
				mg_send_http_error(conn,
				                   412,
				                   "Destination already exists: %s",
				                   dest_path);
				return;
			}
			rc = -1;
			http_status = 400;
		}
	}

#else
	{
		/* Linux uses already UTF-8, we don't need to convert file names. */

		if (do_copy) {
			/* TODO: COPY for Linux. */
			mg_send_http_error(conn, 403, "%s", "COPY forbidden");
			return;
		}

		rc = rename(path, dest_path);
		if (rc) {
			switch (errno) {
			case EEXIST:
				http_status = 412;
				break;
			case EACCES:
				http_status = 403;
				break;
			case ENOENT:
				http_status = 409;
				break;
			}
		}
	}
#endif

	if (rc == 0) {
		/* Create 204 "No Content" response */
		mg_response_header_start(conn, 204);
		mg_response_header_add(conn, "Content-Length", "0", -1);

		/* Send all headers - there is no body */
		mg_response_header_send(conn);
	} else {
		mg_send_http_error(conn, http_status, "Operation failed");
	}
}


static void
put_file(struct mg_connection *conn, const char *path)
{
	struct mg_file file = STRUCT_FILE_INITIALIZER;
	const char *range;
	int64_t r1, r2;
	int rc;

	if (conn == NULL) {
		return;
	}

	DEBUG_TRACE("store %s", path);

	if (mg_stat(conn, path, &file.stat)) {
		/* File already exists */
		conn->status_code = 200;

		if (file.stat.is_directory) {
			/* This is an already existing directory,
			 * so there is nothing to do for the server. */
			rc = 0;

		} else {
			/* File exists and is not a directory. */
			/* Can it be replaced? */

			/* Check if the server may write this file */
			if (access(path, W_OK) == 0) {
				/* Access granted */
				rc = 1;
			} else {
				mg_send_http_error(
				    conn,
				    403,
				    "Error: Put not possible\nReplacing %s is not allowed",
				    path);
				return;
			}
		}
	} else {
		/* File should be created */
		conn->status_code = 201;
		rc = put_dir(conn, path);
	}

	if (rc == 0) {
		/* put_dir returns 0 if path is a directory */

		/* Create response */
		mg_response_header_start(conn, conn->status_code);
		send_no_cache_header(conn);
		send_additional_header(conn);
		mg_response_header_add(conn, "Content-Length", "0", -1);

		/* Send all headers - there is no body */
		mg_response_header_send(conn);

		/* Request to create a directory has been fulfilled successfully.
		 * No need to put a file. */
		return;
	}

	if (rc == -1) {
		/* put_dir returns -1 if the path is too long */
		mg_send_http_error(conn,
		                   414,
		                   "Error: Path too long\nput_dir(%s): %s",
		                   path,
		                   strerror(ERRNO));
		return;
	}

	if (rc == -2) {
		/* put_dir returns -2 if the directory can not be created */
		mg_send_http_error(conn,
		                   500,
		                   "Error: Can not create directory\nput_dir(%s): %s",
		                   path,
		                   strerror(ERRNO));
		return;
	}

	/* A file should be created or overwritten. */
	/* Currently CivetWeb does not need read+write access. */
	if (!mg_fopen(conn, path, MG_FOPEN_MODE_WRITE, &file)
	    || file.access.fp == NULL) {
		(void)mg_fclose(&file.access);
		mg_send_http_error(conn,
		                   500,
		                   "Error: Can not create file\nfopen(%s): %s",
		                   path,
		                   strerror(ERRNO));
		return;
	}

	fclose_on_exec(&file.access, conn);
	range = mg_get_header(conn, "Content-Range");
	r1 = r2 = 0;
	if ((range != NULL) && parse_range_header(range, &r1, &r2) > 0) {
		conn->status_code = 206; /* Partial content */
		if (0 != fseeko(file.access.fp, r1, SEEK_SET)) {
			mg_send_http_error(conn,
			                   500,
			                   "Error: Internal error processing file %s",
			                   path);
			return;
		}
	}

	if (!forward_body_data(conn, file.access.fp, INVALID_SOCKET, NULL)) {
		/* forward_body_data failed.
		 * The error code has already been sent to the client,
		 * and conn->status_code is already set. */
		(void)mg_fclose(&file.access);
		return;
	}

	if (mg_fclose(&file.access) != 0) {
		/* fclose failed. This might have different reasons, but a likely
		 * one is "no space on disk", http 507. */
		conn->status_code = 507;
	}

	/* Create response (status_code has been set before) */
	mg_response_header_start(conn, conn->status_code);
	send_no_cache_header(conn);
	send_additional_header(conn);
	mg_response_header_add(conn, "Content-Length", "0", -1);

	/* Send all headers - there is no body */
	mg_response_header_send(conn);
}


static void
delete_file(struct mg_connection *conn, const char *path)
{
	struct de de;
	memset(&de.file, 0, sizeof(de.file));
	if (!mg_stat(conn, path, &de.file)) {
		/* mg_stat returns 0 if the file does not exist */
		mg_send_http_error(conn,
		                   404,
		                   "Error: Cannot delete file\nFile %s not found",
		                   path);
		return;
	}

	DEBUG_TRACE("delete %s", path);

	if (de.file.is_directory) {
		if (remove_directory(conn, path)) {
			/* Delete is successful: Return 204 without content. */
			mg_send_http_error(conn, 204, "%s", "");
		} else {
			/* Delete is not successful: Return 500 (Server error). */
			mg_send_http_error(conn, 500, "Error: Could not delete %s", path);
		}
		return;
	}

	/* This is an existing file (not a directory).
	 * Check if write permission is granted. */
	if (access(path, W_OK) != 0) {
		/* File is read only */
		mg_send_http_error(
		    conn,
		    403,
		    "Error: Delete not possible\nDeleting %s is not allowed",
		    path);
		return;
	}

	/* Try to delete it. */
	if (mg_remove(conn, path) == 0) {
		/* Delete was successful: Return 204 without content. */
		mg_response_header_start(conn, 204);
		send_no_cache_header(conn);
		send_additional_header(conn);
		mg_response_header_add(conn, "Content-Length", "0", -1);
		mg_response_header_send(conn);

	} else {
		/* Delete not successful (file locked). */
		mg_send_http_error(conn,
		                   423,
		                   "Error: Cannot delete file\nremove(%s): %s",
		                   path,
		                   strerror(ERRNO));
	}
}
#endif /* !NO_FILES */


#if !defined(NO_FILESYSTEMS)
static void
send_ssi_file(struct mg_connection *, const char *, struct mg_file *, int);


static void
do_ssi_include(struct mg_connection *conn,
               const char *ssi,
               char *tag,
               int include_level)
{
	char file_name[MG_BUF_LEN], path[512], *p;
	struct mg_file file = STRUCT_FILE_INITIALIZER;
	size_t len;
	int truncated = 0;

	if (conn == NULL) {
		return;
	}

	/* sscanf() is safe here, since send_ssi_file() also uses buffer
	 * of size MG_BUF_LEN to get the tag. So strlen(tag) is
	 * always < MG_BUF_LEN. */
	if (sscanf(tag, " virtual=\"%511[^\"]\"", file_name) == 1) {
		/* File name is relative to the webserver root */
		file_name[511] = 0;
		(void)mg_snprintf(conn,
		                  &truncated,
		                  path,
		                  sizeof(path),
		                  "%s/%s",
		                  conn->dom_ctx->config[DOCUMENT_ROOT],
		                  file_name);

	} else if (sscanf(tag, " abspath=\"%511[^\"]\"", file_name) == 1) {
		/* File name is relative to the webserver working directory
		 * or it is absolute system path */
		file_name[511] = 0;
		(void)
		    mg_snprintf(conn, &truncated, path, sizeof(path), "%s", file_name);

	} else if ((sscanf(tag, " file=\"%511[^\"]\"", file_name) == 1)
	           || (sscanf(tag, " \"%511[^\"]\"", file_name) == 1)) {
		/* File name is relative to the current document */
		file_name[511] = 0;
		(void)mg_snprintf(conn, &truncated, path, sizeof(path), "%s", ssi);

		if (!truncated) {
			if ((p = strrchr(path, '/')) != NULL) {
				p[1] = '\0';
			}
			len = strlen(path);
			(void)mg_snprintf(conn,
			                  &truncated,
			                  path + len,
			                  sizeof(path) - len,
			                  "%s",
			                  file_name);
		}

	} else {
		mg_cry_internal(conn, "Bad SSI #include: [%s]", tag);
		return;
	}

	if (truncated) {
		mg_cry_internal(conn, "SSI #include path length overflow: [%s]", tag);
		return;
	}

	if (!mg_fopen(conn, path, MG_FOPEN_MODE_READ, &file)) {
		mg_cry_internal(conn,
		                "Cannot open SSI #include: [%s]: fopen(%s): %s",
		                tag,
		                path,
		                strerror(ERRNO));
	} else {
		fclose_on_exec(&file.access, conn);
		if (match_prefix_strlen(conn->dom_ctx->config[SSI_EXTENSIONS], path)
		    > 0) {
			send_ssi_file(conn, path, &file, include_level + 1);
		} else {
			send_file_data(conn, &file, 0, INT64_MAX, 0); /* send static file */
		}
		(void)mg_fclose(&file.access); /* Ignore errors for readonly files */
	}
}


#if !defined(NO_POPEN)
static void
do_ssi_exec(struct mg_connection *conn, char *tag)
{
	char cmd[1024] = "";
	struct mg_file file = STRUCT_FILE_INITIALIZER;

	if (sscanf(tag, " \"%1023[^\"]\"", cmd) != 1) {
		mg_cry_internal(conn, "Bad SSI #exec: [%s]", tag);
	} else {
		cmd[1023] = 0;
		if ((file.access.fp = popen(cmd, "r")) == NULL) {
			mg_cry_internal(conn,
			                "Cannot SSI #exec: [%s]: %s",
			                cmd,
			                strerror(ERRNO));
		} else {
			send_file_data(conn, &file, 0, INT64_MAX, 0); /* send static file */
			pclose(file.access.fp);
		}
	}
}
#endif /* !NO_POPEN */


static int
mg_fgetc(struct mg_file *filep)
{
	if (filep == NULL) {
		return EOF;
	}

	if (filep->access.fp != NULL) {
		return fgetc(filep->access.fp);
	} else {
		return EOF;
	}
}


static void
send_ssi_file(struct mg_connection *conn,
              const char *path,
              struct mg_file *filep,
              int include_level)
{
	char buf[MG_BUF_LEN];
	int ch, len, in_tag, in_ssi_tag;

	if (include_level > 10) {
		mg_cry_internal(conn, "SSI #include level is too deep (%s)", path);
		return;
	}

	in_tag = in_ssi_tag = len = 0;

	/* Read file, byte by byte, and look for SSI include tags */
	while ((ch = mg_fgetc(filep)) != EOF) {

		if (in_tag) {
			/* We are in a tag, either SSI tag or html tag */

			if (ch == '>') {
				/* Tag is closing */
				buf[len++] = '>';

				if (in_ssi_tag) {
					/* Handle SSI tag */
					buf[len] = 0;

					if ((len > 12) && !memcmp(buf + 5, "include", 7)) {
						do_ssi_include(conn, path, buf + 12, include_level + 1);
#if !defined(NO_POPEN)
					} else if ((len > 9) && !memcmp(buf + 5, "exec", 4)) {
						do_ssi_exec(conn, buf + 9);
#endif /* !NO_POPEN */
					} else {
						mg_cry_internal(conn,
						                "%s: unknown SSI "
						                "command: \"%s\"",
						                path,
						                buf);
					}
					len = 0;
					in_ssi_tag = in_tag = 0;

				} else {
					/* Not an SSI tag */
					/* Flush buffer */
					(void)mg_write(conn, buf, (size_t)len);
					len = 0;
					in_tag = 0;
				}

			} else {
				/* Tag is still open */
				buf[len++] = (char)(ch & 0xff);

				if ((len == 5) && !memcmp(buf, "<!--#", 5)) {
					/* All SSI tags start with <!--# */
					in_ssi_tag = 1;
				}

				if ((len + 2) > (int)sizeof(buf)) {
					/* Tag to long for buffer */
					mg_cry_internal(conn, "%s: tag is too large", path);
					return;
				}
			}

		} else {

			/* We are not in a tag yet. */
			if (ch == '<') {
				/* Tag is opening */
				in_tag = 1;

				if (len > 0) {
					/* Flush current buffer.
					 * Buffer is filled with "len" bytes. */
					(void)mg_write(conn, buf, (size_t)len);
				}
				/* Store the < */
				len = 1;
				buf[0] = '<';

			} else {
				/* No Tag */
				/* Add data to buffer */
				buf[len++] = (char)(ch & 0xff);
				/* Flush if buffer is full */
				if (len == (int)sizeof(buf)) {
					mg_write(conn, buf, (size_t)len);
					len = 0;
				}
			}
		}
	}

	/* Send the rest of buffered data */
	if (len > 0) {
		mg_write(conn, buf, (size_t)len);
	}
}


static void
handle_ssi_file_request(struct mg_connection *conn,
                        const char *path,
                        struct mg_file *filep)
{
	char date[64];
	time_t curtime = time(NULL);

	if ((conn == NULL) || (path == NULL) || (filep == NULL)) {
		return;
	}

	if (!mg_fopen(conn, path, MG_FOPEN_MODE_READ, filep)) {
		/* File exists (precondition for calling this function),
		 * but can not be opened by the server. */
		mg_send_http_error(conn,
		                   500,
		                   "Error: Cannot read file\nfopen(%s): %s",
		                   path,
		                   strerror(ERRNO));
	} else {
		/* Set "must_close" for HTTP/1.x, since we do not know the
		 * content length */
		conn->must_close = 1;
		gmt_time_string(date, sizeof(date), &curtime);
		fclose_on_exec(&filep->access, conn);

		/* 200 OK response */
		mg_response_header_start(conn, 200);
		send_no_cache_header(conn);
		send_additional_header(conn);
		send_cors_header(conn);
		mg_response_header_add(conn, "Content-Type", "text/html", -1);
		mg_response_header_send(conn);

		/* Header sent, now send body */
		send_ssi_file(conn, path, filep, 0);
		(void)mg_fclose(&filep->access); /* Ignore errors for readonly files */
	}
}
#endif /* NO_FILESYSTEMS */


#if !defined(NO_FILES)
static void
send_options(struct mg_connection *conn)
{
	if (!conn || !all_methods) {
		return;
	}

	/* We do not set a "Cache-Control" header here, but leave the default.
	 * Since browsers do not send an OPTIONS request, we can not test the
	 * effect anyway. */

	mg_response_header_start(conn, 200);
	mg_response_header_add(conn, "Content-Type", "text/html", -1);

	if (conn->protocol_type == PROTOCOL_TYPE_HTTP1) {
		/* Use the same as before */
		mg_response_header_add(conn, "Allow", all_methods, -1);
		mg_response_header_add(conn, "DAV", "1", -1);
	} else {
		/* TODO: Check this later for HTTP/2 */
		mg_response_header_add(conn, "Allow", "GET, POST", -1);
	}
	send_additional_header(conn);
	mg_response_header_send(conn);
}


/* Writes PROPFIND properties for a collection element */
static int
print_props(struct mg_connection *conn,
            const char *uri,
            const char *name,
            struct mg_file_stat *filep)
{
	size_t i;
	char mtime[64];
	char link_buf[UTF8_PATH_MAX * 2]; /* Path + server root */
	char *link_concat;
	size_t link_concat_len;

	if ((conn == NULL) || (uri == NULL) || (name == NULL) || (filep == NULL)) {
		return 0;
	}

	link_concat_len = strlen(uri) + strlen(name) + 1;
	link_concat = mg_malloc_ctx(link_concat_len, conn->phys_ctx);
	if (!link_concat) {
		return 0;
	}
	strcpy(link_concat, uri);
	strcat(link_concat, name);

	/* Get full link used in request */
	mg_construct_local_link(
	    conn, link_buf, sizeof(link_buf), NULL, 0, link_concat);

	/*
	OutputDebugStringA("print_props:\n  uri: ");
	OutputDebugStringA(uri);
	OutputDebugStringA("\n  name: ");
	OutputDebugStringA(name);
	OutputDebugStringA("\n  link: ");
	OutputDebugStringA(link_buf);
	OutputDebugStringA("\n");
	*/

	gmt_time_string(mtime, sizeof(mtime), &filep->last_modified);
	mg_printf(conn,
	          "<d:response>"
	          "<d:href>%s</d:href>"
	          "<d:propstat>"
	          "<d:prop>"
	          "<d:resourcetype>%s</d:resourcetype>"
	          "<d:getcontentlength>%" INT64_FMT "</d:getcontentlength>"
	          "<d:getlastmodified>%s</d:getlastmodified>"
	          "<d:lockdiscovery>",
	          link_buf,
	          filep->is_directory ? "<d:collection/>" : "",
	          filep->size,
	          mtime);

	for (i = 0; i < NUM_WEBDAV_LOCKS; i++) {
		struct twebdav_lock *dav_lock = conn->phys_ctx->webdav_lock;
		if (!strcmp(dav_lock[i].path, link_buf)) {
			mg_printf(conn,
			          "<d:activelock>"
			          "<d:locktype><d:write/></d:locktype>"
			          "<d:lockscope><d:exclusive/></d:lockscope>"
			          "<d:depth>0</d:depth>"
			          "<d:owner>%s</d:owner>"
			          "<d:timeout>Second-%u</d:timeout>"
			          "<d:locktoken>"
			          "<d:href>%s</d:href>"
			          "</d:locktoken>"
			          "</d:activelock>\n",
			          dav_lock[i].user,
			          (unsigned)LOCK_DURATION_S,
			          dav_lock[i].token);
		}
	}

	mg_printf(conn,
	          "</d:lockdiscovery>"
	          "</d:prop>"
	          "<d:status>HTTP/1.1 200 OK</d:status>"
	          "</d:propstat>"
	          "</d:response>\n");

	mg_free(link_concat);
	return 1;
}


static int
print_dav_dir_entry(struct de *de, void *data)
{
	struct mg_connection *conn = (struct mg_connection *)data;
	if (!de || !conn
	    || !print_props(
	           conn, conn->request_info.local_uri, de->file_name, &de->file)) {
		/* stop scan */
		return 1;
	}
	return 0;
}


static void
handle_propfind(struct mg_connection *conn,
                const char *path,
                struct mg_file_stat *filep)
{
	const char *depth = mg_get_header(conn, "Depth");

	if (!conn || !path || !filep || !conn->dom_ctx) {
		return;
	}

	/* return 207 "Multi-Status" */
	conn->must_close = 1;
	mg_response_header_start(conn, 207);
	send_static_cache_header(conn);
	send_additional_header(conn);
	mg_response_header_add(conn,
	                       "Content-Type",
	                       "application/xml; charset=utf-8",
	                       -1);
	mg_response_header_send(conn);

	/* Content */
	mg_printf(conn,
	          "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	          "<d:multistatus xmlns:d='DAV:'>\n");

	/* Print properties for the requested resource itself */
	print_props(conn, conn->request_info.local_uri, "", filep);

	/* If it is a directory, print directory entries too if Depth is not 0
	 */
	if (filep->is_directory
	    && !mg_strcasecmp(conn->dom_ctx->config[ENABLE_DIRECTORY_LISTING],
	                      "yes")
	    && ((depth == NULL) || (strcmp(depth, "0") != 0))) {
		scan_directory(conn, path, conn, &print_dav_dir_entry);
	}

	mg_printf(conn, "%s\n", "</d:multistatus>");
}


static void
dav_lock_file(struct mg_connection *conn, const char *path)
{
	/* internal function - therefore conn is assumed to be valid */
	char link_buf[UTF8_PATH_MAX * 2]; /* Path + server root */
	uint64_t new_locktime;
	int lock_index = -1;
	int i;
	uint64_t LOCK_DURATION_NS =
	    (uint64_t)(LOCK_DURATION_S) * (uint64_t)1000000000;
	struct twebdav_lock *dav_lock = NULL;

	if (!path || !conn || !conn->dom_ctx || !conn->request_info.remote_user
	    || !conn->phys_ctx) {
		return;
	}

	dav_lock = conn->phys_ctx->webdav_lock;
	mg_get_request_link(conn, link_buf, sizeof(link_buf));

	/* const char *refresh = mg_get_header(conn, "If"); */
	/* Link refresh should have an "If" header:
	 * http://www.webdav.org/specs/rfc2518.html#n-example---refreshing-a-write-lock
	 * But it seems Windows Explorer does not send them.
	 */

	mg_lock_context(conn->phys_ctx);
	new_locktime = mg_get_current_time_ns();

	/* Find a slot for a lock */
	while (lock_index < 0) {
		/* find existing lock */
		for (i = 0; i < NUM_WEBDAV_LOCKS; i++) {
			if (!strcmp(dav_lock[i].path, link_buf)) {
				if (!strcmp(conn->request_info.remote_user, dav_lock[i].user)) {
					/* locked by the same user */
					dav_lock[i].locktime = new_locktime;
					lock_index = i;
					break;
				} else {
					/* already locked by someone else */
					if (new_locktime
					    > (dav_lock[i].locktime + LOCK_DURATION_NS)) {
						/* Lock expired */
						dav_lock[i].path[0] = 0;
					} else {
						/* Lock still valid */
						mg_unlock_context(conn->phys_ctx);
						mg_send_http_error(conn, 423, "%s", "Already locked");
						return;
					}
				}
			}
		}

		/* create new lock token */
		for (i = 0; i < NUM_WEBDAV_LOCKS; i++) {
			if (dav_lock[i].path[0] == 0) {
				char s[32];
				dav_lock[i].locktime = mg_get_current_time_ns();
				sprintf(s, "%" UINT64_FMT, (uint64_t)dav_lock[i].locktime);
				mg_md5(dav_lock[i].token,
				       link_buf,
				       "\x01",
				       s,
				       "\x01",
				       conn->request_info.remote_user,
				       NULL);
				mg_strlcpy(dav_lock[i].path,
				           link_buf,
				           sizeof(dav_lock[i].path));
				mg_strlcpy(dav_lock[i].user,
				           conn->request_info.remote_user,
				           sizeof(dav_lock[i].user));
				lock_index = i;
				break;
			}
		}
		if (lock_index < 0) {
			/* too many locks. Find oldest lock */
			uint64_t oldest_locktime = dav_lock[0].locktime;
			lock_index = 0;
			for (i = 1; i < NUM_WEBDAV_LOCKS; i++) {
				if (dav_lock[i].locktime < oldest_locktime) {
					oldest_locktime = dav_lock[i].locktime;
					lock_index = i;
				}
			}
			/* invalidate oldest lock */
			dav_lock[lock_index].path[0] = 0;
		}
	}
	mg_unlock_context(conn->phys_ctx);

	/* return 200 "OK" */
	conn->must_close = 1;
	mg_response_header_start(conn, 200);
	send_static_cache_header(conn);
	send_additional_header(conn);
	mg_response_header_add(conn,
	                       "Content-Type",
	                       "application/xml; charset=utf-8",
	                       -1);
	mg_response_header_add(conn, "Lock-Token", dav_lock[lock_index].token, -1);
	mg_response_header_send(conn);

	/* Content */
	mg_printf(conn,
	          "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	          "<d:prop xmlns:d=\"DAV:\">\n"
	          "     <d:lockdiscovery>\n"
	          "       <d:activelock>\n"
	          "         <d:lockscope><d:exclusive/></d:lockscope>\n"
	          "         <d:locktype><d:write/></d:locktype>\n"
	          "         <d:owner>\n"
	          "           <d:href>%s</d:href>\n"
	          "         </d:owner>\n"
	          "         <d:timeout>Second-%u</d:timeout>\n"
	          "         <d:locktoken><d:href>%s</d:href></d:locktoken>\n"
	          "         <d:lockroot>\n"
	          "           <d:href>%s</d:href>\n"
	          "         </d:lockroot>\n"
	          "       </d:activelock>\n"
	          "     </d:lockdiscovery>\n"
	          "   </d:prop>\n",
	          dav_lock[lock_index].user,
	          (LOCK_DURATION_S),
	          dav_lock[lock_index].token,
	          dav_lock[lock_index].path);
}


static void
dav_unlock_file(struct mg_connection *conn, const char *path)
{
	/* internal function - therefore conn is assumed to be valid */
	char link_buf[UTF8_PATH_MAX * 2]; /* Path + server root */
	struct twebdav_lock *dav_lock = conn->phys_ctx->webdav_lock;
	int lock_index;

	if (!path || !conn->dom_ctx || !conn->request_info.remote_user) {
		return;
	}

	mg_get_request_link(conn, link_buf, sizeof(link_buf));

	mg_lock_context(conn->phys_ctx);
	/* find existing lock */
	for (lock_index = 0; lock_index < NUM_WEBDAV_LOCKS; lock_index++) {
		if (!strcmp(dav_lock[lock_index].path, link_buf)) {
			/* Success: return 204 "No Content" */
			mg_unlock_context(conn->phys_ctx);
			conn->must_close = 1;
			mg_response_header_start(conn, 204);
			mg_response_header_send(conn);
			return;
		}
	}
	mg_unlock_context(conn->phys_ctx);

	/* Error: Cannot unlock a resource that is not locked */
	mg_send_http_error(conn, 423, "%s", "Lock not found");
}


static void
dav_proppatch(struct mg_connection *conn, const char *path)
{
	char link_buf[UTF8_PATH_MAX * 2]; /* Path + server root */

	if (!conn || !path || !conn->dom_ctx) {
		return;
	}

	/* return 207 "Multi-Status" */
	conn->must_close = 1;
	mg_response_header_start(conn, 207);
	send_static_cache_header(conn);
	send_additional_header(conn);
	mg_response_header_add(conn,
	                       "Content-Type",
	                       "application/xml; charset=utf-8",
	                       -1);
	mg_response_header_send(conn);

	mg_get_request_link(conn, link_buf, sizeof(link_buf));

	/* Content */
	mg_printf(conn,
	          "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	          "<d:multistatus xmlns:d='DAV:'>\n"
	          "<d:response>\n<d:href>%s</d:href>\n",
	          link_buf);
	mg_printf(conn,
	          "<d:propstat><d:status>HTTP/1.1 403 "
	          "Forbidden</d:status></d:propstat>\n");
	mg_printf(conn, "%s\n", "</d:response></d:multistatus>");
}
#endif


CIVETWEB_API void
mg_lock_connection(struct mg_connection *conn)
{
	if (conn) {
		(void)pthread_mutex_lock(&conn->mutex);
	}
}


CIVETWEB_API void
mg_unlock_connection(struct mg_connection *conn)
{
	if (conn) {
		(void)pthread_mutex_unlock(&conn->mutex);
	}
}


CIVETWEB_API void
mg_lock_context(struct mg_context *ctx)
{
	if (ctx && (ctx->context_type == CONTEXT_SERVER)) {
		(void)pthread_mutex_lock(&ctx->nonce_mutex);
	}
}


CIVETWEB_API void
mg_unlock_context(struct mg_context *ctx)
{
	if (ctx && (ctx->context_type == CONTEXT_SERVER)) {
		(void)pthread_mutex_unlock(&ctx->nonce_mutex);
	}
}


#if defined(USE_LUA)
#include "mod_lua.inl"
#endif /* USE_LUA */

#if defined(USE_DUKTAPE)
#include "mod_duktape.inl"
#endif /* USE_DUKTAPE */

#if defined(USE_WEBSOCKET)

#if !defined(NO_SSL_DL)
#if !defined(OPENSSL_API_3_0)
#define SHA_API static
#include "sha1.inl"
#endif
#endif

static int
send_websocket_handshake(struct mg_connection *conn, const char *websock_key)
{
	static const char *magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	char buf[100], sha[20], b64_sha[sizeof(sha) * 2];
	size_t dst_len = sizeof(b64_sha);
#if !defined(OPENSSL_API_3_0)
	SHA_CTX sha_ctx;
#endif
	int truncated;

	/* Calculate Sec-WebSocket-Accept reply from Sec-WebSocket-Key. */
	mg_snprintf(conn, &truncated, buf, sizeof(buf), "%s%s", websock_key, magic);
	if (truncated) {
		conn->must_close = 1;
		return 0;
	}

	DEBUG_TRACE("%s", "Send websocket handshake");

#if defined(OPENSSL_API_3_0)
	EVP_Digest((unsigned char *)buf,
	           (uint32_t)strlen(buf),
	           (unsigned char *)sha,
	           NULL,
	           EVP_get_digestbyname("sha1"),
	           NULL);
#else
	SHA1_Init(&sha_ctx);
	SHA1_Update(&sha_ctx, (unsigned char *)buf, (uint32_t)strlen(buf));
	SHA1_Final((unsigned char *)sha, &sha_ctx);
#endif
	mg_base64_encode((unsigned char *)sha, sizeof(sha), b64_sha, &dst_len);
	mg_printf(conn,
	          "HTTP/1.1 101 Switching Protocols\r\n"
	          "Upgrade: websocket\r\n"
	          "Connection: Upgrade\r\n"
	          "Sec-WebSocket-Accept: %s\r\n",
	          b64_sha);

#if defined(USE_ZLIB) && defined(MG_EXPERIMENTAL_INTERFACES)
	// Send negotiated compression extension parameters
	websocket_deflate_response(conn);
#endif

	if (conn->request_info.acceptedWebSocketSubprotocol) {
		mg_printf(conn,
		          "Sec-WebSocket-Protocol: %s\r\n\r\n",
		          conn->request_info.acceptedWebSocketSubprotocol);
	} else {
		mg_printf(conn, "%s", "\r\n");
	}

	return 1;
}


#if !defined(MG_MAX_UNANSWERED_PING)
/* Configuration of the maximum number of websocket PINGs that might
 * stay unanswered before the connection is considered broken.
 * Note: The name of this define may still change (until it is
 * defined as a compile parameter in a documentation).
 */
#define MG_MAX_UNANSWERED_PING (5)
#endif


static void
read_websocket(struct mg_connection *conn,
               mg_websocket_data_handler ws_data_handler,
               void *callback_data)
{
	/* Pointer to the beginning of the portion of the incoming websocket
	 * message queue.
	 * The original websocket upgrade request is never removed, so the queue
	 * begins after it. */
	unsigned char *buf = (unsigned char *)conn->buf + conn->request_len;
	int n, error, exit_by_callback;
	int ret;

	/* body_len is the length of the entire queue in bytes
	 * len is the length of the current message
	 * data_len is the length of the current message's data payload
	 * header_len is the length of the current message's header */
	size_t i, len, mask_len = 0, header_len, body_len;
	uint64_t data_len = 0;

	/* "The masking key is a 32-bit value chosen at random by the client."
	 * http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-17#section-5
	 */
	unsigned char mask[4];

	/* data points to the place where the message is stored when passed to
	 * the websocket_data callback.  This is either mem on the stack, or a
	 * dynamically allocated buffer if it is too large. */
	unsigned char mem[4096];
	unsigned char mop; /* mask flag and opcode */

	/* Variables used for connection monitoring */
	double timeout = -1.0;
	int enable_ping_pong = 0;
	int ping_count = 0;

	if (conn->dom_ctx->config[ENABLE_WEBSOCKET_PING_PONG]) {
		enable_ping_pong =
		    !mg_strcasecmp(conn->dom_ctx->config[ENABLE_WEBSOCKET_PING_PONG],
		                   "yes");
	}

	if (conn->dom_ctx->config[WEBSOCKET_TIMEOUT]) {
		timeout = atoi(conn->dom_ctx->config[WEBSOCKET_TIMEOUT]) / 1000.0;
	}
	if ((timeout <= 0.0) && (conn->dom_ctx->config[REQUEST_TIMEOUT])) {
		timeout = atoi(conn->dom_ctx->config[REQUEST_TIMEOUT]) / 1000.0;
	}
	if (timeout <= 0.0) {
		timeout = atof(config_options[REQUEST_TIMEOUT].default_value) / 1000.0;
	}

	/* Enter data processing loop */
	DEBUG_TRACE("Websocket connection %s:%u start data processing loop",
	            conn->request_info.remote_addr,
	            conn->request_info.remote_port);
	conn->in_websocket_handling = 1;
	mg_set_thread_name("wsock");

	/* Loop continuously, reading messages from the socket, invoking the
	 * callback, and waiting repeatedly until an error occurs. */
	while (STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)
	       && (!conn->must_close)) {
		header_len = 0;
		DEBUG_ASSERT(conn->data_len >= conn->request_len);
		if ((body_len = (size_t)(conn->data_len - conn->request_len)) >= 2) {
			len = buf[1] & 127;
			mask_len = (buf[1] & 128) ? 4 : 0;
			if ((len < 126) && (body_len >= mask_len)) {
				/* inline 7-bit length field */
				data_len = len;
				header_len = 2 + mask_len;
			} else if ((len == 126) && (body_len >= (4 + mask_len))) {
				/* 16-bit length field */
				header_len = 4 + mask_len;
				data_len = ((((size_t)buf[2]) << 8) + buf[3]);
			} else if (body_len >= (10 + mask_len)) {
				/* 64-bit length field */
				uint32_t l1, l2;
				memcpy(&l1, &buf[2], 4); /* Use memcpy for alignment */
				memcpy(&l2, &buf[6], 4);
				header_len = 10 + mask_len;
				data_len = (((uint64_t)ntohl(l1)) << 32) + ntohl(l2);

				if (data_len > (uint64_t)0x7FFF0000ul) {
					/* no can do */
					mg_cry_internal(
					    conn,
					    "%s",
					    "websocket out of memory; closing connection");
					break;
				}
			}
		}

		if ((header_len > 0) && (body_len >= header_len)) {
			/* Allocate space to hold websocket payload */
			unsigned char *data = mem;

			if ((size_t)data_len > (size_t)sizeof(mem)) {
				data = (unsigned char *)mg_malloc_ctx((size_t)data_len,
				                                      conn->phys_ctx);
				if (data == NULL) {
					/* Allocation failed, exit the loop and then close the
					 * connection */
					mg_cry_internal(
					    conn,
					    "%s",
					    "websocket out of memory; closing connection");
					break;
				}
			}

			/* Copy the mask before we shift the queue and destroy it */
			if (mask_len > 0) {
				memcpy(mask, buf + header_len - mask_len, sizeof(mask));
			} else {
				memset(mask, 0, sizeof(mask));
			}

			/* Read frame payload from the first message in the queue into
			 * data and advance the queue by moving the memory in place. */
			DEBUG_ASSERT(body_len >= header_len);
			if (data_len + (uint64_t)header_len > (uint64_t)body_len) {
				mop = buf[0]; /* current mask and opcode */
				              /* Overflow case */
				len = body_len - header_len;
				memcpy(data, buf + header_len, len);
				error = 0;
				while ((uint64_t)len < data_len) {
					n = pull_inner(NULL,
					               conn,
					               (char *)(data + len),
					               (int)(data_len - len),
					               timeout);
					if (n <= -2) {
						error = 1;
						break;
					} else if (n > 0) {
						len += (size_t)n;
					} else {
						/* Timeout: should retry */
						/* TODO: retry condition */
					}
				}
				if (error) {
					mg_cry_internal(
					    conn,
					    "%s",
					    "Websocket pull failed; closing connection");
					if (data != mem) {
						mg_free(data);
					}
					break;
				}

				conn->data_len = conn->request_len;

			} else {

				mop = buf[0]; /* current mask and opcode, overwritten by
				               * memmove() */

				/* Length of the message being read at the front of the
				 * queue. Cast to 31 bit is OK, since we limited
				 * data_len before. */
				len = (size_t)data_len + header_len;

				/* Copy the data payload into the data pointer for the
				 * callback. Cast to 31 bit is OK, since we
				 * limited data_len */
				memcpy(data, buf + header_len, (size_t)data_len);

				/* Move the queue forward len bytes */
				memmove(buf, buf + len, body_len - len);

				/* Mark the queue as advanced */
				conn->data_len -= (int)len;
			}

			/* Apply mask if necessary */
			if (mask_len > 0) {
				for (i = 0; i < (size_t)data_len; i++) {
					data[i] ^= mask[i & 3];
				}
			}

			exit_by_callback = 0;
			if (enable_ping_pong && ((mop & 0xF) == MG_WEBSOCKET_OPCODE_PONG)) {
				/* filter PONG messages */
				DEBUG_TRACE("PONG from %s:%u",
				            conn->request_info.remote_addr,
				            conn->request_info.remote_port);
				/* No unanwered PINGs left */
				ping_count = 0;
			} else if (enable_ping_pong
			           && ((mop & 0xF) == MG_WEBSOCKET_OPCODE_PING)) {
				/* reply PING messages */
				DEBUG_TRACE("Reply PING from %s:%u",
				            conn->request_info.remote_addr,
				            conn->request_info.remote_port);
				ret = mg_websocket_write(conn,
				                         MG_WEBSOCKET_OPCODE_PONG,
				                         (char *)data,
				                         (size_t)data_len);
				if (ret <= 0) {
					/* Error: send failed */
					DEBUG_TRACE("Reply PONG failed (%i)", ret);
					break;
				}

			} else {
				/* Exit the loop if callback signals to exit (server side),
				 * or "connection close" opcode received (client side). */
				if (ws_data_handler != NULL) {
#if defined(USE_ZLIB) && defined(MG_EXPERIMENTAL_INTERFACES)
					if (mop & 0x40) {
						/* Inflate the data received if bit RSV1 is set. */
						if (!conn->websocket_deflate_initialized) {
							if (websocket_deflate_initialize(conn, 1) != Z_OK)
								exit_by_callback = 1;
						}
						if (!exit_by_callback) {
							size_t inflate_buf_size_old = 0;
							size_t inflate_buf_size =
							    data_len
							    * 4; // Initial guess of the inflated message
							         // size. We double the memory when needed.
							Bytef *inflated = NULL;
							Bytef *new_mem = NULL;
							conn->websocket_inflate_state.avail_in =
							    (uInt)(data_len + 4);
							conn->websocket_inflate_state.next_in = data;
							// Add trailing 0x00 0x00 0xff 0xff bytes
							data[data_len] = '\x00';
							data[data_len + 1] = '\x00';
							data[data_len + 2] = '\xff';
							data[data_len + 3] = '\xff';
							do {
								if (inflate_buf_size_old == 0) {
									new_mem =
									    (Bytef *)mg_calloc(inflate_buf_size,
									                       sizeof(Bytef));
								} else {
									inflate_buf_size *= 2;
									new_mem =
									    (Bytef *)mg_realloc(inflated,
									                        inflate_buf_size);
								}
								if (new_mem == NULL) {
									mg_cry_internal(
									    conn,
									    "Out of memory: Cannot allocate "
									    "inflate buffer of %lu bytes",
									    (unsigned long)inflate_buf_size);
									exit_by_callback = 1;
									break;
								}
								inflated = new_mem;
								conn->websocket_inflate_state.avail_out =
								    (uInt)(inflate_buf_size
								           - inflate_buf_size_old);
								conn->websocket_inflate_state.next_out =
								    inflated + inflate_buf_size_old;
								ret = inflate(&conn->websocket_inflate_state,
								              Z_SYNC_FLUSH);
								if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR
								    || ret == Z_MEM_ERROR) {
									mg_cry_internal(
									    conn,
									    "ZLIB inflate error: %i %s",
									    ret,
									    (conn->websocket_inflate_state.msg
									         ? conn->websocket_inflate_state.msg
									         : "<no error message>"));
									exit_by_callback = 1;
									break;
								}
								inflate_buf_size_old = inflate_buf_size;

							} while (conn->websocket_inflate_state.avail_out
							         == 0);
							inflate_buf_size -=
							    conn->websocket_inflate_state.avail_out;
							if (!ws_data_handler(conn,
							                     mop,
							                     (char *)inflated,
							                     inflate_buf_size,
							                     callback_data)) {
								exit_by_callback = 1;
							}
							mg_free(inflated);
						}
					} else
#endif
					    if (!ws_data_handler(conn,
					                         mop,
					                         (char *)data,
					                         (size_t)data_len,
					                         callback_data)) {
						exit_by_callback = 1;
					}
				}
			}

			/* It a buffer has been allocated, free it again */
			if (data != mem) {
				mg_free(data);
			}

			if (exit_by_callback) {
				DEBUG_TRACE("Callback requests to close connection from %s:%u",
				            conn->request_info.remote_addr,
				            conn->request_info.remote_port);
				break;
			}
			if ((mop & 0xf) == MG_WEBSOCKET_OPCODE_CONNECTION_CLOSE) {
				/* Opcode == 8, connection close */
				DEBUG_TRACE("Message requests to close connection from %s:%u",
				            conn->request_info.remote_addr,
				            conn->request_info.remote_port);
				break;
			}

			/* Not breaking the loop, process next websocket frame. */
		} else {
			/* Read from the socket into the next available location in the
			 * message queue. */
			n = pull_inner(NULL,
			               conn,
			               conn->buf + conn->data_len,
			               conn->buf_size - conn->data_len,
			               timeout);
			if (n <= -2) {
				/* Error, no bytes read */
				DEBUG_TRACE("PULL from %s:%u failed",
				            conn->request_info.remote_addr,
				            conn->request_info.remote_port);
				break;
			}
			if (n > 0) {
				conn->data_len += n;
				/* Reset open PING count */
				ping_count = 0;
			} else {
				if (STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)
				    && (!conn->must_close)) {
					if (ping_count > MG_MAX_UNANSWERED_PING) {
						/* Stop sending PING */
						DEBUG_TRACE("Too many (%i) unanswered ping from %s:%u "
						            "- closing connection",
						            ping_count,
						            conn->request_info.remote_addr,
						            conn->request_info.remote_port);
						break;
					}
					if (enable_ping_pong) {
						/* Send Websocket PING message */
						DEBUG_TRACE("PING to %s:%u",
						            conn->request_info.remote_addr,
						            conn->request_info.remote_port);
						ret = mg_websocket_write(conn,
						                         MG_WEBSOCKET_OPCODE_PING,
						                         NULL,
						                         0);

						if (ret <= 0) {
							/* Error: send failed */
							DEBUG_TRACE("Send PING failed (%i)", ret);
							break;
						}
						ping_count++;
					}
				}
				/* Timeout: should retry */
				/* TODO: get timeout def */
			}
		}
	}

	/* Leave data processing loop */
	mg_set_thread_name("worker");
	conn->in_websocket_handling = 0;
	DEBUG_TRACE("Websocket connection %s:%u left data processing loop",
	            conn->request_info.remote_addr,
	            conn->request_info.remote_port);
}


static int
mg_websocket_write_exec(struct mg_connection *conn,
                        int opcode,
                        const char *data,
                        size_t dataLen,
                        uint32_t masking_key)
{
	unsigned char header[14];
	size_t headerLen;
	int retval;

#if defined(GCC_DIAGNOSTIC)
	/* Disable spurious conversion warning for GCC */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

	/* Note that POSIX/Winsock's send() is threadsafe
	 * http://stackoverflow.com/questions/1981372/are-parallel-calls-to-send-recv-on-the-same-socket-valid
	 * but mongoose's mg_printf/mg_write is not (because of the loop in
	 * push(), although that is only a problem if the packet is large or
	 * outgoing buffer is full). */

	/* TODO: Check if this lock should be moved to user land.
	 * Currently the server sets this lock for websockets, but
	 * not for any other connection. It must be set for every
	 * conn read/written by more than one thread, no matter if
	 * it is a websocket or regular connection. */
	(void)mg_lock_connection(conn);

#if defined(USE_ZLIB) && defined(MG_EXPERIMENTAL_INTERFACES)
	size_t deflated_size = 0;
	Bytef *deflated = 0;
	// Deflate websocket messages over 100kb
	int use_deflate = dataLen > 100 * 1024 && conn->accept_gzip;

	if (use_deflate) {
		if (!conn->websocket_deflate_initialized) {
			if (websocket_deflate_initialize(conn, 1) != Z_OK)
				return 0;
		}

		// Deflating the message
		header[0] = 0xC0u | (unsigned char)((unsigned)opcode & 0xf);
		conn->websocket_deflate_state.avail_in = (uInt)dataLen;
		conn->websocket_deflate_state.next_in = (unsigned char *)data;
		deflated_size = (size_t)compressBound((uLong)dataLen);
		deflated = mg_calloc(deflated_size, sizeof(Bytef));
		if (deflated == NULL) {
			mg_cry_internal(
			    conn,
			    "Out of memory: Cannot allocate deflate buffer of %lu bytes",
			    (unsigned long)deflated_size);
			mg_unlock_connection(conn);
			return -1;
		}
		conn->websocket_deflate_state.avail_out = (uInt)deflated_size;
		conn->websocket_deflate_state.next_out = deflated;
		deflate(&conn->websocket_deflate_state, conn->websocket_deflate_flush);
		dataLen = deflated_size - conn->websocket_deflate_state.avail_out
		          - 4; // Strip trailing 0x00 0x00 0xff 0xff bytes
	} else
#endif
		header[0] = 0x80u | (unsigned char)((unsigned)opcode & 0xf);

#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic pop
#endif

	/* Frame format: http://tools.ietf.org/html/rfc6455#section-5.2 */
	if (dataLen < 126) {
		/* inline 7-bit length field */
		header[1] = (unsigned char)dataLen;
		headerLen = 2;
	} else if (dataLen <= 0xFFFF) {
		/* 16-bit length field */
		uint16_t len = htons((uint16_t)dataLen);
		header[1] = 126;
		memcpy(header + 2, &len, 2);
		headerLen = 4;
	} else {
		/* 64-bit length field */
		uint32_t len1 = htonl((uint32_t)((uint64_t)dataLen >> 32));
		uint32_t len2 = htonl((uint32_t)(dataLen & 0xFFFFFFFFu));
		header[1] = 127;
		memcpy(header + 2, &len1, 4);
		memcpy(header + 6, &len2, 4);
		headerLen = 10;
	}

	if (masking_key) {
		/* add mask */
		header[1] |= 0x80;
		memcpy(header + headerLen, &masking_key, 4);
		headerLen += 4;
	}

	retval = mg_write(conn, header, headerLen);
	if (retval != (int)headerLen) {
		/* Did not send complete header */
		retval = -1;
	} else {
		if (dataLen > 0) {
#if defined(USE_ZLIB) && defined(MG_EXPERIMENTAL_INTERFACES)
			if (use_deflate) {
				retval = mg_write(conn, deflated, dataLen);
				mg_free(deflated);
			} else
#endif
				retval = mg_write(conn, data, dataLen);
		}
		/* if dataLen == 0, the header length (2) is returned */
	}

	/* TODO: Remove this unlock as well, when lock is removed. */
	mg_unlock_connection(conn);

	return retval;
}


CIVETWEB_API int
mg_websocket_write(struct mg_connection *conn,
                   int opcode,
                   const char *data,
                   size_t dataLen)
{
	return mg_websocket_write_exec(conn, opcode, data, dataLen, 0);
}


static void
mask_data(const char *in, size_t in_len, uint32_t masking_key, char *out)
{
	size_t i = 0;

	i = 0;
	if ((in_len > 3) && ((ptrdiff_t)in % 4) == 0) {
		/* Convert in 32 bit words, if data is 4 byte aligned */
		while (i < (in_len - 3)) {
			*(uint32_t *)(void *)(out + i) =
			    *(uint32_t *)(void *)(in + i) ^ masking_key;
			i += 4;
		}
	}
	if (i != in_len) {
		/* convert 1-3 remaining bytes if ((dataLen % 4) != 0)*/
		while (i < in_len) {
			*(uint8_t *)(void *)(out + i) =
			    *(uint8_t *)(void *)(in + i)
			    ^ *(((uint8_t *)&masking_key) + (i % 4));
			i++;
		}
	}
}


CIVETWEB_API int
mg_websocket_client_write(struct mg_connection *conn,
                          int opcode,
                          const char *data,
                          size_t dataLen)
{
	int retval = -1;
	char *masked_data =
	    (char *)mg_malloc_ctx(((dataLen + 7) / 4) * 4, conn->phys_ctx);
	uint32_t masking_key = 0;

	if (masked_data == NULL) {
		/* Return -1 in an error case */
		mg_cry_internal(conn,
		                "%s",
		                "Cannot allocate buffer for masked websocket response: "
		                "Out of memory");
		return -1;
	}

	do {
		/* Get a masking key - but not 0 */
		masking_key = (uint32_t)get_random();
	} while (masking_key == 0);

	mask_data(data, dataLen, masking_key, masked_data);

	retval = mg_websocket_write_exec(
	    conn, opcode, masked_data, dataLen, masking_key);
	mg_free(masked_data);

	return retval;
}


static void
handle_websocket_request(struct mg_connection *conn,
                         const char *path,
                         int is_callback_resource,
                         struct mg_websocket_subprotocols *subprotocols,
                         mg_websocket_connect_handler ws_connect_handler,
                         mg_websocket_ready_handler ws_ready_handler,
                         mg_websocket_data_handler ws_data_handler,
                         mg_websocket_close_handler ws_close_handler,
                         void *cbData)
{
	const char *websock_key = mg_get_header(conn, "Sec-WebSocket-Key");
	const char *version = mg_get_header(conn, "Sec-WebSocket-Version");
	ptrdiff_t lua_websock = 0;

#if !defined(USE_LUA)
	(void)path;
#endif

	/* Step 1: Check websocket protocol version. */
	/* Step 1.1: Check Sec-WebSocket-Key. */
	if (!websock_key) {
		/* The RFC standard version (https://tools.ietf.org/html/rfc6455)
		 * requires a Sec-WebSocket-Key header.
		 */
		/* It could be the hixie draft version
		 * (http://tools.ietf.org/html/draft-hixie-thewebsocketprotocol-76).
		 */
		const char *key1 = mg_get_header(conn, "Sec-WebSocket-Key1");
		const char *key2 = mg_get_header(conn, "Sec-WebSocket-Key2");
		char key3[8];

		if ((key1 != NULL) && (key2 != NULL)) {
			/* This version uses 8 byte body data in a GET request */
			conn->content_len = 8;
			if (8 == mg_read(conn, key3, 8)) {
				/* This is the hixie version */
				mg_send_http_error(conn,
				                   426,
				                   "%s",
				                   "Protocol upgrade to RFC 6455 required");
				return;
			}
		}
		/* This is an unknown version */
		mg_send_http_error(conn, 400, "%s", "Malformed websocket request");
		return;
	}

	/* Step 1.2: Check websocket protocol version. */
	/* The RFC version (https://tools.ietf.org/html/rfc6455) is 13. */
	if ((version == NULL) || (strcmp(version, "13") != 0)) {
		/* Reject wrong versions */
		mg_send_http_error(conn, 426, "%s", "Protocol upgrade required");
		return;
	}

	/* Step 1.3: Could check for "Host", but we do not really need this
	 * value for anything, so just ignore it. */

	/* Step 2: If a callback is responsible, call it. */
	if (is_callback_resource) {
		/* Step 2.1 check and select subprotocol */
		const char *protocols[64]; // max 64 headers
		int nbSubprotocolHeader = get_req_headers(&conn->request_info,
		                                          "Sec-WebSocket-Protocol",
		                                          protocols,
		                                          64);

		if ((nbSubprotocolHeader > 0) && subprotocols) {

			int headerNo, idx;
			size_t len;
			const char *sep, *curSubProtocol,
			    *acceptedWebSocketSubprotocol = NULL;

			/* look for matching subprotocol */
			for (headerNo = 0; headerNo < nbSubprotocolHeader; headerNo++) {
				/* There might be multiple headers ... */
				const char *protocol = protocols[headerNo];
				curSubProtocol = protocol;

				/* ... and in every header there might be a , separated list */
				while (!acceptedWebSocketSubprotocol && (*curSubProtocol)) {

					while ((*curSubProtocol == ' ') || (*curSubProtocol == ','))
						curSubProtocol++;
					sep = strchr(curSubProtocol, ',');
					if (sep) {
						len = (size_t)(sep - curSubProtocol);
					} else {
						len = strlen(curSubProtocol);
					}

					for (idx = 0; idx < subprotocols->nb_subprotocols; idx++) {
						// COMPARE: curSubProtocol ==
						// subprotocols->subprotocols[idx]
						if ((strlen(subprotocols->subprotocols[idx]) == len)
						    && (strncmp(curSubProtocol,
						                subprotocols->subprotocols[idx],
						                len)
						        == 0)) {
							acceptedWebSocketSubprotocol =
							    subprotocols->subprotocols[idx];
							break;
						}
					}
					curSubProtocol += len;
				}
			}

			conn->request_info.acceptedWebSocketSubprotocol =
			    acceptedWebSocketSubprotocol;
		}

#if defined(USE_ZLIB) && defined(MG_EXPERIMENTAL_INTERFACES)
		websocket_deflate_negotiate(conn);
#endif

		if ((ws_connect_handler != NULL)
		    && (ws_connect_handler(conn, cbData) != 0)) {
			/* C callback has returned non-zero, do not proceed with
			 * handshake.
			 */
			/* Note that C callbacks are no longer called when Lua is
			 * responsible, so C can no longer filter callbacks for Lua. */
			return;
		}
	}

#if defined(USE_LUA)
	/* Step 3: No callback. Check if Lua is responsible. */
	else {
		/* Step 3.1: Check if Lua is responsible. */
		if (conn->dom_ctx->config[LUA_WEBSOCKET_EXTENSIONS]) {
			lua_websock = match_prefix_strlen(
			    conn->dom_ctx->config[LUA_WEBSOCKET_EXTENSIONS], path);
		}

		if (lua_websock > 0) {
			/* Step 3.2: Lua is responsible: call it. */
			conn->lua_websocket_state = lua_websocket_new(path, conn);
			if (!conn->lua_websocket_state) {
				/* Lua rejected the new client */
				return;
			}
		}
	}
#endif

	/* Step 4: Check if there is a responsible websocket handler. */
	if (!is_callback_resource && !lua_websock) {
		/* There is no callback, and Lua is not responsible either. */
		/* Reply with a 404 Not Found. We are still at a standard
		 * HTTP request here, before the websocket handshake, so
		 * we can still send standard HTTP error replies. */
		mg_send_http_error(conn, 404, "%s", "Not found");
		return;
	}

	/* Step 5: The websocket connection has been accepted */
	if (!send_websocket_handshake(conn, websock_key)) {
		mg_send_http_error(conn, 500, "%s", "Websocket handshake failed");
		return;
	}

	/* Step 6: Call the ready handler */
	if (is_callback_resource) {
		if (ws_ready_handler != NULL) {
			ws_ready_handler(conn, cbData);
		}
#if defined(USE_LUA)
	} else if (lua_websock) {
		if (!lua_websocket_ready(conn, conn->lua_websocket_state)) {
			/* the ready handler returned false */
			return;
		}
#endif
	}

	/* Step 7: Enter the read loop */
	if (is_callback_resource) {
		read_websocket(conn, ws_data_handler, cbData);
#if defined(USE_LUA)
	} else if (lua_websock) {
		read_websocket(conn, lua_websocket_data, conn->lua_websocket_state);
#endif
	}

#if defined(USE_ZLIB) && defined(MG_EXPERIMENTAL_INTERFACES)
	/* Step 8: Close the deflate & inflate buffers */
	if (conn->websocket_deflate_initialized) {
		deflateEnd(&conn->websocket_deflate_state);
		inflateEnd(&conn->websocket_inflate_state);
	}
#endif

	/* Step 9: Call the close handler */
	if (ws_close_handler) {
		ws_close_handler(conn, cbData);
	}
}
#endif /* !USE_WEBSOCKET */


/* Is upgrade request:
 *   0 = regular HTTP/1.0 or HTTP/1.1 request
 *   1 = upgrade to websocket
 *   2 = upgrade to HTTP/2
 * -1 = upgrade to unknown protocol
 */
static int
should_switch_to_protocol(const struct mg_connection *conn)
{
	const char *connection_headers[8];
	const char *upgrade_to;
	int connection_header_count, i, should_upgrade;

	/* A websocket protocol has the following HTTP headers:
	 *
	 * Connection: Upgrade
	 * Upgrade: Websocket
	 *
	 * It seems some clients use multiple headers:
	 * see https://github.com/civetweb/civetweb/issues/1083
	 */
	connection_header_count = get_req_headers(&conn->request_info,
	                                          "Connection",
	                                          connection_headers,
	                                          8);
	should_upgrade = 0;
	for (i = 0; i < connection_header_count; i++) {
		if (0 != mg_strcasestr(connection_headers[i], "upgrade")) {
			should_upgrade = 1;
		}
	}
	if (!should_upgrade) {
		return PROTOCOL_TYPE_HTTP1;
	}

	upgrade_to = mg_get_header(conn, "Upgrade");
	if (upgrade_to == NULL) {
		/* "Connection: Upgrade" without "Upgrade" Header --> Error */
		return -1;
	}

	/* Upgrade to ... */
	if (0 != mg_strcasestr(upgrade_to, "websocket")) {
		/* The headers "Host", "Sec-WebSocket-Key", "Sec-WebSocket-Protocol" and
		 * "Sec-WebSocket-Version" are also required.
		 * Don't check them here, since even an unsupported websocket protocol
		 * request still IS a websocket request (in contrast to a standard HTTP
		 * request). It will fail later in handle_websocket_request.
		 */
		return PROTOCOL_TYPE_WEBSOCKET; /* Websocket */
	}
	if (0 != mg_strcasestr(upgrade_to, "h2")) {
		return PROTOCOL_TYPE_HTTP2; /* Websocket */
	}

	/* Upgrade to another protocol */
	return -1;
}


static int
parse_match_net(const struct vec *vec, const union usa *sa, int no_strict)
{
	int n;
	unsigned int a, b, c, d, slash;

	if (sscanf(vec->ptr, "%u.%u.%u.%u/%u%n", &a, &b, &c, &d, &slash, &n)
	    != 5) { // NOLINT(cert-err34-c) 'sscanf' used to convert a string to an
		        // integer value, but function will not report conversion
		        // errors; consider using 'strtol' instead
		slash = 32;
		if (sscanf(vec->ptr, "%u.%u.%u.%u%n", &a, &b, &c, &d, &n)
		    != 4) { // NOLINT(cert-err34-c) 'sscanf' used to convert a string to
			        // an integer value, but function will not report conversion
			        // errors; consider using 'strtol' instead
			n = 0;
		}
	}

	if ((n > 0) && ((size_t)n == vec->len)) {
		if ((a < 256) && (b < 256) && (c < 256) && (d < 256) && (slash < 33)) {
			/* IPv4 format */
			if (sa->sa.sa_family == AF_INET) {
				uint32_t ip = ntohl(sa->sin.sin_addr.s_addr);
				uint32_t net = ((uint32_t)a << 24) | ((uint32_t)b << 16)
				               | ((uint32_t)c << 8) | (uint32_t)d;
				uint32_t mask = slash ? (0xFFFFFFFFu << (32 - slash)) : 0;
				return (ip & mask) == net;
			}
			return 0;
		}
	}
#if defined(USE_IPV6)
	else {
		char ad[50];
		const char *p;

		if (sscanf(vec->ptr, "[%49[^]]]/%u%n", ad, &slash, &n) != 2) {
			slash = 128;
			if (sscanf(vec->ptr, "[%49[^]]]%n", ad, &n) != 1) {
				n = 0;
			}
		}

		if ((n <= 0) && no_strict) {
			/* no square brackets? */
			p = strchr(vec->ptr, '/');
			if (p && (p < (vec->ptr + vec->len))) {
				if (((size_t)(p - vec->ptr) < sizeof(ad))
				    && (sscanf(p, "/%u%n", &slash, &n) == 1)) {
					n += (int)(p - vec->ptr);
					mg_strlcpy(ad, vec->ptr, (size_t)(p - vec->ptr) + 1);
				} else {
					n = 0;
				}
			} else if (vec->len < sizeof(ad)) {
				n = (int)vec->len;
				slash = 128;
				mg_strlcpy(ad, vec->ptr, vec->len + 1);
			}
		}

		if ((n > 0) && ((size_t)n == vec->len) && (slash < 129)) {
			p = ad;
			c = 0;
			/* zone indexes are unsupported, at least two colons are needed */
			while (isxdigit((unsigned char)*p) || (*p == '.') || (*p == ':')) {
				if (*(p++) == ':') {
					c++;
				}
			}
			if ((*p == '\0') && (c >= 2)) {
				struct sockaddr_in6 sin6;
				unsigned int i;

				/* for strict validation, an actual IPv6 argument is needed */
				if (sa->sa.sa_family != AF_INET6) {
					return 0;
				}
				if (mg_inet_pton(AF_INET6, ad, &sin6, sizeof(sin6), 0)) {
					/* IPv6 format */
					for (i = 0; i < 16; i++) {
						uint8_t ip = sa->sin6.sin6_addr.s6_addr[i];
						uint8_t net = sin6.sin6_addr.s6_addr[i];
						uint8_t mask = 0;

						if (8 * i + 8 < slash) {
							mask = 0xFFu;
						} else if (8 * i < slash) {
							mask = (uint8_t)(0xFFu << (8 * i + 8 - slash));
						}
						if ((ip & mask) != net) {
							return 0;
						}
					}
					return 1;
				}
			}
		}
	}
#else
	(void)no_strict;
#endif

	/* malformed */
	return -1;
}


static int
set_throttle(const char *spec, const union usa *rsa, const char *uri)
{
	int throttle = 0;
	struct vec vec, val;
	char mult;
	double v;

	while ((spec = next_option(spec, &vec, &val)) != NULL) {
		mult = ',';
		if ((val.ptr == NULL)
		    || (sscanf(val.ptr, "%lf%c", &v, &mult)
		        < 1) // NOLINT(cert-err34-c) 'sscanf' used to convert a string
		             // to an integer value, but function will not report
		             // conversion errors; consider using 'strtol' instead
		    || (v < 0)
		    || ((lowercase(&mult) != 'k') && (lowercase(&mult) != 'm')
		        && (mult != ','))) {
			continue;
		}
		v *= (lowercase(&mult) == 'k')
		         ? 1024
		         : ((lowercase(&mult) == 'm') ? 1048576 : 1);
		if (vec.len == 1 && vec.ptr[0] == '*') {
			throttle = (int)v;
		} else {
			int matched = parse_match_net(&vec, rsa, 0);
			if (matched >= 0) {
				/* a valid IP subnet */
				if (matched) {
					throttle = (int)v;
				}
			} else if (match_prefix(vec.ptr, vec.len, uri) > 0) {
				throttle = (int)v;
			}
		}
	}

	return throttle;
}


/* The mg_upload function is superseded by mg_handle_form_request. */
#include "handle_form.inl"


static int
get_first_ssl_listener_index(const struct mg_context *ctx)
{
	unsigned int i;
	int idx = -1;
	if (ctx) {
		for (i = 0; ((idx == -1) && (i < ctx->num_listening_sockets)); i++) {
			idx = ctx->listening_sockets[i].is_ssl ? ((int)(i)) : -1;
		}
	}
	return idx;
}


/* Return host (without port) */
static void
get_host_from_request_info(struct vec *host, const struct mg_request_info *ri)
{
	const char *host_header =
	    get_header(ri->http_headers, ri->num_headers, "Host");

	host->ptr = NULL;
	host->len = 0;

	if (host_header != NULL) {
		const char *pos;

		/* If the "Host" is an IPv6 address, like [::1], parse until ]
		 * is found. */
		if (*host_header == '[') {
			pos = strchr(host_header, ']');
			if (!pos) {
				/* Malformed hostname starts with '[', but no ']' found */
				DEBUG_TRACE("%s", "Host name format error '[' without ']'");
				return;
			}
			/* terminate after ']' */
			host->ptr = host_header;
			host->len = (size_t)(pos + 1 - host_header);
		} else {
			/* Otherwise, a ':' separates hostname and port number */
			pos = strchr(host_header, ':');
			if (pos != NULL) {
				host->len = (size_t)(pos - host_header);
			} else {
				host->len = strlen(host_header);
			}
			host->ptr = host_header;
		}
	}
}


static int
switch_domain_context(struct mg_connection *conn)
{
	struct vec host;

	get_host_from_request_info(&host, &conn->request_info);

	if (host.ptr) {
		if (conn->ssl) {
			/* This is a HTTPS connection, maybe we have a hostname
			 * from SNI (set in ssl_servername_callback). */
			const char *sslhost = conn->dom_ctx->config[AUTHENTICATION_DOMAIN];
			if (sslhost && (conn->dom_ctx != &(conn->phys_ctx->dd))) {
				/* We are not using the default domain */
				if ((strlen(sslhost) != host.len)
				    || mg_strncasecmp(host.ptr, sslhost, host.len)) {
					/* Mismatch between SNI domain and HTTP domain */
					DEBUG_TRACE("Host mismatch: SNI: %s, HTTPS: %.*s",
					            sslhost,
					            (int)host.len,
					            host.ptr);
					return 0;
				}
			}

		} else {
			struct mg_domain_context *dom = &(conn->phys_ctx->dd);
			while (dom) {
				const char *domName = dom->config[AUTHENTICATION_DOMAIN];
				size_t domNameLen = strlen(domName);
				if ((domNameLen == host.len)
				    && !mg_strncasecmp(host.ptr, domName, host.len)) {

					/* Found matching domain */
					DEBUG_TRACE("HTTP domain %s found",
					            dom->config[AUTHENTICATION_DOMAIN]);

					/* TODO: Check if this is a HTTP or HTTPS domain */
					conn->dom_ctx = dom;
					break;
				}
				mg_lock_context(conn->phys_ctx);
				dom = dom->next;
				mg_unlock_context(conn->phys_ctx);
			}
		}

		DEBUG_TRACE("HTTP%s Host: %.*s",
		            conn->ssl ? "S" : "",
		            (int)host.len,
		            host.ptr);

	} else {
		DEBUG_TRACE("HTTP%s Host is not set", conn->ssl ? "S" : "");
		return 1;
	}

	return 1;
}


static void
redirect_to_https_port(struct mg_connection *conn, int port)
{
	char target_url[MG_BUF_LEN];
	int truncated = 0;
	const char *expect_proto =
	    (conn->protocol_type == PROTOCOL_TYPE_WEBSOCKET) ? "wss" : "https";

	/* Use "308 Permanent Redirect" */
	int redirect_code = 308;

	/* In any case, close the current connection */
	conn->must_close = 1;

	/* Send host, port, uri and (if it exists) ?query_string */
	if (mg_construct_local_link(
	        conn, target_url, sizeof(target_url), expect_proto, port, NULL)
	    < 0) {
		truncated = 1;
	} else if (conn->request_info.query_string != NULL) {
		size_t slen1 = strlen(target_url);
		size_t slen2 = strlen(conn->request_info.query_string);
		if ((slen1 + slen2 + 2) < sizeof(target_url)) {
			target_url[slen1] = '?';
			memcpy(target_url + slen1 + 1,
			       conn->request_info.query_string,
			       slen2);
			target_url[slen1 + slen2 + 1] = 0;
		} else {
			truncated = 1;
		}
	}

	/* Check overflow in location buffer (will not occur if MG_BUF_LEN
	 * is used as buffer size) */
	if (truncated) {
		mg_send_http_error(conn, 500, "%s", "Redirect URL too long");
		return;
	}

	/* Use redirect helper function */
	mg_send_http_redirect(conn, target_url, redirect_code);
}


static void
mg_set_handler_type(struct mg_context *phys_ctx,
                    struct mg_domain_context *dom_ctx,
                    const char *uri,
                    int handler_type,
                    int is_delete_request,
                    mg_request_handler handler,
                    struct mg_websocket_subprotocols *subprotocols,
                    mg_websocket_connect_handler connect_handler,
                    mg_websocket_ready_handler ready_handler,
                    mg_websocket_data_handler data_handler,
                    mg_websocket_close_handler close_handler,
                    mg_authorization_handler auth_handler,
                    void *cbdata)
{
	struct mg_handler_info *tmp_rh, **lastref;
	size_t urilen = strlen(uri);

	if (handler_type == WEBSOCKET_HANDLER) {
		DEBUG_ASSERT(handler == NULL);
		DEBUG_ASSERT(is_delete_request || connect_handler != NULL
		             || ready_handler != NULL || data_handler != NULL
		             || close_handler != NULL);

		DEBUG_ASSERT(auth_handler == NULL);
		if (handler != NULL) {
			return;
		}
		if (!is_delete_request && (connect_handler == NULL)
		    && (ready_handler == NULL) && (data_handler == NULL)
		    && (close_handler == NULL)) {
			return;
		}
		if (auth_handler != NULL) {
			return;
		}

	} else if (handler_type == REQUEST_HANDLER) {
		DEBUG_ASSERT(connect_handler == NULL && ready_handler == NULL
		             && data_handler == NULL && close_handler == NULL);
		DEBUG_ASSERT(is_delete_request || (handler != NULL));
		DEBUG_ASSERT(auth_handler == NULL);

		if ((connect_handler != NULL) || (ready_handler != NULL)
		    || (data_handler != NULL) || (close_handler != NULL)) {
			return;
		}
		if (!is_delete_request && (handler == NULL)) {
			return;
		}
		if (auth_handler != NULL) {
			return;
		}

	} else if (handler_type == AUTH_HANDLER) {
		DEBUG_ASSERT(handler == NULL);
		DEBUG_ASSERT(connect_handler == NULL && ready_handler == NULL
		             && data_handler == NULL && close_handler == NULL);
		DEBUG_ASSERT(is_delete_request || (auth_handler != NULL));
		if (handler != NULL) {
			return;
		}
		if ((connect_handler != NULL) || (ready_handler != NULL)
		    || (data_handler != NULL) || (close_handler != NULL)) {
			return;
		}
		if (!is_delete_request && (auth_handler == NULL)) {
			return;
		}
	} else {
		/* Unknown handler type. */
		return;
	}

	if (!phys_ctx || !dom_ctx) {
		/* no context available */
		return;
	}

	mg_lock_context(phys_ctx);

	/* first try to find an existing handler */
	do {
		lastref = &(dom_ctx->handlers);
		for (tmp_rh = dom_ctx->handlers; tmp_rh != NULL;
		     tmp_rh = tmp_rh->next) {
			if (tmp_rh->handler_type == handler_type
			    && (urilen == tmp_rh->uri_len) && !strcmp(tmp_rh->uri, uri)) {
				if (!is_delete_request) {
					/* update existing handler */
					if (handler_type == REQUEST_HANDLER) {
						/* Wait for end of use before updating */
						if (tmp_rh->refcount) {
							mg_unlock_context(phys_ctx);
							mg_sleep(1);
							mg_lock_context(phys_ctx);
							/* tmp_rh might have been freed, search again. */
							break;
						}
						/* Ok, the handler is no more use -> Update it */
						tmp_rh->handler = handler;
					} else if (handler_type == WEBSOCKET_HANDLER) {
						tmp_rh->subprotocols = subprotocols;
						tmp_rh->connect_handler = connect_handler;
						tmp_rh->ready_handler = ready_handler;
						tmp_rh->data_handler = data_handler;
						tmp_rh->close_handler = close_handler;
					} else { /* AUTH_HANDLER */
						tmp_rh->auth_handler = auth_handler;
					}
					tmp_rh->cbdata = cbdata;
				} else {
					/* remove existing handler */
					if (handler_type == REQUEST_HANDLER) {
						/* Wait for end of use before removing */
						if (tmp_rh->refcount) {
							tmp_rh->removing = 1;
							mg_unlock_context(phys_ctx);
							mg_sleep(1);
							mg_lock_context(phys_ctx);
							/* tmp_rh might have been freed, search again. */
							break;
						}
						/* Ok, the handler is no more used */
					}
					*lastref = tmp_rh->next;
					mg_free(tmp_rh->uri);
					mg_free(tmp_rh);
				}
				mg_unlock_context(phys_ctx);
				return;
			}
			lastref = &(tmp_rh->next);
		}
	} while (tmp_rh != NULL);

	if (is_delete_request) {
		/* no handler to set, this was a remove request to a non-existing
		 * handler */
		mg_unlock_context(phys_ctx);
		return;
	}

	tmp_rh =
	    (struct mg_handler_info *)mg_calloc_ctx(1,
	                                            sizeof(struct mg_handler_info),
	                                            phys_ctx);
	if (tmp_rh == NULL) {
		mg_unlock_context(phys_ctx);
		mg_cry_ctx_internal(phys_ctx,
		                    "%s",
		                    "Cannot create new request handler struct, OOM");
		return;
	}
	tmp_rh->uri = mg_strdup_ctx(uri, phys_ctx);
	if (!tmp_rh->uri) {
		mg_unlock_context(phys_ctx);
		mg_free(tmp_rh);
		mg_cry_ctx_internal(phys_ctx,
		                    "%s",
		                    "Cannot create new request handler struct, OOM");
		return;
	}
	tmp_rh->uri_len = urilen;
	if (handler_type == REQUEST_HANDLER) {
		tmp_rh->refcount = 0;
		tmp_rh->removing = 0;
		tmp_rh->handler = handler;
	} else if (handler_type == WEBSOCKET_HANDLER) {
		tmp_rh->subprotocols = subprotocols;
		tmp_rh->connect_handler = connect_handler;
		tmp_rh->ready_handler = ready_handler;
		tmp_rh->data_handler = data_handler;
		tmp_rh->close_handler = close_handler;
	} else { /* AUTH_HANDLER */
		tmp_rh->auth_handler = auth_handler;
	}
	tmp_rh->cbdata = cbdata;
	tmp_rh->handler_type = handler_type;
	tmp_rh->next = NULL;

	*lastref = tmp_rh;
	mg_unlock_context(phys_ctx);
}


CIVETWEB_API void
mg_set_request_handler(struct mg_context *ctx,
                       const char *uri,
                       mg_request_handler handler,
                       void *cbdata)
{
	mg_set_handler_type(ctx,
	                    &(ctx->dd),
	                    uri,
	                    REQUEST_HANDLER,
	                    handler == NULL,
	                    handler,
	                    NULL,
	                    NULL,
	                    NULL,
	                    NULL,
	                    NULL,
	                    NULL,
	                    cbdata);
}


CIVETWEB_API void
mg_set_websocket_handler(struct mg_context *ctx,
                         const char *uri,
                         mg_websocket_connect_handler connect_handler,
                         mg_websocket_ready_handler ready_handler,
                         mg_websocket_data_handler data_handler,
                         mg_websocket_close_handler close_handler,
                         void *cbdata)
{
	mg_set_websocket_handler_with_subprotocols(ctx,
	                                           uri,
	                                           NULL,
	                                           connect_handler,
	                                           ready_handler,
	                                           data_handler,
	                                           close_handler,
	                                           cbdata);
}


CIVETWEB_API void
mg_set_websocket_handler_with_subprotocols(
    struct mg_context *ctx,
    const char *uri,
    struct mg_websocket_subprotocols *subprotocols,
    mg_websocket_connect_handler connect_handler,
    mg_websocket_ready_handler ready_handler,
    mg_websocket_data_handler data_handler,
    mg_websocket_close_handler close_handler,
    void *cbdata)
{
	int is_delete_request = (connect_handler == NULL) && (ready_handler == NULL)
	                        && (data_handler == NULL)
	                        && (close_handler == NULL);
	mg_set_handler_type(ctx,
	                    &(ctx->dd),
	                    uri,
	                    WEBSOCKET_HANDLER,
	                    is_delete_request,
	                    NULL,
	                    subprotocols,
	                    connect_handler,
	                    ready_handler,
	                    data_handler,
	                    close_handler,
	                    NULL,
	                    cbdata);
}


CIVETWEB_API void
mg_set_auth_handler(struct mg_context *ctx,
                    const char *uri,
                    mg_authorization_handler handler,
                    void *cbdata)
{
	mg_set_handler_type(ctx,
	                    &(ctx->dd),
	                    uri,
	                    AUTH_HANDLER,
	                    handler == NULL,
	                    NULL,
	                    NULL,
	                    NULL,
	                    NULL,
	                    NULL,
	                    NULL,
	                    handler,
	                    cbdata);
}


static int
get_request_handler(struct mg_connection *conn,
                    int handler_type,
                    mg_request_handler *handler,
                    struct mg_websocket_subprotocols **subprotocols,
                    mg_websocket_connect_handler *connect_handler,
                    mg_websocket_ready_handler *ready_handler,
                    mg_websocket_data_handler *data_handler,
                    mg_websocket_close_handler *close_handler,
                    mg_authorization_handler *auth_handler,
                    void **cbdata,
                    struct mg_handler_info **handler_info)
{
	const struct mg_request_info *request_info = mg_get_request_info(conn);
	if (request_info) {
		const char *uri = request_info->local_uri;
		size_t urilen = strlen(uri);
		struct mg_handler_info *tmp_rh;
		int step, matched;

		if (!conn || !conn->phys_ctx || !conn->dom_ctx) {
			return 0;
		}

		mg_lock_context(conn->phys_ctx);

		for (step = 0; step < 3; step++) {
			for (tmp_rh = conn->dom_ctx->handlers; tmp_rh != NULL;
			     tmp_rh = tmp_rh->next) {
				if (tmp_rh->handler_type != handler_type) {
					continue;
				}
				if (step == 0) {
					/* first try for an exact match */
					matched = (tmp_rh->uri_len == urilen)
					          && (strcmp(tmp_rh->uri, uri) == 0);
				} else if (step == 1) {
					/* next try for a partial match, we will accept
					uri/something */
					matched =
					    (tmp_rh->uri_len < urilen)
					    && (uri[tmp_rh->uri_len] == '/')
					    && (memcmp(tmp_rh->uri, uri, tmp_rh->uri_len) == 0);
				} else {
					/* finally try for pattern match */
					matched =
					    match_prefix(tmp_rh->uri, tmp_rh->uri_len, uri) > 0;
				}
				if (matched) {
					if (handler_type == WEBSOCKET_HANDLER) {
						*subprotocols = tmp_rh->subprotocols;
						*connect_handler = tmp_rh->connect_handler;
						*ready_handler = tmp_rh->ready_handler;
						*data_handler = tmp_rh->data_handler;
						*close_handler = tmp_rh->close_handler;
					} else if (handler_type == REQUEST_HANDLER) {
						if (tmp_rh->removing) {
							/* Treat as none found */
							step = 2;
							break;
						}
						*handler = tmp_rh->handler;
						/* Acquire handler and give it back */
						tmp_rh->refcount++;
						*handler_info = tmp_rh;
					} else { /* AUTH_HANDLER */
						*auth_handler = tmp_rh->auth_handler;
					}
					*cbdata = tmp_rh->cbdata;
					mg_unlock_context(conn->phys_ctx);
					return 1;
				}
			}
		}

		mg_unlock_context(conn->phys_ctx);
	}
	return 0; /* none found */
}


/* Check if the script file is in a path, allowed for script files.
 * This can be used if uploading files is possible not only for the server
 * admin, and the upload mechanism does not check the file extension.
 */
static int
is_in_script_path(const struct mg_connection *conn, const char *path)
{
	/* TODO (Feature): Add config value for allowed script path.
	 * Default: All allowed. */
	(void)conn;
	(void)path;
	return 1;
}


#if defined(USE_WEBSOCKET) && defined(MG_EXPERIMENTAL_INTERFACES)
static int
experimental_websocket_client_data_wrapper(struct mg_connection *conn,
                                           int bits,
                                           char *data,
                                           size_t len,
                                           void *cbdata)
{
	struct mg_callbacks *pcallbacks = (struct mg_callbacks *)cbdata;
	if (pcallbacks->websocket_data) {
		return pcallbacks->websocket_data(conn, bits, data, len);
	}
	/* No handler set - assume "OK" */
	return 1;
}


static void
experimental_websocket_client_close_wrapper(const struct mg_connection *conn,
                                            void *cbdata)
{
	struct mg_callbacks *pcallbacks = (struct mg_callbacks *)cbdata;
	if (pcallbacks->connection_close) {
		pcallbacks->connection_close(conn);
	}
}
#endif


/* Decrement recount of handler. conn must not be NULL, handler_info may be NULL
 */
static void
release_handler_ref(struct mg_connection *conn,
                    struct mg_handler_info *handler_info)
{
	if (handler_info != NULL) {
		/* Use context lock for ref counter */
		mg_lock_context(conn->phys_ctx);
		handler_info->refcount--;
		mg_unlock_context(conn->phys_ctx);
	}
}


/* This is the heart of the Civetweb's logic.
 * This function is called when the request is read, parsed and validated,
 * and Civetweb must decide what action to take: serve a file, or
 * a directory, or call embedded function, etcetera. */
static void
handle_request(struct mg_connection *conn)
{
	struct mg_request_info *ri = &conn->request_info;
	char path[UTF8_PATH_MAX];
	int uri_len, ssl_index;
	int is_found = 0, is_script_resource = 0, is_websocket_request = 0,
	    is_put_or_delete_request = 0, is_callback_resource = 0,
	    is_template_text_file = 0, is_webdav_request = 0;
	int i;
	struct mg_file file = STRUCT_FILE_INITIALIZER;
	mg_request_handler callback_handler = NULL;
	struct mg_handler_info *handler_info = NULL;
	struct mg_websocket_subprotocols *subprotocols;
	mg_websocket_connect_handler ws_connect_handler = NULL;
	mg_websocket_ready_handler ws_ready_handler = NULL;
	mg_websocket_data_handler ws_data_handler = NULL;
	mg_websocket_close_handler ws_close_handler = NULL;
	void *callback_data = NULL;
	mg_authorization_handler auth_handler = NULL;
	void *auth_callback_data = NULL;
	int handler_type;
	time_t curtime = time(NULL);
	char date[64];
	char *tmp;

	path[0] = 0;

	/* 0. Reset internal state (required for HTTP/2 proxy) */
	conn->request_state = 0;

	/* 1. get the request url */
	/* 1.1. split into url and query string */
	if ((conn->request_info.query_string = strchr(ri->request_uri, '?'))
	    != NULL) {
		*((char *)conn->request_info.query_string++) = '\0';
	}

	/* 1.2. do a https redirect, if required. Do not decode URIs yet. */
	if (!conn->client.is_ssl && conn->client.ssl_redir) {
		ssl_index = get_first_ssl_listener_index(conn->phys_ctx);
		if (ssl_index >= 0) {
			int port = (int)ntohs(USA_IN_PORT_UNSAFE(
			    &(conn->phys_ctx->listening_sockets[ssl_index].lsa)));
			redirect_to_https_port(conn, port);
		} else {
			/* A http to https forward port has been specified,
			 * but no https port to forward to. */
			mg_send_http_error(conn,
			                   503,
			                   "%s",
			                   "Error: SSL forward not configured properly");
			mg_cry_internal(conn,
			                "%s",
			                "Can not redirect to SSL, no SSL port available");
		}
		return;
	}
	uri_len = (int)strlen(ri->local_uri);

	/* 1.3. decode url (if config says so) */
	if (should_decode_url(conn)) {
		url_decode_in_place((char *)ri->local_uri);
	}

	/* URL decode the query-string only if explicitly set in the configuration
	 */
	if (conn->request_info.query_string) {
		if (should_decode_query_string(conn)) {
			url_decode_in_place((char *)conn->request_info.query_string);
		}
	}

	/* 1.4. clean URIs, so a path like allowed_dir/../forbidden_file is not
	 * possible. The fact that we cleaned the URI is stored in that the
	 * pointer to ri->local_ur and ri->local_uri_raw are now different.
	 * ri->local_uri_raw still points to memory allocated in
	 * worker_thread_run(). ri->local_uri is private to the request so we
	 * don't have to use preallocated memory here. */
	tmp = mg_strdup(ri->local_uri_raw);
	if (!tmp) {
		/* Out of memory. We cannot do anything reasonable here. */
		return;
	}
	remove_dot_segments(tmp);
	ri->local_uri = tmp;

	/* step 1. completed, the url is known now */
	DEBUG_TRACE("REQUEST: %s %s", ri->request_method, ri->local_uri);

	/* 2. if this ip has limited speed, set it for this connection */
	conn->throttle = set_throttle(conn->dom_ctx->config[THROTTLE],
	                              &conn->client.rsa,
	                              ri->local_uri);

	/* 3. call a "handle everything" callback, if registered */
	if (conn->phys_ctx->callbacks.begin_request != NULL) {
		/* Note that since V1.7 the "begin_request" function is called
		 * before an authorization check. If an authorization check is
		 * required, use a request_handler instead. */
		i = conn->phys_ctx->callbacks.begin_request(conn);
		if (i > 0) {
			/* callback already processed the request. Store the
			return value as a status code for the access log. */
			conn->status_code = i;
			if (!conn->must_close) {
				discard_unread_request_data(conn);
			}
			DEBUG_TRACE("%s", "begin_request handled request");
			return;
		} else if (i == 0) {
			/* civetweb should process the request */
		} else {
			/* unspecified - may change with the next version */
			DEBUG_TRACE("%s", "done (undocumented behavior)");
			return;
		}
	}

	/* request not yet handled by a handler or redirect, so the request
	 * is processed here */

	/* 4. Check for CORS preflight requests and handle them (if configured).
	 * https://developer.mozilla.org/en-US/docs/Web/HTTP/Access_control_CORS
	 */
	if (!strcmp(ri->request_method, "OPTIONS")) {
		/* Send a response to CORS preflights only if
		 * access_control_allow_methods is not NULL and not an empty string.
		 * In this case, scripts can still handle CORS. */
		const char *cors_meth_cfg =
		    conn->dom_ctx->config[ACCESS_CONTROL_ALLOW_METHODS];
		const char *cors_orig_cfg =
		    conn->dom_ctx->config[ACCESS_CONTROL_ALLOW_ORIGIN];
		const char *cors_origin =
		    get_header(ri->http_headers, ri->num_headers, "Origin");
		const char *cors_acrm = get_header(ri->http_headers,
		                                   ri->num_headers,
		                                   "Access-Control-Request-Method");

		/* Todo: check if cors_origin is in cors_orig_cfg.
		 * Or, let the client check this. */

		if ((cors_meth_cfg != NULL) && (*cors_meth_cfg != 0)
		    && (cors_orig_cfg != NULL) && (*cors_orig_cfg != 0)
		    && (cors_origin != NULL) && (cors_acrm != NULL)) {
			/* This is a valid CORS preflight, and the server is configured
			 * to handle it automatically. */
			const char *cors_acrh =
			    get_header(ri->http_headers,
			               ri->num_headers,
			               "Access-Control-Request-Headers");
			const char *cors_cred_cfg =
			    conn->dom_ctx->config[ACCESS_CONTROL_ALLOW_CREDENTIALS];
			const char *cors_exphdr_cfg =
			    conn->dom_ctx->config[ACCESS_CONTROL_EXPOSE_HEADERS];

			gmt_time_string(date, sizeof(date), &curtime);
			mg_printf(conn,
			          "HTTP/1.1 200 OK\r\n"
			          "Date: %s\r\n"
			          "Access-Control-Allow-Origin: %s\r\n"
			          "Access-Control-Allow-Methods: %s\r\n"
			          "Content-Length: 0\r\n"
			          "Connection: %s\r\n",
			          date,
			          cors_orig_cfg,
			          ((cors_meth_cfg[0] == '*') ? cors_acrm : cors_meth_cfg),
			          suggest_connection_header(conn));

			if (cors_cred_cfg && *cors_cred_cfg) {
				mg_printf(conn,
				          "Access-Control-Allow-Credentials: %s\r\n",
				          cors_cred_cfg);
			}

			if (cors_exphdr_cfg && *cors_exphdr_cfg) {
				mg_printf(conn,
				          "Access-Control-Expose-Headers: %s\r\n",
				          cors_exphdr_cfg);
			}

			if (cors_acrh || (cors_cred_cfg && *cors_cred_cfg)) {
				/* CORS request is asking for additional headers */
				const char *cors_hdr_cfg =
				    conn->dom_ctx->config[ACCESS_CONTROL_ALLOW_HEADERS];

				if ((cors_hdr_cfg != NULL) && (*cors_hdr_cfg != 0)) {
					/* Allow only if access_control_allow_headers is
					 * not NULL and not an empty string. If this
					 * configuration is set to *, allow everything.
					 * Otherwise this configuration must be a list
					 * of allowed HTTP header names. */
					mg_printf(conn,
					          "Access-Control-Allow-Headers: %s\r\n",
					          ((cors_hdr_cfg[0] == '*') ? cors_acrh
					                                    : cors_hdr_cfg));
				}
			}
			mg_printf(conn, "Access-Control-Max-Age: 60\r\n");
			mg_printf(conn, "\r\n");
			DEBUG_TRACE("%s", "OPTIONS done");
			return;
		}
	}

	/* 5. interpret the url to find out how the request must be handled
	 */
	/* 5.1. first test, if the request targets the regular http(s)://
	 * protocol namespace or the websocket ws(s):// protocol namespace.
	 */
	is_websocket_request = (conn->protocol_type == PROTOCOL_TYPE_WEBSOCKET);
#if defined(USE_WEBSOCKET)
	handler_type = is_websocket_request ? WEBSOCKET_HANDLER : REQUEST_HANDLER;
#else
	handler_type = REQUEST_HANDLER;
#endif /* defined(USE_WEBSOCKET) */

	if (is_websocket_request) {
		HTTP1_only;
	}

	/* 5.2. check if the request will be handled by a callback */
	if (get_request_handler(conn,
	                        handler_type,
	                        &callback_handler,
	                        &subprotocols,
	                        &ws_connect_handler,
	                        &ws_ready_handler,
	                        &ws_data_handler,
	                        &ws_close_handler,
	                        NULL,
	                        &callback_data,
	                        &handler_info)) {
		/* 5.2.1. A callback will handle this request. All requests
		 * handled by a callback have to be considered as requests
		 * to a script resource. */
		is_callback_resource = 1;
		is_script_resource = 1;
		is_put_or_delete_request = is_put_or_delete_method(conn);
		/* Never handle a C callback according to File WebDav rules,
		 * even if it is a webdav method */
		is_webdav_request = 0; /* is_civetweb_webdav_method(conn); */
	} else {
	no_callback_resource:

		/* 5.2.2. No callback is responsible for this request. The URI
		 * addresses a file based resource (static content or Lua/cgi
		 * scripts in the file system). */
		is_callback_resource = 0;
		interpret_uri(conn,
		              path,
		              sizeof(path),
		              &file.stat,
		              &is_found,
		              &is_script_resource,
		              &is_websocket_request,
		              &is_put_or_delete_request,
		              &is_webdav_request,
		              &is_template_text_file);
	}

	/* 5.3. A webdav request (PROPFIND/PROPPATCH/LOCK/UNLOCK) */
	if (is_webdav_request) {
		/* TODO: Do we need a config option? */
		const char *webdav_enable = conn->dom_ctx->config[ENABLE_WEBDAV];
		if (webdav_enable[0] != 'y') {
			mg_send_http_error(conn,
			                   405,
			                   "%s method not allowed",
			                   conn->request_info.request_method);
			DEBUG_TRACE("%s", "webdav rejected");
			return;
		}
	}

	/* 6. authorization check */
	/* 6.1. a custom authorization handler is installed */
	if (get_request_handler(conn,
	                        AUTH_HANDLER,
	                        NULL,
	                        NULL,
	                        NULL,
	                        NULL,
	                        NULL,
	                        NULL,
	                        &auth_handler,
	                        &auth_callback_data,
	                        NULL)) {
		if (!auth_handler(conn, auth_callback_data)) {

			/* Callback handler will not be used anymore. Release it */
			release_handler_ref(conn, handler_info);
			DEBUG_TRACE("%s", "auth handler rejected request");
			return;
		}
	} else if (is_put_or_delete_request && !is_script_resource
	           && !is_callback_resource) {
		HTTP1_only;
		/* 6.2. this request is a PUT/DELETE to a real file */
		/* 6.2.1. thus, the server must have real files */
#if defined(NO_FILES)
		if (1) {
#else
		if (conn->dom_ctx->config[DOCUMENT_ROOT] == NULL
		    || conn->dom_ctx->config[PUT_DELETE_PASSWORDS_FILE] == NULL) {
#endif
			/* This code path will not be called for request handlers */
			DEBUG_ASSERT(handler_info == NULL);

			/* This server does not have any real files, thus the
			 * PUT/DELETE methods are not valid. */
			mg_send_http_error(conn,
			                   405,
			                   "%s method not allowed",
			                   conn->request_info.request_method);
			DEBUG_TRACE("%s", "all file based put/delete requests rejected");
			return;
		}

#if !defined(NO_FILES)
		/* 6.2.2. Check if put authorization for static files is
		 * available.
		 */
		if (!is_authorized_for_put(conn)) {
			send_authorization_request(conn, NULL);
			DEBUG_TRACE("%s", "file write needs authorization");
			return;
		}
#endif

	} else {
		/* 6.3. This is either a OPTIONS, GET, HEAD or POST request,
		 * or it is a PUT or DELETE request to a resource that does not
		 * correspond to a file. Check authorization. */
		if (!check_authorization(conn, path)) {
			send_authorization_request(conn, NULL);

			/* Callback handler will not be used anymore. Release it */
			release_handler_ref(conn, handler_info);
			DEBUG_TRACE("%s", "access authorization required");
			return;
		}
	}

	/* request is authorized or does not need authorization */

	/* 7. check if there are request handlers for this uri */
	if (is_callback_resource) {
		HTTP1_only;
		if (!is_websocket_request) {
			i = callback_handler(conn, callback_data);

			/* Callback handler will not be used anymore. Release it */
			release_handler_ref(conn, handler_info);

			if (i > 0) {
				/* Do nothing, callback has served the request. Store
				 * then return value as status code for the log and discard
				 * all data from the client not used by the callback. */
				conn->status_code = i;
				if (!conn->must_close) {
					discard_unread_request_data(conn);
				}
			} else {
				/* The handler did NOT handle the request. */
				/* Some proper reactions would be:
				 * a) close the connections without sending anything
				 * b) send a 404 not found
				 * c) try if there is a file matching the URI
				 * It would be possible to do a, b or c in the callback
				 * implementation, and return 1 - we cannot do anything
				 * here, that is not possible in the callback.
				 *
				 * TODO: What would be the best reaction here?
				 * (Note: The reaction may change, if there is a better
				 * idea.)
				 */

				/* For the moment, use option c: We look for a proper file,
				 * but since a file request is not always a script resource,
				 * the authorization check might be different. */
				callback_handler = NULL;

				/* Here we are at a dead end:
				 * According to URI matching, a callback should be
				 * responsible for handling the request,
				 * we called it, but the callback declared itself
				 * not responsible.
				 * We use a goto here, to get out of this dead end,
				 * and continue with the default handling.
				 * A goto here is simpler and better to understand
				 * than some curious loop. */
				goto no_callback_resource;
			}
		} else {
#if defined(USE_WEBSOCKET)
			handle_websocket_request(conn,
			                         path,
			                         is_callback_resource,
			                         subprotocols,
			                         ws_connect_handler,
			                         ws_ready_handler,
			                         ws_data_handler,
			                         ws_close_handler,
			                         callback_data);
#endif
		}
		DEBUG_TRACE("%s", "websocket handling done");
		return;
	}

	/* 8. handle websocket requests */
#if defined(USE_WEBSOCKET)
	if (is_websocket_request) {
		HTTP1_only;
		if (is_script_resource) {

			if (is_in_script_path(conn, path)) {
				/* Websocket Lua script */
				handle_websocket_request(conn,
				                         path,
				                         0 /* Lua Script */,
				                         NULL,
				                         NULL,
				                         NULL,
				                         NULL,
				                         NULL,
				                         conn->phys_ctx->user_data);
			} else {
				/* Script was in an illegal path */
				mg_send_http_error(conn, 403, "%s", "Forbidden");
			}
		} else {
			mg_send_http_error(conn, 404, "%s", "Not found");
		}
		DEBUG_TRACE("%s", "websocket script done");
		return;
	} else
#endif

#if defined(NO_FILES)
		/* 9a. In case the server uses only callbacks, this uri is
		 * unknown.
		 * Then, all request handling ends here. */
		mg_send_http_error(conn, 404, "%s", "Not Found");

#else
	/* 9b. This request is either for a static file or resource handled
	 * by a script file. Thus, a DOCUMENT_ROOT must exist. */
	if (conn->dom_ctx->config[DOCUMENT_ROOT] == NULL) {
		mg_send_http_error(conn, 404, "%s", "Not Found");
		DEBUG_TRACE("%s", "no document root available");
		return;
	}

	/* 10. Request is handled by a script */
	if (is_script_resource) {
		HTTP1_only;
		handle_file_based_request(conn, path, &file);
		DEBUG_TRACE("%s", "script handling done");
		return;
	}

	/* Request was not handled by a callback or script. It will be
	 * handled by a server internal method. */

	/* 11. Handle put/delete/mkcol requests */
	if (is_put_or_delete_request) {
		HTTP1_only;
		/* 11.1. PUT method */
		if (!strcmp(ri->request_method, "PUT")) {
			put_file(conn, path);
			DEBUG_TRACE("handling %s request to %s done",
			            ri->request_method,
			            path);
			return;
		}
		/* 11.2. DELETE method */
		if (!strcmp(ri->request_method, "DELETE")) {
			delete_file(conn, path);
			DEBUG_TRACE("handling %s request to %s done",
			            ri->request_method,
			            path);
			return;
		}
		/* 11.3. MKCOL method */
		if (!strcmp(ri->request_method, "MKCOL")) {
			dav_mkcol(conn, path);
			DEBUG_TRACE("handling %s request to %s done",
			            ri->request_method,
			            path);
			return;
		}
		/* 11.4. MOVE method */
		if (!strcmp(ri->request_method, "MOVE")) {
			dav_move_file(conn, path, 0);
			DEBUG_TRACE("handling %s request to %s done",
			            ri->request_method,
			            path);
			return;
		}
		if (!strcmp(ri->request_method, "COPY")) {
			dav_move_file(conn, path, 1);
			DEBUG_TRACE("handling %s request to %s done",
			            ri->request_method,
			            path);
			return;
		}
		/* 11.5. LOCK method */
		if (!strcmp(ri->request_method, "LOCK")) {
			dav_lock_file(conn, path);
			DEBUG_TRACE("handling %s request to %s done",
			            ri->request_method,
			            path);
			return;
		}
		/* 11.6. UNLOCK method */
		if (!strcmp(ri->request_method, "UNLOCK")) {
			dav_unlock_file(conn, path);
			DEBUG_TRACE("handling %s request to %s done",
			            ri->request_method,
			            path);
			return;
		}
		/* 11.7. PROPPATCH method */
		if (!strcmp(ri->request_method, "PROPPATCH")) {
			dav_proppatch(conn, path);
			DEBUG_TRACE("handling %s request to %s done",
			            ri->request_method,
			            path);
			return;
		}
		/* 11.8. Other methods, e.g.: PATCH
		 * This method is not supported for static resources,
		 * only for scripts (Lua, CGI) and callbacks. */
		mg_send_http_error(conn,
		                   405,
		                   "%s method not allowed",
		                   conn->request_info.request_method);
		DEBUG_TRACE("method %s on %s is not supported",
		            ri->request_method,
		            path);
		return;
	}

	/* 11. File does not exist, or it was configured that it should be
	 * hidden */
	if (!is_found || (must_hide_file(conn, path))) {
		mg_send_http_error(conn, 404, "%s", "Not found");
		DEBUG_TRACE("handling %s request to %s: file not found",
		            ri->request_method,
		            path);
		return;
	}

	/* 12. Directory uris should end with a slash */
	if (file.stat.is_directory && ((uri_len = (int)strlen(ri->local_uri)) > 0)
	    && (ri->local_uri[uri_len - 1] != '/')) {

		/* Path + server root */
		size_t buflen = UTF8_PATH_MAX * 2 + 2;
		char *new_path;

		if (ri->query_string) {
			buflen += strlen(ri->query_string);
		}
		new_path = (char *)mg_malloc_ctx(buflen, conn->phys_ctx);
		if (!new_path) {
			mg_send_http_error(conn, 500, "out or memory");
		} else {
			mg_get_request_link(conn, new_path, buflen - 1);
			strcat(new_path, "/");
			if (ri->query_string) {
				/* Append ? and query string */
				strcat(new_path, "?");
				strcat(new_path, ri->query_string);
			}
			mg_send_http_redirect(conn, new_path, 301);
			mg_free(new_path);
		}
		DEBUG_TRACE("%s request to %s: directory redirection sent",
		            ri->request_method,
		            path);
		return;
	}

	/* 13. Handle other methods than GET/HEAD */
	/* 13.1. Handle PROPFIND */
	if (!strcmp(ri->request_method, "PROPFIND")) {
		handle_propfind(conn, path, &file.stat);
		DEBUG_TRACE("handling %s request to %s done", ri->request_method, path);
		return;
	}
	/* 13.2. Handle OPTIONS for files */
	if (!strcmp(ri->request_method, "OPTIONS")) {
		/* This standard handler is only used for real files.
		 * Scripts should support the OPTIONS method themselves, to allow a
		 * maximum flexibility.
		 * Lua and CGI scripts may fully support CORS this way (including
		 * preflights). */
		send_options(conn);
		DEBUG_TRACE("handling %s request to %s done", ri->request_method, path);
		return;
	}
	/* 13.3. everything but GET and HEAD (e.g. POST) */
	if ((0 != strcmp(ri->request_method, "GET"))
	    && (0 != strcmp(ri->request_method, "HEAD"))) {
		mg_send_http_error(conn,
		                   405,
		                   "%s method not allowed",
		                   conn->request_info.request_method);
		DEBUG_TRACE("handling %s request to %s done", ri->request_method, path);
		return;
	}

	/* 14. directories */
	if (file.stat.is_directory) {
		/* Substitute files have already been handled above. */
		/* Here we can either generate and send a directory listing,
		 * or send an "access denied" error. */
		if (!mg_strcasecmp(conn->dom_ctx->config[ENABLE_DIRECTORY_LISTING],
		                   "yes")) {
			handle_directory_request(conn, path);
		} else {
			mg_send_http_error(conn,
			                   403,
			                   "%s",
			                   "Error: Directory listing denied");
		}
		DEBUG_TRACE("handling %s request to %s done", ri->request_method, path);
		return;
	}

	/* 15. Files with search/replace patterns: LSP and SSI */
	if (is_template_text_file) {
		HTTP1_only;
		handle_file_based_request(conn, path, &file);
		DEBUG_TRACE("handling %s request to %s done (template)",
		            ri->request_method,
		            path);
		return;
	}

	/* 16. Static file - maybe cached */
#if !defined(NO_CACHING)
	if ((!conn->in_error_handler) && is_not_modified(conn, &file.stat)) {
		/* Send 304 "Not Modified" - this must not send any body data */
		handle_not_modified_static_file_request(conn, &file);
		DEBUG_TRACE("handling %s request to %s done (not modified)",
		            ri->request_method,
		            path);
		return;
	}
#endif /* !NO_CACHING */

	/* 17. Static file - not cached */
	handle_static_file_request(conn, path, &file, NULL, NULL);
	DEBUG_TRACE("handling %s request to %s done (static)",
	            ri->request_method,
	            path);

#endif /* !defined(NO_FILES) */
}


#if !defined(NO_FILESYSTEMS)
static void
handle_file_based_request(struct mg_connection *conn,
                          const char *path,
                          struct mg_file *file)
{
#if !defined(NO_CGI)
	int cgi_config_idx, inc, max;
#endif

	if (!conn || !conn->dom_ctx) {
		return;
	}

#if defined(USE_LUA)
	if (match_prefix_strlen(conn->dom_ctx->config[LUA_SERVER_PAGE_EXTENSIONS],
	                        path)
	    > 0) {
		if (is_in_script_path(conn, path)) {
			/* Lua server page: an SSI like page containing mostly plain
			 * html code plus some tags with server generated contents. */
			handle_lsp_request(conn, path, file, NULL);
		} else {
			/* Script was in an illegal path */
			mg_send_http_error(conn, 403, "%s", "Forbidden");
		}
		return;
	}

	if (match_prefix_strlen(conn->dom_ctx->config[LUA_SCRIPT_EXTENSIONS], path)
	    > 0) {
		if (is_in_script_path(conn, path)) {
			/* Lua in-server module script: a CGI like script used to
			 * generate the entire reply. */
			mg_exec_lua_script(conn, path, NULL);
		} else {
			/* Script was in an illegal path */
			mg_send_http_error(conn, 403, "%s", "Forbidden");
		}
		return;
	}
#endif

#if defined(USE_DUKTAPE)
	if (match_prefix_strlen(conn->dom_ctx->config[DUKTAPE_SCRIPT_EXTENSIONS],
	                        path)
	    > 0) {
		if (is_in_script_path(conn, path)) {
			/* Call duktape to generate the page */
			mg_exec_duktape_script(conn, path);
		} else {
			/* Script was in an illegal path */
			mg_send_http_error(conn, 403, "%s", "Forbidden");
		}
		return;
	}
#endif

#if !defined(NO_CGI)
	inc = CGI2_EXTENSIONS - CGI_EXTENSIONS;
	max = PUT_DELETE_PASSWORDS_FILE - CGI_EXTENSIONS;
	for (cgi_config_idx = 0; cgi_config_idx < max; cgi_config_idx += inc) {
		if (conn->dom_ctx->config[CGI_EXTENSIONS + cgi_config_idx] != NULL) {
			if (match_prefix_strlen(
			        conn->dom_ctx->config[CGI_EXTENSIONS + cgi_config_idx],
			        path)
			    > 0) {
				if (is_in_script_path(conn, path)) {
					/* CGI scripts may support all HTTP methods */
					handle_cgi_request(conn, path, cgi_config_idx);
				} else {
					/* Script was in an illegal path */
					mg_send_http_error(conn, 403, "%s", "Forbidden");
				}
				return;
			}
		}
	}
#endif /* !NO_CGI */

	if (match_prefix_strlen(conn->dom_ctx->config[SSI_EXTENSIONS], path) > 0) {
		if (is_in_script_path(conn, path)) {
			handle_ssi_file_request(conn, path, file);
		} else {
			/* Script was in an illegal path */
			mg_send_http_error(conn, 403, "%s", "Forbidden");
		}
		return;
	}

#if !defined(NO_CACHING)
	if ((!conn->in_error_handler) && is_not_modified(conn, &file->stat)) {
		/* Send 304 "Not Modified" - this must not send any body data */
		handle_not_modified_static_file_request(conn, file);
		return;
	}
#endif /* !NO_CACHING */

	handle_static_file_request(conn, path, file, NULL, NULL);
}
#endif /* NO_FILESYSTEMS */


static void
close_all_listening_sockets(struct mg_context *ctx)
{
	unsigned int i;
	if (!ctx) {
		return;
	}

	for (i = 0; i < ctx->num_listening_sockets; i++) {
		closesocket(ctx->listening_sockets[i].sock);
#if defined(USE_X_DOM_SOCKET)
		/* For unix domain sockets, the socket name represents a file that has
		 * to be deleted. */
		/* See
		 * https://stackoverflow.com/questions/15716302/so-reuseaddr-and-af-unix
		 */
		if ((ctx->listening_sockets[i].lsa.sin.sin_family == AF_UNIX)
		    && (ctx->listening_sockets[i].sock != INVALID_SOCKET)) {
			IGNORE_UNUSED_RESULT(
			    remove(ctx->listening_sockets[i].lsa.sun.sun_path));
		}
#endif
		ctx->listening_sockets[i].sock = INVALID_SOCKET;
	}
	mg_free(ctx->listening_sockets);
	ctx->listening_sockets = NULL;
	mg_free(ctx->listening_socket_fds);
	ctx->listening_socket_fds = NULL;
}


/* Valid listening port specification is: [ip_address:]port[s]
 * Examples for IPv4: 80, 443s, 127.0.0.1:3128, 192.0.2.3:8080s
 * Examples for IPv6: [::]:80, [::1]:80,
 *   [2001:0db8:7654:3210:FEDC:BA98:7654:3210]:443s
 *   see https://tools.ietf.org/html/rfc3513#section-2.2
 * In order to bind to both, IPv4 and IPv6, you can either add
 * both ports using 8080,[::]:8080, or the short form +8080.
 * Both forms differ in detail: 8080,[::]:8080 create two sockets,
 * one only accepting IPv4 the other only IPv6. +8080 creates
 * one socket accepting IPv4 and IPv6. Depending on the IPv6
 * environment, they might work differently, or might not work
 * at all - it must be tested what options work best in the
 * relevant network environment.
 */
static int
parse_port_string(const struct vec *vec, struct socket *so, int *ip_version)
{
	unsigned int a, b, c, d;
	unsigned port;
	unsigned long portUL;
	int len;
	const char *cb;
	char *endptr;
#if defined(USE_IPV6)
	char buf[100] = {0};
#endif

	/* MacOS needs that. If we do not zero it, subsequent bind() will fail.
	 * Also, all-zeroes in the socket address means binding to all addresses
	 * for both IPv4 and IPv6 (INADDR_ANY and IN6ADDR_ANY_INIT). */
	memset(so, 0, sizeof(*so));
	so->lsa.sin.sin_family = AF_INET;
	*ip_version = 0;

	/* Initialize len as invalid. */
	port = 0;
	len = 0;

	/* Test for different ways to format this string */
	if (sscanf(vec->ptr,
	           "%u.%u.%u.%u:%u%n",
	           &a,
	           &b,
	           &c,
	           &d,
	           &port,
	           &len) // NOLINT(cert-err34-c) 'sscanf' used to convert a string
	                 // to an integer value, but function will not report
	                 // conversion errors; consider using 'strtol' instead
	    == 5) {
		/* Bind to a specific IPv4 address, e.g. 192.168.1.5:8080 */
		so->lsa.sin.sin_addr.s_addr =
		    htonl((a << 24) | (b << 16) | (c << 8) | d);
		so->lsa.sin.sin_port = htons((uint16_t)port);
		*ip_version = 4;

#if defined(USE_IPV6)
	} else if (sscanf(vec->ptr, "[%49[^]]]:%u%n", buf, &port, &len) == 2
	           && ((size_t)len <= vec->len)
	           && mg_inet_pton(
	                  AF_INET6, buf, &so->lsa.sin6, sizeof(so->lsa.sin6), 0)) {
		/* IPv6 address, examples: see above */
		/* so->lsa.sin6.sin6_family = AF_INET6; already set by mg_inet_pton
		 */
		so->lsa.sin6.sin6_port = htons((uint16_t)port);
		*ip_version = 6;
#endif

	} else if ((vec->ptr[0] == '+')
	           && (sscanf(vec->ptr + 1, "%u%n", &port, &len)
	               == 1)) { // NOLINT(cert-err34-c) 'sscanf' used to convert a
		                    // string to an integer value, but function will not
		                    // report conversion errors; consider using 'strtol'
		                    // instead

		/* Port is specified with a +, bind to IPv6 and IPv4, INADDR_ANY */
		/* Add 1 to len for the + character we skipped before */
		len++;

#if defined(USE_IPV6)
		/* Set socket family to IPv6, do not use IPV6_V6ONLY */
		so->lsa.sin6.sin6_family = AF_INET6;
		so->lsa.sin6.sin6_port = htons((uint16_t)port);
		*ip_version = 4 + 6;
#else
		/* Bind to IPv4 only, since IPv6 is not built in. */
		so->lsa.sin.sin_port = htons((uint16_t)port);
		*ip_version = 4;
#endif

	} else if (is_valid_port(portUL = strtoul(vec->ptr, &endptr, 0))
	           && (vec->ptr != endptr)) {
		len = (int)(endptr - vec->ptr);
		port = (uint16_t)portUL;
		/* If only port is specified, bind to IPv4, INADDR_ANY */
		so->lsa.sin.sin_port = htons((uint16_t)port);
		*ip_version = 4;

	} else if ((cb = strchr(vec->ptr, ':')) != NULL) {
		/* String could be a hostname. This check algorithm
		 * will only work for RFC 952 compliant hostnames,
		 * starting with a letter, containing only letters,
		 * digits and hyphen ('-'). Newer specs may allow
		 * more, but this is not guaranteed here, since it
		 * may interfere with rules for port option lists. */

		/* According to RFC 1035, hostnames are restricted to 255 characters
		 * in total (63 between two dots). */
		char hostname[256];
		size_t hostnlen = (size_t)(cb - vec->ptr);

		if ((hostnlen >= vec->len) || (hostnlen >= sizeof(hostname))) {
			/* This would be invalid in any case */
			*ip_version = 0;
			return 0;
		}

		mg_strlcpy(hostname, vec->ptr, hostnlen + 1);

		if (mg_inet_pton(
		        AF_INET, hostname, &so->lsa.sin, sizeof(so->lsa.sin), 1)) {
			if (sscanf(cb + 1, "%u%n", &port, &len)
			    == 1) { // NOLINT(cert-err34-c) 'sscanf' used to convert a
				        // string to an integer value, but function will not
				        // report conversion errors; consider using 'strtol'
				        // instead
				*ip_version = 4;
				so->lsa.sin.sin_port = htons((uint16_t)port);
				len += (int)(hostnlen + 1);
			} else {
				len = 0;
			}
#if defined(USE_IPV6)
		} else if (mg_inet_pton(AF_INET6,
		                        hostname,
		                        &so->lsa.sin6,
		                        sizeof(so->lsa.sin6),
		                        1)) {
			if (sscanf(cb + 1, "%u%n", &port, &len) == 1) {
				*ip_version = 6;
				so->lsa.sin6.sin6_port = htons((uint16_t)port);
				len += (int)(hostnlen + 1);
			} else {
				len = 0;
			}
#endif
		} else {
			len = 0;
		}

#if defined(USE_X_DOM_SOCKET)

	} else if (vec->ptr[0] == 'x') {
		/* unix (linux) domain socket */
		if (vec->len < sizeof(so->lsa.sun.sun_path)) {
			len = vec->len;
			so->lsa.sun.sun_family = AF_UNIX;
			memset(so->lsa.sun.sun_path, 0, sizeof(so->lsa.sun.sun_path));
			memcpy(so->lsa.sun.sun_path, (char *)vec->ptr + 1, vec->len - 1);
			port = 0;
			*ip_version = 99;
		} else {
			/* String too long */
			len = 0;
		}
#endif

	} else {
		/* Parsing failure. */
		len = 0;
	}

	/* sscanf and the option splitting code ensure the following condition
	 * Make sure the port is valid and vector ends with the port, 'o', 's', or
	 * 'r' */
	if ((len > 0) && (is_valid_port(port))) {
		int bad_suffix = 0;
		size_t i;

		/* Parse any suffix character(s) after the port number */
		for (i = len; i < vec->len; i++) {
			unsigned char *opt = NULL;
			switch (vec->ptr[i]) {
			case 'o':
				opt = &so->is_optional;
				break;
			case 'r':
				opt = &so->ssl_redir;
				break;
			case 's':
				opt = &so->is_ssl;
				break;
			default: /* empty */
				break;
			}

			if ((opt) && (*opt == 0))
				*opt = 1;
			else {
				bad_suffix = 1;
				break;
			}
		}

		if ((bad_suffix == 0) && ((so->is_ssl == 0) || (so->ssl_redir == 0))) {
			return 1;
		}
	}

	/* Reset ip_version to 0 if there is an error */
	*ip_version = 0;
	return 0;
}


/* Is there any SSL port in use? */
static int
is_ssl_port_used(const char *ports)
{
	if (ports) {
		/* There are several different allowed syntax variants:
		 * - "80" for a single port using every network interface
		 * - "localhost:80" for a single port using only localhost
		 * - "80,localhost:8080" for two ports, one bound to localhost
		 * - "80,127.0.0.1:8084,[::1]:8086" for three ports, one bound
		 *   to IPv4 localhost, one to IPv6 localhost
		 * - "+80" use port 80 for IPv4 and IPv6
		 * - "+80r,+443s" port 80 (HTTP) is a redirect to port 443 (HTTPS),
		 *   for both: IPv4 and IPv4
		 * - "+443s,localhost:8080" port 443 (HTTPS) for every interface,
		 *   additionally port 8080 bound to localhost connections
		 *
		 * If we just look for 's' anywhere in the string, "localhost:80"
		 * will be detected as SSL (false positive).
		 * Looking for 's' after a digit may cause false positives in
		 * "my24service:8080".
		 * Looking from 's' backward if there are only ':' and numbers
		 * before will not work for "24service:8080" (non SSL, port 8080)
		 * or "24s" (SSL, port 24).
		 *
		 * Remark: Initially hostnames were not allowed to start with a
		 * digit (according to RFC 952), this was allowed later (RFC 1123,
		 * Section 2.1).
		 *
		 * To get this correct, the entire string must be parsed as a whole,
		 * reading it as a list element for element and parsing with an
		 * algorithm equivalent to parse_port_string.
		 *
		 * In fact, we use local interface names here, not arbitrary
		 * hostnames, so in most cases the only name will be "localhost".
		 *
		 * So, for now, we use this simple algorithm, that may still return
		 * a false positive in bizarre cases.
		 */
		int i;
		int portslen = (int)strlen(ports);
		char prevIsNumber = 0;

		for (i = 0; i < portslen; i++) {
			if (prevIsNumber) {
				int suffixCharIdx = (ports[i] == 'o')
				                        ? (i + 1)
				                        : i; /* allow "os" and "or" suffixes */
				if (ports[suffixCharIdx] == 's'
				    || ports[suffixCharIdx] == 'r') {
					return 1;
				}
			}
			if (ports[i] >= '0' && ports[i] <= '9') {
				prevIsNumber = 1;
			} else {
				prevIsNumber = 0;
			}
		}
	}
	return 0;
}


static int
set_ports_option(struct mg_context *phys_ctx)
{
	const char *list;
	int on = 1;
#if defined(USE_IPV6)
	int off = 0;
#endif
	struct vec vec;
	struct socket so, *ptr;

	struct mg_pollfd *pfd;
	union usa usa;
	socklen_t len;
	int ip_version;

	int portsTotal = 0;
	int portsOk = 0;

	const char *opt_txt;
	long opt_listen_backlog;

	if (!phys_ctx) {
		return 0;
	}

	memset(&so, 0, sizeof(so));
	memset(&usa, 0, sizeof(usa));
	len = sizeof(usa);
	list = phys_ctx->dd.config[LISTENING_PORTS];

	while ((list = next_option(list, &vec, NULL)) != NULL) {

		portsTotal++;

		if (!parse_port_string(&vec, &so, &ip_version)) {
			mg_cry_ctx_internal(
			    phys_ctx,
			    "%.*s: invalid port spec (entry %i). Expecting list of: %s",
			    (int)vec.len,
			    vec.ptr,
			    portsTotal,
			    "[IP_ADDRESS:]PORT[s|r]");
			continue;
		}

#if !defined(NO_SSL)
		if (so.is_ssl && phys_ctx->dd.ssl_ctx == NULL) {

			mg_cry_ctx_internal(phys_ctx,
			                    "Cannot add SSL socket (entry %i)",
			                    portsTotal);
			continue;
		}
#endif
		/* Create socket. */
		/* For a list of protocol numbers (e.g., TCP==6) see:
		 * https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
		 */
		if ((so.sock =
		         socket(so.lsa.sa.sa_family,
		                SOCK_STREAM,
		                (ip_version == 99) ? (/* LOCAL */ 0) : (/* TCP */ 6)))
		    == INVALID_SOCKET) {

			mg_cry_ctx_internal(phys_ctx,
			                    "cannot create socket (entry %i)",
			                    portsTotal);
			continue;
		}

#if defined(_WIN32)
		/* Windows SO_REUSEADDR lets many procs binds to a
		 * socket, SO_EXCLUSIVEADDRUSE makes the bind fail
		 * if someone already has the socket -- DTL */
		/* NOTE: If SO_EXCLUSIVEADDRUSE is used,
		 * Windows might need a few seconds before
		 * the same port can be used again in the
		 * same process, so a short Sleep may be
		 * required between mg_stop and mg_start.
		 */
		if (setsockopt(so.sock,
		               SOL_SOCKET,
		               SO_EXCLUSIVEADDRUSE,
		               (SOCK_OPT_TYPE)&on,
		               sizeof(on))
		    != 0) {

			/* Set reuse option, but don't abort on errors. */
			mg_cry_ctx_internal(
			    phys_ctx,
			    "cannot set socket option SO_EXCLUSIVEADDRUSE (entry %i)",
			    portsTotal);
		}
#else
		if (setsockopt(so.sock,
		               SOL_SOCKET,
		               SO_REUSEADDR,
		               (SOCK_OPT_TYPE)&on,
		               sizeof(on))
		    != 0) {

			/* Set reuse option, but don't abort on errors. */
			mg_cry_ctx_internal(
			    phys_ctx,
			    "cannot set socket option SO_REUSEADDR (entry %i)",
			    portsTotal);
		}
#endif

#if defined(USE_X_DOM_SOCKET)
		if (ip_version == 99) {
			/* Unix domain socket */
		} else
#endif

		    if (ip_version > 4) {
			/* Could be 6 for IPv6 onlyor 10 (4+6) for IPv4+IPv6 */
#if defined(USE_IPV6)
			if (ip_version > 6) {
				if (so.lsa.sa.sa_family == AF_INET6
				    && setsockopt(so.sock,
				                  IPPROTO_IPV6,
				                  IPV6_V6ONLY,
				                  (void *)&off,
				                  sizeof(off))
				           != 0) {

					/* Set IPv6 only option, but don't abort on errors. */
					mg_cry_ctx_internal(phys_ctx,
					                    "cannot set socket option "
					                    "IPV6_V6ONLY=off (entry %i)",
					                    portsTotal);
				}
			} else {
				if (so.lsa.sa.sa_family == AF_INET6
				    && setsockopt(so.sock,
				                  IPPROTO_IPV6,
				                  IPV6_V6ONLY,
				                  (void *)&on,
				                  sizeof(on))
				           != 0) {

					/* Set IPv6 only option, but don't abort on errors. */
					mg_cry_ctx_internal(phys_ctx,
					                    "cannot set socket option "
					                    "IPV6_V6ONLY=on (entry %i)",
					                    portsTotal);
				}
			}
#else
			mg_cry_ctx_internal(phys_ctx, "%s", "IPv6 not available");
			closesocket(so.sock);
			so.sock = INVALID_SOCKET;
			continue;
#endif
		}

		if (so.lsa.sa.sa_family == AF_INET) {

			len = sizeof(so.lsa.sin);
			if (bind(so.sock, &so.lsa.sa, len) != 0) {
				mg_cry_ctx_internal(phys_ctx,
				                    "cannot bind to %.*s: %d (%s)",
				                    (int)vec.len,
				                    vec.ptr,
				                    (int)ERRNO,
				                    strerror(errno));
				closesocket(so.sock);
				so.sock = INVALID_SOCKET;
				if (so.is_optional) {
					portsOk++; /* it's okay if we couldn't bind, this port is
					              optional anyway */
				}
				continue;
			}
		}
#if defined(USE_IPV6)
		else if (so.lsa.sa.sa_family == AF_INET6) {

			len = sizeof(so.lsa.sin6);
			if (bind(so.sock, &so.lsa.sa, len) != 0) {
				mg_cry_ctx_internal(phys_ctx,
				                    "cannot bind to IPv6 %.*s: %d (%s)",
				                    (int)vec.len,
				                    vec.ptr,
				                    (int)ERRNO,
				                    strerror(errno));
				closesocket(so.sock);
				so.sock = INVALID_SOCKET;
				if (so.is_optional) {
					portsOk++; /* it's okay if we couldn't bind, this port is
					              optional anyway */
				}
				continue;
			}
		}
#endif
#if defined(USE_X_DOM_SOCKET)
		else if (so.lsa.sa.sa_family == AF_UNIX) {

			len = sizeof(so.lsa.sun);
			if (bind(so.sock, &so.lsa.sa, len) != 0) {
				mg_cry_ctx_internal(phys_ctx,
				                    "cannot bind to unix socket %s: %d (%s)",
				                    so.lsa.sun.sun_path,
				                    (int)ERRNO,
				                    strerror(errno));
				closesocket(so.sock);
				so.sock = INVALID_SOCKET;
				if (so.is_optional) {
					portsOk++; /* it's okay if we couldn't bind, this port is
					              optional anyway */
				}
				continue;
			}
		}
#endif
		else {
			mg_cry_ctx_internal(
			    phys_ctx,
			    "cannot bind: address family not supported (entry %i)",
			    portsTotal);
			closesocket(so.sock);
			so.sock = INVALID_SOCKET;
			continue;
		}

		opt_txt = phys_ctx->dd.config[LISTEN_BACKLOG_SIZE];
		opt_listen_backlog = strtol(opt_txt, NULL, 10);
		if ((opt_listen_backlog > INT_MAX) || (opt_listen_backlog < 1)) {
			mg_cry_ctx_internal(phys_ctx,
			                    "%s value \"%s\" is invalid",
			                    config_options[LISTEN_BACKLOG_SIZE].name,
			                    opt_txt);
			closesocket(so.sock);
			so.sock = INVALID_SOCKET;
			continue;
		}

		if (listen(so.sock, (int)opt_listen_backlog) != 0) {

			mg_cry_ctx_internal(phys_ctx,
			                    "cannot listen to %.*s: %d (%s)",
			                    (int)vec.len,
			                    vec.ptr,
			                    (int)ERRNO,
			                    strerror(errno));
			closesocket(so.sock);
			so.sock = INVALID_SOCKET;
			continue;
		}

		if ((getsockname(so.sock, &(usa.sa), &len) != 0)
		    || (usa.sa.sa_family != so.lsa.sa.sa_family)) {

			int err = (int)ERRNO;
			mg_cry_ctx_internal(phys_ctx,
			                    "call to getsockname failed %.*s: %d (%s)",
			                    (int)vec.len,
			                    vec.ptr,
			                    err,
			                    strerror(errno));
			closesocket(so.sock);
			so.sock = INVALID_SOCKET;
			continue;
		}

		/* Update lsa port in case of random free ports */
#if defined(USE_IPV6)
		if (so.lsa.sa.sa_family == AF_INET6) {
			so.lsa.sin6.sin6_port = usa.sin6.sin6_port;
		} else
#endif
		{
			so.lsa.sin.sin_port = usa.sin.sin_port;
		}

		if ((ptr = (struct socket *)
		         mg_realloc_ctx(phys_ctx->listening_sockets,
		                        (phys_ctx->num_listening_sockets + 1)
		                            * sizeof(phys_ctx->listening_sockets[0]),
		                        phys_ctx))
		    == NULL) {

			mg_cry_ctx_internal(phys_ctx, "%s", "Out of memory");
			closesocket(so.sock);
			so.sock = INVALID_SOCKET;
			continue;
		}

		/* The +2 below includes the original +1 (for the socket we're about to
		 * add), plus another +1 for the thread_shutdown_notification_socket
		 * that we'll also want to poll() on so that mg_stop() can return
		 * quickly
		 */
		if ((pfd = (struct mg_pollfd *)
		         mg_realloc_ctx(phys_ctx->listening_socket_fds,
		                        (phys_ctx->num_listening_sockets + 2)
		                            * sizeof(phys_ctx->listening_socket_fds[0]),
		                        phys_ctx))
		    == NULL) {

			mg_cry_ctx_internal(phys_ctx, "%s", "Out of memory");
			closesocket(so.sock);
			so.sock = INVALID_SOCKET;
			mg_free(ptr);
			continue;
		}

		set_close_on_exec(so.sock, NULL, phys_ctx);
		phys_ctx->listening_sockets = ptr;
		phys_ctx->listening_sockets[phys_ctx->num_listening_sockets] = so;
		phys_ctx->listening_socket_fds = pfd;
		phys_ctx->num_listening_sockets++;
		portsOk++;
	}

	if (portsOk != portsTotal) {
		close_all_listening_sockets(phys_ctx);
		portsOk = 0;
	}

	return portsOk;
}


static const char *
header_val(const struct mg_connection *conn, const char *header)
{
	const char *header_value;

	if ((header_value = mg_get_header(conn, header)) == NULL) {
		return "-";
	} else {
		return header_value;
	}
}


#if defined(MG_EXTERNAL_FUNCTION_log_access)
#include "external_log_access.inl"
#elif !defined(NO_FILESYSTEMS)

static void
log_access(const struct mg_connection *conn)
{
	const struct mg_request_info *ri;
	struct mg_file fi;
	char date[64], src_addr[IP_ADDR_STR_LEN];
#if defined(REENTRANT_TIME)
	struct tm _tm;
	struct tm *tm = &_tm;
#else
	struct tm *tm;
#endif

	const char *referer;
	const char *user_agent;

	char log_buf[4096];

	if (!conn || !conn->dom_ctx) {
		return;
	}

	/* Set log message to "empty" */
	log_buf[0] = 0;

#if defined(USE_LUA)
	if (conn->phys_ctx->lua_bg_log_available) {
		int ret;
		struct mg_context *ctx = conn->phys_ctx;
		lua_State *lstate = (lua_State *)ctx->lua_background_state;
		pthread_mutex_lock(&ctx->lua_bg_mutex);
		/* call "log()" in Lua */
		lua_getglobal(lstate, "log");
		prepare_lua_request_info_inner(conn, lstate);
		push_lua_response_log_data(conn, lstate);

		ret = lua_pcall(lstate, /* args */ 2, /* results */ 1, 0);
		if (ret == 0) {
			int t = lua_type(lstate, -1);
			if (t == LUA_TBOOLEAN) {
				if (lua_toboolean(lstate, -1) == 0) {
					/* log() returned false: do not log */
					pthread_mutex_unlock(&ctx->lua_bg_mutex);
					return;
				}
				/* log returned true: continue logging */
			} else if (t == LUA_TSTRING) {
				size_t len;
				const char *txt = lua_tolstring(lstate, -1, &len);
				if ((len == 0) || (*txt == 0)) {
					/* log() returned empty string: do not log */
					pthread_mutex_unlock(&ctx->lua_bg_mutex);
					return;
				}
				/* Copy test from Lua into log_buf */
				if (len >= sizeof(log_buf)) {
					len = sizeof(log_buf) - 1;
				}
				memcpy(log_buf, txt, len);
				log_buf[len] = 0;
			}
		} else {
			lua_cry(conn, ret, lstate, "lua_background_script", "log");
		}
		pthread_mutex_unlock(&ctx->lua_bg_mutex);
	}
#endif

	if (conn->dom_ctx->config[ACCESS_LOG_FILE] != NULL) {
		if (mg_fopen(conn,
		             conn->dom_ctx->config[ACCESS_LOG_FILE],
		             MG_FOPEN_MODE_APPEND,
		             &fi)
		    == 0) {
			fi.access.fp = NULL;
		}
	} else {
		fi.access.fp = NULL;
	}

	/* Log is written to a file and/or a callback. If both are not set,
	 * executing the rest of the function is pointless. */
	if ((fi.access.fp == NULL)
	    && (conn->phys_ctx->callbacks.log_access == NULL)) {
		return;
	}

	/* If we did not get a log message from Lua, create it here. */
	if (!log_buf[0]) {
#if defined(REENTRANT_TIME)
		localtime_r(&conn->conn_birth_time, tm);
#else
		tm = localtime(&conn->conn_birth_time);
#endif
		if (tm != NULL) {
			strftime(date, sizeof(date), "%d/%b/%Y:%H:%M:%S %z", tm);
		} else {
			mg_strlcpy(date, "01/Jan/1970:00:00:00 +0000", sizeof(date));
		}

		ri = &conn->request_info;

		sockaddr_to_string(src_addr, sizeof(src_addr), &conn->client.rsa);
		referer = header_val(conn, "Referer");
		user_agent = header_val(conn, "User-Agent");

		mg_snprintf(conn,
		            NULL, /* Ignore truncation in access log */
		            log_buf,
		            sizeof(log_buf),
		            "%s - %s [%s] \"%s %s%s%s HTTP/%s\" %d %" INT64_FMT
		            " %s %s",
		            src_addr,
		            (ri->remote_user == NULL) ? "-" : ri->remote_user,
		            date,
		            ri->request_method ? ri->request_method : "-",
		            ri->request_uri ? ri->request_uri : "-",
		            ri->query_string ? "?" : "",
		            ri->query_string ? ri->query_string : "",
		            ri->http_version,
		            conn->status_code,
		            conn->num_bytes_sent,
		            referer,
		            user_agent);
	}

	/* Here we have a log message in log_buf. Call the callback */
	if (conn->phys_ctx->callbacks.log_access) {
		if (conn->phys_ctx->callbacks.log_access(conn, log_buf)) {
			/* do not log if callback returns non-zero */
			if (fi.access.fp) {
				mg_fclose(&fi.access);
			}
			return;
		}
	}

	/* Store in file */
	if (fi.access.fp) {
		int ok = 1;
		flockfile(fi.access.fp);
		if (fprintf(fi.access.fp, "%s\n", log_buf) < 1) {
			ok = 0;
		}
		if (fflush(fi.access.fp) != 0) {
			ok = 0;
		}
		funlockfile(fi.access.fp);
		if (mg_fclose(&fi.access) != 0) {
			ok = 0;
		}
		if (!ok) {
			mg_cry_internal(conn,
			                "Error writing log file %s",
			                conn->dom_ctx->config[ACCESS_LOG_FILE]);
		}
	}
}
#else
#error "Either enable filesystems or provide a custom log_access implementation"
#endif /* Externally provided function */


/* Verify given socket address against the ACL.
 * Return -1 if ACL is malformed, 0 if address is disallowed, 1 if allowed.
 */
static int
check_acl(struct mg_context *phys_ctx, const union usa *sa)
{
	int allowed, flag, matched;
	struct vec vec;

	if (phys_ctx) {
		const char *list = phys_ctx->dd.config[ACCESS_CONTROL_LIST];

		/* If any ACL is set, deny by default */
		allowed = (list == NULL) ? '+' : '-';

		while ((list = next_option(list, &vec, NULL)) != NULL) {
			flag = vec.ptr[0];
			matched = -1;
			if ((vec.len > 0) && ((flag == '+') || (flag == '-'))) {
				vec.ptr++;
				vec.len--;
				matched = parse_match_net(&vec, sa, 1);
			}
			if (matched < 0) {
				mg_cry_ctx_internal(phys_ctx,
				                    "%s: subnet must be [+|-]IP-addr[/x]",
				                    __func__);
				return -1;
			}
			if (matched) {
				allowed = flag;
			}
		}

		return allowed == '+';
	}
	return -1;
}


#if !defined(_WIN32) && !defined(__ZEPHYR__)
static int
set_uid_option(struct mg_context *phys_ctx)
{
	int success = 0;

	if (phys_ctx) {
		/* We are currently running as curr_uid. */
		const uid_t curr_uid = getuid();
		/* If set, we want to run as run_as_user. */
		const char *run_as_user = phys_ctx->dd.config[RUN_AS_USER];
		const struct passwd *to_pw = NULL;

		if ((run_as_user != NULL) && (to_pw = getpwnam(run_as_user)) == NULL) {
			/* run_as_user does not exist on the system. We can't proceed
			 * further. */
			mg_cry_ctx_internal(phys_ctx,
			                    "%s: unknown user [%s]",
			                    __func__,
			                    run_as_user);
		} else if ((run_as_user == NULL) || (curr_uid == to_pw->pw_uid)) {
			/* There was either no request to change user, or we're already
			 * running as run_as_user. Nothing else to do.
			 */
			success = 1;
		} else {
			/* Valid change request.  */
			if (setgid(to_pw->pw_gid) == -1) {
				mg_cry_ctx_internal(phys_ctx,
				                    "%s: setgid(%s): %s",
				                    __func__,
				                    run_as_user,
				                    strerror(errno));
			} else if (setgroups(0, NULL) == -1) {
				mg_cry_ctx_internal(phys_ctx,
				                    "%s: setgroups(): %s",
				                    __func__,
				                    strerror(errno));
			} else if (setuid(to_pw->pw_uid) == -1) {
				mg_cry_ctx_internal(phys_ctx,
				                    "%s: setuid(%s): %s",
				                    __func__,
				                    run_as_user,
				                    strerror(errno));
			} else {
				success = 1;
			}
		}
	}

	return success;
}
#endif /* !_WIN32 */


static void
tls_dtor(void *key)
{
	struct mg_workerTLS *tls = (struct mg_workerTLS *)key;
	/* key == pthread_getspecific(sTlsKey); */

	if (tls) {
		if (tls->is_master == 2) {
			tls->is_master = -3; /* Mark memory as dead */
			mg_free(tls);
		}
	}
	pthread_setspecific(sTlsKey, NULL);
}


#if defined(USE_MBEDTLS)
/* Check if SSL is required.
 * If so, set up ctx->ssl_ctx pointer. */
static int
mg_sslctx_init(struct mg_context *phys_ctx, struct mg_domain_context *dom_ctx)
{
	if (!phys_ctx) {
		return 0;
	}

	if (!dom_ctx) {
		dom_ctx = &(phys_ctx->dd);
	}

	if (!is_ssl_port_used(dom_ctx->config[LISTENING_PORTS])) {
		/* No SSL port is set. No need to setup SSL. */
		return 1;
	}

	dom_ctx->ssl_ctx = (SSL_CTX *)mg_calloc(1, sizeof(*dom_ctx->ssl_ctx));
	if (dom_ctx->ssl_ctx == NULL) {
		fprintf(stderr, "ssl_ctx malloc failed\n");
		return 0;
	}

	return mbed_sslctx_init(dom_ctx->ssl_ctx, dom_ctx->config[SSL_CERTIFICATE])
	               == 0
	           ? 1
	           : 0;
}

#elif defined(USE_GNUTLS)
/* Check if SSL is required.
 * If so, set up ctx->ssl_ctx pointer. */
static int
mg_sslctx_init(struct mg_context *phys_ctx, struct mg_domain_context *dom_ctx)
{
	if (!phys_ctx) {
		return 0;
	}

	if (!dom_ctx) {
		dom_ctx = &(phys_ctx->dd);
	}

	if (!is_ssl_port_used(dom_ctx->config[LISTENING_PORTS])) {
		/* No SSL port is set. No need to setup SSL. */
		return 1;
	}

	dom_ctx->ssl_ctx = (SSL_CTX *)mg_calloc(1, sizeof(*dom_ctx->ssl_ctx));
	if (dom_ctx->ssl_ctx == NULL) {
		fprintf(stderr, "ssl_ctx malloc failed\n");
		return 0;
	}

	return gtls_sslctx_init(dom_ctx->ssl_ctx, dom_ctx->config[SSL_CERTIFICATE])
	               == 0
	           ? 1
	           : 0;
}

#elif !defined(NO_SSL)

static int ssl_use_pem_file(struct mg_context *phys_ctx,
                            struct mg_domain_context *dom_ctx,
                            const char *pem,
                            const char *chain);
static const char *ssl_error(void);


static int
refresh_trust(struct mg_connection *conn)
{
	struct stat cert_buf;
	int64_t t = 0;
	const char *pem;
	const char *chain;
	int should_verify_peer;

	if ((pem = conn->dom_ctx->config[SSL_CERTIFICATE]) == NULL) {
		/* If pem is NULL and conn->phys_ctx->callbacks.init_ssl is not,
		 * refresh_trust still can not work. */
		return 0;
	}
	chain = conn->dom_ctx->config[SSL_CERTIFICATE_CHAIN];
	if (chain == NULL) {
		/* pem is not NULL here */
		chain = pem;
	}
	if (*chain == 0) {
		chain = NULL;
	}

	if (stat(pem, &cert_buf) != -1) {
		t = (int64_t)cert_buf.st_mtime;
	}

	mg_lock_context(conn->phys_ctx);
	if ((t != 0) && (conn->dom_ctx->ssl_cert_last_mtime != t)) {
		conn->dom_ctx->ssl_cert_last_mtime = t;

		should_verify_peer = 0;
		if (conn->dom_ctx->config[SSL_DO_VERIFY_PEER] != NULL) {
			if (mg_strcasecmp(conn->dom_ctx->config[SSL_DO_VERIFY_PEER], "yes")
			    == 0) {
				should_verify_peer = 1;
			} else if (mg_strcasecmp(conn->dom_ctx->config[SSL_DO_VERIFY_PEER],
			                         "optional")
			           == 0) {
				should_verify_peer = 1;
			}
		}

		if (should_verify_peer) {
			char *ca_path = conn->dom_ctx->config[SSL_CA_PATH];
			char *ca_file = conn->dom_ctx->config[SSL_CA_FILE];
			if (SSL_CTX_load_verify_locations(conn->dom_ctx->ssl_ctx,
			                                  ca_file,
			                                  ca_path)
			    != 1) {
				mg_unlock_context(conn->phys_ctx);
				mg_cry_ctx_internal(
				    conn->phys_ctx,
				    "SSL_CTX_load_verify_locations error: %s "
				    "ssl_verify_peer requires setting "
				    "either ssl_ca_path or ssl_ca_file. Is any of them "
				    "present in "
				    "the .conf file?",
				    ssl_error());
				return 0;
			}
		}

		if (ssl_use_pem_file(conn->phys_ctx, conn->dom_ctx, pem, chain) == 0) {
			mg_unlock_context(conn->phys_ctx);
			return 0;
		}
	}
	mg_unlock_context(conn->phys_ctx);

	return 1;
}

#if defined(OPENSSL_API_1_1)
#else
static pthread_mutex_t *ssl_mutexes;
#endif /* OPENSSL_API_1_1 */

static int
sslize(struct mg_connection *conn,
       int (*func)(SSL *),
       const struct mg_client_options *client_options)
{
	int ret, err;
	int short_trust;
	unsigned timeout = 1024;
	unsigned i;

	if (!conn) {
		return 0;
	}

	short_trust =
	    (conn->dom_ctx->config[SSL_SHORT_TRUST] != NULL)
	    && (mg_strcasecmp(conn->dom_ctx->config[SSL_SHORT_TRUST], "yes") == 0);

	if (short_trust) {
		int trust_ret = refresh_trust(conn);
		if (!trust_ret) {
			return trust_ret;
		}
	}

	mg_lock_context(conn->phys_ctx);
	conn->ssl = SSL_new(conn->dom_ctx->ssl_ctx);
	mg_unlock_context(conn->phys_ctx);
	if (conn->ssl == NULL) {
		mg_cry_internal(conn, "sslize error: %s", ssl_error());
		OPENSSL_REMOVE_THREAD_STATE();
		return 0;
	}
	SSL_set_app_data(conn->ssl, (char *)conn);

	ret = SSL_set_fd(conn->ssl, conn->client.sock);
	if (ret != 1) {
		mg_cry_internal(conn, "sslize error: %s", ssl_error());
		SSL_free(conn->ssl);
		conn->ssl = NULL;
		OPENSSL_REMOVE_THREAD_STATE();
		return 0;
	}

	if (client_options) {
		if (client_options->host_name) {
			SSL_set_tlsext_host_name(conn->ssl, client_options->host_name);
		}
	}

	/* Reuse the request timeout for the SSL_Accept/SSL_connect timeout  */
	if (conn->dom_ctx->config[REQUEST_TIMEOUT]) {
		/* NOTE: The loop below acts as a back-off, so we can end
		 * up sleeping for more (or less) than the REQUEST_TIMEOUT. */
		int to = atoi(conn->dom_ctx->config[REQUEST_TIMEOUT]);
		if (to >= 0) {
			timeout = (unsigned)to;
		}
	}

	/* SSL functions may fail and require to be called again:
	 * see https://www.openssl.org/docs/manmaster/ssl/SSL_get_error.html
	 * Here "func" could be SSL_connect or SSL_accept. */
	for (i = 0; i <= timeout; i += 50) {
		ERR_clear_error();
		/* conn->dom_ctx may be changed here (see ssl_servername_callback) */
		ret = func(conn->ssl);
		if (ret != 1) {
			err = SSL_get_error(conn->ssl, ret);
			if ((err == SSL_ERROR_WANT_CONNECT)
			    || (err == SSL_ERROR_WANT_ACCEPT)
			    || (err == SSL_ERROR_WANT_READ) || (err == SSL_ERROR_WANT_WRITE)
			    || (err == SSL_ERROR_WANT_X509_LOOKUP)) {
				if (!STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)) {
					/* Don't wait if the server is going to be stopped. */
					break;
				}
				if (err == SSL_ERROR_WANT_X509_LOOKUP) {
					/* Simply retry the function call. */
					mg_sleep(50);
				} else {
					/* Need to retry the function call "later".
					 * See https://linux.die.net/man/3/ssl_get_error
					 * This is typical for non-blocking sockets. */
					struct mg_pollfd pfd[2];
					int pollres;
					unsigned int num_sock = 1;
					pfd[0].fd = conn->client.sock;
					pfd[0].events = ((err == SSL_ERROR_WANT_CONNECT)
					                 || (err == SSL_ERROR_WANT_WRITE))
					                    ? POLLOUT
					                    : POLLIN;

					if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
						pfd[num_sock].fd =
						    conn->phys_ctx->thread_shutdown_notification_socket;
						pfd[num_sock].events = POLLIN;
						num_sock++;
					}

					pollres = mg_poll(pfd,
					                  num_sock,
					                  50,
					                  &(conn->phys_ctx->stop_flag));
					if (pollres < 0) {
						/* Break if error occurred (-1)
						 * or server shutdown (-2) */
						break;
					}
				}

			} else if (err == SSL_ERROR_SYSCALL) {
				/* This is an IO error. Look at errno. */
				mg_cry_internal(conn, "SSL syscall error %i", ERRNO);
				break;

			} else {
				/* This is an SSL specific error, e.g. SSL_ERROR_SSL */
				mg_cry_internal(conn, "sslize error: %s", ssl_error());
				break;
			}

		} else {
			/* success */
			break;
		}
	}
	ERR_clear_error();

	if (ret != 1) {
		SSL_free(conn->ssl);
		conn->ssl = NULL;
		OPENSSL_REMOVE_THREAD_STATE();
		return 0;
	}

	return 1;
}


/* Return OpenSSL error message (from CRYPTO lib) */
static const char *
ssl_error(void)
{
	unsigned long err;
	err = ERR_get_error();
	return ((err == 0) ? "" : ERR_error_string(err, NULL));
}


static int
hexdump2string(void *mem, int memlen, char *buf, int buflen)
{
	int i;
	const char hexdigit[] = "0123456789abcdef";

	if ((memlen <= 0) || (buflen <= 0)) {
		return 0;
	}
	if (buflen < (3 * memlen)) {
		return 0;
	}

	for (i = 0; i < memlen; i++) {
		if (i > 0) {
			buf[3 * i - 1] = ' ';
		}
		buf[3 * i] = hexdigit[(((uint8_t *)mem)[i] >> 4) & 0xF];
		buf[3 * i + 1] = hexdigit[((uint8_t *)mem)[i] & 0xF];
	}
	buf[3 * memlen - 1] = 0;

	return 1;
}


static int
ssl_get_client_cert_info(const struct mg_connection *conn,
                         struct mg_client_cert *client_cert)
{
	X509 *cert = SSL_get_peer_certificate(conn->ssl);
	if (cert) {
		char str_buf[1024];
		unsigned char buf[256];
		char *str_serial = NULL;
		unsigned int ulen;
		int ilen;
		unsigned char *tmp_buf;
		unsigned char *tmp_p;

		/* Handle to algorithm used for fingerprint */
		const EVP_MD *digest = EVP_get_digestbyname("sha1");

		/* Get Subject and issuer */
		X509_NAME *subj = X509_get_subject_name(cert);
		X509_NAME *iss = X509_get_issuer_name(cert);

		/* Get serial number */
		ASN1_INTEGER *serial = X509_get_serialNumber(cert);

		/* Translate serial number to a hex string */
		BIGNUM *serial_bn = ASN1_INTEGER_to_BN(serial, NULL);
		if (serial_bn) {
			str_serial = BN_bn2hex(serial_bn);
			BN_free(serial_bn);
		}
		client_cert->serial =
		    str_serial ? mg_strdup_ctx(str_serial, conn->phys_ctx) : NULL;

		/* Translate subject and issuer to a string */
		(void)X509_NAME_oneline(subj, str_buf, (int)sizeof(str_buf));
		client_cert->subject = mg_strdup_ctx(str_buf, conn->phys_ctx);
		(void)X509_NAME_oneline(iss, str_buf, (int)sizeof(str_buf));
		client_cert->issuer = mg_strdup_ctx(str_buf, conn->phys_ctx);

		/* Calculate SHA1 fingerprint and store as a hex string */
		ulen = 0;

		/* ASN1_digest is deprecated. Do the calculation manually,
		 * using EVP_Digest. */
		ilen = i2d_X509(cert, NULL);
		tmp_buf = (ilen > 0)
		              ? (unsigned char *)mg_malloc_ctx((unsigned)ilen + 1,
		                                               conn->phys_ctx)
		              : NULL;
		if (tmp_buf) {
			tmp_p = tmp_buf;
			(void)i2d_X509(cert, &tmp_p);
			if (!EVP_Digest(
			        tmp_buf, (unsigned)ilen, buf, &ulen, digest, NULL)) {
				ulen = 0;
			}
			mg_free(tmp_buf);
		}

		if (!hexdump2string(buf, (int)ulen, str_buf, (int)sizeof(str_buf))) {
			*str_buf = 0;
		}
		client_cert->finger = mg_strdup_ctx(str_buf, conn->phys_ctx);

		client_cert->peer_cert = (void *)cert;

		/* Strings returned from bn_bn2hex must be freed using OPENSSL_free,
		 * see https://linux.die.net/man/3/bn_bn2hex */
		OPENSSL_free(str_serial);
		return 1;
	}
	return 0;
}


#if defined(OPENSSL_API_1_1)
#else
static void
ssl_locking_callback(int mode, int mutex_num, const char *file, int line)
{
	(void)line;
	(void)file;

	if (mode & 1) {
		/* 1 is CRYPTO_LOCK */
		(void)pthread_mutex_lock(&ssl_mutexes[mutex_num]);
	} else {
		(void)pthread_mutex_unlock(&ssl_mutexes[mutex_num]);
	}
}
#endif /* OPENSSL_API_1_1 */


#if !defined(NO_SSL_DL)
/* Load a DLL/Shared Object with a TLS/SSL implementation. */
static void *
load_tls_dll(char *ebuf,
             size_t ebuf_len,
             const char *dll_name,
             struct ssl_func *sw,
             int *feature_missing)
{
	union {
		void *p;
		void (*fp)(void);
	} u;
	void *dll_handle;
	struct ssl_func *fp;
	int ok;
	int truncated = 0;

	if ((dll_handle = dlopen(dll_name, RTLD_LAZY)) == NULL) {
		mg_snprintf(NULL,
		            NULL, /* No truncation check for ebuf */
		            ebuf,
		            ebuf_len,
		            "%s: cannot load %s",
		            __func__,
		            dll_name);
		return NULL;
	}

	ok = 1;
	for (fp = sw; fp->name != NULL; fp++) {
#if defined(_WIN32)
		/* GetProcAddress() returns pointer to function */
		u.fp = (void (*)(void))dlsym(dll_handle, fp->name);
#else
		/* dlsym() on UNIX returns void *. ISO C forbids casts of data
		 * pointers to function pointers. We need to use a union to make a
		 * cast. */
		u.p = dlsym(dll_handle, fp->name);
#endif /* _WIN32 */

		/* Set pointer (might be NULL) */
		fp->ptr = u.fp;

		if (u.fp == NULL) {
			DEBUG_TRACE("Missing function: %s\n", fp->name);
			if (feature_missing) {
				feature_missing[fp->required]++;
			}
			if (fp->required == TLS_Mandatory) {
				/* Mandatory function is missing */
				if (ok) {
					/* This is the first missing function.
					 * Create a new error message. */
					mg_snprintf(NULL,
					            &truncated,
					            ebuf,
					            ebuf_len,
					            "%s: %s: cannot find %s",
					            __func__,
					            dll_name,
					            fp->name);
					ok = 0;
				} else {
					/* This is yet anothermissing function.
					 * Append existing error message. */
					size_t cur_len = strlen(ebuf);
					if (!truncated && ((ebuf_len - cur_len) > 3)) {
						mg_snprintf(NULL,
						            &truncated,
						            ebuf + cur_len,
						            ebuf_len - cur_len - 3,
						            ", %s",
						            fp->name);
						if (truncated) {
							/* If truncated, add "..." */
							strcat(ebuf, "...");
						}
					}
				}
			}
		}
	}

	if (!ok) {
		(void)dlclose(dll_handle);
		return NULL;
	}

	return dll_handle;
}


static void *ssllib_dll_handle;    /* Store the ssl library handle. */
static void *cryptolib_dll_handle; /* Store the crypto library handle. */

#endif /* NO_SSL_DL */


#if defined(SSL_ALREADY_INITIALIZED)
static volatile ptrdiff_t cryptolib_users =
    1; /* Reference counter for crypto library. */
#else
static volatile ptrdiff_t cryptolib_users =
    0; /* Reference counter for crypto library. */
#endif


static int
initialize_openssl(char *ebuf, size_t ebuf_len)
{
#if !defined(OPENSSL_API_1_1) && !defined(OPENSSL_API_3_0)
	int i, num_locks;
	size_t size;
#endif

	if (ebuf_len > 0) {
		ebuf[0] = 0;
	}

#if !defined(NO_SSL_DL)
	if (!cryptolib_dll_handle) {
		memset(tls_feature_missing, 0, sizeof(tls_feature_missing));
		cryptolib_dll_handle = load_tls_dll(
		    ebuf, ebuf_len, CRYPTO_LIB, crypto_sw, tls_feature_missing);
		if (!cryptolib_dll_handle) {
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s: error loading library %s",
			            __func__,
			            CRYPTO_LIB);
			DEBUG_TRACE("%s", ebuf);
			return 0;
		}
	}
#endif /* NO_SSL_DL */

	if (mg_atomic_inc(&cryptolib_users) > 1) {
		return 1;
	}

#if !defined(OPENSSL_API_1_1) && !defined(OPENSSL_API_3_0)
	/* Initialize locking callbacks, needed for thread safety.
	 * http://www.openssl.org/support/faq.html#PROG1
	 */
	num_locks = CRYPTO_num_locks();
	if (num_locks < 0) {
		num_locks = 0;
	}
	size = sizeof(pthread_mutex_t) * ((size_t)(num_locks));

	/* allocate mutex array, if required */
	if (num_locks == 0) {
		/* No mutex array required */
		ssl_mutexes = NULL;
	} else {
		/* Mutex array required - allocate it */
		ssl_mutexes = (pthread_mutex_t *)mg_malloc(size);

		/* Check OOM */
		if (ssl_mutexes == NULL) {
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s: cannot allocate mutexes: %s",
			            __func__,
			            ssl_error());
			DEBUG_TRACE("%s", ebuf);
			return 0;
		}

		/* initialize mutex array */
		for (i = 0; i < num_locks; i++) {
			if (0 != pthread_mutex_init(&ssl_mutexes[i], &pthread_mutex_attr)) {
				mg_snprintf(NULL,
				            NULL, /* No truncation check for ebuf */
				            ebuf,
				            ebuf_len,
				            "%s: error initializing mutex %i of %i",
				            __func__,
				            i,
				            num_locks);
				DEBUG_TRACE("%s", ebuf);
				mg_free(ssl_mutexes);
				return 0;
			}
		}
	}

	CRYPTO_set_locking_callback(&ssl_locking_callback);
	CRYPTO_set_id_callback(&mg_current_thread_id);
#endif /* OPENSSL_API_1_1 || OPENSSL_API_3_0 */

#if !defined(NO_SSL_DL)
	if (!ssllib_dll_handle) {
		ssllib_dll_handle =
		    load_tls_dll(ebuf, ebuf_len, SSL_LIB, ssl_sw, tls_feature_missing);
		if (!ssllib_dll_handle) {
#if !defined(OPENSSL_API_1_1)
			mg_free(ssl_mutexes);
#endif
			DEBUG_TRACE("%s", ebuf);
			return 0;
		}
	}
#endif /* NO_SSL_DL */

#if (defined(OPENSSL_API_1_1) || defined(OPENSSL_API_3_0))                     \
    && !defined(NO_SSL_DL)
	/* Initialize SSL library */
	OPENSSL_init_ssl(0, NULL);
	OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS
	                     | OPENSSL_INIT_LOAD_CRYPTO_STRINGS,
	                 NULL);
#else
	/* Initialize SSL library */
	SSL_library_init();
	SSL_load_error_strings();
#endif

	return 1;
}


static int
ssl_use_pem_file(struct mg_context *phys_ctx,
                 struct mg_domain_context *dom_ctx,
                 const char *pem,
                 const char *chain)
{
	if (SSL_CTX_use_certificate_file(dom_ctx->ssl_ctx, pem, 1) == 0) {
		mg_cry_ctx_internal(phys_ctx,
		                    "%s: cannot open certificate file %s: %s",
		                    __func__,
		                    pem,
		                    ssl_error());
		return 0;
	}

	/* could use SSL_CTX_set_default_passwd_cb_userdata */
	if (SSL_CTX_use_PrivateKey_file(dom_ctx->ssl_ctx, pem, 1) == 0) {
		mg_cry_ctx_internal(phys_ctx,
		                    "%s: cannot open private key file %s: %s",
		                    __func__,
		                    pem,
		                    ssl_error());
		return 0;
	}

	if (SSL_CTX_check_private_key(dom_ctx->ssl_ctx) == 0) {
		mg_cry_ctx_internal(phys_ctx,
		                    "%s: certificate and private key do not match: %s",
		                    __func__,
		                    pem);
		return 0;
	}

	/* In contrast to OpenSSL, wolfSSL does not support certificate
	 * chain files that contain private keys and certificates in
	 * SSL_CTX_use_certificate_chain_file.
	 * The CivetWeb-Server used pem-Files that contained both information.
	 * In order to make wolfSSL work, it is split in two files.
	 * One file that contains key and certificate used by the server and
	 * an optional chain file for the ssl stack.
	 */
	if (chain) {
		if (SSL_CTX_use_certificate_chain_file(dom_ctx->ssl_ctx, chain) == 0) {
			mg_cry_ctx_internal(phys_ctx,
			                    "%s: cannot use certificate chain file %s: %s",
			                    __func__,
			                    chain,
			                    ssl_error());
			return 0;
		}
	}
	return 1;
}


#if defined(OPENSSL_API_1_1)
static unsigned long
ssl_get_protocol(int version_id)
{
	long unsigned ret = (long unsigned)SSL_OP_ALL;
	if (version_id > 0)
		ret |= SSL_OP_NO_SSLv2;
	if (version_id > 1)
		ret |= SSL_OP_NO_SSLv3;
	if (version_id > 2)
		ret |= SSL_OP_NO_TLSv1;
	if (version_id > 3)
		ret |= SSL_OP_NO_TLSv1_1;
	if (version_id > 4)
		ret |= SSL_OP_NO_TLSv1_2;
#if defined(SSL_OP_NO_TLSv1_3)
	if (version_id > 5)
		ret |= SSL_OP_NO_TLSv1_3;
#endif
	return ret;
}
#else
static long
ssl_get_protocol(int version_id)
{
	unsigned long ret = (unsigned long)SSL_OP_ALL;
	if (version_id > 0)
		ret |= SSL_OP_NO_SSLv2;
	if (version_id > 1)
		ret |= SSL_OP_NO_SSLv3;
	if (version_id > 2)
		ret |= SSL_OP_NO_TLSv1;
	if (version_id > 3)
		ret |= SSL_OP_NO_TLSv1_1;
	if (version_id > 4)
		ret |= SSL_OP_NO_TLSv1_2;
#if defined(SSL_OP_NO_TLSv1_3)
	if (version_id > 5)
		ret |= SSL_OP_NO_TLSv1_3;
#endif
	return (long)ret;
}
#endif /* OPENSSL_API_1_1 */


/* SSL callback documentation:
 * https://www.openssl.org/docs/man1.1.0/ssl/SSL_set_info_callback.html
 * https://wiki.openssl.org/index.php/Manual:SSL_CTX_set_info_callback(3)
 * https://linux.die.net/man/3/ssl_set_info_callback */
/* Note: There is no "const" for the first argument in the documentation
 * examples, however some (maybe most, but not all) headers of OpenSSL
 * versions / OpenSSL compatibility layers have it. Having a different
 * definition will cause a warning in C and an error in C++. Use "const SSL
 * *", while automatic conversion from "SSL *" works for all compilers,
 * but not other way around */
static void
ssl_info_callback(const SSL *ssl, int what, int ret)
{
	(void)ret;

	if (what & SSL_CB_HANDSHAKE_START) {
		SSL_get_app_data(ssl);
	}
	if (what & SSL_CB_HANDSHAKE_DONE) {
		/* TODO: check for openSSL 1.1 */
		//#define SSL3_FLAGS_NO_RENEGOTIATE_CIPHERS 0x0001
		// ssl->s3->flags |= SSL3_FLAGS_NO_RENEGOTIATE_CIPHERS;
	}
}


static int
ssl_servername_callback(SSL *ssl, int *ad, void *arg)
{
#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#endif /* defined(GCC_DIAGNOSTIC) */

	/* We used an aligned pointer in SSL_set_app_data */
	struct mg_connection *conn = (struct mg_connection *)SSL_get_app_data(ssl);

#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic pop
#endif /* defined(GCC_DIAGNOSTIC) */

	const char *servername = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);

	(void)ad;
	(void)arg;

	if ((conn == NULL) || (conn->phys_ctx == NULL)) {
		DEBUG_ASSERT(0);
		return SSL_TLSEXT_ERR_NOACK;
	}
	conn->dom_ctx = &(conn->phys_ctx->dd);

	/* Old clients (Win XP) will not support SNI. Then, there
	 * is no server name available in the request - we can
	 * only work with the default certificate.
	 * Multiple HTTPS hosts on one IP+port are only possible
	 * with a certificate containing all alternative names.
	 */
	if ((servername == NULL) || (*servername == 0)) {
		DEBUG_TRACE("%s", "SSL connection not supporting SNI");
		mg_lock_context(conn->phys_ctx);
		SSL_set_SSL_CTX(ssl, conn->dom_ctx->ssl_ctx);
		mg_unlock_context(conn->phys_ctx);
		return SSL_TLSEXT_ERR_NOACK;
	}

	DEBUG_TRACE("TLS connection to host %s", servername);

	while (conn->dom_ctx) {
		if (!mg_strcasecmp(servername,
		                   conn->dom_ctx->config[AUTHENTICATION_DOMAIN])) {
			/* Found matching domain */
			DEBUG_TRACE("TLS domain %s found",
			            conn->dom_ctx->config[AUTHENTICATION_DOMAIN]);
			break;
		}
		mg_lock_context(conn->phys_ctx);
		conn->dom_ctx = conn->dom_ctx->next;
		mg_unlock_context(conn->phys_ctx);
	}

	if (conn->dom_ctx == NULL) {
		/* Default domain */
		DEBUG_TRACE("TLS default domain %s used",
		            conn->phys_ctx->dd.config[AUTHENTICATION_DOMAIN]);
		conn->dom_ctx = &(conn->phys_ctx->dd);
	}
	mg_lock_context(conn->phys_ctx);
	SSL_set_SSL_CTX(ssl, conn->dom_ctx->ssl_ctx);
	mg_unlock_context(conn->phys_ctx);
	return SSL_TLSEXT_ERR_OK;
}


#if defined(USE_ALPN)
static const char alpn_proto_list[] = "\x02h2\x08http/1.1\x08http/1.0";
static const char *alpn_proto_order_http1[] = {alpn_proto_list + 3,
                                               alpn_proto_list + 3 + 8,
                                               NULL};
#if defined(USE_HTTP2)
static const char *alpn_proto_order_http2[] = {alpn_proto_list,
                                               alpn_proto_list + 3,
                                               alpn_proto_list + 3 + 8,
                                               NULL};
#endif

static int
alpn_select_cb(SSL *ssl,
               const unsigned char **out,
               unsigned char *outlen,
               const unsigned char *in,
               unsigned int inlen,
               void *arg)
{
	struct mg_domain_context *dom_ctx = (struct mg_domain_context *)arg;
	unsigned int i, j, enable_http2 = 0;
	const char **alpn_proto_order = alpn_proto_order_http1;

	struct mg_workerTLS *tls =
	    (struct mg_workerTLS *)pthread_getspecific(sTlsKey);

	(void)ssl;

	if (tls == NULL) {
		/* Need to store protocol in Thread Local Storage */
		/* If there is no Thread Local Storage, don't use ALPN */
		return SSL_TLSEXT_ERR_NOACK;
	}

#if defined(USE_HTTP2)
	enable_http2 = (0 == strcmp(dom_ctx->config[ENABLE_HTTP2], "yes"));
	if (enable_http2) {
		alpn_proto_order = alpn_proto_order_http2;
	}
#endif

	for (j = 0; alpn_proto_order[j] != NULL; j++) {
		/* check all accepted protocols in this order */
		const char *alpn_proto = alpn_proto_order[j];
		/* search input for matching protocol */
		for (i = 0; i < inlen; i++) {
			if (!memcmp(in + i, alpn_proto, (unsigned char)alpn_proto[0])) {
				*out = in + i + 1;
				*outlen = in[i];
				tls->alpn_proto = alpn_proto;
				return SSL_TLSEXT_ERR_OK;
			}
		}
	}

	/* Nothing found */
	return SSL_TLSEXT_ERR_NOACK;
}


static int
next_protos_advertised_cb(SSL *ssl,
                          const unsigned char **data,
                          unsigned int *len,
                          void *arg)
{
	struct mg_domain_context *dom_ctx = (struct mg_domain_context *)arg;
	*data = (const unsigned char *)alpn_proto_list;
	*len = (unsigned int)strlen((const char *)data);

	(void)ssl;
	(void)dom_ctx;

	return SSL_TLSEXT_ERR_OK;
}


static int
init_alpn(struct mg_context *phys_ctx, struct mg_domain_context *dom_ctx)
{
	unsigned int alpn_len = (unsigned int)strlen((char *)alpn_proto_list);
	int ret = SSL_CTX_set_alpn_protos(dom_ctx->ssl_ctx,
	                                  (const unsigned char *)alpn_proto_list,
	                                  alpn_len);
	if (ret != 0) {
		mg_cry_ctx_internal(phys_ctx,
		                    "SSL_CTX_set_alpn_protos error: %s",
		                    ssl_error());
	}

	SSL_CTX_set_alpn_select_cb(dom_ctx->ssl_ctx,
	                           alpn_select_cb,
	                           (void *)dom_ctx);

	SSL_CTX_set_next_protos_advertised_cb(dom_ctx->ssl_ctx,
	                                      next_protos_advertised_cb,
	                                      (void *)dom_ctx);

	return ret;
}
#endif


/* Setup SSL CTX as required by CivetWeb */
static int
init_ssl_ctx_impl(struct mg_context *phys_ctx,
                  struct mg_domain_context *dom_ctx,
                  const char *pem,
                  const char *chain)
{
	int callback_ret;
	int should_verify_peer;
	int peer_certificate_optional;
	const char *ca_path;
	const char *ca_file;
	int use_default_verify_paths;
	int verify_depth;
	struct timespec now_mt;
	md5_byte_t ssl_context_id[16];
	md5_state_t md5state;
	int protocol_ver;
	int ssl_cache_timeout;

#if (defined(OPENSSL_API_1_1) || defined(OPENSSL_API_3_0))                     \
    && !defined(NO_SSL_DL)
	if ((dom_ctx->ssl_ctx = SSL_CTX_new(TLS_server_method())) == NULL) {
		mg_cry_ctx_internal(phys_ctx,
		                    "SSL_CTX_new (server) error: %s",
		                    ssl_error());
		return 0;
	}
#else
	if ((dom_ctx->ssl_ctx = SSL_CTX_new(SSLv23_server_method())) == NULL) {
		mg_cry_ctx_internal(phys_ctx,
		                    "SSL_CTX_new (server) error: %s",
		                    ssl_error());
		return 0;
	}
#endif /* OPENSSL_API_1_1 || OPENSSL_API_3_0 */

#if defined(SSL_OP_NO_TLSv1_3)
	SSL_CTX_clear_options(dom_ctx->ssl_ctx,
	                      SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1
	                          | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2
	                          | SSL_OP_NO_TLSv1_3);
#else
	SSL_CTX_clear_options(dom_ctx->ssl_ctx,
	                      SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1
	                          | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2);
#endif

	protocol_ver = atoi(dom_ctx->config[SSL_PROTOCOL_VERSION]);
	SSL_CTX_set_options(dom_ctx->ssl_ctx, ssl_get_protocol(protocol_ver));
	SSL_CTX_set_options(dom_ctx->ssl_ctx, SSL_OP_SINGLE_DH_USE);
	SSL_CTX_set_options(dom_ctx->ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
	SSL_CTX_set_options(dom_ctx->ssl_ctx,
	                    SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
	SSL_CTX_set_options(dom_ctx->ssl_ctx, SSL_OP_NO_COMPRESSION);

#if defined(SSL_OP_NO_RENEGOTIATION)
	SSL_CTX_set_options(dom_ctx->ssl_ctx, SSL_OP_NO_RENEGOTIATION);
#endif

#if !defined(NO_SSL_DL)
	SSL_CTX_set_ecdh_auto(dom_ctx->ssl_ctx, 1);
#endif /* NO_SSL_DL */

	/* In SSL documentation examples callback defined without const
	 * specifier 'void (*)(SSL *, int, int)'   See:
	 * https://www.openssl.org/docs/man1.0.2/ssl/ssl.html
	 * https://www.openssl.org/docs/man1.1.0/ssl/ssl.html
	 * But in the source code const SSL is used:
	 * 'void (*)(const SSL *, int, int)' See:
	 * https://github.com/openssl/openssl/blob/1d97c8435171a7af575f73c526d79e1ef0ee5960/ssl/ssl.h#L1173
	 * Problem about wrong documentation described, but not resolved:
	 * https://bugs.launchpad.net/ubuntu/+source/openssl/+bug/1147526
	 * Wrong const cast ignored on C or can be suppressed by compiler flags.
	 * But when compiled with modern C++ compiler, correct const should be
	 * provided
	 */
	SSL_CTX_set_info_callback(dom_ctx->ssl_ctx, ssl_info_callback);

	SSL_CTX_set_tlsext_servername_callback(dom_ctx->ssl_ctx,
	                                       ssl_servername_callback);

	/* If a callback has been specified, call it. */
	callback_ret = (phys_ctx->callbacks.init_ssl == NULL)
	                   ? 0
	                   : (phys_ctx->callbacks.init_ssl(dom_ctx->ssl_ctx,
	                                                   phys_ctx->user_data));

	/* If callback returns 0, civetweb sets up the SSL certificate.
	 * If it returns 1, civetweb assumes the callback already did this.
	 * If it returns -1, initializing ssl fails. */
	if (callback_ret < 0) {
		mg_cry_ctx_internal(phys_ctx,
		                    "SSL callback returned error: %i",
		                    callback_ret);
		return 0;
	}
	if (callback_ret > 0) {
		/* Callback did everything. */
		return 1;
	}

	/* If a domain callback has been specified, call it. */
	callback_ret = (phys_ctx->callbacks.init_ssl_domain == NULL)
	                   ? 0
	                   : (phys_ctx->callbacks.init_ssl_domain(
	                         dom_ctx->config[AUTHENTICATION_DOMAIN],
	                         dom_ctx->ssl_ctx,
	                         phys_ctx->user_data));

	/* If domain callback returns 0, civetweb sets up the SSL certificate.
	 * If it returns 1, civetweb assumes the callback already did this.
	 * If it returns -1, initializing ssl fails. */
	if (callback_ret < 0) {
		mg_cry_ctx_internal(phys_ctx,
		                    "Domain SSL callback returned error: %i",
		                    callback_ret);
		return 0;
	}
	if (callback_ret > 0) {
		/* Domain callback did everything. */
		return 1;
	}

	/* Use some combination of start time, domain and port as a SSL
	 * context ID. This should be unique on the current machine. */
	md5_init(&md5state);
	clock_gettime(CLOCK_MONOTONIC, &now_mt);
	md5_append(&md5state, (const md5_byte_t *)&now_mt, sizeof(now_mt));
	md5_append(&md5state,
	           (const md5_byte_t *)phys_ctx->dd.config[LISTENING_PORTS],
	           strlen(phys_ctx->dd.config[LISTENING_PORTS]));
	md5_append(&md5state,
	           (const md5_byte_t *)dom_ctx->config[AUTHENTICATION_DOMAIN],
	           strlen(dom_ctx->config[AUTHENTICATION_DOMAIN]));
	md5_append(&md5state, (const md5_byte_t *)phys_ctx, sizeof(*phys_ctx));
	md5_append(&md5state, (const md5_byte_t *)dom_ctx, sizeof(*dom_ctx));
	md5_finish(&md5state, ssl_context_id);

	SSL_CTX_set_session_id_context(dom_ctx->ssl_ctx,
	                               (unsigned char *)ssl_context_id,
	                               sizeof(ssl_context_id));

	if (pem != NULL) {
		if (!ssl_use_pem_file(phys_ctx, dom_ctx, pem, chain)) {
			return 0;
		}
	}

	/* Should we support client certificates? */
	/* Default is "no". */
	should_verify_peer = 0;
	peer_certificate_optional = 0;
	if (dom_ctx->config[SSL_DO_VERIFY_PEER] != NULL) {
		if (mg_strcasecmp(dom_ctx->config[SSL_DO_VERIFY_PEER], "yes") == 0) {
			/* Yes, they are mandatory */
			should_verify_peer = 1;
		} else if (mg_strcasecmp(dom_ctx->config[SSL_DO_VERIFY_PEER],
		                         "optional")
		           == 0) {
			/* Yes, they are optional */
			should_verify_peer = 1;
			peer_certificate_optional = 1;
		}
	}

	use_default_verify_paths =
	    (dom_ctx->config[SSL_DEFAULT_VERIFY_PATHS] != NULL)
	    && (mg_strcasecmp(dom_ctx->config[SSL_DEFAULT_VERIFY_PATHS], "yes")
	        == 0);

	if (should_verify_peer) {
		ca_path = dom_ctx->config[SSL_CA_PATH];
		ca_file = dom_ctx->config[SSL_CA_FILE];
		if (SSL_CTX_load_verify_locations(dom_ctx->ssl_ctx, ca_file, ca_path)
		    != 1) {
			mg_cry_ctx_internal(phys_ctx,
			                    "SSL_CTX_load_verify_locations error: %s "
			                    "ssl_verify_peer requires setting "
			                    "either ssl_ca_path or ssl_ca_file. "
			                    "Is any of them present in the "
			                    ".conf file?",
			                    ssl_error());
			return 0;
		}

		if (peer_certificate_optional) {
			SSL_CTX_set_verify(dom_ctx->ssl_ctx, SSL_VERIFY_PEER, NULL);
		} else {
			SSL_CTX_set_verify(dom_ctx->ssl_ctx,
			                   SSL_VERIFY_PEER
			                       | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
			                   NULL);
		}

		if (use_default_verify_paths
		    && (SSL_CTX_set_default_verify_paths(dom_ctx->ssl_ctx) != 1)) {
			mg_cry_ctx_internal(phys_ctx,
			                    "SSL_CTX_set_default_verify_paths error: %s",
			                    ssl_error());
			return 0;
		}

		if (dom_ctx->config[SSL_VERIFY_DEPTH]) {
			verify_depth = atoi(dom_ctx->config[SSL_VERIFY_DEPTH]);
			SSL_CTX_set_verify_depth(dom_ctx->ssl_ctx, verify_depth);
		}
	}

	if (dom_ctx->config[SSL_CIPHER_LIST] != NULL) {
		if (SSL_CTX_set_cipher_list(dom_ctx->ssl_ctx,
		                            dom_ctx->config[SSL_CIPHER_LIST])
		    != 1) {
			mg_cry_ctx_internal(phys_ctx,
			                    "SSL_CTX_set_cipher_list error: %s",
			                    ssl_error());
		}
	}

	/* SSL session caching */
	ssl_cache_timeout = ((dom_ctx->config[SSL_CACHE_TIMEOUT] != NULL)
	                         ? atoi(dom_ctx->config[SSL_CACHE_TIMEOUT])
	                         : 0);
	if (ssl_cache_timeout > 0) {
		SSL_CTX_set_session_cache_mode(dom_ctx->ssl_ctx, SSL_SESS_CACHE_BOTH);
		/* SSL_CTX_sess_set_cache_size(dom_ctx->ssl_ctx, 10000);  ... use
		 * default */
		SSL_CTX_set_timeout(dom_ctx->ssl_ctx, (long)ssl_cache_timeout);
	}

#if defined(USE_ALPN)
	/* Initialize ALPN only of TLS library (OpenSSL version) supports ALPN */
#if !defined(NO_SSL_DL)
	if (!tls_feature_missing[TLS_ALPN])
#endif
	{
		init_alpn(phys_ctx, dom_ctx);
	}
#endif

	return 1;
}


/* Check if SSL is required.
 * If so, dynamically load SSL library
 * and set up ctx->ssl_ctx pointer. */
static int
init_ssl_ctx(struct mg_context *phys_ctx, struct mg_domain_context *dom_ctx)
{
	void *ssl_ctx = 0;
	int callback_ret;
	const char *pem;
	const char *chain;
	char ebuf[128];

	if (!phys_ctx) {
		return 0;
	}

	if (!dom_ctx) {
		dom_ctx = &(phys_ctx->dd);
	}

	if (!is_ssl_port_used(dom_ctx->config[LISTENING_PORTS])) {
		/* No SSL port is set. No need to setup SSL. */
		return 1;
	}

	/* Check for external SSL_CTX */
	callback_ret =
	    (phys_ctx->callbacks.external_ssl_ctx == NULL)
	        ? 0
	        : (phys_ctx->callbacks.external_ssl_ctx(&ssl_ctx,
	                                                phys_ctx->user_data));

	if (callback_ret < 0) {
		/* Callback exists and returns <0: Initializing failed. */
		mg_cry_ctx_internal(phys_ctx,
		                    "external_ssl_ctx callback returned error: %i",
		                    callback_ret);
		return 0;
	} else if (callback_ret > 0) {
		/* Callback exists and returns >0: Initializing complete,
		 * civetweb should not modify the SSL context. */
		dom_ctx->ssl_ctx = (SSL_CTX *)ssl_ctx;
		if (!initialize_openssl(ebuf, sizeof(ebuf))) {
			mg_cry_ctx_internal(phys_ctx, "%s", ebuf);
			return 0;
		}
		return 1;
	}
	/* If the callback does not exist or return 0, civetweb must initialize
	 * the SSL context. Handle "domain" callback next. */

	/* Check for external domain SSL_CTX callback. */
	callback_ret = (phys_ctx->callbacks.external_ssl_ctx_domain == NULL)
	                   ? 0
	                   : (phys_ctx->callbacks.external_ssl_ctx_domain(
	                         dom_ctx->config[AUTHENTICATION_DOMAIN],
	                         &ssl_ctx,
	                         phys_ctx->user_data));

	if (callback_ret < 0) {
		/* Callback < 0: Error. Abort init. */
		mg_cry_ctx_internal(
		    phys_ctx,
		    "external_ssl_ctx_domain callback returned error: %i",
		    callback_ret);
		return 0;
	} else if (callback_ret > 0) {
		/* Callback > 0: Consider init done. */
		dom_ctx->ssl_ctx = (SSL_CTX *)ssl_ctx;
		if (!initialize_openssl(ebuf, sizeof(ebuf))) {
			mg_cry_ctx_internal(phys_ctx, "%s", ebuf);
			return 0;
		}
		return 1;
	}
	/* else: external_ssl_ctx/external_ssl_ctx_domain do not exist or return
	 * 0, CivetWeb should continue initializing SSL */

	/* If PEM file is not specified and the init_ssl callbacks
	 * are not specified, setup will fail. */
	if (((pem = dom_ctx->config[SSL_CERTIFICATE]) == NULL)
	    && (phys_ctx->callbacks.init_ssl == NULL)
	    && (phys_ctx->callbacks.init_ssl_domain == NULL)) {
		/* No certificate and no init_ssl callbacks:
		 * Essential data to set up TLS is missing.
		 */
		mg_cry_ctx_internal(phys_ctx,
		                    "Initializing SSL failed: -%s is not set",
		                    config_options[SSL_CERTIFICATE].name);
		return 0;
	}

	/* If a certificate chain is configured, use it. */
	chain = dom_ctx->config[SSL_CERTIFICATE_CHAIN];
	if (chain == NULL) {
		/* Default: certificate chain in PEM file */
		chain = pem;
	}
	if ((chain != NULL) && (*chain == 0)) {
		/* If the chain is an empty string, don't use it. */
		chain = NULL;
	}

	if (!initialize_openssl(ebuf, sizeof(ebuf))) {
		mg_cry_ctx_internal(phys_ctx, "%s", ebuf);
		return 0;
	}

	return init_ssl_ctx_impl(phys_ctx, dom_ctx, pem, chain);
}


static void
uninitialize_openssl(void)
{
#if defined(OPENSSL_API_1_1) || defined(OPENSSL_API_3_0)

	if (mg_atomic_dec(&cryptolib_users) == 0) {

		/* Shutdown according to
		 * https://wiki.openssl.org/index.php/Library_Initialization#Cleanup
		 * http://stackoverflow.com/questions/29845527/how-to-properly-uninitialize-openssl
		 */
		CONF_modules_unload(1);
#else
	int i;

	if (mg_atomic_dec(&cryptolib_users) == 0) {

		/* Shutdown according to
		 * https://wiki.openssl.org/index.php/Library_Initialization#Cleanup
		 * http://stackoverflow.com/questions/29845527/how-to-properly-uninitialize-openssl
		 */
		CRYPTO_set_locking_callback(NULL);
		CRYPTO_set_id_callback(NULL);
		ENGINE_cleanup();
		CONF_modules_unload(1);
		ERR_free_strings();
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		OPENSSL_REMOVE_THREAD_STATE();

		for (i = 0; i < CRYPTO_num_locks(); i++) {
			pthread_mutex_destroy(&ssl_mutexes[i]);
		}
		mg_free(ssl_mutexes);
		ssl_mutexes = NULL;
#endif /* OPENSSL_API_1_1 || OPENSSL_API_3_0 */
	}
}
#endif /* !defined(NO_SSL) && !defined(USE_MBEDTLS) && !defined(USE_GNUTLS) */


#if !defined(NO_FILESYSTEMS)
static int
set_gpass_option(struct mg_context *phys_ctx, struct mg_domain_context *dom_ctx)
{
	if (phys_ctx) {
		struct mg_file file = STRUCT_FILE_INITIALIZER;
		const char *path;
		struct mg_connection fc;
		if (!dom_ctx) {
			dom_ctx = &(phys_ctx->dd);
		}
		path = dom_ctx->config[GLOBAL_PASSWORDS_FILE];
		if ((path != NULL)
		    && !mg_stat(fake_connection(&fc, phys_ctx), path, &file.stat)) {
			mg_cry_ctx_internal(phys_ctx,
			                    "Cannot open %s: %s",
			                    path,
			                    strerror(ERRNO));
			return 0;
		}
		return 1;
	}
	return 0;
}
#endif /* NO_FILESYSTEMS */


static int
set_acl_option(struct mg_context *phys_ctx)
{
	union usa sa;
	memset(&sa, 0, sizeof(sa));
#if defined(USE_IPV6)
	sa.sin6.sin6_family = AF_INET6;
#else
	sa.sin.sin_family = AF_INET;
#endif
	return check_acl(phys_ctx, &sa) != -1;
}


static void
reset_per_request_attributes(struct mg_connection *conn)
{
	if (!conn) {
		return;
	}

	conn->num_bytes_sent = conn->consumed_content = 0;

	conn->path_info = NULL;
	conn->status_code = -1;
	conn->content_len = -1;
	conn->is_chunked = 0;
	conn->must_close = 0;
	conn->request_len = 0;
	conn->request_state = 0;
	conn->throttle = 0;
	conn->accept_gzip = 0;

	conn->response_info.content_length = conn->request_info.content_length = -1;
	conn->response_info.http_version = conn->request_info.http_version = NULL;
	conn->response_info.num_headers = conn->request_info.num_headers = 0;
	conn->response_info.status_text = NULL;
	conn->response_info.status_code = 0;

	conn->request_info.remote_user = NULL;
	conn->request_info.request_method = NULL;
	conn->request_info.request_uri = NULL;

	/* Free cleaned local URI (if any) */
	if (conn->request_info.local_uri != conn->request_info.local_uri_raw) {
		mg_free((void *)conn->request_info.local_uri);
		conn->request_info.local_uri = NULL;
	}
	conn->request_info.local_uri = NULL;

#if defined(USE_SERVER_STATS)
	conn->processing_time = 0;
#endif
}


static int
set_tcp_nodelay(const struct socket *so, int nodelay_on)
{
	if ((so->lsa.sa.sa_family == AF_INET)
	    || (so->lsa.sa.sa_family == AF_INET6)) {
		/* Only for TCP sockets */
		if (setsockopt(so->sock,
		               IPPROTO_TCP,
		               TCP_NODELAY,
		               (SOCK_OPT_TYPE)&nodelay_on,
		               sizeof(nodelay_on))
		    != 0) {
			/* Error */
			return 1;
		}
	}
	/* OK */
	return 0;
}


#if !defined(__ZEPHYR__)
static void
close_socket_gracefully(struct mg_connection *conn)
{
#if defined(_WIN32)
	char buf[MG_BUF_LEN];
	int n;
#endif
	struct linger linger;
	int error_code = 0;
	int linger_timeout = -2;
	socklen_t opt_len = sizeof(error_code);

	if (!conn) {
		return;
	}

	/* http://msdn.microsoft.com/en-us/library/ms739165(v=vs.85).aspx:
	 * "Note that enabling a nonzero timeout on a nonblocking socket
	 * is not recommended.", so set it to blocking now */
	set_blocking_mode(conn->client.sock);

	/* Send FIN to the client */
	shutdown(conn->client.sock, SHUTDOWN_WR);

#if defined(_WIN32)
	/* Read and discard pending incoming data. If we do not do that and
	 * close
	 * the socket, the data in the send buffer may be discarded. This
	 * behaviour is seen on Windows, when client keeps sending data
	 * when server decides to close the connection; then when client
	 * does recv() it gets no data back. */
	do {
		n = pull_inner(NULL, conn, buf, sizeof(buf), /* Timeout in s: */ 1.0);
	} while (n > 0);
#endif

	if (conn->dom_ctx->config[LINGER_TIMEOUT]) {
		linger_timeout = atoi(conn->dom_ctx->config[LINGER_TIMEOUT]);
	}

	/* Set linger option according to configuration */
	if (linger_timeout >= 0) {
		/* Set linger option to avoid socket hanging out after close. This
		 * prevent ephemeral port exhaust problem under high QPS. */
		linger.l_onoff = 1;

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
		/* Data type of linger structure elements may differ,
		 * so we don't know what cast we need here.
		 * Disable type conversion warnings. */

		linger.l_linger = (linger_timeout + 999) / 1000;

#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic pop
#endif
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

	} else {
		linger.l_onoff = 0;
		linger.l_linger = 0;
	}

	if (linger_timeout < -1) {
		/* Default: don't configure any linger */
	} else if (getsockopt(conn->client.sock,
	                      SOL_SOCKET,
	                      SO_ERROR,
#if defined(_WIN32) /* WinSock uses different data type here */
	                      (char *)&error_code,
#else
	                      &error_code,
#endif
	                      &opt_len)
	           != 0) {
		/* Cannot determine if socket is already closed. This should
		 * not occur and never did in a test. Log an error message
		 * and continue. */
		mg_cry_internal(conn,
		                "%s: getsockopt(SOL_SOCKET SO_ERROR) failed: %s",
		                __func__,
		                strerror(ERRNO));
#if defined(_WIN32)
	} else if (error_code == WSAECONNRESET) {
#else
	} else if (error_code == ECONNRESET) {
#endif
		/* Socket already closed by client/peer, close socket without linger
		 */
	} else {

		/* Set linger timeout */
		if (setsockopt(conn->client.sock,
		               SOL_SOCKET,
		               SO_LINGER,
		               (char *)&linger,
		               sizeof(linger))
		    != 0) {
			mg_cry_internal(
			    conn,
			    "%s: setsockopt(SOL_SOCKET SO_LINGER(%i,%i)) failed: %s",
			    __func__,
			    linger.l_onoff,
			    linger.l_linger,
			    strerror(ERRNO));
		}
	}

	/* Now we know that our FIN is ACK-ed, safe to close */
	closesocket(conn->client.sock);
	conn->client.sock = INVALID_SOCKET;
}
#endif


static void
close_connection(struct mg_connection *conn)
{
#if defined(USE_SERVER_STATS)
	conn->conn_state = 6; /* to close */
#endif

#if defined(USE_LUA) && defined(USE_WEBSOCKET)
	if (conn->lua_websocket_state) {
		lua_websocket_close(conn, conn->lua_websocket_state);
		conn->lua_websocket_state = NULL;
	}
#endif

	mg_lock_connection(conn);

	/* Set close flag, so keep-alive loops will stop */
	conn->must_close = 1;

	/* call the connection_close callback if assigned */
	if (conn->phys_ctx->callbacks.connection_close != NULL) {
		if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
			conn->phys_ctx->callbacks.connection_close(conn);
		}
	}

	/* Reset user data, after close callback is called.
	 * Do not reuse it. If the user needs a destructor,
	 * it must be done in the connection_close callback. */
	mg_set_user_connection_data(conn, NULL);

#if defined(USE_SERVER_STATS)
	conn->conn_state = 7; /* closing */
#endif

#if defined(USE_MBEDTLS)
	if (conn->ssl != NULL) {
		mbed_ssl_close(conn->ssl);
		conn->ssl = NULL;
	}
#elif defined(USE_GNUTLS)
	if (conn->ssl != NULL) {
		gtls_ssl_close(conn->ssl);
		conn->ssl = NULL;
	}
#elif !defined(NO_SSL)
	if (conn->ssl != NULL) {
		/* Run SSL_shutdown twice to ensure completely close SSL connection
		 */
		SSL_shutdown(conn->ssl);
		SSL_free(conn->ssl);
		OPENSSL_REMOVE_THREAD_STATE();
		conn->ssl = NULL;
	}
#endif
	if (conn->client.sock != INVALID_SOCKET) {
#if defined(__ZEPHYR__)
		closesocket(conn->client.sock);
#else
		close_socket_gracefully(conn);
#endif
		conn->client.sock = INVALID_SOCKET;
	}

	/* call the connection_closed callback if assigned */
	if (conn->phys_ctx->callbacks.connection_closed != NULL) {
		if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
			conn->phys_ctx->callbacks.connection_closed(conn);
		}
	}

	mg_unlock_connection(conn);

#if defined(USE_SERVER_STATS)
	conn->conn_state = 8; /* closed */
#endif
}


CIVETWEB_API void
mg_close_connection(struct mg_connection *conn)
{
	if ((conn == NULL) || (conn->phys_ctx == NULL)) {
		return;
	}

#if defined(USE_WEBSOCKET)
	if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
		if (conn->in_websocket_handling) {
			/* Set close flag, so the server thread can exit. */
			conn->must_close = 1;
			return;
		}
	}
	if (conn->phys_ctx->context_type == CONTEXT_WS_CLIENT) {

		unsigned int i;

		/* client context: loops must end */
		STOP_FLAG_ASSIGN(&conn->phys_ctx->stop_flag, 1);
		conn->must_close = 1;

		/* We need to get the client thread out of the select/recv call
		 * here. */
		/* Since we use a sleep quantum of some seconds to check for recv
		 * timeouts, we will just wait a few seconds in mg_join_thread. */

		/* join worker thread */
		for (i = 0; i < conn->phys_ctx->spawned_worker_threads; i++) {
			mg_join_thread(conn->phys_ctx->worker_threadids[i]);
		}
	}
#endif /* defined(USE_WEBSOCKET) */

	close_connection(conn);

#if !defined(NO_SSL) && !defined(USE_MBEDTLS) && !defined(USE_GNUTLS) // TODO: mbedTLS client
	if (((conn->phys_ctx->context_type == CONTEXT_HTTP_CLIENT)
	     || (conn->phys_ctx->context_type == CONTEXT_WS_CLIENT))
	    && (conn->phys_ctx->dd.ssl_ctx != NULL)) {
		SSL_CTX_free(conn->phys_ctx->dd.ssl_ctx);
	}
#endif

#if defined(USE_WEBSOCKET)
	if (conn->phys_ctx->context_type == CONTEXT_WS_CLIENT) {
		mg_free(conn->phys_ctx->worker_threadids);
		(void)pthread_mutex_destroy(&conn->mutex);
		mg_free(conn);
	} else if (conn->phys_ctx->context_type == CONTEXT_HTTP_CLIENT) {
		(void)pthread_mutex_destroy(&conn->mutex);
		mg_free(conn);
	}
#else
	if (conn->phys_ctx->context_type == CONTEXT_HTTP_CLIENT) { /* Client */
		(void)pthread_mutex_destroy(&conn->mutex);
		mg_free(conn);
	}
#endif /* defined(USE_WEBSOCKET) */
}


static struct mg_connection *
mg_connect_client_impl(const struct mg_client_options *client_options,
                       int use_ssl,
                       struct mg_init_data *init,
                       struct mg_error_data *error)
{
	struct mg_connection *conn = NULL;
	SOCKET sock;
	union usa sa;
	struct sockaddr *psa;
	socklen_t len;

	unsigned max_req_size =
	    (unsigned)atoi(config_options[MAX_REQUEST_SIZE].default_value);

	/* Size of structures, aligned to 8 bytes */
	size_t conn_size = ((sizeof(struct mg_connection) + 7) >> 3) << 3;
	size_t ctx_size = ((sizeof(struct mg_context) + 7) >> 3) << 3;
	size_t alloc_size = conn_size + ctx_size + max_req_size;

	(void)init; /* TODO: Implement required options */

	conn = (struct mg_connection *)mg_calloc(1, alloc_size);

	if (error != NULL) {
		error->code = MG_ERROR_DATA_CODE_OK;
		error->code_sub = 0;
		if (error->text_buffer_size > 0) {
			error->text[0] = 0;
		}
	}

	if (conn == NULL) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OUT_OF_MEMORY;
			error->code_sub = (unsigned)alloc_size;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "calloc(): %s",
			            strerror(ERRNO));
		}
		return NULL;
	}

#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#endif /* defined(GCC_DIAGNOSTIC) */
	/* conn_size is aligned to 8 bytes */

	conn->phys_ctx = (struct mg_context *)(((char *)conn) + conn_size);

#if defined(GCC_DIAGNOSTIC)
#pragma GCC diagnostic pop
#endif /* defined(GCC_DIAGNOSTIC) */

	conn->buf = (((char *)conn) + conn_size + ctx_size);
	conn->buf_size = (int)max_req_size;
	conn->phys_ctx->context_type = CONTEXT_HTTP_CLIENT;
	conn->dom_ctx = &(conn->phys_ctx->dd);

	if (!connect_socket(conn->phys_ctx,
	                    client_options->host,
	                    client_options->port,
	                    use_ssl,
	                    error,
	                    &sock,
	                    &sa)) {
		/* "error" will be set by connect_socket. */
		/* free all memory and return NULL; */
		mg_free(conn);
		return NULL;
	}

#if !defined(NO_SSL) && !defined(USE_MBEDTLS) && !defined(USE_GNUTLS) // TODO: mbedTLS client
#if (defined(OPENSSL_API_1_1) || defined(OPENSSL_API_3_0))                     \
    && !defined(NO_SSL_DL)

	if (use_ssl
	    && (conn->dom_ctx->ssl_ctx = SSL_CTX_new(TLS_client_method()))
	           == NULL) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_TLS_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "SSL_CTX_new error: %s",
			            ssl_error());
		}

		closesocket(sock);
		mg_free(conn);
		return NULL;
	}

#else

	if (use_ssl
	    && (conn->dom_ctx->ssl_ctx = SSL_CTX_new(SSLv23_client_method()))
	           == NULL) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_TLS_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "SSL_CTX_new error: %s",
			            ssl_error());
		}

		closesocket(sock);
		mg_free(conn);
		return NULL;
	}

#endif /* OPENSSL_API_1_1 || OPENSSL_API_3_0 */
#endif /* NO_SSL */

#if defined(USE_IPV6)
	len = (sa.sa.sa_family == AF_INET) ? sizeof(conn->client.rsa.sin)
	                                   : sizeof(conn->client.rsa.sin6);
	psa = (sa.sa.sa_family == AF_INET)
	          ? (struct sockaddr *)&(conn->client.rsa.sin)
	          : (struct sockaddr *)&(conn->client.rsa.sin6);
#else
	len = sizeof(conn->client.rsa.sin);
	psa = (struct sockaddr *)&(conn->client.rsa.sin);
#endif

	conn->client.sock = sock;
	conn->client.lsa = sa;

	if (getsockname(sock, psa, &len) != 0) {
		mg_cry_internal(conn,
		                "%s: getsockname() failed: %s",
		                __func__,
		                strerror(ERRNO));
	}

	conn->client.is_ssl = use_ssl ? 1 : 0;
	if (0 != pthread_mutex_init(&conn->mutex, &pthread_mutex_attr)) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OS_ERROR;
			error->code_sub = (unsigned)ERRNO;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for ebuf */
			            error->text,
			            error->text_buffer_size,
			            "Can not create mutex");
		}
#if !defined(NO_SSL) && !defined(USE_MBEDTLS) && !defined(USE_GNUTLS) // TODO: mbedTLS client
		SSL_CTX_free(conn->dom_ctx->ssl_ctx);
#endif
		closesocket(sock);
		mg_free(conn);
		return NULL;
	}

#if !defined(NO_SSL) && !defined(USE_MBEDTLS) && !defined(USE_GNUTLS) // TODO: mbedTLS client
	if (use_ssl) {
		/* TODO: Check ssl_verify_peer and ssl_ca_path here.
		 * SSL_CTX_set_verify call is needed to switch off server
		 * certificate checking, which is off by default in OpenSSL and
		 * on in yaSSL. */
		/* TODO: SSL_CTX_set_verify(conn->dom_ctx,
		 * SSL_VERIFY_PEER, verify_ssl_server); */

		if (client_options->client_cert) {
			if (!ssl_use_pem_file(conn->phys_ctx,
			                      conn->dom_ctx,
			                      client_options->client_cert,
			                      NULL)) {
				if (error != NULL) {
					error->code = MG_ERROR_DATA_CODE_TLS_CLIENT_CERT_ERROR;
					mg_snprintf(NULL,
					            NULL, /* No truncation check for ebuf */
					            error->text,
					            error->text_buffer_size,
					            "Can not use SSL client certificate");
				}

				SSL_CTX_free(conn->dom_ctx->ssl_ctx);
				closesocket(sock);
				mg_free(conn);
				return NULL;
			}
		}

		if (client_options->server_cert) {
			if (SSL_CTX_load_verify_locations(conn->dom_ctx->ssl_ctx,
			                                  client_options->server_cert,
			                                  NULL)
			    != 1) {
				if (error != NULL) {
					error->code = MG_ERROR_DATA_CODE_TLS_SERVER_CERT_ERROR;
					mg_snprintf(NULL,
					            NULL, /* No truncation check for ebuf */
					            error->text,
					            error->text_buffer_size,
					            "SSL_CTX_load_verify_locations error: %s",
					            ssl_error());
				}
				SSL_CTX_free(conn->dom_ctx->ssl_ctx);
				closesocket(sock);
				mg_free(conn);
				return NULL;
			}
			SSL_CTX_set_verify(conn->dom_ctx->ssl_ctx, SSL_VERIFY_PEER, NULL);
		} else {
			SSL_CTX_set_verify(conn->dom_ctx->ssl_ctx, SSL_VERIFY_NONE, NULL);
		}

		if (!sslize(conn, SSL_connect, client_options)) {
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_TLS_CONNECT_ERROR;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for ebuf */
				            error->text,
				            error->text_buffer_size,
				            "SSL connection error");
			}
			SSL_CTX_free(conn->dom_ctx->ssl_ctx);
			closesocket(sock);
			mg_free(conn);
			return NULL;
		}
	}
#endif

	return conn;
}


CIVETWEB_API struct mg_connection *
mg_connect_client_secure(const struct mg_client_options *client_options,
                         char *error_buffer,
                         size_t error_buffer_size)
{
	struct mg_init_data init;
	struct mg_error_data error;

	memset(&init, 0, sizeof(init));
	memset(&error, 0, sizeof(error));
	error.text_buffer_size = error_buffer_size;
	error.text = error_buffer;
	return mg_connect_client_impl(client_options, 1, &init, &error);
}


CIVETWEB_API struct mg_connection *
mg_connect_client(const char *host,
                  int port,
                  int use_ssl,
                  char *error_buffer,
                  size_t error_buffer_size)
{
	struct mg_client_options opts;
	struct mg_init_data init;
	struct mg_error_data error;

	memset(&init, 0, sizeof(init));

	memset(&error, 0, sizeof(error));
	error.text_buffer_size = error_buffer_size;
	error.text = error_buffer;

	memset(&opts, 0, sizeof(opts));
	opts.host = host;
	opts.port = port;
	if (use_ssl) {
		opts.host_name = host;
	}

	return mg_connect_client_impl(&opts, use_ssl, &init, &error);
}


#if defined(MG_EXPERIMENTAL_INTERFACES)
CIVETWEB_API struct mg_connection *
mg_connect_client2(const char *host,
                   const char *protocol,
                   int port,
                   const char *path,
                   struct mg_init_data *init,
                   struct mg_error_data *error)
{
	(void)path;

	int is_ssl, is_ws;
	/* void *user_data = (init != NULL) ? init->user_data : NULL; -- TODO */

	if (error != NULL) {
		error->code = MG_ERROR_DATA_CODE_OK;
		error->code_sub = 0;
		if (error->text_buffer_size > 0) {
			*error->text = 0;
		}
	}

	if ((host == NULL) || (protocol == NULL)) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_PARAM;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "Invalid parameters");
		}
		return NULL;
	}

	/* check all known protocols */
	if (!mg_strcasecmp(protocol, "http")) {
		is_ssl = 0;
		is_ws = 0;
	} else if (!mg_strcasecmp(protocol, "https")) {
		is_ssl = 1;
		is_ws = 0;
#if defined(USE_WEBSOCKET)
	} else if (!mg_strcasecmp(protocol, "ws")) {
		is_ssl = 0;
		is_ws = 1;
	} else if (!mg_strcasecmp(protocol, "wss")) {
		is_ssl = 1;
		is_ws = 1;
#endif
	} else {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_PARAM;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "Protocol %s not supported",
			            protocol);
		}
		return NULL;
	}

	/* TODO: The current implementation here just calls the old
	 * implementations, without using any new options. This is just a first
	 * step to test the new interfaces. */
#if defined(USE_WEBSOCKET)
	if (is_ws) {
		/* TODO: implement all options */
		return mg_connect_websocket_client(
		    host,
		    port,
		    is_ssl,
		    ((error != NULL) ? error->text : NULL),
		    ((error != NULL) ? error->text_buffer_size : 0),
		    (path ? path : ""),
		    NULL /* TODO: origin */,
		    experimental_websocket_client_data_wrapper,
		    experimental_websocket_client_close_wrapper,
		    (void *)init->callbacks);
	}
#else
	(void)is_ws;
#endif

	/* TODO: all additional options */
	struct mg_client_options opts;

	memset(&opts, 0, sizeof(opts));
	opts.host = host;
	opts.port = port;

	return mg_connect_client_impl(&opts, is_ssl, init, error);
}
#endif


static const struct {
	const char *proto;
	size_t proto_len;
	unsigned default_port;
} abs_uri_protocols[] = {{"http://", 7, 80},
                         {"https://", 8, 443},
                         {"ws://", 5, 80},
                         {"wss://", 6, 443},
                         {NULL, 0, 0}};


/* Check if the uri is valid.
 * return 0 for invalid uri,
 * return 1 for *,
 * return 2 for relative uri,
 * return 3 for absolute uri without port,
 * return 4 for absolute uri with port */
static int
get_uri_type(const char *uri)
{
	int i;
	const char *hostend, *portbegin;
	char *portend;
	unsigned long port;

	/* According to the HTTP standard
	 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html#sec5.1.2
	 * URI can be an asterisk (*) or should start with slash (relative uri),
	 * or it should start with the protocol (absolute uri). */
	if ((uri[0] == '*') && (uri[1] == '\0')) {
		/* asterisk */
		return 1;
	}

	/* Valid URIs according to RFC 3986
	 * (https://www.ietf.org/rfc/rfc3986.txt)
	 * must only contain reserved characters :/?#[]@!$&'()*+,;=
	 * and unreserved characters A-Z a-z 0-9 and -._~
	 * and % encoded symbols.
	 */
	for (i = 0; uri[i] != 0; i++) {
		if ((unsigned char)uri[i] < 33) {
			/* control characters and spaces are invalid */
			return 0;
		}
		/* Allow everything else here (See #894) */
	}

	/* A relative uri starts with a / character */
	if (uri[0] == '/') {
		/* relative uri */
		return 2;
	}

	/* It could be an absolute uri: */
	/* This function only checks if the uri is valid, not if it is
	 * addressing the current server. So civetweb can also be used
	 * as a proxy server. */
	for (i = 0; abs_uri_protocols[i].proto != NULL; i++) {
		if (mg_strncasecmp(uri,
		                   abs_uri_protocols[i].proto,
		                   abs_uri_protocols[i].proto_len)
		    == 0) {

			hostend = strchr(uri + abs_uri_protocols[i].proto_len, '/');
			if (!hostend) {
				return 0;
			}
			portbegin = strchr(uri + abs_uri_protocols[i].proto_len, ':');
			if (!portbegin) {
				return 3;
			}

			port = strtoul(portbegin + 1, &portend, 10);
			if ((portend != hostend) || (port <= 0) || !is_valid_port(port)) {
				return 0;
			}

			return 4;
		}
	}

	return 0;
}


/* Return NULL or the relative uri at the current server */
static const char *
get_rel_url_at_current_server(const char *uri, const struct mg_connection *conn)
{
	const char *server_domain;
	size_t server_domain_len;
	size_t request_domain_len = 0;
	unsigned long port = 0;
	int i, auth_domain_check_enabled;
	const char *hostbegin = NULL;
	const char *hostend = NULL;
	const char *portbegin;
	char *portend;

	auth_domain_check_enabled =
	    !mg_strcasecmp(conn->dom_ctx->config[ENABLE_AUTH_DOMAIN_CHECK], "yes");

	/* DNS is case insensitive, so use case insensitive string compare here
	 */
	for (i = 0; abs_uri_protocols[i].proto != NULL; i++) {
		if (mg_strncasecmp(uri,
		                   abs_uri_protocols[i].proto,
		                   abs_uri_protocols[i].proto_len)
		    == 0) {

			hostbegin = uri + abs_uri_protocols[i].proto_len;
			hostend = strchr(hostbegin, '/');
			if (!hostend) {
				return 0;
			}
			portbegin = strchr(hostbegin, ':');
			if ((!portbegin) || (portbegin > hostend)) {
				port = abs_uri_protocols[i].default_port;
				request_domain_len = (size_t)(hostend - hostbegin);
			} else {
				port = strtoul(portbegin + 1, &portend, 10);
				if ((portend != hostend) || (port <= 0)
				    || !is_valid_port(port)) {
					return 0;
				}
				request_domain_len = (size_t)(portbegin - hostbegin);
			}
			/* protocol found, port set */
			break;
		}
	}

	if (!port) {
		/* port remains 0 if the protocol is not found */
		return 0;
	}

	/* Check if the request is directed to a different server. */
	/* First check if the port is the same. */
	if (ntohs(USA_IN_PORT_UNSAFE(&conn->client.lsa)) != port) {
		/* Request is directed to a different port */
		return 0;
	}

	/* Finally check if the server corresponds to the authentication
	 * domain of the server (the server domain).
	 * Allow full matches (like http://mydomain.com/path/file.ext), and
	 * allow subdomain matches (like http://www.mydomain.com/path/file.ext),
	 * but do not allow substrings (like
	 * http://notmydomain.com/path/file.ext
	 * or http://mydomain.com.fake/path/file.ext).
	 */
	if (auth_domain_check_enabled) {
		server_domain = conn->dom_ctx->config[AUTHENTICATION_DOMAIN];
		server_domain_len = strlen(server_domain);
		if ((server_domain_len == 0) || (hostbegin == NULL)) {
			return 0;
		}
		if ((request_domain_len == server_domain_len)
		    && (!memcmp(server_domain, hostbegin, server_domain_len))) {
			/* Request is directed to this server - full name match. */
		} else {
			if (request_domain_len < (server_domain_len + 2)) {
				/* Request is directed to another server: The server name
				 * is longer than the request name.
				 * Drop this case here to avoid overflows in the
				 * following checks. */
				return 0;
			}
			if (hostbegin[request_domain_len - server_domain_len - 1] != '.') {
				/* Request is directed to another server: It could be a
				 * substring
				 * like notmyserver.com */
				return 0;
			}
			if (0
			    != memcmp(server_domain,
			              hostbegin + request_domain_len - server_domain_len,
			              server_domain_len)) {
				/* Request is directed to another server:
				 * The server name is different. */
				return 0;
			}
		}
	}

	return hostend;
}


static int
get_message(struct mg_connection *conn, char *ebuf, size_t ebuf_len, int *err)
{
	if (ebuf_len > 0) {
		ebuf[0] = '\0';
	}
	*err = 0;

	reset_per_request_attributes(conn);

	if (!conn) {
		mg_snprintf(conn,
		            NULL, /* No truncation check for ebuf */
		            ebuf,
		            ebuf_len,
		            "%s",
		            "Internal error");
		*err = 500;
		return 0;
	}

	/* Set the time the request was received. This value should be used for
	 * timeouts. */
	clock_gettime(CLOCK_MONOTONIC, &(conn->req_time));

	conn->request_len =
	    read_message(NULL, conn, conn->buf, conn->buf_size, &conn->data_len);
	DEBUG_ASSERT(conn->request_len < 0 || conn->data_len >= conn->request_len);
	if ((conn->request_len >= 0) && (conn->data_len < conn->request_len)) {
		mg_snprintf(conn,
		            NULL, /* No truncation check for ebuf */
		            ebuf,
		            ebuf_len,
		            "%s",
		            "Invalid message size");
		*err = 500;
		return 0;
	}

	if ((conn->request_len == 0) && (conn->data_len == conn->buf_size)) {
		mg_snprintf(conn,
		            NULL, /* No truncation check for ebuf */
		            ebuf,
		            ebuf_len,
		            "%s",
		            "Message too large");
		*err = 413;
		return 0;
	}

	if (conn->request_len <= 0) {
		if (conn->data_len > 0) {
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s",
			            conn->request_len == -3 ? "Request timeout"
			                                    : "Malformed message");
			*err = 400;
		} else {
			/* Server did not recv anything -> just close the connection */
			conn->must_close = 1;
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s",
			            "No data received");
			*err = 0;
		}
		return 0;
	}
	return 1;
}


static int
get_request(struct mg_connection *conn, char *ebuf, size_t ebuf_len, int *err)
{
	const char *cl;

	conn->connection_type =
	    CONNECTION_TYPE_REQUEST; /* request (valid of not) */

	if (!get_message(conn, ebuf, ebuf_len, err)) {
		return 0;
	}

	if (parse_http_request(conn->buf, conn->buf_size, &conn->request_info)
	    <= 0) {
		mg_snprintf(conn,
		            NULL, /* No truncation check for ebuf */
		            ebuf,
		            ebuf_len,
		            "%s",
		            "Bad request");
		*err = 400;
		return 0;
	}

	/* Message is a valid request */

	if (!switch_domain_context(conn)) {
		mg_snprintf(conn,
		            NULL, /* No truncation check for ebuf */
		            ebuf,
		            ebuf_len,
		            "%s",
		            "Bad request: Host mismatch");
		*err = 400;
		return 0;
	}

#if USE_ZLIB
	if (((cl = get_header(conn->request_info.http_headers,
	                      conn->request_info.num_headers,
	                      "Accept-Encoding"))
	     != NULL)
	    && strstr(cl, "gzip")) {
		conn->accept_gzip = 1;
	}
#endif
	if (((cl = get_header(conn->request_info.http_headers,
	                      conn->request_info.num_headers,
	                      "Transfer-Encoding"))
	     != NULL)
	    && mg_strcasecmp(cl, "identity")) {
		if (mg_strcasecmp(cl, "chunked")) {
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s",
			            "Bad request");
			*err = 400;
			return 0;
		}
		conn->is_chunked = 1;
		conn->content_len = 0; /* not yet read */
	} else if ((cl = get_header(conn->request_info.http_headers,
	                            conn->request_info.num_headers,
	                            "Content-Length"))
	           != NULL) {
		/* Request has content length set */
		char *endptr = NULL;
		conn->content_len = strtoll(cl, &endptr, 10);
		if ((endptr == cl) || (conn->content_len < 0)) {
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s",
			            "Bad request");
			*err = 411;
			return 0;
		}
		/* Publish the content length back to the request info. */
		conn->request_info.content_length = conn->content_len;
	} else {
		/* There is no exception, see RFC7230. */
		conn->content_len = 0;
	}

	return 1;
}


/* conn is assumed to be valid in this internal function */
static int
get_response(struct mg_connection *conn, char *ebuf, size_t ebuf_len, int *err)
{
	const char *cl;

	conn->connection_type =
	    CONNECTION_TYPE_RESPONSE; /* response (valid or not) */

	if (!get_message(conn, ebuf, ebuf_len, err)) {
		return 0;
	}

	if (parse_http_response(conn->buf, conn->buf_size, &conn->response_info)
	    <= 0) {
		mg_snprintf(conn,
		            NULL, /* No truncation check for ebuf */
		            ebuf,
		            ebuf_len,
		            "%s",
		            "Bad response");
		*err = 400;
		return 0;
	}

	/* Message is a valid response */

	if (((cl = get_header(conn->response_info.http_headers,
	                      conn->response_info.num_headers,
	                      "Transfer-Encoding"))
	     != NULL)
	    && mg_strcasecmp(cl, "identity")) {
		if (mg_strcasecmp(cl, "chunked")) {
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s",
			            "Bad request");
			*err = 400;
			return 0;
		}
		conn->is_chunked = 1;
		conn->content_len = 0; /* not yet read */
	} else if ((cl = get_header(conn->response_info.http_headers,
	                            conn->response_info.num_headers,
	                            "Content-Length"))
	           != NULL) {
		char *endptr = NULL;
		conn->content_len = strtoll(cl, &endptr, 10);
		if ((endptr == cl) || (conn->content_len < 0)) {
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s",
			            "Bad request");
			*err = 411;
			return 0;
		}
		/* Publish the content length back to the response info. */
		conn->response_info.content_length = conn->content_len;

		/* TODO: check if it is still used in response_info */
		conn->request_info.content_length = conn->content_len;

		/* TODO: we should also consider HEAD method */
		if (conn->response_info.status_code == 304) {
			conn->content_len = 0;
		}
	} else {
		/* TODO: we should also consider HEAD method */
		if (((conn->response_info.status_code >= 100)
		     && (conn->response_info.status_code <= 199))
		    || (conn->response_info.status_code == 204)
		    || (conn->response_info.status_code == 304)) {
			conn->content_len = 0;
		} else {
			conn->content_len = -1; /* unknown content length */
		}
	}

	return 1;
}


CIVETWEB_API int
mg_get_response(struct mg_connection *conn,
                char *ebuf,
                size_t ebuf_len,
                int timeout)
{
	int err, ret;
	char txt[32]; /* will not overflow */
	char *save_timeout;
	char *new_timeout;

	if (ebuf_len > 0) {
		ebuf[0] = '\0';
	}

	if (!conn) {
		mg_snprintf(conn,
		            NULL, /* No truncation check for ebuf */
		            ebuf,
		            ebuf_len,
		            "%s",
		            "Parameter error");
		return -1;
	}

	/* Reset the previous responses */
	conn->data_len = 0;

	/* Implementation of API function for HTTP clients */
	save_timeout = conn->dom_ctx->config[REQUEST_TIMEOUT];

	if (timeout >= 0) {
		mg_snprintf(conn, NULL, txt, sizeof(txt), "%i", timeout);
		new_timeout = txt;
	} else {
		new_timeout = NULL;
	}

	conn->dom_ctx->config[REQUEST_TIMEOUT] = new_timeout;
	ret = get_response(conn, ebuf, ebuf_len, &err);
	conn->dom_ctx->config[REQUEST_TIMEOUT] = save_timeout;

	/* TODO: here, the URI is the http response code */
	conn->request_info.local_uri_raw = conn->request_info.request_uri;
	conn->request_info.local_uri = conn->request_info.local_uri_raw;

	/* TODO (mid): Define proper return values - maybe return length?
	 * For the first test use <0 for error and >0 for OK */
	return (ret == 0) ? -1 : +1;
}


CIVETWEB_API struct mg_connection *
mg_download(const char *host,
            int port,
            int use_ssl,
            char *ebuf,
            size_t ebuf_len,
            const char *fmt,
            ...)
{
	struct mg_connection *conn;
	va_list ap;
	int i;
	int reqerr;

	if (ebuf_len > 0) {
		ebuf[0] = '\0';
	}

	va_start(ap, fmt);

	/* open a connection */
	conn = mg_connect_client(host, port, use_ssl, ebuf, ebuf_len);

	if (conn != NULL) {
		i = mg_vprintf(conn, fmt, ap);
		if (i <= 0) {
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            ebuf_len,
			            "%s",
			            "Error sending request");
		} else {
			/* make sure the buffer is clear */
			conn->data_len = 0;
			get_response(conn, ebuf, ebuf_len, &reqerr);

			/* TODO: here, the URI is the http response code */
			conn->request_info.local_uri = conn->request_info.request_uri;
		}
	}

	/* if an error occurred, close the connection */
	if ((ebuf[0] != '\0') && (conn != NULL)) {
		mg_close_connection(conn);
		conn = NULL;
	}

	va_end(ap);
	return conn;
}


struct websocket_client_thread_data {
	struct mg_connection *conn;
	mg_websocket_data_handler data_handler;
	mg_websocket_close_handler close_handler;
	void *callback_data;
};


#if defined(USE_WEBSOCKET)
#if defined(_WIN32)
static unsigned __stdcall websocket_client_thread(void *data)
#else
static void *
websocket_client_thread(void *data)
#endif
{
	struct websocket_client_thread_data *cdata =
	    (struct websocket_client_thread_data *)data;

	void *user_thread_ptr = NULL;

#if !defined(_WIN32) && !defined(__ZEPHYR__)
	struct sigaction sa;

	/* Ignore SIGPIPE */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
#endif

	mg_set_thread_name("ws-clnt");

	if (cdata->conn->phys_ctx) {
		if (cdata->conn->phys_ctx->callbacks.init_thread) {
			/* 3 indicates a websocket client thread */
			/* TODO: check if conn->phys_ctx can be set */
			user_thread_ptr = cdata->conn->phys_ctx->callbacks.init_thread(
			    cdata->conn->phys_ctx, 3);
		}
	}

	read_websocket(cdata->conn, cdata->data_handler, cdata->callback_data);

	DEBUG_TRACE("%s", "Websocket client thread exited\n");

	if (cdata->close_handler != NULL) {
		cdata->close_handler(cdata->conn, cdata->callback_data);
	}

	/* The websocket_client context has only this thread. If it runs out,
	set the stop_flag to 2 (= "stopped"). */
	STOP_FLAG_ASSIGN(&cdata->conn->phys_ctx->stop_flag, 2);

	if (cdata->conn->phys_ctx->callbacks.exit_thread) {
		cdata->conn->phys_ctx->callbacks.exit_thread(cdata->conn->phys_ctx,
		                                             3,
		                                             user_thread_ptr);
	}

	mg_free((void *)cdata);

#if defined(_WIN32)
	return 0;
#else
	return NULL;
#endif
}
#endif


#if defined(USE_WEBSOCKET)
static void
generate_websocket_magic(char *magic25)
{
	uint64_t rnd;
	unsigned char buffer[2 * sizeof(rnd)];

	rnd = get_random();
	memcpy(buffer, &rnd, sizeof(rnd));
	rnd = get_random();
	memcpy(buffer + sizeof(rnd), &rnd, sizeof(rnd));

	size_t dst_len = 24 + 1;
	mg_base64_encode(buffer, sizeof(buffer), magic25, &dst_len);
}
#endif


static struct mg_connection *
mg_connect_websocket_client_impl(const struct mg_client_options *client_options,
                                 int use_ssl,
                                 char *error_buffer,
                                 size_t error_buffer_size,
                                 const char *path,
                                 const char *origin,
                                 const char *extensions,
                                 mg_websocket_data_handler data_func,
                                 mg_websocket_close_handler close_func,
                                 void *user_data)
{
	struct mg_connection *conn = NULL;

#if defined(USE_WEBSOCKET)
	struct websocket_client_thread_data *thread_data;
	char magic[32];
	generate_websocket_magic(magic);

	const char *host = client_options->host;
	int i;

	struct mg_init_data init;
	struct mg_error_data error;

	memset(&init, 0, sizeof(init));
	memset(&error, 0, sizeof(error));
	error.text_buffer_size = error_buffer_size;
	error.text = error_buffer;

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif

	/* Establish the client connection and request upgrade */
	conn = mg_connect_client_impl(client_options, use_ssl, &init, &error);

	/* Connection object will be null if something goes wrong */
	if (conn == NULL) {
		/* error_buffer should be already filled ... */
		if (!error_buffer[0]) {
			/* ... if not add an error message */
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            error_buffer,
			            error_buffer_size,
			            "Unexpected error");
		}
		return NULL;
	}

	if (origin != NULL) {
		if (extensions != NULL) {
			i = mg_printf(conn,
			              "GET %s HTTP/1.1\r\n"
			              "Host: %s\r\n"
			              "Upgrade: websocket\r\n"
			              "Connection: Upgrade\r\n"
			              "Sec-WebSocket-Key: %s\r\n"
			              "Sec-WebSocket-Version: 13\r\n"
			              "Sec-WebSocket-Extensions: %s\r\n"
			              "Origin: %s\r\n"
			              "\r\n",
			              path,
			              host,
			              magic,
			              extensions,
			              origin);
		} else {
			i = mg_printf(conn,
			              "GET %s HTTP/1.1\r\n"
			              "Host: %s\r\n"
			              "Upgrade: websocket\r\n"
			              "Connection: Upgrade\r\n"
			              "Sec-WebSocket-Key: %s\r\n"
			              "Sec-WebSocket-Version: 13\r\n"
			              "Origin: %s\r\n"
			              "\r\n",
			              path,
			              host,
			              magic,
			              origin);
		}
	} else {

		if (extensions != NULL) {
			i = mg_printf(conn,
			              "GET %s HTTP/1.1\r\n"
			              "Host: %s\r\n"
			              "Upgrade: websocket\r\n"
			              "Connection: Upgrade\r\n"
			              "Sec-WebSocket-Key: %s\r\n"
			              "Sec-WebSocket-Version: 13\r\n"
			              "Sec-WebSocket-Extensions: %s\r\n"
			              "\r\n",
			              path,
			              host,
			              magic,
			              extensions);
		} else {
			i = mg_printf(conn,
			              "GET %s HTTP/1.1\r\n"
			              "Host: %s\r\n"
			              "Upgrade: websocket\r\n"
			              "Connection: Upgrade\r\n"
			              "Sec-WebSocket-Key: %s\r\n"
			              "Sec-WebSocket-Version: 13\r\n"
			              "\r\n",
			              path,
			              host,
			              magic);
		}
	}
	if (i <= 0) {
		mg_snprintf(conn,
		            NULL, /* No truncation check for ebuf */
		            error_buffer,
		            error_buffer_size,
		            "%s",
		            "Error sending request");
		mg_close_connection(conn);
		return NULL;
	}

	conn->data_len = 0;
	if (!get_response(conn, error_buffer, error_buffer_size, &i)) {
		mg_close_connection(conn);
		return NULL;
	}
	conn->request_info.local_uri_raw = conn->request_info.request_uri;
	conn->request_info.local_uri = conn->request_info.local_uri_raw;

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

	if (conn->response_info.status_code != 101) {
		/* We sent an "upgrade" request. For a correct websocket
		 * protocol handshake, we expect a "101 Continue" response.
		 * Otherwise it is a protocol violation. Maybe the HTTP
		 * Server does not know websockets. */
		if (!*error_buffer) {
			/* set an error, if not yet set */
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            error_buffer,
			            error_buffer_size,
			            "Unexpected server reply");
		}

		DEBUG_TRACE("Websocket client connect error: %s\r\n", error_buffer);
		mg_close_connection(conn);
		return NULL;
	}

	thread_data = (struct websocket_client_thread_data *)mg_calloc_ctx(
	    1, sizeof(struct websocket_client_thread_data), conn->phys_ctx);
	if (!thread_data) {
		DEBUG_TRACE("%s\r\n", "Out of memory");
		mg_close_connection(conn);
		return NULL;
	}

	thread_data->conn = conn;
	thread_data->data_handler = data_func;
	thread_data->close_handler = close_func;
	thread_data->callback_data = user_data;

	conn->phys_ctx->worker_threadids =
	    (pthread_t *)mg_calloc_ctx(1, sizeof(pthread_t), conn->phys_ctx);
	if (!conn->phys_ctx->worker_threadids) {
		DEBUG_TRACE("%s\r\n", "Out of memory");
		mg_free(thread_data);
		mg_close_connection(conn);
		return NULL;
	}

	/* Now upgrade to ws/wss client context */
	conn->phys_ctx->user_data = user_data;
	conn->phys_ctx->context_type = CONTEXT_WS_CLIENT;
	conn->phys_ctx->cfg_max_worker_threads = 1; /* one worker thread */
	conn->phys_ctx->spawned_worker_threads = 1; /* one worker thread */

	/* Start a thread to read the websocket client connection
	 * This thread will automatically stop when mg_disconnect is
	 * called on the client connection */
	if (mg_start_thread_with_id(websocket_client_thread,
	                            thread_data,
	                            conn->phys_ctx->worker_threadids)
	    != 0) {
		conn->phys_ctx->spawned_worker_threads = 0;
		mg_free(thread_data);
		mg_close_connection(conn);
		conn = NULL;
		DEBUG_TRACE("%s",
		            "Websocket client connect thread could not be started\r\n");
	}

#else
	/* Appease "unused parameter" warnings */
	(void)client_options;
	(void)use_ssl;
	(void)error_buffer;
	(void)error_buffer_size;
	(void)path;
	(void)origin;
	(void)extensions;
	(void)user_data;
	(void)data_func;
	(void)close_func;
#endif

	return conn;
}


CIVETWEB_API struct mg_connection *
mg_connect_websocket_client(const char *host,
                            int port,
                            int use_ssl,
                            char *error_buffer,
                            size_t error_buffer_size,
                            const char *path,
                            const char *origin,
                            mg_websocket_data_handler data_func,
                            mg_websocket_close_handler close_func,
                            void *user_data)
{
	struct mg_client_options client_options;
	memset(&client_options, 0, sizeof(client_options));
	client_options.host = host;
	client_options.port = port;
	if (use_ssl) {
		client_options.host_name = host;
	}

	return mg_connect_websocket_client_impl(&client_options,
	                                        use_ssl,
	                                        error_buffer,
	                                        error_buffer_size,
	                                        path,
	                                        origin,
	                                        NULL,
	                                        data_func,
	                                        close_func,
	                                        user_data);
}


CIVETWEB_API struct mg_connection *
mg_connect_websocket_client_secure(
    const struct mg_client_options *client_options,
    char *error_buffer,
    size_t error_buffer_size,
    const char *path,
    const char *origin,
    mg_websocket_data_handler data_func,
    mg_websocket_close_handler close_func,
    void *user_data)
{
	if (!client_options) {
		return NULL;
	}
	return mg_connect_websocket_client_impl(client_options,
	                                        1,
	                                        error_buffer,
	                                        error_buffer_size,
	                                        path,
	                                        origin,
	                                        NULL,
	                                        data_func,
	                                        close_func,
	                                        user_data);
}


CIVETWEB_API struct mg_connection *
mg_connect_websocket_client_extensions(const char *host,
                                       int port,
                                       int use_ssl,
                                       char *error_buffer,
                                       size_t error_buffer_size,
                                       const char *path,
                                       const char *origin,
                                       const char *extensions,
                                       mg_websocket_data_handler data_func,
                                       mg_websocket_close_handler close_func,
                                       void *user_data)
{
	struct mg_client_options client_options;
	memset(&client_options, 0, sizeof(client_options));
	client_options.host = host;
	client_options.port = port;

	return mg_connect_websocket_client_impl(&client_options,
	                                        use_ssl,
	                                        error_buffer,
	                                        error_buffer_size,
	                                        path,
	                                        origin,
	                                        extensions,
	                                        data_func,
	                                        close_func,
	                                        user_data);
}


CIVETWEB_API struct mg_connection *
mg_connect_websocket_client_secure_extensions(
    const struct mg_client_options *client_options,
    char *error_buffer,
    size_t error_buffer_size,
    const char *path,
    const char *origin,
    const char *extensions,
    mg_websocket_data_handler data_func,
    mg_websocket_close_handler close_func,
    void *user_data)
{
	if (!client_options) {
		return NULL;
	}
	return mg_connect_websocket_client_impl(client_options,
	                                        1,
	                                        error_buffer,
	                                        error_buffer_size,
	                                        path,
	                                        origin,
	                                        extensions,
	                                        data_func,
	                                        close_func,
	                                        user_data);
}


/* Prepare connection data structure */
static void
init_connection(struct mg_connection *conn)
{
	/* Is keep alive allowed by the server */
	int keep_alive_enabled =
	    !mg_strcasecmp(conn->dom_ctx->config[ENABLE_KEEP_ALIVE], "yes");

	if (!keep_alive_enabled) {
		conn->must_close = 1;
	}

	/* Important: on new connection, reset the receiving buffer. Credit
	 * goes to crule42. */
	conn->data_len = 0;
	conn->handled_requests = 0;
	conn->connection_type = CONNECTION_TYPE_INVALID;
	conn->request_info.acceptedWebSocketSubprotocol = NULL;
	mg_set_user_connection_data(conn, NULL);

#if defined(USE_SERVER_STATS)
	conn->conn_state = 2; /* init */
#endif

	/* call the init_connection callback if assigned */
	if (conn->phys_ctx->callbacks.init_connection != NULL) {
		if (conn->phys_ctx->context_type == CONTEXT_SERVER) {
			void *conn_data = NULL;
			conn->phys_ctx->callbacks.init_connection(conn, &conn_data);
			mg_set_user_connection_data(conn, conn_data);
		}
	}
}


/* Process a connection - may handle multiple requests
 * using the same connection.
 * Must be called with a valid connection (conn  and
 * conn->phys_ctx must be valid).
 */
static void
process_new_connection(struct mg_connection *conn)
{
	struct mg_request_info *ri = &conn->request_info;
	int keep_alive, discard_len;
	char ebuf[100];
	const char *hostend;
	int reqerr, uri_type;

#if defined(USE_SERVER_STATS)
	ptrdiff_t mcon = mg_atomic_inc(&(conn->phys_ctx->active_connections));
	mg_atomic_add(&(conn->phys_ctx->total_connections), 1);
	mg_atomic_max(&(conn->phys_ctx->max_active_connections), mcon);
#endif

	DEBUG_TRACE("Start processing connection from %s",
	            conn->request_info.remote_addr);

	/* Loop over multiple requests sent using the same connection
	 * (while "keep alive"). */
	do {
		DEBUG_TRACE("calling get_request (%i times for this connection)",
		            conn->handled_requests + 1);

#if defined(USE_SERVER_STATS)
		conn->conn_state = 3; /* ready */
#endif

		if (!get_request(conn, ebuf, sizeof(ebuf), &reqerr)) {
			/* The request sent by the client could not be understood by
			 * the server, or it was incomplete or a timeout. Send an
			 * error message and close the connection. */
			if (reqerr > 0) {
				DEBUG_ASSERT(ebuf[0] != '\0');
				mg_send_http_error(conn, reqerr, "%s", ebuf);
			}

		} else if (strcmp(ri->http_version, "1.0")
		           && strcmp(ri->http_version, "1.1")) {
			/* HTTP/2 is not allowed here */
			mg_snprintf(conn,
			            NULL, /* No truncation check for ebuf */
			            ebuf,
			            sizeof(ebuf),
			            "Bad HTTP version: [%s]",
			            ri->http_version);
			mg_send_http_error(conn, 505, "%s", ebuf);
		}

		if (ebuf[0] == '\0') {
			uri_type = get_uri_type(conn->request_info.request_uri);
			switch (uri_type) {
			case 1:
				/* Asterisk */
				conn->request_info.local_uri_raw = 0;
				/* TODO: Deal with '*'. */
				break;
			case 2:
				/* relative uri */
				conn->request_info.local_uri_raw =
				    conn->request_info.request_uri;
				break;
			case 3:
			case 4:
				/* absolute uri (with/without port) */
				hostend = get_rel_url_at_current_server(
				    conn->request_info.request_uri, conn);
				if (hostend) {
					conn->request_info.local_uri_raw = hostend;
				} else {
					conn->request_info.local_uri_raw = NULL;
				}
				break;
			default:
				mg_snprintf(conn,
				            NULL, /* No truncation check for ebuf */
				            ebuf,
				            sizeof(ebuf),
				            "Invalid URI");
				mg_send_http_error(conn, 400, "%s", ebuf);
				conn->request_info.local_uri_raw = NULL;
				break;
			}
			conn->request_info.local_uri =
			    (char *)conn->request_info.local_uri_raw;
		}

		if (ebuf[0] != '\0') {
			conn->protocol_type = -1;

		} else {
			/* HTTP/1 allows protocol upgrade */
			conn->protocol_type = should_switch_to_protocol(conn);

			if (conn->protocol_type == PROTOCOL_TYPE_HTTP2) {
				/* This will occur, if a HTTP/1.1 request should be upgraded
				 * to HTTP/2 - but not if HTTP/2 is negotiated using ALPN.
				 * Since most (all?) major browsers only support HTTP/2 using
				 * ALPN, this is hard to test and very low priority.
				 * Deactivate it (at least for now).
				 */
				conn->protocol_type = PROTOCOL_TYPE_HTTP1;
			}
		}

		DEBUG_TRACE("http: %s, error: %s",
		            (ri->http_version ? ri->http_version : "none"),
		            (ebuf[0] ? ebuf : "none"));

		if (ebuf[0] == '\0') {
			if (conn->request_info.local_uri) {

				/* handle request to local server */
				handle_request_stat_log(conn);

			} else {
				/* TODO: handle non-local request (PROXY) */
				conn->must_close = 1;
			}
		} else {
			conn->must_close = 1;
		}

		/* Response complete. Free header buffer */
		free_buffered_response_header_list(conn);

		if (ri->remote_user != NULL) {
			mg_free((void *)ri->remote_user);
			/* Important! When having connections with and without auth
			 * would cause double free and then crash */
			ri->remote_user = NULL;
		}

		/* NOTE(lsm): order is important here. should_keep_alive() call
		 * is using parsed request, which will be invalid after
		 * memmove's below.
		 * Therefore, memorize should_keep_alive() result now for later
		 * use in loop exit condition. */
		/* Enable it only if this request is completely discardable. */
		keep_alive = STOP_FLAG_IS_ZERO(&conn->phys_ctx->stop_flag)
		             && should_keep_alive(conn) && (conn->content_len >= 0)
		             && (conn->request_len > 0)
		             && ((conn->is_chunked == 4)
		                 || (!conn->is_chunked
		                     && ((conn->consumed_content == conn->content_len)
		                         || ((conn->request_len + conn->content_len)
		                             <= conn->data_len))))
		             && (conn->protocol_type == PROTOCOL_TYPE_HTTP1);

		if (keep_alive) {
			/* Discard all buffered data for this request */
			discard_len =
			    ((conn->request_len + conn->content_len) < conn->data_len)
			        ? (int)(conn->request_len + conn->content_len)
			        : conn->data_len;
			conn->data_len -= discard_len;

			if (conn->data_len > 0) {
				DEBUG_TRACE("discard_len = %d", discard_len);
				memmove(conn->buf,
				        conn->buf + discard_len,
				        (size_t)conn->data_len);
			}
		}

		DEBUG_ASSERT(conn->data_len >= 0);
		DEBUG_ASSERT(conn->data_len <= conn->buf_size);

		if ((conn->data_len < 0) || (conn->data_len > conn->buf_size)) {
			DEBUG_TRACE("internal error: data_len = %li, buf_size = %li",
			            (long int)conn->data_len,
			            (long int)conn->buf_size);
			break;
		}
		conn->handled_requests++;
	} while (keep_alive);

	DEBUG_TRACE("Done processing connection from %s (%f sec)",
	            conn->request_info.remote_addr,
	            difftime(time(NULL), conn->conn_birth_time));

	close_connection(conn);

#if defined(USE_SERVER_STATS)
	mg_atomic_add(&(conn->phys_ctx->total_requests), conn->handled_requests);
	mg_atomic_dec(&(conn->phys_ctx->active_connections));
#endif
}

static int
mg_start_worker_thread(struct mg_context *ctx,
                       int only_if_no_idle_threads); /* forward declaration */

#if defined(ALTERNATIVE_QUEUE)

static void
produce_socket(struct mg_context *ctx, const struct socket *sp)
{
	unsigned int i;

	(void)mg_start_worker_thread(
	    ctx, 1); /* will start a worker-thread only if there aren't currently
	                any idle worker-threads */

	while (!ctx->stop_flag) {
		for (i = 0; i < ctx->spawned_worker_threads; i++) {
			/* find a free worker slot and signal it */
			if (ctx->client_socks[i].in_use == 2) {
				(void)pthread_mutex_lock(&ctx->thread_mutex);
				if ((ctx->client_socks[i].in_use == 2) && !ctx->stop_flag) {
					ctx->client_socks[i] = *sp;
					ctx->client_socks[i].in_use = 1;
					/* socket has been moved to the consumer */
					(void)pthread_mutex_unlock(&ctx->thread_mutex);
					(void)event_signal(ctx->client_wait_events[i]);
					return;
				}
				(void)pthread_mutex_unlock(&ctx->thread_mutex);
			}
		}
		/* queue is full */
		mg_sleep(1);
	}
	/* must consume */
	set_blocking_mode(sp->sock);
	closesocket(sp->sock);
}


static int
consume_socket(struct mg_context *ctx,
               struct socket *sp,
               int thread_index,
               int counter_was_preincremented)
{
	DEBUG_TRACE("%s", "going idle");
	(void)pthread_mutex_lock(&ctx->thread_mutex);
	if (counter_was_preincremented
	    == 0) { /* first call only: the master-thread pre-incremented this
		           before he spawned us */
		ctx->idle_worker_thread_count++;
	}
	ctx->client_socks[thread_index].in_use = 2;
	(void)pthread_mutex_unlock(&ctx->thread_mutex);

	event_wait(ctx->client_wait_events[thread_index]);

	(void)pthread_mutex_lock(&ctx->thread_mutex);
	*sp = ctx->client_socks[thread_index];
	if (ctx->stop_flag) {
		(void)pthread_mutex_unlock(&ctx->thread_mutex);
		if (sp->in_use == 1) {
			/* must consume */
			set_blocking_mode(sp->sock);
			closesocket(sp->sock);
		}
		return 0;
	}
	ctx->idle_worker_thread_count--;
	(void)pthread_mutex_unlock(&ctx->thread_mutex);
	if (sp->in_use == 1) {
		DEBUG_TRACE("grabbed socket %d, going busy", sp->sock);
		return 1;
	}
	/* must not reach here */
	DEBUG_ASSERT(0);
	return 0;
}

#else /* ALTERNATIVE_QUEUE */

/* Worker threads take accepted socket from the queue */
static int
consume_socket(struct mg_context *ctx,
               struct socket *sp,
               int thread_index,
               int counter_was_preincremented)
{
	(void)thread_index;

	DEBUG_TRACE("%s", "going idle");
	(void)pthread_mutex_lock(&ctx->thread_mutex);
	if (counter_was_preincremented
	    == 0) { /* first call only: the master-thread pre-incremented this
		           before he spawned us */
		ctx->idle_worker_thread_count++;
	}

	/* If the queue is empty, wait. We're idle at this point. */
	while ((ctx->sq_head == ctx->sq_tail)
	       && (STOP_FLAG_IS_ZERO(&ctx->stop_flag))) {
		pthread_cond_wait(&ctx->sq_full, &ctx->thread_mutex);
	}

	/* If we're stopping, sq_head may be equal to sq_tail. */
	if (ctx->sq_head > ctx->sq_tail) {
		/* Copy socket from the queue and increment tail */
		*sp = ctx->squeue[ctx->sq_tail % ctx->sq_size];
		ctx->sq_tail++;

		DEBUG_TRACE("grabbed socket %d, going busy", sp ? sp->sock : -1);

		/* Wrap pointers if needed */
		while (ctx->sq_tail > ctx->sq_size) {
			ctx->sq_tail -= ctx->sq_size;
			ctx->sq_head -= ctx->sq_size;
		}
	}

	(void)pthread_cond_signal(&ctx->sq_empty);

	ctx->idle_worker_thread_count--;
	(void)pthread_mutex_unlock(&ctx->thread_mutex);

	return STOP_FLAG_IS_ZERO(&ctx->stop_flag);
}


/* Master thread adds accepted socket to a queue */
static void
produce_socket(struct mg_context *ctx, const struct socket *sp)
{
	int queue_filled;

	(void)pthread_mutex_lock(&ctx->thread_mutex);

	queue_filled = ctx->sq_head - ctx->sq_tail;

	/* If the queue is full, wait */
	while (STOP_FLAG_IS_ZERO(&ctx->stop_flag)
	       && (queue_filled >= ctx->sq_size)) {
		ctx->sq_blocked = 1; /* Status information: All threads busy */
#if defined(USE_SERVER_STATS)
		if (queue_filled > ctx->sq_max_fill) {
			ctx->sq_max_fill = queue_filled;
		}
#endif
		(void)pthread_cond_wait(&ctx->sq_empty, &ctx->thread_mutex);
		ctx->sq_blocked = 0; /* Not blocked now */
		queue_filled = ctx->sq_head - ctx->sq_tail;
	}

	if (queue_filled < ctx->sq_size) {
		/* Copy socket to the queue and increment head */
		ctx->squeue[ctx->sq_head % ctx->sq_size] = *sp;
		ctx->sq_head++;
		DEBUG_TRACE("queued socket %d", sp ? sp->sock : -1);
	}

	queue_filled = ctx->sq_head - ctx->sq_tail;
#if defined(USE_SERVER_STATS)
	if (queue_filled > ctx->sq_max_fill) {
		ctx->sq_max_fill = queue_filled;
	}
#endif

	(void)pthread_cond_signal(&ctx->sq_full);
	(void)pthread_mutex_unlock(&ctx->thread_mutex);

	(void)mg_start_worker_thread(
	    ctx, 1); /* will start a worker-thread only if there aren't currently
	                any idle worker-threads */
}
#endif /* ALTERNATIVE_QUEUE */


static void
worker_thread_run(struct mg_connection *conn)
{
	struct mg_context *ctx = conn->phys_ctx;
	int thread_index;
	struct mg_workerTLS tls;
	int first_call_to_consume_socket = 1;

	mg_set_thread_name("worker");

	tls.is_master = 0;
	tls.thread_idx = (unsigned)mg_atomic_inc(&thread_idx_max);
#if defined(_WIN32)
	tls.pthread_cond_helper_mutex = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif

	/* Initialize thread local storage before calling any callback */
	pthread_setspecific(sTlsKey, &tls);

	/* Check if there is a user callback */
	if (ctx->callbacks.init_thread) {
		/* call init_thread for a worker thread (type 1), and store the
		 * return value */
		tls.user_ptr = ctx->callbacks.init_thread(ctx, 1);
	} else {
		/* No callback: set user pointer to NULL */
		tls.user_ptr = NULL;
	}

	/* Connection structure has been pre-allocated */
	thread_index = (int)(conn - ctx->worker_connections);
	if ((thread_index < 0)
	    || ((unsigned)thread_index >= (unsigned)ctx->cfg_max_worker_threads)) {
		mg_cry_ctx_internal(ctx,
		                    "Internal error: Invalid worker index %i",
		                    thread_index);
		return;
	}

	/* Request buffers are not pre-allocated. They are private to the
	 * request and do not contain any state information that might be
	 * of interest to anyone observing a server status.  */
	conn->buf = (char *)mg_malloc_ctx(ctx->max_request_size, conn->phys_ctx);
	if (conn->buf == NULL) {
		mg_cry_ctx_internal(
		    ctx,
		    "Out of memory: Cannot allocate buffer for worker %i",
		    thread_index);
		return;
	}
	conn->buf_size = (int)ctx->max_request_size;

	conn->dom_ctx = &(ctx->dd); /* Use default domain and default host */

	conn->tls_user_ptr = tls.user_ptr; /* store ptr for quick access */

	conn->request_info.user_data = ctx->user_data;
	/* Allocate a mutex for this connection to allow communication both
	 * within the request handler and from elsewhere in the application
	 */
	if (0 != pthread_mutex_init(&conn->mutex, &pthread_mutex_attr)) {
		mg_free(conn->buf);
		mg_cry_ctx_internal(ctx, "%s", "Cannot create mutex");
		return;
	}

#if defined(USE_SERVER_STATS)
	conn->conn_state = 1; /* not consumed */
#endif

	/* Call consume_socket() even when ctx->stop_flag > 0, to let it
	 * signal sq_empty condvar to wake up the master waiting in
	 * produce_socket() */
	while (consume_socket(
	    ctx, &conn->client, thread_index, first_call_to_consume_socket)) {
		first_call_to_consume_socket = 0;

		/* New connections must start with new protocol negotiation */
		tls.alpn_proto = NULL;

#if defined(USE_SERVER_STATS)
		conn->conn_close_time = 0;
#endif
		conn->conn_birth_time = time(NULL);

		/* Fill in IP, port info early so even if SSL setup below fails,
		 * error handler would have the corresponding info.
		 * Thanks to Johannes Winkelmann for the patch.
		 */
		conn->request_info.remote_port =
		    ntohs(USA_IN_PORT_UNSAFE(&conn->client.rsa));

		conn->request_info.server_port =
		    ntohs(USA_IN_PORT_UNSAFE(&conn->client.lsa));

		sockaddr_to_string(conn->request_info.remote_addr,
		                   sizeof(conn->request_info.remote_addr),
		                   &conn->client.rsa);

		DEBUG_TRACE("Incoming %sconnection from %s",
		            (conn->client.is_ssl ? "SSL " : ""),
		            conn->request_info.remote_addr);

		conn->request_info.is_ssl = conn->client.is_ssl;

		if (conn->client.is_ssl) {

#if defined(USE_MBEDTLS)
			/* HTTPS connection */
			if (mbed_ssl_accept(&(conn->ssl),
			                    conn->dom_ctx->ssl_ctx,
			                    (int *)&(conn->client.sock),
			                    conn->phys_ctx)
			    == 0) {
				/* conn->dom_ctx is set in get_request */
				/* process HTTPS connection */
				init_connection(conn);
				conn->connection_type = CONNECTION_TYPE_REQUEST;
				conn->protocol_type = PROTOCOL_TYPE_HTTP1;
				process_new_connection(conn);
			} else {
				/* make sure the connection is cleaned up on SSL failure */
				close_connection(conn);
			}

#elif defined(USE_GNUTLS)
			/* HTTPS connection */
			if (gtls_ssl_accept(&(conn->ssl),
			                    conn->dom_ctx->ssl_ctx,
			                    conn->client.sock,
			                    conn->phys_ctx)
			    == 0) {
				/* conn->dom_ctx is set in get_request */
				/* process HTTPS connection */
				init_connection(conn);
				conn->connection_type = CONNECTION_TYPE_REQUEST;
				conn->protocol_type = PROTOCOL_TYPE_HTTP1;
				process_new_connection(conn);
			} else {
				/* make sure the connection is cleaned up on SSL failure */
				close_connection(conn);
			}

#elif !defined(NO_SSL)
			/* HTTPS connection */
			if (sslize(conn, SSL_accept, NULL)) {
				/* conn->dom_ctx is set in get_request */

				/* Get SSL client certificate information (if set) */
				struct mg_client_cert client_cert;
				if (ssl_get_client_cert_info(conn, &client_cert)) {
					conn->request_info.client_cert = &client_cert;
				}

				/* process HTTPS connection */
#if defined(USE_HTTP2)
				if ((tls.alpn_proto != NULL)
				    && (!memcmp(tls.alpn_proto, "\x02h2", 3))) {
					/* process HTTPS/2 connection */
					init_connection(conn);
					conn->connection_type = CONNECTION_TYPE_REQUEST;
					conn->protocol_type = PROTOCOL_TYPE_HTTP2;
					conn->content_len =
					    -1;               /* content length is not predefined */
					conn->is_chunked = 0; /* HTTP2 is never chunked */
					process_new_http2_connection(conn);
				} else
#endif
				{
					/* process HTTPS/1.x or WEBSOCKET-SECURE connection */
					init_connection(conn);
					conn->connection_type = CONNECTION_TYPE_REQUEST;
					/* Start with HTTP, WS will be an "upgrade" request later */
					conn->protocol_type = PROTOCOL_TYPE_HTTP1;
					process_new_connection(conn);
				}

				/* Free client certificate info */
				if (conn->request_info.client_cert) {
					mg_free((void *)(conn->request_info.client_cert->subject));
					mg_free((void *)(conn->request_info.client_cert->issuer));
					mg_free((void *)(conn->request_info.client_cert->serial));
					mg_free((void *)(conn->request_info.client_cert->finger));
					/* Free certificate memory */
					X509_free(
					    (X509 *)conn->request_info.client_cert->peer_cert);
					conn->request_info.client_cert->peer_cert = 0;
					conn->request_info.client_cert->subject = 0;
					conn->request_info.client_cert->issuer = 0;
					conn->request_info.client_cert->serial = 0;
					conn->request_info.client_cert->finger = 0;
					conn->request_info.client_cert = 0;
				}
			} else {
				/* make sure the connection is cleaned up on SSL failure */
				close_connection(conn);
			}
#endif

		} else {
			/* process HTTP connection */
			init_connection(conn);
			conn->connection_type = CONNECTION_TYPE_REQUEST;
			/* Start with HTTP, WS will be an "upgrade" request later */
			conn->protocol_type = PROTOCOL_TYPE_HTTP1;
			process_new_connection(conn);
		}

		DEBUG_TRACE("%s", "Connection closed");

#if defined(USE_SERVER_STATS)
		conn->conn_close_time = time(NULL);
#endif
	}

	/* Call exit thread user callback */
	if (ctx->callbacks.exit_thread) {
		ctx->callbacks.exit_thread(ctx, 1, tls.user_ptr);
	}

	/* delete thread local storage objects */
	pthread_setspecific(sTlsKey, NULL);
#if defined(_WIN32)
	CloseHandle(tls.pthread_cond_helper_mutex);
#endif
	pthread_mutex_destroy(&conn->mutex);

	/* Free the request buffer. */
	conn->buf_size = 0;
	mg_free(conn->buf);
	conn->buf = NULL;

	/* Free cleaned URI (if any) */
	if (conn->request_info.local_uri != conn->request_info.local_uri_raw) {
		mg_free((void *)conn->request_info.local_uri);
		conn->request_info.local_uri = NULL;
	}

#if defined(USE_SERVER_STATS)
	conn->conn_state = 9; /* done */
#endif

	DEBUG_TRACE("%s", "exiting");
}


/* Threads have different return types on Windows and Unix. */
#if defined(_WIN32)
static unsigned __stdcall worker_thread(void *thread_func_param)
{
	worker_thread_run((struct mg_connection *)thread_func_param);
	return 0;
}
#else
static void *
worker_thread(void *thread_func_param)
{
#if !defined(__ZEPHYR__)
	struct sigaction sa;

	/* Ignore SIGPIPE */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
#endif

	worker_thread_run((struct mg_connection *)thread_func_param);
	return NULL;
}
#endif /* _WIN32 */


/* This is an internal function, thus all arguments are expected to be
 * valid - a NULL check is not required. */
static void
accept_new_connection(const struct socket *listener, struct mg_context *ctx)
{
	struct socket so;
	char src_addr[IP_ADDR_STR_LEN];
	socklen_t len = sizeof(so.rsa);
#if !defined(__ZEPHYR__)
	int on = 1;
#endif
	memset(&so, 0, sizeof(so));

	if ((so.sock = accept(listener->sock, &so.rsa.sa, &len))
	    == INVALID_SOCKET) {
	} else if (check_acl(ctx, &so.rsa) != 1) {
		sockaddr_to_string(src_addr, sizeof(src_addr), &so.rsa);
		mg_cry_ctx_internal(ctx,
		                    "%s: %s is not allowed to connect",
		                    __func__,
		                    src_addr);
		closesocket(so.sock);
	} else {
		/* Put so socket structure into the queue */
		DEBUG_TRACE("Accepted socket %d", (int)so.sock);
		set_close_on_exec(so.sock, NULL, ctx);
		so.is_ssl = listener->is_ssl;
		so.ssl_redir = listener->ssl_redir;
		so.is_optional = listener->is_optional;
		if (getsockname(so.sock, &so.lsa.sa, &len) != 0) {
			mg_cry_ctx_internal(ctx,
			                    "%s: getsockname() failed: %s",
			                    __func__,
			                    strerror(ERRNO));
		}

#if !defined(__ZEPHYR__)
		if ((so.lsa.sa.sa_family == AF_INET)
		    || (so.lsa.sa.sa_family == AF_INET6)) {
			/* Set TCP keep-alive for TCP sockets (IPv4 and IPv6).
			 * This is needed because if HTTP-level keep-alive
			 * is enabled, and client resets the connection, server won't get
			 * TCP FIN or RST and will keep the connection open forever. With
			 * TCP keep-alive, next keep-alive handshake will figure out that
			 * the client is down and will close the server end.
			 * Thanks to Igor Klopov who suggested the patch. */
			if (setsockopt(so.sock,
			               SOL_SOCKET,
			               SO_KEEPALIVE,
			               (SOCK_OPT_TYPE)&on,
			               sizeof(on))
			    != 0) {
				mg_cry_ctx_internal(
				    ctx,
				    "%s: setsockopt(SOL_SOCKET SO_KEEPALIVE) failed: %s",
				    __func__,
				    strerror(ERRNO));
			}
		}
#endif

		/* Disable TCP Nagle's algorithm. Normally TCP packets are coalesced
		 * to effectively fill up the underlying IP packet payload and
		 * reduce the overhead of sending lots of small buffers. However
		 * this hurts the server's throughput (ie. operations per second)
		 * when HTTP 1.1 persistent connections are used and the responses
		 * are relatively small (eg. less than 1400 bytes).
		 */
		if ((ctx->dd.config[CONFIG_TCP_NODELAY] != NULL)
		    && (!strcmp(ctx->dd.config[CONFIG_TCP_NODELAY], "1"))) {
			if (set_tcp_nodelay(&so, 1) != 0) {
				mg_cry_ctx_internal(
				    ctx,
				    "%s: setsockopt(IPPROTO_TCP TCP_NODELAY) failed: %s",
				    __func__,
				    strerror(ERRNO));
			}
		}

		/* The "non blocking" property should already be
		 * inherited from the parent socket. Set it for
		 * non-compliant socket implementations. */
		set_non_blocking_mode(so.sock);

		so.in_use = 0;
		produce_socket(ctx, &so);
	}
}


static void
master_thread_run(struct mg_context *ctx)
{
	struct mg_workerTLS tls;
	struct mg_pollfd *pfd;
	unsigned int i;
	unsigned int workerthreadcount;

	if (!ctx) {
		return;
	}

	mg_set_thread_name("master");

	/* Increase priority of the master thread */
#if defined(_WIN32)
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
#elif defined(USE_MASTER_THREAD_PRIORITY)
	int min_prio = sched_get_priority_min(SCHED_RR);
	int max_prio = sched_get_priority_max(SCHED_RR);
	if ((min_prio >= 0) && (max_prio >= 0)
	    && ((USE_MASTER_THREAD_PRIORITY) <= max_prio)
	    && ((USE_MASTER_THREAD_PRIORITY) >= min_prio)) {
		struct sched_param sched_param = {0};
		sched_param.sched_priority = (USE_MASTER_THREAD_PRIORITY);
		pthread_setschedparam(pthread_self(), SCHED_RR, &sched_param);
	}
#endif

	/* Initialize thread local storage */
#if defined(_WIN32)
	tls.pthread_cond_helper_mutex = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
	tls.is_master = 1;
	pthread_setspecific(sTlsKey, &tls);

	if (ctx->callbacks.init_thread) {
		/* Callback for the master thread (type 0) */
		tls.user_ptr = ctx->callbacks.init_thread(ctx, 0);
	} else {
		tls.user_ptr = NULL;
	}

	/* Lua background script "start" event */
#if defined(USE_LUA)
	if (ctx->lua_background_state) {
		lua_State *lstate = (lua_State *)ctx->lua_background_state;
		pthread_mutex_lock(&ctx->lua_bg_mutex);

		/* call "start()" in Lua */
		lua_getglobal(lstate, "start");
		if (lua_type(lstate, -1) == LUA_TFUNCTION) {
			int ret = lua_pcall(lstate, /* args */ 0, /* results */ 0, 0);
			if (ret != 0) {
				struct mg_connection fc;
				lua_cry(fake_connection(&fc, ctx),
				        ret,
				        lstate,
				        "lua_background_script",
				        "start");
			}
		} else {
			lua_pop(lstate, 1);
		}

		/* determine if there is a "log()" function in Lua background script */
		lua_getglobal(lstate, "log");
		if (lua_type(lstate, -1) == LUA_TFUNCTION) {
			ctx->lua_bg_log_available = 1;
		}
		lua_pop(lstate, 1);

		pthread_mutex_unlock(&ctx->lua_bg_mutex);
	}
#endif

	/* Server starts *now* */
	ctx->start_time = time(NULL);

	/* Server accept loop */
	pfd = ctx->listening_socket_fds;
	while (STOP_FLAG_IS_ZERO(&ctx->stop_flag)) {
		for (i = 0; i < ctx->num_listening_sockets; i++) {
			pfd[i].fd = ctx->listening_sockets[i].sock;
			pfd[i].events = POLLIN;
		}

		/* We listen on this socket just so that mg_stop() can cause mg_poll()
		 * to return ASAP. Don't worry, we did allocate an extra slot at the end
		 * of listening_socket_fds[] just to hold this
		 */
		pfd[ctx->num_listening_sockets].fd =
		    ctx->thread_shutdown_notification_socket;
		pfd[ctx->num_listening_sockets].events = POLLIN;

		if (mg_poll(pfd,
		            ctx->num_listening_sockets
		                + 1, // +1 for the thread_shutdown_notification_socket
		            SOCKET_TIMEOUT_QUANTUM,
		            &(ctx->stop_flag))
		    > 0) {
			for (i = 0; i < ctx->num_listening_sockets; i++) {
				/* NOTE(lsm): on QNX, poll() returns POLLRDNORM after the
				 * successful poll, and POLLIN is defined as
				 * (POLLRDNORM | POLLRDBAND)
				 * Therefore, we're checking pfd[i].revents & POLLIN, not
				 * pfd[i].revents == POLLIN. */
				if (STOP_FLAG_IS_ZERO(&ctx->stop_flag)
				    && (pfd[i].revents & POLLIN)) {
					accept_new_connection(&ctx->listening_sockets[i], ctx);
				}
			}
		}
	}

	/* Here stop_flag is 1 - Initiate shutdown. */
	DEBUG_TRACE("%s", "stopping workers");

	/* Stop signal received: somebody called mg_stop. Quit. */
	close_all_listening_sockets(ctx);

	/* Wakeup workers that are waiting for connections to handle. */
#if defined(ALTERNATIVE_QUEUE)
	for (i = 0; i < ctx->spawned_worker_threads; i++) {
		event_signal(ctx->client_wait_events[i]);
	}
#else
	(void)pthread_mutex_lock(&ctx->thread_mutex);
	pthread_cond_broadcast(&ctx->sq_full);
	(void)pthread_mutex_unlock(&ctx->thread_mutex);
#endif

	/* Join all worker threads to avoid leaking threads. */
	workerthreadcount = ctx->spawned_worker_threads;
	for (i = 0; i < workerthreadcount; i++) {
		if (ctx->worker_threadids[i] != 0) {
			mg_join_thread(ctx->worker_threadids[i]);
		}
	}

#if defined(USE_LUA)
	/* Free Lua state of lua background task */
	if (ctx->lua_background_state) {
		lua_State *lstate = (lua_State *)ctx->lua_background_state;
		ctx->lua_bg_log_available = 0;

		/* call "stop()" in Lua */
		pthread_mutex_lock(&ctx->lua_bg_mutex);
		lua_getglobal(lstate, "stop");
		if (lua_type(lstate, -1) == LUA_TFUNCTION) {
			int ret = lua_pcall(lstate, /* args */ 0, /* results */ 0, 0);
			if (ret != 0) {
				struct mg_connection fc;
				lua_cry(fake_connection(&fc, ctx),
				        ret,
				        lstate,
				        "lua_background_script",
				        "stop");
			}
		}
		DEBUG_TRACE("Close Lua background state %p", lstate);
		lua_close(lstate);

		ctx->lua_background_state = 0;
		pthread_mutex_unlock(&ctx->lua_bg_mutex);
	}
#endif

	DEBUG_TRACE("%s", "exiting");

	/* call exit thread callback */
	if (ctx->callbacks.exit_thread) {
		/* Callback for the master thread (type 0) */
		ctx->callbacks.exit_thread(ctx, 0, tls.user_ptr);
	}

#if defined(_WIN32)
	CloseHandle(tls.pthread_cond_helper_mutex);
#endif
	pthread_setspecific(sTlsKey, NULL);

	/* Signal mg_stop() that we're done.
	 * WARNING: This must be the very last thing this
	 * thread does, as ctx becomes invalid after this line. */
	STOP_FLAG_ASSIGN(&ctx->stop_flag, 2);
}


/* Threads have different return types on Windows and Unix. */
#if defined(_WIN32)
static unsigned __stdcall master_thread(void *thread_func_param)
{
	master_thread_run((struct mg_context *)thread_func_param);
	return 0;
}
#else
static void *
master_thread(void *thread_func_param)
{
#if !defined(__ZEPHYR__)
	struct sigaction sa;

	/* Ignore SIGPIPE */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
#endif

	master_thread_run((struct mg_context *)thread_func_param);
	return NULL;
}
#endif /* _WIN32 */


static void
free_context(struct mg_context *ctx)
{
	int i;
	struct mg_handler_info *tmp_rh;

	if (ctx == NULL) {
		return;
	}

	/* Call user callback */
	if (ctx->callbacks.exit_context) {
		ctx->callbacks.exit_context(ctx);
	}

	/* All threads exited, no sync is needed. Destroy thread mutex and
	 * condvars
	 */
	(void)pthread_mutex_destroy(&ctx->thread_mutex);

#if defined(ALTERNATIVE_QUEUE)
	mg_free(ctx->client_socks);
	if (ctx->client_wait_events != NULL) {
		for (i = 0; (unsigned)i < ctx->spawned_worker_threads; i++) {
			event_destroy(ctx->client_wait_events[i]);
		}
		mg_free(ctx->client_wait_events);
	}
#else
	(void)pthread_cond_destroy(&ctx->sq_empty);
	(void)pthread_cond_destroy(&ctx->sq_full);
	mg_free(ctx->squeue);
#endif

	/* Destroy other context global data structures mutex */
	(void)pthread_mutex_destroy(&ctx->nonce_mutex);

#if defined(USE_LUA)
	(void)pthread_mutex_destroy(&ctx->lua_bg_mutex);
#endif

	/* Deallocate shutdown-triggering socket-pair */
	if (ctx->user_shutdown_notification_socket >= 0) {
		closesocket(ctx->user_shutdown_notification_socket);
	}
	if (ctx->thread_shutdown_notification_socket >= 0) {
		closesocket(ctx->thread_shutdown_notification_socket);
	}

	/* Deallocate config parameters */
	for (i = 0; i < NUM_OPTIONS; i++) {
		if (ctx->dd.config[i] != NULL) {
#if defined(_MSC_VER)
#pragma warning(suppress : 6001)
#endif
			mg_free(ctx->dd.config[i]);
		}
	}

	/* Deallocate request handlers */
	while (ctx->dd.handlers) {
		tmp_rh = ctx->dd.handlers;
		ctx->dd.handlers = tmp_rh->next;
		mg_free(tmp_rh->uri);
		mg_free(tmp_rh);
	}

#if defined(USE_MBEDTLS)
	if (ctx->dd.ssl_ctx != NULL) {
		mbed_sslctx_uninit(ctx->dd.ssl_ctx);
		mg_free(ctx->dd.ssl_ctx);
		ctx->dd.ssl_ctx = NULL;
	}

#elif defined(USE_GNUTLS)
	if (ctx->dd.ssl_ctx != NULL) {
		gtls_sslctx_uninit(ctx->dd.ssl_ctx);
		mg_free(ctx->dd.ssl_ctx);
		ctx->dd.ssl_ctx = NULL;
	}

#elif !defined(NO_SSL)
	/* Deallocate SSL context */
	if (ctx->dd.ssl_ctx != NULL) {
		void *ssl_ctx = (void *)ctx->dd.ssl_ctx;
		int callback_ret =
		    (ctx->callbacks.external_ssl_ctx == NULL)
		        ? 0
		        : (ctx->callbacks.external_ssl_ctx(&ssl_ctx, ctx->user_data));

		if (callback_ret == 0) {
			SSL_CTX_free(ctx->dd.ssl_ctx);
		}
		/* else: ignore error and omit SSL_CTX_free in case
		 * callback_ret is 1 */
	}
#endif /* !NO_SSL */

	/* Deallocate worker thread ID array */
	mg_free(ctx->worker_threadids);

	/* Deallocate worker thread ID array */
	mg_free(ctx->worker_connections);

	/* deallocate system name string */
	mg_free(ctx->systemName);

	/* Deallocate context itself */
	mg_free(ctx);
}


CIVETWEB_API void
mg_stop(struct mg_context *ctx)
{
	pthread_t mt;
	if (!ctx) {
		return;
	}

	/* We don't use a lock here. Calling mg_stop with the same ctx from
	 * two threads is not allowed. */
	mt = ctx->masterthreadid;
	if (mt == 0) {
		return;
	}

	ctx->masterthreadid = 0;

	/* Set stop flag, so all threads know they have to exit. */
	STOP_FLAG_ASSIGN(&ctx->stop_flag, 1);

	/* Closing this socket will cause mg_poll() in all the I/O threads to return
	 * immediately */
	closesocket(ctx->user_shutdown_notification_socket);
	ctx->user_shutdown_notification_socket =
	    -1; /* to avoid calling closesocket() again in free_context() */

	/* Join timer thread */
#if defined(USE_TIMERS)
	timers_exit(ctx);
#endif

	/* Wait until everything has stopped. */
	while (!STOP_FLAG_IS_TWO(&ctx->stop_flag)) {
		(void)mg_sleep(10);
	}

	/* Wait to stop master thread */
	mg_join_thread(mt);

	/* Close remaining Lua states */
#if defined(USE_LUA)
	lua_ctx_exit(ctx);
#endif

	/* Free memory */
	free_context(ctx);
}


static void
get_system_name(char **sysName)
{
#if defined(_WIN32)
	char name[128];
	DWORD dwVersion = 0;
	DWORD dwMajorVersion = 0;
	DWORD dwMinorVersion = 0;
	DWORD dwBuild = 0;
	BOOL wowRet, isWoW = FALSE;

#if defined(_MSC_VER)
#pragma warning(push)
	/* GetVersion was declared deprecated */
#pragma warning(disable : 4996)
#endif
	dwVersion = GetVersion();
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	dwBuild = ((dwVersion < 0x80000000) ? (DWORD)(HIWORD(dwVersion)) : 0);
	(void)dwBuild;

	wowRet = IsWow64Process(GetCurrentProcess(), &isWoW);

	sprintf(name,
	        "Windows %u.%u%s",
	        (unsigned)dwMajorVersion,
	        (unsigned)dwMinorVersion,
	        (wowRet ? (isWoW ? " (WoW64)" : "") : " (?)"));

	*sysName = mg_strdup(name);

#elif defined(__rtems__)
	*sysName = mg_strdup("RTEMS");
#elif defined(__ZEPHYR__)
	*sysName = mg_strdup("Zephyr OS");
#else
	struct utsname name;
	memset(&name, 0, sizeof(name));
	uname(&name);
	*sysName = mg_strdup(name.sysname);
#endif
}


static void
legacy_init(const char **options)
{
	const char *ports_option = config_options[LISTENING_PORTS].default_value;

	if (options) {
		const char **run_options = options;
		const char *optname = config_options[LISTENING_PORTS].name;

		/* Try to find the "listening_ports" option */
		while (*run_options) {
			if (!strcmp(*run_options, optname)) {
				ports_option = run_options[1];
			}
			run_options += 2;
		}
	}

	if (is_ssl_port_used(ports_option)) {
		/* Initialize with SSL support */
		mg_init_library(MG_FEATURES_TLS);
	} else {
		/* Initialize without SSL support */
		mg_init_library(MG_FEATURES_DEFAULT);
	}
}

/* we'll assume it's only Windows that doesn't have socketpair() available */
#if !defined(HAVE_SOCKETPAIR) && !defined(_WIN32)
#define HAVE_SOCKETPAIR 1
#endif

static int
mg_socketpair(int *sockA, int *sockB)
{
	int temp[2] = {-1, -1};
	int asock = -1;

	/** Default to unallocated */
	*sockA = -1;
	*sockB = -1;

#if defined(HAVE_SOCKETPAIR)
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, temp);
	if (ret == 0) {
		*sockA = temp[0];
		*sockB = temp[1];
		set_close_on_exec(*sockA, NULL, NULL);
		set_close_on_exec(*sockB, NULL, NULL);
	}
	(void)asock; /* not used */
	return ret;
#else
	/** No socketpair() call is available, so we'll have to roll our own
	 * implementation */
	asock = socket(PF_INET, SOCK_STREAM, 0);
	if (asock >= 0) {
		struct sockaddr_in addr;
		struct sockaddr *pa = (struct sockaddr *)&addr;
		socklen_t addrLen = sizeof(addr);

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = 0;

		if ((bind(asock, pa, sizeof(addr)) == 0)
		    && (getsockname(asock, pa, &addrLen) == 0)
		    && (listen(asock, 1) == 0)) {
			temp[0] = socket(PF_INET, SOCK_STREAM, 0);
			if ((temp[0] >= 0) && (connect(temp[0], pa, sizeof(addr)) == 0)) {
				temp[1] = accept(asock, pa, &addrLen);
				if (temp[1] >= 0) {
					closesocket(asock);
					*sockA = temp[0];
					*sockB = temp[1];
					set_close_on_exec(*sockA, NULL, NULL);
					set_close_on_exec(*sockB, NULL, NULL);
					return 0; /* success! */
				}
			}
		}
	}

	/* Cleanup */
	if (asock >= 0)
		closesocket(asock);
	if (temp[0] >= 0)
		closesocket(temp[0]);
	if (temp[1] >= 0)
		closesocket(temp[1]);
	return -1; /* fail! */
#endif
}

static int
mg_start_worker_thread(struct mg_context *ctx, int only_if_no_idle_threads)
{
	const unsigned int i = ctx->spawned_worker_threads;
	if (i >= ctx->cfg_max_worker_threads) {
		return -1; /* Oops, we hit our worker-thread limit!  No more worker
		              threads, ever! */
	}

	(void)pthread_mutex_lock(&ctx->thread_mutex);
#if defined(ALTERNATIVE_QUEUE)
	if ((only_if_no_idle_threads) && (ctx->idle_worker_thread_count > 0)) {
#else
	if ((only_if_no_idle_threads)
	    && (ctx->idle_worker_thread_count
	        > (unsigned)(ctx->sq_head - ctx->sq_tail))) {
#endif
		(void)pthread_mutex_unlock(&ctx->thread_mutex);
		return -2; /* There are idle threads available, so no need to spawn a
		              new worker thread now */
	}
	ctx->idle_worker_thread_count++; /* we do this here to avoid a race
	                                    condition while the thread is starting
	                                    up */
	(void)pthread_mutex_unlock(&ctx->thread_mutex);

	ctx->worker_connections[i].phys_ctx = ctx;
	int ret = mg_start_thread_with_id(worker_thread,
	                                  &ctx->worker_connections[i],
	                                  &ctx->worker_threadids[i]);
	if (ret == 0) {
		ctx->spawned_worker_threads++; /* note that we've filled another slot in
		                                  the table */
		DEBUG_TRACE("Started worker_thread #%i", ctx->spawned_worker_threads);
	} else {
		(void)pthread_mutex_lock(&ctx->thread_mutex);
		ctx->idle_worker_thread_count--; /* whoops, roll-back on error */
		(void)pthread_mutex_unlock(&ctx->thread_mutex);
	}
	return ret;
}

CIVETWEB_API struct mg_context *
mg_start2(struct mg_init_data *init, struct mg_error_data *error)
{
	struct mg_context *ctx;
	const char *name, *value, *default_value;
	int idx, ok, prespawnthreadcount, workerthreadcount;
	unsigned int i;
	int itmp;
	void (*exit_callback)(const struct mg_context *ctx) = 0;
	const char **options =
	    ((init != NULL) ? (init->configuration_options) : (NULL));

	struct mg_workerTLS tls;

	if (error != NULL) {
		error->code = MG_ERROR_DATA_CODE_OK;
		error->code_sub = 0;
		if (error->text_buffer_size > 0) {
			*error->text = 0;
		}
	}

	if (mg_init_library_called == 0) {
		/* Legacy INIT, if mg_start is called without mg_init_library.
		 * Note: This will cause a memory leak when unloading the library.
		 */
		legacy_init(options);
	}
	if (mg_init_library_called == 0) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_LIBRARY_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "Library uninitialized");
		}
		return NULL;
	}

	/* Allocate context and initialize reasonable general case defaults. */
	ctx = (struct mg_context *)mg_calloc(1, sizeof(*ctx));
	if (ctx == NULL) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OUT_OF_MEMORY;
			error->code_sub = (unsigned)sizeof(*ctx);
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "Out of memory");
		}
		return NULL;
	}

	/* Random number generator will initialize at the first call */
	ctx->dd.auth_nonce_mask =
	    (uint64_t)get_random() ^ (uint64_t)(ptrdiff_t)(options);

	/* Save started thread index to reuse in other external API calls
	 * For the sake of thread synchronization all non-civetweb threads
	 * can be considered as single external thread */
	ctx->starter_thread_idx = (unsigned)mg_atomic_inc(&thread_idx_max);
	tls.is_master = -1; /* Thread calling mg_start */
	tls.thread_idx = ctx->starter_thread_idx;
#if defined(_WIN32)
	tls.pthread_cond_helper_mutex = NULL;
#endif
	pthread_setspecific(sTlsKey, &tls);

	ok = (0 == pthread_mutex_init(&ctx->thread_mutex, &pthread_mutex_attr));
#if !defined(ALTERNATIVE_QUEUE)
	ok &= (0 == pthread_cond_init(&ctx->sq_empty, NULL));
	ok &= (0 == pthread_cond_init(&ctx->sq_full, NULL));
	ctx->sq_blocked = 0;
#endif
	ok &= (0 == pthread_mutex_init(&ctx->nonce_mutex, &pthread_mutex_attr));
#if defined(USE_LUA)
	ok &= (0 == pthread_mutex_init(&ctx->lua_bg_mutex, &pthread_mutex_attr));
#endif

	/** mg_stop() will close the user_shutdown_notification_socket, and that
	 * will cause poll() to return immediately in the master-thread, so that
	 * mg_stop() can also return immediately.
	 */
	ok &= (0
	       == mg_socketpair(&ctx->user_shutdown_notification_socket,
	                        &ctx->thread_shutdown_notification_socket));

	if (!ok) {
		unsigned error_id = (unsigned)ERRNO;
		const char *err_msg =
		    "Cannot initialize thread synchronization objects";
		/* Fatal error - abort start. However, this situation should never
		 * occur in practice. */

		mg_cry_ctx_internal(ctx, "%s", err_msg);
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OS_ERROR;
			error->code_sub = error_id;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		mg_free(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}

	if ((init != NULL) && (init->callbacks != NULL)) {
		/* Set all callbacks except exit_context. */
		ctx->callbacks = *init->callbacks;
		exit_callback = init->callbacks->exit_context;
		/* The exit callback is activated once the context is successfully
		 * created. It should not be called, if an incomplete context object
		 * is deleted during a failed initialization. */
		ctx->callbacks.exit_context = 0;
	}
	ctx->user_data = ((init != NULL) ? (init->user_data) : (NULL));
	ctx->dd.handlers = NULL;
	ctx->dd.next = NULL;

#if defined(USE_LUA)
	lua_ctx_init(ctx);
#endif

	/* Store options */
	while (options && (name = *options++) != NULL) {
		idx = get_option_index(name);
		if (idx == -1) {
			mg_cry_ctx_internal(ctx, "Invalid option: %s", name);
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_INVALID_OPTION;
				error->code_sub = (unsigned)-1;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for error buffers */
				            error->text,
				            error->text_buffer_size,
				            "Invalid configuration option: %s",
				            name);
			}

			free_context(ctx);
			pthread_setspecific(sTlsKey, NULL);
			return NULL;

		} else if ((value = *options++) == NULL) {
			mg_cry_ctx_internal(ctx, "%s: option value cannot be NULL", name);
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_INVALID_OPTION;
				error->code_sub = (unsigned)idx;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for error buffers */
				            error->text,
				            error->text_buffer_size,
				            "Invalid configuration option value: %s",
				            name);
			}

			free_context(ctx);
			pthread_setspecific(sTlsKey, NULL);
			return NULL;
		}
		if (ctx->dd.config[idx] != NULL) {
			/* A duplicate configuration option is not an error - the last
			 * option value will be used. */
			mg_cry_ctx_internal(ctx, "warning: %s: duplicate option", name);
			mg_free(ctx->dd.config[idx]);
		}
		ctx->dd.config[idx] = mg_strdup_ctx(value, ctx);
		DEBUG_TRACE("[%s] -> [%s]", name, value);
	}

	/* Set default value if needed */
	for (i = 0; config_options[i].name != NULL; i++) {
		default_value = config_options[i].default_value;
		if ((ctx->dd.config[i] == NULL) && (default_value != NULL)) {
			ctx->dd.config[i] = mg_strdup_ctx(default_value, ctx);
		}
	}

	/* Request size option */
	itmp = atoi(ctx->dd.config[MAX_REQUEST_SIZE]);
	if (itmp < 1024) {
		mg_cry_ctx_internal(ctx,
		                    "%s too small",
		                    config_options[MAX_REQUEST_SIZE].name);
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_OPTION;
			error->code_sub = (unsigned)MAX_REQUEST_SIZE;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "Invalid configuration option value: %s",
			            config_options[MAX_REQUEST_SIZE].name);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
	ctx->max_request_size = (unsigned)itmp;

	/* Queue length */
#if !defined(ALTERNATIVE_QUEUE)
	itmp = atoi(ctx->dd.config[CONNECTION_QUEUE_SIZE]);
	if (itmp < 1) {
		mg_cry_ctx_internal(ctx,
		                    "%s too small",
		                    config_options[CONNECTION_QUEUE_SIZE].name);
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_OPTION;
			error->code_sub = CONNECTION_QUEUE_SIZE;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "Invalid configuration option value: %s",
			            config_options[CONNECTION_QUEUE_SIZE].name);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
	ctx->squeue =
	    (struct socket *)mg_calloc((unsigned int)itmp, sizeof(struct socket));
	if (ctx->squeue == NULL) {
		mg_cry_ctx_internal(ctx,
		                    "Out of memory: Cannot allocate %s",
		                    config_options[CONNECTION_QUEUE_SIZE].name);
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OUT_OF_MEMORY;
			error->code_sub = (unsigned)itmp * (unsigned)sizeof(struct socket);
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "Out of memory: Cannot allocate %s",
			            config_options[CONNECTION_QUEUE_SIZE].name);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
	ctx->sq_size = itmp;
#endif

	/* Worker thread count option */
	workerthreadcount = atoi(ctx->dd.config[NUM_THREADS]);
	prespawnthreadcount = atoi(ctx->dd.config[PRESPAWN_THREADS]);

	if ((prespawnthreadcount < 0)
	    || (prespawnthreadcount > workerthreadcount)) {
		prespawnthreadcount =
		    workerthreadcount; /* can't prespawn more than all of them! */
	}

	if ((workerthreadcount > MAX_WORKER_THREADS) || (workerthreadcount <= 0)) {
		if (workerthreadcount <= 0) {
			mg_cry_ctx_internal(ctx, "%s", "Invalid number of worker threads");
		} else {
			mg_cry_ctx_internal(ctx, "%s", "Too many worker threads");
		}
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_OPTION;
			error->code_sub = NUM_THREADS;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "Invalid configuration option value: %s",
			            config_options[NUM_THREADS].name);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}

	/* Document root */
#if defined(NO_FILES)
	if (ctx->dd.config[DOCUMENT_ROOT] != NULL) {
		mg_cry_ctx_internal(ctx, "%s", "Document root must not be set");
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_OPTION;
			error->code_sub = (unsigned)DOCUMENT_ROOT;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "Invalid configuration option value: %s",
			            config_options[DOCUMENT_ROOT].name);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
#endif

	get_system_name(&ctx->systemName);

#if defined(USE_LUA)
	/* If a Lua background script has been configured, start it. */
	ctx->lua_bg_log_available = 0;
	if (ctx->dd.config[LUA_BACKGROUND_SCRIPT] != NULL) {
		char ebuf[256];
		struct vec opt_vec;
		struct vec eq_vec;
		const char *sparams;

		memset(ebuf, 0, sizeof(ebuf));
		pthread_mutex_lock(&ctx->lua_bg_mutex);

		/* Create a Lua state, load all standard libraries and the mg table */
		lua_State *state = mg_lua_context_script_prepare(
		    ctx->dd.config[LUA_BACKGROUND_SCRIPT], ctx, ebuf, sizeof(ebuf));
		if (!state) {
			mg_cry_ctx_internal(ctx,
			                    "lua_background_script load error: %s",
			                    ebuf);
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_SCRIPT_ERROR;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for error buffers */
				            error->text,
				            error->text_buffer_size,
				            "Error in script %s: %s",
				            config_options[LUA_BACKGROUND_SCRIPT].name,
				            ebuf);
			}

			pthread_mutex_unlock(&ctx->lua_bg_mutex);

			free_context(ctx);
			pthread_setspecific(sTlsKey, NULL);
			return NULL;
		}

		/* Add a table with parameters into mg.params */
		sparams = ctx->dd.config[LUA_BACKGROUND_SCRIPT_PARAMS];
		if (sparams && sparams[0]) {
			lua_getglobal(state, "mg");
			lua_pushstring(state, "params");
			lua_newtable(state);

			while ((sparams = next_option(sparams, &opt_vec, &eq_vec))
			       != NULL) {
				reg_llstring(
				    state, opt_vec.ptr, opt_vec.len, eq_vec.ptr, eq_vec.len);
				if (mg_strncasecmp(sparams, opt_vec.ptr, opt_vec.len) == 0)
					break;
			}
			lua_rawset(state, -3);
			lua_pop(state, 1);
		}

		/* Call script */
		state = mg_lua_context_script_run(state,
		                                  ctx->dd.config[LUA_BACKGROUND_SCRIPT],
		                                  ctx,
		                                  ebuf,
		                                  sizeof(ebuf));
		if (!state) {
			mg_cry_ctx_internal(ctx,
			                    "lua_background_script start error: %s",
			                    ebuf);
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_SCRIPT_ERROR;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for error buffers */
				            error->text,
				            error->text_buffer_size,
				            "Error in script %s: %s",
				            config_options[DOCUMENT_ROOT].name,
				            ebuf);
			}
			pthread_mutex_unlock(&ctx->lua_bg_mutex);

			free_context(ctx);
			pthread_setspecific(sTlsKey, NULL);
			return NULL;
		}

		/* state remains valid */
		ctx->lua_background_state = (void *)state;
		pthread_mutex_unlock(&ctx->lua_bg_mutex);

	} else {
		ctx->lua_background_state = 0;
	}
#endif

	/* Step by step initialization of ctx - depending on build options */
#if !defined(NO_FILESYSTEMS)
	if (!set_gpass_option(ctx, NULL)) {
		const char *err_msg = "Invalid global password file";
		/* Fatal error - abort start. */
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_PASS_FILE;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}
		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
#endif

#if defined(USE_MBEDTLS) || defined(USE_GNUTLS)
	if (!mg_sslctx_init(ctx, NULL)) {
		const char *err_msg = "Error initializing SSL context";
		/* Fatal error - abort start. */
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_TLS_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}

#elif !defined(NO_SSL)
	if (!init_ssl_ctx(ctx, NULL)) {
		const char *err_msg = "Error initializing SSL context";
		/* Fatal error - abort start. */
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_TLS_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
#endif

	if (!set_ports_option(ctx)) {
		const char *err_msg = "Failed to setup server ports";
		/* Fatal error - abort start. */
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_PORTS_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}

#if !defined(_WIN32) && !defined(__ZEPHYR__)
	if (!set_uid_option(ctx)) {
		const char *err_msg = "Failed to run as configured user";
		/* Fatal error - abort start. */
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_USER_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
#endif

	if (!set_acl_option(ctx)) {
		const char *err_msg = "Failed to setup access control list";
		/* Fatal error - abort start. */
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_ACL_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}

	ctx->cfg_max_worker_threads = ((unsigned int)(workerthreadcount));
	ctx->worker_threadids =
	    (pthread_t *)mg_calloc_ctx(ctx->cfg_max_worker_threads,
	                               sizeof(pthread_t),
	                               ctx);

	if (ctx->worker_threadids == NULL) {
		const char *err_msg = "Not enough memory for worker thread ID array";
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OUT_OF_MEMORY;
			error->code_sub = (unsigned)ctx->cfg_max_worker_threads
			                  * (unsigned)sizeof(pthread_t);
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
	ctx->worker_connections =
	    (struct mg_connection *)mg_calloc_ctx(ctx->cfg_max_worker_threads,
	                                          sizeof(struct mg_connection),
	                                          ctx);
	if (ctx->worker_connections == NULL) {
		const char *err_msg =
		    "Not enough memory for worker thread connection array";
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OUT_OF_MEMORY;
			error->code_sub = (unsigned)ctx->cfg_max_worker_threads
			                  * (unsigned)sizeof(struct mg_connection);
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}

#if defined(ALTERNATIVE_QUEUE)
	ctx->client_wait_events =
	    (void **)mg_calloc_ctx(ctx->cfg_max_worker_threads,
	                           sizeof(ctx->client_wait_events[0]),
	                           ctx);
	if (ctx->client_wait_events == NULL) {
		const char *err_msg = "Not enough memory for worker event array";
		mg_cry_ctx_internal(ctx, "%s", err_msg);
		mg_free(ctx->worker_threadids);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OUT_OF_MEMORY;
			error->code_sub = (unsigned)ctx->cfg_max_worker_threads
			                  * (unsigned)sizeof(ctx->client_wait_events[0]);
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}

	ctx->client_socks =
	    (struct socket *)mg_calloc_ctx(ctx->cfg_max_worker_threads,
	                                   sizeof(ctx->client_socks[0]),
	                                   ctx);
	if (ctx->client_socks == NULL) {
		const char *err_msg = "Not enough memory for worker socket array";
		mg_cry_ctx_internal(ctx, "%s", err_msg);
		mg_free(ctx->client_wait_events);
		mg_free(ctx->worker_threadids);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OUT_OF_MEMORY;
			error->code_sub = (unsigned)ctx->cfg_max_worker_threads
			                  * (unsigned)sizeof(ctx->client_socks[0]);
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}

	for (i = 0; (unsigned)i < ctx->cfg_max_worker_threads; i++) {
		ctx->client_wait_events[i] = event_create();
		if (ctx->client_wait_events[i] == 0) {
			const char *err_msg = "Error creating worker event %i";
			mg_cry_ctx_internal(ctx, err_msg, i);
			while (i > 0) {
				i--;
				event_destroy(ctx->client_wait_events[i]);
			}
			mg_free(ctx->client_socks);
			mg_free(ctx->client_wait_events);
			mg_free(ctx->worker_threadids);

			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_OS_ERROR;
				error->code_sub = (unsigned)ERRNO;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for error buffers */
				            error->text,
				            error->text_buffer_size,
				            err_msg,
				            i);
			}

			free_context(ctx);
			pthread_setspecific(sTlsKey, NULL);
			return NULL;
		}
	}
#endif

#if defined(USE_TIMERS)
	if (timers_init(ctx) != 0) {
		const char *err_msg = "Error creating timers";
		mg_cry_ctx_internal(ctx, "%s", err_msg);

		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OS_ERROR;
			error->code_sub = (unsigned)ERRNO;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            err_msg);
		}

		free_context(ctx);
		pthread_setspecific(sTlsKey, NULL);
		return NULL;
	}
#endif

	/* Context has been created - init user libraries */
	if (ctx->callbacks.init_context) {
		ctx->callbacks.init_context(ctx);
	}

	/* From now, the context is successfully created.
	 * When it is destroyed, the exit callback should be called. */
	ctx->callbacks.exit_context = exit_callback;
	ctx->context_type = CONTEXT_SERVER; /* server context */

	/* Start worker threads */
	for (i = 0; (int)i < prespawnthreadcount; i++) {
		/* worker_thread sets up the other fields */
		if (mg_start_worker_thread(ctx, 0) != 0) {
			long error_no = (long)ERRNO;

			/* thread was not created */
			if (ctx->spawned_worker_threads > 0) {
				/* If the second, third, ... thread cannot be created, set a
				 * warning, but keep running. */
				mg_cry_ctx_internal(ctx,
				                    "Cannot start worker thread %i: error %ld",
				                    ctx->spawned_worker_threads + 1,
				                    error_no);

				/* If the server initialization should stop here, all
				 * threads that have already been created must be stopped
				 * first, before any free_context(ctx) call.
				 */

			} else {
				/* If the first worker thread cannot be created, stop
				 * initialization and free the entire server context. */
				mg_cry_ctx_internal(ctx,
				                    "Cannot create threads: error %ld",
				                    error_no);

				if (error != NULL) {
					error->code = MG_ERROR_DATA_CODE_OS_ERROR;
					error->code_sub = (unsigned)error_no;
					mg_snprintf(
					    NULL,
					    NULL, /* No truncation check for error buffers */
					    error->text,
					    error->text_buffer_size,
					    "Cannot create first worker thread: error %ld",
					    error_no);
				}

				free_context(ctx);
				pthread_setspecific(sTlsKey, NULL);
				return NULL;
			}
			break;
		}
	}

	/* Start master (listening) thread */
	mg_start_thread_with_id(master_thread, ctx, &ctx->masterthreadid);

	pthread_setspecific(sTlsKey, NULL);
	return ctx;
}


CIVETWEB_API struct mg_context *
mg_start(const struct mg_callbacks *callbacks,
         void *user_data,
         const char **options)
{
	struct mg_init_data init = {0};
	init.callbacks = callbacks;
	init.user_data = user_data;
	init.configuration_options = options;

	return mg_start2(&init, NULL);
}


/* Add an additional domain to an already running web server. */
CIVETWEB_API int
mg_start_domain2(struct mg_context *ctx,
                 const char **options,
                 struct mg_error_data *error)
{
	const char *name;
	const char *value;
	const char *default_value;
	struct mg_domain_context *new_dom;
	struct mg_domain_context *dom;
	int idx, i;

	if (error != NULL) {
		error->code = MG_ERROR_DATA_CODE_OK;
		error->code_sub = 0;
		if (error->text_buffer_size > 0) {
			*error->text = 0;
		}
	}

	if ((ctx == NULL) || (options == NULL)) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INVALID_PARAM;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "Invalid parameters");
		}
		return -1;
	}

	if (!STOP_FLAG_IS_ZERO(&ctx->stop_flag)) {
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_SERVER_STOPPED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "Server already stopped");
		}
		return -7;
	}

	new_dom = (struct mg_domain_context *)
	    mg_calloc_ctx(1, sizeof(struct mg_domain_context), ctx);

	if (!new_dom) {
		/* Out of memory */
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_OUT_OF_MEMORY;
			error->code_sub = (unsigned)sizeof(struct mg_domain_context);
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "Out or memory");
		}
		return -6;
	}

	/* Store options - TODO: unite duplicate code */
	while (options && (name = *options++) != NULL) {
		idx = get_option_index(name);
		if (idx == -1) {
			mg_cry_ctx_internal(ctx, "Invalid option: %s", name);
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_INVALID_OPTION;
				error->code_sub = (unsigned)-1;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for error buffers */
				            error->text,
				            error->text_buffer_size,
				            "Invalid option: %s",
				            name);
			}
			mg_free(new_dom);
			return -2;
		} else if ((value = *options++) == NULL) {
			mg_cry_ctx_internal(ctx, "%s: option value cannot be NULL", name);
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_INVALID_OPTION;
				error->code_sub = (unsigned)idx;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for error buffers */
				            error->text,
				            error->text_buffer_size,
				            "Invalid option value: %s",
				            name);
			}
			mg_free(new_dom);
			return -2;
		}
		if (new_dom->config[idx] != NULL) {
			/* Duplicate option: Later values overwrite earlier ones. */
			mg_cry_ctx_internal(ctx, "warning: %s: duplicate option", name);
			mg_free(new_dom->config[idx]);
		}
		new_dom->config[idx] = mg_strdup_ctx(value, ctx);
		DEBUG_TRACE("[%s] -> [%s]", name, value);
	}

	/* Authentication domain is mandatory */
	/* TODO: Maybe use a new option hostname? */
	if (!new_dom->config[AUTHENTICATION_DOMAIN]) {
		mg_cry_ctx_internal(ctx, "%s", "authentication domain required");
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_MISSING_OPTION;
			error->code_sub = AUTHENTICATION_DOMAIN;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "Mandatory option %s missing",
			            config_options[AUTHENTICATION_DOMAIN].name);
		}
		mg_free(new_dom);
		return -4;
	}

	/* Set default value if needed. Take the config value from
	 * ctx as a default value. */
	for (i = 0; config_options[i].name != NULL; i++) {
		default_value = ctx->dd.config[i];
		if ((new_dom->config[i] == NULL) && (default_value != NULL)) {
			new_dom->config[i] = mg_strdup_ctx(default_value, ctx);
		}
	}

	new_dom->handlers = NULL;
	new_dom->next = NULL;
	new_dom->nonce_count = 0;
	new_dom->auth_nonce_mask = get_random() ^ (get_random() << 31);

#if defined(USE_LUA) && defined(USE_WEBSOCKET)
	new_dom->shared_lua_websockets = NULL;
#endif

#if !defined(NO_SSL) && !defined(USE_MBEDTLS) && !defined(USE_GNUTLS)
	if (!init_ssl_ctx(ctx, new_dom)) {
		/* Init SSL failed */
		if (error != NULL) {
			error->code = MG_ERROR_DATA_CODE_INIT_TLS_FAILED;
			mg_snprintf(NULL,
			            NULL, /* No truncation check for error buffers */
			            error->text,
			            error->text_buffer_size,
			            "%s",
			            "Initializing SSL context failed");
		}
		mg_free(new_dom);
		return -3;
	}
#endif

	/* Add element to linked list. */
	mg_lock_context(ctx);

	idx = 0;
	dom = &(ctx->dd);
	for (;;) {
		if (!mg_strcasecmp(new_dom->config[AUTHENTICATION_DOMAIN],
		                   dom->config[AUTHENTICATION_DOMAIN])) {
			/* Domain collision */
			mg_cry_ctx_internal(ctx,
			                    "domain %s already in use",
			                    new_dom->config[AUTHENTICATION_DOMAIN]);
			if (error != NULL) {
				error->code = MG_ERROR_DATA_CODE_DUPLICATE_DOMAIN;
				mg_snprintf(NULL,
				            NULL, /* No truncation check for error buffers */
				            error->text,
				            error->text_buffer_size,
				            "Domain %s specified by %s is already in use",
				            new_dom->config[AUTHENTICATION_DOMAIN],
				            config_options[AUTHENTICATION_DOMAIN].name);
			}
			mg_free(new_dom);
			mg_unlock_context(ctx);
			return -5;
		}

		/* Count number of domains */
		idx++;

		if (dom->next == NULL) {
			dom->next = new_dom;
			break;
		}
		dom = dom->next;
	}

	mg_unlock_context(ctx);

	/* Return domain number */
	return idx;
}


CIVETWEB_API int
mg_start_domain(struct mg_context *ctx, const char **options)
{
	return mg_start_domain2(ctx, options, NULL);
}


/* Feature check API function */
CIVETWEB_API unsigned
mg_check_feature(unsigned feature)
{
	static const unsigned feature_set = 0
	/* Set bits for available features according to API documentation.
	 * This bit mask is created at compile time, according to the active
	 * preprocessor defines. It is a single const value at runtime. */
#if !defined(NO_FILES)
	                                    | MG_FEATURES_FILES
#endif
#if !defined(NO_SSL) || defined(USE_MBEDTLS) || defined(USE_GNUTLS)
	                                    | MG_FEATURES_SSL
#endif
#if !defined(NO_CGI)
	                                    | MG_FEATURES_CGI
#endif
#if defined(USE_IPV6)
	                                    | MG_FEATURES_IPV6
#endif
#if defined(USE_WEBSOCKET)
	                                    | MG_FEATURES_WEBSOCKET
#endif
#if defined(USE_LUA)
	                                    | MG_FEATURES_LUA
#endif
#if defined(USE_DUKTAPE)
	                                    | MG_FEATURES_SSJS
#endif
#if !defined(NO_CACHING)
	                                    | MG_FEATURES_CACHE
#endif
#if defined(USE_SERVER_STATS)
	                                    | MG_FEATURES_STATS
#endif
#if defined(USE_ZLIB)
	                                    | MG_FEATURES_COMPRESSION
#endif
#if defined(USE_HTTP2)
	                                    | MG_FEATURES_HTTP2
#endif
#if defined(USE_X_DOM_SOCKET)
	                                    | MG_FEATURES_X_DOMAIN_SOCKET
#endif

	/* Set some extra bits not defined in the API documentation.
	 * These bits may change without further notice. */
#if defined(MG_LEGACY_INTERFACE)
	                                    | 0x80000000u
#endif
#if defined(MG_EXPERIMENTAL_INTERFACES)
	                                    | 0x40000000u
#endif
#if !defined(NO_RESPONSE_BUFFERING)
	                                    | 0x20000000u
#endif
#if defined(MEMORY_DEBUGGING)
	                                    | 0x10000000u
#endif
	    ;
	return (feature & feature_set);
}


static size_t
mg_str_append(char **dst, char *end, const char *src)
{
	size_t len = strlen(src);
	if (*dst != end) {
		/* Append src if enough space, or close dst. */
		if ((size_t)(end - *dst) > len) {
			strcpy(*dst, src);
			*dst += len;
		} else {
			*dst = end;
		}
	}
	return len;
}


/* Get system information. It can be printed or stored by the caller.
 * Return the size of available information. */
CIVETWEB_API int
mg_get_system_info(char *buffer, int buflen)
{
	char *end, *append_eoobj = NULL, block[256];
	size_t system_info_length = 0;

#if defined(_WIN32)
	static const char eol[] = "\r\n", eoobj[] = "\r\n}\r\n";
#else
	static const char eol[] = "\n", eoobj[] = "\n}\n";
#endif

	if ((buffer == NULL) || (buflen < 1)) {
		buflen = 0;
		end = buffer;
	} else {
		*buffer = 0;
		end = buffer + buflen;
	}
	if (buflen > (int)(sizeof(eoobj) - 1)) {
		/* has enough space to append eoobj */
		append_eoobj = buffer;
		if (end) {
			end -= sizeof(eoobj) - 1;
		}
	}

	system_info_length += mg_str_append(&buffer, end, "{");

	/* Server version */
	{
		const char *version = mg_version();
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            "%s\"version\" : \"%s\"",
		            eol,
		            version);
		system_info_length += mg_str_append(&buffer, end, block);
	}

	/* System info */
	{
#if defined(_WIN32)
		DWORD dwVersion = 0;
		DWORD dwMajorVersion = 0;
		DWORD dwMinorVersion = 0;
		SYSTEM_INFO si;

		GetSystemInfo(&si);

#if defined(_MSC_VER)
#pragma warning(push)
		/* GetVersion was declared deprecated */
#pragma warning(disable : 4996)
#endif
		dwVersion = GetVersion();
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

		dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"os\" : \"Windows %u.%u\"",
		            eol,
		            (unsigned)dwMajorVersion,
		            (unsigned)dwMinorVersion);
		system_info_length += mg_str_append(&buffer, end, block);

		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"cpu\" : \"type %u, cores %u, mask %x\"",
		            eol,
		            (unsigned)si.wProcessorArchitecture,
		            (unsigned)si.dwNumberOfProcessors,
		            (unsigned)si.dwActiveProcessorMask);
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__rtems__)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"os\" : \"%s %s\"",
		            eol,
		           "RTEMS",
		            rtems_version());
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__ZEPHYR__)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"os\" : \"%s\"",
		            eol,
		            "Zephyr OS",
		            ZEPHYR_VERSION);
		system_info_length += mg_str_append(&buffer, end, block);
#else
		struct utsname name;
		memset(&name, 0, sizeof(name));
		uname(&name);

		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"os\" : \"%s %s (%s) - %s\"",
		            eol,
		            name.sysname,
		            name.version,
		            name.release,
		            name.machine);
		system_info_length += mg_str_append(&buffer, end, block);
#endif
	}

	/* Features */
	{
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"features\" : %lu"
		            ",%s\"feature_list\" : \"Server:%s%s%s%s%s%s%s%s%s\"",
		            eol,
		            (unsigned long)mg_check_feature(0xFFFFFFFFu),
		            eol,
		            mg_check_feature(MG_FEATURES_FILES) ? " Files" : "",
		            mg_check_feature(MG_FEATURES_SSL) ? " HTTPS" : "",
		            mg_check_feature(MG_FEATURES_CGI) ? " CGI" : "",
		            mg_check_feature(MG_FEATURES_IPV6) ? " IPv6" : "",
		            mg_check_feature(MG_FEATURES_WEBSOCKET) ? " WebSockets"
		                                                    : "",
		            mg_check_feature(MG_FEATURES_LUA) ? " Lua" : "",
		            mg_check_feature(MG_FEATURES_SSJS) ? " JavaScript" : "",
		            mg_check_feature(MG_FEATURES_CACHE) ? " Cache" : "",
		            mg_check_feature(MG_FEATURES_STATS) ? " Stats" : "");
		system_info_length += mg_str_append(&buffer, end, block);

#if defined(USE_LUA)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"lua_version\" : \"%u (%s)\"",
		            eol,
		            (unsigned)LUA_VERSION_NUM,
		            LUA_RELEASE);
		system_info_length += mg_str_append(&buffer, end, block);
#endif
#if defined(USE_DUKTAPE)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"javascript\" : \"Duktape %u.%u.%u\"",
		            eol,
		            (unsigned)DUK_VERSION / 10000,
		            ((unsigned)DUK_VERSION / 100) % 100,
		            (unsigned)DUK_VERSION % 100);
		system_info_length += mg_str_append(&buffer, end, block);
#endif
	}

	/* Build identifier. If BUILD_DATE is not set, __DATE__ will be used. */
	{
#if defined(BUILD_DATE)
		const char *bd = BUILD_DATE;
#else
#if defined(GCC_DIAGNOSTIC)
#if GCC_VERSION >= 40900
#pragma GCC diagnostic push
		/* Disable idiotic compiler warning -Wdate-time, appeared in gcc5. This
		 * does not work in some versions. If "BUILD_DATE" is defined to some
		 * string, it is used instead of __DATE__. */
#pragma GCC diagnostic ignored "-Wdate-time"
#endif
#endif
		const char *bd = __DATE__;
#if defined(GCC_DIAGNOSTIC)
#if GCC_VERSION >= 40900
#pragma GCC diagnostic pop
#endif
#endif
#endif

		mg_snprintf(
		    NULL, NULL, block, sizeof(block), ",%s\"build\" : \"%s\"", eol, bd);

		system_info_length += mg_str_append(&buffer, end, block);
	}

	/* Compiler information */
	/* http://sourceforge.net/p/predef/wiki/Compilers/ */
	{
#if defined(_MSC_VER)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"MSC: %u (%u)\"",
		            eol,
		            (unsigned)_MSC_VER,
		            (unsigned)_MSC_FULL_VER);
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__MINGW64__)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"MinGW64: %u.%u\"",
		            eol,
		            (unsigned)__MINGW64_VERSION_MAJOR,
		            (unsigned)__MINGW64_VERSION_MINOR);
		system_info_length += mg_str_append(&buffer, end, block);
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"MinGW32: %u.%u\"",
		            eol,
		            (unsigned)__MINGW32_MAJOR_VERSION,
		            (unsigned)__MINGW32_MINOR_VERSION);
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__MINGW32__)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"MinGW32: %u.%u\"",
		            eol,
		            (unsigned)__MINGW32_MAJOR_VERSION,
		            (unsigned)__MINGW32_MINOR_VERSION);
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__clang__)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"clang: %u.%u.%u (%s)\"",
		            eol,
		            __clang_major__,
		            __clang_minor__,
		            __clang_patchlevel__,
		            __clang_version__);
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__GNUC__)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"gcc: %u.%u.%u\"",
		            eol,
		            (unsigned)__GNUC__,
		            (unsigned)__GNUC_MINOR__,
		            (unsigned)__GNUC_PATCHLEVEL__);
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__INTEL_COMPILER)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"Intel C/C++: %u\"",
		            eol,
		            (unsigned)__INTEL_COMPILER);
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__BORLANDC__)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"Borland C: 0x%x\"",
		            eol,
		            (unsigned)__BORLANDC__);
		system_info_length += mg_str_append(&buffer, end, block);
#elif defined(__SUNPRO_C)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"Solaris: 0x%x\"",
		            eol,
		            (unsigned)__SUNPRO_C);
		system_info_length += mg_str_append(&buffer, end, block);
#else
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"compiler\" : \"other\"",
		            eol);
		system_info_length += mg_str_append(&buffer, end, block);
#endif
	}

	/* Determine 32/64 bit data mode.
	 * see https://en.wikipedia.org/wiki/64-bit_computing */
	{
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"data_model\" : \"int:%u/%u/%u/%u, float:%u/%u/%u, "
		            "char:%u/%u, "
		            "ptr:%u, size:%u, time:%u\"",
		            eol,
		            (unsigned)sizeof(short),
		            (unsigned)sizeof(int),
		            (unsigned)sizeof(long),
		            (unsigned)sizeof(long long),
		            (unsigned)sizeof(float),
		            (unsigned)sizeof(double),
		            (unsigned)sizeof(long double),
		            (unsigned)sizeof(char),
		            (unsigned)sizeof(wchar_t),
		            (unsigned)sizeof(void *),
		            (unsigned)sizeof(size_t),
		            (unsigned)sizeof(time_t));
		system_info_length += mg_str_append(&buffer, end, block);
	}

	/* Terminate string */
	if (append_eoobj) {
		strcat(append_eoobj, eoobj);
	}
	system_info_length += sizeof(eoobj) - 1;

	return (int)system_info_length;
}


/* Get context information. It can be printed or stored by the caller.
 * Return the size of available information. */
CIVETWEB_API int
mg_get_context_info(const struct mg_context *ctx, char *buffer, int buflen)
{
#if defined(USE_SERVER_STATS)
	char *end, *append_eoobj = NULL, block[256];
	size_t context_info_length = 0;

#if defined(_WIN32)
	static const char eol[] = "\r\n", eoobj[] = "\r\n}\r\n";
#else
	static const char eol[] = "\n", eoobj[] = "\n}\n";
#endif
	struct mg_memory_stat *ms = get_memory_stat((struct mg_context *)ctx);

	if ((buffer == NULL) || (buflen < 1)) {
		buflen = 0;
		end = buffer;
	} else {
		*buffer = 0;
		end = buffer + buflen;
	}
	if (buflen > (int)(sizeof(eoobj) - 1)) {
		/* has enough space to append eoobj */
		append_eoobj = buffer;
		end -= sizeof(eoobj) - 1;
	}

	context_info_length += mg_str_append(&buffer, end, "{");

	if (ms) { /* <-- should be always true */
		      /* Memory information */
		int blockCount = (int)ms->blockCount;
		int64_t totalMemUsed = ms->totalMemUsed;
		int64_t maxMemUsed = ms->maxMemUsed;
		if (totalMemUsed > maxMemUsed) {
			maxMemUsed = totalMemUsed;
		}

		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            "%s\"memory\" : {%s"
		            "\"blocks\" : %i,%s"
		            "\"used\" : %" INT64_FMT ",%s"
		            "\"maxUsed\" : %" INT64_FMT "%s"
		            "}",
		            eol,
		            eol,
		            blockCount,
		            eol,
		            totalMemUsed,
		            eol,
		            maxMemUsed,
		            eol);
		context_info_length += mg_str_append(&buffer, end, block);
	}

	if (ctx) {
		/* Declare all variables at begin of the block, to comply
		 * with old C standards. */
		char start_time_str[64] = {0};
		char now_str[64] = {0};
		time_t start_time = ctx->start_time;
		time_t now = time(NULL);
		int64_t total_data_read, total_data_written;
		int active_connections = (int)ctx->active_connections;
		int max_active_connections = (int)ctx->max_active_connections;
		int total_connections = (int)ctx->total_connections;
		if (active_connections > max_active_connections) {
			max_active_connections = active_connections;
		}
		if (active_connections > total_connections) {
			total_connections = active_connections;
		}

		/* Connections information */
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"connections\" : {%s"
		            "\"active\" : %i,%s"
		            "\"maxActive\" : %i,%s"
		            "\"total\" : %i%s"
		            "}",
		            eol,
		            eol,
		            active_connections,
		            eol,
		            max_active_connections,
		            eol,
		            total_connections,
		            eol);
		context_info_length += mg_str_append(&buffer, end, block);

		/* Queue information */
#if !defined(ALTERNATIVE_QUEUE)
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"queue\" : {%s"
		            "\"length\" : %i,%s"
		            "\"filled\" : %i,%s"
		            "\"maxFilled\" : %i,%s"
		            "\"full\" : %s%s"
		            "}",
		            eol,
		            eol,
		            ctx->sq_size,
		            eol,
		            ctx->sq_head - ctx->sq_tail,
		            eol,
		            ctx->sq_max_fill,
		            eol,
		            (ctx->sq_blocked ? "true" : "false"),
		            eol);
		context_info_length += mg_str_append(&buffer, end, block);
#endif

		/* Requests information */
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"requests\" : {%s"
		            "\"total\" : %lu%s"
		            "}",
		            eol,
		            eol,
		            (unsigned long)ctx->total_requests,
		            eol);
		context_info_length += mg_str_append(&buffer, end, block);

		/* Data information */
		total_data_read =
		    mg_atomic_add64((volatile int64_t *)&ctx->total_data_read, 0);
		total_data_written =
		    mg_atomic_add64((volatile int64_t *)&ctx->total_data_written, 0);
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"data\" : {%s"
		            "\"read\" : %" INT64_FMT ",%s"
		            "\"written\" : %" INT64_FMT "%s"
		            "}",
		            eol,
		            eol,
		            total_data_read,
		            eol,
		            total_data_written,
		            eol);
		context_info_length += mg_str_append(&buffer, end, block);

		/* Execution time information */
		gmt_time_string(start_time_str,
		                sizeof(start_time_str) - 1,
		                &start_time);
		gmt_time_string(now_str, sizeof(now_str) - 1, &now);

		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            ",%s\"time\" : {%s"
		            "\"uptime\" : %.0f,%s"
		            "\"start\" : \"%s\",%s"
		            "\"now\" : \"%s\"%s"
		            "}",
		            eol,
		            eol,
		            difftime(now, start_time),
		            eol,
		            start_time_str,
		            eol,
		            now_str,
		            eol);
		context_info_length += mg_str_append(&buffer, end, block);
	}

	/* Terminate string */
	if (append_eoobj) {
		strcat(append_eoobj, eoobj);
	}
	context_info_length += sizeof(eoobj) - 1;

	return (int)context_info_length;
#else
	(void)ctx;
	if ((buffer != NULL) && (buflen > 0)) {
		*buffer = 0;
	}
	return 0;
#endif
}


CIVETWEB_API void
mg_disable_connection_keep_alive(struct mg_connection *conn)
{
	/* https://github.com/civetweb/civetweb/issues/727 */
	if (conn != NULL) {
		conn->must_close = 1;
	}
}


#if defined(MG_EXPERIMENTAL_INTERFACES)
/* Get connection information. It can be printed or stored by the caller.
 * Return the size of available information. */
CIVETWEB_API int
mg_get_connection_info(const struct mg_context *ctx,
                       int idx,
                       char *buffer,
                       int buflen)
{
	const struct mg_connection *conn;
	const struct mg_request_info *ri;
	char *end, *append_eoobj = NULL, block[256];
	size_t connection_info_length = 0;
	int state = 0;
	const char *state_str = "unknown";

#if defined(_WIN32)
	static const char eol[] = "\r\n", eoobj[] = "\r\n}\r\n";
#else
	static const char eol[] = "\n", eoobj[] = "\n}\n";
#endif

	if ((buffer == NULL) || (buflen < 1)) {
		buflen = 0;
		end = buffer;
	} else {
		*buffer = 0;
		end = buffer + buflen;
	}
	if (buflen > (int)(sizeof(eoobj) - 1)) {
		/* has enough space to append eoobj */
		append_eoobj = buffer;
		end -= sizeof(eoobj) - 1;
	}

	if ((ctx == NULL) || (idx < 0)) {
		/* Parameter error */
		return 0;
	}

	if ((unsigned)idx >= ctx->cfg_max_worker_threads) {
		/* Out of range */
		return 0;
	}

	/* Take connection [idx]. This connection is not locked in
	 * any way, so some other thread might use it. */
	conn = (ctx->worker_connections) + idx;

	/* Initialize output string */
	connection_info_length += mg_str_append(&buffer, end, "{");

	/* Init variables */
	ri = &(conn->request_info);

#if defined(USE_SERVER_STATS)
	state = conn->conn_state;

	/* State as string */
	switch (state) {
	case 0:
		state_str = "undefined";
		break;
	case 1:
		state_str = "not used";
		break;
	case 2:
		state_str = "init";
		break;
	case 3:
		state_str = "ready";
		break;
	case 4:
		state_str = "processing";
		break;
	case 5:
		state_str = "processed";
		break;
	case 6:
		state_str = "to close";
		break;
	case 7:
		state_str = "closing";
		break;
	case 8:
		state_str = "closed";
		break;
	case 9:
		state_str = "done";
		break;
	}
#endif

	/* Connection info */
	if ((state >= 3) && (state < 9)) {
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            "%s\"connection\" : {%s"
		            "\"remote\" : {%s"
		            "\"protocol\" : \"%s\",%s"
		            "\"addr\" : \"%s\",%s"
		            "\"port\" : %u%s"
		            "},%s"
		            "\"handled_requests\" : %u%s"
		            "}",
		            eol,
		            eol,
		            eol,
		            get_proto_name(conn),
		            eol,
		            ri->remote_addr,
		            eol,
		            ri->remote_port,
		            eol,
		            eol,
		            conn->handled_requests,
		            eol);
		connection_info_length += mg_str_append(&buffer, end, block);
	}

	/* Request info */
	if ((state >= 4) && (state < 6)) {
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            "%s%s\"request_info\" : {%s"
		            "\"method\" : \"%s\",%s"
		            "\"uri\" : \"%s\",%s"
		            "\"query\" : %s%s%s%s"
		            "}",
		            (connection_info_length > 1 ? "," : ""),
		            eol,
		            eol,
		            ri->request_method,
		            eol,
		            ri->request_uri,
		            eol,
		            ri->query_string ? "\"" : "",
		            ri->query_string ? ri->query_string : "null",
		            ri->query_string ? "\"" : "",
		            eol);
		connection_info_length += mg_str_append(&buffer, end, block);
	}

	/* Execution time information */
	if ((state >= 2) && (state < 9)) {
		char start_time_str[64] = {0};
		char close_time_str[64] = {0};
		time_t start_time = conn->conn_birth_time;
		time_t close_time = 0;
		double time_diff;

		gmt_time_string(start_time_str,
		                sizeof(start_time_str) - 1,
		                &start_time);
#if defined(USE_SERVER_STATS)
		close_time = conn->conn_close_time;
#endif
		if (close_time != 0) {
			time_diff = difftime(close_time, start_time);
			gmt_time_string(close_time_str,
			                sizeof(close_time_str) - 1,
			                &close_time);
		} else {
			time_t now = time(NULL);
			time_diff = difftime(now, start_time);
			close_time_str[0] = 0; /* or use "now" ? */
		}

		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            "%s%s\"time\" : {%s"
		            "\"uptime\" : %.0f,%s"
		            "\"start\" : \"%s\",%s"
		            "\"closed\" : \"%s\"%s"
		            "}",
		            (connection_info_length > 1 ? "," : ""),
		            eol,
		            eol,
		            time_diff,
		            eol,
		            start_time_str,
		            eol,
		            close_time_str,
		            eol);
		connection_info_length += mg_str_append(&buffer, end, block);
	}

	/* Remote user name */
	if ((ri->remote_user) && (state < 9)) {
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            "%s%s\"user\" : {%s"
		            "\"name\" : \"%s\",%s"
		            "}",
		            (connection_info_length > 1 ? "," : ""),
		            eol,
		            eol,
		            ri->remote_user,
		            eol);
		connection_info_length += mg_str_append(&buffer, end, block);
	}

	/* Data block */
	if (state >= 3) {
		mg_snprintf(NULL,
		            NULL,
		            block,
		            sizeof(block),
		            "%s%s\"data\" : {%s"
		            "\"read\" : %" INT64_FMT ",%s"
		            "\"written\" : %" INT64_FMT "%s"
		            "}",
		            (connection_info_length > 1 ? "," : ""),
		            eol,
		            eol,
		            conn->consumed_content,
		            eol,
		            conn->num_bytes_sent,
		            eol);
		connection_info_length += mg_str_append(&buffer, end, block);
	}

	/* State */
	mg_snprintf(NULL,
	            NULL,
	            block,
	            sizeof(block),
	            "%s%s\"state\" : \"%s\"",
	            (connection_info_length > 1 ? "," : ""),
	            eol,
	            state_str);
	connection_info_length += mg_str_append(&buffer, end, block);

	/* Terminate string */
	if (append_eoobj) {
		strcat(append_eoobj, eoobj);
	}
	connection_info_length += sizeof(eoobj) - 1;

	return (int)connection_info_length;
}


#if 0
/* Get handler information. Not fully implemented. Is it required? */
CIVETWEB_API int
mg_get_handler_info(struct mg_context *ctx,
	char *buffer,
	int buflen)
{
	int handler_info_len = 0;
	struct mg_handler_info *tmp_rh;
	mg_lock_context(ctx);

	for (tmp_rh = ctx->dd.handlers; tmp_rh != NULL; tmp_rh = tmp_rh->next) {

		if (buflen > handler_info_len + tmp_rh->uri_len) {
			memcpy(buffer + handler_info_len, tmp_rh->uri, tmp_rh->uri_len);
		}
		handler_info_len += tmp_rh->uri_len;

		switch (tmp_rh->handler_type) {
		case REQUEST_HANDLER:
			(void)tmp_rh->handler;
			break;
		case WEBSOCKET_HANDLER:
			(void)tmp_rh->connect_handler;
			(void)tmp_rh->ready_handler;
			(void)tmp_rh->data_handler;
			(void)tmp_rh->close_handler;
			break;
		case AUTH_HANDLER:
			(void)tmp_rh->auth_handler;
			break;
		}
		(void)cbdata;
	}

	mg_unlock_context(ctx);
	return handler_info_len;
}
#endif
#endif


/* Initialize this library. This function does not need to be thread safe.
 */
CIVETWEB_API unsigned
mg_init_library(unsigned features)
{
	unsigned features_to_init = mg_check_feature(features & 0xFFu);
	unsigned features_inited = features_to_init;

	if (mg_init_library_called <= 0) {
		/* Not initialized yet */
		if (0 != pthread_mutex_init(&global_lock_mutex, NULL)) {
			return 0;
		}
	}

	mg_global_lock();

	if (mg_init_library_called <= 0) {
		int i;
		size_t len;

#if defined(_WIN32)
		int file_mutex_init = 1;
		int wsa = 1;
#else
		int mutexattr_init = 1;
#endif
		int failed = 1;
		int key_create = pthread_key_create(&sTlsKey, tls_dtor);

		if (key_create == 0) {
#if defined(_WIN32)
			file_mutex_init =
			    pthread_mutex_init(&global_log_file_lock, &pthread_mutex_attr);
			if (file_mutex_init == 0) {
				/* Start WinSock */
				WSADATA data;
				failed = wsa = WSAStartup(MAKEWORD(2, 2), &data);
			}
#else
			mutexattr_init = pthread_mutexattr_init(&pthread_mutex_attr);
			if (mutexattr_init == 0) {
				failed = pthread_mutexattr_settype(&pthread_mutex_attr,
				                                   PTHREAD_MUTEX_RECURSIVE);
			}
#endif
		}

		if (failed) {
#if defined(_WIN32)
			if (wsa == 0) {
				(void)WSACleanup();
			}
			if (file_mutex_init == 0) {
				(void)pthread_mutex_destroy(&global_log_file_lock);
			}
#else
			if (mutexattr_init == 0) {
				(void)pthread_mutexattr_destroy(&pthread_mutex_attr);
			}
#endif
			if (key_create == 0) {
				(void)pthread_key_delete(sTlsKey);
			}
			mg_global_unlock();
			(void)pthread_mutex_destroy(&global_lock_mutex);
			return 0;
		}

		len = 1;
		for (i = 0; http_methods[i].name != NULL; i++) {
			size_t sl = strlen(http_methods[i].name);
			len += sl;
			if (i > 0) {
				len += 2;
			}
		}
		all_methods = (char *)mg_malloc(len);
		if (!all_methods) {
			/* Must never happen */
			mg_global_unlock();
			(void)pthread_mutex_destroy(&global_lock_mutex);
			return 0;
		}
		all_methods[0] = 0;
		for (i = 0; http_methods[i].name != NULL; i++) {
			if (i > 0) {
				strcat(all_methods, ", ");
				strcat(all_methods, http_methods[i].name);
			} else {
				strcpy(all_methods, http_methods[i].name);
			}
		}
	}

#if defined(USE_LUA)
	lua_init_optional_libraries();
#endif

#if (defined(OPENSSL_API_1_0) || defined(OPENSSL_API_1_1)                      \
     || defined(OPENSSL_API_3_0))                                              \
    && !defined(NO_SSL)

	if (features_to_init & MG_FEATURES_SSL) {
		if (!mg_openssl_initialized) {
			char ebuf[128];
			if (initialize_openssl(ebuf, sizeof(ebuf))) {
				mg_openssl_initialized = 1;
			} else {
				(void)ebuf;
				DEBUG_TRACE("Initializing SSL failed: %s", ebuf);
				features_inited &= ~((unsigned)(MG_FEATURES_SSL));
			}
		} else {
			/* ssl already initialized */
		}
	}

#endif

	if (mg_init_library_called <= 0) {
		mg_init_library_called = 1;
	} else {
		mg_init_library_called++;
	}
	mg_global_unlock();

	return features_inited;
}


/* Un-initialize this library. */
CIVETWEB_API unsigned
mg_exit_library(void)
{
	if (mg_init_library_called <= 0) {
		return 0;
	}

	mg_global_lock();

	mg_init_library_called--;
	if (mg_init_library_called == 0) {
#if (defined(OPENSSL_API_1_0) || defined(OPENSSL_API_1_1)) && !defined(NO_SSL)
		if (mg_openssl_initialized) {
			uninitialize_openssl();
			mg_openssl_initialized = 0;
		}
#endif

#if defined(_WIN32)
		(void)WSACleanup();
		(void)pthread_mutex_destroy(&global_log_file_lock);
#else
		(void)pthread_mutexattr_destroy(&pthread_mutex_attr);
#endif

		(void)pthread_key_delete(sTlsKey);

#if defined(USE_LUA)
		lua_exit_optional_libraries();
#endif
		mg_free(all_methods);
		all_methods = NULL;

		mg_global_unlock();
		(void)pthread_mutex_destroy(&global_lock_mutex);
		return 1;
	}

	mg_global_unlock();
	return 1;
}


/* End of civetweb.c */
