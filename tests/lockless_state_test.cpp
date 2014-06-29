#include "mbl_test.h"
#include "mbl/shared_mem.h"
#include "mbl/lockless_state.h"

#include <time.h>
#include <cstdlib>
#include <stdio.h>

#include <new>

namespace mbl { namespace tests {

static const char k_sharedMemName[] = "mbl_lockless_state_test";
static const int k_nIters = 100000;

namespace {

struct test_data {
    int nCount;
};

typedef lockless_state_reader< test_data > reader;
typedef lockless_state_writer< test_data > writer;

}

bool lockless_state_test( const char * ) {
    bool bSuccess = false;
    do { 
        (void) shared_mem::unlink( k_sharedMemName );

        shared_mem writeMem;
        if ( !writeMem.open(
            k_sharedMemName,
            shared_mem::modes::create_new,
            sizeof( writer ),
            true
        ))
            break;

        shared_mem readMem;
        if ( !readMem.open(
            k_sharedMemName,
            shared_mem::modes::open_existing,
            sizeof( reader ),
            true
        ))
            break;

        reader *pReader = new ( readMem.ptr() ) reader();
        writer *pWriter = new ( writeMem.ptr() ) writer();

        int nSeed = time( nullptr );
        srand( nSeed );
        printf( "    RNG seed: 0x%x\n", nSeed );

        if ( pReader->is_initialized() )
            break;

        pWriter->initialize();
        if ( !pReader->is_initialized() )
            break;

        test_data lastData;
        lastData.nCount = 0;
        for ( int nIter = 0; nIter < k_nIters; ++nIter )
        {
            if ( rand() % 2 != 0 )
                pWriter->swap_writer();

            pWriter->write().nCount = nIter;

            if ( rand() % 2 != 0 )
                pWriter->swap_writer();

            pWriter->write().nCount = nIter;

            if ( rand() % 2 != 0 )
                pReader->swap_reader();

            if ( lastData.nCount > pReader->read().nCount )
            {
                printf( "    COUNT INVERSION\n" );
                break;
            }

            lastData = pReader->read();

            if ( rand() %2 != 0 )
                pReader->swap_reader();

            if ( lastData.nCount > pReader->read().nCount )
            {
                printf( "    COUNT INVERSION\n" );
                break;
            }

            lastData = pReader->read();

            if ( nIter == k_nIters - 1 )
                bSuccess = true;
        }

    } while( false );

    (void) shared_mem::unlink( k_sharedMemName );
    return bSuccess;
}

} }