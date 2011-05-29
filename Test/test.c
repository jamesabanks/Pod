#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



// Testing infrastructures come down to basicly the same thing.  A
// heirachically organized bunch of routines.  And that isn't magical or
// complicated.  So I'll try my own.
//
// I kinda wonder if an option to specify an indent string would be useful.
// But that would mean an option infrastructure and I just shudder at adding
// that much stuff for any reason.  Or I could roll my own.  Yet another
// project.  Not now.
//
// And putting time stamps in.  And so on...
//
// It might make sense to move test_files into its own file (or make it a text
// file which is read into test).



// test_files is a null terminated list of executables (binaries or scripts) to
// be run if no parameter (a specific binary or script) is specified.  An
// executable can run one test or many tests.

char *test_files[] = {
    "test1",
    NULL
};



// run_test
//
// I split run_test out from main because I wanted to be able to run a single
// test by itself.  The way that was obvious to me was to have a single
// parameter that would be run as a test if it existed. Moving the testing to
// a single function that works on a single, provided filename allows that.

static int run_test(char *name)
{
    char *argv[] = { NULL, NULL };
    char *env[] = { NULL };
    pid_t pid;
    int retval;
    int status;
    char *estr;
    int temp;
    
    retval = 0;
    fprintf(stdout, "Running test '%s'\n", name);
    pid = fork();
    if (pid == 0) {
        argv[0] = name;
        execve(name, argv, env);
        estr = strerror(errno);
        fprintf(stdout, "    Execve failure: %s (%d)\n", estr, errno);
        exit(errno);
    } else {
        // TODO Handling the results of wait is not complete.  Make it
        // complete.
        pid = wait(&status);
        if (pid < 0) {
            fprintf(stdout, "    Wait failure: %s\n", strerror(errno));
            retval = 1;
        }
    }
    if (status == 0) {
        fprintf(stdout, "Test '%s' SUCCESS\n", name);
    } else {
        if (WIFEXITED(status)) {
            temp = WEXITSTATUS(status);
            fprintf(stdout, "Test '%s' FAILED, error %d\n", name, temp);
        } else {
            fprintf(stdout, "Test '%s' FAILED (and/or didn't exit)\n", name);
        }
    }

    return retval;
}



// main
//
// This program can be run without arguments or with one argument.  If it is
// run without arguments it will iterate through "test_files" (above), trying
// to execute each.  If this program is run with one parameter it will try to
// run only that executable specified in the parameter.

int main(int argc, char *argv[])
{
    int i;
    int retval;

    retval = 0;
    if (argc > 2) {
        fprintf(stderr, "Too many arguments\n");
        retval = 1;
    } else if (argc == 2) {
        retval = run_test(argv[1]);
    } else {
        i = 0;
        while (test_files[i] != NULL) {
            retval = run_test(test_files[i]);
            if (retval != 0) {
                fprintf(stdout, "\n");
                break;
            }
            i++;
            if (test_files[i] != NULL) {
                fprintf(stdout, "\n");
            }
        }
    }

    return retval;
}
