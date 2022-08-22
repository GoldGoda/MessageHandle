#pragma once
#include <cstdint>

namespace utils
{
    enum endianess { little_endian = 0, big_endian = 1 };
    
    endianess get_endian()
    {
        uint32_t tmp = 1;
        if(*reinterpret_cast<uint8_t*>(&tmp) == 0)
            return endianess::big_endian;
        else
            return endianess::little_endian;
    }

    bool is_little_endian()
    {
        return get_endian() == little_endian;
    }
    
    bool is_big_endian()
    {
        return get_endian() == big_endian;
    }

    int16_t swap_int16(int16_t num) 
    { 
        return    num << 8 & 0xFF00 
                | num >> 8 & 0x00FF; 
    }

    uint32_t swap_int32(uint32_t num)
    {
        return    num << 24 & 0xFF000000
                | num <<  8 & 0x00FF0000
                | num >>  8 & 0x0000FF00
                | num >> 24 & 0x000000FF;
    }

    uint64_t swap_int64(uint64_t num)
    {
     return ((num & 0x00000000000000FFULL) << 56) | 
            ((num & 0x000000000000FF00ULL) << 40) | 
            ((num & 0x0000000000FF0000ULL) << 24) | 
            ((num & 0x00000000FF000000ULL) <<  8) |
            ((num & 0x000000FF00000000ULL) >>  8) | 
            ((num & 0x0000FF0000000000ULL) >> 24) | 
            ((num & 0x00FF000000000000ULL) >> 40) | 
            ((num & 0xFF00000000000000ULL) >> 56);
        return num;
    }

    template <typename TYPE>
    constexpr TYPE swap_type(TYPE value)
    {
        if constexpr(sizeof(value) == 2)
            return swap_int16(value);
        else if constexpr(sizeof(value) == 4)
            return swap_int32(value);
        else if constexpr(sizeof(value) == 8)
            return swap_int64(value);
        return value;
    } 


//NECESITA UNA ACTUALIZACION
      template <typename VALUE>
      VALUE network_endian_handle(VALUE value)
      {
         if(utils::is_little_endian())
            return utils::swap_type(value);
         return value;
      }


    template <typename T>
    T universal_swap_endian (T value)
    {
        union {
            T src;
            unsigned char dst[sizeof(T)];
        } source, dest;

        source.src = value;
        for (size_t k = 0; k < sizeof(T); ++k)
            dest.dst[k] = source.dst[sizeof(T) - k - 1];

        return dest.src;
    }

}