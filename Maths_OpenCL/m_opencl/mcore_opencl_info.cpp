// load_opencl_info.
#include <iostream>
#include <windows.h>
#include <CL/cl.h>

#include "mcore_opencl.h"
#include "maths_opencl_tool.h"

using namespace std;
// version 1.0.0

#define _INFOPRINT_HEAD ""

#define CLCHAR_LENGTH_PLATFORM 30
#define CLCHAR_LENGTH_DEVICE   50

/*
@ 获取系统平台设备信息.
clGetPlatformIDs, clGetDeviceIDs
*/
mocl_i32 MOCL_GetSystemInfo() {
	mocl_i32 return_state = NULL;
	cl_uint numPlatforms = NULL;
	cl_platform_id* platforms = nullptr;

	// print info color
	WORD _colorold;
	HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(handle, &csbi);
	_colorold = csbi.wAttributes;
	SetConsoleTextAttribute(handle, 0x05);

	// clGetPlatfromIDs get.platforms_number
	cl_int status = clGetPlatformIDs(NULL, nullptr, &numPlatforms);
	if (status != CL_SUCCESS) {
		cout << "error : getting platforms failed";
		return_state = 1;
	}
	if (!numPlatforms)
		return_state = -1;

	cout << "FIND " << numPlatforms << " PLATFORM(S)" << endl;

	platforms = new cl_platform_id[numPlatforms];
	status = clGetPlatformIDs(numPlatforms, platforms, nullptr);

	mocl_size deviceNumber = NULL;

	// Print platform information.
	for (mocl_ui32 i = 0; i < numPlatforms; i++) {
		// platform
		cl_char* param = new cl_char[CLCHAR_LENGTH_PLATFORM]; 
		cout << "PLATFORM " << i << " INFOMATION: " << endl;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, CLCHAR_LENGTH_PLATFORM, param, nullptr);    cout << _INFOPRINT_HEAD << "> name:    " << param << endl;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, CLCHAR_LENGTH_PLATFORM, param, nullptr);  cout << _INFOPRINT_HEAD << "> vendor:  " << param << endl;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, CLCHAR_LENGTH_PLATFORM, param, nullptr); cout << _INFOPRINT_HEAD << "> version: " << param << endl;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, CLCHAR_LENGTH_PLATFORM, param, nullptr); cout << _INFOPRINT_HEAD << "> profile: " << param << endl;
		cout << "\n###############################################\n" << endl;
		delete[] param;

		// get device.
		cl_device_id* devices = nullptr;
		cl_uint numDevices = NULL;

		status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, NULL, nullptr, &numDevices);
		devices = new cl_device_id[numDevices];

		cout << "PLATFORM " << i << " HAS " << numDevices << " DEVICE(S): " << endl;
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices, nullptr);

		// Print device information.
		for (mocl_ui32 j = 0; j < numDevices; j++) {
			// device
			cl_char* device_param = new cl_char[CLCHAR_LENGTH_DEVICE]; cout << "DEVICE " << j << " INFOMATION: " << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, CLCHAR_LENGTH_DEVICE, device_param, nullptr);    cout << " - name:     " << device_param << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, CLCHAR_LENGTH_DEVICE, device_param, nullptr);  cout << " - vendor:   " << device_param << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, CLCHAR_LENGTH_DEVICE, device_param, nullptr); cout << " - version:  " << device_param << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, CLCHAR_LENGTH_DEVICE, device_param, nullptr); cout << " - profile:  " << device_param << endl;

			// opencl system size_t
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(mocl_size), &deviceNumber, nullptr);
			cout << _INFOPRINT_HEAD << "Constant_NumberMax:  " << deviceNumber << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(mocl_size), &deviceNumber, nullptr);
			cout << _INFOPRINT_HEAD << "Clock_FrequencyMax:  " << deviceNumber << " MHz" << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mocl_size), &deviceNumber, nullptr);
			cout << _INFOPRINT_HEAD << "Global_MemorySize:   " << deviceNumber / double(1024.0 * 1024.0) << " MiB" << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(mocl_size), &deviceNumber, nullptr);
			cout << _INFOPRINT_HEAD << "Global_CahceSize:    " << deviceNumber / 1024.0 << " KiB" << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mocl_size), &deviceNumber, nullptr);
			cout << _INFOPRINT_HEAD << "Const_BufferSize:    " << deviceNumber / double(1024.0 * 1024.0) << " MiB" << endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(mocl_size), &deviceNumber, nullptr);
			cout << _INFOPRINT_HEAD << "WorkGroup_SizeMax:   " << deviceNumber << endl;
			cout << "-----------------------------------------------" << endl;

			delete[] device_param;
		}
		delete[] devices;
	}
	delete[] platforms;
	// 修改回原色.
	SetConsoleTextAttribute(handle, _colorold);

	return return_state;
}