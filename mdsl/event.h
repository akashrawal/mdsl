
/* event.h
 * Publish-subscribe mechanism
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

typedef struct _MdslEventBase MdslEventBase;

typedef enum
{
	MDSL_EVENT_TYPE_PUBLISHER,
	MDSL_EVENT_TYPE_SUBSCRIBER,
	MDSL_EVENT_TYPE_ITERATOR
} MdslEventType;

struct _MdslEventBase 
{
	MdslEventType type;
	MdslEventBase *next, *prev;
};

/**
 * Initializes an event structure.
 * An event structure must be initialized in order to use it as a publisher
 * element of an an empty event ring.
 *
 * \param event An uninitialized event structure
 */
void mdsl_event_init(MdslEventBase* event);

/**
 * Removes a subscriber/iterator from the event ring it is part of, if it is.
 * This function can be called multiple times on a given event structure.
 *
 * \param event An initialized event structure (event ring/subscriber/iterator)
 */
void mdsl_event_dispose(MdslEventBase* event);

/**
 * Adds a subscriber to an event ring. 
 *
 * \param event An initialized event structure representing an event ring
 * \param subscriber An uninitialized event structure representing
 *                   a subscriber
 */
void mdsl_event_subscribe(MdslEventBase* event, MdslEventBase* subscriber);

/**
 * Inserts an iterator in an event ring. 
 *
 * See mdsl_event_next
 *
 * \param event An initialized event structure representing an event ring.
 * \param iter An uninitialized event structure representing an iterator.
 */
void mdsl_event_begin(MdslEventBase *event, MdslEventBase* iter);

/**
 * Tries to advances an iterator in an event ring.
 *
 * The iterator must be inserted by calling mdsl_event_begin.
 *
 * Care must be taken if the process of visiting a subscriber invokes a
 * user provided callback function. The callback function can remove any
 * subscriber during the iteration process. If you need access to the element
 * being visited, you might need to back-up its contents. While multiple
 * iterators are supported by this function (mdsl_event_next will 
 * never return a pointer to an iterator.), you might need to ensure that
 * your iteration logic is re-enterant. Subscribers added to the event ring
 * after mdsl_event_begin is called on an iterator will not be visited by the
 * said iterator. Subscribers removed from the event ring while iteration is
 * in progress will not be visited, even when added back to the event ring.
 *
 * Once iteration is finished, mdsl_event_next will return NULL. The iterator
 * then must be removed by calling mdsl_event_dispose.
 *
 * \param iter An inserted event structure representing an iterator
 * \return A subscriber that should be visited, or NULL if there are 
 *         no more subscribers.
 */
MdslEventBase* mdsl_event_next(MdslEventBase* iter);

