#pragma once

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cassert>

namespace dm {

using u8 = uint8_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

using String = std::string;
using StringRef = std::basic_string_view<char>;

template<class T>
using Ptr = std::shared_ptr<T>;


inline void writeLength(const u32 length, char *pos)
{
    assert(length < 36*36);

    u32 v = length / 36;
    pos[0] = v > 9 ? v + 'a' - 10 : v + '0';

    v = length % 36;
    pos[1] = v > 9 ? v + 'a' - 10 : v + '0';
}

}   // end of namespace dm;
