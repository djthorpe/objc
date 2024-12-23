#include <stdint.h>
#include <stdio.h>
#include <objc/runtime.h>

uint8_t _objc_empty_cache = 0;

id objc_msgSend_impl(id receiver, SEL selector) {
    //Class cls = receiver->isa;
    printf("receiver=%p selector=%p \n",receiver, selector);

    // Return nil for now
    return nil;
}
