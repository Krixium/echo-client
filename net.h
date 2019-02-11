#ifndef NET_H
#define NET_H

int createConnectedSocket(int *sock, const char* address, const short port);
void sendAndListen(const int sock, const char* message, char *buffer, const unsigned int length);
void readAllFromSocket(const int sock, char *buffer, const unsigned int size);

#endif // NET_H