// load_opencl.
// libray: OpenCL[AMD]
// rcsz. [debug x64]

#ifndef _LOAD_OPENCL_H
#define _LOAD_OPENCL_H
#include <atlstr.h>
#include <CL/cl.h>
#include "maths_opencl_tool.h"
#include "../frameworklog/crimson_logframe.h"

#define _CVT_SIZET        size_t
#define _FLOAT_NUMSIZE(n) n * (size_t)4 // sizeof(float)
#define _FLOAT_SIZENUM(n) n / (size_t)4

#define _ERROR_CONTEXT      0xC0EE00
#define _ERROR_COMMANDQUEUE 0xC0EE01
#define _ERROR_PROGRAM      0xC0EE02
#define _ERROR_KERNEL       0xC0EE03

#define WORKGROUP_1DFILL    1 // matrix1D Work_Group_y_file

#define MCLASS_STATE_CMEMOBJ 0x1E0001 // Create mem_objects error
#define MCLASS_STATE_CKERNEL 0x1E0002 // Create kernel error

struct mocl_CLpacket {
	cl_device_id Device;           // OCL设备
	cl_context Context;            // OCL上下文
	cl_command_queue CommandQueue; // OCL命令队列
	cl_program Program;            // OCL程序
	cl_kernel Kernel;              // OCL核函数

	cl_mem* MemObjects;            // OCL内存对象
	mocl_size MemObjects_num;
};

mocl_bit _mocl_cleanpacket(
	cl_context context, cl_command_queue commandQueue, cl_program program, cl_kernel kernel,
	cl_mem* memObjects, mocl_size mObjsize
);

mocl_i32 MOCL_GetSystemInfo();
mocl_bit MOCL_ClearCLpacket(mocl_CLpacket packet);

#define MOCL_FUNC_CLEARPK MOCL_ClearCLpacket
#define MOCL_PRINT_SYSTEM MOCL_GetSystemInfo

// maths_opencl_core [OpenCL API] [Easy]
// version 1.2.5
class _MLOAD_OPENCL_CORE {
protected:
	mocl_size _ARRAY_SIZE = NULL;     // length * sizeof(float), 1d = xlen, 2d = xlen * ylen
	mocl_size _Matrix_xlength = NULL; // out_matrix x [1D]
	mocl_size _Matrix_ylength = NULL; // out_matrix y [2D]

	virtual mocl_charptr     _mocl_ReadKernelSourceFile(mocl_ccharptr filename, mocl_sizeptr length);
	virtual cl_context       _mocl_CreateContext(cl_device_id* device);
	virtual cl_command_queue _mocl_CreateCommandQueue(cl_context context, cl_device_id device);
	virtual cl_program       _mocl_CreateProgram(cl_context context, cl_device_id device, mocl_ccharptr filename);

	// mem_objects matrix * n
	virtual mocl_bit _mocl_CreateMemObjects(cl_context context, mocl_size memObject_num, cl_mem* memObjects, mocl_Matrix2D inDat); // 1D

	// mem_objects_num = MEMOBJ_in + MEMOBJ_out, ( clCreateBuufer + clEnqueueWriteBuffer )
	mocl_bit _mocl_CreateMEMOBJ_HIGH(cl_context context, mocl_size MEMOBJ_in, mocl_size MEMOBJ_out, cl_mem* memObjects);
	mocl_bit _mocl_WriteMEMOBJ_HIGH(cl_command_queue command, mocl_size MEMOBJ_in, cl_mem* memObjects, mocl_Matrix2D inDat);

	virtual mocl_bit _mocl_SetKernelFunction(cl_kernel kernel, mocl_size memObj_num, cl_mem* memObj);
};

// load opencl [1D/2D] [kernel function file]
// version 1.2.5
class MEASY_OPENCL :public _MLOAD_OPENCL_CORE {
protected:
	static mocl_size WORK_GROUP_SIZE[2];
	COUNT_TIME* _TotalTime = new COUNT_TIME[1]; // 计时

	mocl_CLpacket _MOCL_EXDAT = {};

public:
	~MEASY_OPENCL() {
		MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
		delete[] _TotalTime;
	};
	// Create_memobj => compute => readbuffer 用时[ms]
	mocl_fp64 EMCL_totalTime = NULL;

	// Context => Command_queue => Program => Kernel
	void EMCL_InitMopencl(mocl_ccharptr CLfile_path, mocl_ccharptr CLfunction);

	// set gpu work.group.size Mat1D => x, Mat2D => [x,y]
	void EMCL_SetWorkGroupSize(mocl_size x, mocl_size y);

	// => Memory_objects inD data = nD + 1D 
	mocl_bit EMCL_Matrix1Dahpla(mocl_Matrix2D inMatrix);
	mocl_bit EMCL_Matrix2Dahpla(mocl_Matrix3D inMatrix);

	// GPU mem_data => Main [mat1D]
	mocl_Matrix1D EMCL_ReadOutMatrix1D();
	mocl_Matrix2D EMCL_ReadOutMatrix2D();
};

// load opencl [image(2D)] [kernel function file]
// version 1.0.5
class MHIGH_OPENCL :public _MLOAD_OPENCL_CORE {
protected:
	static mocl_size WORK_GROUP_SIZE[2];
	COUNT_TIME* _TotalTime = new COUNT_TIME[1]; // 计时

	mocl_size _MemObj_in = NULL;
	mocl_size _MemObj_out = NULL;
	mocl_CLpacket _MOCL_EXDAT = {};

public:
	~MHIGH_OPENCL() {
		MOCL_FUNC_CLEARPK(_MOCL_EXDAT);
		delete[] _TotalTime;
	};
	// writebuffer => compute => readbuffer 用时[ms]
	mocl_fp64 HMCL_totalTime = NULL;
	mocl_ui32 statecode = NULL;

	void HMCL_FreePacker();
	// Context => Command_queue => Program => Kernel
	void HMCL_InitMopencl(mocl_ccharptr CLfile_path, mocl_ccharptr CLfunction);

	// set gpu work.group.size Mat1D => x, Mat2D => [x,y]
	void HMCL_SetWorkGroupSize(mocl_size x, mocl_size y);

	// => Create_Memory_objects 
	// 1D: data_length = x.len, 2D: data_length = x.len * y.len
	void HMCL_ResizeComputeIO(mocl_size In_matrix_num, mocl_size Out_matrix_num, mocl_size data_length);

	// => write data
	mocl_bit HMCL_Matrix2Dbeta(mocl_Matrix3D inMatrix);
	// GPU mem_data => main
	mocl_Matrix3D HMCL_ReadOutMatrix2D();
};

#endif