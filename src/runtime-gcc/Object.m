#include <objc/objc.h>
#include <stdlib.h>

@implementation Object

+ (void)initialize {
  // No-op
}

+ (id)alloc {
  id obj = (id)objc_malloc(class_getInstanceSize(self));
  if (obj) {
    object_setClass(obj, self);
  }
  return obj;
}

- (id)init {
  return self;
}

- (void)dealloc {
  objc_free(self);
}

- (Class)class {
  return object_getClass(self);
}

+ (Class)class {
  return self;
}

- (Class)superclass {
  return object_getSuperclass(self);
}

+ (Class)superclass {
  return class_getSuperclass(self);
}

+ (const char *)name {
  return class_getName(self);
}

- (BOOL)isEqual:(id)anObject {
  return self == anObject;
}

- (BOOL)isKindOfClass:(Class)cls {
  return object_isKindOfClass(self, cls);
}

/**
 * @brief Checks if the class conforms to a protocol.
 */
+ (BOOL)conformsTo:(Protocol *)aProtocolObject {
  return class_conformsTo(self, (objc_protocol_t *)aProtocolObject);
}

/**
 * @brief Checks if the receiver's class conforms to a protocol.
 */
- (BOOL)conformsTo:(Protocol *)aProtocolObject {
  return class_conformsTo(object_getClass(self),
                          (objc_protocol_t *)aProtocolObject);
}

@end
