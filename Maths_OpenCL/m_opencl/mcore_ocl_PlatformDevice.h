// mcore_ocl_PlatformDevice.
// rcsz 2023. version 1.0.0

#ifndef _MCORE_OCL_PLATFORMDEVICE_H
#define _MCORE_OCL_PLATFORMDEVICE_H
#include <atlstr.h>
#include "mcore_opencl.h"
#include "maths_opencl_tool.h"

class MLOAD_OPENCL_DEVICE :public _MLOAD_OPENCL_CORE {
protected:
	cl_platform_id* platformsptr = nullptr;
	mocl_ui32 platforms_num = NULL; // 平台数量.

	// GetPlatformNumber => platforms_num
	void GetDeviceID(mocl_ui32 platform, mocl_ui32 device, cl_device_id* id);

public:
	MLOAD_OPENCL_DEVICE() {};
	~MLOAD_OPENCL_DEVICE() {
		if (platformsptr != nullptr) {
			delete[] platformsptr;
			platformsptr = nullptr;
		}
	};

	mocl_ui32 GetPlatformNumber();
	mocl_ui32 GetDevicemNumber(mocl_ui32 platform);
};

#endif