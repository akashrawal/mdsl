/* event.c
 * Unit tests for event ring implementaion
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

#include <stdio.h>

#include <mdsl/mdsl.h>

#define testcase(x) \
	do { \
		fprintf(stderr, "Test " #x "\n"); \
		x; \
	} while (0)


//Test support functions
typedef struct
{
	MdslEventBase parent;
	int value;
} TestSubscriber;

#define MAX_SUBSCRIBERS 10
TestSubscriber subscribers[MAX_SUBSCRIBERS];
int visit_id;
void init_test() {
	int i;
	visit_id = 0;
	for (i = 0; i < MAX_SUBSCRIBERS; i++)
		subscribers[i].value = -1;
}

void visit(TestSubscriber *sub) {
	sub->value = visit_id++;
}

void check_value(int idx, int value) {
	mdsl_assert(subscribers[idx].value == value,
			"Assertion failure, idx=%d, LHS=%d, RHS=%d",
			idx, subscribers[idx].value, value);
}

void check_counter(int value) {
	mdsl_assert(visit_id == value,
			"Assertion failure, LHS=%d, RHS=%d",
			visit_id, value);
}

//Test cases
void test_empty() {
	MdslEventBase ring[1];
	MdslEventBase iter[1];
	TestSubscriber *cur;

	init_test();

	mdsl_event_init(ring);

	mdsl_event_begin(ring, iter);	
	while ((cur = (TestSubscriber*) mdsl_event_next(iter)))
		visit(cur);
	mdsl_event_dispose(iter);
	
	check_value(0, -1);
	check_counter(0);
}

void test_add() {
	MdslEventBase ring[1];
	MdslEventBase iter[1];
	TestSubscriber *cur;

	init_test();

	mdsl_event_init(ring);
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 0));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 1));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 2));

	mdsl_event_begin(ring, iter);	
	while ((cur = (TestSubscriber*) mdsl_event_next(iter)))
	{
		mdsl_debug("offset = %d", (int) (cur - subscribers));
		visit(cur);
	}
	mdsl_event_dispose(iter);
	
	check_value(0, 2);
	check_value(1, 1);
	check_value(2, 0);
	check_value(3, -1);
	check_counter(3);
}

void test_remove() {
	MdslEventBase ring[1];
	MdslEventBase iter[1];
	TestSubscriber *cur;

	init_test();

	mdsl_event_init(ring);
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 0));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 1));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 2));
	mdsl_event_dispose((MdslEventBase*) (subscribers + 1));

	mdsl_event_begin(ring, iter);	
	while ((cur = (TestSubscriber*) mdsl_event_next(iter)))
	{
		mdsl_debug("offset = %d", (int) (cur - subscribers));
		visit(cur);
	}
	mdsl_event_dispose(iter);
	
	check_value(0, 1);
	check_value(1, -1);
	check_value(2, 0);
	check_value(3, -1);
	check_counter(2);
}

void test_remove_iter() {
	MdslEventBase ring[1];
	MdslEventBase iter[1];
	TestSubscriber *cur;

	init_test();

	mdsl_event_init(ring);
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 0));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 1));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 2));

	mdsl_event_begin(ring, iter);	
	cur = (TestSubscriber*) mdsl_event_next(iter);
	visit(cur);
	mdsl_event_dispose((MdslEventBase*) (subscribers + 1));

	while ((cur = (TestSubscriber*) mdsl_event_next(iter)))
	{
		mdsl_debug("offset = %d", (int) (cur - subscribers));
		visit(cur);
	}
	mdsl_event_dispose(iter);
	
	check_value(0, 1);
	check_value(1, -1);
	check_value(2, 0);
	check_value(3, -1);
	check_counter(2);
}

void test_add_iter() {
	MdslEventBase ring[1];
	MdslEventBase iter[1];
	TestSubscriber *cur;

	init_test();

	mdsl_event_init(ring);
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 0));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 1));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 2));

	mdsl_event_begin(ring, iter);	
	cur = (TestSubscriber*) mdsl_event_next(iter);
	visit(cur);
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 3));

	while ((cur = (TestSubscriber*) mdsl_event_next(iter)))
	{
		mdsl_debug("offset = %d", (int) (cur - subscribers));
		visit(cur);
	}
	mdsl_event_dispose(iter);
	
	check_value(0, 2);
	check_value(1, 1);
	check_value(2, 0);
	check_value(3, -1);
	check_counter(3);
}

void test_double_iter() {
	MdslEventBase ring[1];
	MdslEventBase iter[1], iter2[1];
	TestSubscriber *cur;

	init_test();

	mdsl_event_init(ring);
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 0));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 1));
	mdsl_event_subscribe(ring, (MdslEventBase*) (subscribers + 2));

	mdsl_event_begin(ring, iter2);	
	mdsl_event_next(iter2);

	mdsl_event_begin(ring, iter);	
	while ((cur = (TestSubscriber*) mdsl_event_next(iter)))
	{
		mdsl_debug("offset = %d", (int) (cur - subscribers));
		visit(cur);
	}
	mdsl_event_dispose(iter);
	
	check_value(0, 2);
	check_value(1, 1);
	check_value(2, 0);
	check_value(3, -1);
	check_counter(3);
}


int main()
{
	testcase(test_empty());
	testcase(test_add());
	testcase(test_remove());
	testcase(test_remove_iter());
	testcase(test_add_iter());
	testcase(test_double_iter());

	return 0;
}
