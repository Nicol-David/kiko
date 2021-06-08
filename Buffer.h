#pragma once


#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>
#include "Socket.h"
//#include <unistd.h>  // ssize_t




/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer //public muduo::copyable
{
 public:
  static const size_t kCheapPrepend = 8; //预备字节的大小
  static const size_t kInitialSize = 1024;//缓冲区的大小

  explicit Buffer(size_t initialSize = kInitialSize)
    : buffer_(kCheapPrepend + initialSize),
      readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend)
  {
    assert(readableBytes() == 0);
    assert(writableBytes() == initialSize);
    assert(prependableBytes() == kCheapPrepend);
  }

  // implicit copy-ctor, move-ctor, dtor and assignment are fine
  // NOTE: implicit move-ctor is added in g++ 4.6

  void swap(Buffer& rhs)//交换两个buffer
  {
    buffer_.swap(rhs.buffer_);
    std::swap(readerIndex_, rhs.readerIndex_);
    std::swap(writerIndex_, rhs.writerIndex_);
  }

  size_t readableBytes() const//可读的字节数
  { return writerIndex_ - readerIndex_; }

  size_t writableBytes() const//可写的字节数
  { return buffer_.size() - writerIndex_; }

  size_t prependableBytes() const//预备字节数
  { return readerIndex_; }

  const char* peek() const//返回第一个可读的字符的地址
  { return begin() + readerIndex_; }

  const char* findCRLF() const//寻找停止符，因为停止符代表着一句完整的tcp消息。如果返回Null表示没有找到
  {
    // FIXME: replace with memmem()?
    const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
    return crlf == beginWrite() ? NULL : crlf;
  }

  const char* findCRLF(const char* start) const//从指定位置开始寻找停止符
  {
    assert(peek() <= start);
    assert(start <= beginWrite());
    // FIXME: replace with memmem()?
    const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2);
    return crlf == beginWrite() ? NULL : crlf;
  }

  const char* findEOL() const//寻找'\n'
  {
    const void* eol = memchr(peek(), '\n', readableBytes());
    return static_cast<const char*>(eol);
  }

  const char* findEOL(const char* start) const//从指定位置寻找'\n'
  {
    assert(peek() <= start);
    assert(start <= beginWrite());
    const void* eol = memchr(start, '\n', beginWrite() - start);
    return static_cast<const char*>(eol);
  }

  // retrieve returns void, to prevent
  // string str(retrieve(readableBytes()), readableBytes());
  // the evaluation of two functions are unspecified
  void retrieve(size_t len)//读取缓冲区后，更改readerIndex_。len表示有len个字节已经被读取
  {
    assert(len <= readableBytes());
    if (len < readableBytes())
    {
      readerIndex_ += len;
    }
    else//如果数据全部被读完，则复位缓冲区
    {
      retrieveAll();
    }
  }

  void retrieveUntil(const char* end)//读取缓冲区后，更改readerIndex_。end表示当前readerIndex_的指针
  {
    assert(peek() <= end);
    assert(end <= beginWrite());
    retrieve(end - peek());
  }

  void retrieveInt64()//读取64位后，更新readerIndex_
  {
    retrieve(sizeof(int64_t));
  }

  void retrieveInt32()//读取32位后，更新readerIndex_
  {
    retrieve(sizeof(int32_t));
  }

  void retrieveInt16()//读取16位后，更新readerIndex_
  {
    retrieve(sizeof(int16_t));
  }

  void retrieveInt8()//读取8位后，更新readerIndex_
  {
    retrieve(sizeof(int8_t));
  }

  void retrieveAll()//复位readerIndex_，writerIndex_
  {
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
  }

  std::string retrieveAllAsString()//将缓区中所有的数据读取为string
  {
    return retrieveAsString(readableBytes());
  }

  std::string retrieveAsString(size_t len)//将缓区中len长度的数据读取为string
  {
    assert(len <= readableBytes());
    std::string result(peek(), len);
    retrieve(len);
    return result;
  }

  //ringPiece toStringPiece() const
  //
  //return StringPiece(peek(), static_cast<int>(readableBytes()));
 //

 // append(const StringPiece& str)//向缓冲区中添加数据
 //
 // append(str.data(), str.size());
 //

  void append(const char* /*restrict*/ data, size_t len)//向缓冲区中添加len长度的数据，
  {
    ensureWritableBytes(len);//确保有足够空间
    std::copy(data, data+len, beginWrite());
    hasWritten(len);//更改writerIndex_
  }

  void append(const void* /*restrict*/ data, size_t len)
  {
    append(static_cast<const char*>(data), len);
  }

  void ensureWritableBytes(size_t len)//保证有足够的空间写入字节
  {
    if (writableBytes() < len)
    {
      makeSpace(len);//如果可写空间不够，则分配足够的空间
    }
    assert(writableBytes() >= len);
  }

  char* beginWrite()//writerIndex_对应的指针
  { return begin() + writerIndex_; }

  const char* beginWrite() const
  { return begin() + writerIndex_; }

  void hasWritten(size_t len)//更改writerIndex_
  {
    assert(len <= writableBytes());
    writerIndex_ += len;
  }

  void unwrite(size_t len)//没有写成功
  {
    assert(len <= readableBytes());
    writerIndex_ -= len;
  }

  ///
  /// Append int64_t using network endian
  ///
  void appendInt64(int64_t x)//将64位数据用网络字节序写入缓冲区
  {
    int64_t be64 = sockets::hostToNetwork64(x);
    append(&be64, sizeof be64);
  }

  ///
  /// Append int32_t using network endian
  ///
  void appendInt32(int32_t x)//将32位数据用网络字节序写入缓冲区
  {
    int32_t be32 = sockets::hostToNetwork32(x);
    append(&be32, sizeof be32);
  }

  void appendInt16(int16_t x)//将16位数据用网络字节序写入缓冲区
  {
    int16_t be16 = sockets::hostToNetwork16(x);
    append(&be16, sizeof be16);
  }

  void appendInt8(int8_t x)//将8位数据用网络字节序写入缓冲区
  {
    append(&x, sizeof x);
  }

  ///
  /// Read int64_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int32_t)
  int64_t readInt64()//读取64数据，并更新readerIndex_
  {
    int64_t result = peekInt64();
    retrieveInt64();
    return result;
  }

  ///
  /// Read int32_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int32_t)
  int32_t readInt32()//读取32数据，并更新readerIndex_
  {
    int32_t result = peekInt32();
    retrieveInt32();
    return result;
  }

  int16_t readInt16()//读取16数据，并更新readerIndex_
  {
    int16_t result = peekInt16();
    retrieveInt16();
    return result;
  }

  int8_t readInt8()//读取8数据，并更新readerIndex_
  {
    int8_t result = peekInt8();
    retrieveInt8();
    return result;
  }

  ///
  /// Peek int64_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int64_t)
  int64_t peekInt64() const //取出64位数据，并转换为主机字节序
  {
    assert(readableBytes() >= sizeof(int64_t));
    int64_t be64 = 0;
    ::memcpy(&be64, peek(), sizeof be64);
    return sockets::networkToHost64(be64);
  }

  ///
  /// Peek int32_t from network endian
  ///
  /// Require: buf->readableBytes() >= sizeof(int32_t)
  int32_t peekInt32() const //取出32位数据，并转换为主机字节序
  {
    assert(readableBytes() >= sizeof(int32_t));
    int32_t be32 = 0;
    ::memcpy(&be32, peek(), sizeof be32);
    return sockets::networkToHost32(be32);
  }

  int16_t peekInt16() const //取出16位数据，并转换为主机字节序
  {
    assert(readableBytes() >= sizeof(int16_t));
    int16_t be16 = 0;
    ::memcpy(&be16, peek(), sizeof be16);
    return sockets::networkToHost16(be16);
  }

  int8_t peekInt8() const //取出8位数据，并转换为主机字节序
  {
    assert(readableBytes() >= sizeof(int8_t));
    int8_t x = *peek();
    return x;
  }

  ///
  /// Prepend int64_t using network endian
  ///
  void prependInt64(int64_t x)//向可读数据前面添加64位数据
  {
    int64_t be64 = sockets::hostToNetwork64(x);
    prepend(&be64, sizeof be64);
  }

  ///
  /// Prepend int32_t using network endian
  ///
  void prependInt32(int32_t x)//向可读数据前面添加32位数据
  {
    int32_t be32 = sockets::hostToNetwork32(x);
    prepend(&be32, sizeof be32);
  }

  void prependInt16(int16_t x)//向可读数据前面添加16位数据
  {
    int16_t be16 = sockets::hostToNetwork16(x);
    prepend(&be16, sizeof be16);
  }

  void prependInt8(int8_t x)//向可读数据前面添加8位数据
  {
    prepend(&x, sizeof x);
  }

  void prepend(const void* /*restrict*/ data, size_t len)//向可读数据前面添加数据
  {
    assert(len <= prependableBytes());
    readerIndex_ -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d+len, begin()+readerIndex_);
  }

  //id shrink(size_t reserve)
  //
  //// FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
  //Buffer other;
  //other.ensureWritableBytes(readableBytes()+reserve);
  //other.append(toStringPiece());
  //swap(other);
  //

  size_t internalCapacity() const
  {
    return buffer_.capacity();
  }

  /// Read data directly into buffer.
  ///
  /// It may implement with readv(2)
  /// @return result of read(2), @c errno is saved
  ssize_t readFd(int fd);

 private:

  char* begin()//返回缓冲区第一个字节地址
  { return &*buffer_.begin(); }

  const char* begin() const
  { return &*buffer_.begin(); }

  void makeSpace(size_t len)//分配足够的空间
  {
    if (writableBytes() + prependableBytes() < len + kCheapPrepend)//如果空间不够则扩展缓冲区
    {
      // FIXME: move readable data
      buffer_.resize(writerIndex_+len);
    }
    else//如果空间足够则搬挪已有数据
    {
      // move readable data to the front, make space inside buffer
      assert(kCheapPrepend < readerIndex_);
      size_t readable = readableBytes();
      std::copy(begin()+readerIndex_,
                begin()+writerIndex_,
                begin()+kCheapPrepend);
      readerIndex_ = kCheapPrepend;
      writerIndex_ = readerIndex_ + readable;
      assert(readable == readableBytes());
    }
  }

 private:
  std::vector<char> buffer_;//缓冲区
  size_t readerIndex_;//读的索引
  size_t writerIndex_;//写的索引

  static const char kCRLF[];
};



