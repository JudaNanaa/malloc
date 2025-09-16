#include "../includes/malloc_internal.h"

pthread_mutex_t g_malloc_lock = PTHREAD_MUTEX_INITIALIZER;

void	close_trace_file_fd(void)
{
	close(g_malloc.trace_file_fd);
}

void	malloc_init(void)
{
	char	*env;
	char	*end;

	g_malloc.set = true;
	env = getenv("MALLOC_VERBOSE");
	if (env && env[0] == '1')
		g_malloc.verbose = true;
	env = getenv("MALLOC_FAIL_SIZE");
	if (env)
		g_malloc.fail_size = atoi(env);
	env = getenv("MALLOC_NO_DEFRAG");
	if (env && env[0] == '1')
		g_malloc.no_defrag = true;
	env = getenv("MALLOC_TRACE_FILE");
	if (env)
	{
		g_malloc.trace_file_fd = open(env, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (g_malloc.trace_file_fd == -1)
			print_err("Fail to pen trace file\n");
		atexit(close_trace_file_fd);
	}
	env = getenv("MALLOC_TINY_SIZE");
	if (env)
	{
		g_malloc.tiny_malloc_size = strtoul(env, &end, 10);
		if (g_malloc.tiny_malloc_size <= 0)
		{
			print_err("malloc_init() : MALLOC_TINY_SIZE env var is not good\n");
			abort();
		}
	}
	env = getenv("MALLOC_SMALL_SIZE");
	if (env)
	{
		g_malloc.small_malloc_size = strtoul(env, &end, 10);
		if (g_malloc.small_malloc_size <= 0)
		{
			print_err("malloc_init() : MALLOC_SMALL_SIZE env var is not good\n");
			abort();
		}
	}
	g_malloc.tiny_malloc_size = ALIGN(g_malloc.tiny_malloc_size);
	g_malloc.small_malloc_size = ALIGN(g_malloc.small_malloc_size);
}
