/* dict.c
 * Fast dictionary implementation
 * 
 * Copyright 2015-2019 Akash Rawal
 * This file is part of Modular Middleware.
 * 
 * Modular Middleware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Modular Middleware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Modular Middleware.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "incl.h"

#include "private.h"

#define MAX_NODE_LEN 32

typedef struct 
{
	void *value;
	ByteMap next;
	uint8_t len;
	uint8_t ekey[];
} DictNode;

struct _MdslDict
{
	MdslRC parent;
	DictNode root;
};

mdsl_rc_define(MdslDict, mdsl_dict);

mdsl_declare_array(DictNode *, DictNodeArray, dict_node_array);


static DictNode *alloc_node(const uint8_t *ekey, size_t len)
{
	DictNode *dn = (DictNode *) mdsl_alloc(sizeof(DictNode) + len);
	int i;
	for (i = 0; i < len; i++)
		dn->ekey[i] = ekey[i];
	dn->len = len;
	dn->value = NULL;
	byte_map_init(&(dn->next));

	return dn;
}

static DictNode *create_node
	(MdslDict *dict, const void *key, size_t key_len)
{
	const uint8_t *ekey = (const uint8_t *) key;

	//Find the node to graft a branch
	DictNode *target_ptr_node = NULL;
	int target_ptr_chr = 0;
	DictNode *target = &(dict->root);
	int target_offset = 0;

	int ekey_offset = 0;

	while (target && ekey_offset < key_len)
	{
		int i;
		int remains = key_len - ekey_offset;
		int run_len = target->len;
		if (run_len >= remains)
		{
			for (i = 0; i < remains; i++)
			{
				if (target->ekey[i] != ekey[ekey_offset + i])
					break;
			}
		}
		else
		{
			for (i = 0; i < run_len; i++)
			{
				if (target->ekey[i] != ekey[ekey_offset + i])
					break;
			}
			if (i == run_len)
			{
				DictNode *next = byte_map_get
					(&(target->next), ekey[ekey_offset + run_len]);
				if (next)
				{
					target_ptr_node = target;
					target_ptr_chr = ekey[ekey_offset + run_len];
					target = next;
					ekey_offset += run_len + 1;
					continue;
				}
			}
		}
		
		target_offset += i;
		ekey_offset += i;
		break;
	}
	
	//Graft the starting point for the new branch
	DictNode *start_node;
	if (target_offset == target->len)
	{
		start_node = target;
	}
	else
	{
		mdsl_assert(target != &(dict->root),
				"Assertion failure (cannot split root node)");
		//Splice target, second part --> start_node
		DictNode *p1 = alloc_node(target->ekey, target_offset);
		DictNode *p2 = alloc_node(target->ekey + target_offset + 1, 
				target->len - target_offset - 1);
		byte_map_set(&(p1->next), target->ekey[target_offset], p2);
		start_node = p1;
		p2->next = target->next;
		p2->value = target->value;
		byte_map_set(&(target_ptr_node->next), target_ptr_chr, p1);
		start_node = p1;
		free(target);
	}

	//Grow the new branch
	DictNode *res = start_node;

	while (ekey_offset < key_len)
	{
		int run_len = key_len - ekey_offset;
		if (run_len > MAX_NODE_LEN + 1)
			run_len = MAX_NODE_LEN + 1;

		DictNode *ext = alloc_node(ekey + ekey_offset + 1, run_len - 1);
		byte_map_set(&(res->next), ekey[ekey_offset], ext);
		res = ext;
		ekey_offset += run_len;
	}

	return res;
}

static void *collect_node(MdslDict *dict, const void *key, size_t key_len)
{
	const uint8_t *ekey = (const uint8_t *) key;
	const uint8_t *lkey = ekey + key_len;
	void *res = NULL;
	DictNodeArray array[1];
	dict_node_array_init(array);

	DictNode *iter = &(dict->root);

	//Find the node and set the value
	while (iter)
	{
		int remains = lkey - ekey;
		int run_len = iter->len;
		if (remains < run_len)
		{
			iter = NULL;
			break;
		}

		int i;
		for (i = 0; i < run_len; i++)
		{
			if (ekey[i] != iter->ekey[i])
				break;
		}
		if (i < run_len)
		{
			iter = NULL;
			break;
		}

		dict_node_array_append(array, iter);
		if (remains == run_len)
		{
			res = iter->value;
			iter->value = NULL;
			break;
		}
		else
		{
			iter = byte_map_get(&(iter->next), ekey[run_len]);
			ekey += run_len + 1;
		}
	}

	ekey = (const uint8_t *) key;
	int array_size = dict_node_array_size(array);
	while (iter)
	{
		DictNode *ptr_node;
		uint8_t ptr_chr;
		int reduce_len = iter->len + 1;
		if (array_size > 1)
		{
			ptr_node = array->data[array_size - 2];
			ptr_chr = ekey[key_len - reduce_len];
		}
		else
		{
			//Do not touch root node
			break;
		}

		mdsl_assert(ptr_node, "TMP: Assertion failure");

		//Coalesc forwards if possible
		do 
		{
			//Check if coalescing can be done
			if (iter->value)
				break;

			if (byte_map_get_size(&(iter->next)) != 1)
				break;

			uint8_t chr;
			DictNode *next;
			byte_map_get_tuples(&(iter->next), &chr, (void **) &next);

			int buf_len = iter->len + 1 + next->len;
			if (buf_len > MAX_NODE_LEN)
				break;

			//Do the coalescing
			uint8_t buf[MAX_NODE_LEN];

			memcpy(buf, iter->ekey, iter->len);
			buf[iter->len] = chr;
			memcpy(buf + iter->len + 1, next->ekey, next->len);

			DictNode *nn = alloc_node(buf, buf_len);
			nn->next = next->next;
			nn->value = next->value;

			//Amend the structure to replace the old nodes
			byte_map_set(&(ptr_node->next), ptr_chr, nn);

			byte_map_clear(&(iter->next));
			free(iter);
			free(next);
			iter = nn;
			array->data[array_size - 1] = nn;

		} while(0);

		//Check if this node should be freed
		if ((iter->value) || byte_map_get_size(&(iter->next)) != 0)
			break;

		//Remove useless node	
		byte_map_clear(&(iter->next));
		free(iter);

		//Correct data structures
		byte_map_set(&(ptr_node->next), ptr_chr, NULL);

		array_size--; 
		if (array_size == 0)
			break;
		iter = array->data[array_size - 1];
		key_len -= reduce_len;
	}


	free(array->data);
	return res;
}

void *mdsl_dict_set
	(MdslDict *dict, const void *key, size_t key_len, void *value)
{
	if (value)
	{
		DictNode *node = create_node(dict, key, key_len);
		void *old_value = node->value;
		node->value = value;
		return old_value;
	}
	else
	{
		return collect_node(dict, key, key_len);
	}
}

void *mdsl_dict_get
	(MdslDict *dict, const void *key, size_t key_len)
{
	const uint8_t *ekey = (const uint8_t *) key;
	const uint8_t *lkey = ekey + key_len;

	DictNode *iter = &(dict->root);

	while (iter)
	{
		int remains = lkey - ekey;
		int run_len = iter->len;
		if (remains < run_len)
		{
			return NULL;
		}

		int i;
		for (i = 0; i < run_len; i++)
		{
			if (ekey[i] != iter->ekey[i])
				return NULL;
		}

		if (remains == run_len)
		{
			return iter->value;
		}
		else
		{
			iter = byte_map_get(&(iter->next), ekey[run_len]);
			ekey += run_len + 1;
		}
	}

	return NULL;
}

static void mdsl_dict_destroy(MdslDict *dict)
{
	DictNodeArray stack[1];

	dict_node_array_init(stack);

	dict_node_array_append(stack, &(dict->root));

	while (dict_node_array_size(stack) > 0)
	{
		DictNode *node = dict_node_array_pop(stack);

		uint8_t keys[256];
		DictNode *values[256];
		int n_tuples = byte_map_get_tuples
			(&(node->next), keys, (void **) values);

		int i;
		for (i = 0; i < n_tuples; i++)
			dict_node_array_append(stack, values[i]);

		byte_map_clear(&(node->next));
		if (node != &(dict->root))
			free(node);
	}

	free(stack->data);
	free(dict);
}

MdslDict *mdsl_dict_new()
{
	MdslDict *dict = (MdslDict *) mdsl_alloc(sizeof(MdslDict));

	mdsl_rc_init(dict);

    dict->root.value = NULL;
    byte_map_init(&(dict->root.next));
    dict->root.len = 0;

	return dict;
}

//Debugging functions
static void mdsl_dict_dump_rec(DictNode *node, int level, FILE *stream)
{
	int i, j;
	fprintf(stream, "<%d|", node->len);
	for (i = 0; i < node->len; i++)
		fprintf(stream, "%c", node->ekey[i]);
	fprintf(stream, "|%p|%p>\n", node, node->value);

	uint8_t keys[256];
	DictNode *values[256];
	int n_tuples = byte_map_get_tuples
		(&(node->next), keys, (void **) values);

	for (i = 0; i < n_tuples; i++)
	{
		for (j = 0; j <= level; j++)
			fprintf(stream, "  ");
		fprintf(stream, "[%c]", keys[i]);
		mdsl_dict_dump_rec(values[i], level + 1, stream);
	}
}

void mdsl_dict_fdump(MdslDict *dict, FILE *stream)
{
	fprintf(stream, "[#]");
	mdsl_dict_dump_rec(&(dict->root), 0, stream);
}

void mdsl_dict_dump(MdslDict *dict)
{
	mdsl_debug("Dumping tree %p", dict);
	mdsl_dict_fdump(dict, stderr);
}


