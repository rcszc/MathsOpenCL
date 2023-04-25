// crimson_logframe.
// rcsz. File: .log

#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include "crimson_logframe.h"

using namespace std;

// system color def.
#define WINSYS_LOGCOLOR_RED    0x04
#define WINSYS_LOGCOLOR_ORANGE 0x06
#define WINSYS_LOGCOLOR_GREY   0x08
#define WINSYS_LOGCOLOR_LBLUE  0x0B
#define WINSYS_LOGCOLOR_BLUE   0x09
#define WINSYS_LOGCOLOR_PURPLE 0x05

string ENGINE_LOGLABLE[4] = { "<Error> ","<Warring> ","<Info> ","<Trace> " };

// value_fill_zero.
char* __NumberFillzero(const char* intime, int numberbits) {
	static char* _returnstr = {};
	string strvalue = intime;
	string retstr = {};

	for (LOG_UINT32 i = 0; i < LST(numberbits) - strvalue.size(); i++)
		retstr += '0';
	retstr += strvalue;

	_returnstr = new char[retstr.size() + 1];
	strcpy_s(_returnstr, retstr.size() + 1, retstr.c_str());
	return _returnstr;
}

// Time [xxxx.xx.xx.xx:xx:xx:xx ms].
char* __GetCurrenttime() {
	SYSTEMTIME _time = {};
	GetLocalTime(&_time); // GetSystemTime ms
	char tmpbuff[16] = {};

	sprintf_s(tmpbuff, "%d", _time.wYear);   string year = tmpbuff;
	sprintf_s(tmpbuff, "%d", _time.wMonth);  string month = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wDay);    string day = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wHour);   string hour = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wMinute); string minute = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wSecond); string second = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wMilliseconds); string millisecond = __NumberFillzero(tmpbuff, 3);

	string current_time = "null";
	current_time = "[" + year + "." + month + "." + day + " " + hour + ":" + minute + ":" + second + " " + millisecond + " ms]";
	char* rettimelable = new char[LST(current_time.size() + 1)];
	strcpy_s(rettimelable, LST(current_time.size() + 1), current_time.c_str());
	return rettimelable;
}

void __freechar(char* _CharMem, bool _Flag) {
	delete[] _CharMem;
	_CharMem = nullptr;
	_Flag = FALSE;
}

// crimson log frame console.
namespace CRLOG_CONS {

	// str.Highlight + lable
	void _crlog_coretool::__PrintColor(LOG_CCHAR _lable, WORD _color) {
		string retprinttext = __GetCurrenttime();
		CONSOLE_SCREEN_BUFFER_INFO csbi = {};

		handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(handle, &csbi);
		colorold = csbi.wAttributes;
		SetConsoleTextAttribute(handle, _color);

		retprinttext += ":";
		retprinttext += _lable;
		_printloglable = new char[retprinttext.size() + 1];
		strcpy_s(_printloglable, retprinttext.size() + 1, retprinttext.c_str());
	}

	// console log.
	char* _crlog_coretool::__logPrintSet(LOG_UINT32 MOD) {
		switch (MOD) {
		case(CLOG_ERROR):   { __PrintColor("[ERROR] ", WINSYS_LOGCOLOR_RED); break; }
		case(CLOG_WARRING): { __PrintColor("[WARRING] ", WINSYS_LOGCOLOR_ORANGE); break; }
		case(CLOG_INFO):    { __PrintColor("[INFO] ", WINSYS_LOGCOLOR_GREY); break; }
		case(CLOG_TRACE):   { __PrintColor("[TRACE] ", WINSYS_LOGCOLOR_LBLUE); break; }
		case(CLOG_PERF):    { __PrintColor("[PERF] ", WINSYS_LOGCOLOR_PURPLE); break; }
		}
		return _printloglable;
	}

	void _crlog_coretool::__logPrintEnd() const {
		// SET => End.free
		if (_setflag) { 
			__freechar(_printloglable, &_setflag);
		};
		SetConsoleTextAttribute(handle, colorold);
	}

	_crlog_ostream ClogOut;
	const _crlog_ostream& _crlog_ostream::operator<<(__int32 Integer) const {
		printf("%d", Integer);
		return *this;
	}
	const _crlog_ostream& _crlog_ostream::operator<<(unsigned long long Length) const {
		printf("%llu", Length);
		return *this;
	}
	const _crlog_ostream& _crlog_ostream::operator<<(float Float32) const {
		printf("%0.3f", Float32);
		return *this;
	}
	const _crlog_ostream& _crlog_ostream::operator<<(LOG_CCHAR Charstr) const {
		// Terminator, free lable.
		if (Charstr == CRLOGEND) __logPrintEnd();
		printf("%s", Charstr);
		return *this;
	}

	char* _crlog_ostream::SET(LOG_UINT32 Lable) {
		_setflag = TRUE;
		return __logPrintSet(Lable);
	}
}

// crimson log frame logfile.
namespace CRLOG_FILE {

	// crimson log frame file.
	void CRIMSON_LOGFRAME::LOGFRAME_logOperations(LOG_CCHAR FilePath, LOG_UINT32 MOD) {
		switch (MOD) {
		case(CLOGFILE_FTEST): {
			fstream _test_framelog(FilePath);
			// Test file.
			if (_test_framelog) {
				string _test_outinfo = "File_Test Exist: ";
				_test_outinfo += FilePath;
				CRLOG_CONS::ClogOut << CRLOG_CONS::ClogOut.SET(CLOG_INFO) << _test_outinfo.c_str() << CRLOGEND;
				_test_framelog.close();
			}
			else {
				File_state = FALSE;
				CRLOG_CONS::ClogOut << CRLOG_CONS::ClogOut.SET(CLOG_WARRING) << "File_Test NotExist." << CRLOGEND;
				_test_framelog.close();
			}
			break;
		}
		case(CLOGFILE_CLEAR): {
			fstream _clear_framelog(FilePath, ios::out | ios::trunc); // clear
			_clear_framelog.close();
			break;
		}
		case(CLOGFILE_WRITE): {
			fstream _write_framelog(FilePath, ios::app);
			_write_framelog << __GetCurrenttime() << _WriteData << endl;
			_write_framelog.close();
			break;
		}
		default: {
			File_state = FALSE;
			CRLOG_CONS::ClogOut << CRLOG_CONS::ClogOut.SET(CLOG_ERROR) << "'LOGFRAME_logOperations' MOD_error." << CRLOGEND;
		} }
	}

	void CRIMSON_LOGFRAME::__logaddlable(LOG_CCHAR text, LOG_UINT32 lable_num) {
		string chartemp = text;
		_WriteData = new char[LST(ENGINE_LOGLABLE[0].size() + chartemp.size() + 1)];
		strcpy_s(_WriteData, LST(ENGINE_LOGLABLE[0].size() + 1), (ENGINE_LOGLABLE[0] + chartemp).c_str());
	}

	void CRIMSON_LOGFRAME::LOGFRAME_logTextSet(LOG_CCHAR LogText, LOG_UINT32 MOD) {
		switch (MOD) {
		case(CLOG_ERROR): { __logaddlable(LogText, 0); break; }
		case(CLOG_WARRING): { __logaddlable(LogText, 1); break; }
		case(CLOG_INFO): { __logaddlable(LogText, 2); break; }
		case(CLOG_TRACE): { __logaddlable(LogText, 3); break; }
		default: {
			File_state = FALSE;
			CRLOG_CONS::ClogOut << CRLOG_CONS::ClogOut.SET(CLOG_ERROR) << "'LOGFRAME_logTextSet' MOD_error." << CRLOGEND;
		} }
	}
}