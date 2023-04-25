// opencl_maths.
// m_opencl rcsz.

#include <iostream>
#include <Windows.h>

#include "m_opencl/mcore_opencl.h"
#include "m_opencl/maths_opencl_tool.h"
#include "m_opencl/mcore_ocl_PlatformDevice.h"
#include "framelog/crimson_logframe.h"

using namespace std;

#define TEST_MATRIX_1D_ENTRY     10240
#define TEST_MATRIX_HIGH2D_ENTRY 2048

// HIGH 模型为输入 N * 2D 矩阵, 输出 N * 2D 矩阵.
// resizeIO 后可连续读写计算. (场景:反复调用 CL_Kernel)
void Test_high_matrix() {

	// Demo: 图片泛光处理 [bloom].

    // 初始化一个 n * 2D 矩阵 [3D]
	mocl_Matrix3D TestRGBmat = MOCL_TOOL_NEWMATRIX2D(3, TEST_MATRIX_HIGH2D_ENTRY, TEST_MATRIX_HIGH2D_ENTRY);
	// 读取结果用矩阵.
	mocl_Matrix3D TestRGBmatOut = {};

	// computing model [Beta]
	MHIGH_OPENCL* TestHighOCL = new MHIGH_OPENCL[1];

	TestHighOCL->HMCL_InitMopencl("Demo_Bloom.cl", "DemoKernelBloom");                    // 初始化OpenCL + 加载.cl核文件核函数
	TestHighOCL->HMCL_SetWorkGroupSize(16, 16);                                           // 设置GPU工作组大小
	TestHighOCL->HMCL_ResizeComputeIO(3, 3, mocl_size(pow(TEST_MATRIX_HIGH2D_ENTRY, 2))); // 设置读写矩阵数量 + data.length

	TestHighOCL->HMCL_Matrix2Dbeta(TestRGBmat);                                           // 输入 n * 2D 矩阵数据计算 [3D]
	TestRGBmatOut = TestHighOCL->HMCL_ReadOutMatrix2D();                                  // 写回 n * 2D 矩阵        [3D]

	// [MHIGH_OPENCL] ==> void HMCL_SetPlatformDevice(mocl_ui32 NP, mocl_ui32 ND); // 设置平台和设备. [Test]

	// "HMCL_totalTime" 计算时间 ( load + cpmpute + store )

	CRLOG_CONS::ClogOut << CRLOG_CONS::ClogOut.SET(CLOG_INFO) << "M_High_GPU.CALC: " << (mocl_fp32)TestHighOCL->HMCL_totalTime << "ms" << CRLOGEND;
	CRLOG_CONS::ClogOut << CRLOG_CONS::ClogOut.SET(CLOG_INFO) << "ComputeSpeed: " << 
		mocl_fp32(TestRGBmatOut.mat_xlength * TestRGBmatOut.mat_ylength * TestRGBmatOut.mat_zlength * 4) / (mocl_fp32)TestHighOCL->HMCL_totalTime;

	CRLOG_CONS::ClogOut << " Bytes / ms" << CRLOGEND;

	MOCL_TOOL_FREEMATRIX3D(TestRGBmatOut); // Free 矩阵内存
	delete[] TestHighOCL;                  // Free MHIGH_OPENCL
}

// EASY 模型为输入 N * 1D/2D 矩阵, 输出 1 * 1D/2D矩阵.
// 适用场景为一次性调用 CL_Kernel.
void Test_easy_matrix() {

	// Demo: 1D矩阵加法.

	// 创建一个 n * 1D 矩阵 [2D]
	mocl_Matrix2D TestMatrix1 = MOCL_TOOL_NEWMATRIX1D(2, TEST_MATRIX_1D_ENTRY);
	// 读取结果用矩阵.
	mocl_Matrix1D TestOutMat1 = {};

	// Test m_opencl load 1d
	MEASY_OPENCL* Test = new MEASY_OPENCL[1];

	Test->EMCL_InitMopencl("Demo_EasyMatrix1d.cl", "Test_Kernel"); // 初始化OpenCL + 加载.cl核文件核函数
	Test->EMCL_SetWorkGroupSize(16, WORKGROUP_1DFILL);             // 设置GPU工作组大小
	Test->EMCL_Matrix1Dahpla(TestMatrix1);                         // 输入 n * 1D 矩阵数据计算

	TestOutMat1 = Test->EMCL_ReadOutMatrix1D();                    // 写回矩阵 + 清理opencl各种对象 [packer]

	CRLOG_CONS::ClogOut << CRLOG_CONS::ClogOut.SET(CLOG_PERF) << "GPU.CALC time: " << (mocl_fp32)Test->EMCL_totalTime << "ms" << CRLOGEND;

	MOCL_TOOL_FREEMATRIX1D(TestOutMat1); // Free 矩阵内存
	delete[] Test;                       // Free MEASY_OPENCL
}

int main() {
	// 打印平台系统信息.
	MOCL_PRINT_SYSTEM();

	// Model Demo.
	 Test_high_matrix();
	 Test_easy_matrix();
	{
		COUNT_TIME timer; // 计时类 微秒级.

		timer.Timing_Start(); // 启动计时.
		timer.Timing_Ended(); // 结束计时 return Time[double].
	}
	{
		MLOAD_OPENCL_DEVICE GetNumber; // 获取平台设备数量.

		GetNumber.GetPlatformNumber();    // 符合OCL平台数量.
		GetNumber.GetDevicemNumber(NULL); // 指定平台GPU设备数量.
	}
	
	return NULL;
}