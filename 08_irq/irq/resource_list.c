// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>

#include "resource_list.h"

struct resource_node {
	void *data;
	resorce_fnc_t init_fnc;
	resorce_fnc_t deinit_fnc;

	struct list_head resource_node_head;
};

static int init_resource(struct resource_node* entry)
{
	if (entry->init_fnc == NULL ||
	entry->data == NULL)
		return -EINVAL;

	return entry->init_fnc(entry->data);
}
static int deinit_resource(struct resource_node* entry)
{
	if (entry->deinit_fnc == NULL ||
	entry->data == NULL)
		return -EINVAL;

	return entry->deinit_fnc(entry->data);
}

static int resorce_list_del(struct resource_node* entry)
{
	list_del(&entry->resource_node_head);
	kzfree(entry);

	return 0;
}

int resorce_list_init(struct resorce_list *list_ptr)
{
	INIT_LIST_HEAD(&list_ptr->resorce_list_head);
}

int resorce_init_new(struct resorce_list *list_ptr, void *init_data,
		     resorce_fnc_t init_fnc,
		     resorce_fnc_t deinit_fnc)
{
	int res;
	struct resource_node *entry = NULL;

	if (list_ptr == NULL) {
		pr_err("%s: linked list doesn't exist\n", __func__);
		return -EINVAL;
	}

	if (init_data == NULL ||
	init_fnc == NULL ||
	deinit_fnc == NULL) {
		pr_err("%s: one or more input is NULL\n", __func__);
		return -EINVAL;
	}

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (entry == NULL)	{
		pr_err("%s: failed to aloc entry\n", __func__);
		return -ENOMEM;
	}

	entry->data = init_data;
	entry->init_fnc = init_fnc;
	entry->deinit_fnc = deinit_fnc;

	res = init_resource(entry);

	if (res < 0){
		pr_info("%s: resource init failed \n", __func__);
		resorce_deinit_all(list_ptr);
		kzfree(entry);
		return res;
	}

	list_add_tail(&entry->resource_node_head, &list_ptr->resorce_list_head);
	return 0;

}

int resorce_deinit_all(struct resorce_list *list_ptr)
{
	int res = 0;
	struct resource_node *entry = NULL;

	if (list_ptr == NULL) {
		pr_err("%s: linked list doesn't exist\n", __func__);
		return -EINVAL;
	}

	list_for_each_entry(entry, &list_ptr->resorce_list_head, resource_node_head) {
		deinit_resource(entry);
		resorce_list_del(entry);
	}

	return res;
}
