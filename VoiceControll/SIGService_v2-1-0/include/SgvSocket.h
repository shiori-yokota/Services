#ifndef __SGVSOCKET__
#define __SGVSOCKET__

#include <winsock2.h>
//#include "SgvLog.h"

namespace sigverse
{
	class SgvSocket
	{
	public:

		SgvSocket();
		SgvSocket(SOCKET sock);

		~SgvSocket();

		//! socket ������
		bool initWinsock();

		//! socket �I��
		void finalWinsock(){WSACleanup();}

		//! �ڑ�
		bool connectTo(const char *hostname, int port);

		//! �f�[�^���M
		bool sendData( const char *msg, int size);

		//! �f�[�^��M
		bool recvData( char *msg, int size);

		//! �\�P�b�g�擾
		SOCKET getSocket() { return m_sock; }


	private:
		// �\�P�b�g
		SOCKET m_sock;

		// ���O
		//Sgv::LogPtr mLog;

	};
}

#endif