#include "ExportFilesToDB.h"
 
#ifdef _WIN32
#include <io.h>
#include <iostream>
#include <fstream>
#else
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

ExportFilesToDB::ExportFilesToDB(const char * dbName, const char* key)
{
	m_dataPath = "";
	encodeKey = key;
	SQLiteUtil::shareSQLiteUtil()->openDB(dbName);
}

ExportFilesToDB::~ExportFilesToDB()
{ 
	SQLiteUtil::purgeSQLiteUtil();
}

void ExportFilesToDB::startAnlayseFolderPath(const char * filePath)
{ 
	 m_dataPath = filePath;
	 m_vdataFileList.clear();
	 this->dfsFolder(filePath);
}
 
string& replace_all_distinct(string& str,const string& old_value, const string& new_value)   
{   
	for(string::size_type pos(0);pos!=string::npos;pos+=new_value.length())   
	{   
		if( (pos=str.find(old_value,pos)) != std::string::npos )   
			str.replace(pos,old_value.length(),new_value);   
		else  
			break;   
	}   
	return str;   
}  

void buffer_split(std::string s, std::string delim,std::vector< std::string >* ret)  
{  
	size_t last = 0;  
	size_t index=s.find_first_of(delim,last);  
	std::string spliteItem = "";
	while (index!=std::string::npos)  
	{  
		spliteItem = s.substr(last,index-last);
		if(!spliteItem.empty())
		{
			ret->push_back(spliteItem);    
		}
		last=index+1;  
		index=s.find_first_of(delim,last);  
	}  
}

std::string getTableNameFromFileName(std::string fileName)
{   
	 size_t index = fileName.find(".txt");
	 if(index!=std::string::npos)
	 {
		 std::string tableName = "";
		 tableName = fileName.substr(0,index)+"Data";
		 return tableName;
	 }
	 return ""; 
}

void ExportFilesToDB::runExportDBHandler()
{
	std::vector<std::string> *pRecordVector = new std::vector<std::string>();
	for(size_t index=0;index<m_vdataFileList.size();index++)
	{    
		pRecordVector->clear();
		std::string fileName = m_dataPath+"\\"+m_vdataFileList.at(index).c_str(); 
		std::ifstream importFileStream;
		importFileStream.open(fileName.c_str());
		std::string recordItem;

		int lineIndex = 0;
		while(getline(importFileStream,recordItem))
		{
			if(!recordItem.empty()) 
			{
				pRecordVector->push_back(recordItem);
			}else
			{
				printf("error line is %d",lineIndex);
			}
			lineIndex ++;
		}  
		importFileStream.close();
		char buffer[256] = {0};
		std::string tableName = getTableNameFromFileName(m_vdataFileList.at(index)).c_str();
		sprintf(buffer,"create table %s(id NUMERIC PRIMARY KEY,content TEXT)",tableName.c_str());
		SQLiteUtil::shareSQLiteUtil()->createTable(buffer,tableName.c_str()); 
		std::string content = "";
		size_t recordSize = pRecordVector->size();   

		//和在采用合并插入 最大合并数目为500 
		size_t tmpIndex= 0; 
		size_t valueIndex=0;
		std::string appendSegment = "),";
		for(size_t repeatIndex = 0;repeatIndex < recordSize;repeatIndex++)
		{    
			if(recordSize < SQLite_MAX_SYNC_INSERT_LINE)
			break;

			if ((repeatIndex+1) % SQLite_MAX_SYNC_INSERT_LINE == 0)
			{  
				tmpIndex = repeatIndex + 1;
				memset(buffer,0,sizeof(buffer));
				sprintf(buffer,"insert into %s(id,content) VALUES",tableName.c_str());
				content= buffer; 
				appendSegment = "),";
				for (size_t index=0;index<SQLite_MAX_SYNC_INSERT_LINE;index++)
				{   
					if(index==SQLite_MAX_SYNC_INSERT_LINE-1)
					{
						appendSegment = ")";
					}
					valueIndex = index+tmpIndex-SQLite_MAX_SYNC_INSERT_LINE;
					sprintf(buffer,"%d",valueIndex);
					content = content + "( " + buffer + ", \'" +pRecordVector->at(valueIndex).c_str() +"\'" + appendSegment;
				}  
				SQLiteUtil::shareSQLiteUtil()->insertData(content,tableName); 
			}
		}
		if(tmpIndex < recordSize)
		{
			//追加封包
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"insert into %s(id,content) VALUES",tableName.c_str());
			content = buffer;  
			appendSegment = "),";
			for (size_t index=tmpIndex;index<recordSize;index++)
			{   
				if(index == recordSize -1)
				{
					appendSegment = ")";
				}
				sprintf(buffer,"%d",index);
				content = content + "( " + buffer + ", \'" +pRecordVector->at(index).c_str() +"\'" + appendSegment;
			}   
			SQLiteUtil::shareSQLiteUtil()->insertData(content,tableName);
		}  
	}  
	if(pRecordVector)
	{
		delete pRecordVector;
		pRecordVector = NULL;
	} 

	printf("\n 数据库写入完成 \n");
}

void ExportFilesToDB::exportAllTableInfo()
{
	for(size_t index=1;index<m_vdataFileList.size();index++)
	{
		printf("Export: Name: %s \n",m_vdataFileList.at(index).c_str());
	} 
}

void ExportFilesToDB::dfsFolder(std::string folderPath)
{ 
	_finddata_t FileInfo;
	std::string strfind = folderPath + "\\*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo); 
	if (Handle == -1L)
	{ 
		printf("find Folder Error! \n");
		return ;
	}
	do
	{
		if (FileInfo.attrib & _A_SUBDIR)    
		{
			if( (strcmp(FileInfo.name,".") != 0 ) &&(strcmp(FileInfo.name,"..") != 0))   
			{
				std::string newPath = folderPath + "\\" + FileInfo.name;
				dfsFolder(newPath);
			}
		}
		else  
		{ 
			std::string fileName = FileInfo.name;
			if (std::string::npos != fileName.find(".txt"))
			{ 
				m_vdataFileList.push_back(FileInfo.name);
				printf("Loading File: %s \n",FileInfo.name);
			} 
		}
	}while (_findnext(Handle, &FileInfo) == 0); 
	_findclose(Handle); 
}

void ExportFilesToDB::closeDB()
{
	SQLiteUtil *sql = SQLiteUtil::shareSQLiteUtil();
	if(0 != encodeKey.compare(""))
	{
		sql->enCodeSqlite3(encodeKey.c_str());
	}
	sql->closeDB();
}