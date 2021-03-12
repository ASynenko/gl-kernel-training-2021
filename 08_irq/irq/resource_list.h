#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include <linux/types.h>
#include <linux/list.h>

#define resorce_init_new_generic(list_ptr, data, init, deinit) \
	resorce_init_new(list_ptr, (void *)(data), (resorce_fnc_t)(init), (resorce_fnc_t)(deinit) )

typedef int (*resorce_fnc_t)(void *);

struct resorce_list {
	struct list_head resorce_list_head;
};

int resorce_list_init(struct resorce_list *list_ptr);

int resorce_init_new(struct resorce_list *list_ptr, void *init_data,
		     resorce_fnc_t init_fnc,
		     resorce_fnc_t deinit_fnc);

int resorce_deinit_all(struct resorce_list *list_ptr);


#endif // RESOURCE_LIST_H
