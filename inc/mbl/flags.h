#pragma once

#include <type_traits>

namespace mbl {

template< typename T >
class TFlags {
    typedef typename std::underlying_type<T>::type underlying;
    static_assert( std::is_enum<T>::value, "non-enum flag type" );

    T m_eValue;
public:
    TFlags( T eValue ) : m_eValue( eValue ) { }
    inline operator T() { return value(); }
    inline T &value() { return m_eValue; }
    inline const T &value() const { return m_eValue; }

    inline underlying as_int() const {
        return static_cast< size_t >( value() );
    }

    bool is_set( T flag ) {
        return !!( as_int() & static_cast<size_t>( flag ) );
    }

    void set( T flag ) {
        value() = static_cast<T>(
            as_int() | static_cast<size_t>( flag )
        );
    }
};

}