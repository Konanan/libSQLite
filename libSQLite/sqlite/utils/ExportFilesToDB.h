#ifndef EXPORTFILES_DB_H
#define EXPORTFILES_DB_H
 
#include "SQLiteUtil.h"
#include <istream>
#include <vector>
using namespace std;
 
//扫描目录下所有txt文件
//逐一读取txt文件,并解析读取分割数据
//插入将txt内容转换为对应的Table表存储
//关闭数据接口

class ExportFilesToDB
{   
public:
	ExportFilesToDB(const char * dbName, const char* key = "");
	~ExportFilesToDB();

public:
	void startAnlayseFolderPath(const char * filePath);
	void runExportDBHandler();
	void exportAllTableInfo();
	void closeDB();

private:
	void dfsFolder(std::string folderPath);

private:
	string m_dataPath;
	string encodeKey;
	vector<string> m_vdataFileList;

};
 
#endif
