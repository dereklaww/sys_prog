import os
import signal


#Remove existing fifos if not cleaned up from last time.
try:
    os.unlink('/tmp/pe_trader_3')
    os.unlink('/tmp/pe_exchange_3')
except FileNotFoundError:
    pass

signal.signal(signal.SIGINT, lambda x,y: None) #Setup signal handler with no-op lambda expression

os.mkfifo('/tmp/pe_trader_3') #Fifos can be constructed in python
os.mkfifo('/tmp/pe_exchange_3')

r = open('/tmp/pe_trader_3','r')
w = open('/tmp/pe_exchange_3','w')

w.write('MARKET SELL CPU 3 1000;') #FIFOs can be writen to like any other file
w.flush()
#close writing pipe
w.close()

signal.pause() #mimics pause(2)
print('PYTHON: Recieved ' + r.read()) #FIFOs can be read from like any other file


print('PYTHON: ending process')

#Close reading pipe
r.close()

#cleanup fifos
os.unlink('/tmp/pe_trader_3')
os.unlink('/tmp/pe_exchange_3')
