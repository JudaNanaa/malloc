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
	env = getenv("malloc_VERBOSE");
	if (env && env[0] == '1')
		g_malloc.verbose = true;
	env = getenv("malloc_FAIL_SIZE");
	if (env)
		g_malloc.fail_size = atoi(env);
	env = getenv("malloc_NO_DEFRAG");
	if (env && env[0] == '1')
		g_malloc.no_defrag = true;
	env = getenv("malloc_TRACE_FILE");
	if (env)
	{
		g_malloc.trace_file_fd = open(env, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (g_malloc.trace_file_fd == -1)
			print_err("Fail to pen trace file");
		atexit(close_trace_file_fd);
	}
	env = getenv("malloc_TINY_SIZE");
	if (env)
	{
		g_malloc.tiny_malloc_size = strtoul(env, &end, 10);
		if (g_malloc.tiny_malloc_size <= 0)
		{
			print_err("malloc_init() : malloc_TINY_SIZE env var is not good");
			abort();
		}
	}
	env = getenv("malloc_SMALL_SIZE");
	if (env)
	{
		g_malloc.small_malloc_size = strtoul(env, &end, 10);
		if (g_malloc.small_malloc_size <= 0)
		{
			print_err("malloc_init() : malloc_SMALL_SIZE env var is not good");
			abort();
		}
	}
	g_malloc.tiny_malloc_size = ALIGN(g_malloc.tiny_malloc_size);
	g_malloc.small_malloc_size = ALIGN(g_malloc.small_malloc_size);
}
