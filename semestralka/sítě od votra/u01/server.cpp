//============================================================================
// Name        : server.cpp
// Author      : Jakub Votrubec
// Version     : 
// Copyright   : Your copyright notice
// Description : TCP server - PSI u01
//============================================================================

#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <array>
#include <functional>
#include <string>
#include <utility>
#include <thread>
#include <cmath>
#include <sstream>
#include <iomanip>
using namespace std;

#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <wait.h>

#define BUFFER_SIZE           (1024* 1)
#define TIMEOUT               1
#define TIMEOUT_RECHARGING    5 
#define SERVER_BACKLOG        10

enum class EState
{
	// INITIAL HANDSHAKE
	HANDSHAKE_USERNAME, 
	HANDSHAKE_KEY, 
	HANDSHAKE_CONFIRMATION, 
	HANDSHAKE_KEY_REQUEST,
	LOGIN_OK,
	// MOVING
	MOVE,
	PICK_UP,
	// MESSAGE RETRIEVAL
	MESSAGE,
	// ERRORS
	LOGIN_FAILED, 
	SYNTAX_ERROR,
	LOGIC_ERROR,
	KEY_OUT_OF_RANGE_ERROR,
	// LOGOUT
	LOGOUT
};

enum class EDirection : char
{
	NORTH = 'N', 
	SOUTH = 'S', 
	EAST = 'E', 
	WEST = 'W',
	UNKNOWN = 'U'
};

enum class EMove : char
{
	MOVE = 'M', 
	TURN_LEFT = 'L',
	TURN_RIGHT = 'R',
	UNKNOWN = 'U'
};

struct TPos
{
	int t_PosX;
	int t_PosY;
	TPos() :
		t_PosX(100),
		t_PosY(100)
	{}
	TPos(int x, int y) :
		t_PosX(x),
		t_PosY(y)
	{}
	bool   operator == (const TPos & other) { return (this->t_PosX == other.t_PosX && this->t_PosY == other.t_PosY); }
	bool   operator != (const TPos & other) { return !(*this == other); }
	double distance (const TPos & other)  { return sqrt(pow(this->t_PosX - other.t_PosX, 2.0) + pow(this->t_PosY - other.t_PosY, 2.0)); }
};

class CRobotHandler
{
	public:
		CRobotHandler(int client_socket);
		~CRobotHandler();
		int Start();
	private:
		int  readInput();
		int  proccessInput();
		void setupStateFuncs();
		void setupServerClientMsgs();
		void setupKeyIds();
		void setupMoveMsgs();
		uint userHash(size_t key, bool server);
		void nextMove();
		void sendMove(const EMove move);
		void obstacleMove();
		int  checkOK(const string & msg, TPos & rcvPos);
		bool checkLeftovers();
	private:
		int                    m_ClientSocket;
		int                    m_MoveCnt;
		bool                   m_MoveInit;
		bool	               m_End;
		bool                   m_Recharging;
		TPos                   m_CurrPos;
		size_t                 m_KeyId;
		string                 m_InputData;
		string                 m_RoboUsername;
		string                 m_SecretMsg;
		EMove                  m_LastMove;
		EState                 m_CurrState;
		EDirection             m_CurrDir;
		queue<string>          m_Msgs;
		queue<EMove>           m_MoveQueue;
		vector<pair<int, int>> m_KeyIds;
		map<EState, string>    m_ServerClientMsgs;
		map<EMove, string>     m_MoveMsgs;
		map<EState, function<EState(string)>> m_StateFuncs;
};

int handle_connection(int cl_sckt);
bool isNum(const string & snum);
void printMoveQueue(const queue<EMove> & moveQueue);
string convertForScreen(string data);

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return -1;
	}

	// Create server socket
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0)
	{
		printf("Could not create a socket\n");
		return -1;
	}

	// Set port according to input
	uint port = atoi(argv[1]);
	if(port > 65535
       || port < 1024)
	{
		perror("Invalid port! (1024 < port < 65535)");
		close(s);
		return -1;
	}

	// Setup address
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind socket to interface
	if(bind(s, (struct sockaddr*) &address, sizeof(address)) < 0)
	{
		perror("Problem with bind()");
		close(s);
		return -1;
	}

	// Set socket to listen for connections
	if(listen(s, SERVER_BACKLOG) < 0)
	{
		perror("Problem with listen()");
		close(s);
		return -1;
	}

	printf("Listening on port %u\n", port);

	vector<thread> threads;
	struct sockaddr_in remote_address;
    socklen_t size;

    while(true){
        // Cekam na prichozi spojeni
        int client_socket = accept(s, (struct sockaddr *) &remote_address, &size);
        if(client_socket < 0){
			printf("client_socket = %d\n", client_socket);
            perror("Problem s accept()!");
            close(s);
            return -1;
        }
		printf("A robot connected!\n");
		
		pid_t pid = fork();
		if(pid == 0)
		{
			close(s);
			return handle_connection(client_socket);
		}
		int status = 0;
		waitpid(0, &status, WNOHANG);
		close(client_socket);
	}

	close(s);
	return 0;	
}

int handle_connection(int cl_sckt)
{
	CRobotHandler robot_handler(cl_sckt);
	printf("Starting Robot Handler\n");
	return robot_handler.Start();
}

CRobotHandler::CRobotHandler(int client_socket) :
	m_ClientSocket(client_socket),
	m_End(false),
	m_Recharging(false),
	m_CurrState(EState::HANDSHAKE_USERNAME),
	m_CurrDir(EDirection::UNKNOWN),
	m_MoveInit(true),
	m_MoveCnt(0),
	m_LastMove(EMove::UNKNOWN)
{
	// Setup error msgs
	setupServerClientMsgs();
	// Setup key ids
	setupKeyIds();
	// Setup State map
	setupStateFuncs();
	// Setup move msgs
	setupMoveMsgs();
}

CRobotHandler::~CRobotHandler()
{
	close(m_ClientSocket);
}

int CRobotHandler::Start()
{
	EState tmpState;
	int retval = -1;
	while(!m_End)
	{
		// Read input from client_socket and
		while(m_Msgs.empty())
		{
			retval = readInput();
			if(retval <= 0)
				return retval;
			// Proccess input into messages
			if(!proccessInput())
			{	
				m_CurrState = EState::SYNTAX_ERROR;
				break;
			}
		}
	//	printf("Proccessing msg: %s\n", m_Msgs.front().c_str());
		// Use state function
		// If next msg if RECHARGING of FULL_CHARGE, handle recharging
		if(m_Recharging && m_Msgs.front() == "FULL POWER")
		{
			printf("Recharging done\n");
			m_Recharging = false;
			m_Msgs.pop();
			continue;
		}
		if(m_Msgs.front() == "RECHARGING" && !m_Recharging)
		{
			printf("Recharging\n");
			m_Recharging = true;
			m_Msgs.pop();
			continue;
		}
		if((m_Msgs.front() == "FULL POWER" && !m_Recharging)
		|| (m_Recharging && m_Msgs.front() != "FULL POWER"))
		{
			m_CurrState = EState::LOGIC_ERROR;
		}
		// Handle current state
//		printf("Running state function\n");
		m_CurrState = m_StateFuncs.at(m_CurrState)(m_Msgs.front());
		// Check for errors
		switch(m_CurrState)
		{
			case EState::LOGIN_FAILED:
			case EState::SYNTAX_ERROR:
			case EState::LOGIC_ERROR:
			case EState::KEY_OUT_OF_RANGE_ERROR:
				m_StateFuncs.at(m_CurrState)("ERROR");
				break;
			default:
				break;
		}
	}
	return 0;
}

int CRobotHandler::readInput()
{
	// Read and store input using recv()
	// Check for TIMEOUT or TIMEOUT_RECHARGING if in recharging state

	struct timeval timeout;
	timeout.tv_usec = 0;
	if(m_Recharging)
		timeout.tv_sec = TIMEOUT_RECHARGING;
	else
		timeout.tv_sec = TIMEOUT;

	fd_set sockets;
	int retval;
	char buffer[BUFFER_SIZE];
	
	FD_ZERO(&sockets);
	FD_SET(m_ClientSocket, &sockets);
	retval = select(m_ClientSocket + 1, &sockets, NULL, NULL, &timeout);
	if(retval < 0)
	{
		perror("Error in select(): ");
		return -1;
	}
	if(!FD_ISSET(m_ClientSocket, &sockets))
	{
		printf("Connection timeout!\n");
		return 0;
	}
	int bytes_read = recv(m_ClientSocket, buffer, BUFFER_SIZE - 1, 0);
	if(bytes_read <= 0)
	{
	//	printf("Read %d bytes: %s\n", bytes_read, buffer);
		perror("Error in reading from socket: ");
		return -2;
	}
	/* Změněno během prezentace */
	for(int i = 0; i < bytes_read; i++)
		m_InputData += buffer[i];
	printf("Input: %s\n", convertForScreen(m_InputData).c_str());
	return 1;
}

int CRobotHandler::proccessInput()
{
	start:
	// Proccess raw input data into messages
	string delim = "\a\b";
	size_t pos;
	string msg;
	while((pos = m_InputData.find(delim)) != string::npos)
	{
		msg = m_InputData.substr(0, pos);
		// Add messages to message queue
		m_Msgs.push(msg);
		m_InputData.erase(0, pos + delim.length());
	}
	return checkLeftovers();
	// Leftover msgs (if any), are left in m_InputData
}

void CRobotHandler::setupStateFuncs()
{
	// Insert lambda funtions into state map
	m_StateFuncs.insert(make_pair(EState::HANDSHAKE_USERNAME,     [this](const string msg){
	//	printf("HANDSHAKE_USERNAME\n");
		int usernameLen = 18;
		if(msg.length() > usernameLen 
		|| msg.length() < 0
		|| msg.find("\a\b", 0) != string::npos)
			return EState::SYNTAX_ERROR;
		m_RoboUsername = msg;
		send(m_ClientSocket, 
		     m_ServerClientMsgs.find(EState::HANDSHAKE_KEY_REQUEST)->second.c_str(), 
			 m_ServerClientMsgs.find(EState::HANDSHAKE_KEY_REQUEST)->second.length(), 
			 0);
		m_Msgs.pop();
		return EState::HANDSHAKE_KEY;
	}));
	m_StateFuncs.insert(make_pair(EState::HANDSHAKE_KEY,          [this](const string msg){
	//	printf("HANDSHAKE_KEY\n");
		int keyLen = 3;
		if(msg.length() > 3 || msg.length() < 0)
			return EState::SYNTAX_ERROR;
		try{m_KeyId = stoi(msg);}catch(const std::invalid_argument& ia)
		{
			return EState::SYNTAX_ERROR;
		}
		if(m_KeyId < 0 || m_KeyId >= m_KeyIds.size())
			return EState::KEY_OUT_OF_RANGE_ERROR;
		uint hash = userHash(m_KeyId, true);
		string confirmation = to_string(hash) + "\a\b";
		send(m_ClientSocket, confirmation.c_str(), confirmation.length(), 0);
		m_Msgs.pop();
		return EState::HANDSHAKE_CONFIRMATION;
	}));
	m_StateFuncs.insert(make_pair(EState::HANDSHAKE_CONFIRMATION, [this](const string msg){
	//	printf("HANDSHAKE_CONFIRMATION\n");
		uint cHash = 0;
		int hashLen = 5;
		if(msg.length() > 5 || msg.length() < 0 || !isNum(msg))
			return EState::SYNTAX_ERROR;
		try{cHash = stoi(msg);}catch(const std::invalid_argument& ia)
		{
			return EState::SYNTAX_ERROR;
		}
		if(cHash != userHash(m_KeyId, false))
			return EState::LOGIN_FAILED;
		send(m_ClientSocket, 
		     m_ServerClientMsgs.find(EState::LOGIN_OK)->second.c_str(), 
			 m_ServerClientMsgs.find(EState::LOGIN_OK)->second.length(), 
			 0);
		m_Msgs.pop();
		m_CurrState = EState::MOVE;
		/*
		int moveRet = moveInit();
		if(moveRet == 0)
		{
			m_End = true;
			return EState::LOGOUT;
		}
		else if(moveRet == -1)
			return EState::SYNTAX_ERROR;
		// stading at the target, pick up message
		else if(moveRet == 2)
		{
			send(
				m_ClientSocket, 
				m_ServerClientMsgs.at(EState::PICK_UP).c_str(), 
				m_ServerClientMsgs.at(EState::PICK_UP).length(), 
				0
			);
			return EState::MESSAGE;
		}
		*/
		m_MoveQueue.push(EMove::MOVE);
		m_MoveQueue.push(EMove::MOVE);
		if(m_MoveQueue.empty())
			nextMove();
		sendMove(m_MoveQueue.front());
		m_MoveQueue.pop();
		m_LastMove = EMove::UNKNOWN;
		return EState::MOVE;
	}));
	m_StateFuncs.insert(make_pair(EState::MOVE,                   [this](const string msg){
	//	printf("MOVE\n");
		printMoveQueue(m_MoveQueue);
		TPos newPos;
		if(!checkOK(msg, newPos))
			return EState::SYNTAX_ERROR;
		if(newPos == TPos(0, 0))
		{
			m_CurrPos = newPos;
			return EState::PICK_UP;
		}
		if(m_LastMove == EMove::UNKNOWN)
		{
		//	printf("Last move unknown\n");
			m_CurrPos = newPos;
			m_MoveQueue.push(EMove::MOVE);
		}
		if(m_MoveInit && m_CurrPos != newPos)
		{
			// determine the direction the robot is facing
			if(m_CurrPos.t_PosX < newPos.t_PosX)
				m_CurrDir = EDirection::EAST;
			else if(m_CurrPos.t_PosX > newPos.t_PosX)
				m_CurrDir = EDirection::WEST;
			else if(m_CurrPos.t_PosY < newPos.t_PosY)
				m_CurrDir = EDirection::NORTH;
			else if(m_CurrPos.t_PosY > newPos.t_PosY)
				m_CurrDir = EDirection::SOUTH;
			printf("Started facing %c\n", static_cast<char>(m_CurrDir));
			m_MoveInit = false;
		}
		if(m_MoveInit && m_CurrPos == newPos && m_LastMove == EMove::MOVE)
		{
			printf("Blocked at start\n");
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
		if(!m_MoveInit && m_CurrPos == newPos && m_LastMove == EMove::MOVE)
			obstacleMove();
		if((m_LastMove == EMove::TURN_LEFT || m_LastMove == EMove::TURN_RIGHT) && m_CurrPos != newPos)
			printf("MOVEMENT DOES NOT MATCH!\n");
		m_CurrPos = newPos;
		if(m_MoveQueue.empty())
			nextMove();
		printf("Current position: (%d, %d)\n", m_CurrPos.t_PosX, m_CurrPos.t_PosY);
		sendMove(m_MoveQueue.front());
		m_MoveQueue.pop();
		m_Msgs.pop();
		return EState::MOVE;
	}));
	m_StateFuncs.insert(make_pair(EState::PICK_UP,                [this](const string msg){
	//	printf("PICK_UP\n");
		TPos newPos;
		if(!checkOK(msg, newPos))
			return EState::SYNTAX_ERROR;
		if(newPos != TPos(0, 0))
			throw("PICK_UP sem bych se neměl dostat!\n");
		send(
			m_ClientSocket, 
			m_ServerClientMsgs.at(EState::PICK_UP).c_str(), 
			m_ServerClientMsgs.at(EState::PICK_UP).length(), 
			0);
		m_Msgs.pop();
		return EState::MESSAGE;
	}));
	m_StateFuncs.insert(make_pair(EState::MESSAGE,                [this](const string msg){
	//	printf("MESSAGE\n");
		if(msg.length() > 100 || msg.length() < 0)
			return EState::SYNTAX_ERROR;
		m_SecretMsg = msg;
		send(
			m_ClientSocket,
			m_ServerClientMsgs.at(EState::LOGOUT).c_str(), 
			m_ServerClientMsgs.at(EState::LOGOUT).length(), 
		0);
		printf("Secret msg: %s\n", msg.c_str());
		m_End = true;
		m_Msgs.pop();
		return EState::LOGOUT;
	}));
	m_StateFuncs.insert(make_pair(EState::LOGIN_FAILED,           [this](const string msg){
		printf("LOGIN_FAILED\n");
		send(m_ClientSocket, 
			m_ServerClientMsgs.find(EState::LOGIN_FAILED)->second.c_str(), 
			m_ServerClientMsgs.find(EState::LOGIN_FAILED)->second.length(), 
			0);
		m_End = true;
		return EState::LOGOUT;
	}));
	m_StateFuncs.insert(make_pair(EState::SYNTAX_ERROR,           [this](const string msg){
		printf("SYNTAX_ERROR\n");
		send(m_ClientSocket, 
			m_ServerClientMsgs.find(EState::SYNTAX_ERROR)->second.c_str(), 
			m_ServerClientMsgs.find(EState::SYNTAX_ERROR)->second.length(), 
			0);
		m_End = true;
		return EState::LOGOUT;
	}));
	m_StateFuncs.insert(make_pair(EState::LOGIC_ERROR,            [this](const string msg){
		printf("LOGIC_ERROR\n");
		send(m_ClientSocket, 
			m_ServerClientMsgs.find(EState::LOGIC_ERROR)->second.c_str(), 
			m_ServerClientMsgs.find(EState::LOGIC_ERROR)->second.length(), 
			0);
		m_End = true;
		return EState::LOGOUT;
	}));
	m_StateFuncs.insert(make_pair(EState::KEY_OUT_OF_RANGE_ERROR, [this](const string msg){
		printf("KEY_OUT_OF_RANGE_ERROR\n");
		send(m_ClientSocket, 
			m_ServerClientMsgs.find(EState::KEY_OUT_OF_RANGE_ERROR)->second.c_str(), 
			m_ServerClientMsgs.find(EState::KEY_OUT_OF_RANGE_ERROR)->second.length(), 
			0);
		m_End = true;
		return EState::LOGOUT;
	}));
}

void CRobotHandler::setupServerClientMsgs()
{
	// Insert msgs into error map
	m_ServerClientMsgs.insert(make_pair(EState::PICK_UP,                "105 GET MESSAGE\a\b"));
	m_ServerClientMsgs.insert(make_pair(EState::LOGOUT,                 "106 LOGOUT\a\b"));
	m_ServerClientMsgs.insert(make_pair(EState::HANDSHAKE_KEY_REQUEST,  "107 KEY REQUEST\a\b"));
	m_ServerClientMsgs.insert(make_pair(EState::LOGIN_OK,               "200 OK\a\b"));
	m_ServerClientMsgs.insert(make_pair(EState::LOGIN_FAILED,           "300 LOGIN FAILED\a\b"));
	m_ServerClientMsgs.insert(make_pair(EState::SYNTAX_ERROR,           "301 SYNTAX ERROR\a\b"));
	m_ServerClientMsgs.insert(make_pair(EState::LOGIC_ERROR,            "302 LOGIC ERROR\a\b"));
	m_ServerClientMsgs.insert(make_pair(EState::KEY_OUT_OF_RANGE_ERROR, "303 KEY OUT OF RANGE\a\b"));
}

void CRobotHandler::setupMoveMsgs()
{
	m_MoveMsgs.insert(make_pair(EMove::MOVE,       "102 MOVE\a\b"));
	m_MoveMsgs.insert(make_pair(EMove::TURN_LEFT,  "103 TURN LEFT\a\b"));
	m_MoveMsgs.insert(make_pair(EMove::TURN_RIGHT, "104 TURN RIGHT\a\b"));
}

void CRobotHandler::setupKeyIds()
{
	m_KeyIds.push_back(make_pair(23019, 32037));
	m_KeyIds.push_back(make_pair(32037, 29295));
	m_KeyIds.push_back(make_pair(18789, 13603));
	m_KeyIds.push_back(make_pair(16443, 29533));
	m_KeyIds.push_back(make_pair(18189, 21952));
}

uint CRobotHandler::userHash(size_t key, bool server)
{
	uint usrH = 0;
	for(auto c : m_RoboUsername)
		usrH += c;
	usrH = (usrH * 1000) % 65536;
	usrH += server ? m_KeyIds[key].first : m_KeyIds[key].second;
	return usrH % 65536;
}

void CRobotHandler::nextMove()
{
	printf("Computing next move...\n");
	double dist[4]; // N, S, E, W
	dist[0] = TPos(m_CurrPos.t_PosX, m_CurrPos.t_PosY + 1).distance(TPos(0, 0));
	dist[1] = TPos(m_CurrPos.t_PosX, m_CurrPos.t_PosY - 1).distance(TPos(0, 0));
	dist[2] = TPos(m_CurrPos.t_PosX + 1, m_CurrPos.t_PosY).distance(TPos(0, 0));
	dist[3] = TPos(m_CurrPos.t_PosX - 1, m_CurrPos.t_PosY).distance(TPos(0, 0));

	double minDist = dist[0];
	for(size_t i = 1; i < 4; i++)
		if(minDist > dist[i])
			minDist = dist[i];

	// NORTH
	if(minDist == dist[0])
	{
		if(m_CurrDir == EDirection::NORTH)
		{
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::EAST)
		{
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::WEST)
		{
			m_MoveQueue.push(EMove::TURN_RIGHT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::SOUTH)
		{
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
	}
	// SOUTH
	else if(minDist == dist[1])
	{
		if(m_CurrDir == EDirection::NORTH)
		{
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::EAST)
		{
			m_MoveQueue.push(EMove::TURN_RIGHT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::WEST)
		{
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::SOUTH)
		{
			m_MoveQueue.push(EMove::MOVE);
		}
	}
	// EAST
	else if(minDist == dist[2])
	{
		if(m_CurrDir == EDirection::NORTH)
		{
			m_MoveQueue.push(EMove::TURN_RIGHT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::EAST)
		{
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::WEST)
		{
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::SOUTH)
		{
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
	}
	// WEST
	else
	{
		if(m_CurrDir == EDirection::NORTH)
		{
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::EAST)
		{
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::TURN_LEFT);
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::WEST)
		{
			m_MoveQueue.push(EMove::MOVE);
		}
		else if(m_CurrDir == EDirection::SOUTH)
		{
			m_MoveQueue.push(EMove::TURN_RIGHT);
			m_MoveQueue.push(EMove::MOVE);
		}
	}
}

void CRobotHandler::obstacleMove()
{
	printf("Obstacle encountered...\n");
	m_MoveQueue.push(EMove::TURN_LEFT);
	m_MoveQueue.push(EMove::MOVE);
	m_MoveQueue.push(EMove::TURN_RIGHT);
	m_MoveQueue.push(EMove::MOVE);
	m_MoveQueue.push(EMove::MOVE);
	m_MoveQueue.push(EMove::TURN_RIGHT);
	m_MoveQueue.push(EMove::MOVE);
	m_MoveQueue.push(EMove::TURN_LEFT);
}

void CRobotHandler::sendMove(const EMove move)
{
	// determine the direction the robot is facing
	if(m_CurrDir != EDirection::UNKNOWN)
	{
		if(move == EMove::TURN_LEFT)
		{
			if(m_CurrDir == EDirection::NORTH)
				m_CurrDir = EDirection::WEST;
			else if(m_CurrDir == EDirection::SOUTH)
				m_CurrDir = EDirection::EAST;
			else if(m_CurrDir == EDirection::WEST)
				m_CurrDir = EDirection::SOUTH;
			else if(m_CurrDir == EDirection::EAST)
				m_CurrDir = EDirection::NORTH;
		}
		else if(move == EMove::TURN_RIGHT)
		{
			if(m_CurrDir == EDirection::NORTH)
				m_CurrDir = EDirection::EAST;
			else if(m_CurrDir == EDirection::SOUTH)
				m_CurrDir = EDirection::WEST;
			else if(m_CurrDir == EDirection::WEST)
				m_CurrDir = EDirection::NORTH;
			else if(m_CurrDir == EDirection::EAST)
				m_CurrDir = EDirection::SOUTH;
		}
	}
	printf("Sending MOVE: %c\n", static_cast<char>(move));
	send(m_ClientSocket, 
		 m_MoveMsgs.find(move)->second.c_str(), 
		 m_MoveMsgs.find(move)->second.length(), 
		 0);
	m_LastMove = move;
	m_MoveCnt += 1;
}

int CRobotHandler::checkOK(const string & msg, TPos & rcvPos)
{
	string inputMsg = msg;
	string delim = " ";
	size_t pos;
	string msg_tmp;
	vector<string> parts;
	while((pos = inputMsg.find(delim)) != string::npos)
	{
		msg_tmp = inputMsg.substr(0, pos);
		// Add messages to message queue
		parts.push_back(msg_tmp);
		inputMsg.erase(0, pos + delim.length());
	}

	if(parts.size() != 2)
		return 0;

	parts.push_back(inputMsg);

	if(parts[0] != "OK"
	|| !isNum(parts[1])
	|| !isNum(parts[2]))
		return 0;

	rcvPos.t_PosX = stoi(parts[1]);
	rcvPos.t_PosY = stoi(parts[2]);
	return 1;
}

bool isNum(const string & snum)
{
	for (size_t i = 0; i < snum.length(); i++)
	{
		if((i == 0 && !isdigit(snum[i]) && snum[i] != '-')
		|| (i != 0 && !isdigit(snum[i])))
			return false;
	}
	return true;
}

bool CRobotHandler::checkLeftovers()
{
	switch(m_CurrState)
	{
		case EState::HANDSHAKE_USERNAME:
			if(m_InputData.length() > 18)
				return false;
			break;
		case EState::MESSAGE:
			if(m_InputData.length() > 98)
				return false;
			break;
		case EState::HANDSHAKE_KEY:
			if((!isNum(m_InputData) || m_InputData.length() > 10) && m_InputData[m_InputData.length()-1] != '\a')
				return false;
			break;
		default:
			if(m_InputData.length() > 10)
				return false;
			break;
	}
	return true;
}

void printMoveQueue(const queue<EMove> & moveQueue)
{
	queue<EMove> tmpQue(moveQueue);
	printf("  Move que: ");
	while(!tmpQue.empty())
	{
		printf("%c ", static_cast<char>(tmpQue.front()));
		tmpQue.pop();
	}
	printf("\n");
}

string convertForScreen(string data) {
   string screenData("");
   for (unsigned i = 0; i < data.size(); ++i) {
      if (data[i] >= ' ' and data[i] <= '~' and data[i] != '"' and data[i] != '\\') {
         screenData += data[i];
         continue;
      }
      switch (data[i]) {
      case '\0':
         screenData += "\\0";
         break;
      case '\a':
         screenData += "\\a";
         break;
      case '\b':
         screenData += "\\b";
         break;
      case '\t':
         screenData += "\\t";
         break;
      case '\n':
         screenData += "\\n";
         break;
      case '\v':
         screenData += "\\v";
         break;
      case '\f':
         screenData += "\\f";
         break;
      case '\r':
         screenData += "\\r";
         break;
      case '\e':
         screenData += "\\e";
         break;
         case '"':
            screenData += "\\\"";
            break;
      default:
         stringstream ss;
         ss << "\\x" << std::setfill ('0') << std::setw(2) << std::hex << (unsigned short) data[i];
         screenData += ss.str();
         break;
      }
   }
   return screenData;
}
