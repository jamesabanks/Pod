#ifndef INCLUDE_POD_OBJECT_H
#define INCLUDE_POD_OBJECT_H

/******************************************************************** 
 *  pod_object.h
 *  Copyright (c) 2011-2012, James A. Banks
 *  All rights reserved.
 *  See file LICENSE for details.
 ********************************************************************/

#include "pod_node.h"



// This is object-oriented programing, C-style.  All pod data is stored in
// instances of subclasses of pod_object, which itself would be abstract,
// if C had such things.  This isn't intended to be thorough, complete, etc.,
// (whatever those words mean) object system.  Just enough to get the job
// done.  I'm writing pod objects to always be allocated on the heap (via
// malloc) and never on the stack (as parameters or local variables).


    // Every subclass of object should have its first member be the type of
    // pod_object.  When the structure is initialized, the type member should
    // be set to the value appropriate to the type of struct.  Practically,
    // POD_OBJECT_TYPE itself should never be used.

#define POD_OBJECT_TYPE 0x60


    // Every subclass needs to define a destructor and set the destroy member
    // to it at initialization (or construction, if you please).  The destroy
    // member is called to clear variables, release resources, and finally,
    // free the struct's memory.  The call is something like obj->destroy(obj).
    // C doesn't have anything like a "this" or a "self" variable.

typedef void pod_destroy(void *);


    // And finally, the struct pod_object definition:

struct pod_object;
typedef struct pod_object pod_object;

struct pod_object {
    pod_node n;
    int type;
    pod_destroy *destroy;
};



#endif /* INCLUDE_POD_OBJECT_H */
