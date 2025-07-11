#pragma once

#include "api.h"
#include <objc/objc.h>

///////////////////////////////////////////////////////////////////////////////////
// TYPES

typedef struct objc_protocol objc_protocol_t;

///////////////////////////////////////////////////////////////////////////////////
// METHODS

/*
 * Initializes the Objective-C runtime protocol table
 */
void __objc_protocol_init();

/*
 * Register a protocol in the Objective-C runtime.
 */
void __objc_protocol_register(objc_protocol_t *protocol);

/*
 * Register protocols from a list of protocols
 */
void __objc_protocol_list_register(struct objc_protocol_list *list);
