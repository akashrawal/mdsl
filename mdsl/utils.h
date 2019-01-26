/* utils.h
 * Internal utilities
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
 * \addtogroup mdsl_utils
 * \{
 * 
 * This section documents a collection of important utility functions.
 */
 
//Failable operations
typedef enum
{
	MDSL_SUCCESS = 0,
	MDSL_FAILURE = -1
} MdslStatus;

//Adds integer to pointer
#define MDSL_PTR_ADD(ptr, bytes) ((void *) (((int8_t *) (ptr)) + (bytes)))

#define mdsl_encl_struct(ptr, type, member) \
	((type *) (MDSL_PTR_ADD((ptr), -(offsetof(type, member)))))

#define MDSL_VAR_ARRAY_SIZE 2

//Errors
#ifdef __GNUC__

#define mdsl_context_error(context, ...) \
	do { \
		fprintf(stderr, context ": %s: %s:%d: ERROR:", \
				__PRETTY_FUNCTION__, __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		mdsl_warn_break(1); \
	} while (0)

#define mdsl_context_warn(context, ...) \
	do { \
		fprintf(stderr, context ": %s: %s:%d: WARNING:", \
				__PRETTY_FUNCTION__, __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		mdsl_warn_break(0); \
	} while (0)

#define mdsl_context_debug(context, ...) \
	do { \
		fprintf(stderr, context ": %s: %s:%d: DEBUG:", \
				__PRETTY_FUNCTION__, __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		mdsl_warn_break(0); \
	} while (0)

#define mdsl_context_assert(context, expr, ...) \
	do { \
		if (! (expr)) \
		{ \
			fprintf(stderr, context ": %s: %s:%d: ERROR:", \
					__PRETTY_FUNCTION__, __FILE__, __LINE__); \
			fprintf(stderr, __VA_ARGS__); \
			fprintf(stderr, "\nFailed assertion: [" #expr "]\n"); \
			mdsl_warn_break(1); \
		} \
	} while (0)

#else

#define mdsl_context_error(context, ...) \
	do { \
		fprintf(stderr, context ": %s:%d: ERROR:", __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		mdsl_warn_break(1); \
	} while (0)

#define mdsl_context_warn(context, ...) \
	do { \
		fprintf(stderr, context ": %s:%d: WARNING:", __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		mdsl_warn_break(0); \
	} while (0)

#define mdsl_context_debug(context, ...) \
	do { \
		fprintf(stderr, context ": %s:%d: DEBUG:", __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		mdsl_warn_break(0); \
	} while (0)

#define mdsl_context_assert(context, expr, ...) \
	do { \
		if (! (expr)) \
		{ \
			fprintf(stderr, context ": %s:%d: ERROR:", \
					__FILE__, __LINE__); \
			fprintf(stderr, __VA_ARGS__); \
			fprintf(stderr, " \nFailed assertion [" #expr "]\n"); \
			mdsl_warn_break(1); \
		} \
	} while (0)

#endif	

#define mdsl_error(...) mdsl_context_error("MDSL", __VA_ARGS__)
#define mdsl_warn(...) mdsl_context_warn("MDSL", __VA_ARGS__)
#define mdsl_debug(...) mdsl_context_debug("MDSL", __VA_ARGS__)
#define mdsl_assert(expr, ...) mdsl_context_assert("MDSL", expr, __VA_ARGS__)

/**If you want to break at an error or warning from Modular Middleware
 * use debugger to break at this function.
 * 
 * \param to_abort In case of error its value is 1, else it is 0.
 */
void mdsl_warn_break(int to_abort);

//Allocation
#define mdsl_tryalloc malloc
#define mdsl_tryrealloc realloc
#define mdsl_free free

/**Allocates memory of size bytes. 
 * If memory allocation fails the program is aborted.
 * 
 * This function uses malloc() internally.
 * \param size Number of bytes to allocate
 * \return Newly allocated memory, free with free()
 */
void *mdsl_alloc(size_t size);

/**Reallocates memory old_mem to size bytes. 
 * If memory allocation fails program is aborted.
 * 
 * This function uses realloc() internally.
 * \param old_mem Original memory to resize
 * \param size Number of bytes to allocate
 * \return Newly allocated memory, free with free()
 */
void *mdsl_realloc(void *old_mem, size_t size);

/**Allocates two memory blocks of size1 and size2 bytes. 
 * If memory allocation fails the program is aborted.
 * 
 * \param size1 Number of bytes to allocate for first memory block
 * \param size2 Number of bytes to allocate for second memory block
 * \param mem2_return Return location for second memory block
 * \return First memory block, free with free() to free both memory blocks.
 */
void *mdsl_alloc2(size_t size1, size_t size2, void **mem2_return);

/**Allocates two memory blocks of size1 and size2 bytes. 
 * If memory allocation fails NULL is returned and mem2_return 
 * argument is unmodified.
 * \param size1 Number of bytes to allocate for first memory block
 * \param size2 Number of bytes to allocate for second memory block
 * \param mem2_return Return location for second memory block
 * \return First memory block, free with free() to free both memory blocks.
 */
void *mdsl_tryalloc2(size_t size1, size_t size2, void **mem2_return);

#define mdsl_new(type) ((type *) mdsl_alloc(sizeof(type)))

//TODO: Check if this is correct
#define mdsl_alloc_boundary (2 * sizeof(void *))

#define mdsl_offset_align(offset) \
	(((offset) + mdsl_alloc_boundary - 1) \
	- (((offset) + mdsl_alloc_boundary - 1) % mdsl_alloc_boundary))

/**Allocates a new memory and copies the given string into it.
 * If memory allocation fails the program is aborted.
 * 
 * \param str the string to copy
 * \return Newly allocated string. free with free().
 */
char *mdsl_strdup(const char *str);

/**Allocates a new memory, copies the contents of _mem_ and returns it.
 * If memory allocation fails the program is aborted.
 * 
 * \param mem The memory to be copied
 * \param len The number of bytes to be copied
 * \return newly allocated memory containing the data from _mem_,
 *         free with free().
 */
void *mdsl_memdup(const void *mem, size_t len);

//Template code for reference counting
typedef struct 
{
	int refcount;
} MdslRC;

#define mdsl_rc_declare(TypeName, type_name) \
	void type_name ## _ref(TypeName *object); \
	void type_name ## _unref(TypeName *object); \
	int type_name ## _get_refcount(TypeName *object)

#define mdsl_rc_define(TypeName, type_name) \
	static void type_name ## _destroy(TypeName *object); \
	void type_name ## _ref(TypeName *object) \
	{ \
		MdslRC *x = (MdslRC *) object; \
		 \
		x->refcount++; \
	} \
	void type_name ## _unref(TypeName *object) \
	{ \
		MdslRC *x = (MdslRC *) object; \
		 \
		x->refcount--; \
		if (x->refcount <= 0) \
		{ \
			type_name ## _destroy(object); \
		} \
	} \
	int type_name ## _get_refcount(TypeName *object) \
	{ \
		MdslRC *x = (MdslRC *) object; \
		 \
		return x->refcount; \
	} \
	typedef int MdslRcDefineTemplateEnd ## TypeName;

#define mdsl_rc_init(object) \
	do { \
		MdslRC *x = (MdslRC *) object; \
		x->refcount = 1; \
	} while (0)




/**
 * \}
 */
