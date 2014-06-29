#include <mbl/shared_mem.h>
#include <cstdlib>
#include <time.h>

namespace mbl { namespace tests {

static const char k_memName[] = "mbl_test_shared_mem";

static bool shared_mem_parent( void ) {
    shared_mem mem;
    (void) shared_mem::unlink( k_memName );

    if (!mem.open(
        k_memName,
        shared_mem::modes::create_new,
        sizeof( bool ),
        true
    ))
        return false;

    shared_mem mem2;
    if (!mem2.open(
        k_memName,
        shared_mem::modes::open_existing,
        sizeof( bool ),
        true
    ))
        return false;

    volatile bool *m_pWrite = (volatile bool*) mem.ptr();
    volatile bool *m_pRead = (volatile bool*) mem2.ptr();

    if ( *m_pRead || *m_pWrite )
        return false;

    *m_pWrite = true;
    if ( *m_pRead != true )
        return false;

    (void) shared_mem::unlink( k_memName );
    return true;
}

bool shared_mem_test( const char *pArg ) {
    bool bPass = shared_mem_parent();
    (void) shared_mem::unlink( k_memName );
    return bPass;
}

} }