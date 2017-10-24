CC = g++
CCFLAGS = -std=c++11 -g
LDFLAGS = -lpthread
OBJS = main.o threadpool.o mythread.o httpreq.o httpresp.o md5.o

lab3 : $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o lab3 $(LDFLAGS)

main.o: main.cc md5.h threadpool.hpp mythread.hpp keyvaluestore.cc keyvaluestore.hpp fifoqueue.cc fifoqueue.hpp httpresp.hpp httpreq.hpp
	$(CC) $(CCFLAGS) -c main.cc -o main.o

threadpool.o: threadpool.cc threadpool.hpp mythread.hpp keyvaluestore.cc keyvaluestore.hpp fifoqueue.cc fifoqueue.hpp
	$(CC) $(CCFLAGS) -c threadpool.cc -o threadpool.o

mythread.o: mythread.cc mythread.hpp keyvaluestore.cc keyvaluestore.hpp fifoqueue.cc fifoqueue.hpp
	$(CC) $(CCFLAGS) -c mythread.cc -o mythread.o

httpreq.o: httpreq.cc httpreq.hpp
	$(CC) $(CCFLAGS) -c httpreq.cc -o httpreq.o

httpresp.o: httpresp.cc httpresp.hpp
	$(CC) $(CCFLAGS) -c httpresp.cc -o httpresp.o

md5.o: md5.cc md5.h
	$(CC) $(CCFLAGS) -c md5.cc -o md5.o

clean :
	rm *.o lab3
