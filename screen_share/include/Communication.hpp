#include <WinSock2.h>
#include <iostream>
#include <string>
#include <vector>

#define RECV_SIZE_FIELD 4

class Stream {
public:
	Stream(std::string address, UINT16 port);
	~Stream();
	std::vector<char> RecvBySize();
	void SendBySize(char* data, int size);
	//UINT8* Recv(int amount);
private:
	SOCKET mSocket;
	std::string mAddress;
	UINT16 mPort;
};