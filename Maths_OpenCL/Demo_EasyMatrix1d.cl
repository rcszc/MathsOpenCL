__kernel void Test_Kernel(global const float *Test_vecA, global const float *Test_vecB, global float *output) {
	int count = get_global_id(0);
	output[count] = Test_vecA[count] + Test_vecB[count];
}