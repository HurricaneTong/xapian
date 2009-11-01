/* chert_alltermslist.h: A termlist containing all terms in a chert database.
 *
 * Copyright (C) 2005,2007,2008,2009 Olly Betts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#ifndef XAPIAN_INCLUDED_CHERT_ALLTERMSLIST_H
#define XAPIAN_INCLUDED_CHERT_ALLTERMSLIST_H

#include "alltermslist.h"
#include "chert_database.h"
#include "chert_postlist.h"

class ChertCursor;

class ChertAllTermsList : public AllTermsList {
    /// Copying is not allowed.
    ChertAllTermsList(const ChertAllTermsList &);

    /// Assignment is not allowed.
    void operator=(const ChertAllTermsList &);

    /// Keep a reference to our database to stop it being deleted.
    Xapian::Internal::RefCntPtr<const ChertDatabase> database;

    /** A cursor which runs through the postlist table reading termnames from
     *  the keys.
     */
    ChertCursor * cursor;

    /// The termname at the current position.
    string current_term;

    /// The prefix to restrict the terms to.
    string prefix;

    /** The term frequency of the term at the current position.
     *
     *  If this value is zero, then we haven't read the term frequency or
     *  collection frequency for the current term yet.  We need to call
     *  read_termfreq_and_collfreq() to read these.
     */
    mutable Xapian::doccount termfreq;

    /// The collection frequency of the term at the current position.
    mutable Xapian::termcount collfreq;

    /// Read and cache the term frequency and collection frequency.
    void read_termfreq_and_collfreq() const;

  public:
    ChertAllTermsList(Xapian::Internal::RefCntPtr<const ChertDatabase> database_,
		      const string & prefix_)
	    : database(database_), prefix(prefix_), termfreq(0) {
	cursor = database->postlist_table.cursor_get();
	Assert(cursor); // The postlist table isn't optional.

	// Position the cursor on the highest key before the first key we want,
	// so that the first call to next() will put us on the first key we
	// want.
	if (prefix.empty()) {
	    cursor->find_entry_lt(string("\x00\xff", 2));
	} else {
	    cursor->find_entry_lt(pack_string_preserving_sort(prefix));
	}
    }

    /// Destructor.
    ~ChertAllTermsList();

    /** Returns the current termname.
     *
     *  Either next() or skip_to() must have been called before this
     *  method can be called.
     */
    string get_termname() const;

    /** Returns the term frequency of the current term.
     *
     *  Either next() or skip_to() must have been called before this
     *  method can be called.
     */
    Xapian::doccount get_termfreq() const;

    /** Returns the collection frequency of the current term.
     *
     *  Either next() or skip_to() must have been called before this
     *  method can be called.
     */
    Xapian::termcount get_collection_freq() const;

    /// Advance to the next term in the list.
    TermList * next();

    /// Advance to the first term which is >= tname.
    TermList * skip_to(const string &tname);

    /// True if we're off the end of the list
    bool at_end() const;
};

#endif /* XAPIAN_INCLUDED_CHERT_ALLTERMSLIST_H */