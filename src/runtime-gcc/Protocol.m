#include <objc/objc.h>

@implementation Protocol

/**
 * @brief Returns the name of the protocol.
 */
- (const char *)name {
  return _name;
}

/**
 * @brief Checks if this protocol conforms to another protocol.
 */
- (BOOL)conformsTo:(Protocol *)aProtocolObject {
  return proto_conformsTo((objc_protocol_t *)self,
                          (objc_protocol_t *)aProtocolObject);
}

@end
