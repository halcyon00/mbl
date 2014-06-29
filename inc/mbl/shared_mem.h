#pragma once
#include <cstdlib>

namespace mbl {

class shared_mem {
    int m_nHandle;
    void *m_pMemory;
    size_t m_nLength;

public:
    enum class modes;

    shared_mem();
    ~shared_mem();

    bool open (
        const char *pName,
        modes eOpenMode,
        size_t nMinSize,
        bool bWrite
    );
    void close ();

    inline void *ptr () { return m_pMemory; }
    inline size_t length () { return m_nLength; }

    static bool unlink( const char *pName );
};

enum class shared_mem::modes {
    open_existing,
    create_new,
    open_or_create
};

}
