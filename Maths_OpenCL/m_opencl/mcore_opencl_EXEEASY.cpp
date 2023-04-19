// load_opencl_EXEEASY.
#include <iostream>
#include <windows.h>
#include <CL/cl.h>

#include "mcore_opencl.h"
#include "maths_opencl_tool.h"
#include "../framelog/crimson_logframe.h"

using namespace std;
using namespace CRLOG_CONS;
// computing model [Ahpla], class MEASY_OPENCL

// init opencl [packet]
void MEASY_OPENCL::EMCL_InitMopencl(mocl_ccharptr CLfile_path, mocl_ccharptr CLfunction) {
	// init config opencl
	_MOCL_EXDAT.Context = _mocl_CreateContext(&_MOCL_EXDAT.Device);
	if (!_MOCL_EXDAT.Context)
		exit(_ERROR_CONTEXT);
	_MOCL_EXDAT.CommandQueue = _mocl_CreateCommandQueue(_MOCL_EXDAT.Context, _MOCL_EXDAT.Device);
	if (!_MOCL_EXDAT.CommandQueue) {
		MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
		exit(_ERROR_COMMANDQUEUE);
	}
	_MOCL_EXDAT.Program = _mocl_CreateProgram(_MOCL_EXDAT.Context, _MOCL_EXDAT.Device, CLfile_path);
	if (!_MOCL_EXDAT.Program) {
		MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
		exit(_ERROR_PROGRAM);
	}
	_MOCL_EXDAT.Kernel = clCreateKernel(_MOCL_EXDAT.Program, CLfunction, NULL);
	if (!_MOCL_EXDAT.Kernel) {
		MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
		exit(_ERROR_KERNEL);
	}
}

mocl_size MEASY_OPENCL::WORK_GROUP_SIZE[2] = { 1,1 }; // GPU工作组
void MEASY_OPENCL::EMCL_SetWorkGroupSize(mocl_size x, mocl_size y) {
	if ((x > 1) && (y > 1)) {
		WORK_GROUP_SIZE[0] = x;
		WORK_GROUP_SIZE[1] = y;
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPECL Ahpla: set work group n > 1" << CRLOGEND;
}

/*
@ 创建内存对象 & data=>buffer
*/
// Matrix 1D
// input.Matrix.size = output.Matrix.size
// Matrix.Number = inMatrix_num + 1
mocl_bit MEASY_OPENCL::EMCL_Matrix1Dahpla(mocl_Matrix2D inMatrix) {
	mocl_bit returnstate = true;
	_MOCL_EXDAT.MemObjects_num = inMatrix.mat_xlength + 1;
	_MOCL_EXDAT.MemObjects = new cl_mem[_MOCL_EXDAT.MemObjects_num];

	_Matrix_xlength = inMatrix.mat_ylength;
	_ARRAY_SIZE = _FLOAT_NUMSIZE(inMatrix.mat_ylength);

	_TotalTime->Timing_Start();
	_mocl_CreateMemObjects(_MOCL_EXDAT.Context, _MOCL_EXDAT.MemObjects_num, _MOCL_EXDAT.MemObjects, inMatrix);
	_mocl_SetKernelFunction(_MOCL_EXDAT.Kernel, _MOCL_EXDAT.MemObjects_num, _MOCL_EXDAT.MemObjects);

	// opencl core function
	// (global_work_size) kernel loop, matrix_num[1] => "get_global_id(0)"
	mocl_i32 errorcode = clEnqueueNDRangeKernel(_MOCL_EXDAT.CommandQueue, _MOCL_EXDAT.Kernel,
		1, NULL, &_Matrix_xlength, &WORK_GROUP_SIZE[0], NULL, nullptr, nullptr);

	if (errorcode != CL_SUCCESS) {
		// Execution failed.
		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPECL Ahpla: queueing kernel for execution. code:" << errorcode << CRLOGEND;

		MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
		returnstate = false;
	}
	return returnstate;
}

// Matrix 2D
mocl_bit MEASY_OPENCL::EMCL_Matrix2Dahpla(mocl_Matrix3D inMatrix) {
	mocl_bit returnstate = true;
	_MOCL_EXDAT.MemObjects_num = inMatrix.mat_xlength + 1;
	_MOCL_EXDAT.MemObjects = new cl_mem[_MOCL_EXDAT.MemObjects_num];

	_Matrix_xlength = inMatrix.mat_ylength;
	_Matrix_ylength = inMatrix.mat_zlength;
	_ARRAY_SIZE = _FLOAT_NUMSIZE(inMatrix.mat_ylength * inMatrix.mat_zlength);

	// convert.
	mocl_Matrix2D inbuffer = MOCL_TOOL_3DTO2D(inMatrix);
	
	_TotalTime->Timing_Start();
	_mocl_CreateMemObjects(_MOCL_EXDAT.Context, _MOCL_EXDAT.MemObjects_num, _MOCL_EXDAT.MemObjects, inbuffer);
	_mocl_SetKernelFunction(_MOCL_EXDAT.Kernel, _MOCL_EXDAT.MemObjects_num, _MOCL_EXDAT.MemObjects);
	mocl_size matrix_num[2] = { _Matrix_xlength, _Matrix_ylength };

	// opencl core function
	// (global_work_size) kernel loop, matrix_num[2] => "get_global_id(n)"
	mocl_i32 errorcode = clEnqueueNDRangeKernel(_MOCL_EXDAT.CommandQueue, _MOCL_EXDAT.Kernel,
		2, NULL, matrix_num, WORK_GROUP_SIZE, NULL, nullptr, nullptr);

	if (errorcode != CL_SUCCESS) {
		// Execution failed.
		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPECL Ahpla: queueing kernel for execution.code:" << errorcode << CRLOGEND;

		MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
		returnstate = false;
	}
	return returnstate;
}

/*
@ 结果拷贝到主机
output 1 matrix
*/
// Output_matrix_number = 1
mocl_Matrix1D MEASY_OPENCL::EMCL_ReadOutMatrix1D() {
	mocl_Matrix1D returnMatrx = {};
	// "clEnqueueReadBuffer" out Matrix 1D
	returnMatrx.mat_xlength = _FLOAT_SIZENUM(_ARRAY_SIZE);
	returnMatrx.data = new mocl_fp32[returnMatrx.mat_xlength];

	// read output.
	mocl_i32 errorcode = clEnqueueReadBuffer(_MOCL_EXDAT.CommandQueue,
		_MOCL_EXDAT.MemObjects[_MOCL_EXDAT.MemObjects_num - 1], CL_TRUE, NULL, _ARRAY_SIZE, &returnMatrx.data[0], NULL, NULL, NULL);
	// kernel compute end.
	EMCL_totalTime = _TotalTime->Timing_Ended();

	// convert. [old]
	// returnMatData = MOCL_TOOL_1DIN2D(readgpubuffer, _Matrix_xlength, _Matrix_ylength);

	MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
	if (errorcode != CL_SUCCESS) {

		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPECL Ahpla: reading result buffer. code:" << errorcode << CRLOGEND;
		
		delete[] returnMatrx.data;
		returnMatrx.mat_xlength = NULL;
	}
	return returnMatrx;
}

mocl_Matrix2D MEASY_OPENCL::EMCL_ReadOutMatrix2D() {
	mocl_Matrix1D readgpubuffer = {};
	// "clEnqueueReadBuffer" out Matrix 1D
	readgpubuffer.data = new mocl_fp32[_FLOAT_SIZENUM(_ARRAY_SIZE)];
	readgpubuffer.mat_xlength = _FLOAT_SIZENUM(_ARRAY_SIZE);

	// read output.
	mocl_i32 errorcode = clEnqueueReadBuffer(_MOCL_EXDAT.CommandQueue,
		_MOCL_EXDAT.MemObjects[_MOCL_EXDAT.MemObjects_num - 1], CL_TRUE, NULL, _ARRAY_SIZE, &readgpubuffer.data[0], NULL, NULL, NULL);
	// kernel compute end.
	EMCL_totalTime = _TotalTime->Timing_Ended();

	mocl_Matrix2D returnMatrx = MOCL_TOOL_1DTO2D(readgpubuffer, _Matrix_xlength, _Matrix_ylength);

	MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
	if (errorcode != CL_SUCCESS) {

		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPECL Ahpla: reading result buffer. code:" << errorcode << CRLOGEND;
		
		MOCL_TOOL_FREEMATRIX2D(returnMatrx);
	}
	return returnMatrx;
}