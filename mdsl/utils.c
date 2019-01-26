/* utils.c
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

#include "incl.h"

//If you want to break at an error or warning break at this function.
void mdsl_warn_break(int to_abort)
{
	if (to_abort)
		abort();
}

//Memory allocation functions
void *mdsl_alloc(size_t size)
{
	if (size == 0)
		mdsl_error("Cannot allocate memory of zero bytes");
	void *mem = mdsl_tryalloc(size);
	if (! mem)
	{
		mdsl_error("Cannot allocate memory of %d bytes", (int) size);
	}
	return mem;
}

void *mdsl_realloc(void *old_mem, size_t size)
{
	if (size == 0)
		mdsl_error("Cannot allocate memory of zero bytes");
	void *mem = mdsl_tryrealloc(old_mem, size);
	if (! mem)
	{
		mdsl_error("Cannot allocate memory of %d bytes", (int) size);
	}
	return mem;
}

void *mdsl_alloc2(size_t size1, size_t size2, void **mem2_return)
{
	void *mem1;
	
	//mem2 must be aligned to size of two pointers.
	//Adjust size1 for that.
	size1 = mdsl_offset_align(size1);
	
	//Now allocate
	mem1 = mdsl_tryalloc(size1 + size2);
	if (! mem1)
	{
		mdsl_error("Cannot allocate memory of %d bytes", (int) (size1 + size2));
	}
	
	*mem2_return = MDSL_PTR_ADD(mem1, size1);
	return mem1;
}

void *mdsl_tryalloc2(size_t size1, size_t size2, void **mem2_return)
{
	void *mem1;
	
	//mem2 must be aligned to size of two pointers.
	//Adjust size1 for that.
	size1 = mdsl_offset_align(size1);
	
	//Now allocate
	mem1 = mdsl_tryalloc(size1 + size2);
	if (! mem1)
	{
		return NULL;
	}
	
	*mem2_return = MDSL_PTR_ADD(mem1, size1);
	return mem1;
}

char *mdsl_strdup(const char *str)
{
	char *res = strdup(str);
	if (!res)
	{
		mdsl_error("Memory allocation failed.");
	}
	return res;
}

void *mdsl_memdup(const void *mem, size_t len)
{
	void *res = mdsl_alloc(len);
	memcpy(res, mem, len);
	return res;
}

