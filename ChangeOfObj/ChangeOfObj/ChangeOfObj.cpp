#include "SIGService.h"
#include <tchar.h>
#include <string>
#include "global.h"
#include <conio.h>

#define Text "../Release/object.txt"
bool init = true;
bool start = false;
bool first = true;

string Day;
string Time;
string TextName;
stringstream year, month, day, hour, minute, second;

class MyService : public sigverse::SIGService
{

public:
	MyService(std::string name) : SIGService(name){};
	~MyService();
	// メッセージ受信時に呼び出される関数
	void onRecvMsg(sigverse::RecvMsgEvent &evt);
	// 定期的に呼び出される関数
	double onAction();
	void MyService::LoadText(void);
	void MyService::SendToController(std::string name, std::string pos);
	void MyService::NextObj(void);
	std::string MyService::getLocalTime(void);
	void strSplit(string msg, string separator);
	void string2double(const string &str);
private:
	typedef std::pair<std::string, std::string> obj;
	std::vector<obj> obj_pos;

	stringstream ss;
	string headStr;
	string bodyStr;
	int obj_index;
	int question_index;
	int obj_counter;
	int question_counter;
};


MyService::~MyService()
{
	// 切断します
	this->disconnect();
}

double MyService::onAction()
{
	if (init){
		LoadText();
		//Time = getLocalTime();
		obj_index = obj_pos.size();
		question_counter = 0;
		init = false;
	}
	else{
		if (start){
			string message;
			if (first){
				for (int i = 0; i < obj_pos.size(); ++i){
					SendToController(obj_pos[i].first, obj_pos[i].second);
				}
				this->sendMsgToCtr("man_0", "end");
				first = false;
				cout << "あたらしい物体配置が完了した場合：はじめ" << endl;
			}
			cout << "入力待ち：";
			cin >> message;
			if (message == "はじめ"){
				this->sendMsgToCtr("man_0", "go");
				this->sendMsg("sigverse_DB", "Start");
				start = false;
			}
		}
		if (obj_counter == obj_index - 1){
			cout << "終了" << endl;
			obj_counter = 0;
		}
	}
	return 0.1;
}

void MyService::onRecvMsg(sigverse::RecvMsgEvent &evt)
{
	string sender = evt.getSender();
	string msg = evt.getMsg();
	strSplit(msg, ":");
	if (sender == "man_0"){
		if (msg == "Start"){
			start = true;
		}
		else if (headStr == "Question") {
			this->string2double(bodyStr);
			cout << "Question index is " << question_index << endl;
		}
		else if (headStr == "NEXT") {
			this->sendMsgToCtr("man_0", "go");
		}
	}
	else if (sender == "sigverse_DB"){
		if (msg == "あり") {
			cout << "同じ質問をします" << endl;
			this->sendMsgToCtr("man_0", "roop");
		}
		else if (msg == "なし") {
			cout << "次の質問をします" << endl;
			this->sendMsgToCtr("man_0", "inc");
			question_counter++;
			cout << question_counter << endl;
			if (question_counter > question_index - 1) {
				cout << "これ以上質問がないので次の物体に切り替えます" << endl;
				this->sendMsgToCtr("man_0", "fin");
				obj_counter++;
				question_counter = 0;
			}
		}
	}
}

void MyService::LoadText(void){
	ifstream fin;
	string s;
	fin.open(Text, ios::in);
	if (!fin){
		cout << "Error: cannot open file." << endl;
		exit(1);
	}

	while (getline(fin, s)){
		std::string::size_type blank = s.find(' ');
		string obj_name = s.substr(0, blank);
		string pos = s.substr(blank + 1);
		obj_pos.push_back(obj(s.substr(0, blank), s.substr(blank + 1)));
	}
	fin.close();
}

void MyService::SendToController(std::string name, std::string pos){

	if (name.c_str() != NULL || pos.c_str() != NULL){
		this->sendMsgToCtr("man_0", name);
		this->sendMsgToCtr("man_0", pos);
	}

}

//std::string MyService::getLocalTime(void){
//	time_t timer;
//	struct tm *local;
//
//	timer = time(NULL);
//	local = localtime(&timer);
//	double y, mon, d, h, min, s;
//
//	y = local->tm_year + 1900;
//	mon = local->tm_mon + 1;
//	d = local->tm_mday;
//	h = local->tm_hour;
//	min = local->tm_min;
//	s = local->tm_sec;
//
//	ss << year << std::setw(2) << std::setfill('0') << month << std::setw(2) << std::setfill('0') << day << std::setw(2) << std::setfill('0') << hour << std::setw(2) << std::setfill('0') << minute << std::setw(2) << std::setfill('0') << second;
//
//	string name = ss.str();
//
//	ss.str("");
//	ss.clear(stringstream::goodbit);
//
//	return name;
//}

void MyService::strSplit(string msg, string separator) {
	int strPos1 = 0;
	int strPos2;
	string head;
	string body;
	strPos2 = msg.find_first_of(separator, strPos1);
	head.assign(msg, strPos1, strPos2 - strPos1);
	body.assign(msg, strPos2 + 1, msg.length() - strPos2);
	headStr = head;
	bodyStr = body;
}

void MyService::string2double(const std::string &str) {		ss << str;	ss >> question_index;}

int main(int argc, char** argv)
{
	// クラスMyServiceのインスタンスを作成します
	MyService srv("ChangeObj");
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
