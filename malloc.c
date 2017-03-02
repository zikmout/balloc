#include "libft/libft.h"
#include "includes/malloc.h"

t_zone		*init_ts(t_zone **begin, size_t zone_size) {

	void	*ptr;
	t_head	*head;
	t_zone	*zone;

	head = NULL;
	ptr = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	zone = ptr;

	head = ptr + sizeof(*(*begin));
	head->addr = ptr + sizeof(*zone) + sizeof(*head);//head + sizeof(*head);
	head->empty = 1;
	head->size = zone_size - sizeof(*zone) - sizeof(*head);
	head->next = NULL;
	head->prev = NULL;

	zone->entry = head;
	zone->zleft = head->size;
	zone->next = NULL;

	if (*begin)
	{
		write(1, "--NewZone--\n", 12);
		return zone;
	}
	*begin = zone;
	return NULL;
}

void		*new_zone_alloc(t_zone **zcur, size_t size) {

	size_t	zone_size;

	(size <= TMAX_SIZE) ? (zone_size = TZMAX_SIZE) : (zone_size = SZMAX_SIZE);
	(*zcur)->next = init_ts(zcur, zone_size);
	write(1, "ZZ*****-----> debug\n", 21);
	//print_zone(g_e.tiny);
	new_alloc_end(&(*zcur)->next, &((*zcur)->next->entry), size);
	return (((*zcur)->next)->entry->addr);
}

void		new_alloc_end(t_zone **zcur, t_head **hcur, size_t size) {

	t_head	*end;

	end = (*hcur)->addr + size;
	(*zcur)->zleft = (*zcur)->zleft - size - sizeof(*end);
	write(1, "EE*****-----> debug\n", 21);
	end->addr = (*hcur)->addr + size + sizeof(*end);
	end->size = (*zcur)->zleft;
	end->empty = 1;
	end->next = NULL;
	end->prev = (*hcur);

	(*hcur)->empty = 0;
	(*hcur)->size = size;
	(*hcur)->next = end;
}

void		*malloc(size_t size) {

	t_zone	*zcur;
	t_head	*hcur;

	if (!size)
		return NULL;
	printf("******** MALLOC of size -> %zu\n", size);
	if (size > SMAX_SIZE)
		return malloc_large(size);
	(size <= TMAX_SIZE) ? (zcur = g_e.tiny) : (zcur = g_e.small);

	while (zcur) {
		hcur = zcur->entry;
		while (hcur) {

			if (hcur->next && hcur->empty == 1 && hcur->size >= size) {
				write(1, "-> new alloc middle\n", 20);
				hcur->empty = (int)size;
				return hcur->addr;
			}
			else if (!hcur->next && zcur->zleft >= size + sizeof(*hcur)) {
				write(1, "-> new alloc end\n", 17);
				new_alloc_end(&zcur, &hcur, size);
				return hcur->addr;
			}

			hcur = hcur->next;
		}
		if (zcur->next == NULL)
			break ;
		zcur = zcur->next;
	}

	return new_zone_alloc(&zcur, size);
}

void		print_zone(t_zone *begin) {

	t_zone *zhead;
	t_head *cur;
	size_t nb;

	nb = 0;
	zhead = begin;
	while (zhead) {
		printf("\n---> Zone %zu\n", nb);
		printf("zone itelf => %p\n", zhead);
		printf("zone->entry => %p\n", zhead->entry);
		printf("zone->zleft => %zu\n", zhead->zleft);
		printf("zone->next => %p\n\n", zhead->next);

		cur = zhead->entry;
		while (cur) {
			printf("     cur itelf => %p\n", cur);
			printf("     cur->addr => %p\n", cur->addr);
			printf("     cur->empty => %d\n", cur->empty);
			printf("     cur->size => %zu\n", cur->size);
			printf("     cur->next => %p\n", cur->next);
			printf("     cur->prev => %p\n\n", cur->prev);
			cur = cur->next;
		}
		nb = nb + 1;
		zhead = zhead->next;
	}
}


t_head		*list_find_end(t_head *begin) {

	if (!begin->next)
		return begin;
	else
		return list_find_end(begin->next);
}

void		print_large(t_head *begin) {

	t_head *hcur;

	hcur = begin;
	while (hcur) {
		printf("     LARGEhcur itelf => %p\n", hcur);
		printf("     LARGEhcur->addr => %p\n", hcur->addr);
		printf("     LARGEhcur->empty => %d\n", hcur->empty);
		printf("     LARGEhcur->size => %zu\n", hcur->size);
		printf("     LARGEhcur->next => %p\n\n", hcur->next);
		hcur = hcur->next;
	}
}

void		*realloc(void *ptr, size_t size) {

	t_zone	*test;
	t_head	*found;

	printf("******** REALLOC of pointer -> %p for size %zu\n", ptr, size);
	found = locate(g_e.tiny, &test, ptr);
	if (found && test) {
		printf("Its a tiny\n");
		if (found->size >= size) {
			found->empty = (int)size;
			return found->addr;
		}
		else if (found->size < size) {
			free(ptr);
			return malloc(size);
		}
		return NULL;
	}
	/*
	   found = locate(g_e.small, &test, ptr);
	   if (found && test) {
	   printf("Its a small\n");
	   return NULL;
	   }
	   */
	return NULL;
}

t_head		*parse_large(t_head *begin, void *ptr) {

	t_head	*hcur;
	t_head	*hcur_prev;
	t_head	*tmp;

	tmp = NULL;
	hcur_prev = NULL;
	hcur = begin;
	while (hcur) {
		if (hcur->addr == ptr) {
			if (hcur_prev != NULL && hcur->next != NULL) {
				printf("(middle) HCUR PREV ----> %zu\n", hcur_prev->size);
				hcur_prev->next = hcur->next;
				printf("Retour mmap %d\n", munmap((void *)(hcur), hcur->size + sizeof(*hcur)));
				return NULL;
			}
			else if (hcur->next == NULL) {
				printf("(fin) HCUR SIZE ----> %zu\n", hcur->size);
				if (hcur_prev == NULL) {
					printf("Retour mmap %d\n", munmap((void *)(g_e.large), hcur->size + sizeof(*hcur)));
					g_e.large = NULL;
					return NULL;
				}
				else {
					hcur_prev->next = NULL;
					printf("Retour mmap %d\n", munmap((void *)(hcur), hcur->size + sizeof(*hcur)));
					return NULL;
				}
				return NULL;
			}
			else {
				printf("(debut) HCUR SIZE ----> %zu\n", hcur->size);
				g_e.large = hcur->next;
				printf("Retour mmap %d\n", munmap((void *)(hcur), hcur->size + sizeof(*hcur)));
				return NULL;
			}
		}
		hcur_prev = hcur;
		hcur = hcur->next;
	}
	return NULL;
}

void		*malloc_large(size_t size) {

	t_head	*hcur;
	void	*ptr;

	ptr = mmap(NULL, size + 32, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	hcur = ptr;
	hcur->addr = hcur + sizeof(*hcur);
	hcur->empty = 0;
	hcur->size = size;
	hcur->next = NULL;

	if (g_e.large == NULL)
		g_e.large = hcur;
	else
		list_find_end(g_e.large)->next = hcur;

	return hcur->addr;
}

void		*realloc_large(void *ptr, size_t size) {

	size = 35;
	ptr = NULL;

	return NULL;
}


t_head		*locate(t_zone *begin, t_zone **head, void *ptr) {

	t_zone	*zcur;
	t_head	*hcur;

	zcur = begin;
	while (zcur) {
		hcur = zcur->entry;
		while (hcur) {
			if (hcur->addr == ptr) {
				*head = zcur;
				return hcur;
			}
			hcur = hcur->next;
		}
		zcur = zcur->next;
	}
	return NULL;
}

void		free_zone(t_zone *to_free) {



}

void		check_fusion(t_zone *test, t_head *found) {

	write(1, "ENTER -------------FUSION\n", 26);
	if (found->prev && found->next && found->prev->empty == 1 && found->next->empty == 1) {

		found->prev->next = found->next->next;
		found->prev->size = found->prev->size + found->size + found->next->size;
		if (found->next->next)
			found->next->next->prev = found->prev;
		test->zleft = test->zleft + sizeof(*found) + sizeof(*found);
		if (!found->prev->prev && !found->prev->next) {
			printf("Vider la ZZZZZZZZZZZZZZZZZZZZZZZZZONOOOOONNNNE\n");
			//free_zone(test);
		}
	}
	else {
		if (found->prev && found->prev->empty == 1) {
			found->prev->next = found->next;
			found->prev->size = found->prev->size + found->size;
			found->next->prev = found->prev;
			test->zleft = test->zleft + sizeof(*found);
		}
		if (found->next && found->next->empty == 1) {

			found->prev->next = found->next;
			found->next->prev = found->prev;
			found->next->size = found->next->size + found->size;
			test->zleft = test->zleft + sizeof(*found);
		}
	}
}

void		free(void *ptr) {

	t_zone	*test;
	t_head	*found;

	found = locate(g_e.tiny, &test, ptr);
	if (found) {
		printf("found->size = %zu\n", found->size);
		found->empty = 1;
		check_fusion(test, found);
	}

}

t_head		*parse_ts(t_zone *begin, void *ptr) {

	t_zone	*zcur;
	t_head	*hcur;

	zcur = begin;
	while (zcur) {
		hcur = zcur->entry;
		while (hcur) {
			if (hcur->addr == ptr) {
				hcur->empty = 1;

				list_find_end(hcur)->size = zcur->zleft;
				//zcur->zleft = zcur->zleft + hcur->size;
				return hcur;
			}
			hcur = hcur->next;
		}
		zcur = zcur->next;
	}
	return NULL;
}

/*
   void		free(void *ptr) {

   t_head	*found;
   printf("******** FREE of pointer -> %p\n", ptr);
   if (g_e.tiny) {
   found = parse_ts(g_e.tiny, ptr);
   if (found) {
   printf("T-Return of parse_ts size ===> %zu\n", found->size);
   return ;
   }
   }
   if (g_e.small) {
   found = parse_ts(g_e.small, ptr);
   if (found) {
   printf("S-Return of parse_ts size ===> %zu\n", found->size);
   return ;
   }
   }
   if (g_e.large) {
   found = parse_large(g_e.large, ptr);
   if (found) {
   printf("L-Return of parse_ts size ===> %zu\n", found->size);
   return ;
   }
   }
   }
   */
