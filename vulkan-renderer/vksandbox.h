﻿#pragma once
#ifndef VKSANDBOX_H
#define VKSANDBOX_H
#include"util.h"


	#define SEARCHPATH "C:/Projects/vksandbox/vksandbox/";
//	#define SEARCHPATH "D:/c2/vksandbox/vksandbox/";


extern int w, h, mx, my, mouse_bypass;
extern char keyboard[256], timer;
extern wchar_t g_wbuf[G_BUF_SIZE];
extern ArrayHandle exedir;

typedef enum IOKeyEnum
{
#if defined _MSC_VER || defined _WINDOWS || defined _WIN32
#	define	IOKEY(LinVAL, VAL, LABEL)	KEY_##LABEL=VAL,
#elif defined __linux__
#	define	IOKEY(VAL, WinVAL, LABEL)	KEY_##LABEL=VAL,
#endif
	
//IO value, win32 value, label
IOKEY(0x00, 0x00, UNKNOWN)


//direct map keys
IOKEY(0x01, 0x01, LBUTTON)		//inserted
IOKEY(0x02, 0x04, MBUTTON)		//inserted
IOKEY(0x03, 0x02, RBUTTON)		//inserted

IOKEY(0x08, 0x08, BKSP)
IOKEY(0x09, 0x09, TAB)

IOKEY(0x0D, 0x0D, ENTER)

IOKEY(0x1B, 0x1B, ESC)

IOKEY(0x20, 0x20, SPACE)
IOKEY(0x30, 0x30, 0)
IOKEY(0x31, 0x31, 1)
IOKEY(0x32, 0x32, 2)
IOKEY(0x33, 0x33, 3)
IOKEY(0x34, 0x34, 4)
IOKEY(0x35, 0x35, 5)
IOKEY(0x36, 0x36, 6)
IOKEY(0x37, 0x37, 7)
IOKEY(0x38, 0x38, 8)
IOKEY(0x39, 0x39, 9)
IOKEY(0x41, 0x41, A)
IOKEY(0x42, 0x42, B)
IOKEY(0x43, 0x43, C)
IOKEY(0x44, 0x44, D)
IOKEY(0x45, 0x45, E)
IOKEY(0x46, 0x46, F)
IOKEY(0x47, 0x47, G)
IOKEY(0x48, 0x48, H)
IOKEY(0x49, 0x49, I)
IOKEY(0x4A, 0x4A, J)
IOKEY(0x4B, 0x4B, K)
IOKEY(0x4C, 0x4C, L)
IOKEY(0x4D, 0x4D, M)
IOKEY(0x4E, 0x4E, N)
IOKEY(0x4F, 0x4F, O)
IOKEY(0x50, 0x50, P)
IOKEY(0x51, 0x51, Q)
IOKEY(0x52, 0x52, R)
IOKEY(0x53, 0x53, S)
IOKEY(0x54, 0x54, T)
IOKEY(0x55, 0x55, U)
IOKEY(0x56, 0x56, V)
IOKEY(0x57, 0x57, W)
IOKEY(0x58, 0x58, X)
IOKEY(0x59, 0x59, Y)
IOKEY(0x5A, 0x5A, Z)


//other keys
IOKEY(0x13, 0x13, PAUSE)
IOKEY(0x14, 0x91, SCROLLLOCK)
IOKEY(0x15, 0x2C, PRINTSCR)

IOKEY(0x27, 0xDE, QUOTE)		//inserted '\'' with '\"' 0x22
IOKEY(0x2B, 0xBB, PLUS)			//inserted '+' with '=' 0x3D
IOKEY(0x2C, 0xBC, COMMA)		//inserted ',' with '<' 0x3C
IOKEY(0x2D, 0xBD, MINUS)		//inserted '-' with '_' 0x5F
IOKEY(0x2E, 0xBE, PERIOD)		//inserted '.' with '>' 0x3E
IOKEY(0x2F, 0xBF, SLASH)		//inserted '/' with '?' 0x3F

IOKEY(0x3B, 0xBA, SEMICOLON)	//inserted ';' with ':' 0x3A

IOKEY(0x5B, 0xDB, LBRACKET)		//inserted '[' with '{' 0x7B
IOKEY(0x5C, 0xDC, BACKSLASH)	//inserted '\\' with '|' 0x7C
IOKEY(0x5D, 0xDD, RBRACKET)		//inserted ']' with '}' 0x7D
IOKEY(0x60, 0xC0, GRAVEACCENT)	//inserted '`' with '~' 0x7E

IOKEY(0x7F, 0x2E, DEL)

IOKEY(0x84, 0x10, SHIFT)		//inserted
IOKEY(0x85, 0x11, CTRL)			//inserted
IOKEY(0x86, 0x12, ALT)			//inserted
IOKEY(0x87, 0x00, START)		//inserted

IOKEY(0xA0, 0x60, NP_0)
IOKEY(0xA1, 0x61, NP_1)
IOKEY(0xA2, 0x62, NP_2)
IOKEY(0xA3, 0x63, NP_3)
IOKEY(0xA4, 0x64, NP_4)
IOKEY(0xA5, 0x65, NP_5)
IOKEY(0xA6, 0x66, NP_6)
IOKEY(0xA7, 0x67, NP_7)
IOKEY(0xA8, 0x68, NP_8)
IOKEY(0xA9, 0x69, NP_9)
IOKEY(0xAA, 0x6A, NP_MUL)
IOKEY(0xAB, 0x6B, NP_PLUS)
IOKEY(0xAC, 0x6D, NP_MINUS)
IOKEY(0xAD, 0x6E, NP_PERIOD)
IOKEY(0xAE, 0x6F, NP_DIV)

IOKEY(0xBE, 0x70, F1)
IOKEY(0xBF, 0x71, F2)
IOKEY(0xC0, 0x72, F3)
IOKEY(0xC1, 0x73, F4)
IOKEY(0xC2, 0x74, F5)
IOKEY(0xC3, 0x75, F6)
IOKEY(0xC4, 0x76, F7)
IOKEY(0xC5, 0x77, F8)
IOKEY(0xC6, 0x78, F9)
IOKEY(0xC7, 0x79, F10)
IOKEY(0xC8, 0x7A, F11)
IOKEY(0xC9, 0x7B, F12)

IOKEY(0xD0, 0x24, HOME)
IOKEY(0xD1, 0x25, LEFT)
IOKEY(0xD2, 0x26, UP)
IOKEY(0xD3, 0x27, RIGHT)
IOKEY(0xD4, 0x28, DOWN)
IOKEY(0xD5, 0x21, PGUP)
IOKEY(0xD6, 0x22, PGDN)
IOKEY(0xD7, 0x23, END)

IOKEY(0xE5, 0x14, CAPSLOCK)

IOKEY(0xE3, 0x2D, INSERT)
IOKEY(0xEB, 0x03, BREAK)

IOKEY(0xFF, 0x90, NUMLOCK)

#undef	IOKEY
} IOKey;


//callbacks - implement these in your application:
int io_init(int argc, char **argv);//return false to abort
void io_resize();
int io_mousemove();//return true to redraw
int io_mousewheel(int forward);
int io_keydn(IOKey key, char c);
int io_keyup(IOKey key, char c);
void io_timer();
void io_render();
int io_quit_request();//return 1 to exit
void io_cleanup();//cleanup


void   console_start();
void   console_end();
void   console_buffer_size(short x, short y);
void   console_log(const char *format, ...);
void   console_pause();
int    console_scan(char *buf, int len);
int    console_scan_int();
double console_scan_float();
	
int sys_check(const char *file, int line, const char *info);
#define SYS_ASSERT(SUCCESS)	((void)((SUCCESS)!=0||sys_check(file, __LINE__, 0)))

typedef enum MessageBoxTypeEnum
{
	MBOX_OK,
	MBOX_OKCANCEL,
	MBOX_YESNOCANCEL,
} MessageBoxType;
int messagebox(MessageBoxType type, const char *title, const char *format, ...);//returns index of pressed button

typedef struct FilterStruct
{
	const char *comment, *ext;
} Filter;
ArrayHandle dialog_open_folder();
ArrayHandle	dialog_open_file(Filter *filters, int nfilters, int multiple);
const char*	dialog_save_file(Filter *filters, int nfilters, const char *initialname);

void get_window_title(char *buf, int len);
void set_window_title(const char *format, ...);

int copy_to_clipboard(const char *a, int size);
ArrayHandle paste_from_clipboard(int loud);

#define GET_KEY_STATE(KEY)	(keyboard[KEY]=(GetAsyncKeyState(KEY)>>15)!=0)

void timer_start(int ms);
void timer_stop();

void set_mouse(int x, int y);
void get_mouse(int *px, int *py);
void show_mouse(int show);

void* get_hwnd();
void* get_hinstance();


//Macros for 3D		DO NOT NEST THE MACROS LIKE f(g(x), h(y))
#if 1
#define		vec2_copy(DST, SRC)		(DST)[0]=(SRC)[0], (DST)[1]=(SRC)[1]
#define		vec2_add(DST, A, B)		(DST)[0]=(A)[0]+(B)[0], (DST)[1]=(A)[1]+(B)[1]
#define		vec2_sub(DST, A, B)		(DST)[0]=(A)[0]-(B)[0], (DST)[1]=(A)[1]-(B)[1]
#define		vec2_add1(DST, V, S)	(DST)[0]=(V)[0]+(S), (DST)[1]=(V)[1]+(S)
#define		vec2_sub1(DST, V, S)	(DST)[0]=(V)[0]-(S), (DST)[1]=(V)[1]-(S)
#define		vec2_mul1(DST, V, S)	(DST)[0]=(V)[0]*(S), (DST)[1]=(V)[1]*(S)
#define		vec2_div1(DST, V, S)	(DST)[0]=(V)[0]/(S), (DST)[1]=(V)[1]/(S)
#define		vec2_dot(A, B)			((A)[0]*(B)[0]+(A)[1]*(B)[1])
#define		vec2_cross(DST, A, B)	((A)[0]*(B)[1]-(A)[1]*(B)[0])
#define		vec2_abs(A)				sqrtf(vec2_dot(A, A))
#define		vec2_abs2(A)			vec2_dot(A, A)
#define		vec2_arg(A)				atan((A)[1]/(A)[0])
#define		vec2_arg2(A)			atan2((A)[1], (A)[0])
#define		vec2_eq(A, B)			((A)[0]==(B)[0]&&(A)[1]==(B)[1])
#define		vec2_ne(A, B)			((A)[0]!=(B)[0]||(A)[1]!=(B)[1])
#define		vec2_neg(DST, A)		(DST)[0]=-(A)[0], (DST)[1]=-(A)[1]

#define		mat2_mul_vec2(DST, M2, V2)	(DST)[0]=(M2)[0]*(V2)[0]+(M2)[1]*(V2)[1], (DST)[1]=(M2)[2]*(V2)[0]+(M2)[3]*(V2)[1]

#define		vec3_copy(DST, SRC)		(DST)[0]=(SRC)[0], (DST)[1]=(SRC)[1], (DST)[2]=(SRC)[2]
#define		vec3_set1(V3, GAIN)		(V3)[0]=(V3)[1]=(V3)[2]=GAIN
#define		vec3_setp(V3, POINTER)	(V3)[0]=(POINTER)[0], (V3)[1]=(POINTER)[1], (V3)[2]=(POINTER)[2]
#define		vec3_seti(V3, X, Y, Z)	(V3)[0]=X, (V3)[1]=Y, (V3)[2]=Z
#define		vec3_add(DST, A, B)		(DST)[0]=(A)[0]+(B)[0], (DST)[1]=(A)[1]+(B)[1], (DST)[2]=(A)[2]+(B)[2]
#define		vec3_sub(DST, A, B)		(DST)[0]=(A)[0]-(B)[0], (DST)[1]=(A)[1]-(B)[1], (DST)[2]=(A)[2]-(B)[2]
#define		vec3_add1(DST, V, S)	(DST)[0]=(V)[0]+(S), (DST)[1]=(V)[1]+(S), (DST)[2]=(V)[2]+(S)
#define		vec3_sub1(DST, V, S)	(DST)[0]=(V)[0]-(S), (DST)[1]=(V)[1]-(S), (DST)[2]=(V)[2]-(S)
#define		vec3_mul1(DST, V, S)	(DST)[0]=(V)[0]*(S), (DST)[1]=(V)[1]*(S), (DST)[2]=(V)[2]*(S)
#define		vec3_div1(DST, V, S)	(DST)[0]=(V)[0]/(S), (DST)[1]=(V)[1]/(S), (DST)[2]=(V)[2]/(S)
#define		vec3_dot(A, B)			((A)[0]*(B)[0]+(A)[1]*(B)[1]+(A)[2]*(B)[2])
#define		vec3_cross(DST, A, B)\
	(DST)[0]=(A)[1]*(B)[2]-(A)[2]*(B)[1],\
	(DST)[1]=(A)[2]*(B)[0]-(A)[0]*(B)[2],\
	(DST)[2]=(A)[0]*(B)[1]-(A)[1]*(B)[0]
#define		vec3_triple_product(DST, A, B, C, TEMP_F1, TEMP_F2)\
	TEMP_F1=vec3_dot(A, C), TEMP_F2=vec3_dot(B, C), (DST)[0]=TEMP_F1*(B)[0]-TEMP_F2*(C)[0], (DST)[1]=TEMP_F1*(B)[1]-TEMP_F2*(C)[1], (DST)[2]=TEMP_F1*(B)[2]-TEMP_F2*(C)[2]
#define		vec3_abs(A)				sqrtf(vec3_dot(A, A))
#define		vec3_abs2(A)			vec3_dot(A, A)
#define		vec3_theta(A)			atan((A)[2]/sqrtf((A)[0]*(A)[0]+(A)[1]*(A)[1]))
#define		vec3_phi(A)				atan((A)[1]/(A)[0])
#define		vec3_phi2(A)			atan2((A)[1], (A)[0])
#define		vec3_isnan(A)			((A)[0]!=(A)[0]||(A)[1]!=(A)[1]||(A)[2]!=(A)[2])
#define		vec3_isnan_or_inf(A)	(vec3_isnan(A)||fabsf((A)[0])==infinity||fabsf((A)[1])==infinity||fabsf((A)[2])==infinity)
#define		vec3_eq(A, B)			((A)[0]==(B)[0]&&(A)[1]==(B)[1]&&(A)[2]==(B)[2])
#define		vec3_ne(A, B)			((A)[0]!=(B)[0]||(A)[1]!=(B)[1]||(A)[2]!=(B)[2])
#define		vec3_neg(DST, A)		(DST)[0]=-(A)[0], (DST)[1]=-(A)[1], (DST)[2]=-(A)[2]
#define		vec3_normalize(DST, A, TEMP_F)		TEMP_F=1/vec3_abs(A), vec3_div1(DST, A, TEMP_F)
#define		vec3_mix(DST, A, B, X)\
	(DST)[0]=(A)[0]+((B)[0]-(A)[0])*(X),\
	(DST)[1]=(A)[1]+((B)[1]-(A)[1])*(X),\
	(DST)[2]=(A)[2]+((B)[2]-(A)[2])*(X)

//column-major
#define		mat3_diag(MAT3, GAIN)	memset(MAT3, 0, 9*sizeof(float)), (MAT)[0]=(MAT)[1]=(MAT)[2]=GAIN
//#define	mat3_diag(MAT3, GAIN)	(MAT3)[0]=GAIN, (MAT3)[1]=0, (MAT3)[2]=0, (MAT3)[3]=0, (MAT3)[4]=GAIN, (MAT3)[5]=0, (MAT3)[6]=0, (MAT3)[7]=0, (MAT3)[8]=GAIN

#define		vec4_copy(DST, SRC)		(DST)[0]=(SRC)[0], (DST)[1]=(SRC)[1], (DST)[2]=(SRC)[2], (DST)[3]=(SRC)[3]
#define		vec4_dot(DST, A, B, TEMP_V1, TEMP_V2)		TEMP_V1=_mm_loadu_ps(A), TEMP_V1=_mm_mul_ps(TEMP_V1, _mm_loadu_ps(B)), TEMP_V1=_mm_hadd_ps(TEMP_V1, TEMP_V1), TEMP_V1=_mm_hadd_ps(TEMP_V1, TEMP_V1), _mm_store_ss(DST, TEMP_V1)
#define		vec4_add(DST, A, B)		(DST)[0]=(A)[0]+(B)[0], (DST)[1]=(A)[1]+(B)[1], (DST)[2]=(A)[2]+(B)[2], (DST)[3]=(A)[3]+(B)[3]
#define		vec4_sub(DST, A, B)		(DST)[0]=(A)[0]-(B)[0], (DST)[1]=(A)[1]-(B)[1], (DST)[2]=(A)[2]-(B)[2], (DST)[3]=(A)[3]-(B)[3]
#define		vec4_mul1(DST, V, S)	(DST)[0]=(V)[0]*(S), (DST)[1]=(V)[1]*(S), (DST)[2]=(V)[2]*(S), (DST)[3]=(V)[3]*(S)
//#define		vec4_add(DST, A, B)		{_mm_storeu_ps(DST, _mm_add_ps(_mm_loadu_ps(A), _mm_loadu_ps(B)));}
//#define		vec4_sub(DST, A, B)		{_mm_storeu_ps(DST, _mm_sub_ps(_mm_loadu_ps(A), _mm_loadu_ps(B)));}
//#define		vec4_mul1(DST, A, S)	{_mm_storeu_ps(DST, _mm_sub_ps(_mm_loadu_ps(A), _mm_set1_ps(S)));}

//column-major
#define		mat4_copy(DST, SRC)		memcpy(DST, SRC, 16*sizeof(float));
#define		mat4_identity(M4, GAIN)	memset(M4, 0, 16*sizeof(float)), (M4)[0]=(M4)[5]=(M4)[10]=(M4)[15]=GAIN
#define		mat4_data(M4, X, Y)		(M4)[(X)<<2|(Y)]
#define		mat4_mat3(DST, M4)\
	(DST)[0]=(M4)[0], (DST)[1]=(M4)[1], (DST)[2]=(M4)[2],\
	(DST)[3]=(M4)[4], (DST)[4]=(M4)[5], (DST)[5]=(M4)[6],\
	(DST)[6]=(M4)[8], (DST)[7]=(M4)[9], (DST)[8]=(M4)[10],
#define		mat4_transpose(DST, M4, TEMP_8V)\
	(TEMP_8V)[0]=_mm_loadu_ps(M4),\
	(TEMP_8V)[1]=_mm_loadu_ps((M4)+4),\
	(TEMP_8V)[2]=_mm_loadu_ps((M4)+8),\
	(TEMP_8V)[3]=_mm_loadu_ps((M4)+12),\
	(TEMP_8V)[4]=_mm_unpacklo_ps((TEMP_8V)[0], (TEMP_8V)[1]),\
	(TEMP_8V)[5]=_mm_unpacklo_ps((TEMP_8V)[2], (TEMP_8V)[3]),\
	(TEMP_8V)[6]=_mm_unpackhi_ps((TEMP_8V)[0], (TEMP_8V)[1]),\
	(TEMP_8V)[7]=_mm_unpackhi_ps((TEMP_8V)[2], (TEMP_8V)[3]),\
	_mm_storeu_ps(DST, _mm_movelh_ps((TEMP_8V)[4], (TEMP_8V)[5])),\
	_mm_storeu_ps((DST)+4, _mm_movehl_ps((TEMP_8V)[5], (TEMP_8V)[4])),\
	_mm_storeu_ps((DST)+8, _mm_movelh_ps((TEMP_8V)[6], (TEMP_8V)[7])),\
	_mm_storeu_ps((DST)+12, _mm_movehl_ps((TEMP_8V)[7], (TEMP_8V)[6]))
#define		mat4_mul_vec4(DST, M4, V4, TEMP_V)\
	TEMP_V=_mm_mul_ps(_mm_loadu_ps(M4), _mm_set1_ps((V4)[0])),\
	TEMP_V=_mm_add_ps(TEMP_V, _mm_mul_ps(_mm_loadu_ps(M4+4), _mm_set1_ps((V4)[1]))),\
	TEMP_V=_mm_add_ps(TEMP_V, _mm_mul_ps(_mm_loadu_ps(M4+8), _mm_set1_ps((V4)[2]))),\
	TEMP_V=_mm_add_ps(TEMP_V, _mm_mul_ps(_mm_loadu_ps(M4+12), _mm_set1_ps((V4)[3]))),\
	_mm_storeu_ps(DST, TEMP_V)
#define		mat4_mul_mat4(DST_NEW, M4A, M4B, TEMP_V)\
	mat4_mul_vec4(DST_NEW,		M4A, M4B,		TEMP_V),\
	mat4_mul_vec4(DST_NEW+4,	M4A, M4B+4,		TEMP_V),\
	mat4_mul_vec4(DST_NEW+8,	M4A, M4B+8,		TEMP_V),\
	mat4_mul_vec4(DST_NEW+12,	M4A, M4B+12,	TEMP_V)
#define		mat4_translate(M4, V3, TEMP_V)\
	TEMP_V=_mm_mul_ps(_mm_loadu_ps(M4), _mm_set1_ps((V3)[0])),\
	TEMP_V=_mm_add_ps(TEMP_V, _mm_mul_ps(_mm_loadu_ps(M4+4), _mm_set1_ps((V3)[1]))),\
	TEMP_V=_mm_add_ps(TEMP_V, _mm_mul_ps(_mm_loadu_ps(M4+8), _mm_set1_ps((V3)[2]))),\
	TEMP_V=_mm_add_ps(TEMP_V, _mm_loadu_ps(M4+12)),\
	_mm_storeu_ps(M4+12, TEMP_V)
#define		mat4_rotate(DST_NEW, M4, ANGLE, DIR, TEMP_VEC2, TEMP_VEC3A, TEMP_VEC3B)\
	vec3_normalize(TEMP_VEC3A, DIR, (TEMP_VEC2)[0]),\
	(TEMP_VEC2)[0]=cosf(ANGLE),\
	(TEMP_VEC2)[1]=1-(TEMP_VEC2)[0],\
	vec3_mul1(TEMP_VEC3B, TEMP_VEC3A, (TEMP_VEC2)[1]),\
	(TEMP_VEC2)[1]=sinf(ANGLE),\
	(DST_NEW)[0]=(TEMP_VEC3B)[0]*(TEMP_VEC3A)[0]+(TEMP_VEC2)[0],\
	(DST_NEW)[1]=(TEMP_VEC3B)[0]*(TEMP_VEC3A)[1]+(TEMP_VEC2)[1]*(TEMP_VEC3A)[2],\
	(DST_NEW)[2]=(TEMP_VEC3B)[0]*(TEMP_VEC3A)[2]-(TEMP_VEC2)[1]*(TEMP_VEC3A)[1],\
	(DST_NEW)[3]=0,\
	\
	(DST_NEW)[4]=(TEMP_VEC3B)[1]*(TEMP_VEC3A)[0]-(TEMP_VEC2)[1]*(TEMP_VEC3A)[2],\
	(DST_NEW)[5]=(TEMP_VEC3B)[1]*(TEMP_VEC3A)[1]+(TEMP_VEC2)[0],\
	(DST_NEW)[6]=(TEMP_VEC3B)[1]*(TEMP_VEC3A)[2]+(TEMP_VEC2)[1]*(TEMP_VEC3A)[0],\
	(DST_NEW)[7]=0,\
	\
	(DST_NEW)[8]=(TEMP_VEC3B)[2]*(TEMP_VEC3A)[0]+(TEMP_VEC2)[1]*(TEMP_VEC3A)[1],\
	(DST_NEW)[9]=(TEMP_VEC3B)[2]*(TEMP_VEC3A)[1]-(TEMP_VEC2)[1]*(TEMP_VEC3A)[0],\
	(DST_NEW)[10]=(TEMP_VEC3B)[2]*(TEMP_VEC3A)[2]+(TEMP_VEC2)[0],\
	(DST_NEW)[11]=0,\
	\
	(DST_NEW)[12]=0,\
	(DST_NEW)[13]=0,\
	(DST_NEW)[14]=0,\
	(DST_NEW)[15]=1
#define		mat4_scale(M4, AMMOUNT, TEMP_V0)\
		_mm_storeu_ps(M4, _mm_mul_ps(_mm_loadu_ps(M4), _mm_set1_ps((AMMOUNT)[0]))),\
		_mm_storeu_ps(M4+4, _mm_mul_ps(_mm_loadu_ps(M4+4), _mm_set1_ps((AMMOUNT)[1]))),\
		_mm_storeu_ps(M4+8, _mm_mul_ps(_mm_loadu_ps(M4+8), _mm_set1_ps((AMMOUNT)[2]))),\

void	mat4_lookAt(float *dst, const float *cam, const float *center, const float *up);
void	mat4_FPSView(float *dst, const float *campos, float yaw, float pitch);
void	mat4_perspective(float *dst, float tanfov, float w_by_h, float znear, float zfar);
void	mat4_normalmat3(float *dst, float *m4);//inverse transpose of top left 3x3 submatrix

typedef struct CameraStruct
{
	float
		x, y, z,//position
		ax, ay,//yaw/phi, pitch/theta
		tanfov,
		move_speed, turn_speed,
		cax, sax, cay, say;
} Camera;
#define cam_copy(DSTCAM, SRCCAM) memcpy(&(DSTCAM), &(SRCCAM), sizeof(DSTCAM))
#define cam_moveForward(CAM, SPEED) (CAM).x+=(SPEED)*(CAM).cax*(CAM).cay, (CAM).y+=(SPEED)*(CAM).sax*(CAM).cay, (CAM).z+=(SPEED)*(CAM).say
#define cam_moveBack(CAM, SPEED)    (CAM).x-=(SPEED)*(CAM).cax*(CAM).cay, (CAM).y-=(SPEED)*(CAM).sax*(CAM).cay, (CAM).z-=(SPEED)*(CAM).say
#define cam_moveLeft(CAM, SPEED)    (CAM).x-=(SPEED)*(CAM).sax, (CAM).y+=(SPEED)*(CAM).cax
#define cam_moveRight(CAM, SPEED)   (CAM).x+=(SPEED)*(CAM).sax, (CAM).y-=(SPEED)*(CAM).cax
#define cam_moveUp(CAM, SPEED)      (CAM).z+=SPEED
#define cam_moveDown(CAM, SPEED)    (CAM).z-=SPEED
#define cam_update_ax(CAM)          (CAM).ax=fmodf((CAM).ax, (float)(2*M_PI)), (CAM).cax=cosf((CAM).ax), (CAM).sax=sinf((CAM).ax)
#define cam_update_ay(CAM)          (CAM).ay=fmodf((CAM).ay, (float)(2*M_PI)), (CAM).cay=cosf((CAM).ay), (CAM).say=sinf((CAM).ay)
#define cam_turnUp(CAM, SPEED)      (CAM).ay+=(SPEED)*(CAM).turn_speed, cam_update_ay(CAM)
#define cam_turnDown(CAM, SPEED)    (CAM).ay-=(SPEED)*(CAM).turn_speed, cam_update_ay(CAM)
#define cam_turnLeft(CAM, SPEED)    (CAM).ax+=(SPEED)*(CAM).turn_speed, cam_update_ax(CAM)
#define cam_turnRight(CAM, SPEED)   (CAM).ax-=(SPEED)*(CAM).turn_speed, cam_update_ax(CAM)
#define cam_turnMouse(CAM, DX, DY, SENSITIVITY)\
	(CAM).ax-=(SENSITIVITY)*(CAM).turn_speed*(DX), cam_update_ax(CAM),\
	(CAM).ay-=(SENSITIVITY)*(CAM).turn_speed*(DY), cam_update_ay(CAM)
#define		cam_zoomIn(CAM, RATIO)  (CAM).tanfov/=RATIO, (CAM).turn_speed=(CAM).tanfov>1?1:(CAM).tanfov
#define		cam_zoomOut(CAM, RATIO) (CAM).tanfov*=RATIO, (CAM).turn_speed=(CAM).tanfov>1?1:(CAM).tanfov
#define		cam_accelerate(GAIN)    (CAM).move_speed*=GAIN

#define		cam_relworld2cam(CAM, DISP, DST_CP)\
	(DST_CP)[2]=(DISP)[0]*(CAM).cax+(DISP)[1]*(CAM).sax,\
	(DST_CP)[0]=(DISP)[0]*(CAM).sax-(DISP)[1]*(CAM).cax,\
	(DST_CP)[1]=(DST_CP)[2]*(CAM).say-(DISP)[2]*(CAM).cay,\
	(DST_CP)[2]=(DST_CP)[2]*(CAM).cay+(DISP)[2]*(CAM).say
#define		cam_world2cam(CAM, P, DST_CP, TEMP_3F)\
	vec3_sub(TEMP_3F, P, &(CAM).x),\
	cam_relworld2cam(CAM, TEMP_3F, DST_CP)
#define		cam_cam2screen(CAM, CP, DST_S, X0, Y0)\
	(DST_S)[1]=(X0)/((CP)[2]*(CAM).tanfov),\
	(DST_S)[0]=(X0)+(CP)[0]*(DST_S)[1],\
	(DST_S)[1]=(Y0)+(CP)[1]*(DST_S)[1]
#endif

#define screen2NDC_x(Xs)      (SN_x1*(Xs)+SN_x0)
#define screen2NDC_y(Ys)      (SN_y1*(Ys)+SN_y0)
#define screen2NDC_x_bias(Xs) (SN_x1*(Xs+0.5f)+SN_x0)
#define screen2NDC_y_bias(Ys) (SN_y1*(Ys+0.5f)+SN_y0)
#define NDC2screen_x(X)       (NS_x1*(X)+NS_x0)
#define NDC2screen_y(Y)       (NS_y1*(Y)+NS_y0)
#define NDC2screen_x_bias(X)  (NS_x1*(X)+NS_x0-0.5f)
#define NDC2screen_y_bias(Y)  (NS_y1*(Y)+NS_y0-0.5f)


extern Camera cam;


typedef struct VertexStruct
{
	float
		x, y, z,
		r, g, b,//can be reused as normals or particle velocity
		tx, ty;
} Vertex;
#if 0
typedef struct ParticleStruct
{
	float
		px, py,
		vx, vy,
		r, g, b, a;
} Particle;
#endif
typedef struct TextureStruct
{
	int iw, ih, ch, ch0;
	unsigned char *data;
} Texture;
void load_texture(Texture *texture, const char *searchpath, const char *filetitle, int ch);
void load_model(const char *searchpath, const char *filetitle, ArrayHandle *vertices, ArrayHandle *indices);


//vulkan wrapper
int vkw_init(const char *searchpath, Texture *atlas, Texture *texture, ArrayHandle vertices, ArrayHandle indices, ArrayHandle particles, int anisotropyenable, int miplevels, int enableMSAA);
int vkw_resize(int w, int h);
int vkw_render();
int vkw_cleanup();

//text
extern ArrayHandle text_vertices;
void init_atlas_coords(Texture const *atlas);
int set_textcolor(int color);
int set_textbk(int color);
float set_textzoom(float zoom);
float print_line(float tab_origin, float x, float y, const char *msg, int msg_length, int req_cols, int *ret_idx, int *ret_cols);
float GUIPrint(float tab_origin, float x, float y, const char *format, ...);

//region
typedef struct RegionStruct
{
	float rx0, ry0, rdx, rdy;
	int cx0, cy0, cdx, cdy;
} Region;
extern const Region *const currentregion;//{x0, y0, dx, dy}
void set_region(Region *r);//r is region (viewport), c is crop (scissor)


#endif//VKSANDBOX_H
