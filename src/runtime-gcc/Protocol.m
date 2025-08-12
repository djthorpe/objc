#include <objc/objc.h>

@implementation Protocol

/**
 * @brief Returns the name of the protocol.
 */
- (const char *)name {
  return _name;
}

/**
 * @brief Returns the class name of the protocol.
 * @return A C string containing the protocol class name.
 */
+ (const char *)name {
  return "Protocol";
}

/**
 * @brief Checks if this protocol conforms to another protocol.
 */
- (BOOL)conformsTo:(Protocol *)aProtocolObject {
  return proto_conformsTo((objc_protocol_t *)self,
                          (objc_protocol_t *)aProtocolObject);
}

/**
 * @brief Determines if this protocol is equal to another object.
 */
- (BOOL)isEqual:(id)anObject {
  if (self == anObject) {
    return YES;
  }
  if (object_getClass(self) != object_getClass(anObject)) {
    return NO;
  }
  Protocol *otherProtocol = (Protocol *)anObject;
  return sys_strcmp([self name], [otherProtocol name]) == 0;
}

@end
