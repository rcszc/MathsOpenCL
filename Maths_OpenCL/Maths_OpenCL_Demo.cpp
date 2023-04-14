// opencl_maths.
// m_opencl rcsz.

#include <iostream>
#include <Windows.h>

#include "m_opencl/mcore_opencl.h"
#include "m_opencl/maths_opencl_tool.h"
#include "framelog/crimson_logframe.h"

using namespace std;

#define TEST_MATRIX_1D_ENTRY     10240
#define TEST_MATRIX_HIGH2D_ENTRY 2048

void Test_high_matrix() {

	// Demo 将图片泛光处理 bloom

    // 初始化一个 n * 2D 矩阵 [3D]
	mocl_Matrix3D TestRGBmat = MOCL_TOOL_NEWMATRIX2D(3, TEST_MATRIX_HIGH2D_ENTRY, TEST_MATRIX_HIGH2D_ENTRY);
	mocl_Matrix3D TestRGBmatOut = {};

	// computing model [Beta]
	MHIGH_OPENCL* TestHighOCL = new MHIGH_OPENCL[1];

	TestHighOCL->HMCL_InitMopencl("Demo_Bloom.cl", "DemoKernelBloom");         // 初始化OpenCL + 加载.cl核文件核函数
	TestHighOCL->HMCL_SetWorkGroupSize(16, 16);                                // 设置GPU工作组大小
	TestHighOCL->HMCL_ResizeComputeIO(3, 3, pow(TEST_MATRIX_HIGH2D_ENTRY, 2)); // 设置读写矩阵数量 + data.length

	TestHighOCL->HMCL_Matrix2Dbeta(TestRGBmat);                                // 输入 n * 2D 矩阵数据计算
	TestRGBmatOut = TestHighOCL->HMCL_ReadOutMatrix2D();                       // 写回 n * 2D 矩阵

	TestHighOCL->HMCL_FreePacker();                                            // 清理opencl各种对象 [packer]

	// "HMCL_totalTime" 计算时间 ( load + cpmpute + store )

	cout << "M_High_GPU.CALC: " << TestHighOCL->HMCL_totalTime << "ms" << endl; 
	cout << "Float.TensorData: " << TestRGBmatOut.mat_xlength << " " << TestRGBmatOut.mat_ylength << " " << TestRGBmatOut.mat_zlength << endl;
	cout << "Time / DataByte: " << TestRGBmatOut.mat_xlength * TestRGBmatOut.mat_ylength * TestRGBmatOut.mat_zlength * 4 / TestHighOCL->HMCL_totalTime;
	cout << " byte / ms" << endl;

	MOCL_TOOL_FREEMATRIX3D(TestRGBmatOut); // 释放矩阵内存
	delete[] TestHighOCL;
}

int main() {
	// 打印平台系统信息.
	MOCL_FUNC_SYSTEM();

	// High Demo.
	Test_high_matrix();
	
	// Easy Demo.

	COUNT_TIME timer; // 计时类 微秒级
	
	// 创建一个 n * 1D 矩阵 [2D]
	mocl_Matrix2D TestMatrix1 = MOCL_TOOL_NEWMATRIX1D(2, TEST_MATRIX_1D_ENTRY);
	mocl_Matrix1D TestOutMat1 = {};
	mocl_Matrix1D TestOutMat1CPU = {};

	int a[2] = { 0,100 };
	for (int _count = 0; _count < 2; _count++)
		for (int i = 0; i < TEST_MATRIX_1D_ENTRY; i++)
			TestMatrix1.data[_count][i] = float(a[_count] + i);


	timer.Timing_Start(); // 启动计时

	// CPU 计算
	TestOutMat1CPU.data = new float[TEST_MATRIX_1D_ENTRY];
	for (int i = 0; i < TEST_MATRIX_1D_ENTRY; i++)
		TestOutMat1CPU.data[i] = TestMatrix1.data[0][i] + TestMatrix1.data[1][i];

	// 计时结束
	cout << "CPU.CALC time: " << timer.Timing_Ended() << "ms" << endl;

	delete[] TestOutMat1CPU.data;

	// Test m_opencl load 1d
	MEASY_OPENCL* Test = new MEASY_OPENCL[1];

	Test->EMCL_InitMopencl("Demo_EasyMatrix1d.cl", "Test_Kernel"); // 初始化OpenCL + 加载.cl核文件核函数
	Test->EMCL_SetWorkGroupSize(10, WORKGROUP_1DFILL);             // 设置GPU工作组大小
	Test->EMCL_Matrix1Dahpla(TestMatrix1);                         // 输入 n * 1D 矩阵数据计算

	TestOutMat1 = Test->EMCL_ReadOutMatrix1D();                    // 写回矩阵 + 清理opencl各种对象 [packer]

	cout << "GPU.CALC time: " << Test->EMCL_totalTime << "ms" << endl;

	MOCL_TOOL_FREEMATRIX1D(TestOutMat1); // 释放矩阵内存
	delete[] Test;
	
	return NULL;
}