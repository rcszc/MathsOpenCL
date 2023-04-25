// mcore_ocl_PlatformDevice.
#include <iostream>
#include <windows.h>
#include <CL/cl.h>

#include "mcore_ocl_PlatformDevice.h"
#include "mcore_opencl.h"
#include "../framelog/crimson_logframe.h"

using namespace std;
using namespace CRLOG_CONS;

mocl_ui32 MLOAD_OPENCL_DEVICE::GetPlatformNumber() {
	cl_platform_id* platforms = nullptr;
	cl_uint numplatforms = NULL;
	cl_int errorcode = NULL;

	clGetPlatformIDs(NULL, nullptr, &numplatforms);
	platforms = new cl_platform_id[numplatforms];
	errorcode = clGetPlatformIDs(numplatforms, platforms, nullptr);

	if (errorcode != CL_SUCCESS || !numplatforms) {
		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPENCL OCLPD: Failed platforms. code:" << errorcode << CRLOGEND;
	}
	platforms_num = numplatforms;
	platformsptr = platforms;
	return platforms_num;
}

mocl_ui32 MLOAD_OPENCL_DEVICE::GetDevicemNumber(mocl_ui32 platform) {
	cl_uint numdevices = NULL;
	cl_int errorcode = NULL;

	errorcode = clGetDeviceIDs(platformsptr[platform], CL_DEVICE_TYPE_GPU, NULL, nullptr, &numdevices);
	if (errorcode != CL_SUCCESS || !numdevices) {
		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPENCL OCLPD: There is no GPU. code:" << errorcode << CRLOGEND;
	}
	return numdevices;
}

void MLOAD_OPENCL_DEVICE::GetDeviceID(mocl_ui32 platform, mocl_ui32 device, cl_device_id* deviceid) {
	cl_platform_id platforms = nullptr;
	cl_int errorcode = NULL;
	mocl_ui32 numplateforms = NULL;
	mocl_ui32 numdevices = NULL;

	clGetPlatformIDs(platform, &platforms, &numplateforms);

	if (platform <= numplateforms) {
		errorcode = clGetDeviceIDs(platforms, CL_DEVICE_TYPE_GPU, device, deviceid, &numdevices);
		if (errorcode != CL_SUCCESS || !numdevices) {
			ClogOut << ClogOut.SET(CLOG_ERROR) <<
				"MOPENCL OCLPD: Get DeviceID. code:" << errorcode << CRLOGEND;
		}
	}
	else {
		deviceid = nullptr;
		ClogOut << ClogOut.SET(CLOG_ERROR) << "MOPENCL OCLPD: no device." << CRLOGEND;
	}
}