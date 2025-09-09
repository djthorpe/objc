#pragma once
#include "api.h"

///////////////////////////////////////////////////////////////////////////////////////////
// Dispatch Table

/*
 * Create a new dispatch table for the class
 */
bool _objc_dtable_init(Class cls);

/*
 * Check whether an IMP exists in the dispatch table
 */
bool _objc_dtable_exists(Class cls, void *sel_id);

/*
 * Add an IMP to the dispatch table (linear probing). No deletes supported.
 */
bool _objc_dtable_add(Class cls, void *sel_id);
