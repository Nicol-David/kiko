
#include "Buffer.h"
#include <errno.h>
#include <sys/uio.h>


const char Buffer::kCRLF[] = "\r\n"; //停止符

const size_t Buffer::kCheapPrepend;//预备字节的大小
const size_t Buffer::kInitialSize;//缓冲区的大小




//这个函数是将fd中的内容读写到buffer中。
//使用readv函数来读取，读取过程分为三步：
//1.将fd中的内容直接读入buffer缓冲区中
//2.如果缓冲区可写的空间不足，将剩余的内容读取到额外空间中extrabuf
//3.将额外空间中extrabuf中的内容，重写写入buffer缓冲区中（此时buffer缓冲区大小已经别调整到合适大小）
ssize_t Buffer::readFd(int fd)
{
  // saved an ioctl()/FIONREAD call to tell how much to read
  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writable = writableBytes();
  vec[0].iov_base = begin()+writerIndex_;
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  // when there is enough space in this buffer, don't read into extrabuf.
  // when extrabuf is used, we read 128k-1 bytes at most.
  //这里使用了scatter/gatherIO模式，这是
  //scatter/gather方式是与block dma方式相对应的一种dma方式。
// 在dma传输数据的过程中，要求源物理地址和目标物理地址必须是连续的。但在有的计算机体系中，如IA，
// 连续的存储器地址在物理上不一定是连续的，则dma传输要分成多次完成。
// 如果传输完一块物理连续的数据后发起一次中断，同时主机进行下一块物理连续的传输，则这种方式即为block dma方式。
// scatter/gather方式则不同，它是用一个链表描述物理不连续的存储器，然后把链表首地址告诉dma master。
// dma master传输完一块物理连续的数据后，就不用再发中断了，而是根据链表传输下一块物理连续的数据，最后发起一次中断。 
// 很显然scatter/gather方式比block dma方式效率高。
  const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
  const ssize_t n = sockets::readv(fd, vec, iovcnt);
  //该函数只调用了一次read(2),
  if (n < 0)
  {
    //avedErrno = errno;
  }
  else if (static_cast<size_t>(n) <= writable)
  {
    writerIndex_ += n;
  } 
  else
  {
    writerIndex_ = buffer_.size();
    append(extrabuf, n - writable);
  }
  // if (n == writable + sizeof extrabuf)
  // {
  //   goto line_30;
  // }
  return n;
}

