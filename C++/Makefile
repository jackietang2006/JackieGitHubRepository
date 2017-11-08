tcpServer: tcpServer.cc
	gcc -std=c++11 -o $@ $@.cc
tcpClient: tcpClient.cc
	gcc -std=c++11 -o $@ $@.cc
udpServer: udpServer.cc
	gcc -std=c++11 -o $@ $@.cc
udpClient: udpClient.cc
	gcc -std=c++11 -o $@ $@.cc
rawtcp: rawtcp.cc
	gcc -std=c++11 -o $@ $@.cc
rawudp: rawudp.cc RawUdp.cc utilities.cc
	g++ -std=c++11 -o $@ rawudp.cc RawUdp.cc utilities.cc
