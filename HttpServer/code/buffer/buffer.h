#ifndef BUFFER_H
#define BUFFER_H

#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/uio.h> //readv
#include <vector>
#include <atomic>
#include <cstring>
#include <assert.h>

class Buffer
{
public:
    Buffer(int initBufferSize=1024);
    ~Buffer() = default;

    size_t WritableBytes() const;       // 缓冲区可写大小
    size_t ReadableBytes() const ;      // 缓冲区可读大小
    size_t PrependableBytes() const;

    void RetrieveAll() ;                // 恢复所有初始值
    void Retrieve(size_t len);          // 刷新len长度的缓冲区
    void RetrieveUntil(const char* end);    // 刷新(end - readpos)长度的缓冲区
    std::string RetrieveAllToStr();         

    void EnsureWriteable(size_t len);   // 确保缓冲区有足够空间写入
    // void HasReaded(size_t len);         // 已从缓冲区读出len字符
    void HasWritten(size_t len);        // 已写入len字符到缓冲区

    // 缓冲区读指针
    const char* Peek() const;                  

    // 缓冲区写指针
    char* BeginWrite();                 
    const char* BeginWriteConst() const;

    /*  往缓冲区添加数据  */
    void Append(const std::string& str);    
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);

    ssize_t ReadFd(int fd, int* Errno);     // 读入数据
    ssize_t WriteFd(int fd, int* Errno);    // 写入数据

private:
    // 缓冲区首指针
    char* BeginPtr_();                  
    const char* BeginPtr_() const;
    void MakeSpace_(size_t len);            // 缓冲区扩充

    std::vector<char> buffer_;   // 缓冲区
    std::atomic<std::size_t> readPos_;    // 当前读取位置 
    std::atomic<std::size_t> writePos_;   // 当前写入位置

};

#endif // BUFFER_H