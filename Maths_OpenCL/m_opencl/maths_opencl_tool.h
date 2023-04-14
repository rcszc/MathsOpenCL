// maths_opencl.
// rcsz 2023.
// version 1.5.0

#ifndef _MATHS_OPENCL_TOOL_H
#define _MATHS_OPENCL_TOOL_H
#include <atlstr.h>

#define MOPENCL_SYSTEM_GETTIME64 ::GetTickCount64()

// Maths def. [MSVC170]

typedef __int32          mocl_i32;
typedef unsigned __int32 mocl_ui32;
typedef float            mocl_fp32;
typedef double           mocl_fp64;

typedef bool               mocl_bit;
typedef char               mocl_char;
typedef unsigned char      mocl_byte;
typedef unsigned long long mocl_size;

typedef float*   mocl_mat1ptr;
typedef float**  mocl_mat2ptr;
typedef float*** mocl_mat3ptr;

typedef char*               mocl_charptr;
typedef const char*         mocl_ccharptr;
typedef unsigned long long* mocl_sizeptr;

// struct Matrix. 

struct mocl_Matrix1D {
	mocl_mat1ptr data;
	mocl_size mat_xlength;
};

struct mocl_Matrix2D {
	mocl_mat2ptr data;
	mocl_size mat_xlength;
	mocl_size mat_ylength;
};

struct mocl_Matrix3D {
	mocl_mat3ptr data;
	mocl_size mat_xlength;
	mocl_size mat_ylength;
	mocl_size mat_zlength;
};

struct mocl_MatrixImgRGB { // 图像RGB通道矩阵[2D]
	mocl_mat2ptr dataR;
	mocl_mat2ptr dataG;
	mocl_mat2ptr dataB;
	mocl_size mat_width;
	mocl_size mat_height;
};

struct mocl_MatrixParticle { // 粒子矩阵[1D] (Test)
	mocl_mat1ptr data_posx;
	mocl_mat1ptr data_posy;

	mocl_mat1ptr data_vectorx;
	mocl_mat1ptr data_vectory;

	mocl_mat1ptr data_cr;
	mocl_mat1ptr data_cb;
	mocl_mat1ptr data_cg;

	mocl_mat1ptr data_colorvector;
	mocl_mat1ptr data_life;

	mocl_size particle_length;
};

// mocl timer ms.
class COUNT_TIME {
protected:
	LARGE_INTEGER litmp = {};
	LONGLONG _tvalue[2] = {};
	double returntime = 0.0, frequency = 0.0, dfm = 0.0;
public:
	void Timing_Start();
	double Timing_Ended();
};

// Maths function.

mocl_size MOCL_Matrix1Free(mocl_Matrix1D matrix);
mocl_size MOCL_Matrix2Free(mocl_Matrix2D matrix);
mocl_size MOCL_Matrix3Free(mocl_Matrix3D matrix);
mocl_size MOCL_MatrixImgFree(mocl_MatrixImgRGB matrix);

mocl_Matrix2D     MOCL_Matrix3toMatrix2(mocl_Matrix3D matrix);
mocl_Matrix1D     MOCL_Matrix2toMatrix1(mocl_Matrix2D matrix);
mocl_Matrix2D     MOCL_Matrix1toMatrix2(mocl_Matrix1D matrix, mocl_size xlen, mocl_size ylen);
mocl_Matrix3D     MOCL_MatrixImgtoMatrix3(mocl_MatrixImgRGB matrix);
mocl_MatrixImgRGB MOCL_Matrix3toMatrixImg(mocl_Matrix3D matrix);

mocl_Matrix2D     MOCL_CreateMatrix1(mocl_size Matrix_Num, mocl_size xlength);
mocl_Matrix3D     MOCL_CreateMatrix2(mocl_size Matrix_Num, mocl_size xlength, mocl_size ylength);
mocl_MatrixImgRGB MOCL_CreateMatrixImg(mocl_size width, mocl_size height);

// 矩阵内存释放.
#define MOCL_TOOL_FREEMATRIX1D  MOCL_Matrix1Free    // Free matrix1D ( delete[] data, length.set = NULL ) return free.size
#define MOCL_TOOL_FREEMATRIX2D  MOCL_Matrix2Free    // Free matrix2D ( delete[] data, length.set = NULL ) return free.size
#define MOCL_TOOL_FREEMATRIX3D  MOCL_Matrix3Free    // Free matrix3D ( delete[] data, length.set = NULL ) return free.size
#define MOCL_TOOL_FREEMATRIXIMG MOCL_MatrixImgFree  // Free matrixImg ( delete[] data R G B, length.set = NULL ) return free.size

// 矩阵数据转换.
#define MOCL_TOOL_3DTO2D  MOCL_Matrix3toMatrix2  // ( in_matrix_3d ) return matrix_2d
#define MOCL_TOOL_2DTO1D  MOCL_Matrix2toMatrix1  // ( in_matrix_2d ) return matrix_1d
#define MOCL_TOOL_1DTO2D  MOCL_Matrix1toMatrix2  // ( in_matrix_1d, matrix_2d_x_length, matrix_2d_y_length ) return matrix_2d

#define MOCL_TOOL_IMGTO3D MOCL_MatrixImgtoMatrix3  // ( in_matrix_ImgRGB ) return matrix_3d
#define MOCL_TOOL_3DTOIMG MOCL_Matrix3toMatrixImg  // ( in_matrix_3d ) return in_matrix_ImgRGB

// 矩阵内存创建.
#define MOCL_TOOL_NEWMATRIX1D  MOCL_CreateMatrix1    // ( create_matrix_number, matrix_1d_x_length ) return matrix_2d
#define MOCL_TOOL_NEWMATRIX2D  MOCL_CreateMatrix2    // ( create_matrix_number, matrix_2d_x_length, matrix_2d_y_length ) return matrix_3d
#define MOCL_TOOL_NEWMATRIXIMG MOCL_CreateMatrixImg  // ( matrix_width, matrix_height ) return matrix_ImgRGB

// test func.
void MOCLt_Matrix1DvalueToMatrix3(mocl_Matrix3D mat, mocl_size matcount, mocl_Matrix1D matin, mocl_size x, mocl_size y);

#endif