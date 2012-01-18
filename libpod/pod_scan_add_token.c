#include <assert.h>
#include "pod_scan.h"
#include "pod_log.h"
#include "pod_boolean.h"



    // pod_scan_merge_down
    //
    // This routine takes whatever is on top of the stack and adds it into to
    // the next element down.  This only works if this second element is a
    // structure (that is, a list or a map).  Furthermore, if it's a map, then
    // only a mapping can be added.  If this routine is called with the top
    // element as a string with the POD_IS_KEY flag set, the routine will clear
    // the POD_IS_KEY flag.  So this routine shouldn't be called by the "="
    // token.
    //
    // Returns:
    //      int     The error id of any problem that occurred (0 = no error)

int pod_scan_merge_down(pod_stream *stream)
{
    pod_mapping *mapping;
    //pod_object *object;
    pod_object *popped;
    pod_object *top;
    int warning;

    warning = 0;

    top = pod_list_peek(stream->s_stack);

    if (top == NULL) {
        // The stack is empty, nothing to do
        return 0;
    } else if (top->type == POD_MAPPING_TYPE) {
        mapping = (pod_mapping *) top;
        if (mapping->value == NULL) {
            // Error: trailing "="
            // ie, foo = . -> foo= . -> foo(error)
            warning = POD_EQUALS_AT_END;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            mapping = (pod_mapping *) pod_list_pop(stream->s_stack);
            pod_list_push(stream->s_stack, (pod_object *) mapping->key);
            mapping->key = NULL;
            mapping->o.destroy(mapping);
        }
    }

    popped = pod_list_pop(stream->s_stack);
    top = pod_list_peek(stream->s_stack);

    if (top == NULL) {
        // The stack is now empty, so popped is the whole pod.
        stream->result_pod = popped;
    } else if (top->type == POD_LIST_TYPE) {
        // The top of the stack is a list, so add popped.
        pod_list_append((pod_list *) top, popped);
    } else if (top->type == POD_MAPPING_TYPE) {
        if (popped->type != POD_MAPPING_TYPE) {
            ((pod_mapping *) top)->value = popped;
            warning = pod_scan_merge_down(stream);
            pod_stream_log(stream, warning, __FILE__, __LINE__);
        } else {
            // Error: a mapping cannot be the value of another mapping
            // I don't think this can happen
            warning = POD_TWO_MAPPINGS;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            popped->destroy(popped);
        }
    } else {
        // Error: tried to insert element into string
        // I don't think this can happen
        popped->destroy(popped);
    }

    return warning;
}



int pod_scan_add_token(pod_stream *stream, pod_scan_token token)
{
    pod_string *current;
    pod_list *list;
    pod_mapping *mapping;
    pod_object *top;
    int warning;

    warning = 0;
    top = pod_list_peek(stream->s_stack);
    switch (token) {
        case POD_TOKEN_STRING:
            if (top->type == POD_STRING_TYPE) {
                pod_scan_merge_down(stream);
            }
            current = pod_string_dup_text(stream->s_buffer);
            stream->s_buffer->used = 0;
            stream->have_concat = false;
            pod_list_push(stream->s_stack, (pod_object *) current);
            break;
        case POD_TOKEN_EQUALS:
            if (top->type == POD_STRING_TYPE) {
                // make a new mapping with the string as the key
                // push it on the stack
                mapping = pod_mapping_create();
                assert(mapping != NULL);
                mapping->key = (pod_string *) pod_list_pop(stream->s_stack);
                pod_list_push(stream->s_stack, (pod_object *) mapping);
            } else {
                // Error: the token prior to "=" needs to be a string.
                warning = POD_EQUALS_NOT_PRECEDED_BY_KEY;
                pod_stream_log(stream, warning, __FILE__, __LINE__);
            }
            break;

        case POD_TOKEN_BEGIN_BLURB:
        case POD_TOKEN_END_BLURB:
            break;

        case POD_TOKEN_BEGIN_LIST:
            if (top->type == POD_STRING_TYPE) {
                pod_scan_merge_down(stream);
            }
            list = pod_list_create();
            assert(list != NULL);
            pod_list_push(stream->s_stack, (pod_object *) list);
            break;
        case POD_TOKEN_END_LIST:
            if (top->type == POD_STRING_TYPE || top->type == POD_MAPPING_TYPE) {
                pod_scan_merge_down(stream);
            }
            top = pod_list_peek(stream->s_stack);
            if (top->type == POD_LIST_TYPE) {
                pod_scan_merge_down(stream);
            } else {
                // Error, encountered unmatched "}"
                warning = POD_UNMATCHED_END_LIST;
                pod_stream_log(stream, warning, __FILE__, __LINE__);
            }
            break;
        case POD_TOKEN_POD_SYNC:
            while (! pod_list_is_empty(stream->s_stack)) {
                pod_scan_merge_down(stream);
            }
            break;
        default:
            // Error: unknown token
            warning = POD_UNKNOWN_TOKEN;
            pod_stream_log(stream, warning, __FILE__, __LINE__);
            break;
    }

    return warning;
}

