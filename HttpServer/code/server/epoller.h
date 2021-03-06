#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>  // epoll_ctl()
#include <fcntl.h>      // fcntl()
#include <unistd.h>     // close()
#include <vector>
#include <assert.h>

class Epoller
{
public:
    Epoller(int maxEvent=1024);
    ~Epoller();

    bool AddFd(int fd, uint32_t events_);
    
    bool ModFd(int fd, uint32_t events_);
    
    bool DelFd(int fd);
    
    int Wait(int timeouts);     // 就绪文件描述符个数

    int GetEventFd(size_t i) const;

    uint32_t GetEvents(size_t i) const;

private:
    int epollFd_;
    std::vector<struct epoll_event> events_;

};


#endif //EPOLLER_H