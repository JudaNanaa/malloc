#include "../includes/malloc_internal.h"
#include <pthread.h>

bool g_malloc_is_set(void)
{
	bool is_set;

	pthread_mutex_lock(&g_malloc_lock);
	is_set = g_malloc.set;
	pthread_mutex_unlock(&g_malloc_lock);
	return is_set;
}

bool g_malloc_verbose(void)
{
	bool verbose;

	pthread_mutex_lock(&g_malloc_lock);
	verbose = g_malloc.verbose;
	pthread_mutex_unlock(&g_malloc_lock);
	return verbose;
}

bool g_malloc_no_defrag(void)
{
	bool no_defrag;

	pthread_mutex_lock(&g_malloc_lock);
	no_defrag = g_malloc.no_defrag;
	pthread_mutex_unlock(&g_malloc_lock);
	return no_defrag;
}

int g_malloc_fail_size(void)
{
	int fail_size;

	pthread_mutex_lock(&g_malloc_lock);
	fail_size = g_malloc.fail_size;
	pthread_mutex_unlock(&g_malloc_lock);
	return fail_size;
}

int g_malloc_trace_file_fd(void)
{
	int trace_file_fd;

	pthread_mutex_lock(&g_malloc_lock);
	trace_file_fd = g_malloc.trace_file_fd;
	pthread_mutex_unlock(&g_malloc_lock);
	return trace_file_fd;
}
