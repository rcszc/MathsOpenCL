// load_opencl_EXEHIGH.
#include <iostream>
#include <windows.h>
#include <CL/cl.h>

#include "mcore_opencl.h"
#include "maths_opencl_tool.h"
#include "../framelog/crimson_logframe.h"

using namespace std;
using namespace CRLOG_CONS;
// 仅支持 Matrix2D ( in.Matrix3D, out.Matrix3D )
// 高性能计算建议:
// GPU memory >= 10240 MiB, CPU memory >= 32768 MiB
// GPU Mem.io.speed >= 25600 Mb/s

//init opencl [packet]
void MHIGH_OPENCL::HMCL_InitMopencl(mocl_ccharptr CLfile_path, mocl_ccharptr CLfunction) {
	// init config opencl
	_MOCL_EXDAT.Context = _mocl_CreateContext(&_MOCL_EXDAT.Device);
	if (!_MOCL_EXDAT.Context)
		_mocl_errorexit(_ERROR_CONTEXT, _MOCL_EXDAT);

	_MOCL_EXDAT.CommandQueue = _mocl_CreateCommandQueue(_MOCL_EXDAT.Context, _MOCL_EXDAT.Device);
	if (!_MOCL_EXDAT.CommandQueue)
		_mocl_errorexit(_ERROR_COMMANDQUEUE, _MOCL_EXDAT);

	_MOCL_EXDAT.Program = _mocl_CreateProgram(_MOCL_EXDAT.Context, _MOCL_EXDAT.Device, CLfile_path);
	if (!_MOCL_EXDAT.Program)
		_mocl_errorexit(_ERROR_PROGRAM, _MOCL_EXDAT);

	_MOCL_EXDAT.Kernel = clCreateKernel(_MOCL_EXDAT.Program, CLfunction, NULL);
	if (!_MOCL_EXDAT.Kernel)
		_mocl_errorexit(_ERROR_KERNEL, _MOCL_EXDAT);
}

mocl_size MHIGH_OPENCL::WORK_GROUP_SIZE[2] = { 1,1 }; // GPU工作组
void MHIGH_OPENCL::HMCL_SetWorkGroupSize(mocl_size x, mocl_size y) {
	if ((x > 1) && (y > 1)) {
		WORK_GROUP_SIZE[0] = x;
		WORK_GROUP_SIZE[1] = y;
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPECL Beta: set work group n > 1" << CRLOGEND;
}

// set input_memobj_num output_memobj_num
// Create mem objects
void MHIGH_OPENCL::HMCL_ResizeComputeIO(mocl_size In_matrix_num, mocl_size Out_matrix_num, mocl_size data_length) {
	_MemObj_in = In_matrix_num;
	_MemObj_out = Out_matrix_num;
	_ARRAY_SIZE = _FLOAT_NUMSIZE(data_length);

	if (_MOCL_EXDAT.MemObjects == nullptr) {
		_MOCL_EXDAT.MemObjects_num = In_matrix_num + Out_matrix_num;
		_MOCL_EXDAT.MemObjects = new cl_mem[_MOCL_EXDAT.MemObjects_num];
	}
	else {
		// free buffer.
		for (mocl_i32 i = 0; i < _MOCL_EXDAT.MemObjects_num; i++)
			clReleaseMemObject(_MOCL_EXDAT.MemObjects[i]);
	}
	_mocl_CreateMEMOBJ_HIGH(_MOCL_EXDAT.Context, In_matrix_num, Out_matrix_num, _MOCL_EXDAT.MemObjects);
}

void MHIGH_OPENCL::HMCL_SetPlatformDevice(mocl_ui32 NP, mocl_ui32 ND) {
	if ((NP > 1) && (ND > 1)) {
		Nplatform = NP;
		Ndevice = ND;
	}
	else
		ClogOut << ClogOut.SET(CLOG_WARRING) << "MOPECL Beta: set PlatformDevice Failed." << CRLOGEND;
}

/*
@ data => buffer matrix2d
size = set.IO
*/
// input.Matrix.size = output.Matrix.size
// input.Matrix.number / input.Matrix.number
mocl_bit MHIGH_OPENCL::HMCL_Matrix2Dbeta(mocl_Matrix3D inMatrix) {
	mocl_bit returnstate = true;
	if (inMatrix.mat_xlength == _MemObj_in) {

		_Matrix_xlength = inMatrix.mat_ylength;
		_Matrix_ylength = inMatrix.mat_zlength;
		// convert.
		mocl_Matrix2D inbuffer = MOCL_TOOL_3DTO2D(inMatrix);

		_TotalTime->Timing_Start();
		// write data => buffer
		if (_mocl_WriteMEMOBJ_HIGH(_MOCL_EXDAT.CommandQueue, _MemObj_in, _MOCL_EXDAT.MemObjects, inbuffer))
			statecode = MCLASS_STATE_CMEMOBJ;
		if (_mocl_SetKernelFunction(_MOCL_EXDAT.Kernel, _MOCL_EXDAT.MemObjects_num, _MOCL_EXDAT.MemObjects))
			statecode = MCLASS_STATE_CKERNEL;

		mocl_size matrix_num[2] = { _Matrix_xlength, _Matrix_ylength };
		mocl_i32 errorcode = clEnqueueNDRangeKernel(_MOCL_EXDAT.CommandQueue, _MOCL_EXDAT.Kernel,
			2, NULL, matrix_num, WORK_GROUP_SIZE, NULL, nullptr, nullptr);

		if (errorcode != CL_SUCCESS) {
			// Execution failed.
			ClogOut << ClogOut.SET(CLOG_ERROR) <<
				"MOPECL Beta: queueing kernel for execution. code:" << errorcode << CRLOGEND;
			
			MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
			returnstate = false;
		}
	}
	else {
		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPECL Beta: inMatrix entry != set_in_matrix" << CRLOGEND;
		returnstate = false;
	}
	return returnstate;
}

/*
@ 结果拷贝到主机
output n * matrix
*/
// output.matrix = n * 2d = 3d
mocl_Matrix3D MHIGH_OPENCL::HMCL_ReadOutMatrix2D() {
	mocl_Matrix1D readgpubuffer = {};
	mocl_i32 errorcode = NULL;
	// "clEnqueueReadBuffer" out Matrix 1D
	readgpubuffer.data = new mocl_fp32[_FLOAT_SIZENUM(_ARRAY_SIZE)];
	readgpubuffer.mat_xlength = _FLOAT_SIZENUM(_ARRAY_SIZE);

	mocl_Matrix3D returnMatrx = MOCL_TOOL_NEWMATRIX2D(_MemObj_out, _Matrix_xlength, _Matrix_ylength);
    
	// read output.
	// num = [0, _MemObj_out] out = [_MemObj_in, _MemObj_out]
	mocl_size count_out = NULL;
	for (mocl_size i = _MemObj_in; i < _MemObj_in + _MemObj_out; i++) {
		errorcode = clEnqueueReadBuffer(_MOCL_EXDAT.CommandQueue,
			_MOCL_EXDAT.MemObjects[i], CL_TRUE, NULL, _ARRAY_SIZE, &readgpubuffer.data[0], NULL, NULL, NULL);

		MOCLt_Matrix1DvalueToMatrix3(returnMatrx, count_out, readgpubuffer, _Matrix_xlength, _Matrix_ylength);
		count_out++;
	}
	MOCL_TOOL_FREEMATRIX1D(readgpubuffer);
	// kernel compute end.
	HMCL_totalTime = _TotalTime->Timing_Ended();

	if (errorcode != CL_SUCCESS) {
		ClogOut << ClogOut.SET(CLOG_ERROR) <<
			"MOPECL Beta: reading result buffer. code:" << errorcode << CRLOGEND;
		
		MOCL_TOOL_FREEMATRIX3D(returnMatrx);
	}
	return returnMatrx;
}

void MHIGH_OPENCL::HMCL_FreePacker() {
	MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
}