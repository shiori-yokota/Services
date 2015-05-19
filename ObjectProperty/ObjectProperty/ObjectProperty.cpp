#include "mysql_connection.h"
// Database include files
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
//Sigverse include files
#include "SIGService.h"
#include <tchar.h>
#include <string>

bool start = false;

//namespace
using namespace std;
using namespace sql;
//////////////////////////////////////////
struct vector3{
	double x;
	double y;
	double z;
};

class MyService : public sigverse::SIGService
{
public:
	MyService(std::string name) : SIGService(name){};
	~MyService();
	void onRecvMsg(sigverse::RecvMsgEvent &evt);
	void mysqlRead(std::string object);
	void mysqlWrite(string Property);
	double onAction();
	void InitToMysql();
	void strSplit(string msg, string separator);
private:
	string obj_id;
	string attribute;
	string headStr;
	string bodyStr;

};

MyService::~MyService()
{
	this->disconnect();
}

double MyService::onAction()
{
	if (start){
		string Property;
		cout << "属性値：";
		cin >> Property;
		if (Property != "no") {
			this->mysqlWrite(Property);
			this->sendMsg("ChangeObj","あり");
		}
		else {
			this->sendMsg("ChangeObj", "なし");
			start = false;
		}
	}
	return 1.0;
}

void MyService::onRecvMsg(sigverse::RecvMsgEvent &evt)
{
	string sender = evt.getSender();
	string msg = evt.getMsg();
	strSplit(msg, ":");
	if (sender == "man_0"){
		if (msg == "Start") {
			cout << msg << endl;
			//初期設定
			this->InitToMysql();
		}
		else if (headStr == "id") {
			obj_id = bodyStr;
			cout << obj_id << endl;
		}
		else {
			attribute = msg;
			if (attribute == "how to use") attribute = "how_to_use";
			cout << attribute << endl;
			start = true;
		}
	}
	else if (sender == "ChangeObj"){
		if (msg == "Start") {
			cout << msg << endl;
			//初期設定
			this->InitToMysql();
		}
	}
}

// Write to the database
void MyService::mysqlWrite(string Property){
	try {
		Driver *driver;
		Connection *con;
		Statement *stmt;
		string command;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://150.37.224.108:3306", "sig", "sigverse");
		/* Connect to the MySQL test database */
		con->setSchema("ObjectProperties");
		stmt = con->createStatement();

		/* Insert new property into attribute*/
		command = "INSERT IGNORE INTO " + attribute + " VALUES('" + Property + "');";
		cout << command << endl;
		stmt->execute(command);
		//stmt->execute("INSERT IGNORE INTO " + attribute + " VALUES('" + Property + "');");

		/* Create new table */
		string query;
		PreparedStatement *pstmt;
		ResultSet *rows;
		query = "SHOW TABLES FROM ObjectProperties LIKE '" + Property + "';";
		cout << query << endl;
		pstmt = con->prepareStatement(query);
		rows = pstmt->executeQuery();
		rows->next();
		int count = rows->rowsCount();
		cout << count << endl;
		if (count == 0){
			cout << "not exist" << endl;
			cout << "create table " << Property << endl;
			stmt->execute("CREATE TABLE " + Property + "  (id char (50) unique, count int) ");
		}
		else cout << "already exist" << endl;

		/* Insert object_id into property */
		command = "INSERT INTO " + Property + " VALUES('" + obj_id + "',1) ON DUPLICATE KEY UPDATE count = count + 1;";
		cout << command << endl;
		stmt->execute(command);

		start = false;
		delete stmt;
		delete pstmt;
		delete con;
		delete rows;
		// Error handler
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	cout << endl;
}

// Read from the database
void MyService::mysqlRead(string name){
	try {
		Driver *driver;
		Connection *con;
		Statement *stmt;
		ResultSet *res;
		PreparedStatement *pstmt;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://150.37.224.108:3306", "sig", "sigverse");
		/* Connect to the MySQL test database */
		con->setSchema("ObjectProperties");
		pstmt = con->prepareStatement("select id from " + obj_id + ";");
		res = pstmt->executeQuery();
		// count rows
		//ResultSet *rows;
		//pstmt = con->prepareStatement("select count(*) from " + objConcept + ";");
		//rows = pstmt->executeQuery();
		//rows->next();
		//int count = rows->getInt(1);
		//cout << count << endl;
		cout << "message = " << obj_id << endl;
		cout << "***** targetOBJ *****" << endl;
		while (res->next()) {
			string select = res->getString("id");
			cout << select << endl;
		}
		cout << "*********************" << endl;

		delete res;
		delete pstmt;
		delete con;
		// Error handler
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	cout << endl;
}

void MyService::InitToMysql(){
	try {
		Driver *driver;
		Connection *con;
		Statement *stmt;
		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://150.37.224.108:3306", "sig", "sigverse");
		/* Connect to the MySQL test database */
		con->setSchema("ObjectProperties");
		stmt = con->createStatement();
		stmt->execute("DROP TABLE IF EXISTS attribute");
		stmt->execute("DROP TABLE IF EXISTS name");
		stmt->execute("DROP TABLE IF EXISTS color");
		stmt->execute("DROP TABLE IF EXISTS shape");
		stmt->execute("DROP TABLE IF EXISTS state");
		stmt->execute("DROP TABLE IF EXISTS how_to_use");
		stmt->execute("DROP TABLE IF EXISTS gabunomi");

		// create concept
		stmt->execute("CREATE TABLE attribute  (attribute char (50)) ");
		stmt->execute("INSERT INTO attribute VALUES ('name'),('color'),('shape'),('state'),('how to use')");
		// create name
		stmt->execute("CREATE TABLE name  (property char (50) unique) ");
		// create color
		stmt->execute("CREATE TABLE color  (property char (50) unique) ");
		// create shape
		stmt->execute("CREATE TABLE shape  (property char (50) unique) ");
		// create state
		stmt->execute("CREATE TABLE state  (property char (50) unique) ");
		// create how_to_use
		stmt->execute("CREATE TABLE how_to_use  (property char (50) unique) ");

		delete stmt;
		delete con;
		// Error handler
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	cout << endl;
}

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

int main(int argc, char** argv)
{
	MyService srv("sigverse_DB");
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