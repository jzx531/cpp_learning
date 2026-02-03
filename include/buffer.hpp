#ifndef BUFFER_HPP_
#define BUFFER_HPP_

/*
向缓冲区中写入数据；
获取可读数据起始地址的接口；
获取可读数据长度的接口；
移动读写位置的接口；
初始化缓冲区的操作（将读写位置初始化–在一个缓冲区所有数据处理完毕之后）；
提供交换缓冲区的操作（交换空间地址，并不交换空间数据）。
*/

#include "util.hpp"
#include <vector>
#include <cassert>

namespace LOG
{
    #define DEFAULT_BUFFER_SIZE (1 * 1024 * 1024)
    #define THRESHOLD_BUFFER_SIZE (8 * 1024 * 1024)
    #define INCREMENT_BUFFER_SIZE (1 * 1024 * 1024)
    
    class Buffer
    {
    private:
        std::vector<char> _buffer;
        size_t _reader_idx; // 当前可读数据的指针
        size_t _writer_idx; // 当前可写数据的指针

        void ensureEnoughSize(size_t len)
        {
            if(writeAbleSize()>len) return;
            size_t new_size = 0;
            if(_buffer.size()<THRESHOLD_BUFFER_SIZE)
            {
                new_size= _buffer.size()*2 +len;
            }
            else{
                new_size = _buffer.size()+INCREMENT_BUFFER_SIZE+len;
            }
            _buffer.resize(new_size);
        }

        //移动写指针
        void moveWriter(size_t len)
        {
            assert(writeAbleSize()>len);
            _writer_idx+=len;
        }

        //移动读指针
        void moveReader(size_t len)
        {
            assert(readAbleSize()>len);
            _reader_idx+=len;
        }

    public:
        Buffer():_buffer(DEFAULT_BUFFER_SIZE),_writer_idx(0),_reader_idx(0) {}
        //向缓冲区写入数据
        void push(const char* data,size_t len)
        {
            // 1.空间不够，需要进行扩容
            ensureEnoughSize(len);
            //2.将data中的数据写入缓冲区
            std::copy(data,data+len,&_buffer[_writer_idx]);
            // 3.将当前写入位置向后偏移
            moveWriter(len);
        }

             // 返回可读数据的起始地址
        const char* begin()
        {
            return &_buffer[_reader_idx];
        }
        // 返回可写数据的长度
        size_t writeAbleSize()
        {
            // 对于扩容思路并没有用， 仅针对固定大小缓冲区
            return (_buffer.size() - _writer_idx);
        }
        // 返回可读数据的长度
        size_t readAbleSize()
        { 
            return (_writer_idx - _reader_idx);
        }
        void reset()
        {
            _writer_idx = 0;
            _reader_idx = 0;
        }
        bool empty()
        {
            return _writer_idx == _reader_idx;
        }
        // 对buffer实现交换操作
        void swap(Buffer & other)
        {
            _buffer.swap(other._buffer);
            std::swap(_reader_idx, other._reader_idx);
            std::swap(_writer_idx, other._writer_idx);
        }

    };
}



#endif

