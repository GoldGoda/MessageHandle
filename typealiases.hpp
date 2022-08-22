#pragma once
#include <cstdint>

namespace PACKET
{
    using byte_t = uint8_t;
    using message_id_t = uint16_t;  //INTO A HEADER AND THE MESSAGE TEMPLATE
    using message_size_t = uint32_t;
    using obj_size_t = uint32_t;
    using obj_counter_t = uint32_t;
} // namespace PACKET