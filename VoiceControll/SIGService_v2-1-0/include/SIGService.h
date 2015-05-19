#ifndef __SGV_MESSAGE__
#define __SGV_MESSAGE__

#include "SgvSocket.h"
#include "ViewImage.h"
#include <vector>
#include <map>
#include <string>


namespace sigverse
{
	class RecvMsgEvent
	{
	private:
		std::string   m_from;
		std::string   m_msg;
	public:
		bool  setData(std::string data, int size);

		//! ���M���G�[�W�F���g�����擾����                                                                
		const char *getSender() { return m_from.c_str(); }

		//! i�Ԗڂ̕�����̎擾                                                                           
		const char *getMsg() { return m_msg.c_str(); }
	};

	class SIGService
	{
	public:
		SIGService(std::string name);
		SIGService();
		virtual ~SIGService();

		//! ���b�Z�[�W���M 
		bool sendMsg(std::string to, std::string msg);

		//! �T�[�o������G���e�B�e�B�R���g���[���Ƀ��b�Z�[�W�𒼐ڑ��M
		//! �T�[�r�X�ɐڑ��ς݂̃G���e�B�e�B�ɂ̂ݑ��M��
		bool sendMsgToCtr(std::string to, std::string msg);

		//! ���b�Z�[�W���M(���肪����) 
		bool sendMsg(std::vector<std::string> to, std::string msg);

		//! �T�[�o�ɐڑ�
		bool connect(std::string host, int port);

		//! �T�[�o�Ɛؒf
		bool disconnect();

		//! �R���g���[���Ƃ̐ڑ���ؒf����(�G���e�B�e�B���w��)
		bool disconnectFromController(std::string entityName);

		//! �ڑ����̑S�R���g���[���Ɛؒf����
		void disconnectFromAllController();

		//! �r���[���[�Ƃ̐ڑ���ؒf����
		void disconnectFromViewer();

		//! �f�[�^��M���[�v���J�n����(���b�Z�[�W��M�R���g���[������̐ڑ����N�G�X�g�J�n�j
		//@ time ���[�v����(sec), -1.0�͖���
		void startLoop(double time = -1.0);

		//! �T�[�o�A�R���g���[���A�r���[���[����f�[�^����M���Ă��邩�ǂ����`�F�b�N����(usec�}�C�N���b��)
		bool checkRecvData(int usec);

		//! �r���[���[�ɐڑ�����
		//@ �ڑ�����ɂ̓r���[���[�𗧂��グ�Ă����K�v������
		bool connectToViewer();

		//! �r���[���[�ɐڑ�����
		bool getIsConnectedView(){return m_connectedView;}

		/** 
		* @brief �G���e�B�e�B�����J�����摜�f�[�^���r���[���[����擾
		* @param entityName       �J���������G���e�B�e�B��
		* @param camID            �J������ID�ԍ�
		* @param cbit             �r�b�g�[�x
		* @param size             �C���[�W�T�C�Y
		*/
		ViewImage* captureView(std::string entityName, 
			int camID = 1,
			ColorBitType cbit = COLORBIT_24, 
			ImageDataSize size = IMAGE_320X240);

		/** 
		* @brief �G���e�B�e�B�����J�������王�������ɂ���I�u�W�F�N�g�܂ł̋������擾
		* @param entityName       ���G���e�B�e�B��
		* @param offset           �����f�[�^offset�l
		* @param range            �����f�[�^�擾�\�͈�
		* @param camID            �J������ID�ԍ�
		* @param cbit             �r�b�g�[�x
		*/
		unsigned char distanceSensor(std::string entityName, 
			double offset = 0.0, 
			double range = 255.0, 
			int camID = 1,
			ColorBitType cbit = DEPTHBIT_8
			);

		/** 
		* @brief �G���e�B�e�B�����J�������琅���ʕ����̋����f�[�^�擾
		* @param entityName       �G���e�B�e�B��
		* @param offset           �����f�[�^offset�l
		* @param range            �����f�[�^�擾�\�͈�
		* @param camID            �J������ID�ԍ�
		* @param cbit             �r�b�g�[�x
		* @param size             �f�[�^�T�C�Y
		*/
		ViewImage* distanceSensor1D(std::string entityName, 
			double offset = 0.0, 
			double range = 255.0, 
			int camID = 1,
			ColorBitType cbit = DEPTHBIT_8,
			sigverse::ImageDataSize size = IMAGE_320X1);

		/** 
		* @brief �G���e�B�e�B�����J��������2���������f�[�^�擾
		* @param entityName       �G���e�B�e�B��
		* @param offset           �����f�[�^offset�l
		* @param range            �����f�[�^�擾�\�͈�
		* @param camID            �J������ID�ԍ�
		* @param cbit             �r�b�g�[�x
		* @param size             �f�[�^�T�C�Y
		*/
		ViewImage* distanceSensor2D(std::string entityName, 
			double offset = 0.0, 
			double range = 255.0, 
			int camID = 1,
			ColorBitType cbit = DEPTHBIT_8,
			sigverse::ImageDataSize size = IMAGE_320X240);


		/** 
		* @brief �␳�O�̐[�x�摜(zbuffer)�擾
		* @param entityName       �G���e�B�e�B��
		* @param offset           �����f�[�^offset�l
		* @param range            �����f�[�^�擾�\�͈�
		* @param camID            �J������ID�ԍ�
		* @param cbit             �r�b�g�[�x
		* @param size             �f�[�^�T�C�Y
		*/
		ViewImage* getDepthImage(std::string entityName, 
			double offset = 0.0, 
			double range = 255.0, 
			int camID = 1,
			ColorBitType cbit = DEPTHBIT_8,
			sigverse::ImageDataSize size = IMAGE_320X240);


		//! �����̃T�[�r�X�����擾����
		std::string getName() { return m_name; }

		//! �����̃T�[�r�X����ݒ肷��
		void setName(std::string name) { m_name = name; }

		//! �T�[�o�ɐڑ����̃T�[�r�X�̐����擾
		int getNewServiceNum() { return (int)m_elseServices.size() ; }

		//! �T�[�o�ɐڑ����̑��̃T�[�r�X�������ׂĎ擾����
		std::vector<std::string> getAllOtherServices() { return m_elseServices; }

		//! �ڑ�����Ă���G���e�B�e�B�̖��O���擾
		std::vector<std::string> getAllConnectedEntitiesName() {
			return m_entitiesName;
		}

		//! �T�[�o�ɐڑ�����Ă��鑼�̃T�[�r�X�����擾����(��x�擾����ƊJ��)
		/*
		std::string getElseServiceName() {
			int size = (int)m_elseServices.size(); 
			std::string name = m_elseServices[size - 1];
			m_elseServices.pop_back();
			return name;
		}
		*/

		//! �T�[�o�ɐڑ�����Ă��鑼�̃T�[�r�X��ǉ�����
		//void setElseServiceName(std::string name) {
		//m_elseServices.push_back(name);
		//}

		SgvSocket* getSgvSocket(){
			return m_sgvsock;
		}

		//! �R���g���[����SgvSocket���擾����
		SgvSocket* getControllerSocket(std::string name){
			std::map<std::string, sigverse::SgvSocket*>::iterator it;
			it = m_consocks.find(name);
			// SgvSocket����������
			if(it != m_consocks.end())
				return (*it).second;
			// SgvSocket��������Ȃ�����
			else
				return NULL;
		}

		//! �ڑ����̃R���g���[���̐����擾
		int getConnectedControllerNum() {
			return (int)m_consocks.size();
		}

		//! �r���[���[��disconnect�����Ƃ��Ɏ����I�Ƀ��[�v���甲����
		void setAutoExitLoop(bool flag){m_autoExitLoop = flag;}

		//! �r���[���[��disconnect�����Ƃ��Ɏ����I�Ƀv���Z�X���I������
		void setAutoExitProc(bool flag){m_autoExitProc = flag;}

		//! �ŏ��ɌĂяo�����
		virtual void onInit(){}

		//! ���b�Z�[�W��M�C�x���g�n���h��
		virtual void onRecvMsg(RecvMsgEvent &evt) {}

		//! ����I�ɌĂяo�����
		virtual double onAction() { return 0.0;}

	private:
		ViewImage* getDistanceImage(std::string entityName, 
			double offset, 
			double range, 
			int camID,
			int dimension,
			ColorBitType cbit,
			sigverse::ImageDataSize size);


	protected:
		//! socket(for server)
		sigverse::SgvSocket *m_sgvsock;

		//! socket(for viewer)
		sigverse::SgvSocket *m_viewsock;

		//! �G���e�B�e�B�̖��O�ƃR���g���[���p�\�P�b�g�̑Ή�
		std::map<std::string, SgvSocket*> m_consocks;

		//! �ڑ����̑S�G���e�B�e�B��
		std::vector<std::string> m_entitiesName;

		//! name
		std::string m_name;

		//! ���̃T�[�r�X�̖��O
		std::vector<std::string> m_elseServices;

		//! �ڑ���T�[�o�z�X�g��
		std::string m_host;

		//!�T�[�r�X���X�^�[�g���Ă��邩�ǂ���
		bool m_start;

		//!�R���g���[�����ڑ�����
		bool m_connected;

		//! �r���[���[�Ɛڑ�����
		bool m_connectedView;

		//! �����I�Ƀ��[�v�𔲂��邩�ǂ���
		bool m_autoExitLoop;

		//! �����I�ɃT�[�r�X�v���o�C�_���I�����邩�ǂ���
		bool m_autoExitProc;

		//! ���[�v�����ǂ���
		bool m_onLoop;
	};
}

#endif
