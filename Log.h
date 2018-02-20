/*************************************************************************
	> File Name: Log.h
	> Author: 
	> Mail: 
	> Created Time: 2018年02月13日 星期二 17时02分15秒
 ************************************************************************/

#ifndef _LOG_H
#define _LOG_H

#include <string>
#include <fstream>

using namespace libfly;

namespace Log
{
    namespace LevelType{//less is more important.
         extern const int _TRACE;
         extern const int INFO;
         extern const int _ERROR;
        extern const int NAUGHT; //no output
    }
}

class LogClass
{
    public:
        simpleLogClass();
        void trace(std::string strTrace);
        void info(std::string strInfo);
        void error(std::string strInfo);

        std::string fileName;
        std::string fileOldName;
        long   fileMaxSize;
        int    outputLevel;
    private:
        void outputDebugInfo(int _outputLevel, std::string strInfo);
        void limitFileSize();
        long getFileSize();
        void writeTime(ofstream &file);
};


#endif
