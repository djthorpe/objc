#pragma once

/*
 * The object class for NXFoundation
 */
@interface NXObject : Object {
@private
    id _zone; // The zone this object is allocated in
}

/**
 * Allocates a new instance of object in a specific zone.
 */
+(id) allocWithZone:(NXZone* )zone;

@end
