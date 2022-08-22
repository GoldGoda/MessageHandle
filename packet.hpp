#pragma once
#include <iostream>
#include <cstring>
#include <vector>
#include <type_traits>

#include "endianness.hpp"
#include "typealiases.hpp"


namespace PACKET{
   class MessageHeader{ public: message_id_t id{ 0 }; message_size_t size {0}; };
   static constexpr auto header_size  { sizeof(MessageHeader) };

   template<message_id_t MTYPE>
   class Message
   { 
      using self_t = Message&;

      private:
      MessageHeader header_       {};
      std::vector<byte_t> data_   {};
      obj_counter_t  obj_counter_ {0};
      obj_counter_t obj_cursor_   {0};
      std::vector<obj_counter_t> obj_pos_list_{}; //list 


      obj_size_t get_index_by_num(obj_size_t object) 
      {
         return obj_pos_list_[object];
      }

      template <typename OBJECT_T>
      OBJECT_T get_item_from_storage(obj_size_t object)
      {
         obj_size_t obj_position = get_index_by_num(object);    
         if constexpr (std::is_same<OBJECT_T, char*>::value == true || 
               std::is_same<OBJECT_T, const char*>::value == true)
               {
                  obj_size_t obj_size = utils::network_endian_handle( *(obj_size_t*)(data_.data()+obj_position) );
                  char* tmp = new char[obj_size]; 
                  memcpy(tmp, data_.data()+obj_position+sizeof(obj_size_t), obj_size);
                  return tmp;
               }
         else 
         {
            return utils::network_endian_handle( 
               *(obj_size_t*)(data_.data()+obj_position+sizeof(obj_size_t)) );
            
         }
      }


      public:
      Message(message_size_t size = 4) : header_({MTYPE, size}){ data_.reserve(size);  }
      Message(MessageHeader &header) 
      {
         set_header(header);
      }

      void set_header(MessageHeader &header) 
      {
         header_.id = utils::network_endian_handle(header.id); 
         header_.size = utils::network_endian_handle(header.size); 
         data_.reserve(header_.size);
      }
      
      ~Message() { }


      message_id_t id()     {  return header_.id;   }
      message_size_t size() {  return header_.size; }
      byte_t* body()        {  return data_.data();  }

      MessageHeader *header() //PODRIA MEJORARSE
      {  
         static MessageHeader tmp{
            utils::network_endian_handle(header_.id),
            utils::network_endian_handle(header_.size)
         };
         return &tmp;
      }


      //SE PUEDE MEJORAR
      void prepare()
      {
         obj_size_t obj_pos = 0;
         obj_size_t obj_size = 0;
         do
         {
            obj_size =  utils::swap_type( *(obj_size_t*)(data_.data()+obj_pos ) );
            obj_pos_list_.push_back(obj_pos);
            obj_pos += sizeof(obj_size)+ obj_size;
         }while(obj_pos != header_.size);
      }

      void set_index(obj_size_t object) {  obj_cursor_ = get_index_by_num(object); }


      template<typename TYPE>
      TYPE get(obj_size_t object)   {  return get_item_from_storage<TYPE>(object); }

      template<typename OBJECT_T>
      void add(const OBJECT_T& object)
      {
         size_t data_size = data_.size();
         if constexpr (std::is_same<OBJECT_T, char*>::value == true || 
               std::is_same<OBJECT_T, const char*>::value == true   ||
               std::is_array<OBJECT_T>::value == true)
               {
                  obj_size_t obj_size = strlen(object)+1; 
                  data_.resize(data_size + sizeof(obj_size) + obj_size);
                  obj_size_t s_size_bytes = utils::network_endian_handle(obj_size);
                  memcpy(data_.data()+data_size, &s_size_bytes, sizeof(obj_size));
                  memcpy(data_.data() + data_size + sizeof(obj_size) , object, obj_size);
               }
         else
         {
            obj_size_t obj_size = sizeof(object);

            data_.resize(data_size + sizeof(obj_size) + obj_size);

            obj_size_t s_size_bytes = utils::network_endian_handle(obj_size);
            memcpy(data_.data()+data_size, &s_size_bytes, sizeof(s_size_bytes));

            //object to body
            OBJECT_T s_bytes = utils::network_endian_handle(object);
            memcpy(data_.data() + data_size + sizeof(obj_size) , &s_bytes, obj_size);
         }
         
         header_.size = data_.size();
         obj_pos_list_.push_back( data_size );
         ++obj_counter_; 
      }

      template<typename OBJECT_T>
      self_t  operator << (const OBJECT_T& object)
      {
         this->add(object);
         return *this;
      }


      template<typename OBJECT_T>
      self_t  operator >> (OBJECT_T& object)
      {
         obj_size_t obj_size = utils::network_endian_handle( *(obj_size_t*)(data_.data()+obj_cursor_));
         if constexpr (std::is_same<OBJECT_T, char*>::value == true ||
               std::is_array<OBJECT_T>::value == true)
               {
                  memcpy(object, data_.data()+ obj_cursor_ + sizeof(obj_size), obj_size);
               }
         else
         {
            memcpy(&object, data_.data()+ obj_cursor_ + sizeof(obj_size), obj_size);
            object = utils::network_endian_handle(object);
         }
         return *this;
      }
   };
}
