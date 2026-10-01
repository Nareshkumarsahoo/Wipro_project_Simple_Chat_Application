CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra

SERVER_SRC = src/ChatServer.cpp
CLIENT_SRC = src/ChatClient.cpp

SERVER_OUT = server
CLIENT_OUT = client


all: server client


server:
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o $(SERVER_OUT) -pthread -lsqlite3


client:
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $(CLIENT_OUT) -pthread


clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT)


run-server: server
	./$(SERVER_OUT)


run-client: client
	./$(CLIENT_OUT)
