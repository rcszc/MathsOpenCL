// crimson_logframe_write.
// rcsz. File: .log

#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include "crimson_logframe.h"

using namespace std;

#define WINSYS_LOGCOLOR_RED    0x04
#define WINSYS_LOGCOLOR_ORANGE 0x06
#define WINSYS_LOGCOLOR_GREY   0x08
#define WINSYS_LOGCOLOR_LBLUE  0x0B
#define WINSYS_LOGCOLOR_BLUE   0x09
#define WINSYS_LOGCOLOR_PURPLE 0x05

string ENGINE_LOGLABLE[4] = { "<Error> ","<Warring> ","<Info> ","<Trace> " };

// value_fill_zero
char* __number_fillzero(const char* intime, int numberbits) {
	static char* _returnstr = {};
	string strvalue = intime;
	string retstr = {};

	for (int i = 0; i < numberbits - strvalue.size(); i++)
		retstr += '0';
	retstr += strvalue;

	_returnstr = new char[retstr.size() + 1];
	strcpy_s(_returnstr, retstr.size() + 1, retstr.c_str());
	return _returnstr;
}

// Time [xxxx.xx.xx.xx:xx:xx:xx ms]
char* __get_currenttime() {
	SYSTEMTIME _time = {};
	GetLocalTime(&_time); // GetSystemTime ms
	char tmpbuff[16] = {};

	sprintf_s(tmpbuff, "%d", _time.wYear);   string year = tmpbuff;
	sprintf_s(tmpbuff, "%d", _time.wMonth);  string month = __number_fillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wDay);    string day = __number_fillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wHour);   string hour = __number_fillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wMinute); string minute = __number_fillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wSecond); string second = __number_fillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wMilliseconds); string millisecond = __number_fillzero(tmpbuff, 3);

	string current_time = "null";
	current_time = "[" + year + "." + month + "." + day + " " + hour + ":" + minute + ":" + second + " " + millisecond + " ms]";
	char* ret = new char[LST(current_time.size() + 1)];
	strcpy_s(ret, LST(current_time.size() + 1), current_time.c_str());
	return ret;
}

// str高亮 + cout
void __LOGprintf(WLOGCCHAR _logstr, WLOGCCHAR _lable, WORD _color) {
	WORD _colorold;
	HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(handle, &csbi);
	_colorold = csbi.wAttributes;
	SetConsoleTextAttribute(handle, _color);
	cout << __get_currenttime() << ":" << _lable << _logstr << endl;
	SetConsoleTextAttribute(handle, _colorold);
}

// 控制台输出
bool CONSOLE_LOGPRINTF(LOG_UINT32 MOD, WLOGCCHAR logcchar, LOG_UINT64 level) {
	string _outstr = {};
	for (int i = 0; i < level; i++)
		_outstr += " ";
	_outstr += logcchar;
	switch (MOD) { 
	case(CLF_WRITELOG_ERROR):   { __LOGprintf(_outstr.c_str(), "[ERROR] ", WINSYS_LOGCOLOR_RED); break; }
	case(CLF_WRITELOG_WARRING): { __LOGprintf(_outstr.c_str(), "[WARRING] ", WINSYS_LOGCOLOR_ORANGE); break; }
	case(CLF_WRITELOG_INFO):    { __LOGprintf(_outstr.c_str(), "[INFO] ", WINSYS_LOGCOLOR_GREY); break; }
	case(CLF_WRITELOG_TRACE):   { __LOGprintf(_outstr.c_str(), "[TRACE] ", WINSYS_LOGCOLOR_LBLUE); break; }
	case(CLF_WRITELOG_PERF):    { __LOGprintf(_outstr.c_str(), "[PERF] ", WINSYS_LOGCOLOR_PURPLE); break; }
	}
	return TRUE;
}

void CRIMSON_LOGFRAME::LOGFRAME_logOperations(WLOGCCHAR _filepath, LOG_UINT32 MOD) {
	switch (MOD) {
	case(CLF_FILELOG_EXISTS): {
		fstream _test_framelog(_filepath); // test file
		if (_test_framelog) {
			string _test_outinfo = "File test_file ";
			_test_outinfo += _filepath;
			CONSOLE_LOGPRINTF(CLF_WRITELOG_INFO, _test_outinfo.c_str(), LEVEL_MAX);
			_test_framelog.close();
		}
		else {
			CONSOLE_LOGPRINTF(CLF_WRITELOG_INFO, "File test_file error", LEVEL_MAX);
			File_state = FALSE;
			_test_framelog.close();
		}
		break;
	}
	case(CLF_FILELOG_CLEARFILE): {
		fstream _clear_framelog(_filepath, ios::out | ios::trunc); // clear
		_clear_framelog.close();
		break;
	}
	case(CLF_FILELOG_WRITE): {
		fstream _write_framelog(_filepath, ios::app);
		_write_framelog << __get_currenttime() << _WriteData << endl;
		_write_framelog.close();
		break;
	}
	default: {
		File_state = FALSE;
		cout << CLF_LOG_INFOHEAD << "'LOGFRAME_logOperations' MOD_error" << endl;
	}
	}
}

void CRIMSON_LOGFRAME::__logaddlable(WLOGCCHAR text, LOG_UINT32 lable_num) {
	string chartemp = text;
	_WriteData = new char[LST(ENGINE_LOGLABLE[0].size() + chartemp.size() + 1)];
	strcpy_s(_WriteData, LST(ENGINE_LOGLABLE[0].size() + 1), (ENGINE_LOGLABLE[0] + chartemp).c_str());
}

void CRIMSON_LOGFRAME::LOGFRAME_logTextSet(WLOGCCHAR _logtext, LOG_UINT32 MOD) {
	switch (MOD) {
	case(CLF_WRITELOG_ERROR):   { __logaddlable(_logtext, 0); break; }
	case(CLF_WRITELOG_WARRING): { __logaddlable(_logtext, 1); break; }
	case(CLF_WRITELOG_INFO):    { __logaddlable(_logtext, 2); break; }
	case(CLF_WRITELOG_TRACE):   { __logaddlable(_logtext, 3); break; }
	default: {
		File_state = FALSE;
		cout << CLF_LOG_INFOHEAD << "'LOGFRAME_logTextSet' MOD_error" << endl;
	}
	}
}

// Tool function

char* LOGTOOL_TEXTVALUE(WLOGCCHAR str, int num) {
	char* _retstr = {};
	string _ret = str;
	_ret += " " + to_string(num);
	_retstr = new char[_ret.size() + 1];
	strcpy_s(_retstr, _ret.size() + 1, _ret.c_str());
	return _retstr;
}

char* LOGTOOL_TEXTVALUE(WLOGCCHAR str, float num) {
    char* _retstr = {};
	string _ret = str;
	_ret += " " + to_string(num);
	_retstr = new char[_ret.size() + 1];
	strcpy_s(_retstr, _ret.size() + 1, _ret.c_str());
	return _retstr;
}