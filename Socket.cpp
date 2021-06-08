#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/epoll.h>
#include <fcntl.h>


#include "Socket.h"


int sockets::SocketBind(int port){ //最基础的网络编程代码，这里是完成了一个封装
	/*关于socket*/
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);//IPV4, TCPint optval = 1;//设置为此套接字结束后，其对应的端口，可以立即被新的套接字绑定
	assert(listen_fd >= 0);
	int optval = 1;//设置为此套接字结束后，其对应的端口，可以立即被新的套接字绑定
	setsockopt(listen_fd, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof(optval));
	// 设置服务器IP和Port，和监听描述副绑定
	struct sockaddr_in server_addr;
	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons((unsigned short)port);
	int ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	assert(ret != -1);
	ret = listen(listen_fd, 19);
	assert(ret != -1);
	printf("\ncreatfd:%d\n", listen_fd);
	return listen_fd;
}

ssize_t sockets::read(int sockfd, void *buf, size_t count)
{
  return ::read(sockfd, buf, count);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
  return ::write(sockfd, buf, count);
}


uint64_t sockets::hostToNetwork64(uint64_t host64)
{
  return htobe64(host64);
}

uint32_t sockets::hostToNetwork32(uint32_t host32)
{
  return htobe32(host32);
}

uint16_t sockets::hostToNetwork16(uint16_t host16)
{
  return htobe16(host16);
}

uint64_t sockets::networkToHost64(uint64_t net64)
{
  return be64toh(net64);
}

uint32_t sockets::networkToHost32(uint32_t net32)
{
  return be32toh(net32);
}

uint16_t sockets::networkToHost16(uint16_t net16)
{
  return be16toh(net16);
}


ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
  return ::readv(sockfd, iov, iovcnt);
}
