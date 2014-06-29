#pragma once

namespace mbl {
namespace tests {

    extern const char *g_pImageName;

    bool shared_mem_test( const char *pArg );
    bool lockless_state_test( const char *pArg );
}
}
