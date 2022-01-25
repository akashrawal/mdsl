
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

#include "incl.h"

static void mdsl_event_dispose_lazy(MdslEventBase *event)
{
	event->next->prev = event->prev;
	event->prev->next = event->next;
}

static void mdsl_event_add(MdslEventBase* event, MdslEventBase* el)
{
	el->prev = event;
	el->next = event->next;
	event->next->prev = el;
	event->next = el;
}

void mdsl_event_init(MdslEventBase* event)
{
	event->type = MDSL_EVENT_TYPE_PUBLISHER;
	event->next = event;
	event->prev = event;
}

void mdsl_event_dispose(MdslEventBase* event)
{
	if (event->next != event)
	{
		mdsl_event_dispose_lazy(event);
		mdsl_event_init(event);
	}
}

void mdsl_event_subscribe(MdslEventBase* event, MdslEventBase* subscriber)
{
	subscriber->type = MDSL_EVENT_TYPE_SUBSCRIBER;
	mdsl_event_add(event, subscriber);
}

void mdsl_event_begin(MdslEventBase *event, MdslEventBase* iter)
{
	iter->type = MDSL_EVENT_TYPE_ITERATOR;
	mdsl_event_add(event, iter);
}

MdslEventBase* mdsl_event_next(MdslEventBase* iter)
{
	while (1)
	{
		MdslEventBase* next = iter->next;
		if (next->type != MDSL_EVENT_TYPE_PUBLISHER)
		{
			mdsl_event_dispose_lazy(iter);
			mdsl_event_add(next, iter);
			if (next->type != MDSL_EVENT_TYPE_ITERATOR)
				return next;
		}
		else
		{
			return NULL;
		}
	}
}
