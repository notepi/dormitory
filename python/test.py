#!/usr/bin/python
# -*- coding: UTF-8 -*-
import threading
#Timer����ʱ������Thread�������࣬
#������ָ��ʱ������һ��������
def func():
  print 'hello timer!'
  timer = threading.Timer(5, func)
  timer.start()
  
timer = threading.Timer(5, func)
timer.start()

while (1): 
	pass
	