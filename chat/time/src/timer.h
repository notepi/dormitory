#ifndef __TIMER_H__
#define __TIMER_H__


class tw_timer {
	public:
		tw_timer():prev(NULL), next(NULL){};
			
			
	public:	
		time_t expire;//延时到期时间
		void (*cb_func)(client_data*);//延时到期后操作函数
		client_data *client;
		tw_timer *next;//指向下个定时器
		tw_timer *prev;//指向上个定时器
};

class timer_list {
	
};
#endif
