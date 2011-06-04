#include <stdio.h>
#include "../pod_object.h"



// This test is rather meaningless, too (like test_pod_char only moreso).
// It checks the value of POD_OBJECT_TYPE.  Which is 0xb0000.  So if this gets
// changed, it will catch that.  At this time pod_object isn't meant to ever
// be used directly, so there is no code to test.

int main(int argc, char *argv)
{
    int error_count;

    error_count = 0;
    if (POD_OBJECT_TYPE != 0xb0000) {
        error_count++;
        fprintf(stdout, "    POD_OBJECT_TYPE is not the expected value.\n");
        fprintf(stdout,
                "      (expected 0xb0000, got 0x%x.)\n",
                POD_OBJECT_TYPE);
    }

    return error_count;
}
