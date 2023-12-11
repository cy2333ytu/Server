#include"utils.h"
#include"../log/Logging.h"

#include<sys/socket.h>
#include<unistd.h>
#include<signal.h>
#include<cstring>
#include<fcntl.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netinet/tcp.h>

namespace ccy{
const int MAXBUFF = 4096;

ssize_t readN(int fd, void *buf, size_t n){
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readTotal = 0;

    char *ptr = (char*) buf;
    while(nleft > 0){
        nread = read(fd, ptr, nleft);
        if(nread < 0){
            if(errno == EINTR) nread = 0;
            else if(errno == EAGAIN) return readTotal;
                else 
                    return -1;
        }else if(nread == 0) break;

        readTotal += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readTotal;
}

ssize_t readN(int fd, std::string &inBuffer, bool &zero){
    ssize_t nread = 0;
    ssize_t readTotal = 0;

    while(true){
        char buff[MAXBUFF];
        if((nread = read(fd, buff, MAXBUFF)) < 0) {
            if (errno == EINTR)
                continue;
                else if (errno == EAGAIN) return readTotal;
                    else {
                        perror("read error");
                        return -1;
                    }
        } else if (nread == 0) {
                zero = true;
                break;
            }
    readTotal += nread;
    inBuffer += std::string(buff, buff + nread);
  }
    return readTotal;
}

ssize_t readN(int fd, std::string &inBuffer) {
    ssize_t nread = 0;
    ssize_t readTotal = 0;

    while (true) {
        char buff[MAXBUFF];
        if ((nread = read(fd, buff, MAXBUFF)) < 0) {
            if (errno == EINTR) {
                continue;  // Retry on interrupted system call
            } else if (errno == EAGAIN) {
                return readTotal;  // Return readTotal for non-blocking I/O
            } else {
                perror("read error");
                return -1;  // Other read errorCCY
            }
        } else if (nread == 0) {
            break;  // End of file
        }

        readTotal += nread;
        inBuffer.append(buff, nread);  // Append to the string
    }

    return readTotal;
}

ssize_t writeN(int fd, void *buf, size_t n){
    size_t nleft = n;
    size_t nwritten = 0;
    size_t writeTotal = 0;

    char *ptr = (char*)buf;
    while(nleft > 0){
        if((nwritten = write(fd, buf, nleft)) <= 0){
            if(nwritten < 0){
                if(errno == EINTR){
                    nwritten = 0;
                    continue;
                }else if(errno == EAGAIN){
                    return writeTotal;
                }else return -1;
            }
        }
        writeTotal += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeTotal;
}

ssize_t writeN(int fd, std::string &sbuf) {
    size_t nleft = sbuf.size();
    ssize_t nwritten = 0;
    ssize_t writeTotal = 0;
    const char *ptr = sbuf.data(); 

    while (nleft > 0) {
        nwritten = write(fd, ptr, nleft);

        if (nwritten < 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else if (errno == EAGAIN) {
                break;
            } else {
                perror("writeN error");
                return -1;
            }
        } else if (nwritten == 0) {
            break;
        }

        writeTotal += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }

    // write fail
    if (writeTotal == static_cast<ssize_t>(sbuf.size())) {
        sbuf.clear();
    } else {
        sbuf = sbuf.substr(writeTotal);
    }

    return writeTotal;
}

/*
    ensures that the process ignores the SIGPIPE signal, 
    preventing it from terminating abruptly when attempting
    communication over pipes or sockets,in scenarios like 
    handling client connections where unexpected disconnections
    should not lead to a server's premature termination
*/
void handle_for_Singlepipe(){
    struct sigaction sa;
    std::memset(&sa, SIGPIPE, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;

    if(sigaction(SIGPIPE, &sa, nullptr) != 0){
        LOG <<"Failed to set SIGPIPE handler." << strerror(errno);
    }else{
        LOG <<"SIGPIPE handler set to ignore.";
    }
    
}

int setSocketNonBlocking(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1) return -1;
    
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1) return -1;
    
    return 0;
}

void setSocketNodelay(int fd){
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}

void setSocketNoLinger(int fd){
    struct linger linger_;
    linger_.l_linger = 30;
    linger_.l_onoff  = 1;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&linger_, sizeof(linger_));
}

void shutDownWR(int fd){
    shutdown(fd, SHUT_WR);
}

int socket_Bind_Listen(int port){
    if(port < 0 || port > 65535) return -1;
    int listenFd = 0;
    if((listenFd = socket(AF_INET, SOCK_STREAM, 0)) ==  -1) return -1;

    int optval = 1;
    if(setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optval,
        sizeof(optval)) == -1){
            close(listenFd);
            return -1;
        }
    struct sockaddr_in serverAddr;
    bzero((char*)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons((unsigned short)port);
    if(bind(listenFd, (struct sockaddr*)&serverAddr, 
        sizeof(serverAddr)) == -1){
            close(listenFd);
            return -1;
    }
    if(listen(listenFd, 2048) == -1){
        close(listenFd);
        return -1;
    }
    if(listenFd == -1){
        close(listenFd);
        return -1;
    }
    return listenFd;
}

}