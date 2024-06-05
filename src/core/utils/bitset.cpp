/*
* Bitset.cpp is a modified version of the Boost dynamic bitset class,
 * and adheres to the the Boost Software License, Version 1.0.

 * Modifications were made to suit specific needs and design preferences.
 * Any changes beyond this point do not necessarily reflect the views of Boost.
 *
 * License information for Boost Software License, Version 1.0 can be found here:
 * https://www.boost.org/LICENSE_1_0.txt
 */

#include "bitset.h"

Bitset operator&(const Bitset& x, const Bitset& y)
{
    Bitset b(x);
    return b &= y;
}

Bitset operator|(const Bitset& x, const Bitset& y)
{
    Bitset b(x);
    return b |= y;
}

bool operator!=(Bitset& a, Bitset& b)
{
    return !(a == b);
}

bool operator==(const Bitset& a, const Bitset& b)
{
    return (a.m_num_bits == b.m_num_bits)
           && (a.m_bitset == b.m_bitset);
}