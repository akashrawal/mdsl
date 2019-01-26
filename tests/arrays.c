/* arrays.c
 * Unit tests for resizable buffers and array templates
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

#include <stdio.h>

#include <mdsl/mdsl.h>

#define testcase(x) \
	do { \
		fprintf(stderr, "Test " #x "\n"); \
		x; \
	} while (0)

#define CAPACITY 1000

char tdata[CAPACITY];
void test_init()
{
	int i;

	tdata[0] = 0;
	tdata[1] = 1;

	for (i = 2; i < CAPACITY; i++)
		tdata[i] = tdata[i - 1] + tdata[i - 2];
}

void test_rbuf(int size1, int size2, int size3)
{
	MdslRBuf rbuf[1];
	mdsl_rbuf_init(rbuf);

	mdsl_rbuf_append(rbuf, tdata, size1);
	mdsl_rbuf_append(rbuf, tdata + size1, size2 - size1);
	mdsl_rbuf_resize(rbuf, size3);

	int i;
	for (i = 0; i < size3; i++)
	{
		mdsl_assert(rbuf->data[i] == tdata[i],
				"Data corrucption, i = %d", i);
	}

	mdsl_free(rbuf->data);
}

mdsl_declare_array(int, IntArray, int_array);

void test_stack(int msize)
{
	IntArray test_array[1];

	int_array_init(test_array);

	int i;
	for (i = 0; i < msize; i++)
		int_array_append(test_array, tdata[i]);

	for (i = 0; i < msize; i++)
	{
		mdsl_assert(test_array->data[i] == tdata[i],
				"Data corruption, i = %d", i);
	}

	for (i = msize - 1; i >= 0; i--)
	{
		int val = int_array_pop(test_array);
		mdsl_assert(val == tdata[i], 
				"Incorrect pop() operation; i=%d", i);
	}

	free(test_array->data);
}

int main()
{
	test_init();

	testcase(test_rbuf(128, 128, 64));
	testcase(test_rbuf(64, 128, 64));
	testcase(test_rbuf(32, 128, 64));
	testcase(test_rbuf(0, 128, 64));
	testcase(test_rbuf(32, 128, 32));
	testcase(test_rbuf(128, 128, 8));
	testcase(test_rbuf(128, 128, 0));

	testcase(test_stack(1));

	return 0;
}
