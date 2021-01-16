#include "pool_list.h"

#define PREALLOC_NUM (16)

static int expand_pool_list(pool_list_t *pool, int num, int size)
{
    int i = 0;
    char *ptr = NULL;
    struct list_head *slub = NULL;
    
    ptr = (char *)calloc(sizeof(struct list_head) + num * (size + sizeof(pool_elem_t)),sizeof(char));
    if (!ptr) {
        return -1;
    }

    slub = (struct list_head *)ptr;
    list_add(slub, &pool->slub);
    ptr += sizeof(struct list_head);

    elem = (pool_elem_t *)(ptr + num * size);
    for (i=0; i<num; i++) {
        elem[i].addr = ptr;
        list_add(&elem[i].list, &pool->free);
        ptr += size;
    }
	
    pool->free_num += num;
    return 0;
}

int pool_list_init(pool_list_t *pool, int elem_size)
{
	INIT_LIST_HEAD(&pool->available);
	INIT_LIST_HEAD(&pool->free);
	INIT_LIST_HEAD(&pool->slub);
	
	pool->max_slub = -1;
	pool->elem_size = elem_size;
	pool->free_num = 0;
	pool->prealloc_num = PREALLOC_NUM;
	
	return 0;
}

int pool_list_set_limit(int max_slub, int prealloc_num)
{
	pool->max_slub = max_slub;
	pool->prealloc_num = prealloc_num;
	
	return 0;
}

pool_elem_t *pool_list_get_free_elem(pool_list_t *pool)
{
	int ret = 0;
	pool_elem_t *elem = NULL;
	
	//no elem,so try expand it
	if (list_empty(&pool->free)) {
		if ((-1 == pool->max_slub) || (pool->cur_slub < pool->max_slub)) {
			ret = expand_pool(pool,pool->prealloc_num,pool->elem_size);
			if (0 == ret) {
				pool->cur_slub++;
			}
		}
	}
	
	//failed to expand or not allowed to expand
	if (list_empty(&pool->free)) {
		return NULL;
	}
	
	elem = list_first_entry(&pool->free, pool_elem_t,list);
	list_del(&elem->list);
	pool->free_num--;
	return elem;
}

inline int pool_list_put_available_elem(pool_list_t *pool, pool_elem_t *)
{
	list_add_tail(&elem->list, &pool->available);
	return 0;
}

inline pool_elem_t *pool_list_get_available_elem(pool_list_t *pool)
{
	pool_elem_t *elem = NULL;
	
	if (list_empty(&pool->available)) {
		return NULL;
	}
	
	elem = list_first_entry(&pool->available, pool_elem_t,list);
	return elem;
}

inline int pool_list_put_free_elem(pool_list_t *pool)
{
	list_move(&elem->list, &pool->free);
    pool->free_num++;
	return 0;
}



