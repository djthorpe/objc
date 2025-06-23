#include <stdio.h>
#include <objc/objc.h>
#include "api.h"

///////////////////////////////////////////////////////////////////////////////

void __objc_method_list_register(struct objc_method_list_t *p) {
    printf("__objc_method_list_register %p\n",p);
    if (p == NULL) {
        return; // Nothing to register
    }
    for (int i=0 ; i<p->count ; i++)
	{
        // TODO
	}
}
