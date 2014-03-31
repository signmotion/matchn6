/**
* (!) ��� ����������� �� ����� ����� ������ ���� ����������� ��
* ����������� C++.
*
* # ��������� ��������� ���, ����� �� ����������� ��� � OpenCL / C++.
*/

// ����������� ���� ��� OpenCL, ����� ������������ ��������� �� C++
// �� ��������� if-def'���.

typedef float    real_t;
typedef float2   real2_t;
//typedef float3   real3_t;
typedef float4   real4_t;

typedef bool     cl_bool;
typedef char     cl_char;
typedef uchar    cl_uchar;
typedef int      cl_int;
typedef uint     cl_uint;
typedef long     cl_long;
typedef ulong    cl_ulong;
typedef short    cl_short;
typedef ushort   cl_ushort;
typedef real_t   cl_real;
typedef real2_t  cl_real2;
//typedef real3_t  cl_real3;
typedef real4_t  cl_real4;

#define nullptr  0


// #! ����� ������� �����������. ����� ��� ���������� ���� ������� ��������
//    ������������� ������... ��� ���������� ������.
// #! ������ Unicode � ���� OpenCL (��������� �����, ��������, ����� �����������
//    ������� �� ���������) ��� ����, ���������� � Unicode, ����� ��������
//    � ������������� �������.
// # ������� ������� ��� ������� ������������� ������ > https://sourceforge.net/projects/clcc
