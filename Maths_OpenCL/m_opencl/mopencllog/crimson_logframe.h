// crimson_logframe.
// Maths_OpenCL 定制.
// rcsz. version 1.3.2

#ifndef _crimson_logframe_H
#define _crimson_logframe_H
#include <atlstr.h>

#define CLF_LOG_INFOHEAD "[logframe]: "

#define LST (size_t)      // => size_t [ull]
#define LCB (const char*) // => cchar

#define LEVEL_MAX 0

#define CLF_FILELOG_WRITE     0xA000F0A1 // write file
#define CLF_FILELOG_EXISTS    0xA000F0A0 // test  file
#define CLF_FILELOG_CLEARFILE 0xA000F0C0 // clear file

#define CLF_WRITELOG_ERROR    0xB000F0A0 // log_write <错误>
#define CLF_WRITELOG_WARRING  0xB000F0A1 // log_write <警告>
#define CLF_WRITELOG_INFO     0xB000F0A2 // log_write <信息>
#define CLF_WRITELOG_TRACE    0xB000F0A3 // log_write <跟踪>
#define CLF_WRITELOG_PERF     0xB000F0A4 // log_write <性能>

typedef const char*      WLOGCCHAR;
typedef unsigned __int32 LOG_UINT32;
typedef unsigned __int64 LOG_UINT64;

// MOD def "CLF_WRITELOG_"
// level max 0.1.2...
bool CONSOLE_LOGPRINTF(LOG_UINT32 MOD, WLOGCCHAR logcchar, LOG_UINT64 level);
#define CONLOG CONSOLE_LOGPRINTF // ( MOD, print_char, level )

// crimson framelog system
// version 1.1.0
class CRIMSON_LOGFRAME {
protected:
	char* _WriteData = {};
	void __logaddlable(WLOGCCHAR text, LOG_UINT32 lable_num);

public:
	CRIMSON_LOGFRAME() {}
	~CRIMSON_LOGFRAME() {
		delete _WriteData;
	}
	bool File_state = TRUE; // file state

	// LOG file_operation, MOD: def "CLF_FILELOG_"
	void LOGFRAME_logOperations(WLOGCCHAR _filepath, LOG_UINT32 MOD);

	// LOG write_settext, MOD: def "CLF_WRITELOG_"
	void LOGFRAME_logTextSet(WLOGCCHAR _logtext, LOG_UINT32 MOD);
};

// Tool. str + num[int].str
char* LOGTOOL_TEXTVALUE(WLOGCCHAR str, int num);
char* LOGTOOL_TEXTVALUE(WLOGCCHAR str, float num);

#define VATEXT LOGTOOL_TEXTVALUE // ( str,value ) return str + " " + value

#endif