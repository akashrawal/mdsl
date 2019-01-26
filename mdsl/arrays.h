/* arrays.h
 * Array functions
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

/**
 * \addtogroup mdsl_arrays
 * \{
 * 
 * Easy to use array templates
 */

//Resizable buffer
#define MDSL_RBUF_MIN_LEN 8

typedef struct
{
	char *data;
	size_t len, alloc_len;
} MdslRBuf;

/**Initializes a resizable buffer
 * \param rbuf Pointer to structure to initialize
 */
void mdsl_rbuf_init(MdslRBuf *rbuf);

/**Resizes a resizable buffer
 * \param rbuf Resizable buffer
 * \param new_len Nes length
 */
void mdsl_rbuf_resize(MdslRBuf *rbuf, size_t new_len);

/**Copies data to end of the buffer, expanding it to accomodate
 * both existing and new content.
 * \param rbuf Resizable buffer
 * \param data Pointer to memory to copy; at least _len_ bytes should be 
 *             addressable
 * \param len  Length
 */
void mdsl_rbuf_append(MdslRBuf *rbuf, const void *data, size_t len);
void mdsl_rbuf_append1(MdslRBuf *rbuf, char val);

//Template for dynamic arrays
#define mdsl_declare_array(TypeName, ArrayTypeName, array_type_name) \
typedef union { MdslRBuf parent; TypeName *data; } ArrayTypeName; \
static inline void array_type_name ## _init(ArrayTypeName *array) \
{ \
	mdsl_rbuf_init((MdslRBuf *) array); \
} \
static inline void array_type_name ## _resize \
	(ArrayTypeName *array, size_t new_len) \
{ \
	mdsl_rbuf_resize((MdslRBuf *) array, new_len * sizeof(TypeName)); \
} \
static inline void array_type_name ## _append \
	(ArrayTypeName *array, TypeName data) \
{ \
	mdsl_rbuf_append((MdslRBuf *) array, &data, sizeof(TypeName)); \
} \
static inline size_t array_type_name ## _size(ArrayTypeName *array) \
{ \
	return ((MdslRBuf *) array)->len / sizeof(TypeName); \
} \
static inline TypeName array_type_name ## _pop(ArrayTypeName *array) \
{ \
	size_t len = array_type_name ## _size(array); \
	if (len == 0) \
		mdsl_error("Cannot pop from empty stack"); \
	TypeName res = array->data[len - 1]; \
	array_type_name ## _resize(array, len - 1); \
	return res; \
} \
typedef int MdslDynamicArrayEnd ## ArrayTypeName

//Template for dynamic array queues
#define mdsl_declare_queue(TypeName, ArrayTypeName, array_type_name) \
typedef struct\
{\
	TypeName *data;\
	size_t start, len, alloc_len;\
} ArrayTypeName;\
static inline void array_type_name ## _init(ArrayTypeName *array)\
{\
	array->start = array->len = 0;\
	array->alloc_len = MDSL_RBUF_MIN_LEN;\
	array->data = mdsl_alloc(array->alloc_len * sizeof(TypeName));\
}\
static inline TypeName *array_type_name ## _head(ArrayTypeName *array)\
{\
	return array->data + array->start;\
}\
static inline size_t array_type_name ## _size(ArrayTypeName *array)\
{\
	return array->len;\
}\
static inline TypeName * array_type_name ## _alloc(ArrayTypeName *array)\
{\
	if (array->start + array->len >= array->alloc_len)\
	{\
		if (array->len >= (array->alloc_len / 2))\
		{\
			size_t new_alloc_len = array->alloc_len * 2;\
			TypeName *new_data = mdsl_alloc(sizeof(TypeName) * new_alloc_len);\
			memcpy(new_data, array->data + array->start, \
					array->len * sizeof(TypeName));\
			free(array->data);\
			array->data = new_data;\
			array->alloc_len = new_alloc_len;\
			array->start = 0;\
		}\
		else\
		{\
			memmove(array->data, array->data + array->start, \
					array->len * sizeof(TypeName));\
			array->start = 0;\
		}\
	}\
	TypeName *res = array->data + array->start + array->len;\
	array->len++;\
	return res;\
}\
static inline TypeName * array_type_name ## _alloc_n\
(ArrayTypeName *array, size_t n)\
{\
	if (array->start + array->len + n > array->alloc_len)\
	{\
		if (array->len + n > (array->alloc_len / 2))\
		{\
			size_t new_alloc_len = (array->len * 2) + n;\
			TypeName *new_data = mdsl_alloc(sizeof(TypeName) * new_alloc_len);\
			memcpy(new_data, array->data + array->start, \
					array->len * sizeof(TypeName));\
			free(array->data);\
			array->data = new_data;\
			array->alloc_len = new_alloc_len;\
			array->start = 0;\
		}\
		else\
		{\
			memmove(array->data, array->data + array->start, \
					array->len * sizeof(TypeName));\
			array->start = 0;\
		}\
	}\
	TypeName *res = array->data + array->start + array->len;\
	array->len += n;\
	return res;\
}\
static inline void array_type_name ## _push(ArrayTypeName *array, \
		TypeName element)\
{\
	*array_type_name ## _alloc(array) = element;\
}\
static inline TypeName array_type_name ## _pop(ArrayTypeName *array)\
{\
	if (array->len == 0)\
		mdsl_error("Cannot pop from empty queue");\
	TypeName res = array->data[array->start];\
	array->start++;\
	array->len--;\
	size_t new_alloc_len = array->len + MDSL_RBUF_MIN_LEN; \
	if (new_alloc_len < array->alloc_len / 4)\
	{\
		TypeName *new_data = mdsl_alloc(sizeof(TypeName) * new_alloc_len);\
		memcpy(new_data, array->data + array->start, \
				array->len * sizeof(TypeName));\
		free(array->data);\
		array->data = new_data;\
		array->alloc_len = new_alloc_len;\
		array->start = 0;\
	}\
	return res;\
}\
static inline void array_type_name ## _pop_n(ArrayTypeName *array, size_t n)\
{\
	if (array->len < n)\
		mdsl_error("Too few elements to pop from queue(%d from %d)", \
				n, array->len);\
	array->start += n;\
	array->len -= n;\
	size_t new_alloc_len = array->len + MDSL_RBUF_MIN_LEN; \
	if (new_alloc_len < array->alloc_len / 4)\
	{\
		TypeName *new_data = mdsl_alloc(sizeof(TypeName) * new_alloc_len);\
		memcpy(new_data, array->data + array->start, \
				array->len * sizeof(TypeName));\
		free(array->data);\
		array->data = new_data;\
		array->alloc_len = new_alloc_len;\
		array->start = 0;\
	}\
	return res;\
}\
static inline void array_type_name ## _destroy(ArrayTypeName *array)\
{\
	free(array->data);\
}\
typedef int MdslDynamicQueueEnd ## ArrayTypeName

/**
 * \}
 */
