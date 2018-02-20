/*************************************************************************
	> File Name: Log.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年02月13日 星期二 17时02分24秒
 ************************************************************************/

#include<iostream>
#include "Log.h"

#include <cstdio>  

#include <boost/thread/thread.hpp>

using namespace libfly;

namespace simpleLog
{
	namespace LevelType{//less is more important.
		const int _TRACE = 90;
		const int INFO = 50;
		const int _ERROR = 30;
		const int NAUGHT = 0; //no output
	}
}

simpleLogClass::simpleLogClass()
{
	fileName = "simpleLog.log";
	fileOldName = "simpleLog.old.log";
	fileMaxSize = 64 * 1024;//default is 64KB size.
	outputLevel = simpleLog::LevelType::_TRACE;
}

long simpleLogClass::getFileSize() {
	FILE * pFile;
	long size;

	pFile = fopen(fileName.c_str(), "rb");
	if (pFile == NULL)
		return 0l;

	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	fclose(pFile);
	return size;
}

void simpleLogClass::limitFileSize()
{
	if (getFileSize() > fileMaxSize)
	{
		//if old name file exist, remove it.
		remove(fileOldName.c_str());

		//rename the current file to old name file.
		rename(fileName.c_str(), fileOldName.c_str());
	}
}

void simpleLogClass::writeTime(ofstream &file)
{
	time_t t;
	tm *lt;
	t = time(NULL);
	lt = localtime(&t);

	file << (lt->tm_year + 1900) << "-" << (lt->tm_mon + 1) << "-" << lt->tm_mday << " ";
	file << lt->tm_hour << ":" << lt->tm_min << ":" << lt->tm_sec << "  ";
}

void simpleLogClass::outputDebugInfo(int _outputLevel, std::string strInfo)
{
	static boost::mutex mInfo;
	boost::lock_guard<boost::mutex> lock(mInfo);

	limitFileSize();

	std::string strLevel;
	switch (_outputLevel)
	{
	case simpleLog::LevelType::_TRACE:
		strLevel = "trace";
		break;
	case simpleLog::LevelType::INFO:
		strLevel = "info";
		break;
	case simpleLog::LevelType::_ERROR:
		strLevel = "error";
		break;
	}

	std::ofstream file(fileName.c_str(), ios::app | ios::out);
	writeTime(file);
	file << strLevel.c_str() << " " << strInfo.c_str() << endl;
	file.close();
}

void simpleLogClass::trace(std::string strTrace)
{
	if (outputLevel >= simpleLog::LevelType::_TRACE)
	{
		outputDebugInfo(simpleLog::LevelType::_TRACE, strTrace);
	}
}

void simpleLogClass::info(std::string strInfo)
{
	if (outputLevel >= simpleLog::LevelType::INFO)
	{
		outputDebugInfo(simpleLog::LevelType::INFO, strInfo);
	}
}

void simpleLogClass::error(std::string strInfo)
{
	if (outputLevel >= simpleLog::LevelType::_ERROR)
	{
		outputDebugInfo(simpleLog::LevelType::_ERROR, strInfo);
	}
}
