DESCRIPTION===
		A multithreaded data server, operating over the HTTP protocol.
		md5.cc and md5.h sourced from the RSA Data Security, Inc. MD5 Message-Digest Algorithm 
		found at http://www.zedwood.com/article/cpp-md5-function

===FILES INCLUDED===

		fifoqueue.cc				Makefile
		fifoqueue.hpp				md5.cc
		httpreq.cc					md5.h
		httpreq.hpp					mythread.cc
		httpresp.cc					mythread.hpp
		httpresp.hpp				README.txt
		keyvaluestore.cc			test-httpreq.cc
		keyvaluestore.hpp			threadpool.cc
		lab2_tools/					threadpool.hpp
			lab2_workload.py
			README
			reqgen.py
		main.cc
	

===COMMANDS===

		Use the command 'make' to compile the code.
		Use 'make clean' to delete all object files and executables.
		Use './lab3 -n <N> -p <P> -l <L>' to run tests on the server, using N threads on port P. L specifies the number of key-value stores into which the key space will be divided - this was 1 for lab2.
		If no connections are received for 3 seconds, the server will terminate and compute its statistics. This also means that the server must begin receiving connections within 3 seconds of it being started.

===OUTPUT===
		The program will output the server's lifetime statistics.
