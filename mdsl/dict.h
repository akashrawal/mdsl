/* dict.h
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

typedef struct _MdslDict MdslDict;

void *mdsl_dict_set
	(MdslDict *dict, const void *key, size_t key_len, void *value);

void *mdsl_dict_get
	(MdslDict *dict, const void *key, size_t key_len);

MdslDict *mdsl_dict_new();

mdsl_rc_declare(MdslDict, mdsl_dict);

void mdsl_dict_dump(MdslDict *dict);
void mdsl_dict_fdump(MdslDict *dict, FILE *stream);
