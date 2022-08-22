#pragma once
#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <deque>



void print_hex_value(uint16_t value)
{
   std::cout << std::hex << std::setw(2) << std::setfill('0') << value;
}

template <typename TYPE>
void show_mem_row(TYPE *object, uint32_t byte_rows = 1, uint32_t row_size = 0)
{
   auto *ptr = object;
   uint8_t *ptr_tmp = nullptr;
   size_t object_size = sizeof(*object);

   for (size_t e = 0; e < byte_rows; e++)
   {
      ptr_tmp = reinterpret_cast<uint8_t*>(object) + e;
      std::cout << reinterpret_cast<uint16_t*>(ptr_tmp) << " - ";

      if (row_size != 0)
         object_size = row_size;

      for (int i = 0; i < object_size; i++)
      {
         print_hex_value(*ptr_tmp);
         std::cout << " ";
         ptr_tmp++;
      }
      
      std::cout << "\n";
      ptr++;
   }
   
   ptr_tmp = nullptr;
}
