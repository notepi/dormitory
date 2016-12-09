/*
* Copyright (c) 2008,福建富士通信息软件有限公司网络安全事业部
* All rights reserved.
*
* 文件名称：Map.h
* 文件标识：
* 摘    要：封装map模版,方法 使其线程安全 
*           其中使用迭代器时需要在前边 和 后面加上 xx.lock() 和 xx.unlock
*            
* 当前版本：1.0
* 作    者：dengwt <dengwt@ffcs.cn >
* 完成日期：2008年11月12日
*
* 取代版本：
* 原作者  ：
* 完成日期：
*/

#ifndef _MAPHEAD_
#define _MAPHEAD_

#include <stdio.h>
#include <unistd.h>
#include <map>
#include <pthread.h>

using namespace std;


template<class K ,class T>
class _Map
{
   public:
     typedef typename map<K,T>::iterator iterator;
      
     _Map(void)
	{
	pthread_mutex_init( &_lock,NULL);

	}
     ~_Map(void);
     
     
     typename map<K,T>::iterator insert( typename map<K,T>::iterator pos, const pair<K,T> &val );
     void insert( typename map<K,T>::iterator start, typename map<K,T>::iterator end );
     pair<typename map<K,T>::iterator, bool> insert( const pair<K,T> &val );   
     
     typename map<K,T>::iterator unlock_insert( typename map<K,T>::iterator pos, const pair<K,T> &val );
     void unlock_insert( typename map<K,T>::iterator start, typename map<K,T>::iterator end );
     pair<typename map<K,T>::iterator, bool> unlock_insert( const pair<K,T> &val );  
     
     void erase(typename map<K,T>::iterator _Where);
     void erase(typename map<K,T>::iterator _First, typename map<K,T>::iterator _Last);
     typename map<K,T>::size_type erase(K& _Key);
      
     void unlock_erase(typename map<K,T>::iterator _Where);
     void unlock_erase(typename map<K,T>::iterator _First, typename map<K,T>::iterator _Last);
     typename map<K,T>::size_type unlock_erase(K& _Key);
      
   
   
     typename map<K,T>::iterator find( const K &key );      
     typename map<K,T>::iterator unlock_find( const K &key );   
      
     void clear();
     void unlock_clear();
     
     //暂时无枷锁的函数
     typename map<K,T>::size_type size(){
       return M.size();
     }
     
     typename map<K,T>::iterator begin(){
       
       return M.begin();
     }
     
     typename map<K,T>::iterator  end(){
       return M.end();
     }
     
     void lock(){
        pthread_mutex_lock(&_lock);
        return;
     }
     
     void unlock(){
        pthread_mutex_unlock(&_lock);
        return;
     } 
     
     T &operator[](const K &key );
     
    protected:
     
     pthread_mutex_t _lock;
     pthread_cond_t _cond;
     map<K,T> M;
    private :
     
};

/*template<class K ,class T>
_Map<K,T>::_Map<K,T>()
{
    pthread_mutex_init( &_lock,NULL);
}
*/
template<class K ,class T>
_Map<K,T>::~_Map<K,T>()
{
    pthread_mutex_init( &_lock,NULL);
    M.clear();
}

template<class K ,class T>
T &_Map<K,T>::operator[](const K &key )
{
    lock();
    T& tret=M[key];
    unlock();
    return tret;
}

     
template<class K ,class T>
pair<typename map<K,T>::iterator, bool> _Map<K,T>::insert( const pair<K,T> &val )
{
    lock();
    pair<typename map<K,T>::iterator, bool> tpair=M.insert(val);
    unlock();
    return tpair;
}

template<class K ,class T>
typename map<K,T>::iterator _Map<K,T>::insert( typename map<K,T>::iterator pos, const pair<K,T> &val )
{
    lock();
    typename map<K,T>::iterator iter =M.insert(pos,val);
    unlock();
    return iter;
}
  
template<class K ,class T>
void _Map<K,T>::insert( typename map<K,T>::iterator start, typename map<K,T>::iterator end )
{

    lock();
    M.insert(start,end);
    unlock();
}
 
 
template<class K ,class T>
pair<typename map<K,T>::iterator, bool> _Map<K,T>::unlock_insert( const pair<K,T> &val )
{
    return M.insert(val);
}

template<class K ,class T>
typename map<K,T>::iterator _Map<K,T>::unlock_insert( typename map<K,T>::iterator pos, const pair<K,T> &val )
{
    return M.insert(pos,val);
}
  
template<class K ,class T>
void _Map<K,T>::unlock_insert( typename map<K,T>::iterator start, typename map<K,T>::iterator end )
{

    M.insert(start,end);

}
  
  

template<class K ,class T>
typename map<K,T>::iterator  _Map<K,T>::find( const K &key )
{
    typename map<K,T>::iterator iter;
    lock();
    iter = M.find(key);
    unlock();
    
    return iter;
}

template<class K ,class T>
typename map<K,T>::iterator  _Map<K,T>::unlock_find( const K &key )
{
    return  M.find(key);
   
}


template<class K ,class T>
void _Map<K,T>::erase(typename map<K,T>::iterator _Where)
{

   lock();
     M.erase(_Where);
   unlock();
   
}

template<class K ,class T>
void _Map<K,T>::erase(typename map<K,T>::iterator _First, typename map<K,T>::iterator _Last)
{
   lock();
     M.erase(_First,_Last);
   unlock();

}

template<class K ,class T>
typename map<K,T>::size_type _Map<K,T>::erase(K& _Key)
{
   typename map<K,T>::size_type size;
    
   lock();
     size =M.erase(_Key);
   unlock();
   return size;
    
}

template<class K ,class T>
void _Map<K,T>::unlock_erase(typename map<K,T>::iterator _Where)
{
    M.erase(_Where);
   
}

template<class K ,class T>
void _Map<K,T>::unlock_erase(typename map<K,T>::iterator _First, typename map<K,T>::iterator _Last)
{
   M.erase(_First,_Last);
   
}

template<class K ,class T>
typename map<K,T>::size_type _Map<K,T>::unlock_erase(K& _Key)
{
   return M.erase(_Key);
  
    
}


template<class K ,class T>
void _Map<K,T>::clear()
{
   lock();
    M.clear();
   unlock();
}

template<class K ,class T>
void _Map<K,T>::unlock_clear()
{
    M.clear();
}


#endif
