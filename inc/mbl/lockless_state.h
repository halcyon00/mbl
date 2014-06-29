#pragma once

#include <atomic>
#include <utility>

static_assert( ATOMIC_INT_LOCK_FREE, "locks used in atomic int" ); 

namespace mbl {
template< typename TState >
class lockless_state {
    typedef TState state_type;
    union state_info {
        struct {
            unsigned clean : 2;
            unsigned dirty : 2;
            unsigned stage : 2;
            bool updated : 1;
            bool initialized : 1;
        };

        int values;
    };

    volatile std::atomic_int m_info;
    unsigned char m_storage[ sizeof( state_type ) * 3 ];
    state_type *m_pState;

    state_info get_info() {
        state_info info; info.values = m_info.load(); return info;
    }

protected:
    lockless_state();
    void initialize();

    // Returns true on new data
    bool swap_reader();
    // Returns true if data was consumed
    bool swap_writer();

    TState &write();
    const TState &read();

public:
    bool is_initialized() { return get_info().initialized; }

};

template< typename TState >
class lockless_state_reader : public lockless_state< TState > {
    typedef lockless_state< TState > base;
public:
    lockless_state_reader() : base() { }

    using base::swap_reader;
    using base::read;
};

template< typename TState >
class lockless_state_writer : private lockless_state< TState > {
    typedef lockless_state< TState > base;
public:
    lockless_state_writer() : base() { }

    using base::initialize;
    using base::swap_writer;
    using base::write;
};

template< typename TState >
lockless_state< TState >::lockless_state() :
    m_info(),
m_pState( reinterpret_cast< state_type* >( m_storage )) {
}

template< typename TState >
void lockless_state< TState >::initialize() {
    state_info info;
    info.clean = 0;
    info.dirty = 1;
    info.stage = 2;
    info.updated = false;
    info.initialized = true;

    new ( &m_pState[ info.clean ] ) state_type();
    new ( &m_pState[ info.dirty ] ) state_type();
    new ( &m_pState[ info.stage ] ) state_type();

    m_info.store( info.values );
}

template< typename TState >
TState &lockless_state< TState >::write() { 
    state_info info = get_info();
    return m_pState[ info.dirty ];
}

template< typename TState >
const TState &lockless_state< TState >::read() {
    state_info info = get_info();
    return m_pState[ info.clean ];
}

template< typename TState >
bool lockless_state< TState >::swap_reader() {
    for ( bool bSwapped = false; !bSwapped; ) {
        state_info info = get_info();
        if ( info.updated == false )
            return false;

        state_info newinfo = info;
        newinfo.updated = false;
        newinfo.clean = info.stage;
        newinfo.stage = info.clean;

        bSwapped = m_info.compare_exchange_weak( info.values, newinfo.values );
    }

    return true;
}

template< typename TState >
bool lockless_state< TState >::swap_writer() {
    bool bUpdated;

    for ( bool bSwapped = false; !bSwapped; ) {
        state_info info = get_info();
        bUpdated = info.updated;

        state_info newinfo = info;
        newinfo.updated = true;
        newinfo.dirty = info.stage;
        newinfo.stage = info.dirty;

        bSwapped = m_info.compare_exchange_weak( info.values, newinfo.values );
    }

    return !bUpdated;
}

}