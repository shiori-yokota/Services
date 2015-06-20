#include "SIGService.h"
#include <tchar.h>
#include <string>
#include <direct.h>
#include <sstream>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <stdlib.h>

#pragma comment(lib, "SIGService.lib")

bool loop = false;

// SIGService���p�������N���X���쐬���܂�
class MyService : public sigverse::SIGService
{

public:
	MyService(std::string name) : SIGService(name){};
	~MyService();
	// ���b�Z�[�W��M���ɌĂяo�����֐�
	void onRecvMsg(sigverse::RecvMsgEvent &evt);
	// ����I�ɌĂяo�����֐�
	double onAction();
	std::string MyService::get_time(void);
	void strSplit(std::string msg, std::string separator);

	//�t�H���_�̖��O
	std::string FolderName;
	//��l�̎��_�̉摜���ۑ������t�H���_
	std::string AvatarName;
	std::string AvatarViewName;
	//��O�Ҏ��_�̉摜���ۑ������t�H���_
	std::string ThirdParty;
	std::string ThirdPartyView;
	//time���ۑ������
	std::string Time;
	//Object id
	std::string objName;
	//strSpllit
	std::string headStr;
	std::string bodyStr;
};

MyService::~MyService()
{
	// �ؒf���܂�
	this->disconnect();
}

double MyService::onAction()
{
	if (loop == true){
		this->sendMsgToCtr("logger1", "getTime");
		sigverse::ViewImage *img;
		sigverse::ViewImage *img2;
		img = this->captureView("man_0",1);
		img2 = this->captureView("logger1", 1);
		double loggerTime_d = atof(Time.c_str());
		int loggerTime_i = loggerTime_d * 1000;
		std::ostringstream oss;
		oss << std::setw(7) << std::setfill('0') << loggerTime_i;
		if (img != NULL && img != NULL){
			AvatarViewName = AvatarName + "/" + oss.str() + ".bmp";
			ThirdPartyView = ThirdParty + "/" + oss.str() + ".bmp";
			if (Time != ""){
				// �摜�f�[�^���擾���܂�
				char *buf = img->getBuffer();
				char *buf2 = img2->getBuffer();
				//Windows BMP �`���ŕۑ����܂�
				char fname[256];
				char fname2[256];
				sprintf(fname, AvatarViewName.c_str());
				sprintf(fname2, ThirdPartyView.c_str());
				img->saveAsWindowsBMP(fname);
				img2->saveAsWindowsBMP(fname2);
			}
			//�K�v�Ȃ��Ȃ�����폜���܂�
			delete img;
			delete img2;
		}
	}
	return 0.5;
}

void MyService::onRecvMsg(sigverse::RecvMsgEvent &evt)
{
	// ���b�Z�[�W���M���ƃ��b�Z�[�W���擾���܂�
	std::string sender = evt.getSender();
	std::string msg = evt.getMsg();

	if (sender == "logger1"){
		strSplit(msg, " ");
		if (msg != "rec" && msg != "play" && msg != "stop"){
			Time = msg;
		}
		if (headStr == "ObjName"){
			objName = bodyStr;
			//std::cout << objName << std::endl;
		}
	}
	else if (sender == "sigverse_DB"){
		//std::cout << msg << std::endl;
		sigverse::ViewImage *img_avater;
		sigverse::ViewImage *img_thirdParty;
		img_avater = this->captureView("man_0", 1);
		img_thirdParty = this->captureView("logger1", 1);
		if (img_avater != NULL && img_thirdParty != NULL){
			AvatarViewName = AvatarName + "/" + objName + "_" + msg + ".bmp";
			ThirdPartyView = ThirdParty + "/" + objName + "_" + msg + ".bmp";
			if (Time != ""){
				// �摜�f�[�^���擾���܂�
				char *buf = img_avater->getBuffer();
				char *buf2 = img_thirdParty->getBuffer();
				//Windows BMP �`���ŕۑ����܂�
				img_avater->saveAsWindowsBMP(AvatarViewName.c_str());
				img_thirdParty->saveAsWindowsBMP(ThirdPartyView.c_str());
			}
			//�K�v�Ȃ��Ȃ�����폜���܂�
			delete img_avater;
			delete img_thirdParty;
		}
	}
	// ���b�Z�[�W��"capture"�������ꍇ�摜�f�[�^���擾���܂�
	if (msg == "capture") {
		loop = true;
		FolderName = get_time();
		AvatarName = FolderName + "/" + "Avatar";
		ThirdParty = FolderName + "/" + "ThirdParty";
		if (_mkdir(FolderName.c_str()) != 0){
			loop = false;
		}
		else{
			if (_mkdir(AvatarName.c_str()) != 0){
				loop = false;
			}
			if (_mkdir(ThirdParty.c_str()) != 0){
				loop = false;
			}
		}
	}
	if (msg == "stop"){
		loop = false;
	}
}


std::string MyService::get_time(void){
	SYSTEMTIME st;

	GetLocalTime(&st);
	struct tm *date;
	time_t now;
	int y, mon, d, h, min, s;
	std::string Day;
	std::string Time;
	std::stringstream year, month, day, hour, minute, second, millisecond;

	//time(&now);
	//date = localtime(&now);

	//y = date->tm_year + 1900;
	//mon = date->tm_mon + 1;
	//d = date->tm_mday;
	//h = date->tm_hour;
	//min = date->tm_min;
	//s = date->tm_sec;

	year << std::setw(4) << std::setfill('0') << st.wYear;
	month << std::setw(2) << std::setfill('0') << st.wMonth;
	day << std::setw(2) << std::setfill('0') << st.wDay;
	hour << std::setw(2) << std::setfill('0') << st.wHour;
	minute << std::setw(2) << std::setfill('0') << st.wMinute;
	second << std::setw(2) << std::setfill('0') << st.wSecond;
	millisecond << std::setw(3) << std::setfill('0') << st.wMilliseconds;

	Day = year.str() + month.str() + day.str();
	Time = hour.str() + minute.str() + second.str() +"_"+ millisecond.str();

	return Day + "_" + Time;
}

void MyService::strSplit(std::string msg, std::string separator){
	int strPos1 = 0;
	int strPos2;
	std::string head;
	std::string body;
	strPos2 = msg.find_first_of(separator, strPos1);
	head.assign(msg, strPos1, strPos2 - strPos1);
	body.assign(msg, strPos2 + 1, msg.length() - strPos2);
	headStr = head;
	bodyStr = body;
}

int main(int argc, char** argv)
{
	// �N���XMyService�̃C���X�^���X���쐬���܂�
	MyService srv("AvatarView");
	unsigned short port = (unsigned short)(atoi(argv[2]));
	srv.connect(argv[1], port);

	// �r���[���[�ɐڑ����܂�
	if (srv.connectToViewer()){

		// �r���[���[���T�[�o����disconnect������loop�������Ŕ����܂�
		srv.setAutoExitLoop(true);
	}
	// ���C�����[�v���X�^�[�g�����܂�
	srv.startLoop();
	return 0;
}
