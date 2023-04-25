// crimson_logframe. mopencl.
// rcsz. version 2.0.0

#ifndef _crimson_logframe_H
#define _crimson_logframe_H
#include <atlstr.h>
#include <string>

#define CLF_LOG_INFOHEAD "[logframe]: "

#define LST (size_t)      // => size_t [ull]
#define LCB (const char*) // => cchar

#define LEVEL_MAX 0

#define CLOGFILE_WRITE  0xA000F0A1 // WRITE File
#define CLOGFILE_FTEST  0xA000F0A2 // TEST  File
#define CLOGFILE_CLEAR  0xA000F0C0 // CLEAR File

#define CLOG_ERROR    0xB000F0A0 // Log_out <错误>
#define CLOG_WARRING  0xB000F0A1 // Log_out <警告>
#define CLOG_INFO     0xB000F0A2 // Log_out <信息>
#define CLOG_TRACE    0xB000F0A3 // Log_out <跟踪>
#define CLOG_PERF     0xB000F0A4 // Log_out <性能>

typedef const char*      LOG_CCHAR;
typedef unsigned __int32 LOG_UINT32;
typedef unsigned __int64 LOG_UINT64;

#define CRLOGEND "\n"

// crimson framelog console.
// version 2.2.0
namespace CRLOG_CONS {
	class _crlog_coretool {
	protected:
		HANDLE handle = nullptr;
		WORD colorold = NULL;
		bool _setflag = FALSE;
		char* _printloglable = nullptr;

		void __PrintColor(LOG_CCHAR _lable, WORD _color);
		char* __logPrintSet(LOG_UINT32 MOD);
		void __logPrintEnd() const;
	};

	class _crlog_ostream :public _crlog_coretool {
	public:
		const _crlog_ostream& operator<<(__int32 Integer) const;
		const _crlog_ostream& operator<<(unsigned long long Length) const;
		const _crlog_ostream& operator<<(float Float32) const;
		const _crlog_ostream& operator<<(LOG_CCHAR Cahrstr) const;

		// lable def "CLOG_"
		char* SET(LOG_UINT32 Lable);
	};

	extern _crlog_ostream ClogOut;
}

// crimson framelog logfile.
// version 1.5.0
namespace CRLOG_FILE {
	class CRIMSON_LOGFRAME {
	protected:
		char* _WriteData = nullptr;
		void __logaddlable(LOG_CCHAR text, LOG_UINT32 lable_num);

	public:
		CRIMSON_LOGFRAME() {}
		~CRIMSON_LOGFRAME() {
			delete _WriteData;
		}
		bool File_state = TRUE; // file state

		// log.file_operation, Mode: def "CLF_LOGFILE_"
		void LOGFRAME_logOperations(LOG_CCHAR FilePath, LOG_UINT32 MOD);

		// log.write_settext, Mode: def "CLF_LOGLABLE_"
		void LOGFRAME_logTextSet(LOG_CCHAR LogText, LOG_UINT32 MOD);
	};
}

#endif
// Demo: ClogOut << ClogOut.SET(CLOG_INFO) << "Test Info: " << 114514 << CRLOGEND;