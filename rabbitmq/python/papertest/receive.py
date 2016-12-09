#!/usr/bin/env python  
import pika  
  
connection = pika.BlockingConnection(pika.ConnectionParameters(  
        host='localhost'))  
channel = connection.channel()  
  
channel.queue_declare(queue='hello1',
					  auto_delete=True)  
  
print ' [*] Waiting for messages. To exit press CTRL+C'  
  
def callback(ch, method, properties, body):  
    print " [x] Received %r" % (body,)  
  
channel.basic_consume(callback,  
                      queue='hello1',  
                      no_ack=True)  
  
channel.start_consuming()