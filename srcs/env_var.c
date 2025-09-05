#include "../includes/malloc_internal.h"

void	close_trace_file_fd(void)
{
	close(g_malloc.trace_file_fd);
}

void	malloc_init(void)
{
	char	*env;

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
			ft_putendl_fd("Fail to pen trace file", STDERR_FILENO);
		atexit(close_trace_file_fd);
	}
}
