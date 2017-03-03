#ifndef MALLOC_H
# define MALLOC_H
# define PAGE_SIZE getpagesize()
# define TMAX_SIZE (size_t)4056 // 4096 -  40
//# define TMAX_SIZE 992 // 1024 - 32
# define TNB 2048
# define TZMAX_SIZE (size_t)(4 * TNB) // 8192 - 24= 8136
//# define TZMAX_SIZE (size_t)((TMAX_SIZE + 32) * TNB) // 1024 * 2048 = 2 097 152 (= 2Mb)

# define SMAX_SIZE (size_t)131032 // 131072 - 32 - 24
# define SNB 128
# define SZMAX_SIZE (size_t)((SMAX_SIZE + 40) * SNB) // 131072 * 128 = 16 777 215 (= 16Mb)

//# define align4(x) (((((x)-1)>>2)<<2)+4) // 32bit architecture
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "../libft/libft.h"

typedef struct s_head t_head;
struct s_head
{
	void	*addr;
	int		empty;
	size_t	size;
	t_head	*prev;
	t_head	*next;
};

typedef struct s_zone t_zone;
struct s_zone
{
	t_head	*entry;
	size_t	zleft;
	t_zone	*next;
};


typedef struct s_env t_env;
struct s_env
{
	t_zone *tiny;
	t_zone *small;
	t_head *large;
};

t_env g_e;

t_zone	*init_ts(t_zone **zone, size_t zone_size);
void	*malloc(size_t size);
void	print_zone(t_zone *begin);
void	*new_zone_alloc(t_zone **zcur, size_t size);
void	*malloc_large(size_t size);
void	print_large(t_head *begin);
t_head	*list_find_end(t_head *begin);
void	new_alloc_end(t_zone **zcur, t_head **hcur, size_t size);
t_head	*parse_large(t_head *begin, void *ptr);
void	free(void *ptr);

int		free_zone(t_zone *to_free, size_t zone_size);
void	check_fusion(t_zone *test, t_head *found);
void	init_all(size_t size);

t_head	*locate(t_zone *begin, t_zone **head, void *ptr);
t_head	*locate_head(t_head *head, void *ptr);
#endif
