/* arrays.c
 * Array functions
 * 
 * Copyright 2015-2020 Akash Rawal
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

//Resizable buffer

void mdsl_rbuf_init(MdslRBuf *rbuf)
{
	rbuf->len = 0;
	rbuf->alloc_len = MDSL_RBUF_MIN_LEN;
	rbuf->data = (char *) mdsl_alloc(rbuf->alloc_len);
}

void mdsl_rbuf_resize(MdslRBuf *rbuf, size_t new_len)
{
	if (rbuf->alloc_len < new_len)
	{
		rbuf->alloc_len = (2 * rbuf->alloc_len) + new_len;
		rbuf->data = (char *) mdsl_realloc(rbuf->data, rbuf->alloc_len);
	}
	else if ((rbuf->alloc_len / 4) > (new_len + MDSL_RBUF_MIN_LEN))
	{
		rbuf->alloc_len = new_len + MDSL_RBUF_MIN_LEN;
		rbuf->data = (char *) mdsl_realloc(rbuf->data, rbuf->alloc_len);
	}
	rbuf->len = new_len;
}

void mdsl_rbuf_append(MdslRBuf *rbuf, const void *data, size_t len)
{
	size_t orig_len = rbuf->len;
	mdsl_rbuf_resize(rbuf, orig_len + len);
	memcpy(rbuf->data + orig_len, data, len);
}

void mdsl_rbuf_append1(MdslRBuf *rbuf, char val)
{
	size_t orig_len = rbuf->len;
	mdsl_rbuf_resize(rbuf, orig_len + 1);
	rbuf->data[orig_len] = val;
}
