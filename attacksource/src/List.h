/*
* Copyright (c) 2008,������ʿͨ��Ϣ������޹�˾���簲ȫ��ҵ��
* All rights reserved.
*
* �ļ����ƣ�List.h
* �ļ���ʶ��
* ժ    Ҫ����װlistģ��,���ط��� ʹ���̰߳�ȫ 
*           ����ʹ�õ�����ʱ��Ҫ��ǰ�� �� ������� xx.lock() �� xx.unlock
*            
* ��ǰ�汾��1.0
* ��    �ߣ�dengwt <dengwt@ffcs.cn >
* ������ڣ�2008��11��10��
*
* ȡ���汾��
* ԭ����  ��
* ������ڣ�
*/
#ifndef _LISTHEAD_
#define _LISTHEAD_

#include <stdio.h>
#include <unistd.h>
#include <list>
#include <pthread.h>


using namespace std;

/*Ŀǰû�н� allocator ģ��ӽ�ȥ,��ʱ���ò���*/

template<class T >
class _List
{
   public:
     _List(void)
	{
	   pthread_mutex_init( &_lock,NULL);
  	  pthread_cond_init(&_cond,NULL);
	}
     ~_List(void);
     
     typedef typename list<T>::iterator iterator;
      
     void clear();
     void unlock_clear();
     bool empty();
     bool unlock_empty();
     
     typename list <T>::size_type size();
     typename list <T>::size_type unlock_size();
      
     void push_back(T elem);
     void unlock_push_back(T elem);
     void pop_front();
     void unlock_pop_front();
     
     T& front();
     T& unlock_front();
     
     typename list<T>::iterator unlock_erase( typename list<T>::iterator pos );

     //�ݲ��ṩ �Ǽ�������
     void produce(T &elem);
     void unlock_produce( T &elem);
     T& unlock_consume();
     
     void splice(typename list<T>::iterator _Where,_List<T>& _Right);
     void splice(typename list<T>::iterator _Where,_List<T>& _Right,typename list<T>::iterator _First);
     void splice(typename list<T>::iterator _Where,_List<T>& _Right,typename list<T>::iterator _First,typename list<T>::iterator _Last);
     void reverse();
     
     
     //���޼�������
     typename list<T>::iterator begin();
     typename list<T>::iterator  end();
     
     void unlock();
     void lock();
    
      
     pthread_mutex_t _lock;
     pthread_cond_t _cond;
     
    protected:
      
     
     list<T> L;


};

/*template<class T>
_List<T>::_List<T>(void)
{
    pthread_mutex_init( &_lock,NULL);
    pthread_cond_init(&_cond,NULL);
}
*/
template<class T>
_List<T>::~_List<T>(void)
{
    pthread_mutex_destroy(&_lock);
    pthread_cond_destroy(&_cond);
    L.clear();
}

template<class T>
void _List<T>::lock()
{
  // pthread_cleanup_push(PTHREAD_MUTEX_UNLOCK,(void *)&(_lock) );
   pthread_mutex_lock(&(_lock));
}

template<class T>
void _List<T>::unlock() 
{
   pthread_mutex_unlock(&_lock);
   //pthread_cleanup_pop(0);
}  

template<class T>
typename list<T>::iterator _List<T>::begin(){
  return L.begin();
}

template<class T>
typename list<T>::iterator  _List<T>::end(){
  return L.end();
}
     

template<class T>
bool _List<T>::empty()
{
    bool ret;
    lock();
    ret = L.empty();
    unlock();
    
    return ret;
}

template<class T>
bool _List<T>::unlock_empty()
{
    return L.empty();    
}


template<class T>
void _List<T>::clear()
{
    lock();
    L.clear();
    unlock();
}

template<class T>
void _List<T>::unlock_clear()
{
    L.clear();
}


template<class T>
typename list <T>::size_type _List<T>::size()
{
    typename list <T>::size_type ret;
    lock();
    ret = L.size();
    unlock();
    
    return ret;
}

template<class T>
typename list <T>::size_type _List<T>::unlock_size()
{
    return L.size();
}


template<class T>
void _List<T>::reverse()
{
    lock();
    L.reverse();
    unlock();
}

template<class T>
typename list<T>::iterator _List<T>::unlock_erase( typename list<T>::iterator pos )
{
   return L.erase(pos);
}
  
template<class T>
void _List<T>::unlock_pop_front()
{
    L.pop_front();
}

template<class T>
void _List<T>::pop_front()
{
    lock();
    L.pop_front();
    unlock();
}
  
  
template<class T>
void _List<T>::push_back( T elem)
{
    lock();
    L.push_back(elem);
    unlock();
}

template<class T>
void _List<T>::unlock_push_back( T elem)
{
    L.push_back(elem);
}

template<class T>
void _List<T>::produce( T &elem)
{   
    //printf("produce lock\n");
    lock();

     L.push_back(elem);
     if (L.size() == 1) pthread_cond_signal(&_cond);
    
    unlock();
    //printf("produce unlock\n");
}

template<class T>
void _List<T>::unlock_produce( T &elem)
{   
     L.push_back(elem);
     if (L.size() == 1) pthread_cond_signal(&_cond); 
}


template<class T>
T& _List<T>::unlock_consume()
{   
    while(L.empty()){
        pthread_cond_wait(&_cond,&_lock);
    }

    return L.front();
}


//��ʱ�ڵ���ڴ���Ҫ�Լ��ͷ�
template<class T>
T& _List<T>::front() 
{
    lock();
    T& ret =  L.front();
    unlock();
    return  ret;
}

template<class T>
T& _List<T>::unlock_front() 
{
    return  L.front();
    
}


template<class T>
void _List<T>::splice(typename list<T>::iterator _Where,_List<T> &_Right)
{
    lock();
    pthread_mutex_lock(&_Right._lock);//��������
    
    L.splice(_Where,_Right.L);
    
    unlock();
    pthread_mutex_unlock(&_Right._lock);//��������
    
    return;

}
template<class T>
void _List<T>::splice(typename list<T>::iterator _Where,_List<T>& _Right,typename list<T>::iterator _First)
{
    lock();
    pthread_mutex_lock(&_Right._lock);//��������
    
    L.splice(_Where,_Right.L,_First);
    
    unlock();
    pthread_mutex_unlock(&_Right._lock);//��������
}

template<class T>
void _List<T>::splice(typename list<T>::iterator _Where,_List<T>& _Right,typename list<T>::iterator _First,typename list<T>::iterator _Last)
{
    lock();
    pthread_mutex_lock(&_Right._lock);//��������
    
    L.splice(_Where,_Right.L,_First,_Last);
    
    unlock();
    pthread_mutex_unlock(&_Right._lock);//��������
}

#endif

