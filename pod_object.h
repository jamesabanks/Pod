#ifndef INCLUDE_POD_OBJECT_H
#define INCLUDE_POD_OBJECT_H



// This is object-oriented programing, C-style.  All pod data is stored in
// instances of subclasses of pod_object, which itself would be abstract,
// if C supported such concepts.  This isn't intended to be a robust, complete,
// etc., (whatever those words mean) object system.  Just enough to get the job
// done.  I'm writing pod objects to always be allocated on the heap (via
// malloc) and never on the stack (as parameters or local variables).


    // Every subclass of object should have its first member be the type of
    // pod_object.  When the structure is initialized, the type member should
    // be set to the value appropriate to the type of struct.  Practically,
    // POD_OBJECT_TYPE itself should never be used.

extern const int POD_OBJECT_TYPE;


    // Every subclass needs to define a destructor and set the destroy member
    // to it at initialization (or construction, if you please).  The destroy
    // member is called to clear variables, release resources, and finally,
    // free the struct's memory.  The call is something like obj->destroy(obj).
    // C doesn't have anything like a "this" or a "self" variable.

typedef void pod_destroy(void *);


    // And finally, the struct pod_object definition:

struct pod_object {
    int type;
    struct pod_object *next;
    struct pod_object *previous;
    pod_destroy *destroy;
};



#endif /* INCLUDE_POD_OBJECT_H */
