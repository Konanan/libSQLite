#ifndef EXPORTFILES_DB_H
#define EXPORTFILES_DB_H
 
#include "SQLiteUtil.h"
#include <istream>
#include <vector>
using namespace std;
 
//ɨ��Ŀ¼������txt�ļ�
//��һ��ȡtxt�ļ�,��������ȡ�ָ�����
//���뽫txt����ת��Ϊ��Ӧ��Table��洢
//�ر����ݽӿ�

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
