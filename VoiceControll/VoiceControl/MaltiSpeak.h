#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<time.h>
#include<windows.h>
#include<vector>
//#include<thread>
//#include<mutex>

class MaltiSpeak{
private:
	void Speak(std::string message, int time);
	void waitFinish(int waitTime);
	bool talkFinishFlag;
	FILE *fp;
	//std::mutex m;
	//std::thread timeThread;

public:
	~MaltiSpeak();

	/**
	 * initialize
	 *
	 * �������֐��@�ŏ��ɌĂ�
	 */
	void initialize();

	/**
	 * noWaitSpeak
	 *
	 * ���b�֐��@�֐����Ăяo���Ɣ��b���J�n���邪�C�֐��͂����ɔ��������D���b�̏I���͌��m�o���Ȃ��D
	 * ����
	 *�@message ���b���镶����
	 */
	void noWaitSpeak(std::string message);

	/**
	 * waitSpeak
	 *
	 * ���b�֐��@���b���w�肵���b���֐����ő҂D�@�֐�����Sleep���Ă�ł���ׁC���b�������ł��Ȃ��D
	 *
	 * ����
	 *�@message ���b���镶����
	 *�@waitTime ���b�̊ԑ҂��ԁ@������ɂ���ĈقȂ�ׁC��������K�v����
	 */
	void waitSpeak(std::string message, int waitTime);

	/**
	 * joinSpeak
	 *
	 * ���b�֐��@�֐����Ăяo���Ɣ��b����D�X���b�h��Ɨ����Ď��s�D�֐��͂��������o�����CwaitJoin�֐��Ŕ��b�̏I���҂����ォ��ĂԂ��Ƃ��o����D
	 * �@�@�@�@�@�܂�waitJoin�֐����ĂԑO�ɍēxjoinSpeak�����s���ꂽ�ꍇ�O�Ɏ��s���ꂽ���b������������ɔ��b����D
	 *
	 * ����
	 *�@message ���b���镶����
	 *�@talkTime ���b���鎞�ԁ@������ɂ���ĈقȂ�ׁC��������K�v����
	 */
	void joinSpeak(std::string message, int talkTime);

	/**
	 * waitJoin
	 *
	 * joinSpeak�֐��Ŏ��s���ꂽ���b�̏I����҂֐��D�@���b��ʃX���b�h�Ŏ��s�㍇�����Ɏg�p
	 * joinSpeak�֐������b���̏ꍇ���b�I���܂ő҂��C���b���I����Ă����ꍇ�͂����ɏI���D
	 * ����Ɠ������������Ƃ��Ȃǂɗp���Ă��������D
	 */
	void waitJoin();

	/**
	 * speakState
	 *
	 * ���݂̔��b��Ԃ�߂�l�Ƃ���֐�
	 * waitSpeak�֐���joinSpeak�֐��̂ݗL���D�@noWaitSpeak���g�p�����ꍇ�K��true�ɂȂ�̂Œ���
	 *
	 * �߂�l
	 *�@bool
	 *�@�@���b���̏ꍇfalse
	 *�@�@���b���������Ă���ꍇtrue
	 */
	bool speakState();

	/**
	 * file_name
	 *
	 * OpenJTalk�̔��b���e�������Ă���e�L�X�g�t�@�C���̃p�X
	 */
	std::string file_name;
};