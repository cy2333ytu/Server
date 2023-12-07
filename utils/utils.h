#pragma once
#include<cstdlib>
#include<string>

namespace ccy{

ssize_t readN(int fd, void *buf, size_t n);
ssize_t readN(int fd, std::string &inBuffer, bool &zero);
ssize_t readN(int fd, std::string &inBuffer);
ssize_t writeN(int fd, void *buff, size_t n);
ssize_t writeN(int fd, std::string &sbuff);

void handle_for_Singlepipe();
int setSocketNonBlocking(int fd);
void setSocketNodelay(int fd);
void setSocketNoLinger(int fd);
void shutDownWR(int fd);
int socket_Bind_Listen(int port);

}