//#include <sapi.h>
//#include <sphelper.h>
#include <string>
#include <iostream>
#include <fstream>
#include "SIGService.h"

//includes for Shared memory
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include <mmsystem.h>
#include "MaltiSpeak.h"



#pragma comment(lib, "user32.lib")
#pragma comment(lib,"winmm.lib")

bool Enable;

namespace WordCodeConvert{
	/* convUtf8toSJis
	* �����R�[�h�ϊ��̂��ߊO��������肵���\�[�X
	* ��{�I�Ɏg�p���Ȃ�
	*/
	bool convUtf8toSJis(BYTE* pSource, BYTE* pDist, int* pSize){
		*pSize = 0;

		//UTF-8����UTF-16�֕ϊ�
		const int nSize = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pSource, -1, NULL, 0);

		BYTE* buffUtf16 = new BYTE[nSize * 2 + 2];
		::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pSource, -1, (LPWSTR)buffUtf16, nSize);

		//UTF-16����Shift-JIS�֕ϊ�
		const int nSizeSJis = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL);
		if (!pDist){
			*pSize = nSizeSJis;
			delete buffUtf16;
			return TRUE;
		}

		BYTE* buffSJis = new BYTE[nSizeSJis * 2];
		ZeroMemory(buffSJis, nSizeSJis * 2);
		::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffSJis, nSizeSJis, NULL, NULL);

		*pSize = strlen((const char *)buffSJis);
		memcpy(pDist, buffSJis, *pSize);

		delete buffUtf16;
		delete buffSJis;

		return TRUE;
	}

	/* utf8toSjis
	* ���� std::string �߂�l std::string �ɕύX��������
	* ���ʂȂ��Ƃ��������肱�̊֐���p����
	* UTF8�̕������Shift-JIS�ɕϊ�����
	*/
	std::string utf8toSjis(std::string utf8_string){
		int nSize = 0;
		const char* constchar = utf8_string.c_str();
		BYTE* pSource = (BYTE*)constchar;
		convUtf8toSJis(pSource, NULL, &nSize);
		BYTE* pDist = new BYTE[nSize + 1];
		ZeroMemory(pDist, nSize + 1);
		convUtf8toSJis(pSource, pDist, &nSize);
		std::string returnString((char*)pDist);
		return returnString;
	}
}

class VoiceRecognition : public sigverse::SIGService
{

public:
	VoiceRecognition(std::string name) : SIGService(name){};
	~VoiceRecognition();
	double onAction();
	void onRecvMsg(sigverse::RecvMsgEvent &evt);
	void  onInit();
	std::string japaneseMessage2englishMessage(std::string japaneseMessage);
	std::string englishMessage2japaneseMessage(std::string englishMessage);
	std::string preview_string;
	MaltiSpeak ms;
	std::string m_pointedObject;
	std::string m_pointedtrash;
	std::string robotName;

	std::string storageSpaceName0;	//�e�[�u��
	std::string storageSpaceName1;	//�L�b�`��
	std::string storageSpaceName2;	//���S��
	std::string storageSpaceName3;	//�{�I
	std::string storageSpaceName4;	//�H��I
	std::string storageSpaceName5;	//�I

	//�����ŕЕt���邩�ǂ���
	std::string autoMode;	//�����ŕЕt����
	std::string selectMode;	//�Еt������w��
	bool autoflag; //�����ŕЕt����Ȃ�true
};

VoiceRecognition::~VoiceRecognition()
{

	this->disconnect();
}

/*japaneseMessage2englishMessage
*����std::string  �߂�lstd::string
*SIGVerse���őS�p���g���Ȃ��̂ő��M���b�Z�[�W����{�ꂩ��p��ɒu��������
*/
std::string VoiceRecognition::japaneseMessage2englishMessage(std::string japaneseMessage){
	std::string ja_str = japaneseMessage;
	std::string en_str;

	//�N���[���i�b�v�J�n
	
	if (ja_str == "�n�߂܂�") en_str = "go";

	//�c���Ώۂ̎w��
	else if (std::string::npos != ja_str.find("��") || std::string::npos != ja_str.find("��")){
		en_str = storageSpaceName0;
		autoflag = false;
	}
	//else if (ja_str == "������Ƃ���") en_str = "take";

	//�̂Ă�or�u���Ώۂ̎w��
	else if (std::string::npos != ja_str.find("�L�b�`��")){
		en_str = storageSpaceName1;
		autoflag = false;
	}
	//else if (ja_str == "����Ɏ̂Ă�") en_str = "put";
	//else if (ja_str == "�����Ɏ̂Ă�") en_str = "put";
	
	else if (std::string::npos != ja_str.find("���S��")){
		en_str = storageSpaceName2;
		autoflag = false;
	}
	else if (std::string::npos != ja_str.find("��") || std::string::npos != ja_str.find("�{")){
		en_str = storageSpaceName3;
		autoflag = false;
	}
	//else if (ja_str == "�����ɂ�����") en_str = "put";

	else if (std::string::npos != ja_str.find("��") || std::string::npos != ja_str.find("�H��")){
		en_str = storageSpaceName4;
		autoflag = false;
	}
	else if (std::string::npos != ja_str.find("��") || std::string::npos != ja_str.find("�I")){
		en_str = storageSpaceName5;
		autoflag = false;
	}
	else if (std::string::npos != ja_str.find("�����Â�") || std::string::npos != ja_str.find("�ЂÂ�") || std::string::npos != ja_str.find("�Еt��")){
		en_str = autoMode;
		autoflag = true;
	}
	//�̂Ă�or�u���Ώۂ��Ԉ�����ꍇ��蒼��
	else if (ja_str == "�I���܂�") en_str = "finish";	

	//����̖��߈ȊO�̏ꍇ
	else {
		en_str = "error";
		PlaySound("MySound.wav", NULL, SND_FILENAME | SND_ASYNC);
	}
	return en_str;
}

/*englishMessage2japaneseMessage
*����std::string  �߂�lstd::string
*OpenJtalk�ɓ��{��Ŕ��b���������̂Ŏ�M���b�Z�[�W���p�ꂩ����{��ɒu��������
*/
std::string VoiceRecognition::englishMessage2japaneseMessage(std::string englishMessage){
	std::string en_str = englishMessage;
	std::string ja_str;

	if (en_str == "start")
	{
		ja_str = "����ł͕ЂÂ����n�߂܂�";
	}
	else if (en_str == "please pass my hand")
	{
		ja_str = "�Еt���������̂���n���Ă�������";
	}
	/*else if (en_str == "Message is not accepted")
	{
		ja_str = "�������܂���B";
	}*/
	else if (en_str == "please choose the trashbox")
	{
		ja_str = "�w�����Ă�������";
	}
	else if (en_str == "Clock") 
	{
		m_pointedObject = "���v";
		ja_str = m_pointedObject + "�A��u���܂�";
	}
	else if (en_str == "Bear") 
	{
		m_pointedObject = "�F�̂ʂ������";
		ja_str = m_pointedObject + "�A��u���܂�";
	}
	else if (en_str == "Penguin")
	{
		m_pointedObject = "��������̃y���M��";
		ja_str = "�A" +m_pointedObject + "�A��u���܂�";
	}
	else if (en_str == "Cup")
	{
		m_pointedObject = "�J�b�v";
		ja_str = "�A" + m_pointedObject + "�A��u���܂�";
	}
	else if (en_str == "finish")
	{
		ja_str = "�Y��ɕЕt���܂���";
	}
	else if (en_str == "Please show me which trash box to use")
	{
		ja_str = m_pointedObject + "�A���^�Ԑ�������ĉ�����";
	}
	else if (en_str == storageSpaceName0)
	{
		m_pointedtrash = storageSpaceName0;
		if (autoflag == false){
			ja_str = "������܂����A" + m_pointedObject + "���A" + "�e�[�u��" + "�A�ɒu���ɍs���܂�";
		}
		else{
			ja_str = m_pointedObject + "�́A�e�[�u���Ɉ�ԉ^�΂�Ă��܂��B�e�[�u���Ɏ����Ă����܂�";
		}
	}
	else if (en_str == storageSpaceName1)
	{
		m_pointedtrash = storageSpaceName1;
		if (autoflag == false){
			ja_str = "������܂����A" + m_pointedObject + "���A" + "�L�b�`��" + "�A�ɒu���ɍs���܂�";
		}
		else{
			ja_str = m_pointedObject + "�́A�L�b�`���Ɉ�ԉ^�΂�Ă��܂��B�L�b�`���Ɏ����Ă����܂�";
		}
	}
	else if (en_str == storageSpaceName2)
	{
		m_pointedtrash = storageSpaceName2;
		if (autoflag == false){
			ja_str = "������܂����A" + m_pointedObject + "���A" + "���S��" + "�A�ɒu���ɍs���܂�";
		}
		else{
			ja_str = m_pointedObject + "�́A���S���Ɉ�ԉ^�΂�Ă��܂��B���S���Ɏ����Ă����܂�";
		}
	}
	else if (en_str == storageSpaceName3)
	{
		m_pointedtrash = storageSpaceName3;
		if (autoflag == false){
			ja_str = "������܂����A" + m_pointedObject + "���A" + "�{�I" + "�A�ɒu���ɍs���܂�";
		}
		else{
			ja_str = m_pointedObject + "�́A�{�I�Ɉ�ԉ^�΂�Ă��܂��B�{�I�Ɏ����Ă����܂�";
		}
	}
	else if (en_str == storageSpaceName4)
	{
		m_pointedtrash = storageSpaceName4;
		if (autoflag == false){
			ja_str = "������܂����A" + m_pointedObject + "���A" + "�H��I" + "�A�ɒu���ɍs���܂�";
		}
		else{
			ja_str = m_pointedObject + "�́A�H��I�Ɉ�ԉ^�΂�Ă��܂��B�H��I�Ɏ����Ă����܂�";
		}
	}
	else if (en_str == storageSpaceName5)
	{
		m_pointedtrash = storageSpaceName5;
		if (autoflag == false){
			ja_str = "������܂����A" + m_pointedObject + "���A" + "�I" + "�A�ɒu���ɍs���܂�";
		}
		else{
			ja_str = m_pointedObject + "�́A�I�Ɉ�ԉ^�΂�Ă��܂��B�I�Ɏ����Ă����܂�";
		}
	}
	return ja_str;
}


void VoiceRecognition::onInit(){
	robotName = "sobit";

	//ShellExecute(NULL, "open", "juliusbat\\julius-run.bat", NULL, NULL, SW_SHOW);

	std::ofstream ofs("juliusbat\\sentence.txt");
	ofs.clear(); //�O�񕶎���폜
	ofs.close();

	Enable = false;
	preview_string = "";
	
	storageSpaceName0 = "Table";
	storageSpaceName1 = "Kitchen";
	storageSpaceName2 = "Wagon";
	storageSpaceName3 = "BookShelf";
	storageSpaceName4 = "CupBoard";
	storageSpaceName5 = "Shelf";

	autoMode = "auto_mode";
	selectMode = "select_mode";

	autoflag = false;

	ms.initialize();
}



double VoiceRecognition::onAction()
{
	if (Enable)
	{
		std::ifstream readfile("juliusbat\\sentence.txt"); //julius�F����������擾
		std::string str;
		std::getline(readfile, str); //1�s�ǂݍ���
		readfile.close();

		str = WordCodeConvert::utf8toSjis(str); //�����R�[�h�̕ϊ�

		size_t pos;
		while ((pos = str.find_first_of(" �@�A�B")) != std::string::npos){ //��������̔��p�E�S�p�X�y�[�X�C��Ǔ_���폜
			str.erase(pos, 1);
		}
		if (str != preview_string){
			std::string send_msg = japaneseMessage2englishMessage(str);//���b�Z�[�W�̒u������

			this->sendMsg(robotName, send_msg.c_str());
			preview_string = str;
		}
	}
	return 0.1;
}

void VoiceRecognition::onRecvMsg(sigverse::RecvMsgEvent &evt)
{
	std::string sender = evt.getSender();
	std::string msg = evt.getMsg();
	std::string s = msg;

	printf("Message  : %s  \n", s.c_str());
	std::wstring ws;
	printf("Sender  :  %s  \n", sender.c_str());

	if (strcmp(s.c_str(), "Stop_Reco") == 0)
	{
		Enable = false;
	}
	else if (strcmp(s.c_str(), "Start_Reco") == 0)
	{
		Enable = true;
	}
	else{
		if (strcmp(s.c_str(), "rec") != 0){
			ms.noWaitSpeak(s);//���b
		}
	}
}

int main(int argc, char** argv)
{
	VoiceRecognition srv("VoiceReco_Service");
	//srv.onInit();
	unsigned short port = (unsigned short)(atoi(argv[2]));
	srv.connect(argv[1], port);
	srv.connectToViewer();
	srv.setAutoExitProc(true);
	//srv.connect("136.187.35.129", 9005);
	srv.startLoop();
	return 0;
}