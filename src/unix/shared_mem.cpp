#include "mbl/shared_mem.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

namespace mbl {

shared_mem::shared_mem() : m_nHandle( -1 ), m_pMemory( nullptr ) { }
shared_mem::~shared_mem() { close(); }

bool shared_mem::open (
    const char *pName,
    shared_mem::modes eOpenMode,
    size_t nMinSize,
    bool bWrite
) {

    if ( m_nHandle != -1 )
        return false;

    int nOpenFlags = 0;
    switch ( eOpenMode ) {
        case modes::open_existing: break;
        case modes::create_new:
            nOpenFlags = O_CREAT | O_EXCL;
            break;
        case modes::open_or_create:
            nOpenFlags = O_CREAT;
    }

    nOpenFlags |= bWrite ? O_RDWR : O_RDONLY;

    char name[ NAME_MAX + 2 ];
    snprintf( name, sizeof( name ), "/%s", pName );
    m_nHandle = shm_open( name, nOpenFlags, S_IRUSR | S_IWUSR );

    if ( m_nHandle == -1 )
        return false;

    struct stat stats;
    if ( fstat( m_nHandle, &stats ) == -1 )
        goto error;

    if ( stats.st_size < nMinSize )
    {
        if ( ftruncate( m_nHandle, nMinSize ) == -1 )
            goto error;
        stats.st_size = nMinSize;
    }

    m_nLength = stats.st_size;
    m_pMemory = mmap(
        nullptr,
        m_nLength,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        m_nHandle,
        0 // offset
    );

    if ( m_pMemory == MAP_FAILED ) {
        m_pMemory = nullptr;
        goto error;
    }

    return true;

error:
    close();
    return false;
}

void shared_mem::close () {
    if ( m_pMemory != nullptr )
    {
        munmap( m_pMemory, m_nLength );
        m_nLength = 0;
        m_pMemory = nullptr;
    }

    if ( m_nHandle != -1 )
    {
        ::close( m_nHandle );
        m_nHandle = -1;
    }
}

bool shared_mem::unlink( const char *pName ) {
    char name[ NAME_MAX + 2];
    snprintf( name, sizeof( name ), "/%s", pName );
    return shm_unlink( name ) == 0;
}

}