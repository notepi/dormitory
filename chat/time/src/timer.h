#ifndef __TIMER_H__
#define __TIMER_H__


class tw_timer {
	public:
		tw_timer():prev(NULL), next(NULL){};
			
			
	public:	
		time_t expire;//��ʱ����ʱ��
		void (*cb_func)(client_data*);//��ʱ���ں��������
		client_data *client;
		tw_timer *next;//ָ���¸���ʱ��
		tw_timer *prev;//ָ���ϸ���ʱ��
};

class timer_list {
	
};
#endif
