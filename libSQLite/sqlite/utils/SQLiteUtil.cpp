#include "SQLiteUtil.h" 
 
SQLiteUtil*SQLiteUtil::m_pSQLiteUtilInstance = NULL; 
 
SQLiteUtil* SQLiteUtil::shareSQLiteUtil()
{ 
	if(!m_pSQLiteUtilInstance)
	{  
		m_pSQLiteUtilInstance = new SQLiteUtil(); 
	}
	return m_pSQLiteUtilInstance; 
}
  
void SQLiteUtil::purgeSQLiteUtil()
{
	if(m_pSQLiteUtilInstance)
	{
		delete m_pSQLiteUtilInstance;
		m_pSQLiteUtilInstance = NULL;
	}
}

SQLiteUtil::SQLiteUtil()
{
	m_pSqlite3	= NULL;
	m_nResult	= 0;
	m_sSqlCode	= "";
	m_sErrorMsg = "";
	m_sDBName	= "";
}
SQLiteUtil::~SQLiteUtil()
{
	closeDB();
}
  
void SQLiteUtil::debugInfoLog(std::string msg)
{
	printf("%s Failed!  Error Code : %d  Error Reason: %s\n ",msg.c_str(),m_nResult, m_sErrorMsg);
}
 
bool SQLiteUtil::openDB(const char *dbName)
{ 
	this->closeDB();
    m_nResult = sqlite3_open(dbName, &m_pSqlite3);
    if(SQLITE_OK == m_nResult)
	{
		return true;
	}
	debugInfoLog("openDB");
    return false;
}
 
int isExisted(void *para, int n_column, char** column_value, char** column_name )
{
    bool *isExisted_ = (bool*)para;
    *isExisted_ = (**column_value)!='0';
    return 0;
}
   
bool SQLiteUtil::tableIsExist( std::string name )
{
    if (m_pSqlite3)
    { 
        bool tableIsExisted;
        m_sSqlCode = "select count(type) from sqlite_master where type='table' and name ='"+name+"'";
        m_nResult =sqlite3_exec(m_pSqlite3,m_sSqlCode.c_str(),isExisted,&tableIsExisted,&m_sErrorMsg);
        return tableIsExisted;
    }
    return false;
}
 
// eg: std::string sqls = "create table user(id integer,username text,password text)";
void SQLiteUtil::createTable( std::string sql,std::string name)
{
    if (!tableIsExist(name) && m_pSqlite3)
    { 
        m_nResult = sqlite3_exec(m_pSqlite3,sql.c_str(),NULL,NULL,&m_sErrorMsg);
        if( m_nResult != SQLITE_OK )
		{
			printf("Error Table Name:%s  ErrorMsg:%s  sqlStr:%s ",name.c_str(),m_sErrorMsg,sql.c_str());
			debugInfoLog("createTable");
		}
    } 
}

//@示例语句sqlstr="drop table name";
void SQLiteUtil::deleteTable( std::string sql,std::string name )
{
    if (tableIsExist(name)&&m_pSqlite3)
    {
        m_nResult = sqlite3_exec(m_pSqlite3,sql.c_str(),NULL,NULL,&m_sErrorMsg);
        if( m_nResult != SQLITE_OK )
		{
			printf("Error Table Name:%s  ErrorMsg:%s  sqlStr:%s ",name.c_str(),m_sErrorMsg,sql.c_str());
			debugInfoLog("deleteTable"); 
		}
    }
}

void SQLiteUtil::insertData(std::string sql,std::string name)
{
	if(m_pSqlite3) 
	{ 
		m_nResult = sqlite3_exec( m_pSqlite3, sql.c_str() , NULL, NULL, &m_sErrorMsg );
		if(m_nResult != SQLITE_OK )
		{
			//printf("Error Table Name:%s  ErrorMsg:%s  sqlStr:%s ",name.c_str(),m_sErrorMsg,sql.c_str());
			 //debugInfoLog("insertData");
		}
	}	
}


//eg "delete from MyTable_1 where ID = 2";
void SQLiteUtil::deleteData(std::string sql)
{
	if(m_pSqlite3)
	{
		m_nResult=sqlite3_exec( m_pSqlite3, sql.c_str() , NULL, NULL, &m_sErrorMsg );
		if(m_nResult != SQLITE_OK )
		debugInfoLog("deleteData");
	} 
}


//eg: sqlstr="update MyTable_1 set name='威震天' where ID = 3";
void SQLiteUtil::updateData(std::string sql)
{
	if(m_pSqlite3)
	{ 
		m_nResult = sqlite3_exec( m_pSqlite3, sql.c_str() , NULL, NULL, &m_sErrorMsg );
		if(m_nResult != SQLITE_OK )
		debugInfoLog("updateData");
	}
}
 
//getDataCount的回调函数
int loadRecordCount( void * para, int n_column, char ** column_value, char ** column_name )
{
    int *count=(int*)para;
    *count=n_column;
    return 0;
}

//@示例语句std::string sqlsssss = "select count(*) from user";
int SQLiteUtil::getDataCount( std::string sql )
{   int count=0; 
	if(m_pSqlite3)
	{
	   m_nResult = sqlite3_exec( m_pSqlite3, sql.c_str() , loadRecordCount, &count, &m_sErrorMsg );
	   if(m_nResult != SQLITE_OK )
	   debugInfoLog("getDataCount"); 
	}
	return count; 
}

int loadRecord( void * para, int n_column, char ** column_value, char ** column_name )
{  
	vector<string> *v = (vector<string> *) para;
	v->push_back(string(column_value[1]));
	return 0;
}

vector<string>* SQLiteUtil::getDataInfo(std::string &sql)
{
	if(m_pSqlite3)
	{
		vector<string> *v = new vector<string>();
		m_nResult = sqlite3_exec(m_pSqlite3,sql.c_str(),loadRecord,v,&m_sErrorMsg);
		if(m_nResult!= SQLITE_OK)
		{
		   return v;
		}
	}
	return NULL;
}
 
void SQLiteUtil::closeDB()
{
	if(m_pSqlite3)
	{
		m_nResult = sqlite3_close(m_pSqlite3);
		if( m_nResult != SQLITE_OK )
		debugInfoLog("closeDB"); 
		m_pSqlite3 = NULL;
	} 
}

bool SQLiteUtil::enCodeSqlite3(std::string keys)
{
	if(m_pSqlite3)
	{
		m_nResult = sqlite3_rekey(m_pSqlite3,keys.c_str(),keys.length());
		if( m_nResult != SQLITE_OK )
		{
			debugInfoLog("enCodeSqlite3"); 
			return false;
		} 
		return true;
	}
	return false;
}
 

bool SQLiteUtil::deCodeSqlite3(std::string keys)
{
	if(m_pSqlite3)
	{
		m_nResult = sqlite3_key(m_pSqlite3,keys.c_str(),keys.length());
		if( m_nResult != SQLITE_OK )
		{
			debugInfoLog("deCodeSqlite3"); 
			return false;
		} 
		return true;
	}
	return false;
}
	 