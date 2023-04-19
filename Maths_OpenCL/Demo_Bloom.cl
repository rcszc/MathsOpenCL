__kernel void DemoKernelBloom(global const float *MatR, global const float *MatG, global const float *MatB, 
                              global float *outR, global float *outG, global float *outB) {

    // length = i * conv_kernel_len

	int i = get_global_id(0);
    int j = get_global_id(1);

    int width = get_global_size(0);
    int height = get_global_size(1);

    int conv_kernel = 20;

    float sum_valueR = 0;
    float sum_valueG = 0;
    float sum_valueB = 0;

    // 模糊卷积 RGB matrix.
    // blur conv color R

    for(int x = i - conv_kernel / 2; x < i + conv_kernel; x++)
    {
        for(int y = j - conv_kernel / 2; y < j + conv_kernel; y++)
        {
            if ((x >= 0) && (x < height) && (y >= 0) && (y < width))
			    sum_valueR += MatR[x * width + y];
        }
    }
	outR[i * width + j] = sum_valueR / (conv_kernel * conv_kernel);

    // blur conv color G

    for(int x = i - conv_kernel / 2; x < i + conv_kernel; x++)
    {
        for(int y = j - conv_kernel / 2; y < j + conv_kernel; y++)
        {
            if ((x >= 0) && (x < height) && (y >= 0) && (y < width))
			    sum_valueG += MatG[x * width + y];
        }
    }
	outG[i * width + j] = sum_valueG / (conv_kernel * conv_kernel);

    // blur conv color B

    for(int x = i - conv_kernel / 2; x < i + conv_kernel; x++)
    {
        for(int y = j - conv_kernel / 2; y < j + conv_kernel; y++)
        {
            if ((x >= 0) && (x < height) && (y >= 0) && (y < width))
			    sum_valueB += MatB[x * width + y];
        }
    }
	outB[i * width + j] = sum_valueB / (conv_kernel * conv_kernel);

    // 与原图像混合 RGB martix.

    outR[i * width + j] += MatR[i * width + j];
    outG[i * width + j] += MatG[i * width + j];
    outB[i * width + j] += MatB[i * width + j];
}