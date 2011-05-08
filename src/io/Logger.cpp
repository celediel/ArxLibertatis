
#include "io/Logger.h"

#include <cstdlib>

#include "platform/Platform.h"

#define BASH_COLOR !ARX_COMPILER_MSVC

using std::cout;

Logger::LogLevel Logger::logLevel = Logger::Info;

struct LogSetting {
	string codefile;
	Logger::LogLevel logLevel;
};

const LogSetting blackList[] = {
	{ "FTL.cpp", Logger::Warning },
	{ "PakManager.cpp", Logger::Fatal },
	{ "Filesystem.cpp", Logger::Fatal },
	{ "Object.cpp", Logger::Warning },
	{ "Speech.cpp", Logger::Error },
	{ "IO.cpp", Logger::Debug },
	{ "audio", Logger::Debug }
};

Logger::Logger(const std::string& file, int line, Logger::LogLevel level) {
  writeInfo(file.c_str(), line, level);
}

Logger::Logger(const char* file, int line, Logger::LogLevel level) {
	writeInfo(file, line, level);
}

Logger::~Logger() {
  if (print)
  {
    std::cout<<std::endl;
#if ARX_COMPILER_MSVC
	OutputDebugString("\n");
#endif
  }
  if (fatal)
	  exit(0);
}

void Logger::writeInfo(const char * longFile, int line, Logger::LogLevel level) {

  const char* file = std::strrchr(longFile, '/');
  if(file == 0)
    file = std::strrchr(longFile, '\\');
  ++file;
	
  fatal = false;
  LogLevel curLevel = getLogLevel(longFile);
  if(level < curLevel || curLevel == None) {
	  print = false;
	  return;
  }
  
  print = true;
  switch(level) {
    case Info:
      log(1,32,"INFO",file, line);
      break;
    case Warning:
      log(1,33,"WARNING",file, line);
      break;
    case Error:
      log(1,31,"ERROR",file, line);
      break;
    case Debug:
      log(1,36,"DEBUG",file, line);
      break;
    case Fatal:
      log(4,31,"FATAL",file, line);
      fatal = true;
      break;
    default:
      log(1,32,"INFO",file, line);
  };
}

void Logger::log(int mode, int color, const string & level,
		const string & file, int line) {
#if BASH_COLOR
	cout<<"[\e["<< mode <<";"<< color <<"m" << level << "\e[m]  "
			<<"\e[0;35m"<<file<<"\e[m:\e[0;33m"<<line<<"\e[m"<<"  ";
#else
	stringstream ss;
	ss <<"["<< level << "]  "<<file<<":"<<line<<"  ";
	cout << ss.str();
#if ARX_COMPILER_MSVC
	OutputDebugString(ss.str().c_str());
#endif
#endif
}

Logger::LogLevel Logger::getLogLevel(const string & file) {
	for (unsigned i=0; i < sizeof(blackList)/sizeof(*blackList); i++) {
		if (file.find(blackList[i].codefile) != string::npos)
			return blackList[i].logLevel;
	}
	return logLevel;
}

Logger & Logger::operator<<(const nullstr & s) {
	if(print) {
		if(s.str) {
			*this << "\"" << s.str << "\"";
		} else {
			*this << "NULL";
		}
	}
	return *this;
}

