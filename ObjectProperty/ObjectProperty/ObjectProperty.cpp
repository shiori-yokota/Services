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
#include <sstream>

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
	double onAction();
	void InitToMysql();
	void mysqlRead(void);
	void mysqlSearchInit(void);
	void mysqlSearch(void);
	void mysqlWrite(string Property);
	void strSplit(string msg, string separator);
	string Int2String(int number);
private:
	//typedef std::pair<std::string, int> Property;
	//std::vector<Property> Properties;

	std::map<std::string, int>Properties;
	std::vector<std::string> Obj_list;
	std::vector<std::string> List;
	std::vector<std::string> RequestedProperties;

	std::string obj_id;
	std::string file_dir;
	std::string dir_prop;
	std::string property;
	std::string attribute;
	std::string headStr;
	std::string bodyStr;

	int obj_count;

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
			this->sendMsg("AvatarView",Property);
			std::string msg = "PickUp " + Property;
			this->sendMsg("man_0", msg);
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
		else if (headStr == "DirProp"){
			file_dir = bodyStr;
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
	else if (sender == "Robot"){
		if (msg == "Start") {
			cout << msg << endl;
		}
		else if (headStr == "Request"){
			property = bodyStr;
			RequestedProperties.push_back(property);
			this->mysqlRead();
		}
		else if (headStr == "Search"){
			if (bodyStr == "Init"){
				this->mysqlSearchInit();
			}
			else{
				this->mysqlSearch();
			}
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

		///* Create new table */
		//string query;
		//PreparedStatement *pstmt;
		//ResultSet *rows;
		//query = "SHOW TABLES FROM ObjectProperties LIKE '" + Property + "';";
		//cout << query << endl;
		//pstmt = con->prepareStatement(query);
		//rows = pstmt->executeQuery();
		//rows->next();
		//int count = rows->rowsCount();
		//cout << count << endl;
		//if (count == 0){
		//	cout << "not exist" << endl;
		//	cout << "create table " << Property << endl;
		//	stmt->execute("CREATE TABLE " + Property + "  (id char (50) unique, count int) ");
		//}
		//else cout << "already exist" << endl;

		///* Insert object_id into property */
		//command = "INSERT INTO " + Property + " VALUES('" + obj_id + "',1) ON DUPLICATE KEY UPDATE count = count + 1;";
		//cout << command << endl;
		//stmt->execute(command);

		/* Create table */
		//stmt->execute("CREATE TABLE " + Property + " (id char (50) , dir_property char (50)) ");
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
			stmt->execute("CREATE TABLE " + Property + "  (id char (50) , dir_property char (50)) ");
		}
		else cout << "already exist" << endl;

		/* Insert object_id into property */
		dir_prop = file_dir + obj_id + "_" + Property;
		command = "INSERT INTO " + Property + " VALUES('" + obj_id + "','" + dir_prop + "');";
		std::cout << command << std::endl;
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
void MyService::mysqlRead(void){
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
		stmt = con->createStatement();
		std::cout << "Mysqlに接続しました" << std::endl;

		obj_count = 0;
		pstmt = con->prepareStatement("select id from " + property + ";");
		res = pstmt->executeQuery();
		cout << "Property = " << property << endl;
		cout << "***** targetOBJ *****" << endl;
		while (res->next()) {
			string select = res->getString("id");
			cout << select << endl;
			Obj_list.push_back(select);
			obj_count++;
		}
		cout << "*********************" << endl;

		string num_obj;
		string tmp = Int2String(obj_count);
		num_obj = "Count:" + tmp;
		std::cout << num_obj << std::endl;
		sendMsgToCtr("Robot", num_obj);

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

void MyService::mysqlSearchInit(void){
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
		stmt = con->createStatement();
		std::cout << "Mysqlに接続しました" << std::endl;

		pstmt = con->prepareStatement("select table_name from information_schema.columns where column_name='id' and table_schema='ObjectProperties';");
		res = pstmt->executeQuery();
		while (res->next()) {
			string select = res->getString("table_name");
			List.push_back(select);
		}
		std::cout << "List size is " << List.size() << std::endl;
		std::string msg = "Search:init_finish";
		std::cout << msg << std::endl;
		sendMsgToCtr("Robot", msg);

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


void MyService::mysqlSearch(void){
	try {
		Driver *driver;
		Connection *con;
		Statement *stmt;
		PreparedStatement *pstmt;
		ResultSet *rows;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://150.37.224.108:3306", "sig", "sigverse");
		/* Connect to the MySQL test database */
		con->setSchema("ObjectProperties");
		stmt = con->createStatement();
		std::cout << "Mysqlに接続しました" << std::endl;



		for (int i = 0; i < List.size(); i++){
			std::vector<std::string>::iterator LItr = std::find(RequestedProperties.begin(), RequestedProperties.end(), List[i]);
			if (LItr != RequestedProperties.end()){
				continue;
			}
			else{
				for (int j = 0; j < Obj_list.size(); j++){
					string query;
					//何回List[i]を検索したか
					query = "select id from " + List[i] + " where id='" + Obj_list[j] + "';";
					//std::cout << query << std::endl;
					pstmt = con->prepareStatement(query);
					rows = pstmt->executeQuery();
					rows->next();
					int count = rows->rowsCount();
					if (count == 0){
						cout << "not exist" << endl;
					}
					else {
						std::cout << "追加します" << std::endl;
						//Properties.push_back(Property(List[i], num + 1));
						std::map<std::string, int>::iterator itr = Properties.find(List[i]);
						if (itr == Properties.end()){
							//std::cout << "insert:" << List[i] << std::endl;
							Properties.insert(std::map<std::string, int>::value_type(List[i], 1));
						}
						else{
							itr->second++;
						}
					}
				}
			}
		}
		std::cout << (unsigned int)Properties.size() << std::endl;
		std::map<std::string, int>::iterator itr = Properties.begin();
		while (itr != Properties.end()){
			std::cout << (*itr).first << ":" << (*itr).second << std::endl;
			++itr;
		}

		delete con;
		delete pstmt;
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
		stmt->execute("DROP TABLE IF EXISTS attachment");
		stmt->execute("DROP TABLE IF EXISTS how_to_use");

		// create concept
		stmt->execute("CREATE TABLE attribute  (attribute char (50)) ");
		stmt->execute("INSERT INTO attribute VALUES ('name'),('color'),('shape'),('attachment'),('how_to_use')");
		// create name
		stmt->execute("CREATE TABLE name  (property char (50) unique) ");
		// create color
		stmt->execute("CREATE TABLE color  (property char (50) unique) ");
		// create shape
		stmt->execute("CREATE TABLE shape  (property char (50) unique) ");
		// create state
		stmt->execute("CREATE TABLE attachment  (property char (50) unique) ");
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

string MyService::Int2String(int number){
	stringstream ss;
	ss << number;
	return ss.str();

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