#ifdef DEBUG
#include <stdio.h>
#endif
#include <string.h>

#include "api.h"
#include "protocol.h"
#include <objc/objc.h>

#define PROTOCOL_TABLE_SIZE 32
objc_protocol_t *protocol_table[PROTOCOL_TABLE_SIZE + 1];

///////////////////////////////////////////////////////////////////////////////

void __objc_protocol_init() {
  static BOOL init = NO;
  if (init) {
    return; // Already initialized
  }
  init = YES;

  for (int i = 0; i <= PROTOCOL_TABLE_SIZE; i++) {
    protocol_table[i] = NULL;
  }
}

void __objc_protocol_register(objc_protocol_t *p) {
  if (p == NULL || p->name == NULL) {
    return;
  }
#ifdef DEBUG
  printf("__objc_protocol_register <%s>\n", p->name);
#endif
  for (int i = 0; i < PROTOCOL_TABLE_SIZE; i++) {
    if (protocol_table[i] == p || protocol_table[i] == NULL) {
      protocol_table[i] = p;
      return;
    }
    if (strcmp(protocol_table[i]->name, p->name) == 0) {
      panicf("Duplicate protocol named: %s", p->name);
    }
  }
  panicf("Protocol table is full, cannot register protocol: %s", p->name);
}

void __objc_protocol_list_register(struct objc_protocol_list *list) {
  if (list == NULL) {
    return; // Nothing to register
  }
  for (size_t i = 0; i < list->count; i++) {
    objc_protocol_t *protocol = list->protocols[i];
    if (protocol != NULL && protocol->name != NULL) {
      __objc_protocol_register(protocol);
    }
  }
  if (list->next != NULL) {
    __objc_protocol_list_register(list->next); // Register next protocol list
  }
}
