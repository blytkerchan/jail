/* Modifications for use in Jail: Just Another Interpreted Language
 * Copyright (c) 2003-2004, Ronald Landheer-Cieslak
 * All rights reserved
 * 
 * This is free software. You may distribute it and/or modify it and
 * distribute modified forms provided that the following terms are met:
 *
 * * Redistributions of the source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer;
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the distribution;
 * * None of the names of the authors of this software may be used to endorse
 *   or promote this software, derived software or any distribution of this 
 *   software or any distribution of which this software is part, without 
 *   prior written permission from the authors involved;
 * * Unless you have received a written statement from Ronald Landheer-Cieslak
 *   that says otherwise, the terms of the GNU General Public License, as 
 *   published by the Free Software Foundation, version 2 or (at your option)
 *   any later version, also apply.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __G_HASH_H__
#define __G_HASH_H__

/* rlc -- we need a declaration for NULL */
#include <stdlib.h>
/* rlc -- the gtypes header is in the same directory as this one and neither
 *        will be installed.. */
#include "gtypes.h"

/* rlc -- don't want this
G_BEGIN_DECLS */

typedef struct _GHashTable  GHashTable;

typedef gboolean  (*GHRFunc)  (gpointer  key,
                               gpointer  value,
                               gpointer  user_data);

/* Hash tables
 */
GHashTable* g_hash_table_new		   (GHashFunc	    hash_func,
					    GEqualFunc	    key_equal_func);
GHashTable* g_hash_table_new_full      	   (GHashFunc	    hash_func,
					    GEqualFunc	    key_equal_func,
					    GDestroyNotify  key_destroy_func,
					    GDestroyNotify  value_destroy_func);
void	    g_hash_table_destroy	   (GHashTable	   *hash_table);
void	    g_hash_table_insert		   (GHashTable	   *hash_table,
					    gpointer	    key,
					    gpointer	    value);
void        g_hash_table_replace           (GHashTable     *hash_table,
					    gpointer	    key,
					    gpointer	    value);
gboolean    g_hash_table_remove		   (GHashTable	   *hash_table,
					    gconstpointer   key);
gboolean    g_hash_table_steal             (GHashTable     *hash_table,
					    gconstpointer   key);
gpointer    g_hash_table_lookup		   (GHashTable	   *hash_table,
					    gconstpointer   key);
gboolean    g_hash_table_lookup_extended   (GHashTable	   *hash_table,
					    gconstpointer   lookup_key,
					    gpointer	   *orig_key,
					    gpointer	   *value);
void	    g_hash_table_foreach	   (GHashTable	   *hash_table,
					    GHFunc	    func,
					    gpointer	    user_data);
guint	    g_hash_table_foreach_remove	   (GHashTable	   *hash_table,
					    GHRFunc	    func,
					    gpointer	    user_data);
guint	    g_hash_table_foreach_steal	   (GHashTable	   *hash_table,
					    GHRFunc	    func,
					    gpointer	    user_data);
guint	    g_hash_table_size		   (GHashTable	   *hash_table);

#ifndef G_DISABLE_DEPRECATED

/* The following two functions are deprecated and will be removed in
 * the next major release. They do no good. */
#define g_hash_table_freeze(hash_table) ((void)0)
#define g_hash_table_thaw(hash_table) ((void)0)

#endif /* G_DISABLE_DEPRECATED */

/* Hash Functions
 */
gboolean g_str_equal (gconstpointer  v,
                      gconstpointer  v2);
guint    g_str_hash  (gconstpointer  v);

gboolean g_int_equal (gconstpointer  v,
                      gconstpointer  v2);
guint    g_int_hash  (gconstpointer  v);

/* This "hash" function will just return the key's address as an
 * unsigned integer. Useful for hashing on plain addresses or
 * simple integer values.
 * Passing NULL into g_hash_table_new() as GHashFunc has the
 * same effect as passing g_direct_hash().
 */
guint    g_direct_hash  (gconstpointer  v) ;
gboolean g_direct_equal (gconstpointer  v,
                         gconstpointer  v2) ;

/* rlc -- don't want this
G_END_DECLS */

#endif /* __G_HASH_H__ */

