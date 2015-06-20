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

// SIGServiceを継承したクラスを作成します
class MyService : public sigverse::SIGService
{

public:
	MyService(std::string name) : SIGService(name){};
	~MyService();
	// メッセージ受信時に呼び出される関数
	void onRecvMsg(sigverse::RecvMsgEvent &evt);
	// 定期的に呼び出される関数
	double onAction();
	std::string MyService::get_time(void);
	void strSplit(std::string msg, std::string separator);

	//フォルダの名前
	std::string FolderName;
	//一人称視点の画像が保存されるフォルダ
	std::string AvatarName;
	std::string AvatarViewName;
	//第三者視点の画像が保存されるフォルダ
	std::string ThirdParty;
	std::string ThirdPartyView;
	//timeが保存される
	std::string Time;
	//Object id
	std::string objName;
	//strSpllit
	std::string headStr;
	std::string bodyStr;
};

MyService::~MyService()
{
	// 切断します
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
				// 画像データを取得します
				char *buf = img->getBuffer();
				char *buf2 = img2->getBuffer();
				//Windows BMP 形式で保存します
				char fname[256];
				char fname2[256];
				sprintf(fname, AvatarViewName.c_str());
				sprintf(fname2, ThirdPartyView.c_str());
				img->saveAsWindowsBMP(fname);
				img2->saveAsWindowsBMP(fname2);
			}
			//必要なくなったら削除します
			delete img;
			delete img2;
		}
	}
	return 0.5;
}

void MyService::onRecvMsg(sigverse::RecvMsgEvent &evt)
{
	// メッセージ送信元とメッセージを取得します
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
				// 画像データを取得します
				char *buf = img_avater->getBuffer();
				char *buf2 = img_thirdParty->getBuffer();
				//Windows BMP 形式で保存します
				img_avater->saveAsWindowsBMP(AvatarViewName.c_str());
				img_thirdParty->saveAsWindowsBMP(ThirdPartyView.c_str());
			}
			//必要なくなったら削除します
			delete img_avater;
			delete img_thirdParty;
		}
	}
	// メッセージが"capture"だった場合画像データを取得します
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
	// クラスMyServiceのインスタンスを作成します
	MyService srv("AvatarView");
	unsigned short port = (unsigned short)(atoi(argv[2]));
	srv.connect(argv[1], port);

	// ビューワーに接続します
	if (srv.connectToViewer()){

		// ビューワーがサーバからdisconnectしたらloopを自動で抜けます
		srv.setAutoExitLoop(true);
	}
	// メインループをスタートさせます
	srv.startLoop();
	return 0;
}
