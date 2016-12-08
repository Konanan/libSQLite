#ifndef SQLITEUTIL_H
#define SQLITEUTIL_H
 
#include <string>
#include <map>
#include <vector>
#include "../sqlite3/sqlite3.h"

using namespace std;

const int SQLite_MAX_SYNC_INSERT_LINE = 400;
const int SQLIte_EMPTY_LINE_LENGTH = 512;

class SQLiteUtil{  
public:  
	 static SQLiteUtil* shareSQLiteUtil();
	 static void purgeSQLiteUtil();
	 SQLiteUtil();
	 virtual ~SQLiteUtil();
protected:  
	 static SQLiteUtil *  m_pSQLiteUtilInstance;
	 int				  m_nResult;
	 sqlite3 *			  m_pSqlite3;
	 std::string		  m_sSqlCode; 
	 std::string		  m_sDBName;
	 char *				  m_sErrorMsg;   
protected:
	void debugInfoLog(std::string msg);
public: 
	bool openDB(const char *dbName);
	
	void closeDB();

    bool tableIsExist(std::string name);
 
    void createTable(std::string sql,std::string name);
 
    void deleteTable(std::string sql,std::string name);
 
    void insertData(std::string sql,std::string name);
 
    void deleteData(std::string sql);
 
    void updateData(std::string sql);
 
    int  getDataCount(std::string sql);

	//�������ݿ���� (������close֮ǰ����)
	bool enCodeSqlite3(std::string keys);

	//�������ݿ����(�����ݿ���� ��Ҫ��open ֮�����Ͻ��ܲſ���ִ��)
	bool deCodeSqlite3(std::string keys);
	 
	vector<string>* getDataInfo(std::string &sql);
};
 
#endif

