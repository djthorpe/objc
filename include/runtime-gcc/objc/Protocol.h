/**
 * @file Protocol.h
 * @brief Objective-C protocols definition.
 *
 * This file provides the definition of the Protocol class, which is used to
 * declare and implement Objective-C protocols.
 */
#pragma once

@interface Protocol : Object {
@private
  const char *protocol_name;
  struct objc_protocol_list *protocol_list;
  struct objc_method_list *instance_methods;
  struct objc_method_list *class_methods;
}

@end
