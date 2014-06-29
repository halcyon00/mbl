#include "mbl_test.h"
#include <stdio.h>
#include <string.h>

const char *g_pImageName = nullptr;

typedef bool ( *test_func_ptr ) ( const char *pArg );

struct mbl_test_row {
    const char *pTestName;
    test_func_ptr pTestFunc;
};

static bool all_tests( const char * );
static bool list_tests( const char * );
static const mbl_test_row s_test_table[] = {
    { "all", &all_tests },
    { "help", &list_tests },
    { "list", &list_tests },
    { "shared_mem", &mbl::tests::shared_mem_test },
    { "lockless_state", &mbl::tests::lockless_state_test }
};

static bool run_test( const mbl_test_row &test, const char *pArg ) {
    printf( "Running test: %s\n", test.pTestName );
    bool bPassed = test.pTestFunc( pArg );
    printf( bPassed ? "[PASSSED]\n" : "[FAILED]\n" );
    return bPassed;
}


static bool list_tests( const char * ) {
    printf( "Tests:\n" );
    for ( const auto &test : s_test_table ) {
        printf( "    %s\n", test.pTestName);
    }
    return true;
}

static bool all_tests( const char * ) {
    bool bPassing = true;
    for ( const auto &test : s_test_table ) {
        if ( test.pTestFunc == &all_tests )
            continue;
        if ( test.pTestFunc == &list_tests )
            continue;

        bPassing &= run_test( test, nullptr );
    }

    return bPassing;
}

int main( int argc, const char **argv )
{
    g_pImageName = argv[0];

    const mbl_test_row *pRow = s_test_table;
    if ( argc > 1 ) {

        for ( const auto &test : s_test_table ) {
            if ( !strcmp( argv[1], test.pTestName ) ) {
                pRow = &test;
                break;
            }
        }
    }

    const char *pArg = argc > 2 ? argv[2] : nullptr;
    (void) run_test( *pRow, pArg );

    return 0;
}