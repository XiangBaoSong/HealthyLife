#include<vector>
#include<sys/epoll.h>
#include<iostream>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include"stdio.h"

#include"epoll_demultiplexer.h"

EpollDemultiplexer::EpollDemultiplexer() : _max_fd(0) {
	_epoll_fd = epoll_create(1024);
}

EpollDemultiplexer::~EpollDemultiplexer(){
	close(_epoll_fd);
}

int EpollDemultiplexer::wait_event(std::map<int, EventHandler*>& handlers, int timeout){
	std::vector<struct epoll_event> events(_max_fd);
	int num = epoll_wait(_epoll_fd, &events[0], _max_fd, timeout);
	if (num < 0){
		std::cout<<"epoll wait error :"<<strerror(errno)<<std::endl;
		return num;
	}

	printf("得到连接数:%d\n", num);

	for (int i = 0; i < num; i++){
		Handle handle = events[i].data.fd;
		if ( (EPOLLHUP | EPOLLERR) & events[i].events ) {
			assert( NULL != handlers[handle]);
			(handlers[handle])->handle_error();
		
			printf("EPOLLERR | EPOLLHUP\n");
		}
		else if ( (EPOLLIN) & events[i].events || EPOLLRDNORM & events[i].events){
				assert(handlers[handle] != NULL);
				(handlers[handle])->handle_read();

				printf("EPOLLIN \n");
			}
		else if ( EPOLLOUT & events[i].events || EPOLLWRNORM & events[i].events) {
				(handlers[handle])->handle_write();

				printf("EPOLLOUT \n");
			}
		else {
			printf("意外的事件，事件编号：%d \n",events[i].events);
		}

	}

	return num;
}

int EpollDemultiplexer::regist(Handle handle, Event evt){
	struct epoll_event ev;
	ev.data.fd = handle;
	if (evt & WriteEvent){
		ev.events |= EPOLLIN;
	}

	if (evt & WriteEvent) {
		ev.events |= EPOLLOUT;
	}
	ev.events |= EPOLLET;//水平触发模式更加简单

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, handle, &ev) != 0){
		if (ENOENT == errno){
			if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, handle, &ev) != 0){
				std::cerr <<"epoll add error"<< errno <<std::endl;
				return -errno;
			}
		}
	}

	++_max_fd;
	printf("用有连接数:%d \n", _max_fd);

	return 0;
}

int EpollDemultiplexer::remove(Handle handle) {
	struct epoll_event ev;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, handle, &ev) != 0){
		std::cerr <<"epoll_del error"<<strerror(errno)<<std::endl;
		return -errno;
	}
	
	printf("handle=%d断开连接\n", handle);
	--_max_fd;
	return 0;
}


