#!/usr/bin/python
# -*- coding: UTF-8 -*-
import threading
#Timer（定时器）是Thread的派生类，
#用于在指定时间后调用一个方法。
def func():
  print 'hello timer!'
  timer = threading.Timer(5, func)
  timer.start()
  
timer = threading.Timer(5, func)
timer.start()

while (1): 
	pass
	