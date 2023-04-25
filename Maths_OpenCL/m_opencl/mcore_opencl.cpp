// load_opencl.
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <CL/cl.h>

#include "mcore_opencl.h"
#include "maths_opencl_tool.h"
#include "mcore_ocl_PlatformDevice.h"

using namespace std;
using namespace CRLOG_CONS;

#define _END '\0'
#define OCL_PROGRAM_LOGLEN 10240

/*
@ 清理opencl资源
_mocl_cleanpacket => MOCL_ClearCLpacket
*/
mocl_bit _mocl_cleanpacket(
	cl_context context, cl_command_queue commandQueue, cl_program program, cl_kernel kernel,
	cl_mem* memObjects, mocl_size mObjsize
) {
	mocl_bit MEMnullptr = true;
	if (memObjects) {
		for (mocl_i32 i = 0; i < mObjsize; i++)
			clReleaseMemObject(memObjects[i]);
	}
	else
		MEMnullptr = false;

	if (commandQueue) clReleaseCommandQueue(commandQueue);
	if (kernel)       clReleaseKernel(kernel);
	if (program)      clReleaseProgram(program);
	if (context)      clReleaseContext(context);

	return MEMnullptr;
}

mocl_bit MOCL_ClearCLpacket(mocl_CLpacket packet) {
	mocl_bit returnstate = _mocl_cleanpacket(packet.Context, packet.CommandQueue, packet.Program, packet.Kernel,
		packet.MemObjects, packet.MemObjects_num);
	return returnstate;
}

void _mocl_errorexit(mocl_i32 exitcode, mocl_CLpacket packet) {
	MOCL_ClearCLpacket(packet);
	exit(exitcode);
}

// class _MLOAD_OPENCL_CORE
/*
@ 创建opencl上下文
*/
cl_context _MLOAD_OPENCL_CORE::_mocl_CreateContext(cl_device_id* device) {
	mocl_i32 errorcode = NULL;
	cl_context context = NULL;

	getdeviceid = new MLOAD_OPENCL_DEVICE[1];
	getdeviceid->GetDeviceID(Nplatform, Ndevice, device);

	context = clCreateContext(NULL, 1, device, NULL, NULL, &errorcode);
	if (errorcode != CL_SUCCESS) {

		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPENCL Core: Create context. code:" << errorcode << CRLOGEND;
		return nullptr;
	}
	return context;
}

/*
@ 创建opencl命令队列
*/
cl_command_queue _MLOAD_OPENCL_CORE::_mocl_CreateCommandQueue(cl_context context, cl_device_id device) {
	cl_command_queue commandQueue = nullptr;
	// OpenCL 2.0 的用法
	// CommandQueue = clCreateCommandQueue(context, device, 0, NULL);

	commandQueue = clCreateCommandQueueWithProperties(context, device, NULL, NULL);
	if (commandQueue == NULL) {

		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPENCL Core: Failed to create commandQueue for device 0." << CRLOGEND;
		return nullptr;
	}
	return commandQueue;
}

/*
@ 读取opencl核函数文件
Kernel function File: .cl
*/
mocl_charptr _MLOAD_OPENCL_CORE::_mocl_ReadKernelSourceFile(mocl_ccharptr filename, mocl_sizeptr length) {
	FILE* readfile = nullptr;
	mocl_charptr sourceString = nullptr;
	mocl_size sourceLenth = NULL;
	mocl_size readnum = NULL;

	// test.file
	fopen_s(&readfile, filename, "rb");
	if (!readfile) {
		cout << ClogOut.SET(CLOG_WARRING) << "MOPENCL Core: Can't open " << filename << endl;
		return nullptr;
	}

	// file.end
	fseek(readfile, NULL, SEEK_END);
	sourceLenth = ftell(readfile);

	// file.begin
	fseek(readfile, NULL, SEEK_SET);
	sourceString = new mocl_char[sourceLenth + 1];
	sourceString[0] = _END;
	if (sourceString)
		readnum = fread(sourceString, sourceLenth, (_CVT_SIZET)1, readfile);
	if (!readnum) {
		cout << ClogOut.SET(CLOG_WARRING) << "MOPENCL Core: Cant't read source " << filename << endl;
		return nullptr;
	}

	fclose(readfile);
	if (length)
		*length = sourceLenth;
	sourceString[sourceLenth] = _END;

	return sourceString;
}

/*
@ 创建opencl程序
Read kernel function
*/
cl_program _MLOAD_OPENCL_CORE::_mocl_CreateProgram(cl_context context, cl_device_id device, mocl_ccharptr filename) {
	cl_program program = nullptr;
	mocl_size program_length = NULL;
	mocl_i32 errorcode = NULL;

	mocl_charptr const source = _mocl_ReadKernelSourceFile(filename, &program_length);
	program = clCreateProgramWithSource(context, 1, (mocl_ccharptr*)&source, NULL, NULL);

	if (!program) {
		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPENCL Core: Failed to creae CL program from source." << CRLOGEND;
		return nullptr;
	}

	errorcode = clBuildProgram(program, NULL, NULL, NULL, NULL, NULL);
	if (errorcode != CL_SUCCESS) {
		mocl_charptr buildLog = new mocl_char[OCL_PROGRAM_LOGLEN];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, OCL_PROGRAM_LOGLEN * sizeof(mocl_char), buildLog, NULL);
		
		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"CLError in kernel: " << buildLog << CRLOGEND;

		clReleaseProgram(program);
		delete[] buildLog;
		return nullptr;
	}
	return program;
}

/*
@ 创建opencl内存对象
inDat.mat_xlen = memObject_num - 1
memObjects.size = inDat.mat_xlen + 1 [out_object]
IO.Data 1D matrix * n
*/
mocl_bit _MLOAD_OPENCL_CORE::_mocl_CreateMemObjects(cl_context context, mocl_size memObject_num, cl_mem* memObjects, mocl_Matrix2D inDat) {
	mocl_bit returnstate = true;

	if (memObject_num == inDat.mat_xlength + 1) {
		// in matrix data memObject.
		for (mocl_size i = 0; i < inDat.mat_xlength; i++)
			memObjects[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, _ARRAY_SIZE, &inDat.data[i][0], NULL);
		// output MEMdata.
		memObjects[memObject_num - 1] = clCreateBuffer(context, CL_MEM_READ_WRITE, _ARRAY_SIZE, NULL, NULL);

		MOCL_TOOL_FREEMATRIX2D(inDat);

		for (int i = 0; i < memObject_num; i++) {
			if (!memObjects[i]) {
				cout << "CLError creating memeory objects[" << i << "]." << endl;
				returnstate = false;
			}
		}
	}
	else 
		ClogOut << ClogOut.SET(CLOG_ERROR) << "MOPENCL Core: memObject_num != inDat.mat_xlen - 1" << CRLOGEND;

	return returnstate;
}

// High performance computing.
// clCreateBuffer + clEnqueueWriteBuffer
// [clCreateBuffer]
mocl_bit _MLOAD_OPENCL_CORE::_mocl_CreateMEMOBJ_HIGH(cl_context context, mocl_size MEMOBJ_in, mocl_size MEMOBJ_out, cl_mem* memObjects) {
	mocl_size i = NULL;
	mocl_bit returnstate = true;

	if (MEMOBJ_in && MEMOBJ_out) {
		// in matrix data memObject.
		for (i; i < MEMOBJ_in; i++) {
			memObjects[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, _ARRAY_SIZE, nullptr, NULL);
			if (!memObjects[i])
				returnstate = false;
		}
		// output MEMdata.
		for (mocl_size j = MEMOBJ_in; j < MEMOBJ_in + MEMOBJ_out; j++) {
			memObjects[j] = clCreateBuffer(context, CL_MEM_READ_WRITE, _ARRAY_SIZE, nullptr, NULL);
			if (!memObjects[j])
				returnstate = false;
		}
	}
	else
		ClogOut << ClogOut.SET(CLOG_ERROR) << "MOPENCL Core: memObject_IO > 0" << CRLOGEND;

	return returnstate;
}

// [clEnqueueWriteBuffer]
mocl_bit _MLOAD_OPENCL_CORE::_mocl_WriteMEMOBJ_HIGH(cl_command_queue command, mocl_size MEMOBJ_in, cl_mem* memObjects, mocl_Matrix2D inDat) {
	mocl_bit returnstate = true;

	if (MEMOBJ_in) {
		if (inDat.data != nullptr) {
			for (mocl_size i = 0; i < MEMOBJ_in; i++)
				clEnqueueWriteBuffer(command, memObjects[i], CL_TRUE, NULL, _ARRAY_SIZE, &inDat.data[i][0], NULL, NULL, NULL);
		}
	}
	else {
		ClogOut << ClogOut.SET(CLOG_ERROR) << "MOPENCL Core: memObject_in > 0" << CRLOGEND;
		returnstate = false;
	}
	MOCL_TOOL_FREEMATRIX2D(inDat);
	return returnstate;
}

/*
@ 设置Kernel参数
[0,num - 1]
output_parameters: num - 1 (最右参数开始)
*/
mocl_bit _MLOAD_OPENCL_CORE::_mocl_SetKernelFunction(cl_kernel kernel, mocl_size memObj_num, cl_mem* memObj) {
	mocl_bit returnstate = true;
	mocl_i32 errorcode = NULL;

	for (mocl_size i = 0; i < memObj_num; i++) {
		errorcode = clSetKernelArg(kernel, (mocl_ui32)i, sizeof(cl_mem), &memObj[i]);
		if (errorcode != CL_SUCCESS) {
			ClogOut << ClogOut.SET(CLOG_ERROR) <<
				"MOPENCL Core: setting kernel arguments. Num:" << mocl_i32(i) << CRLOGEND;
			returnstate = false;
		}
	}
	return returnstate;
}