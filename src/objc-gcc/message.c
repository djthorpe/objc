#include <stdio.h>
#include <objc/objc.h>
#include "api.h"

///////////////////////////////////////////////////////////////////////////////

IMP objc_msg_lookup(id receiver, SEL selector) {
    if (receiver == NULL) {
        return NULL;
    }
    printf("objc_msg_lookup: %p %p\n",receiver, selector);
    return NULL;
}
