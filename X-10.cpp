#include		<Windows.h>
#include		<gl\glu.h>
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include		<CL\cl.h>
#include		<sys\stat.h>
#include		<fstream>
#include<locale>
#include<codecvt>
#include		<string>
#include		<vector>
#include		<algorithm>
#include		<functional>
#include		<Shlwapi.h>
#include		<gdiplus.h>
#pragma			comment(lib, "Shlwapi.lib")
#pragma			comment(lib, "gdiplus.lib")
#pragma			comment(lib, "OpenGL32.lib")
#pragma			comment(lib, "GLU32.lib")
#pragma			comment(lib, "OpenCL.lib")
//#define AA_MOUSEMOVE_TRIGGER//draw on any mouse move for aa
#define			MOUSE_SENSITIVITY	.003
#define			DTANFOV				1.1
#define			DFOV				1.1
#define			TIMER_ELAPSE		10
int				nRandPols=1, nRandTPols=0;//5	100
void			render				();
struct			POL					{double x1, y1, z1, x2, y2, z2, x3, y3, z3, tx1x, tx1y, tx2x, tx2y, tx3x, tx3y, txXX, txXY, txYX, txYY;char tx;int color;} *env=0, *tenv=0;
const double	TWOPI=2*acos(-1.);
int				*txh=0, *txw=0, ntx=0, npols=0, ntpols=0;
int				h=0, w=0, X0, Y0;

//double const		camx0=400, camy0=400, camz0=400, dcam0=4;
double const	camx0=4, camy0=4, camz0=4, dcam0=0.04;
double const	ax0=225*TWOPI/360, ay0=324.7356103172454*TWOPI/360, tanfov0=1, fov0=.125*TWOPI, da0=2*TWOPI/360;
//double const	ax0=3.1532853373782164, ay0=6.2736568316636330, tanfov0=0.017029283871059675, fov0=.125*TWOPI, da0=2*TWOPI/360;

double			camx=camx0, camy=camy0, camz=camz0, dcam=dcam0;
double			ax=ax0, ay=ay0, tanfov=tanfov0, fov=fov0, da=da0, da_tfov=tanfov, cax=cos(ax), sax=sin(ax), cay=cos(ay), say=sin(ay);

char			mode=1, drag=0, d_bypass=0, timer=0, kb[256]={0}, kp=0, line[128], linelen;
tagPOINT		centerP, mouseP0;
tagRECT			R;
HDC__			*hDC;
HWND__			*ghWnd;
_LARGE_INTEGER	li;
long long		freq, nticks;
#if 0//clock functions benchmark
#include<ctime>
//#include<boost/date_time/posix_time/posix_time.hpp>//only boost header	and not even one
#endif
#if 0//condition vs pointer benchmark
struct BenchmarkColor
{
	virtual void color_inline(int *rgb, int k){rgb[k]=0xA0A0A0;};
	virtual void color(int *rgb, int k);
//	virtual void color_inline(int *rgb, int k)=0;
//	virtual void color(int *rgb, int k)=0;
};
void BenchmarkColor::color(int *rgb, int k){rgb[k]=0xA0A0A0;};
struct BenchmarkColor1:BenchmarkColor
{
	void color_inline(int *rgb, int k){rgb[k]=0xA0A0A0;}
	void color(int *rgb, int k);
};
void BenchmarkColor1::color(int *rgb, int k){rgb[k]=0xA0A0A0;}
struct BenchmarkColor2:BenchmarkColor
{
	void color_inline(int *rgb, int k){rgb[k]=0xB0B0B0;}
	void color(int *rgb, int k);
};
void BenchmarkColor2::color(int *rgb, int k){rgb[k]=0xB0B0B0;}
void f_black(int *rgb, int k){rgb[k]=0xA0A0A0;}
void f_white(int *rgb, int k){rgb[k]=0xB0B0B0;}
#endif
#if 0//declare, initialize x2, render, free
//int *container;
//std::vector<int> container;
//#include<list>
//std::list<int> container;
//#include<set>
//std::set<int> container;
#include<map>
std::map<int, int> container;
#endif
#if 0
int func0(unsigned char r, unsigned char g, unsigned char b){return r+g+b;}
int func1(unsigned char r, unsigned char g, unsigned char b){return r<<16|g<<8|b;}
int func2(unsigned char r, unsigned char g, unsigned char b){return b<<16|g<<8|r;}
int func3(unsigned char r, unsigned char g, unsigned char b){return r*g<<8|g*b|b*r>>8;}
int func4(unsigned char r, unsigned char g, unsigned char b){return b*r<<8|g*b|r*g>>8;}
int func5(unsigned char r, unsigned char g, unsigned char b){return r*g*b;}
int func6(unsigned char r, unsigned char g, unsigned char b){int av=(r+g+b)/3;return av<<16|av<<8|av;}
int func7(unsigned char r, unsigned char g, unsigned char b){return (r^g)<<16|(g^b)<<8|(b^r);}//{return (r+g)/2<<16|(g+b)/2<<8|(b+r)/2;}
struct Func{virtual int operator()(unsigned char r, unsigned char g, unsigned char b)=0;};
struct Func0:Func{int operator()(unsigned char r, unsigned char g, unsigned char b){return r+g+b;}};
struct Func1:Func{int operator()(unsigned char r, unsigned char g, unsigned char b){return r<<16|g<<8|b;}};
struct Func2:Func{int operator()(unsigned char r, unsigned char g, unsigned char b){return b<<16|g<<8|r;}};
struct Func3:Func{int operator()(unsigned char r, unsigned char g, unsigned char b){return r*g<<8|g*b|b*r>>8;}};
struct Func4:Func{int operator()(unsigned char r, unsigned char g, unsigned char b){return b*r<<8|g*b|r*g>>8;}};
struct Func5:Func{int operator()(unsigned char r, unsigned char g, unsigned char b){return r*g*b;}};
struct Func6:Func{int operator()(unsigned char r, unsigned char g, unsigned char b){int av=(r+g+b)/3;return av<<16|av<<8|av;}};
struct Func7:Func{int operator()(unsigned char r, unsigned char g, unsigned char b){return (r^g)<<16|(g^b)<<8|(b^r);}};
#endif
#if 0
class LOL_class
{
	int color;
public:
	int get_color(){return color;}
	void set(unsigned char r, unsigned char g, unsigned char b)
	{
		auto p=(unsigned char*)&color;
		p[0]=p[1]=p[2]=(r+g+b)/2;
	}
	void set_color(unsigned char r, unsigned char g, unsigned char b)
	{
		auto p=(unsigned char*)&color;
		p[0]=r, p[1]=g, p[2]=b;
	}
};
#endif
#if 0
//int glParam=0, gwParam=0;
__declspec(noinline)void AssignPixel(int *rgb, int x, int y, int c){rgb[w*y+x]=c;}
//void f_black(int *rgb, int k){rgb[k]=0x000000;}
//void f_white(int *rgb, int k){rgb[k]=0xFFFFFF;}
#endif

double inv_sqrt(double x)//http://stackoverflow.com/questions/11513344/how-to-implement-the-fast-inverse-square-root-in-java
{
	double t0;
	(long long&)t0=0x5FE6EC85E7DE30DA-((long long&)x>>1);
	return t0*(1.5-.5*x*t0*t0);
}
struct	GUIPrint
{
	static char	line[1024];
	static int	linelen;
	GUIPrint(HDC__ *hDC, int x, int y, const char* format, ...);
	GUIPrint(HDC__ *hDC, int x, int y, int value);
};
char		GUIPrint::line[1024];
int			GUIPrint::linelen;
			GUIPrint::GUIPrint(HDC__ *hDC, int x, int y, const char* a, ...)
{
	linelen=vsprintf_s(line, sizeof(line), a, (char*)(&reinterpret_cast<const char &>(a))+((sizeof(a)+3)&~3));
	if(linelen>0)
		TextOutA(hDC, x, y, line, linelen);
}
			GUIPrint::GUIPrint(HDC__ *hDC, int x, int y, int value)
{
	linelen=sprintf_s(line, 1024, "%d", value);
	if(linelen>0)
		TextOutA(hDC, x, y, line, linelen);
}
		
namespace _3d
{
	double *wbuffer;
	int *rgb, lineColor;
	int bx1, bx2, by1, by2, bw;

	double //x0, y0, z0,
		Xs1, Ys1, Xcp1, Ycp1, Zcp1,
		Xs2, Ys2, Xcp2, Ycp2, Zcp2;
	void line_A_coeff_x(double Xcp1, double Ycp1, double Zcp1, double Xcp2, double Ycp2, double Zcp2, double &a, double &b)
	{
		double t=(Xcp2-Xcp1)*Zcp1-Xcp1*(Zcp2-Zcp1);
		a=(Zcp1-Zcp2)*tanfov/(X0*t), b=((Zcp2-Zcp1)*tanfov+Xcp2-Xcp1)/t;
	}
	void line_A_coeff_y(double Xcp1, double Ycp1, double Zcp1, double Xcp2, double Ycp2, double Zcp2, double &a, double &b)
	{
		double t=(Ycp2-Ycp1)*Zcp1-Ycp1*(Zcp2-Zcp1);
		a=(Zcp1-Zcp2)*tanfov/(X0*t), b=((Zcp2-Zcp1)*Y0*tanfov/X0+Ycp2-Ycp1)/t;
	}
	void _3dLineOnScreen_draw(double x1, double y1, double Xcp1, double Ycp1, double Zcp1, double x2, double y2, double Xcp2, double Ycp2, double Zcp2)
	{
		double xa, ya, xb, yb, dx=x2-x1, dy=y2-y1;
		if(abs(dx)>abs(dy))//horizontal
		{
			double dy_dx=dy/dx;
			if(x1<x2)
			{
				if(x1<bx1)
					xa=bx1, ya=y1+dy_dx*(bx1-x1);
				else
					xa=x1, ya=y1;
				if(x2>bx2-1)
					xb=bx2-1, yb=y1+dy_dx*(bx2-1-x1);
				else
					xb=x2, yb=y2;
			}
			else
			{
				if(x2<bx1)
					xa=bx1, ya=y1+dy_dx*(bx1-x1);
				else
					xa=x2, ya=y2;
				if(x1>bx2-1)
					xb=bx2-1, yb=y1+dy_dx*(bx2-1-x1);
				else
					xb=x1, yb=y1;
			}
			double a, b;
			line_A_coeff_x(Xcp1, Ycp1, Zcp1, Xcp2, Ycp2, Zcp2, a, b);
		/*	{
				double t=(Xcp2-Xcp1)*Zcp1-Xcp1*(Zcp2-Zcp1);
				a=(Zcp1-Zcp2)*tanfov/(X0*t), b=((Zcp2-Zcp1)*tanfov+Xcp2-Xcp1)/t;
			}*/
			for(int x=int(xa), xEnd=int(xb);x<=xEnd;++x)//horizontal
			{
				//int y;
				//{
				//	double Y=y1+dy_dx*(x-x1);
				//	y=int(Y)-(Y<0);
				//}
				int y=int(std::floor(y1+dy_dx*(x-x1)));//-0.5 truncated as 0
				if(y>=by1&&y<by2)
				{
					int pos=w*y+x;
					double A=a*x+b;
					if(A>wbuffer[pos])
						rgb[pos]=lineColor, wbuffer[pos]=A;
					else if(A==wbuffer[pos])
					{
						auto p=(unsigned char*)&rgb[pos], c=(unsigned char*)&lineColor;//little endian
						p[0]=(p[0]+c[0])>>1;//b
						p[1]=(p[1]+c[1])>>1;//g
						p[2]=(p[2]+c[2])>>1;//r
					}//*/
				/*	{
						((unsigned char*)&rgb[pos])[0]=((unsigned char*)&rgb[pos])[0]+((unsigned char*)&lineColor)[0]>>1;
						((unsigned char*)&rgb[pos])[1]=((unsigned char*)&rgb[pos])[1]+((unsigned char*)&lineColor)[1]>>1;
						((unsigned char*)&rgb[pos])[2]=((unsigned char*)&rgb[pos])[2]+((unsigned char*)&lineColor)[2]>>1;
					}//*/
					//	rgb[pos]=0xFFC0CB;
				}
			}
		}
		else//vertical
		{
			double dx_dy=dx/dy;
			if(y1<y2)
			{
				if(y1<by1)
					xa=x1+dx_dy*(by1-y1), ya=by1;
				else
					xa=x1, ya=y1;
				if(y2>by2-1)
					xb=x1+dx_dy*(by2-1-y1), yb=by2-1;
				else
					xb=x2, yb=y2;
			}
			else
			{
				if(y2<by1)
					xa=x1+dx_dy*(by1-y1), ya=by1;
				else
					xa=x2, ya=y2;
				if(y1>by2-1)
					xb=x1+dx_dy*(by2-1-y1), yb=by2-1;
				else
					xb=x1, yb=y1;
			}
			double a, b;
			line_A_coeff_y(Xcp1, Ycp1, Zcp1, Xcp2, Ycp2, Zcp2, a, b);
		/*	{
				double t=(Ycp2-Ycp1)*Zcp1-Ycp1*(Zcp2-Zcp1);
				a=(Zcp1-Zcp2)*tanfov/(X0*t), b=((Zcp2-Zcp1)*Y0*tanfov/X0+Ycp2-Ycp1)/t;
			}*/
			for(int y=int(ya), yEnd=int(yb);y<yEnd;++y)//vertical
			{
				int x=int(std::floor(x1+dx_dy*(y-y1)));//-0.5 truncated as 0
				if(x>=bx1&&x<bx2)
				{
					int pos=w*y+x;
					double A=a*y+b;
					if(A>wbuffer[pos])
						rgb[pos]=lineColor, wbuffer[pos]=A;
					else if(A==wbuffer[pos])
					{
						auto p=(unsigned char*)&rgb[pos], c=(unsigned char*)&lineColor;
						p[0]=(p[0]+c[0])>>1;//b
						p[1]=(p[1]+c[1])>>1;//g
						p[2]=(p[2]+c[2])>>1;//r
					}
				}
			}
		}
	}
	double linearY(double x1, double y1, double x2, double y2, double x){return (y2-y1)/(x2-x1)*(x-x1)+y1;}
	double linearX(double x1, double y1, double x2, double y2, double y){return (x2-x1)/(y2-y1)*(y-y1)+x1;}
	void _3dLineOnScreen(double x1, double y1, double Xcp1, double Ycp1, double Zcp1, double x2, double y2, double Xcp2, double Ycp2, double Zcp2)
	{
		if(abs(x1)>2e9||abs(y1)>2e9||abs(x2)>2e9||abs(y2)>2e9)
		{
			bool valid=true;
			//up
			if(y1<by1)
			{
				if(y2<by1)
					valid=false;
				else
					x1=linearX(x1, y1, x2, y2, by1), y1=by1;
			}
			else if(y2<by1)
				x2=linearX(x1, y1, x2, y2, by1), y2=by1;

			if(valid)
			{
				//right
				if(x1>bx2)
				{
					if(x2>bx2)
						valid=false;
					else
						y1=linearY(x1, y1, x2, y2, bx2), x1=bx2;
				}
				else if(x2>bx2)
					y2=linearY(x1, y1, x2, y2, bx2), x2=bx2;

				if(valid)
				{
					//bottom
					if(y1>by2)
					{
						if(y2>by2)
							valid=false;
						else
							x1=linearX(x1, y1, x2, y2, by2), y1=by2;
					}
					else if(y2>by2)
						x2=linearX(x1, y1, x2, y2, by2), y2=by2;

					if(valid)
					{
						//left
						if(x1<bx1)
						{
							if(x2<bx1)
								valid=false;
							else
								y1=linearY(x1, y1, x2, y2, bx1), x1=bx1;
						}
						else if(x2<bx1)
							y2=linearY(x1, y1, x2, y2, bx1), x2=bx1;

						if(valid)
							_3dLineOnScreen_draw(x1, y1, Xcp1, Ycp1, Zcp1, x2, y2, Xcp2, Ycp2, Zcp2);
					}
				}
			}
		}
		else
			_3dLineOnScreen_draw(x1, y1, Xcp1, Ycp1, Zcp1, x2, y2, Xcp2, Ycp2, Zcp2);
	}
	void _2dExtrapolateLine(double x1, double y1, double Xcp1, double Ycp1, double Zcp1, double x2, double y2, double Xcp2, double Ycp2, double Zcp2)//P1 behind
	{
		if(x1==x2&&y1==y2)
		{
			if(Zcp1>0)
			{
				double Acp1=1/Zcp1;
				if(x1>=bx1&&x1<bx2&&y1>=by1&&y1<by2)
				{
					int pos=int(x1)+bw*int(y1);
					if(Acp1>wbuffer[pos])
						rgb[pos]=lineColor, wbuffer[pos]=Acp1;
				}
			}
		}
		else
		{
			double dx=x2-x1, dy=y2-y1, r=sqrt(2)*(abs(x2)+abs(y2)+2*(X0+Y0))/(abs(dx)+abs(dy));
			_3dLineOnScreen(x2, y2, Xcp1, Ycp1, Zcp1, x2+r*dx, y2+r*dy, Xcp2, Ycp2, Zcp2);
		}
	}
	//world coordinates -> camera coordinates
	void point_world_camera(double dx, double dy, double dz, double &Xcp, double &Ycp, double &Zcp)
	{
		double temp=dx*cax+dy*sax;
		Xcp=dx*sax-dy*cax, Ycp=temp*say-dz*cay, Zcp=temp*cay+dz*say;
	}
	//camera coordinates -> screen coordinates
	void point_camera_screen(double Xcp, double Ycp, double Zcp, double &Xs, double &Ys)
	{
		double temp=X0/(Zcp*tanfov);
		Xs=X0+Xcp*temp, Ys=Y0+Ycp*temp;
	}
	//world coordinates -> screen coordinates		with depth info
	void point_world_screen(double dx, double dy, double dz, double &Xcp, double &Ycp, double &Zcp, double &Xs, double &Ys)
	{
		double temp=dx*cax+dy*sax;
		Xcp=dx*sax-dy*cax, Ycp=temp*say-dz*cay, Zcp=temp*cay+dz*say;
		temp=X0/(Zcp*tanfov);
		Xs=X0+Xcp*temp, Ys=Y0+Ycp*temp;
	}
	void _2dExtrapolateInfRay(double x1, double y1, double Xcp1, double Ycp1, double Zcp1, double x2, double y2, double Xcp2, double Ycp2, double Zcp2)
	{
		if(x1==x2&&y1==y2)
		{
			if(Zcp1>0)
			{
				double Acp1=1/Zcp1;
				if(x1>=bx1&&x1<bx2&&y1>=by1&&y1<by2)
				{
					int pos=int(x1)+bw*int(y1);
					if(Acp1>wbuffer[pos])
						rgb[pos]=lineColor, wbuffer[pos]=Acp1;
				}
			}
		}
		else
		{
			double dx12=x2-x1, dy12=y2-y1, r=sqrt(2)*(abs(x2)+abs(y2)+2*(X0+Y0))/(abs(dx12)+abs(dy12));
			_3dLineOnScreen(x1, y1, Xcp1, Ycp1, Zcp1, x2+r*dx12, y2+r*dy12, Xcp2, Ycp2, Zcp2);//[1 2 ->]
		}
	}
	void _2dExtrapolateInfLine(double x1, double y1, double Xcp1, double Ycp1, double Zcp1, double x2, double y2, double Xcp2, double Ycp2, double Zcp2)
	{
		if(x1==x2&&y1==y2)
		{
			if(Zcp1>0)
			{
				double Acp1=1/Zcp1;
				if(x1>=bx1&&x1<bx2&&y1>=by1&&y1<by2)
				{
					int pos=int(x1)+bw*int(y1);
					if(Acp1>wbuffer[pos])
						rgb[pos]=lineColor, wbuffer[pos]=Acp1;
				}
			}
		}
		else
		{
			double dx12=x2-x1, dy12=y2-y1, r=sqrt(2)*(abs(x2)+abs(y2)+2*(X0+Y0))/(abs(dx12)+abs(dy12));
			_3dLineOnScreen(x1-r*dx12, y1-r*dy12, Xcp1, Ycp1, Zcp1, x1+r*dx12, y1+r*dy12, Xcp2, Ycp2, Zcp2);//[<- 1 2 ->]
		}
	}
	//draws a ray starting at P1 through P2
	void infiniteRay(double x1, double y1, double z1, double x2, double y2, double z2)//[1 2 -> inf]
	{
		double Xcp1, Ycp1, Zcp1;
		point_world_camera(x1-camx, y1-camy, z1-camz, Xcp1, Ycp1, Zcp1);//P1: starting point
		double XcpInf, YcpInf, ZcpInf;
		point_world_camera(x2-x1, y2-y1, z2-z1, XcpInf, YcpInf, ZcpInf);//Inf: direction from camera
		if(ZcpInf>0)//ray pointing forward
		{
			double XsInf, YsInf;
			point_camera_screen(XcpInf, YcpInf, ZcpInf, XsInf, YsInf);//Inf
			double Xcp2, Ycp2, Zcp2;
			point_world_camera(x2-camx, y2-camy, z2-camz, Xcp2, Ycp2, Zcp2);//P2
			if(Zcp1>0)//starts in front of the camera
			{
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_3dLineOnScreen(Xs1, Ys1, Xcp1, Ycp1, Zcp1, XsInf, YsInf, Xcp2, Ycp2, Zcp2);
			}
			else if(Zcp1==0)//starts at the camera plane
			{
				Xcp1+=Xcp1-Xcp2, Ycp1+=Ycp1-Ycp2, Zcp1+=Zcp1-Zcp2;//1 behind
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
			/*	if(_2dNotInMyFace(Xs1, Ys1, Xcp1, Ycp1, Zcp1, XsInf, YsInf, Xcp2, Ycp2, Zcp2))
				{
					double dx12, dy12, r;	_2dExtrapolate(Xs1, Ys1, XsInf, YsInf, dx12, dy12, r);
					_3dLineOnScreen(Xs1, Ys1, Xcp1, Ycp1, Zcp1, XsInf, YsInf, Xcp2, Ycp2, Zcp2);
				}//*/
				_2dExtrapolateLine(Xs1, Ys1, Xcp1, Ycp1, Zcp1, XsInf, YsInf, Xcp2, Ycp2, Zcp2);
			}
			else//starts behind the camera
			{
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_2dExtrapolateLine(Xs1, Ys1, Xcp1, Ycp1, Zcp1, XsInf, YsInf, Xcp2, Ycp2, Zcp2);
			}
		}
		else if(ZcpInf==0)//ray pointing parallel to camera plane
		{
			if(Zcp1>0)//starts in front of the camera
			{
				double Xcp2, Ycp2, Zcp2, Xs2, Ys2;
				point_world_screen(x2-camx, y2-camy, z2-camz, Xcp2, Ycp2, Zcp2, Xs2, Ys2);//P2
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_2dExtrapolateInfRay(Xs2, Ys2, Xcp1, Ycp1, Zcp1, Xs1, Ys1, Xcp2, Ycp2, Zcp2);
			}
		}
		else//ray pointing back
		{
			if(Zcp1>0)//starts in front of the camera
			{
				double XsInf, YsInf;
				point_camera_screen(XcpInf, YcpInf, ZcpInf, XsInf, YsInf);//Inf
				double Xcp2, Ycp2, Zcp2;
				point_world_camera(x2-camx, y2-camy, z2-camz, Xcp2, Ycp2, Zcp2);//P2
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_2dExtrapolateLine(XsInf, YsInf, Xcp1, Ycp1, Zcp1, Xs1, Ys1, Xcp2, Ycp2, Zcp2);
			}
		}
	}
	//draws the line through P1 and P2
	void infiniteLine(double x1, double y1, double z1, double x2, double y2, double z2)//[inf <- 1 2 -> inf]
	{
		double Xcp1, Ycp1, Zcp1;
		point_world_camera(x1-camx, y1-camy, z1-camz, Xcp1, Ycp1, Zcp1);//P1
		double XcpInf, YcpInf, ZcpInf;
		point_world_camera(x2-x1, y2-y1, z2-z1, XcpInf, YcpInf, ZcpInf);//1-2 from cam
		if(ZcpInf>0)//1-2 points forward
		{
			double XsInf, YsInf;
			point_camera_screen(XcpInf, YcpInf, ZcpInf, XsInf, YsInf);//Inf

			double Xcp2, Ycp2, Zcp2;
			point_world_camera(x2-camx, y2-camy, z2-camz, Xcp2, Ycp2, Zcp2);//P2
			if(Zcp1>0)//P1 in front of the camera
			{
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_2dExtrapolateInfRay(XsInf, YsInf, Xcp1, Ycp1, Zcp1, Xs1, Ys1, Xcp2, Ycp2, Zcp2);
			}
			else
			{
				if(Zcp1==0)//P1 at the camera plane
					Xcp1+=Xcp1-Xcp2, Ycp1+=Ycp1-Ycp2, Zcp1+=Zcp1-Zcp2;
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_2dExtrapolateLine(Xs1, Ys1, Xcp1, Ycp1, Zcp1, XsInf, YsInf, Xcp2, Ycp2, Zcp2);//P1 behind the camera
			}
		}
		else if(ZcpInf==0)//1-2 is parallel to cam plane
		{
			if(Zcp1>0)//in front of cam
			{
				double Xcp2, Ycp2, Zcp2, Xs2, Ys2;
				point_world_screen(x2-camx, y2-camy, z2-camz, Xcp2, Ycp2, Zcp2, Xs2, Ys2);//P2
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_2dExtrapolateInfLine(Xs2, Ys2, Xcp1, Ycp1, Zcp1, Xs1, Ys1, Xcp2, Ycp2, Zcp2);
			}
		}
		else//1-2 points back
		{
			double XsInf, YsInf;
			point_camera_screen(XcpInf, YcpInf, ZcpInf, XsInf, YsInf);//Inf

			double Xcp2, Ycp2, Zcp2;
			point_world_camera(x2-camx, y2-camy, z2-camz, Xcp2, Ycp2, Zcp2);//P2
			if(Zcp1>0)//P1 in front of cam
			{
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_2dExtrapolateInfRay(XsInf, YsInf, Xcp1, Ycp1, Zcp1, Xs1, Ys1, Xcp2, Ycp2, Zcp2);
			}
			else
			{
				if(Zcp1==0)//P1 at cam plane
					Xcp1-=Xcp1-Xcp2, Ycp1-=Ycp1-Ycp2, Zcp1-=Zcp1-Zcp2;
				double Xs1, Ys1;
				point_camera_screen(Xcp1, Ycp1, Zcp1, Xs1, Ys1);
				_2dExtrapolateLine(Xs1, Ys1, Xcp1, Ycp1, Zcp1, XsInf, YsInf, Xcp2, Ycp2, Zcp2);//P1 behind the camera
			}
		}
	}
	void point(double x, double y, double z)
	{
		double dx=x-camx, dy=y-camy, dz=z-camz, cpt=dx*cax+dy*sax, Xcp1=dx*sax-dy*cax, Ycp1=cpt*say-dz*cay;
		double Zcp1=cpt*cay+dz*say;
		if(Zcp1>0)
		{
			double Acp1=1/Zcp1;
			cpt=X0/(Zcp1*tanfov);
			double Xs1=X0+Xcp1*cpt, Ys1=Y0+Ycp1*cpt;
			if(Xs1>=bx1&&Xs1<bx2&&Ys1>=by1&&Ys1<by2)
			{
				int pos=int(Xs1)+bw*int(Ys1);
				if(Acp1>wbuffer[pos])
					rgb[pos]=lineColor, wbuffer[pos]=Acp1;
			}
		}
	}
	bool isNanOrInf(double x){return (((int*)&x)[1]&0x7FF00000)==0x7FF00000;}
	void line(double x1, double y1, double z1, double x2, double y2, double z2, int lineColor, int *rgb, double *wbuffer)
	{
		_3d::wbuffer=wbuffer;
		_3d::rgb=rgb, _3d::lineColor=lineColor;
		bx1=0, bx2=w, by1=0, by2=h, bw=w;

		int drawn=0;
		switch(isNanOrInf(x1)+isNanOrInf(y1)+isNanOrInf(z1)+isNanOrInf(x2)+isNanOrInf(y2)+isNanOrInf(z2))
		{
		case 0:
			{
				{
					double dx=x1-camx, dy=y1-camy, dz=z1-camz, cpt=dx*cax+dy*sax;
					Xcp1=dx*sax-dy*cax, Ycp1=cpt*say-dz*cay, Zcp1=cpt*cay+dz*say;
				}
				double dx=x2-camx, dy=y2-camy, dz=z2-camz, cpt=dx*cax+dy*sax, Xcp2=dx*sax-dy*cax, Ycp2=cpt*say-dz*cay, Zcp2=cpt*cay+dz*say;
				if(Zcp2>0)
				{
					if(Zcp1>0)
					{
						cpt=X0/(Zcp1*tanfov), Xs1=X0+Xcp1*cpt, Ys1=Y0+Ycp1*cpt;
						cpt=X0/(Zcp2*tanfov), Xs2=X0+Xcp2*cpt, Ys2=Y0+Ycp2*cpt;
						_3dLineOnScreen(Xs1, Ys1, Xcp1, Ycp1, Zcp1, Xs2, Ys2, Xcp2, Ycp2, Zcp2);
					}
					else if(Zcp1==0)
					{
						Xcp1+=Xcp1-Xcp2, Ycp1+=Ycp1-Ycp2, Zcp1+=Zcp1-Zcp2;
						cpt=X0/(Zcp1*tanfov), Xs1=X0+Xcp1*cpt, Ys1=Y0+Ycp1*cpt;
						cpt=X0/(Zcp2*tanfov), Xs2=X0+Xcp2*cpt, Ys2=Y0+Ycp2*cpt;
						_2dExtrapolateLine(Xs1, Ys1, Xcp1, Ycp1, Zcp1, Xs2, Ys2, Xcp2, Ycp2, Zcp2);
					}
					else
					{
						cpt=X0/(Zcp1*tanfov), Xs1=X0+Xcp1*cpt, Ys1=Y0+Ycp1*cpt;
						cpt=X0/(Zcp2*tanfov), Xs2=X0+Xcp2*cpt, Ys2=Y0+Ycp2*cpt;
						_2dExtrapolateLine(Xs1, Ys1, Xcp1, Ycp1, Zcp1, Xs2, Ys2, Xcp2, Ycp2, Zcp2);
					}
					drawn=1;
				}
				else if(Zcp2==0)
				{
					if(Zcp1>0)
					{
						Xcp2+=Xcp2-Xcp1, Ycp2+=Ycp2-Ycp1, Zcp2+=Zcp2-Zcp1;
						cpt=X0/(Zcp1*tanfov), Xs1=X0+Xcp1*cpt, Ys1=Y0+Ycp1*cpt;
						cpt=X0/(Zcp2*tanfov), Xs2=X0+Xcp2*cpt, Ys2=Y0+Ycp2*cpt;
						_2dExtrapolateLine(Xs2, Ys2, Xcp1, Ycp1, Zcp1, Xs1, Ys1, Xcp2, Ycp2, Zcp2), drawn=1;
					}
				}
				else if(Zcp1>0)
				{
					cpt=X0/(Zcp1*tanfov), Xs1=X0+Xcp1*cpt, Ys1=Y0+Ycp1*cpt;
					cpt=X0/(Zcp2*tanfov), Xs2=X0+Xcp2*cpt, Ys2=Y0+Ycp2*cpt;
					_2dExtrapolateLine(Xs2, Ys2, Xcp1, Ycp1, Zcp1, Xs1, Ys1, Xcp2, Ycp2, Zcp2), drawn=1;
				}
			}
			break;
		case 1:
			{
				if(isinf(x1))
					infiniteRay(x2, y2, z2, x1==_HUGE?x2+1:x2-1, y2, z2), drawn=1;
				else if(isinf(y1))
					infiniteRay(x2, y2, z2, x2, y1==_HUGE?y2+1:y2-1, z2), drawn=1;
				else if(isinf(z1))
					infiniteRay(x2, y2, z2, x2, y2, z1==_HUGE?z2+1:z2-1), drawn=1;
				else if(isinf(x2))
					infiniteRay(x1, y1, z1, x2==_HUGE?x1+1:x1-1, y1, z1), drawn=1;
				else if(isinf(y2))
					infiniteRay(x1, y1, z1, x1, y2==_HUGE?y1+1:y1-1, z1), drawn=1;
				else if(isinf(z2))
					infiniteRay(x1, y1, z1, x1, y1, z2==_HUGE?z1+1:z1-1), drawn=1;
			}
			break;
		case 2:
			{
				if(isinf(x1)&&isinf(x2))
				{
					if(y1==x2&&z1==z2)
						infiniteLine(camx, x2, z2, camx+1, x2, z2), drawn=1;
				}
				else if(isinf(y1)&&isinf(x2))
				{
					if(z1==z2&&x1==x2)
						infiniteLine(x2, camy, z2, x2, camy+1, z2), drawn=1;
				}
				else if(isinf(z1)&&isinf(z2))
				{
					if(x1==x2&&y1==x2)
						infiniteLine(x2, x2, camz, x2, x2, camz+1), drawn=1;
				}
			}
			break;
		}
		if(!drawn)
		{
			if(!(isNanOrInf(x1)+isNanOrInf(y1)+isNanOrInf(z1)))
				point(x1, y1, z1);
			else if(!(isNanOrInf(x2)+isNanOrInf(x2)+isNanOrInf(z2)))
				point(x1, y1, z1);
		}
	}
}

struct			Mode
{
	virtual void initiate()=0;
	virtual void resize()=0;
	virtual void changeFov()=0;
	virtual void render()=0;
	virtual void pushTexture(int *texture_)=0;
	virtual int* popTexture()=0;
	virtual void enqueueTextures()=0;
	virtual void clearTextures()=0;
	virtual void finish()=0;
} *m;
struct			LinearSW:private Mode
{
	int **texture;
	int lstart, lend, *libuffer, *lfbuffer;
	double *wbuffer;
//	double *rgb_r, *rgb_g, *rgb_b;
//	int *rgb_r, *rgb_g, *rgb_b;
//	int *rgb_n;
	int *rgb, rgbn;
	HDC__ *hMemDC;
	HBITMAP__ *hBitmap;
	void initiate();
	void resize();
	void changeFov();
	void render();
	void draft_1behind		(double *v1, double *v2, double *v3);
	void draft_2behind		(double *v1, double *v2, double *v3);
	void draft_start		(double *v1, double *v2);
	void draft				(double *v1, double *v2);
	void draft_crit_start	(double *v1, double *v2);
	void draft_crit			(double *v1, double *v2);
	void pushTexture(int *texture_);
	void enqueueTextures();
	int* popTexture();
	void clearTextures();
	void finish();

#if 1//condition vs pointer benchmark
	void f_black(int *rgb, int k){rgb[k]=0xA0A0A0;}
	void f_white(int *rgb, int k){rgb[k]=0xB0B0B0;}
#endif
} lsw;
void				LinearSW::initiate()
{
	texture=0;
	rgbn=h*w;
	wbuffer=(double*)malloc(rgbn*sizeof(double)), libuffer=(int*)malloc(h*sizeof(int)), lfbuffer=(int*)malloc(h*sizeof(int));
	hMemDC=CreateCompatibleDC(hDC);
//	rgb_r=(double*)malloc(w*h*sizeof(double)), rgb_g=(double*)malloc(w*h*sizeof(double)), rgb_b=(double*)malloc(w*h*sizeof(double)), rgb_n=(int*)malloc(w*h*sizeof(int));
//	rgb_r=(int*)malloc(w*h*sizeof(int)), rgb_g=(int*)malloc(w*h*sizeof(int)), rgb_b=(int*)malloc(w*h*sizeof(int)), rgb_n=(int*)malloc(w*h*sizeof(int));
	tagBITMAPINFO bmpInfo={{sizeof(tagBITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
	hBitmap=(HBITMAP__*)SelectObject(hMemDC, CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&rgb, 0, 0));
	SetBkMode(hMemDC, TRANSPARENT);

#if 0
/*	container=(int*)malloc(rgbn*sizeof(int));
//	container.resize(rgbn);
	for(int k=0;k<rgbn;++k)//buffer, vector
	{
		auto p=(unsigned char*)&container[k];
		p[0]=rand(), p[1]=rand(), p[2]=rand();
	}//*/
/*	container.clear();
	for(int k=0;k<rgbn;++k)//list
	{
		int px;
		auto p=(unsigned char*)&px;
		p[0]=rand(), p[1]=rand(), p[2]=rand();
		container.push_back(px);
	}//*/
/*	container.clear();
	for(int k=0;k<rgbn;++k)//set
	{
		int px;
		auto p=(unsigned char*)&px;
		do
			p[0]=rand(), p[1]=rand(), p[2]=rand();
		while(!container.insert(px).second);
	}//*/
/*	container.clear();
	for(int k=0;k<rgbn;++k)//map
	{
		std::pair<int, int> px;
		auto p=(unsigned char*)&px.second;
		p[0]=rand(), p[1]=rand(), p[2]=rand();
		p=(unsigned char*)&px.first;
		do
			p[0]=rand(), p[1]=rand(), p[2]=rand();
		while(!container.insert(px).second);
	}//*/
#endif
}
void				LinearSW::resize()
{
	rgbn=h*w;
	wbuffer=(double*)realloc(wbuffer, rgbn*sizeof(double)), libuffer=(int*)realloc(libuffer, h*sizeof(int)), lfbuffer=(int*)realloc(lfbuffer, h*sizeof(int));
	DeleteObject((HBITMAP__*)SelectObject(hMemDC, hBitmap));
//	rgb_r=(double*)realloc(rgb_r, w*h*sizeof(double)), rgb_g=(double*)realloc(rgb_g, w*h*sizeof(double)), rgb_b=(double*)realloc(rgb_b, w*h*sizeof(double)), rgb_n=(int*)realloc(rgb_n, w*h*sizeof(int));
//	rgb_r=(int*)realloc(rgb_r, w*h*sizeof(int)), rgb_g=(int*)realloc(rgb_g, w*h*sizeof(int)), rgb_b=(int*)realloc(rgb_b, w*h*sizeof(int)), rgb_n=(int*)realloc(rgb_n, w*h*sizeof(int));
	tagBITMAPINFO bmpInfo={{sizeof(tagBITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
	hBitmap=(HBITMAP__*)SelectObject(hMemDC, CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&rgb, 0, 0));
	SetBkMode(hMemDC, TRANSPARENT);

#if 0
/*	container=(int*)realloc(container, rgbn*sizeof(int));
//	container.resize(rgbn);
	for(int k=0;k<rgbn;++k)//buffer, vector
	{
		auto p=(unsigned char*)&container[k];
		p[0]=rand(), p[1]=rand(), p[2]=rand();
	}//*/
/*	container.clear();
	for(int k=0;k<rgbn;++k)//list
	{
		int px;
		auto p=(unsigned char*)&px;
		p[0]=rand(), p[1]=rand(), p[2]=rand();
		container.push_back(px);
	}//*/
/*	container.clear();
	for(int k=0;k<rgbn;++k)//set
	{
		int px;
		auto p=(unsigned char*)&px;
		do
			p[0]=rand(), p[1]=rand(), p[2]=rand();
		while(!container.insert(px).second);
	}//*/
/*	container.clear();
	for(int k=0;k<rgbn;++k)//map
	{
		std::pair<int, int> px;
		auto p=(unsigned char*)&px.second;
		p[0]=rand(), p[1]=rand(), p[2]=rand();
		p=(unsigned char*)&px.first;
		do
			p[0]=rand(), p[1]=rand(), p[2]=rand();
		while(!container.insert(px).second);
	}//*/
#endif
}
void				LinearSW::changeFov(){}
void				LinearSW::render()
{
//#define LSW_LOOP_FRAME
#ifdef LSW_LOOP_FRAME
	for(;;){
#endif
	memset(rgb, 0xFF, rgbn*sizeof(int)), memset(wbuffer, 0, rgbn*sizeof(double));
#if 1
	double dx, dy, dz, cpt, v1[5], v2[5], v3[5], admittance;//*v: Xs, Ys, Xcp, Ycp, Zcp
	for(int k=0;k<npols;++k)
	{
		dx=env[k].x1-camx, dy=env[k].y1-camy, dz=env[k].z1-camz, cpt=dx*cax+dy*sax, v1[2]=dx*sax-dy*cax, v1[3]=cpt*say-dz*cay, v1[4]=cpt*cay+dz*say, cpt=v1[4]*tanfov/X0, v1[0]=X0+v1[2]/cpt, v1[1]=Y0+v1[3]/cpt;
		dx=env[k].x2-camx, dy=env[k].y2-camy, dz=env[k].z2-camz, cpt=dx*cax+dy*sax, v2[2]=dx*sax-dy*cax, v2[3]=cpt*say-dz*cay, v2[4]=cpt*cay+dz*say, cpt=v2[4]*tanfov/X0, v2[0]=X0+v2[2]/cpt, v2[1]=Y0+v2[3]/cpt;
		dx=env[k].x3-camx, dy=env[k].y3-camy, dz=env[k].z3-camz, cpt=dx*cax+dy*sax, v3[2]=dx*sax-dy*cax, v3[3]=cpt*say-dz*cay, v3[4]=cpt*cay+dz*say, cpt=v3[4]*tanfov/X0, v3[0]=X0+v3[2]/cpt, v3[1]=Y0+v3[3]/cpt;
			 if(v1[4]<0){		 if(v2[4]<0){		 if(v3[4]<0)	continue;
												else				draft_2behind(v3, v1, v2);}
							else{					 if(v3[4]<0)	draft_2behind(v2, v3, v1);
												else				draft_1behind(v2, v3, v1);}}
		else{					 if(v2[4]<0){		 if(v3[4]<0)	draft_2behind(v1, v2, v3);
												else				draft_1behind(v3, v1, v2);}
							else{					 if(v3[4]<0)	draft_1behind(v1, v2, v3);
												else				draft_start(v1, v2), draft(v2, v3), draft(v3, v1);}}
		double upy_x=v2[2]-v1[2],			upy_y=v2[3]-v1[3],			upy_z=v2[4]-v1[4];									//u12	=<12>
		double upx_x=v3[2]-v1[2],			upx_y=v3[3]-v1[3],			upx_z=v3[4]-v1[4];									//ux3	=<13>
		double a=upy_y*upx_z-upx_y*upy_z,	b=upx_x*upy_z-upy_x*upx_z,	c=upy_x*upx_y-upx_x*upy_y;							//abc	=<n>	=<12>x<13>
		double t=a*v1[2]+b*v1[3]+c*v1[4];
		if(!t)continue;
		double B7=a, B8=b, B9=c*X0/tanfov-a*X0-b*Y0;	cpt=t*X0/tanfov;
		double A1=B7/cpt, A2=B8/cpt, A3=B9/cpt;
		if(env[k].tx>=0)
		{
			double	ux3_1=sqrt(upy_x*upy_x+upy_y*upy_y+upy_z*upy_z);	upy_x/=ux3_1,		upy_y/=ux3_1,		upy_z/=ux3_1;		//u12	=<u12>	=<12>/|12|
					ux3_1=upx_x*upy_x+upx_y*upy_y+upx_z*upy_z,			upx_x-=ux3_1*upy_x,	upx_y-=ux3_1*upy_y,	upx_z-=ux3_1*upy_z;	//ux3	=<x3>	=<13>-<13>.<u12><u12>
					ux3_1=sqrt(upx_x*upx_x+upx_y*upx_y+upx_z*upx_z),	upx_x/=ux3_1,		upx_y/=ux3_1,		upx_z/=ux3_1;		//ux3	=<ux3>	=<x3>/|x3|
					ux3_1=upx_x*v1[2]+upx_y*v1[3]+upx_z*v1[4];																	//ux3_1			=<ux3>.<1>
			double	u12_1=upy_x*v1[2]+upy_y*v1[3]+upy_z*v1[4];																	//u12_1			=<u12>.<1>
			double	C1=upx_x*env[k].txXX+upy_x*env[k].txXY, C2=upx_y*env[k].txXX+upy_y*env[k].txXY, C3=upx_z*env[k].txXX+upy_z*env[k].txXY;
			double	C4=upx_x*env[k].txYX+upy_x*env[k].txYY, C5=upx_y*env[k].txYX+upy_y*env[k].txYY, C6=upx_z*env[k].txYX+upy_z*env[k].txYY;
			double	D1=C1*t, D2=C2*t, D3=(C3*X0/tanfov-C1*X0-C2*Y0)*t;
			double	D4=C4*t, D5=C5*t, D6=(C6*X0/tanfov-C4*X0-C5*Y0)*t;
			double	E1=env[k].tx1x-ux3_1*env[k].txXX-u12_1*env[k].txXY;
			double	E2=env[k].tx1y-ux3_1*env[k].txYX-u12_1*env[k].txYY;
			double	B1=D1+E1*B7, B2=D2+E1*B8, B3=D3+E1*B9;
			double	B4=D4+E2*B7, B5=D5+E2*B8, B6=D6+E2*B9;
			double *wb_y=wbuffer;
			int lib_y, lfb_y, *rgb_y=rgb, *txk=texture[env[k].tx], txhk=txh[env[k].tx], txwk=txw[env[k].tx];
			for(int ys=0;ys<h;++ys)
			{
				lib_y=libuffer[ys]<0?0:libuffer[ys], lfb_y=lfbuffer[ys]>w?w:lfbuffer[ys];
				admittance=A1*lib_y+A2*ys+A3, a=B1*lib_y+B2*ys+B3, b=B4*lib_y+B5*ys+B6, c=B7*lib_y+B8*ys+B9;
				for(int xs=lib_y;xs<lfb_y;++xs)
				{
					if(std::abs(admittance-wb_y[xs])<1e-10*std::abs(admittance+wb_y[xs]))//z fighting
					{
						int Xtx=int(a/c)%txwk;Xtx+=txwk&-(Xtx<0);//if(Xtx<0)Xtx+=txwk;
						int Ytx=int(b/c)%txhk;Ytx+=txhk&-(Ytx<0);//if(Ytx<0)Ytx+=txhk;
						auto ps=(unsigned char*)&rgb_y[xs], pt=(unsigned char*)&txk[Xtx+Ytx*txwk];
						ps[0]=(ps[0]+pt[0])>>1;//b	warning C4554
						ps[1]=(ps[1]+pt[1])>>1;//g
						ps[2]=(ps[2]+pt[2])>>1;//r
					}
					else if(admittance>wb_y[xs])
					{
						wb_y[xs]=admittance;
						int Xtx=int(a/c)%txwk;Xtx+=txwk&-(Xtx<0);//if(Xtx<0)Xtx+=txwk;
						int Ytx=int(b/c)%txhk;Ytx+=txhk&-(Ytx<0);//if(Ytx<0)Ytx+=txhk;
						rgb_y[xs]=txk[Xtx+Ytx*txwk];
					}
					admittance+=A1, a+=B1, b+=B4, c+=B7;
				}
				wb_y=wb_y+w, rgb_y=rgb_y+w;
			}
		}
		else
		{
			double *wb_y=wbuffer;
			int lib_y, lfb_y, *rgb_y=rgb, color=env[k].color;
			for(int ys=0;ys<h;++ys)
			{
				lib_y=libuffer[ys]<0?0:libuffer[ys], lfb_y=lfbuffer[ys]>w?w:lfbuffer[ys];
				admittance=A1*lib_y+A2*ys+A3;
				for(int xs=lib_y;xs<lfb_y;++xs)
				{
					if(std::abs(admittance-wb_y[xs])<1e-10*std::abs(admittance+wb_y[xs]))//z fighting
					{
						auto p=(unsigned char*)&rgb_y[xs], c=(unsigned char*)&color;
						p[0]=(p[0]+c[0])>>1;//b		warning C4554
						p[1]=(p[1]+c[1])>>1;//g
						p[2]=(p[2]+c[2])>>1;//r
					}
					else if(admittance>wb_y[xs])
						wb_y[xs]=admittance, rgb_y[xs]=color;
					admittance+=A1;
				}
				wb_y=wb_y+w, rgb_y=rgb_y+w;
			}
		}
	}
	for(int k=0;k<ntpols;++k)
	{
		dx=tenv[k].x1-camx, dy=tenv[k].y1-camy, dz=tenv[k].z1-camz, cpt=dx*cax+dy*sax, v1[2]=dx*sax-dy*cax, v1[3]=cpt*say-dz*cay, v1[4]=cpt*cay+dz*say, cpt=v1[4]*tanfov/X0, v1[0]=X0+v1[2]/cpt, v1[1]=Y0+v1[3]/cpt;
		dx=tenv[k].x2-camx, dy=tenv[k].y2-camy, dz=tenv[k].z2-camz, cpt=dx*cax+dy*sax, v2[2]=dx*sax-dy*cax, v2[3]=cpt*say-dz*cay, v2[4]=cpt*cay+dz*say, cpt=v2[4]*tanfov/X0, v2[0]=X0+v2[2]/cpt, v2[1]=Y0+v2[3]/cpt;
		dx=tenv[k].x3-camx, dy=tenv[k].y3-camy, dz=tenv[k].z3-camz, cpt=dx*cax+dy*sax, v3[2]=dx*sax-dy*cax, v3[3]=cpt*say-dz*cay, v3[4]=cpt*cay+dz*say, cpt=v3[4]*tanfov/X0, v3[0]=X0+v3[2]/cpt, v3[1]=Y0+v3[3]/cpt;
			 if(v1[4]<0){		 if(v2[4]<0){		 if(v3[4]<0)	continue;
												else				draft_2behind(v3, v1, v2);}
							else{					 if(v3[4]<0)	draft_2behind(v2, v3, v1);
												else				draft_1behind(v2, v3, v1);}}
		else{					 if(v2[4]<0){		 if(v3[4]<0)	draft_2behind(v1, v2, v3);
												else				draft_1behind(v3, v1, v2);}
							else{					 if(v3[4]<0)	draft_1behind(v1, v2, v3);
												else				draft_start(v1, v2), draft(v2, v3), draft(v3, v1);}}
		double upy_x=v2[2]-v1[2],			upy_y=v2[3]-v1[3],			upy_z=v2[4]-v1[4];									//u12	=<12>
		double upx_x=v3[2]-v1[2],			upx_y=v3[3]-v1[3],			upx_z=v3[4]-v1[4];									//ux3	=<13>
		double a=upy_y*upx_z-upx_y*upy_z,	b=upx_x*upy_z-upy_x*upx_z,	c=upy_x*upx_y-upx_x*upy_y;							//abc	=<n>	=<12>x<13>
		double t=a*v1[2]+b*v1[3]+c*v1[4];
		if(!t)continue;
		double B7=a, B8=b, B9=c*X0/tanfov-a*X0-b*Y0;	cpt=t*X0/tanfov;
		double A1=B7/cpt, A2=B8/cpt, A3=B9/cpt;
		if(tenv[k].tx>=0)
		{
			double	ux3_1=sqrt(upy_x*upy_x+upy_y*upy_y+upy_z*upy_z);	upy_x/=ux3_1,		upy_y/=ux3_1,		upy_z/=ux3_1;		//u12	=<u12>	=<12>/|12|
					ux3_1=upx_x*upy_x+upx_y*upy_y+upx_z*upy_z,			upx_x-=ux3_1*upy_x,	upx_y-=ux3_1*upy_y,	upx_z-=ux3_1*upy_z;	//ux3	=<x3>	=<13>-<u12>.<13><u12>
					ux3_1=sqrt(upx_x*upx_x+upx_y*upx_y+upx_z*upx_z),	upx_x/=ux3_1,		upx_y/=ux3_1,		upx_z/=ux3_1;		//ux3	=<ux3>	=<x3>/|x3|
					ux3_1=upx_x*v1[2]+upx_y*v1[3]+upx_z*v1[4];																		//ux3_1			=<ux3>.<1>
			double	u12_1=upy_x*v1[2]+upy_y*v1[3]+upy_z*v1[4];																		//u12_1			=<u12>.<1>
			double	C1=upx_x*tenv[k].txXX+upy_x*tenv[k].txXY, C2=upx_y*tenv[k].txXX+upy_y*tenv[k].txXY, C3=upx_z*tenv[k].txXX+upy_z*tenv[k].txXY;
			double	C4=upx_x*tenv[k].txYX+upy_x*tenv[k].txYY, C5=upx_y*tenv[k].txYX+upy_y*tenv[k].txYY, C6=upx_z*tenv[k].txYX+upy_z*tenv[k].txYY;
			double	D1=C1*t, D2=C2*t, D3=(C3*X0/tanfov-C1*X0-C2*Y0)*t;
			double	D4=C4*t, D5=C5*t, D6=(C6*X0/tanfov-C4*X0-C5*Y0)*t;
			double	E1=tenv[k].tx1x-ux3_1*tenv[k].txXX-u12_1*tenv[k].txXY;
			double	E2=tenv[k].tx1y-ux3_1*tenv[k].txYX-u12_1*tenv[k].txYY;
			double	B1=D1+E1*B7, B2=D2+E1*B8, B3=D3+E1*B9;
			double	B4=D4+E2*B7, B5=D5+E2*B8, B6=D6+E2*B9;
			double *wb_y=wbuffer;
			int lib_y, lfb_y, *rgb_y=rgb, *txk=texture[tenv[k].tx], txhk=txh[tenv[k].tx], txwk=txw[tenv[k].tx];
			for(int ys=0;ys<h;++ys)
			{
				lib_y=libuffer[ys]<0?0:libuffer[ys], lfb_y=lfbuffer[ys]>w?w:lfbuffer[ys];
				admittance=A1*lib_y+A2*ys+A3, a=B1*lib_y+B2*ys+B3, b=B4*lib_y+B5*ys+B6, c=B7*lib_y+B8*ys+B9;
				for(int xs=lib_y;xs<lfb_y;++xs)
				{
					if(std::abs(admittance-wb_y[xs])<1e-10*std::abs(admittance+wb_y[xs])||//z fighting
						admittance>wb_y[xs])
					{
						int Xtx=int(a/c)%txwk;Xtx+=txwk&-(Xtx<0);//if(Xtx<0)Xtx+=txwk;
						int Ytx=int(b/c)%txhk;Ytx+=txhk&-(Ytx<0);//if(Ytx<0)Ytx+=txhk;
						auto p=(unsigned char*)&rgb_y[xs], c=(unsigned char*)&txk[Xtx+Ytx*txwk];
						p[0]=p[0]*c[0]>>8;//b
						p[1]=p[1]*c[1]>>8;//g
						p[2]=p[2]*c[2]>>8;//r
					}
					admittance+=A1, a+=B1, b+=B4, c+=B7;
				}
				wb_y=wb_y+w, rgb_y=rgb_y+w;
			}
		}
		else
		{
			double *wb_y=wbuffer;
			int lib_y, lfb_y, *rgb_y=rgb, color=tenv[k].color;
			for(int ys=0;ys<h;++ys)
			{
				lib_y=libuffer[ys]<0?0:libuffer[ys], lfb_y=lfbuffer[ys]>w?w:lfbuffer[ys];
				admittance=A1*lib_y+A2*ys+A3;
				for(int xs=lib_y;xs<lfb_y;++xs)
				{
					if(std::abs(admittance-wb_y[xs])<1e-10*std::abs(admittance+wb_y[xs])||//z fighting
						admittance>=wb_y[xs])
					{
						auto p=(unsigned char*)&rgb_y[xs], c=(unsigned char*)&color;
						p[0]=p[0]*c[0]>>8;//b
						p[1]=p[1]*c[1]>>8;//g
						p[2]=p[2]*c[2]>>8;//r
					}
					admittance+=A1;
				}
				wb_y=wb_y+w, rgb_y=rgb_y+w;
			}
		}
	}
#endif
	
	//distance function
#if 0//curve to horisontal plane signed distance function
	if(w&&h)
	{
		int Xs, Ys;
#ifdef AA_MOUSEMOVE_TRIGGER
		if(drag)
			Xs=w/2, Ys=h/2;
		else
		{
			POINT m;
			GetCursorPos(&m);
			ScreenToClient(ghWnd, &m);
			Xs=m.x, Ys=m.y;
		}
#else
		Xs=w/2, Ys=h/2;
#endif
		GUIPrint(hMemDC, 0, 20, Xs), GUIPrint(hMemDC, 100, 20, Ys);
	/*	int Xs, Ys;
		double Xn, Yn;
#ifdef AA_MOUSEMOVE_TRIGGER
		if(drag)
			Xs=w/2, Ys=h/2;
		else
		{
			POINT m;
			GetCursorPos(&m);
			ScreenToClient(ghWnd, &m);
			Xs=m.x, Ys=m.y;
		}
		Xn=(Xs-X0)*tanfov/X0, Yn=(Ys-Y0)*tanfov/X0;
#else
		Xs=w/2, Ys=h/2;
		Xn=0, Yn=0;
#endif
		GUIPrint(hMemDC, 0, 20, "%f", Xn), GUIPrint(hMemDC, 100, 20, "%f", Yn);//*/
		GUIPrint(hMemDC, 0, 40, "cam (%g, %g, %g) %g, %g", camx, camy, camz, ax*360/TWOPI, ay*360/TWOPI);

		MoveToEx(hMemDC, 0, Ys, 0), LineTo(hMemDC, w, Ys);
		
		auto hOriginal=SelectObject(hMemDC, GetStockObject(DC_PEN));
		SetDCPenColor(hMemDC, 0xB0B0B0);
		double const Ygain=0.1;//X0/tanfov;
		double const
			pi=acos(-1.),
			saxsay=sax*say, camzcay=camz*cay,
			caxsay=cax*say,
			caxcay=cax*cay, saxcay=sax*cay, camzsay=camz*say;
		double Yn=tanfov*(Ys-Y0)/X0;
		double a=cax*say-cax*cay*Yn, b=sax*say-sax*cay*Yn, c=-cay-say*Yn;
	//	double a=-cax, b=sax*say+cay*tanfov*(Ys-Y0)/X0, c=sax*cay-say*tanfov*(Ys-Y0)/X0;
		double mag=sqrt(a*a+b*b+c*c);
		a/=mag, b/=mag, c/=mag;//<a,b,c> unit normal to horizontal plane y=Ys
		GUIPrint(hMemDC, 0, 80, "n (%g, %g, %g)", 0., 1., -tanfov*(Ys-Y0)/X0);
		GUIPrint(hMemDC, 0, 100, "n (%g, %g, %g) %g, %g", a, b, c, atan2(b, a)*360/TWOPI, atan2(c, sqrt(a*a+b*b))*360/TWOPI);
		_3d::line(0, 0, 0, a, b, c, 0, rgb, wbuffer);
		_3d::line(0, 0, 0, a, 0, 0, 0, rgb, wbuffer);
		_3d::line(0, 0, 0, 0, b, 0, 0, rgb, wbuffer);
		_3d::line(0, 0, 0, 0, 0, c, 0, rgb, wbuffer);

		_3d::line(0, b, c, a, b, c, 0, rgb, wbuffer);
		_3d::line(a, 0, c, a, b, c, 0, rgb, wbuffer);
		_3d::line(a, b, 0, a, b, c, 0, rgb, wbuffer);

	//	_3d::line(0+1, 0, 0, a+1, b, c, 0, rgb, wbuffer);
	//	_3d::line(0, 0+1, 0, a, b+1, c, 0, rgb, wbuffer);
	//	_3d::line(0, 0, 0+1, a, b, c+1, 0, rgb, wbuffer);
		double d=a*camx+b*camy+c*camz;
	//	double a=-cax, b=sax*say+cay*tanfov*(Ys-Y0)/X0, c=sax*cay-say*tanfov*(Ys-Y0)/X0, d=a*camx+b*camy+c*camz;//	X
		auto map_spread_x=[&](int x){return 10*tan(.5*pi*(double(x)/(w-1)*2-1));};
		auto map_compress_y=[&](double y)
		{
			return int((tanh(Ygain*y)+1)/2*h);
		//	return int(h-(tanh(Ygain*y)+1)/2*h);
		//	double y1=Ygain*y;
		//	return int(h+((y1<1?y1-1:tanh(log(y1)))+1)/2*h);
		};
		auto evaluate=[&](int x)
		{
			double X=map_spread_x(x);
		//	double reF=0, imF=0;
		//	double reF=X, imF=0;
			double reF=std::cos(X), imF=0;
			double sd=a*X+b*reF+c*imF-d;//signed distance
		/*	double X=map_spread_x(x), refX=std::cos(X);
			double Xcp, Ycp, Zcp;
			{
				double dx=X-camx, dy=refX-camy;
				Xcp=dx*sax-dy*cax;
				Ycp=dx*caxsay+dy*saxsay+camzcay;
				Zcp=dx*caxcay+dy*saxcay-camzsay; 
			}
			double dXn=Xcp/Zcp-Xn, dYn=Ycp/Zcp-Yn;
			double d=std::sqrt(dXn*dXn+dYn*dYn);//*/
			return map_compress_y(sd);
		};
	/*	auto evaluate_min=[&](int x)
		{
			double const step=.01;
			int y_min=evaluate(x);
			for(double x1=x+step, x1end=x+1+step/2;x1<x1end;x1+=step)//minimum in 100 samples
			{
				int y=evaluate(x1);
				if(y_min>y)
					y_min=y;
			}
			return y_min;
		};//*/
		for(int x=0, y=evaluate(x), xEnd=w-1;x<xEnd;)
		{
			MoveToEx(hMemDC, x, y, 0);
			++x, y=evaluate(x);
		//	++x, y=evaluate_min(x);
			LineTo(hMemDC, x, y);
		}
		int y0=h/2;//==map_compress_y(0);
		MoveToEx(hMemDC, 0, y0, 0), LineTo(hMemDC, w, y0);		//level/something is wrong
	//	int y=map_compress_y(0);
	//	MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);//debug: y==0 from code above
	//	MoveToEx(hMemDC, 0, h/2, 0), LineTo(hMemDC, w, h/2);
	//	int y=map_compress_y(tanfov/X0);
	//	MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);
		SelectObject(hMemDC, hOriginal);
	}
#endif
#if 0//curve to pixel magnitude distance function
/*	if(w&&h)
	{
		double Xn, Yn;
		{
			int Xs, Ys;
			if(drag)
				Xs=w/2, Ys=h/2;
			else
			{
				POINT m;
				GetCursorPos(&m);
				ScreenToClient(ghWnd, &m);
				Xs=m.x, Ys=m.y;
			}
			GUIPrint(hMemDC, 0, 0, Xs), GUIPrint(hMemDC, 100, 0, Ys);
			Xn=(Xs-X0)*tanfov/X0, Yn=(Ys-Y0)*tanfov/X0;
		}
		GUIPrint(hMemDC, 0, 20, "%f", Xn), GUIPrint(hMemDC, 100, 20, "%f", Yn);
		int yStart=h, yEnd=0;
		MoveToEx(hMemDC, 0, yStart, 0), LineTo(hMemDC, w, yStart);
		double const Ygain=X0/tanfov;
		double const
			pi=acos(-1.),
			saxsay=sax*say, camzcay=camz*cay,
			caxsay=cax*say,
			caxcay=cax*cay, saxcay=sax*cay, camzsay=camz*say;
		auto map_spread_x=[&](int x){return 10*tan(.5*pi*(double(x)/(w-1)*2-1));};
		auto map_compress_y=[&](double y)
		{
			double y1=Ygain*y;
			return int(yStart+((y1<1?y1-1:tanh(log(y1)))+1)/2*(yEnd-yStart));
		};
		auto eval2=[&](double x)
		{
			double X=map_spread_x(x), refX=std::cos(X);
			double Xcp, Ycp, Zcp;
			{
				double dx=X-camx, dy=refX-camy;
				Xcp=dx*sax-dy*cax;
				Ycp=dx*caxsay+dy*saxsay+camzcay;
				Zcp=dx*caxcay+dy*saxcay-camzsay; 
			}
			double Xn_Xn=Xcp/Zcp-Xn, Yn_Yn=Ycp/Zcp-Yn;
			double d=std::sqrt(Xn_Xn*Xn_Xn+Yn_Yn*Yn_Yn);
			return map_compress_y(d);
		};
		auto evaluate=[&](int x)
		{
			double const step=.01;
			int y=eval2(x);
			for(double x1=x+step, x1end=x+1+step/2;x1<x1end;x1+=step)
			{
				int y2=eval2(x1);
				if(y>y2)
					y=y2;
			}
			return y;
		};
		MoveToEx(hMemDC, 0, evaluate(0), 0);
		for(int x=1;x<w-1;++x)
		{
			int y=evaluate(x);
			LineTo(hMemDC, x, y);
			MoveToEx(hMemDC, x, y, 0);
		}
		LineTo(hMemDC, w-1, evaluate(w-1));
		int y=map_compress_y(tanfov/X0);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);
	}//*/
	if(w&&h)
	{
		double Xn, Yn;
#ifdef AA_MOUSEMOVE_TRIGGER
		{
			int Xs, Ys;
			if(drag)
				Xs=w/2, Ys=h/2;
			else
			{
				POINT m;
				GetCursorPos(&m);
				ScreenToClient(ghWnd, &m);
				Xs=m.x, Ys=m.y;
			}
			GUIPrint(hMemDC, 0, 0, Xs), GUIPrint(hMemDC, 100, 0, Ys);
			Xn=(Xs-X0)*tanfov/X0, Yn=(Ys-Y0)*tanfov/X0;
		}
#else
		Xn=0, Yn=0;
#endif
		GUIPrint(hMemDC, 0, 20, "%f", Xn), GUIPrint(hMemDC, 100, 20, "%f", Yn);
		
		int yStart=h, yEnd=0;
	//	int yStart=h-20, yEnd=0;
		MoveToEx(hMemDC, 0, yStart, 0), LineTo(hMemDC, w, yStart);
		double const Ygain=X0/tanfov;
		double const
			pi=acos(-1.),
			saxsay=sax*say, camzcay=camz*cay,
			caxsay=cax*say,
			caxcay=cax*cay, saxcay=sax*cay, camzsay=camz*say;
	//	auto map_spread_x=[&](int x){return 25*atanh(double(x)/(w-1)*2-1);};
	//	auto map_spread_x=[&](int x){return 2*tan(.5*pi*(double(x)/(w-1)*2-1));};
		auto map_spread_x=[&](int x){return 10*tan(.5*pi*(double(x)/(w-1)*2-1));};
		auto map_compress_y=[&](double y)
		{
			double y1=Ygain*y;
			return int(yStart+((y1<1?y1-1:tanh(log(y1)))+1)/2*(yEnd-yStart));
		};
	//	auto map_compress_y=[&](double y){return int(yStart+(tanh(log(Ygain*y))+1)/2*(yEnd-yStart));};
	//	auto map_compress_y=[&](double y){return int(yStart+tanh(y)*(yEnd-yStart));};
	//	auto map_compress_y=[&](double y){return int(yStart+atan(200*y)*2/pi*(yEnd-yStart));};
	//	auto map_compress_y=[&](double y){return int(yStart+(atan(y)*2/pi+1)/2*(yEnd-yStart));};
		auto eval2=[&](double x)
		{
			double X=map_spread_x(x), refX=std::cos(X);
			double Xcp, Ycp, Zcp;
			{
				double dx=X-camx, dy=refX-camy;
				Xcp=dx*sax-dy*cax;
				Ycp=dx*caxsay+dy*saxsay+camzcay;
				Zcp=dx*caxcay+dy*saxcay-camzsay; 
			}
			double Xn_Xn=Xcp/Zcp-Xn, Yn_Yn=Ycp/Zcp-Yn;
			double d=std::sqrt(Xn_Xn*Xn_Xn+Yn_Yn*Yn_Yn);
			return map_compress_y(d);
		};
		auto evaluate=[&](int x)
		{
			double const step=.01;
			int y=eval2(x);
			for(double x1=x+step, x1end=x+1+step/2;x1<x1end;x1+=step)
			{
				int y2=eval2(x1);
				if(y>y2)
					y=y2;
			}
			return y;

		//	int y=eval2(x), y2=eval2(x1);
		//	return y<y2?y:y2;

		/*	double X=map_spread_x(x), refX=std::cos(X);//std::tan(X);
		//	double X=tan(.5*pi*(double(x)/(w-1)*2-1)), refX=std::cos(X);
			double Xcp, Ycp, Zcp;
			{
				double dx=X-camx, dy=refX-camy;
				Xcp=dx*sax-dy*cax;
				Ycp=dx*caxsay+dy*saxsay+camzcay;
				Zcp=dx*caxcay+dy*saxcay-camzsay; 
			}
			double Xn_Xn=Xcp/Zcp-Xn, Yn_Yn=Ycp/Zcp-Yn;
			double d=std::sqrt(Xn_Xn*Xn_Xn+Yn_Yn*Yn_Yn);
			return map_compress_y(d);
		//	return int(yStart+(atan(d)*2/pi+1)*(yEnd-yStart)/2);//*/
		};
	/*	double Xstart=0, Xend=70, Ystart=0, Yend=70;
		int yStart=h-20, yEnd=0;
		MoveToEx(hMemDC, 0, yStart, 0), LineTo(hMemDC, w, yStart);
		double Xs=(Xend-Xstart)/w, Ys=(yEnd-yStart)/(Yend-Ystart);
			double const
				pi=acos(-1.),
				saxsay=sax*say, camzcay=camz*cay,
				caxsay=cax*say,
				caxcay=cax*cay, saxcay=sax*cay, camzsay=camz*say;
		auto evaluate=[&](int x)
		{
			double X=Xstart+Xs*x, cosX=std::cos(X);
			double Xcp, Ycp, Zcp;
			{
				double X_camx=X-camx, cosX_camy=cosX-camy;
				Xcp=X_camx*sax-cosX_camy*cax;
				Ycp=X_camx*caxsay+cosX_camy*saxsay+camzcay;
				Zcp=X_camx*caxcay+cosX_camy*saxcay-camzsay;
			}
			double Xn_Xn=Xcp/Zcp-Xn, Yn_Yn=Ycp/Zcp-Yn;
			double d=std::sqrt(Xn_Xn*Xn_Xn+Yn_Yn*Yn_Yn);
			return int(yStart+d*Ys);
		//	return std::sqrt(Xn_Xn*Xn_Xn+Yn_Yn*Yn_Yn);

		//	double
		//		Xcp=(X-camx)*sax-(cosX-camy)*cax,
		//		Ycp=(X-camx)*caxsay+(cosX-camy)*saxsay+camzcay,
		//		Zcp=(X-camx)*caxcay+(cosX-camy)*saxcay-camzsay;

		//	double
		//		Xcp=(X-camx)*sax-(cosX-camy)*cax,
		//		Ycp=(X-camx)*cax*say+(cosX-camy)*sax*say+camz*cay,
		//		Zcp=(X-camx)*cax*cay+(cosX-camy)*sax*cay-camz*say;
		//	return std::sqrt((Xcp/Zcp-Xn)*(Xcp/Zcp-Xn)+(Ycp/Zcp-Yn)*(Ycp/Zcp-Yn));
		};//*/

		MoveToEx(hMemDC, 0, evaluate(0), 0);
		for(int x=1;x<w-1;++x)
		{
			int y=evaluate(x);
			LineTo(hMemDC, x, y);
			MoveToEx(hMemDC, x, y, 0);
		}
		LineTo(hMemDC, w-1, evaluate(w-1));
		int y=map_compress_y(tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

	/*	y=map_compress_y(.9*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

		y=map_compress_y(.8*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

		y=map_compress_y(.7*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

		y=map_compress_y(.6*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

		y=map_compress_y(.5*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

		y=map_compress_y(.4*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

		y=map_compress_y(.3*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

		y=map_compress_y(.2*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);

		y=map_compress_y(.1*tanfov/X0);
	//	int y=int(yStart+(atan(tanfov/X0)*2/pi+1)*(yEnd-yStart)/2);
		MoveToEx(hMemDC, 0, y, 0), LineTo(hMemDC, w, y);//*/
	}
#endif
	
	//3d curve
#if 0//draw 3d cos		aa		samples coincide with pixels, 1 aa loop			from g2 c2d contour
	if(w>=1&&h>=1)
	{
		//XY-plane		<u> crosses xy-plane
		int w1=w+1, h1=h+1, w2=w+2, h2=h+2, w3=w+3,
			rgbn12=w1*h2, rgbn21=w2*h1, rgbn22=w2*h2;
		std::vector<char> hit(rgbn22);
		std::vector<double> hitX(rgbn22), hitY(rgbn22), hitLR(rgbn22);//where pixel rays hit the x-y plane	x, y-f
		double t=-camz;
		double ttfov=t*tanfov, tfov_x0=tanfov/X0, ttfov_x0=t*tfov_x0, y0_x0=double(Y0)/X0;
		double A2=-tfov_x0*cay/t, A3=(tanfov*y0_x0*cay+say)/t;
		double	B1=0,					B2=-tfov_x0*cay,				B3=tanfov*y0_x0*cay+say,
				B4=ttfov_x0*sax,		B5=ttfov_x0*cax*say+B2*camx,	B6=t*(-tanfov*(sax+y0_x0*cax*say)+cax*cay)+B3*camx,
				B7=-ttfov_x0*cax,		B8=ttfov_x0*sax*say+B2*camy,	B9=t*( tanfov*(cax-y0_x0*sax*say)+sax*cay)+B3*camy;
		
		double _pi=::acos(-1.);
		auto _1d_int_in_range=[](double x0, double x1){return std::floor(x0)!=std::floor(x1)||std::ceil(x0)!=std::ceil(x1);};
		auto disc=[&](double x0, double x1)
		{
		//	return std::abs(x1-x0)>3.2||_1d_int_in_range(x0/_pi-.5, x1/_pi-.5);		//tan x
			return false;															//cos x
		};

		for(int ys=-1;ys<h1;++ys)
		{
			int hp=w2*(ys+1);
			if(A2*ys+A3>0)//_1_Zcp>0
			{
				double den=B2*ys+B3;//B1=0
				double B56=B5*ys+B6, B89=B8*ys+B9;
				for(int xs=-1;xs<w1;++xs, ++hp)
				{
					hit[hp]=1;
					hitX[hp]=(B4*xs+B56)/den,
				//	hitLR[hp]=(B7*xs+B89)/den - std::cos(hitX[hp]);
				//	hitY[hp]=(B7*xs+B89)/den;

				//	hitF[hp]=0;
				//	hitF[hp]=hitX[hp];
				//	hitF[hp]=std::tan(hitX[hp]);
				//	hitF[hp]=100*std::cos(10*hitX[hp]/100)*std::cos(hitX[hp]/100)+cos(hitX[p]);
				//	hitF[hp]=std::cos(10*hitX[p])*std::cos(hitX[p]);
				//	hitF[hp]=std::cos(hitX[p]*hitX[hp])*std::cos(hitX[hp]);
				//	hitF[hp]=hitX[hp]*std::cos(hitX[hp]);
				//	hitF[hp]=std::sin(1/hitX[hp]);
				//	hitF[hp]=1/std::sin(hitX[hp]);
				//	hitF[hp]=std::sqrt(std::abs(hitX[hp]));
				//	hitF[hp]=std::tan(hitX[hp]);
				//	hitF[hp]=std::exp(hitX[hp]);
				//	hitF[hp]=std::cos(hitX[hp]);

					hitLR[hp] = (hitY[hp]=(B7*xs+B89)/den) - std::cos(hitX[hp]);
				}
			}
		}
		
		const double aa_thickness=1,
				
			_1_aa_thickness=1/aa_thickness;
		const int aa_bound=int(std::round(aa_thickness));
		
		const double pi=::acos(-1.);
		const double _2pi=2*pi;//function period
		const double maxSample=0.5*pi;//1 0.8 0.5 0				max sampling period <= Nyquist period = 0.5 function period

		std::vector<std::pair<double, double>>
	//	std::vector<double>
			Xcross(rgbn12, std::pair<double, double>(-1, -1)),//._		leftmost cross, rightmost cross
			Ycross(rgbn21, std::pair<double, double>(-1, -1));//!		uppermost cross, lowermost cross
		std::vector<char> shade(w3*(h+3));
		auto calculate_cross=[&](std::pair<double, double> &cr, int v0, int v1)
		{
			bool cross=false;
			auto &x0=hitX[v0], &x1=hitX[v1];
			auto &lr0=hitLR[v0], &lr1=hitLR[v1];
			if(std::abs(x1-x0)<maxSample)
			{
				if(cross=lr0>0!=lr1>0)
					cr.first=cr.second=lr0/(lr0-lr1);
			}
			else
			{
				auto &y0=hitY[v0], &y1=hitY[v1];
				if((y0>=-1||y1>=-1)&&(y0<=1||y1<=1))
				{
					double xa, ya, xb, yb;//ordered in-range segment
					if(x0<x1)
						xa=x0, ya=y0, xb=x1, yb=y1;
					else
						xa=x1, ya=y1, xb=x0, yb=y0;
					if(ya<-1)
						xa=(xb-xa)/(yb-ya)*(-1-ya)+xa, ya=-1;
					else if(ya>1)
						xa=(xb-xa)/(yb-ya)*(1-ya)+xa, ya=1;
					if(yb<-1)
						xb=(xb-xa)/(yb-ya)*(-1-ya)+xa, yb=-1;
					else if(yb>1)
						xb=(xb-xa)/(yb-ya)*(1-ya)+xa, yb=1;

					double const a=0.405284734569351, _1_2a=0.5/a, pi_2=pi/2;
					double *leftmost, *rightmost;
					if(x0<x1)
						leftmost=&cr.first, rightmost=&cr.second;
					else
						leftmost=&cr.second, rightmost=&cr.first;
					{
						int headNo;
						{
							double dhc=xa/pi+0.5;
							headNo=int(dhc)-(dhc<0);
						}
						int headSign=1-2*(headNo&1);
						double offset=headNo*pi;
						double xa2=xa-offset, xb2=xb-offset;
						for(;;)
						{
							double m=(yb-ya)/(xb2-xa2), c=ya-m*xa2;
							double discr=m*m-4*headSign*a*(c-headSign);
							if(discr>=0)
							{
								discr=sqrt(discr);
								double r1=(-m*headSign-discr)*_1_2a, r2=(-m*headSign+discr)*_1_2a;
								if(r1>=-pi_2&&r1<pi_2&&r1>=xa2&&r1<xb2)
								{
									cross=true, *leftmost=(offset+r1-x0)/(x1-x0);//r1=min(r1[,r2])
									break;
								}
								else if(r2>=-pi_2&&r2<pi_2&&r2>=xa2&&r2<xb2)
								{
									cross=true, *leftmost=(offset+r2-x0)/(x1-x0);
									break;
								}
							/*	disc=sqrt(disc);
								double _2a=2*headSign*a;
								double r1=(-m*headSign+disc)/_2a, r2=(-m*headSign-disc)/_2a;

								bool r1Valid=r1>=-pi_2&&r1<pi_2&&r1>=xa2&&r1<xb2, r2Valid=r2>=-pi_2&&r2<pi_2&&r2>=xa2&&r2<xb2;
								if(r1Valid)
								{
									if(r2Valid)
									{
										double rLeft=(std::abs(r1+r2)-std::abs(r1-r2))/2;//min
										cross=true, *leftmost=(offset+rLeft-x0)/(x1-x0);
									}
									else
										cross=true, *leftmost=(offset+r1-x0)/(x1-x0);
									break;
								}
								else if(r2Valid)
								{
									cross=true, *leftmost=(offset+r2-x0)/(x1-x0);
									break;
								}//*/
							/*	double rLeft=(std::abs(r1+r2)-std::abs(r1-r2))/2;//min
								if(rLeft>=-pi_2&&rLeft<pi_2&&rLeft>=xa2&&rLeft<xb2)
								{
									cross=true, (x0<x1?cr.first:cr.second)=(rLeft-x0)/(x1-x0);
									break;
								}//*/
							}
							if(xb2<pi_2)
								break;
							headSign=-headSign;
							xa2-=pi, xb2-=pi, offset+=pi;
						}
					}
					{
						int headNo;
						{
							double dhc=xb/pi+0.5;
							headNo=int(dhc)-(dhc<0);
						}
						int headSign=1-2*(headNo&1);
						double offset=headNo*pi;
						double xa2=xa-offset, xb2=xb-offset;
						for(;;)
						{
							double m=(yb-ya)/(xb2-xa2), c=ya-m*xa2;
							double discr=m*m-4*headSign*a*(c-headSign);
							if(discr>=0)
							{
								discr=sqrt(discr);
								double r1=(-m*headSign-discr)*_1_2a, r2=(-m*headSign+discr)*_1_2a;
								if(r2>=-pi_2&&r2<pi_2&&r2>=xa2&&r2<xb2)
								{
									cross=true, *rightmost=(offset+r2-x0)/(x1-x0);//r2=max([r1,]r2)
									break;
								}
								else if(r1>=-pi_2&&r1<pi_2&&r1>=xa2&&r1<xb2)
								{
									cross=true, *rightmost=(offset+r1-x0)/(x1-x0);
									break;
								}
							/*	disc=sqrt(disc);
								double _2a=2*headSign*a;
								double r1=(-m*headSign+disc)/_2a, r2=(-m*headSign-disc)/_2a;

								bool r1Valid=r1>=-pi_2&&r1<pi_2&&r1>=xa2&&r1<xb2, r2Valid=r2>=-pi_2&&r2<pi_2&&r2>=xa2&&r2<xb2;
								if(r1Valid)
								{
									if(r2Valid)
									{
										double rRight=offset+(std::abs(r1+r2)+std::abs(r1-r2))/2;//max
										cross=true, *rightmost=(rRight-x0)/(x1-x0);
									}
									else
										cross=true, *rightmost=(offset+r1-x0)/(x1-x0);
									break;
								}
								else if(r2Valid)
								{
									cross=true, *rightmost=(offset+r2-x0)/(x1-x0);
									break;
								}//*/
							/*	double rRight=(std::abs(r1+r2)+std::abs(r1-r2))/2;//max
								if(rRight>=-pi_2&&rRight<pi_2&&rRight>=xa2&&rRight<xb2)
								{
									cross=true, (x0<x1?cr.second:cr.first)=(rRight-x0)/(x1-x0);
									break;
								}//*/
							}
							if(xa2>-pi_2)
								break;
							headSign=-headSign;
							xa2+=pi, xb2+=pi, offset-=pi;
						}
					}
					if(cross)
					{
						if(cr.first==-1)
							cr.first=cr.second;
						else if(cr.second==-1)
							cr.second=cr.first;
					}
				/*	if(abs(xb-xa)>_2pi)//abs(xb-xa) > function period 2pi
					{
					}
					else//max sampling period x_s < abs(xb-xa) < function period 2pi
					{
						//solve for x with a function approximation
						int headNo;
						{
							double dhc=xa/pi+0.5;
							headNo=int(dhc)-(dhc<0);
						}
						int headSign=1-2*(headNo&1);

						double const a=0.405284734569351, pi_2=pi/2;
						double offset=headNo*pi;
						
						xa-=offset, xb-=offset;
						double m=(yb-ya)/(xb-xa), c=ya-m*xa;
						double disc=m*m-4*headSign*a;
						if(disc>=0)
						{
							disc=sqrt(disc);
							double _2a=2*headSign*a;
							double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
							if(r1<r2==x0<x1)
								cr.first=(r1-x0)/(x1-x0), cr.second=(r2-x0)/(x1-x0);
							else
								cr.first=(r2-x0)/(x1-x0), cr.second=(r1-x0)/(x1-x0);
						//	cross|=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
						}

						headSign=-headSign;
						xa-=pi, xb-=pi;
						double m=(yb-ya)/(xb-xa), c=ya-m*xa;
						double disc=m*m-4*headSign*a;
						if(disc>=0)
						{
							disc=sqrt(disc);
							double _2a=2*headSign*a;
							double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
							if(r1<r2==x0<x1)
							{
								double t;
								if(cr.first>(t=(r1-x0)/(x1-x0)))
									cr.first=t;
								if(cr.second>(t=(r2-x0)/(x1-x0)))
									cr.second=t;
							}
							else
							{
								double t;
								if(cr.first>(t=(r2-x0)/(x1-x0)))
									cr.first=t;
								if(cr.second>(t=(r1-x0)/(x1-x0)))
									cr.second=t;
							}
						//	cross|=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
						}
					}//*/
				/*	if(abs(xb-xa)>_2pi)//abs(xb-xa) > function period 2pi
					{
					}
					else//max sampling period x_s < abs(xb-xa) < function period 2pi
					{	
						xa-=offset, xb-=offset;
						bool second_time=false;
						do
						{
							double m=(yb-ya)/(xb-xa), c=ya-m*xa;
							double disc=m*m-4*headSign*a;
							if(disc>=0)
							{
								disc=sqrt(disc);
								double _2a=2*headSign*a;
								double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
								if(r1<r2==x0<x1)
									cr.first=(r1-x0)/(x1-x0), cr.second=(r2-x0)/(x1-x0);
								else
									cr.first=(r2-x0)/(x1-x0), cr.second=(r1-x0)/(x1-x0);
							//	cross|=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
							}
							headSign=-headSign;
							xa-=pi, xb-=pi;
						}
						while(second_time=!second_time);
					}//*/
				}
			}
			return cross;
		};
		{
			// _	top left
			//|_!
			int p=w2*0	+0;
			auto&h00=hit[p		], &h01=hit[p	+1],
				&h10=hit[p+w2	], &h11=hit[p+w2+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&corner=shade[w3*1	+1];
			corner|=h00&&h01&&!disc(x00, x01)&&calculate_cross(Xcross[w1*0	+0], p			, p		+1	);
			corner|=h10&&h11&&!disc(x10, x11)&&calculate_cross(Xcross[w1*1	+0], p+w2		, p+w2	+1	);
			corner|=h00&&h10&&!disc(x00, x10)&&calculate_cross(Ycross[w2*0	+0], p			, p+w2		);
			corner|=h01&&h11&&!disc(x01, x11)&&calculate_cross(Ycross[w2*0	+1], p		+1	, p+w2	+1	);
		}
		{
			// _	top right
			//._|
			int p=w2*0	+w;
			auto&h00=hit[p		], &h01=hit[p		+1],
				&h10=hit[p+w2	], &h11=hit[p+w2	+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&corner=shade[w3*1	+w];
			corner|=h00&&h01&&!disc(x00, x01)&&calculate_cross(Xcross[w1*0	+w], p			, p		+1	);
			corner|=h10&&h11&&!disc(x10, x11)&&calculate_cross(Xcross[w1*1	+w], p+w2		, p+w2	+1	);
			corner|=h01&&h11&&!disc(x01, x11)&&calculate_cross(Ycross[w2*0	+w1], p		+1	, p+w2	+1	);
		}
		{
			//  .	bottom left
			//|_|
			int p=w2*h	+0;
			auto&h00=hit[p		], &h01=hit[p		+1],
				&h10=hit[p+w2	], &h11=hit[p+w2	+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&lr00=hitLR[p	], &lr01=hitLR[p	+1],
				&lr10=hitLR[p+w2], &lr11=hitLR[p+w2	+1];
			auto&corner=shade[w3*h	+1];
			corner|=h10&&h11&&!disc(x10, x11)&&calculate_cross(Xcross[w1*h1	+0], p+w2		, p+w2	+1	);
			corner|=h00&&h10&&!disc(x00, x10)&&calculate_cross(Ycross[w2*h	+0], p			, p+w2		);
			corner|=h01&&h11&&!disc(x01, x11)&&calculate_cross(Ycross[w2*h	+1], p		+1	, p+w2	+1	);
		}
		for(int x=1;x<w;++x)
		{
			// _	upper row
			//._!
			int p=w2*0	+x;
			auto&h00=hit[p		], &h01=hit[p		+1],
				&h10=hit[p+w2	], &h11=hit[p+w2	+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&lr00=hitLR[p	], &lr01=hitLR[p	+1],
				&lr10=hitLR[p+w2], &lr11=hitLR[p+w2	+1];
			auto&p0=shade[w3*1	+x], &p1=shade[w3*1	+x+1];
			if(h00&&h01&&!disc(x00, x01)&&calculate_cross(Xcross[w1*0	+x	], p		, p		+1	))
				p0=p1=true;
			if(h10&&h11&&!disc(x10, x11)&&calculate_cross(Xcross[w1*1	+x	], p+w2		, p+w2	+1	))
				p0=p1=true;
			if(h01&&h11&&!disc(x01, x11)&&calculate_cross(Ycross[w2*0	+x+1], p	+1	, p+w2	+1	))
				p0=p1=true;
		}
		for(int y=1;y<h;++y)
		{
			//  .	left column
			//|_!
			int p=w2*y	+0;
			auto&h00=hit[p		], &h01=hit[p	+1],
				&h10=hit[p+w2	], &h11=hit[p+w2+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&lr00=hitLR[p	], &lr01=hitLR[p	+1],
				&lr10=hitLR[p+w2], &lr11=hitLR[p+w2	+1];
			auto&p0=shade[w3*y	+1], &p1=shade[w3*(y+1)	+1];
			if(h10&&h11&&!disc(x10, x11)&&calculate_cross(Xcross[w1*(y+1)	+0], p+w2		, p+w2	+1	))
				p0=p1=true;
			if(h00&&h10&&!disc(x00, x10)&&calculate_cross(Ycross[w2*y		+0], p			, p+w2		))
				p0=p1=true;
			if(h01&&h11&&!disc(x01, x11)&&calculate_cross(Ycross[w2*y		+1], p		+1	, p+w2	+1	))
				p0=p1=true;
		}
		auto aa_close=[&](double x, double y)// +.
		{
			return x*y*inv_sqrt(x*x+y*y);
		};
		auto aa_middle=[&](double a, double b)//|. |
		{
			double b_a=b-a;
			return a*inv_sqrt(1+b_a*b_a);
		};
		auto aa_far=[&](double x, double y)// L.
		{
			double x_1=x-1, y_1=y-1;
			return std::abs(x*y-1)*inv_sqrt(x_1*x_1+y_1*y_1);
		};
		for(int y=1;y<h1;++y)
		{
			for(int x=1;x<w1;++x)
			{
				int hp=w2*y+x;
				int sp=w3*y+x;
				{
					int p=w2*y	+0;
					auto					&h01=hit[hp		+1],
						&h10=hit[hp+w2],	&h11=hit[hp+w2	+1];
					auto					&x01=hitX[hp	+1],
						&x10=hitX[hp+w2],	&x11=hitX[hp+w2	+1];
					auto					&lr01=hitLR[hp	+1],
						&lr10=hitLR[hp+w2],	&lr11=hitLR[hp+w2+1];
					if(h10&&h11&&!disc(x10, x11)&&calculate_cross(Xcross[w1*(y+1)	+x], hp+w2		, hp+w2	+1	))
					{
						shade[sp		]=true, shade[sp		+1]=true;
						shade[sp+w3		]=true, shade[sp+w3		+1]=true;
						shade[sp+w3*2	]=true, shade[sp+w3*2	+1]=true;
					}
				//	if(hp+1==239222)
					if(x-1==626&&y-1==236)//
						int LOL_1=0;//
					if(h01&&h11&&!disc(x01, x11)&&calculate_cross(Ycross[hp			+1], hp		+1	, hp+w2	+1	))
					{
						shade[sp	]=true, shade[sp	+1]=true, shade[sp		+2]=true;
						shade[sp+w3	]=true, shade[sp+w3	+1]=true, shade[sp+w3	+2]=true;
					}
					if(shade[sp])
					{
						//	 	Dx
						//	Lx	+	Rx		screen coords upside down
						//		Ux  '
						auto		&Dx=Ycross[w2*(y-1)+x].second,//down cross upmost
							&Lx=Xcross[w1*y+x-1].second,	&Rx=Xcross[w1*y+x].first,//left cross rightmost, right cross leftmost
									&Ux=Ycross[w2* y   +x].first;//up cross downmost
						double d_min=1;
						if(Ux!=-1)
						{
							if(Dx!=-1)
							{
								if(Lx!=-1)
								{
									if(Rx!=-1)	//udlr	>=2 lines	\+\	/+/
									{
										double d;
										d_min=aa_close(Rx, Ux);
										if(d_min>(d=aa_close(Rx, 1-Dx)))
											d_min=d;
										if(d_min>(d=aa_close(1-Lx, 1-Dx)))
											d_min=d;
										if(d_min>(d=aa_close(1-Lx, Ux)))
											d_min=d;
									}
									else		//udl	>=2 lines	<+
									{
										double d;
										d_min=aa_close(1-Lx, 1-Dx);
										if(d_min>(d=aa_close(1-Lx, Ux)))
											d_min=d;
									}
								}
								else
								{
									if(Rx!=-1)	//ud r	>=2 lines	+>
									{
										double d;
										d_min=aa_close(Rx, Ux);
										if(d_min>(d=aa_close(Rx, 1-Dx)))
											d_min=d;
									}
									else		//ud				_F_
									{
										double d;
										d_min=Ux;
										if(d_min>(d=1-Dx))
											d_min=d;
									}
								}
							}
							else
							{
								if(Lx!=-1)
								{
									if(Rx!=-1)	//u lr	>=2 lines	\+/
									{
										double d;
										d_min=aa_close(Rx, 1-Dx);
										if(d_min>(d=aa_close(1-Lx, 1-Dx)))
											d_min=d;
									}
									else		//u l	>=1 line	\+
										d_min=aa_close(1-Lx, Ux);
								}
								else
								{
									if(Rx!=-1)	//u  r	>=1 line	+/
										d_min=aa_close(Rx, Ux);
									else		//u		>=1 line	_+_
									{
										auto &LUx=Ycross[w2*y	+x-1].first, &RUx=Ycross[w2*y	+x+1].first;//left-up cross rightmost, right-up cross leftmost
										double d;
										d_min=Ux;
										if(LUx!=-1&&LUx<Ux&&d_min>(d=aa_middle(Ux, LUx)))
											d_min=d;
										if(RUx!=-1&&RUx<Ux&&d_min>(d=aa_middle(Ux, RUx)))
											d_min=d;
									}
								}
							}
						}
						else
						{
							if(Dx!=-1)
							{
								if(Lx!=-1)
								{
									if(Rx!=-1)	// dlr	>=2 lines	/+\ 
									{
										double d;
										d_min=aa_close(Rx, Ux);
										if(d_min>(d=aa_close(1-Lx, Ux)))
											d_min=d;
									}
									else		// dl	>=1 line	/+
										d_min=aa_close(1-Lx, 1-Dx);
								}
								else
								{
									if(Rx!=-1)	// d r	>=1 line	+\ 
										d_min=aa_close(Rx, 1-Dx);
									else		// d	>=1 line	F
									{
										auto &LDx=Ycross[w2*(y-1)+x-1].second, &RDx=Ycross[w2*(y-1)+x+1].second;//left-down cross rightmost, right-down cross leftmost
										double d;
										d_min=1-Dx;
										if(LDx!=-1&&LDx>Dx&&d_min>(d=aa_middle(1-Dx, 1-LDx)))
											d_min=d;
										if(RDx!=-1&&RDx>Dx&&d_min>(d=aa_middle(1-Dx, 1-RDx)))
											d_min=d;
									}
								}
							}
							else
							{
								if(Lx!=-1)
								{
									if(Rx!=-1)	//  lr	>=1 line
									{
										double d;
										d_min=Rx;
										if(d_min>(d=1-Lx))
											d_min=d;
									}
									else		//  l	>=1 line	|+
									{
										auto &DLx=Xcross[w1*(y-1)+x-1].second, &ULx=Xcross[w1*(y+1)+x-1].first;//down-left cross upmost, up-left cross downmost
										double d;
										d_min=1-Lx;
										if(DLx!=-1&&DLx>Lx&&d_min>(d=aa_middle(1-Lx, 1-DLx)))
											d_min=d;
										if(ULx!=-1&&ULx>Lx&&d_min>(d=aa_middle(1-Lx, 1-ULx)))
											d_min=d;
									}
								}
								else
								{
									if(Rx!=-1)	//   r	>=1 line	+|
									{
										auto &DRx=Xcross[w1*(y-1)+x  ].second, &URx=Xcross[w1*(y+1)+x  ].first;//down-right cross upmost, up-right cross downmost
										double d;
										d_min=Rx;
										if(DRx!=-1&&DRx<Rx&&d_min>(d=aa_middle(Rx, DRx)))
											d_min=d;
										if(URx!=-1&&URx<Rx&&d_min>(d=aa_middle(Rx, URx)))
											d_min=d;
									}
									else		//		>=0 lines
									{
										auto
											&DLx=Xcross[w1*(y-1)+x-1].second,	&LDx=Ycross[w2*(y-1)+x-1].second,//	s-->	DL	 _    _		DR
											&DRx=Xcross[w1*(y-1)+x  ].first,	&RDx=Ycross[w2*(y-1)+x+1].second,//	|	LD		| 1  2 | D		RD
											&ULx=Xcross[w1*(y+1)+x-1].second,	&LUx=Ycross[w2* y   +x-1].first,//	v	LU		    .			RU
											&URx=Xcross[w1*(y+1)+x  ].first,	&RUx=Ycross[w2* y   +x+1].first;//			UL	|_3  4_| U	UR
										double d;
										if(DLx!=-1&&LDx!=-1&&d_min>(d=aa_far(1-DLx, 1-LDx)))//1
											d_min=d;
										if(DRx!=-1&&RDx!=-1&&d_min>(d=aa_far(  DRx, 1-RDx)))//2
											d_min=d;
										if(ULx!=-1&&LUx!=-1&&d_min>(d=aa_far(1-ULx,   LUx)))//3
											d_min=d;
										if(URx!=-1&&RUx!=-1&&d_min>(d=aa_far(  URx,   RUx)))//4
											d_min=d;
										if(d_min==1)
											continue;
										else
											d_min=d_min>aa_thickness?1:d_min*_1_aa_thickness;
									}
								}
							}
						}
						auto prgb=(unsigned char*)&rgb[w*(y-1)	+x-1];
						prgb[0]=unsigned char(prgb[0]*d_min);
						prgb[1]=unsigned char(prgb[1]*d_min);
						prgb[2]=unsigned char(prgb[2]*d_min);
					}
				}
			}
		}
	}
#endif
#if 0//draw curve		pixelated, no aliasing
	{
		//<u> crosses xy-plane
		int w1=w+1, h1=h+1, rgbn11=w1*h1;
		std::vector<int> hit(h1);
		std::vector<double> hitX(rgbn11), hitY(rgbn11), hitF(rgbn11);
		double t=-camz;
		double ttfov=t*tanfov, tfov_x0=tanfov/X0, ttfov_x0=t*tfov_x0, y0_x0=double(Y0)/X0;
		double A2=-tfov_x0*cay/t, A3=(tanfov*y0_x0*cay+say)/t;
		double	B1=0,					B2=-tfov_x0*cay,				B3=tanfov*y0_x0*cay+say,
				B4=ttfov_x0*sax,		B5=ttfov_x0*cax*say+B2*camx,	B6=t*(-tanfov*(sax+y0_x0*cax*say)+cax*cay)+B3*camx,
				B7=-ttfov_x0*cax,		B8=ttfov_x0*sax*say+B2*camy,	B9= t*(tanfov*(cax-y0_x0*sax*say)+sax*cay)+B3*camy;
		
		const double pi=::acos(-1.);
		auto _1d_int_in_range=[](double x0, double x1){return std::floor(x0)!=std::floor(x1)||std::ceil(x0)!=std::ceil(x1);};
		auto disc=[&](double x0, double x1)
		{
		//	return std::abs(x1-x0)>3.2||_1d_int_in_range(x0/pi-.5, x1/pi-.5);	//tan
			return false;														//cos
		};
		for(int ys=0;ys<h1;++ys)
		{
			if(hit[ys]=A2*ys+A3>0)//_1_Zcp>0
			{
				double den=B2*ys+B3;//B1=0
				double B56=B5*ys+B6, B89=B8*ys+B9;
				for(int xs=0, p=w1*ys;xs<w1;++xs, ++p)
				{
					hitX[p]=(B4*xs+B56)/den;
					hitY[p]=(B7*xs+B89)/den;
					
				//	hitF[p]=std::tan(hitX[p]);

				//	hitF[p]=100*cos(10*hitX[p]/100)*cos(hitX[p]/100)+cos(hitX[p]);
					hitF[p]=std::cos(hitX[p]);
				}
			}
		}
		const double _2pi=2*pi;//function period
		const double x_s=0.8*pi;//max sampling period <= Nyquist period				1 0.8 0.5 0
		for(int ys=0;ys<h;++ys)
		{
			if(hit[ys])
			{
				for(int xs=0, hp=w1*ys;xs<w;++xs, ++hp)
				{
					if(!disc(hitX[hp], hitX[hp+1]))
					{
						auto &x0=hitX[hp], &x1=hitX[hp+1], &y0=hitY[hp], &y1=hitY[hp+1];
						bool cross;
						if(abs(x1-x0)<x_s)//max sampling period
							cross=y0>hitF[hp]!=y1>hitF[hp+1];//ends on different sides of curve
						else
						{
							if((y0>=-1||y1>=-1)&&(y0<=1||y1<=1))//segment crosses function range
							{
								double xa, ya, xb, yb;
								if(x0<x1)
									xa=x0, ya=y0, xb=x1, yb=y1;
								else
									xa=x1, ya=y1, xb=x0, yb=y0;
								if(ya<-1)
									xa=(xb-xa)/(yb-ya)*(-1-ya)+xa, ya=-1;
								else if(ya>1)
									xa=(xb-xa)/(yb-ya)*(1-ya)+xa, ya=1;
								if(yb<-1)
									xb=(xb-xa)/(yb-ya)*(-1-ya)+xa, yb=-1;
								else if(yb>1)
									xb=(xb-xa)/(yb-ya)*(1-ya)+xa, yb=1;
								if(!(cross=abs(xb-xa)>_2pi))//max sampling period x_s < abs(xb-xa) < function period 2pi
								{
									//solve for x with a function approximation
									int headNo;
									{
										double dhc=xa/pi+0.5;
										headNo=int(dhc)-(dhc<0);
									}
									int headSign=1-2*(headNo&1);

									double const a=0.405284734569351, pi_2=pi/2;
									double offset=headNo*pi;

								/*	for(bool second_head=false;;)
									{
										double m=(yb-ya)/(xb-xa), c=ya-m*xa;
										double disc=m*m-4*headSign*a*(c-headSign);
										if(disc>=0)
										{
											disc=sqrt(disc);
											double _2a=2*headSign*a;
											double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
											cross|=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
										}
										if(second_head=!second_head)
										{
											headSign=-headSign;
											xa-=pi, xb-=pi;
											continue;
										}
										break;
									}//*/
									xa-=offset, xb-=offset;
									{
										bool second_head=false;
_3d_curve_pixelated_x_appr_eval:
										double m=(yb-ya)/(xb-xa), c=ya-m*xa;
										double disc=m*m-4*headSign*a*(c-headSign);//cos appr	1-axx
										if(disc>=0)
										{
											disc=sqrt(disc);
											double _2a=2*headSign*a;
											double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
											cross|=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
										}
										if(second_head=!second_head)
										{
											headSign=-headSign;
											xa-=pi, xb-=pi;
											goto _3d_curve_pixelated_x_appr_eval;
										}
									}//*/
								/*	xa-=offset, xb-=offset;
									bool second_time=false;
									do
									{
										double m=(yb-ya)/(xb-xa), c=ya-m*xa;
										double disc=m*m-4*headSign*a*(c-headSign);
										if(disc>=0)
										{
											disc=sqrt(disc);
											double _2a=2*headSign*a;
											double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
											cross|=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
										}
										headSign=-headSign;
										xa-=pi, xb-=pi;
									}
									while(second_time=!second_time);//*/
								/*	//half cycle 1
									xa-=offset, xb-=offset;
									double m=(yb-ya)/(xb-xa), c=ya-m*xa;
									double disc=m*m+4*headSign*a*(headSign-c);
									if(disc>=0)
									{
										disc=sqrt(disc);
										double _2a=2*headSign*a;
										double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
										cross=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
									}
									if(!cross)
									{
										//half cycle 2
										headSign=-headSign;
										xa-=pi, xb-=pi;
										double m=(yb-ya)/(xb-xa), c=ya-m*xa;
										double disc=m*m-4*headSign*a*(c-headSign);
										if(disc>=0)
										{
											disc=sqrt(disc);
											double _2a=2*headSign*a;
											double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
											cross=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
										}
									}//*/
								}
							}
							else
								cross=false;
						}
#if 0
					/*	bool no_cross=y0<-1&&y1<-1||y0>1&&y1>1;	//same
						if(!no_cross)
						{
							no_cross=y0>hitF[hp]==y1>hitF[hp];
							if(no_cross)
							{
								no_cross=
							}
							rgb[w*ys+xs]*=no_cross;
							if(ys>0)
								rgb[w*(ys-1)+xs]*=no_cross;
						}//*/
						bool cross=y0>hitF[hp]!=y1>hitF[hp+1];//ends on different sides of curve
						if(!cross&&(y0>=-1||y1>=-1)&&(y0<=1||y1<=1))//ends on same side of curve	& segment crosses range -1<y<1
						{
							auto&x0=hitX[hp], &x1=hitX[hp+1];
							double xa, ya, xb, yb;
							if(x0<x1)
								xa=x0, ya=y0, xb=x1, yb=y1;
							else
								xa=x1, ya=y1, xb=x0, yb=y0;
							if(ya<-1)
								xa=(xb-xa)/(yb-ya)*(-1-ya)+xa, ya=-1;
							else if(ya>1)
								xa=(xb-xa)/(yb-ya)*(1-ya)+xa, ya=1;
							if(yb<-1)
								xb=(xb-xa)/(yb-ya)*(-1-ya)+xa, yb=-1;
							else if(yb>1)
								xb=(xb-xa)/(yb-ya)*(1-ya)+xa, yb=1;
							const double _2pi=2*pi;
							cross=abs(xb-xa)>_2pi;
							if(!cross)
							{
								double dhc=xa/pi+0.5;
								int hc=int(t)-(t<0);
								int sign=1-2*(hc%2);

								double const a=0.405284734569351, pi_2=pi/2;
								double offset=hc*pi;
								//half cycle 1
								xa-=offset, xb-=offset;
								double m=(yb-ya)/(xb-xa), c=ya-m*xa;
								double disc=m*m+4*sign*a*(sign-c);
								if(disc>=0)
								{
									disc=sqrt(disc);
									double _2a=2*sign*a;
									double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
									cross=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
								}
								if(!cross)
								{
									//half cycle 2
									sign=-sign;
									xa-=pi, xb-=pi;
									double m=(yb-ya)/(xb-xa), c=ya-m*xa;
									double disc=m*m-4*sign*a;
									if(disc>=0)
									{
										disc=sqrt(disc);
										double _2a=2*sign*a;
										double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
										cross=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
									}
								}
							}
						/*	if(y0<y1)
							{
								if(y0<-1)
									cross=std::abs((x0-x1)*(-1-y1)/(y0-y1))>2*pi;
								else if(y1>1)
									cross=std::abs((x1-x0)*(1-y0)/(y1-y0))>2*pi;//x
								else
									cross=std::abs(x1-x0)>2*pi;
							}
							else
							{
								if(y1<-1)
									cross=std::abs((x1-x0)*(-1-y0)/(y1-y0))>2*pi;
								else if(y0>1)
									cross=std::abs((x0-x1)*(1-y1)/(y0-y1))>2*pi;//x
								else
									cross=std::abs(x0-x1)>2*pi;
							}
							if(!cross)//abs(x2-x1)<2*pi	//artifacts
							{
								double m=(y1-y0)/(x1-x0), c=y0-m*x0;
								double disc=m*m+2*c+2;
								if(disc>=0)
								{
									disc=sqrt(disc);
									double r1=-m+disc, r2=-m-disc;
									cross=r1>x0&&r1<x1||r1>x1&&r1<x0||r2>x0&&r2<x1||r2>x1&&r2<x0;
								}
							}//*/
						}
#endif
						if(cross)
						{
							rgb[w*ys+xs]=0;
							if(ys>0)
								rgb[w*(ys-1)+xs]=0;
						}
					}
				/*	if((y0>=-1||y1>=-1)&&(y0<=1||y1<=1))
					{
						bool cross=y0>hitF[hp]!=y1>hitF[hp];
						if(!cross)
						{
							auto&x0=hitX[hp], &x1=hitX[hp+1];
							if(y0<y1)
							{
								if(y0<-1)
									cross=std::abs((x0-x1)*(-1-y1)/(y0-y1))>2*pi;
								else//y1>1
									cross=std::abs((x1-x0)*(1-y0)/(y1-y0))>2*pi;
							}
							else
							{
								if(y1<-1)
									cross=std::abs((x1-x0)*(-1-y0)/(y1-y0))>2*pi;
								else//y0>1
									cross=std::abs((x0-x1)*(1-y1)/(y0-y1))>2*pi;
							}
						}
						if(cross)
					//	if(hitLR[hp]>0!=hitLR[hp+w1]>0&&!disc(hitX[hp], hitX[hp+w1]))
						{
							rgb[w*ys+xs]=0;
							if(xs>0)
								rgb[w*ys+xs-1]=0;
						}
					}//*/
				}
				if(hit[ys+1])
				{
					for(int xs=0, hp=w1*ys;xs<w;++xs, ++hp)
					{
						auto &x0=hitX[hp], &x1=hitX[hp+w1], &y0=hitY[hp], &y1=hitY[hp+w1];
						bool cross;
						if(abs(x1-x0)<x_s)//max sampling period
							cross=y0>hitF[hp]!=y1>hitF[hp+w1];//ends on different sides of curve
						else
						{
							if((y0>=-1||y1>=-1)&&(y0<=1||y1<=1))//segment crosses function range
							{
								double xa, ya, xb, yb;
								if(x0<x1)
									xa=x0, ya=y0, xb=x1, yb=y1;
								else
									xa=x1, ya=y1, xb=x0, yb=y0;
								if(ya<-1)
									xa=(xb-xa)/(yb-ya)*(-1-ya)+xa, ya=-1;
								else if(ya>1)
									xa=(xb-xa)/(yb-ya)*(1-ya)+xa, ya=1;
								if(yb<-1)
									xb=(xb-xa)/(yb-ya)*(-1-ya)+xa, yb=-1;
								else if(yb>1)
									xb=(xb-xa)/(yb-ya)*(1-ya)+xa, yb=1;
								if(!(cross=abs(xb-xa)>_2pi))
								{
									int headNo;
									{
										double dhc=xa/pi+0.5;
										headNo=int(dhc)-(dhc<0);
									}
									int headSign=1-2*(headNo&1);

									double const a=0.405284734569351, pi_2=pi/2;
									double offset=headNo*pi;

									xa-=offset, xb-=offset;
									{
										bool second_head=false;
_3d_curve_pixelated_y_appr_eval:
										double m=(yb-ya)/(xb-xa), c=ya-m*xa;
										double disc=m*m-4*headSign*a*(c-headSign);
										if(disc>=0)
										{
											disc=sqrt(disc);
											double _2a=2*headSign*a;
											double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
											cross|=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
										}
										if(second_head=!second_head)
										{
											headSign=-headSign;
											xa-=pi, xb-=pi;
											goto _3d_curve_pixelated_y_appr_eval;
										}
									}
								/*	//half cycle 1
									xa-=offset, xb-=offset;
									double m=(yb-ya)/(xb-xa), c=ya-m*xa;
									double disc=m*m+4*sign*a*(sign-c);
									if(disc>=0)
									{
										disc=sqrt(disc);
										double _2a=2*sign*a;
										double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
										cross=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
									}
									if(!cross)
									{
										//half cycle 2
										sign=-sign;
										xa-=pi, xb-=pi;
										double m=(yb-ya)/(xb-xa), c=ya-m*xa;
										double disc=m*m-4*sign*a;
										if(disc>=0)
										{
											disc=sqrt(disc);
											double _2a=2*sign*a;
											double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
											cross=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
										}
									}//*/
								}
							}
							else
								cross=false;
						}
#if 0
						bool cross=y0>hitF[hp]!=y1>hitF[hp+w1];
						if(!cross&&(y0>=-1||y1>=-1)&&(y0<=1||y1<=1))
						{
							auto&x0=hitX[hp], &x1=hitX[hp+w1];
							double xa, ya, xb, yb;
							if(x0<x1)
								xa=x0, ya=y0, xb=x1, yb=y1;
							else
								xa=x1, ya=y1, xb=x0, yb=y0;
							if(ya<-1)
								xa=(xb-xa)/(yb-ya)*(-1-ya)+xa, ya=-1;
							else if(ya>1)
								xa=(xb-xa)/(yb-ya)*(1-ya)+xa, ya=1;
							if(yb<-1)
								xb=(xb-xa)/(yb-ya)*(-1-ya)+xa, yb=-1;
							else if(yb>1)
								xb=(xb-xa)/(yb-ya)*(1-ya)+xa, yb=1;
							const double _2pi=2*pi;
							cross=abs(xb-xa)>_2pi;
							if(!cross)
							{
								double dhc=xa/pi+0.5;
								int hc=int(t)-(t<0);
								int sign=1-2*(hc%2);

								double const a=0.405284734569351, pi_2=pi/2;
								double offset=hc*pi;
								//half cycle 1
								xa-=offset, xb-=offset;
								double m=(yb-ya)/(xb-xa), c=ya-m*xa;
								double disc=m*m+4*sign*a*(sign-c);
								if(disc>=0)
								{
									disc=sqrt(disc);
									double _2a=2*sign*a;
									double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
									cross=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
								}
								if(!cross)
								{
									//half cycle 2
									sign=-sign;
									xa-=pi, xb-=pi;
									double m=(yb-ya)/(xb-xa), c=ya-m*xa;
									double disc=m*m-4*sign*a;
									if(disc>=0)
									{
										disc=sqrt(disc);
										double _2a=2*sign*a;
										double r1=(-m+disc)/_2a, r2=(-m-disc)/_2a;
										cross=r1>=-pi_2&&r1<pi_2&&r1>=xa&&r1<xb||r2>=-pi_2&&r2<pi_2&&r2>=xa&&r2<xb;
									}
								}
							}
						/*	if(y0<y1)
							{
								if(y0<-1)
									cross=std::abs((x0-x1)*(-1-y1)/(y0-y1))>2*pi;
								else if(y1>1)
									cross=std::abs((x1-x0)*(1-y0)/(y1-y0))>2*pi;//x
								else
									cross=std::abs(x1-x0)>2*pi;
							}
							else
							{
								if(y1<-1)
									cross=std::abs((x1-x0)*(-1-y0)/(y1-y0))>2*pi;
								else if(y0>1)
									cross=std::abs((x0-x1)*(1-y1)/(y0-y1))>2*pi;//x
								else
									cross=std::abs(x0-x1)>2*pi;
							}
							if(!cross)	//artifacts
							{
								double m=(y1-y0)/(x1-x0), c=y0-m*x0;
								double disc=m*m+2*c+2;
								if(disc>=0)
								{
									disc=sqrt(disc);
									double r1=-m+disc, r2=-m-disc;
									cross=r1>x0&&r1<x1||r1>x1&&r1<x0||r2>x0&&r2<x1||r2>x1&&r2<x0;
								}
							}//*/
						}
#endif
						if(cross)
						{
							rgb[w*ys+xs]=0;
							if(xs>0)
								rgb[w*ys+xs-1]=0;
						}
					}
				}
			}
		}
	}
#endif
#if 0//draw curve		aa		samples coincide with pixels, 1 aa loop			from g2 c2d contour
	if(w>=1&&h>=1)
	{
		//XY-plane		<u> crosses xy-plane
		int w1=w+1, h1=h+1, w2=w+2, h2=h+2, w3=w+3,
			rgbn12=w1*h2, rgbn21=w2*h1, rgbn22=w2*h2;
		std::vector<char> hit(rgbn22);
		std::vector<double> hitX(rgbn22), hitLR(rgbn22);//where pixel rays hit the x-y plane	x, y-f
		double t=-camz;
		double ttfov=t*tanfov, tfov_x0=tanfov/X0, ttfov_x0=t*tfov_x0, y0_x0=double(Y0)/X0;
		double A2=-tfov_x0*cay/t, A3=(tanfov*y0_x0*cay+say)/t;
		double	B1=0,					B2=-tfov_x0*cay,				B3=tanfov*y0_x0*cay+say,
				B4=ttfov_x0*sax,		B5=ttfov_x0*cax*say+B2*camx,	B6=t*(-tanfov*(sax+y0_x0*cax*say)+cax*cay)+B3*camx,
				B7=-ttfov_x0*cax,		B8=ttfov_x0*sax*say+B2*camy,	B9=t*( tanfov*(cax-y0_x0*sax*say)+sax*cay)+B3*camy;
		
		double _pi=::acos(-1.);
		auto _1d_int_in_range=[](double x0, double x1){return std::floor(x0)!=std::floor(x1)||std::ceil(x0)!=std::ceil(x1);};
		auto disc=[&](double x0, double x1)
		{
		//	return std::abs(x1-x0)>3.2||_1d_int_in_range(x0/_pi-.5, x1/_pi-.5);		//tan x
			return false;															//cos x
		};

		for(int ys=-1;ys<h1;++ys)
		{
			int hp=w2*(ys+1);
			if(A2*ys+A3>0)//_1_Zcp>0
			{
				double den=B2*ys+B3;//B1=0
				double B56=B5*ys+B6, B89=B8*ys+B9;
				for(int xs=-1;xs<w1;++xs, ++hp)
				{
					hit[hp]=1;
					hitX[hp]=(B4*xs+B56)/den,
					hitLR[hp]=(B7*xs+B89)/den - std::cos(hitX[hp]);
				//	hitY[p]=(B7*xs+B89)/den;

				//	hitF[p]=0;
				//	hitF[p]=hitX[p];
				//	hitF[p]=std::tan(hitX[p]);
				//	hitF[p]=100*std::cos(10*hitX[p]/100)*std::cos(hitX[p]/100)+cos(hitX[p]);
				//	hitF[p]=std::cos(10*hitX[p])*std::cos(hitX[p]);
				//	hitF[p]=std::cos(hitX[p]*hitX[p])*std::cos(hitX[p]);
				//	hitF[p]=hitX[p]*std::cos(hitX[p]);
				//	hitF[p]=std::sin(1/hitX[p]);
				//	hitF[p]=1/std::sin(hitX[p]);
				//	hitF[p]=std::sqrt(std::abs(hitX[p]));
				//	hitF[p]=std::tan(hitX[p]);
				//	hitF[p]=std::exp(hitX[p]);
				//	hitF[p]=std::cos(hitX[p]);
				}
			}
		}
		
		const double aa_thickness=1,
				
			_1_aa_thickness=1/aa_thickness;
		const int aa_bound=int(std::round(aa_thickness));

		std::vector<double>
			Xcross(rgbn12, -1),//._
			Ycross(rgbn21, -1);//!
		std::vector<char> shade(w3*(h+3));
		{
			// _	top left
			//|_!
			int p=w2*0	+0;
			auto&h00=hit[p		], &h01=hit[p	+1],
				&h10=hit[p+w2	], &h11=hit[p+w2+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&lr00=hitLR[p	], &lr01=hitLR[p	+1],
				&lr10=hitLR[p+w2], &lr11=hitLR[p+w2	+1];
			auto&corner=shade[w3*1	+1];
			if(h00&&h01&&lr00>0!=lr01>0&&!disc(x00, x01))
				corner=true, Xcross[w1*0	+0]=lr00/(lr00-lr01);		//zero cross	Y(xs, ys) - F(X(xs, ys))
			if(h10&&h11&&lr10>0!=lr11>0&&!disc(x10, x11))
				corner=true, Xcross[w1*1	+0]=lr10/(lr10-lr11);
			if(h00&&h10&&lr00>0!=lr10>0&&!disc(x00, x10))
				corner=true, Ycross[w2*0	+0]=lr00/(lr00-lr10);
			if(h01&&h11&&lr01>0!=lr11>0&&!disc(x01, x11))
				corner=true, Ycross[w2*0	+1]=lr01/(lr01-lr11);
		}
		{
			// _	top right
			//._|
			int p=w2*0	+w;
			auto&h00=hit[p		], &h01=hit[p		+1],
				&h10=hit[p+w2	], &h11=hit[p+w2	+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&lr00=hitLR[p	], &lr01=hitLR[p	+1],
				&lr10=hitLR[p+w2], &lr11=hitLR[p+w2	+1];
			auto&corner=shade[w3*1	+w];
			if(h00&&h01&&lr00>0!=lr01>0&&!disc(x00, x01))
				corner=true, Xcross[w1*0	+w]=lr00/(lr00-lr01);
			if(h10&&h11&&lr10>0!=lr11>0&&!disc(x10, x11))
				corner=true, Xcross[w1*1	+w]=lr10/(lr10-lr11);
			if(h01&&h11&&lr01>0!=lr11>0&&!disc(x01, x11))
				corner=true, Ycross[w2*0	+w1]=lr01/(lr01-lr11);
		}
		{
			//  .	bottom left
			//|_|
			int p=w2*h	+0;
			auto&h00=hit[p		], &h01=hit[p		+1],
				&h10=hit[p+w2	], &h11=hit[p+w2	+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&lr00=hitLR[p	], &lr01=hitLR[p	+1],
				&lr10=hitLR[p+w2], &lr11=hitLR[p+w2	+1];
			auto&corner=shade[w3*h	+1];
			if(h10&&h11&&lr10>0!=lr11>0&&!disc(x10, x11))
				corner=true, Xcross[w1*h1	+0]=lr10/(lr10-lr11);
			if(h00&&h10&&lr00>0!=lr10>0&&!disc(x00, x10))
				corner=true, Ycross[w2*h	+0]=lr00/(lr00-lr10);
			if(h01&&h11&&lr01>0!=lr11>0&&!disc(x01, x11))
				corner=true, Ycross[w2*h	+1]=lr01/(lr01-lr11);
		}
		for(int x=1;x<w;++x)
		{
			// _	upper row
			//._!
			int p=w2*0	+x;
			auto&h00=hit[p		], &h01=hit[p		+1],
				&h10=hit[p+w2	], &h11=hit[p+w2	+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&lr00=hitLR[p	], &lr01=hitLR[p	+1],
				&lr10=hitLR[p+w2], &lr11=hitLR[p+w2	+1];
			auto&p0=shade[w3*1	+x], &p1=shade[w3*1	+x+1];
			if(h00&&h01&&lr00>0!=lr01>0&&!disc(x00, x01))
				p0=p1=true, Xcross[w1*0	+x]=lr00/(lr00-lr01);
			if(h10&&h11&&lr10>0!=lr11>0&&!disc(x10, x11))
				p0=p1=true, Xcross[w1*1	+x]=lr10/(lr10-lr11);
			if(h01&&h11&&lr01>0!=lr11>0&&!disc(x01, x11))
				p0=p1=true, Ycross[w2*0	+x+1]=lr01/(lr01-lr11);
		}
		for(int y=1;y<h;++y)
		{
			//  .	left column
			//|_!
			int p=w2*y	+0;
			auto&h00=hit[p		], &h01=hit[p	+1],
				&h10=hit[p+w2	], &h11=hit[p+w2+1];
			auto&x00=hitX[p		], &x01=hitX[p		+1],
				&x10=hitX[p+w2	], &x11=hitX[p+w2	+1];
			auto&lr00=hitLR[p	], &lr01=hitLR[p	+1],
				&lr10=hitLR[p+w2], &lr11=hitLR[p+w2	+1];
			auto&p0=shade[w3*y	+1], &p1=shade[w3*(y+1)	+1];
			if(h10&&h11&&lr10>0!=lr11>0&&!disc(x10, x11))
				p0=p1=true, Xcross[w1*(y+1)	+0]=lr10/(lr10-lr11);
			if(h00&&h10&&lr00>0!=lr10>0&&!disc(x00, x10))
				p0=p1=true, Ycross[w2*y		+0]=lr00/(lr00-lr10);
			if(h01&&h11&&lr01>0!=lr11>0&&!disc(x01, x11))
				p0=p1=true, Ycross[w2*y		+1]=lr01/(lr01-lr11);
		}
		auto aa_close=[&](double x, double y)// +.
		{
			return x*y*inv_sqrt(x*x+y*y);
		};
		auto aa_middle=[&](double a, double b)//|. |
		{
			double b_a=b-a;
			return a*inv_sqrt(1+b_a*b_a);
		};
		auto aa_far=[&](double x, double y)// L.
		{
			double x_1=x-1, y_1=y-1;
			return std::abs(x*y-1)*inv_sqrt(x_1*x_1+y_1*y_1);
		};
		for(int y=1;y<h1;++y)
		{
			for(int x=1;x<w1;++x)
			{
				int hp=w2*y+x;
				int sp=w3*y+x;
				{
					int p=w2*y	+0;
					auto					&h01=hit[hp		+1],
						&h10=hit[hp+w2],	&h11=hit[hp+w2	+1];
					auto					&x01=hitX[hp	+1],
						&x10=hitX[hp+w2],	&x11=hitX[hp+w2	+1];
					auto					&lr01=hitLR[hp	+1],
						&lr10=hitLR[hp+w2],	&lr11=hitLR[hp+w2+1];
					if(h10&&h11&&lr10>0!=lr11>0&&!disc(x10, x11))
					{
						Xcross[w1*(y+1)+x]=lr10/(lr10-lr11);
						shade[sp		]=true, shade[sp		+1]=true;
						shade[sp+w3		]=true, shade[sp+w3		+1]=true;
						shade[sp+w3*2	]=true, shade[sp+w3*2	+1]=true;
					}
					if(h01&&h11&&lr01>0!=lr11>0&&!disc(x01, x11))
					{
						Ycross[hp+1]=lr01/(lr01-lr11);
						shade[sp	]=true, shade[sp	+1]=true, shade[sp		+2]=true;
						shade[sp+w3	]=true, shade[sp+w3	+1]=true, shade[sp+w3	+2]=true;
					}
					if(shade[sp])
					{
						//	 	Dx
						//	Lx	+	Rx
						//		Ux  '
						auto		&Dx=Ycross[w2*(y-1)+x],
							&Lx=Xcross[w1*y+x-1],	&Rx=Xcross[w1*y+x],
									&Ux=Ycross[w2* y   +x];
						double d_min=1;
						if(Ux!=-1)
						{
							if(Dx!=-1)
							{
								if(Lx!=-1)
								{
									if(Rx!=-1)	//udlr	>=2 lines	\+\	/+/
									{
										double d;
										d_min=aa_close(Rx, Ux);
										if(d_min>(d=aa_close(Rx, 1-Dx)))
											d_min=d;
										if(d_min>(d=aa_close(1-Lx, 1-Dx)))
											d_min=d;
										if(d_min>(d=aa_close(1-Lx, Ux)))
											d_min=d;
									}
									else		//udl	>=2 lines	<+
									{
										double d;
										d_min=aa_close(1-Lx, 1-Dx);
										if(d_min>(d=aa_close(1-Lx, Ux)))
											d_min=d;
									}
								}
								else
								{
									if(Rx!=-1)	//ud r	>=2 lines	+>
									{
										double d;
										d_min=aa_close(Rx, Ux);
										if(d_min>(d=aa_close(Rx, 1-Dx)))
											d_min=d;
									}
									else		//ud				_F_
									{
										double d;
										d_min=Ux;
										if(d_min>(d=1-Dx))
											d_min=d;
									}
								}
							}
							else
							{
								if(Lx!=-1)
								{
									if(Rx!=-1)	//u lr	>=2 lines	\+/
									{
										double d;
										d_min=aa_close(Rx, 1-Dx);
										if(d_min>(d=aa_close(1-Lx, 1-Dx)))
											d_min=d;
									}
									else		//u l	>=1 line	\+
										d_min=aa_close(1-Lx, Ux);
								}
								else
								{
									if(Rx!=-1)	//u  r	>=1 line	+/
										d_min=aa_close(Rx, Ux);
									else		//u		>=1 line	_+_
									{
										auto &LUx=Ycross[w2*y	+x-1], &RUx=Ycross[w2*y	+x+1];
										double d;
										d_min=Ux;
										if(LUx!=-1&&LUx<Ux&&d_min>(d=aa_middle(Ux, LUx)))
											d_min=d;
										if(RUx!=-1&&RUx<Ux&&d_min>(d=aa_middle(Ux, RUx)))
											d_min=d;
									}
								}
							}
						}
						else
						{
							if(Dx!=-1)
							{
								if(Lx!=-1)
								{
									if(Rx!=-1)	// dlr	>=2 lines	/+\ 
									{
										double d;
										d_min=aa_close(Rx, Ux);
										if(d_min>(d=aa_close(1-Lx, Ux)))
											d_min=d;
									}
									else		// dl	>=1 line	/+
										d_min=aa_close(1-Lx, 1-Dx);
								}
								else
								{
									if(Rx!=-1)	// d r	>=1 line	+\ 
										d_min=aa_close(Rx, 1-Dx);
									else		// d	>=1 line	F
									{
										auto &LDx=Ycross[w2*(y-1)+x-1], &RDx=Ycross[w2*(y-1)+x+1];
										double d;
										d_min=1-Dx;
										if(LDx!=-1&&LDx>Dx&&d_min>(d=aa_middle(1-Dx, 1-LDx)))
											d_min=d;
										if(RDx!=-1&&RDx>Dx&&d_min>(d=aa_middle(1-Dx, 1-RDx)))
											d_min=d;
									}
								}
							}
							else
							{
								if(Lx!=-1)
								{
									if(Rx!=-1)	//  lr	>=1 line
									{
										double d;
										d_min=Rx;
										if(d_min>(d=1-Lx))
											d_min=d;
									}
									else		//  l	>=1 line	|+
									{
										auto &DLx=Xcross[w1*(y-1)+x-1], &ULx=Xcross[w1*(y+1)+x-1];
										double d;
										d_min=1-Lx;
										if(DLx!=-1&&DLx>Lx&&d_min>(d=aa_middle(1-Lx, 1-DLx)))
											d_min=d;
										if(ULx!=-1&&ULx>Lx&&d_min>(d=aa_middle(1-Lx, 1-ULx)))
											d_min=d;
									}
								}
								else
								{
									if(Rx!=-1)	//   r	>=1 line	+|
									{
										auto &DRx=Xcross[w1*(y-1)+x  ], &URx=Xcross[w1*(y+1)+x  ];
										double d;
										d_min=Rx;
										if(DRx!=-1&&DRx<Rx&&d_min>(d=aa_middle(Rx, DRx)))
											d_min=d;
										if(URx!=-1&&URx<Rx&&d_min>(d=aa_middle(Rx, URx)))
											d_min=d;
									}
									else		//		>=0 lines
									{
										auto
											&DLx=Xcross[w1*(y-1)+x-1],	&LDx=Ycross[w2*(y-1)+x-1],//	 _    _
											&DRx=Xcross[w1*(y-1)+x  ],	&RDx=Ycross[w2*(y-1)+x+1],//	| 1  2 | D
											&ULx=Xcross[w1*(y+1)+x-1],	&LUx=Ycross[w2* y   +x-1],//	    .
											&URx=Xcross[w1*(y+1)+x  ],	&RUx=Ycross[w2* y   +x+1];//	|_3  4_| U
										double d;
										if(DLx!=-1&&LDx!=-1&&d_min>(d=aa_far(1-DLx, 1-LDx)))//1
											d_min=d;
										if(DRx!=-1&&RDx!=-1&&d_min>(d=aa_far(  DRx, 1-RDx)))//2
											d_min=d;
										if(ULx!=-1&&LUx!=-1&&d_min>(d=aa_far(1-ULx,   LUx)))//3
											d_min=d;
										if(URx!=-1&&RUx!=-1&&d_min>(d=aa_far(  URx,   RUx)))//4
											d_min=d;
										if(d_min==1)
											continue;
										else
											d_min=d_min>aa_thickness?1:d_min*_1_aa_thickness;
									}
								}
							}
						}
						auto prgb=(unsigned char*)&rgb[w*(y-1)	+x-1];
						prgb[0]=unsigned char(prgb[0]*d_min);
						prgb[1]=unsigned char(prgb[1]*d_min);
						prgb[2]=unsigned char(prgb[2]*d_min);
					}
				}
			}
		}
	}
#endif
#if 0//draw curve		aa
	{
		//XY-plane		<u> crosses xy-plane
		int w1=w+1, h1=h+1, rgbn11=w1*h1;
		std::vector<int> hit(rgbn11);//20 fps
	//	std::vector<int> hit(h1);//19 fps
		std::vector<double> hitX(rgbn11), hitY(rgbn11), hitF(rgbn11);//where pixel rays hit the x-y plane
		double t=-camz;
		double ttfov=t*tanfov, tfov_x0=tanfov/X0, ttfov_x0=t*tfov_x0, y0_x0=double(Y0)/X0;
		double A2=-tfov_x0*cay/t, A3=(tanfov*y0_x0*cay+say)/t;
		double	B1=0,					B2=-tfov_x0*cay,				B3=tanfov*y0_x0*cay+say,
				B4=ttfov_x0*sax,		B5=ttfov_x0*cax*say+B2*camx,	B6=t*(-tanfov*(sax+y0_x0*cax*say)+cax*cay)+B3*camx,
				B7=-ttfov_x0*cax,		B8=ttfov_x0*sax*say+B2*camy,	B9=t*( tanfov*(cax-y0_x0*sax*say)+sax*cay)+B3*camy;
		
		double _pi=::acos(-1.);
		auto _1d_int_in_range=[](double x0, double x1){return std::floor(x0)!=std::floor(x1)||std::ceil(x0)!=std::ceil(x1);};
		auto disc=[&](double x0, double x1)
		{
		//	return std::abs(x1-x0)>3.2||_1d_int_in_range(x0/_pi-.5, x1/_pi-.5);		//tan x
			return false;															//cos x
		};

		for(int ys=0;ys<h1;++ys)
		{
			int p=w1*ys;
			if(A2*ys+A3>0)//_1_Zcp>0
			{
				double den=B2*ys+B3;//B1=0
				double B56=B5*ys+B6, B89=B8*ys+B9;
				for(int xs=0;xs<w1;++xs, ++p)
				{
					hit[p]=1;
					hitX[p]=(B4*xs+B56)/den,
					hitY[p]=(B7*xs+B89)/den;

				//	hitF[p]=0;
				//	hitF[p]=hitX[p];
				//	hitF[p]=std::tan(hitX[p]);
				//	hitF[p]=100*std::cos(10*hitX[p]/100)*std::cos(hitX[p]/100)+cos(hitX[p]);
				//	hitF[p]=std::cos(10*hitX[p])*std::cos(hitX[p]);
				//	hitF[p]=std::cos(hitX[p]*hitX[p])*std::cos(hitX[p]);
				//	hitF[p]=hitX[p]*std::cos(hitX[p]);
				//	hitF[p]=std::sin(1/hitX[p]);
				//	hitF[p]=1/std::sin(hitX[p]);
				//	hitF[p]=std::sqrt(std::abs(hitX[p]));
				//	hitF[p]=std::tan(hitX[p]);
				//	hitF[p]=std::exp(hitX[p]);
					hitF[p]=std::cos(hitX[p]);
				}
			}
		}
	/*	for(int ys=0;ys<h1;++ys)
		{
			if(hit[ys]=A2*ys+A3>0)//_1_Zcp>0
			{
				int p=w1*ys;
				double den=B2*ys+B3;//B1=0
				double B56=B5*ys+B6, B89=B8*ys+B9;
				for(int xs=0;xs<w1;++xs, ++p)
				{
					hitX[p]=(B4*xs+B56)/den,
					hitY[p]=(B7*xs+B89)/den;

					hitF[p]=std::tan(hitX[p]);
				//	hitF[p]=100*std::cos(10*hitX[p]/100)*std::cos(hitX[p]/100)+cos(hitX[p]);
				//	hitF[p]=std::cos(10*hitX[p])*std::cos(hitX[p]);
				//	hitF[p]=std::cos(hitX[p]*hitX[p])*std::cos(hitX[p]);
				//	hitF[p]=hitX[p]*std::cos(hitX[p]);
				//	hitF[p]=std::sin(1/hitX[p]);
				//	hitF[p]=1/std::sin(hitX[p]);
				//	hitF[p]=std::sqrt(std::abs(hitX[p]));
				//	hitF[p]=std::tan(hitX[p]);
				//	hitF[p]=std::exp(hitX[p]);
				//	hitF[p]=std::cos(hitX[p]);
				}
			}
		}//*/
		
		const double aa_thickness=1,
				
			_1_aa_thickness=1/aa_thickness;
		const int aa_bound=int(std::round(aa_thickness));

		int XCsize=w*h1, YCsize=w1*h;
		std::vector<double>
			Xcross(XCsize),//._
			Ycross(YCsize);//!
		std::vector<int> lPixels(rgbn);
		unsigned nPixels=0;
			std::vector<bool> visited(rgbn);
			
		for(int ys=aa_bound, yEnd=h-aa_bound;ys<yEnd;++ys)
		{
			for(int xs=aa_bound, xEnd=w-aa_bound;xs<xEnd;++xs)
			{
				int p=w1*ys+xs;
				if(	hit[p]&&			hit[p+1]&&
					hitY[p]>hitF[p]!=	hitY[p+1]>hitF[p+1]&&
					!disc(hitX[p], hitX[p+1]))
				{
					double d0=hitY[p]-hitF[p], d1=hitY[p+1]-hitF[p+1];
					Xcross[w*ys+xs]=(0-d0)/(d1-d0)-.5;
					if(!visited[p-w1-1])
						lPixels[nPixels++]=short(ys-1)<<16|short(xs-1), visited[p-w1-1]=true;
					if(!visited[p-w1  ])
						lPixels[nPixels++]=short(ys-1)<<16|short(xs  ), visited[p-w1  ]=true;
					if(!visited[p-w1+1])
						lPixels[nPixels++]=short(ys-1)<<16|short(xs+1), visited[p-w1+1]=true;
					if(!visited[p   -1])
						lPixels[nPixels++]=short(ys  )<<16|short(xs-1), visited[p   -1]=true;
					if(!visited[p     ])
						lPixels[nPixels++]=short(ys  )<<16|short(xs  ), visited[p     ]=true;
					if(!visited[p   +1])
						lPixels[nPixels++]=short(ys  )<<16|short(xs+1), visited[p   +1]=true;
				}
				else
					Xcross[w*ys+xs]=-1;
				if(	hit[p]&&			hit[p+w1]&&
					hitY[p]>hitF[p]!=	hitY[p+w1]>hitF[p+w1]&&
					!disc(hitX[p], hitX[p+w1]))
				{
					double d0=hitY[p]-hitF[p], d1=hitY[p+w1]-hitF[p+w1];
					Ycross[p]=(0-d0)/(d1-d0)-.5;
					if(!visited[p-w1-1])
						lPixels[nPixels++]=short(ys-1)<<16|short(xs-1), visited[p-w1-1]=true;
					if(!visited[p-w1  ])
						lPixels[nPixels++]=short(ys-1)<<16|short(xs  ), visited[p-w1  ]=true;
					if(!visited[p-w1+1])
						lPixels[nPixels++]=short(ys-1)<<16|short(xs+1), visited[p-w1+1]=true;
					if(!visited[p   -1])
						lPixels[nPixels++]=short(ys  )<<16|short(xs-1), visited[p   -1]=true;
					if(!visited[p     ])
						lPixels[nPixels++]=short(ys  )<<16|short(xs  ), visited[p     ]=true;
					if(!visited[p   +1])
						lPixels[nPixels++]=short(ys  )<<16|short(xs+1), visited[p   +1]=true;
				}
				else
					Ycross[p]=-1;
			}
#if 0
			if(hit[ys])
			{
				for(int x=aa_bound, xEnd=w-aa_bound;x<xEnd;++x)
				{
					int p=w1*ys+x;
					if(	hitY[p]>hitF[p]!=	hitY[p+1]>hitF[p+1]&&
						!disc(hitX[p], hitX[p+1]))
					{
						double d0=hitY[p]-hitF[p], d1=hitY[p+1]-hitF[p+1];
						Xcross[w*ys+x]=(0-d0)/(d1-d0)-.5;
						if(!visited[p-w1-1])
							lPixels[nPixels++]=short(ys-1)<<16|short(x-1), visited[p-w1-1]=true;
						if(!visited[p-w1  ])
							lPixels[nPixels++]=short(ys-1)<<16|short(x  ), visited[p-w1  ]=true;
						if(!visited[p-w1+1])
							lPixels[nPixels++]=short(ys-1)<<16|short(x+1), visited[p-w1+1]=true;
						if(!visited[p   -1])
							lPixels[nPixels++]=short(ys  )<<16|short(x-1), visited[p   -1]=true;
						if(!visited[p     ])
							lPixels[nPixels++]=short(ys  )<<16|short(x  ), visited[p     ]=true;
						if(!visited[p   +1])
							lPixels[nPixels++]=short(ys  )<<16|short(x+1), visited[p   +1]=true;
					}
					else
						Xcross[w*ys+x]=-1;
					if(hit[ys+1])
					{
						if(	hitY[p]>hitF[p]!=	hitY[p+w1]>hitF[p+w1]&&
							!disc(hitX[p], hitX[p+w1]))
						{
							double d0=hitY[p]-hitF[p], d1=hitY[p+w1]-hitF[p+w1];
							Ycross[p]=(0-d0)/(d1-d0)-.5;
							if(!visited[p-w1-1])
								lPixels[nPixels++]=short(ys-1)<<16|short(x-1), visited[p-w1-1]=true;
							if(!visited[p-w1  ])
								lPixels[nPixels++]=short(ys-1)<<16|short(x  ), visited[p-w1  ]=true;
							if(!visited[p-w1+1])
								lPixels[nPixels++]=short(ys-1)<<16|short(x+1), visited[p-w1+1]=true;
							if(!visited[p   -1])
								lPixels[nPixels++]=short(ys  )<<16|short(x-1), visited[p   -1]=true;
							if(!visited[p     ])
								lPixels[nPixels++]=short(ys  )<<16|short(x  ), visited[p     ]=true;
							if(!visited[p   +1])
								lPixels[nPixels++]=short(ys  )<<16|short(x+1), visited[p   +1]=true;
						}
						else
							Ycross[p]=-1;
					}
				}
			/*	if(hit[ys+1])
				{
					for(int x=aa_bound, xEnd=w-aa_bound;x<xEnd;++x)
					{
						int p=w1*ys+x;
						if(	hitY[p]>hitF[p]!=	hitY[p+w1]>hitF[p+w1]&&
							!disc(hitX[p], hitX[p+w1]))
						{
							double d0=hitY[p]-hitF[p], d1=hitY[p+w1]-hitF[p+w1];
							Ycross[p]=(0-d0)/(d1-d0)-.5;
							if(!visited[p-w1-1])
								lPixels[nPixels++]=short(ys-1)<<16|short(x-1), visited[p-w1-1]=true;
							if(!visited[p-w1  ])
								lPixels[nPixels++]=short(ys-1)<<16|short(x  ), visited[p-w1  ]=true;
							if(!visited[p-w1+1])
								lPixels[nPixels++]=short(ys-1)<<16|short(x+1), visited[p-w1+1]=true;
							if(!visited[p   -1])
								lPixels[nPixels++]=short(ys  )<<16|short(x-1), visited[p   -1]=true;
							if(!visited[p     ])
								lPixels[nPixels++]=short(ys  )<<16|short(x  ), visited[p     ]=true;
							if(!visited[p   +1])
								lPixels[nPixels++]=short(ys  )<<16|short(x+1), visited[p   +1]=true;
						}
						else
							Ycross[p]=-1;
					}
				}//*/
			}
#endif
		}
		auto aa_fn_DU=[&](double Dx, double Ux)
		{
			double b=Ux-Dx;
			double d=std::abs(.5*(Ux+Dx))*inv_sqrt(1+b*b);//exact
			return d>aa_thickness?1:d*_1_aa_thickness;
		};
		auto aa_fn_LD=[&](double Lx, double Dx)
		{
			double a=Lx+.5, b=Dx+.5;
			double d=std::abs(.25-Lx*Dx)*inv_sqrt(a*a+b*b);
			return d>aa_thickness?1:d*_1_aa_thickness;
		};
		auto d_fn_UL_UR=[&](double ULx, double URx)
		{
			double a=ULx-URx, sum=ULx+URx;
			return (sum+2)*(3.23333-a*a)*.15-0.00574;//error [0, 13] /255
		};
		auto d_fn_UR_RU=[&](double URx, double RUx)
		{
			double x1=.5, y1=1+URx, x2=1+RUx, y2=.5;
			double a=.5+URx, b=.5+RUx, c=.25-x2*y1;
			double aa_bb=a*a+b*b, t0=c/aa_bb;
			if(-a*t0<x1)
				return std::sqrt(.25+y1*y1);
			if(-b*t0<y2)
				return std::sqrt(x2*x2+.25);
			return std::abs(c)*inv_sqrt(aa_bb);
		};

	//	std::vector<double> rgbPixels(rgbn);
		for(unsigned p=0;p<nPixels;++p)
		{
			auto &yx=lPixels[p];
			int x=((short*)&yx)[0], y=((short*)&yx)[1];
			auto
				&Dx=Xcross[w *y+x],	&Ux=Xcross[w *(y+1)+ x   ],
				&Lx=Ycross[w1*y+x],	&Rx=Ycross[w1* y   +(x+1)];

			double A=0;
			if(Ux!=-1)
			{
				if(Dx!=-1)
				{
					if(Lx!=-1)
					{
						if(Rx!=-1)	//udlr	>=2 lines	[//]	[\\]	[+]
							continue;//
						else		//udl	>=2 lines	[\|]	[/|]	[<]
							continue;//
					}
					else
					{
						if(Rx!=-1)	//ud r	>=2 lines	[|/]	[|\]	[>]
							continue;//
						else		//ud	>=1 line	[|]
							A=aa_fn_DU(Dx, Ux);
					}
				}
				else
				{
					if(Lx!=-1)
					{
						if(Rx!=-1)	//u lr	>=2 lines	[_\]	[/_]	[^]
							continue;//
						else		//u l	>=1 line	[/]
							A=aa_fn_LD(-Lx, Ux);
					}
					else
					{
						if(Rx!=-1)	//u  r	>=1 line	[\]
							A=aa_fn_LD(-Rx, -Ux);
						else		//u		>=1 line	unreachable
							continue;
					}
				}
			}
			else
			{
				if(Dx!=-1)
				{
					if(Lx!=-1)
					{
						if(Rx!=-1)	// dlr	>=2 lines	[7]		[\-]	[v]
							continue;//
						else		// dl	>=1 line	[\]
							A=aa_fn_LD(Lx, Dx);
					}
					else
					{
						if(Rx!=-1)	// d r	>=1 line	[/]
							A=aa_fn_LD(Rx, -Dx);
						else		// d	>=1 line	unreachable
							continue;
					}
				}
				else
				{
					if(Lx!=-1)
					{
						if(Rx!=-1)	//  lr	>=1 line	[-]
							A=aa_fn_DU(Rx, Lx);
						else		//  l	>=1 line	unreachable
							continue;
					}
					else
					{
						if(Rx!=-1)	//   r	>=1 line	unreachable
							continue;
						else		//		>=0 lines
						{
							if(x-1>=0&&x+1<w&&y-1>=0&&y+1<h)
							{
								auto
									&ULx=Ycross[w1*(y+1)+x  ],	&URx=Ycross[w1*(y+1)+x+1],
									&LUx=Xcross[w *(y+1)+x-1],	&RUx=Xcross[w *(y+1)+x+1],//7_|8|_9
									&LDx=Xcross[w * y   +x-1],	&RDx=Xcross[w * y   +x+1],//4_ + _6
									&DLx=Ycross[w1*(y-1)+x  ],	&DRx=Ycross[w *(y-1)+x+1];//1 |2| 3
								double d_min=_HUGE;
								if(ULx!=-1&&URx!=-1)//8
								{
									double d=d_fn_UL_UR(ULx, URx);
									if(d_min>d)
										d_min=d;
								}
								if(URx!=-1&&RUx!=-1)//9
								{
									double d=d_fn_UR_RU(URx, RUx);
									if(d_min>d)
										d_min=d;
								}
								if(RUx!=-1&&RDx!=-1)//6
								{
									double d=d_fn_UL_UR(RUx, RDx);
									if(d_min>d)
										d_min=d;
								}
								if(RDx!=-1&&DRx!=-1)//3
								{
									double d=d_fn_UR_RU(RDx, -DRx);
									if(d_min>d)
										d_min=d;
								}
								if(DRx!=-1&&DLx!=-1)//2
								{
									double d=d_fn_UL_UR(-DRx, -DLx);
									if(d_min>d)
										d_min=d;
								}
								if(DLx!=-1&&LDx!=-1)//1
								{
									double d=d_fn_UR_RU(-DLx, -LDx);
									if(d_min>d)
										d_min=d;
								}
								if(LDx!=-1&&LUx!=-1)//4
								{
									double d=d_fn_UL_UR(-LDx, -LUx);
									if(d_min>d)
										d_min=d;
								}
								if(LUx!=-1&&ULx!=-1)//7
								{
									double d=d_fn_UR_RU(-LUx, ULx);
									if(d_min>d)
										d_min=d;
								}
								if(d_min!=_HUGE)
									A=d_min>aa_thickness?1:d_min*_1_aa_thickness;
								else
									continue;
							}
						}
					}
				}
			}
			auto prgb=(unsigned char*)&rgb[w*y+x];
			prgb[0]=unsigned char(prgb[0]*A);
			prgb[1]=unsigned char(prgb[1]*A);
			prgb[2]=unsigned char(prgb[2]*A);
		}
	}
#endif
#if 0//draw curve		pixelated
	{
		//<u> crosses xy-plane
		int w1=w+1, h1=h+1, rgbn11=w1*h1;
		std::vector<int> hit(h1);
		std::vector<double> hitX(rgbn11), hitLR(rgbn11);
	//	std::vector<double> hitX(rgbn11), hitY(rgbn11), hitF(rgbn11);
		double t=-camz;
		double ttfov=t*tanfov, tfov_x0=tanfov/X0, ttfov_x0=t*tfov_x0, y0_x0=double(Y0)/X0;
		double A2=-tfov_x0*cay/t, A3=(tanfov*y0_x0*cay+say)/t;
		double	B1=0,					B2=-tfov_x0*cay,				B3=tanfov*y0_x0*cay+say,
				B4=ttfov_x0*sax,		B5=ttfov_x0*cax*say+B2*camx,	B6=t*(-tanfov*(sax+y0_x0*cax*say)+cax*cay)+B3*camx,
				B7=-ttfov_x0*cax,		B8=ttfov_x0*sax*say+B2*camy,	B9= t*(tanfov*(cax-y0_x0*sax*say)+sax*cay)+B3*camy;
		
		double _pi=::acos(-1.);
		auto _1d_int_in_range=[](double x0, double x1){return std::floor(x0)!=std::floor(x1)||std::ceil(x0)!=std::ceil(x1);};
		auto disc=[&](double x0, double x1)
		{
			//tan
		//	if(std::abs(x1-x0)>3.2)	return true;
		//	return _1d_int_in_range(x0/_pi-.5, x1/_pi-.5);

			//cos
			return false;
		};
		for(int ys=0;ys<h1;++ys)
		{
			if(hit[ys]=A2*ys+A3>0)//_1_Zcp>0
			{
				double den=B2*ys+B3;//B1=0
				double B56=B5*ys+B6, B89=B8*ys+B9;
				for(int xs=0, p=w1*ys;xs<w1;++xs, ++p)
				{
					hitX[p]=(B4*xs+B56)/den;
					hitLR[p]=(B7*xs+B89)/den - std::cos(hitX[p]);
				//	hitY[p]=(B7*xs+B89)/den;
					
				//	hitF[p]=std::tan(hitX[p]);

				//	hitF[p]=100*cos(10*hitX[p]/100)*cos(hitX[p]/100)+cos(hitX[p]);
				//	hitF[p]=std::cos(hitX[p]);
				}
			}
		}
		for(int ys=0;ys<h;++ys)
		{
			if(hit[ys])
			{
				for(int xs=0, p=w1*ys;xs<w;++xs, ++p)
				{
					if(hitLR[p]>0!=hitLR[p+1]>0&&!disc(hitX[p], hitX[p+1]))
				//	if(hitY[p]>hitF[p]!=hitY[p+1]>hitF[p+1]&&!disc(hitX[p], hitX[p+1]))
					{
						rgb[w*ys+xs]=0;
						if(ys>0)
							rgb[w*(ys-1)+xs]=0;
					}
				}
				if(hit[ys+1])
				{
					for(int xs=0, p=w1*ys;xs<w;++xs, ++p)
					{
						if(hitLR[p]>0!=hitLR[p+w1]>0&&!disc(hitX[p], hitX[p+w1]))
						{
							rgb[w*ys+xs]=0;
							if(xs>0)
								rgb[w*ys+xs-1]=0;
						}
					}
				}
			}
		}
	}
#endif
#if 0//draw curve		aa
	{
		//static int _h=0, _w=0;
		//static double *hit=0, *Xcross=0, *Ycross=0;
		//if(_h!=h||_w!=w)
		//{
		//	hit		=(double*)realloc(hit	, (_h=h)*sizeof(double));
		//	Xcross	=(double*)realloc(Xcross, (_h=h)*sizeof(double));
		//	Ycross	=(double*)realloc(Ycross, (_h=h)*sizeof(double));
		//}

		//<u> crosses xy-plane
		int w1=w+1, h1=h+1, rgbn11=w1*h1;
		std::vector<int> hit(rgbn11);
		std::vector<double> hitX(rgbn11), hitY(rgbn11), hitF(rgbn11);
		double t=-camz;
		double ttfov=t*tanfov, tfov_x0=tanfov/X0, ttfov_x0=t*tfov_x0, y0_x0=double(Y0)/X0;
		double A2=-tfov_x0*cay/t, A3=(tanfov*y0_x0*cay+say)/t;
		double	B1=0,					B2=-tfov_x0*cay,				B3=tanfov*y0_x0*cay+say,
				B4=ttfov_x0*sax,		B5=ttfov_x0*cax*say+B2*camx,	B6=t*(-tanfov*(sax+y0_x0*cax*say)+cax*cay)+B3*camx,
				B7=-ttfov_x0*cax,		B8=ttfov_x0*sax*say+B2*camy,	B9= t*(tanfov*(cax-y0_x0*sax*say)+sax*cay)+B3*camy;

	/*	double	B1=tfov_x0*cax*cay,				B2=-tfov_x0*sax*cay,				B3=-tanfov*(cax-y0_x0*sax)*cay-say,//den
				B4=-ttfov_x0*sax	+B1*camx,	B5=-ttfov_x0*cax		+B2*camx,	B6=ttfov*(sax+y0_x0*cax)				+B3*camx,//Pxe_x
				B7=-ttfov_x0*cax*say+B1*camy,	B8=ttfov_x0*sax*say		+B2*camy,	B9=t*(tanfov*(cax-y0_x0*sax)*say-cay)	+B3*camy;//Pxe_y
	//	double	B10=ttfov_x0*cax*cay+B1*camz,	B11=-ttfov_x0*sax*cay	+B2*camz,	B12=t*(-tanfov*(cax-y0_x0*sax)*cay-say)	+B3*camz;//Pxe_z=0//*/
	/*	double	B1=tfov_x0*cax*cay,		B2=-tfov_x0*sax*cay,	B3=-tanfov*(cax-y0_x0*sax)*cay-say,//den
				B4=-ttfov_x0*sax,		B5=-ttfov_x0*cax,		B6=ttfov*(sax+y0_x0*cax),//Pxe_x
				B7=-ttfov_x0*cax*say,	B8=ttfov_x0*sax*say,	B9=t*(tanfov*(cax-y0_x0*sax)*say-cay);//Pxe_y
	//	double	B10=ttfov_x0*cax*cay,	B11=-ttfov_x0*sax*cay,	B12=t*(-tanfov*(cax-y0_x0*sax)*cay-say);//Pxe_z=0//*/
	/*	double
			B1=tfov_x0*cax*cay,		B2=-tfov_x0*-sax*cay,	B3=-tanfov*(cax-y0_x0*-sax)*cay+say,//den
			B4=-ttfov_x0*-sax,		B5=-ttfov_x0*cax,		B6=ttfov*(-sax+y0_x0*cax),//Pxe_x
			B7=-ttfov_x0*cax*-say,	B8=ttfov_x0*-sax*-say,	B9=t*(tanfov*(cax-y0_x0*-sax)*-say-cay);//Pxe_y
		//	B10=ttfov_x0*cax*cay,	B11=-ttfov_x0*sax*cay,	B12=t*(-tanfov*(cax-y0_x0*sax)*cay-say);//Pxe_z=0//LOL*/
		
		double _pi=::acos(-1.);
		auto _1d_int_in_range=[](double x0, double x1){return std::floor(x0)!=std::floor(x1)||std::ceil(x0)!=std::ceil(x1);};
		auto disc=[&](double x0, double x1)
		{
			//tan
		//	if(std::abs(x1-x0)>3.2)	return true;
		//	return _1d_int_in_range(x0/_pi-.5, x1/_pi-.5);

			//cos
			return false;
		};

		//cos x
		for(int ys=0;ys<h1;++ys)
		{
			int p=w1*ys;
			if(hit[p]=A2*ys+A3>0)//_1_Zcp>0
			{
				double den=B2*ys+B3;//B1=0
				double B56=B5*ys+B6, B89=B8*ys+B9;
				for(int xs=0;xs<w1;++xs, ++p)
				{
					hit[p]=1;
					hitX[p]=(B4*xs+B56)/den,
					hitY[p]=(B7*xs+B89)/den;

				//	hitF[p]=std::tan(hitX[p]);
				//	hitF[p]=100*std::cos(10*hitX[p]/100)*std::cos(hitX[p]/100)+cos(hitX[p]);
					hitF[p]=std::cos(hitX[p]);
				}

			//	double B23=B2*ys+B3, B56=B5*ys+B6, B89=B8*ys+B9;
			//	for(int xs=0;xs<w1;++xs, ++p)
			//	{
			//		hit[p]=1;
			//		double den=B1*xs+B23;
			//		hitX[p]=(B4*xs+B56)/den,
			//		hitY[p]=(B7*xs+B89)/den;

			//		hitF[p]=100*std::cos(10*hitX[p]/100)*std::cos(hitX[p]/100)+cos(hitX[p]);
				//	hitF[p]=std::cos(10*hitX[p])*std::cos(hitX[p]);
				//	hitF[p]=std::cos(hitX[p]*hitX[p])*std::cos(hitX[p]);
				//	hitF[p]=hitX[p]*std::cos(hitX[p]);
				//	hitF[p]=std::sin(1/hitX[p]);
				//	hitF[p]=1/std::sin(hitX[p]);
				//	hitF[p]=std::sqrt(std::abs(hitX[p]));
				//	hitF[p]=std::tan(hitX[p]);
				//	hitF[p]=std::exp(hitX[p]);
				//	hitF[p]=std::cos(hitX[p]);

				//	double Z=(B10*xs+B11*ys+B12)/den;
 				//	Z=Z;//*/
				/*	double den=B1*xs+B23;
					hit[p]=1;
					hitX[p]=(B4*xs+B56)/den+camx,
					hitY[p]=(B7*xs+B89)/den+camy;
					hitF[p]=std::cos(hitX[p]);
				//	double Z=(B10*xs+B11*ys+B12)/den+camz;
 				//	Z=Z;//*/
				/*	if(xs<w&&ys<h)
					{
						auto prgb=(unsigned char*)&rgb[w*ys+xs];
						prgb[0]*=hitX[p]/100, prgb[1]*=hitX[p]/100, prgb[2]*=hitX[p]/100;
					//	prgb[0]*=hitY[p]/500, prgb[1]*=hitY[p]/500, prgb[2]*=hitY[p]/500;

					//	(unsigned&)(rgb[w*ys+xs])*=hitX[p]/255/255/255/10;

					//	if(hitY[p]>hitF[p])
					//		prgb[0]*=hitY[p]/100, prgb[1]*=hitY[p]/100, prgb[2]*=hitY[p]/100;
					//	else
					//		prgb[0]*=hitX[p]/100, prgb[1]*=hitX[p]/100, prgb[2]*=hitX[p]/100;

					//	prgb[0]/=hitY[p], prgb[1]/=hitY[p], prgb[2]/=hitY[p];

					//	if(hitY[p]>hitF[p])
					//		prgb[0]*=.5, prgb[1]*=.5, prgb[2]*=.5;
					//	else
					//		prgb[0]*=.7, prgb[1]*=.7, prgb[2]*=.7;
					}//*/
			//	}
			}
		}
		
		const double aa_thickness=1,
				
			_1_aa_thickness=1/aa_thickness;
		const int aa_bound=int(std::round(aa_thickness));

		int XCsize=w*h1, YCsize=w1*h;
		std::vector<double>
			Xcross(XCsize),//._
			Ycross(YCsize);//!
		std::vector<int> lPixels(rgbn);
		unsigned nPixels=0;
			std::vector<bool> visited(rgbn);
			
		for(int y=aa_bound, yEnd=h-aa_bound;y<yEnd;++y)
		{
			for(int x=aa_bound, xEnd=w-aa_bound;x<xEnd;++x)
			{
				int p=w1*y+x;
				if(	hit[p]&&			hit[p+1]&&
					hitY[p]>hitF[p]!=	hitY[p+1]>hitF[p+1]&&
					!disc(hitX[p], hitX[p+1]))
				{
					double d0=hitY[p]-hitF[p], d1=hitY[p+1]-hitF[p+1];
					Xcross[w*y+x]=(0-d0)/(d1-d0)-.5;
					if(!visited[p-w1-1	])
						lPixels[nPixels++]=short(y-1)<<16|short(x-1), visited[p-w1	-1	]=true;
					if(!visited[p-w1	])
						lPixels[nPixels++]=short(y-1)<<16|short(x  ), visited[p-w1		]=true;
					if(!visited[p-w1+1	])
						lPixels[nPixels++]=short(y-1)<<16|short(x+1), visited[p-w1	+1	]=true;
					if(!visited[p	-1	])
						lPixels[nPixels++]=short(y  )<<16|short(x-1), visited[p		-1	]=true;
					if(!visited[p		])
						lPixels[nPixels++]=short(y  )<<16|short(x  ), visited[p			]=true;
					if(!visited[p	+1	])
						lPixels[nPixels++]=short(y  )<<16|short(x+1), visited[p		+1	]=true;
				}
				else
					Xcross[w*y+x]=-1;
				if(	hit[p]&&			hit[p+w1]&&
					hitY[p]>hitF[p]!=	hitY[p+w1]>hitF[p+w1]&&
					!disc(hitX[p], hitX[p+w1]))
				{
					double d0=hitY[p]-hitF[p], d1=hitY[p+w1]-hitF[p+w1];
					Ycross[p]=(0-d0)/(d1-d0)-.5;
					if(!visited[p-w1-1	])
						lPixels[nPixels++]=short(y-1)<<16|short(x-1), visited[p-w1	-1	]=true;
					if(!visited[p-w1	])
						lPixels[nPixels++]=short(y-1)<<16|short(x  ), visited[p-w1		]=true;
					if(!visited[p-w1+1	])
						lPixels[nPixels++]=short(y-1)<<16|short(x+1), visited[p-w1	+1	]=true;
					if(!visited[p	-1	])
						lPixels[nPixels++]=short(y  )<<16|short(x-1), visited[p		-1	]=true;
					if(!visited[p		])
						lPixels[nPixels++]=short(y  )<<16|short(x  ), visited[p			]=true;
					if(!visited[p	+1	])
						lPixels[nPixels++]=short(y  )<<16|short(x+1), visited[p		+1	]=true;
				}
				else
					Ycross[p]=-1;
			/*	int ndrP=w*y+x;
				auto
					&v00=(&ndr[ndrP		].r)[component], &v01=(&ndr[ndrP+1].r)[component],
					&v10=(&ndr[ndrP+w1	].r)[component];
				if(std::signbit(v00)==std::signbit(v01))
					Xcross[w*y+x]=-1;
				else
				{
					Xcross[w*y+x]=(0-v00)/(v01-v00)-.5;//zero cross
					//123
					//456
					if(!visited[ndrP-w1-1])
						lPixels[nPixels++]=short(y-1)<<16|short(x-1), visited[ndrP-w1-1]=true;
					if(!visited[ndrP-w1])
						lPixels[nPixels++]=short(y-1)<<16|short(x  ), visited[ndrP-w1]=true;
					if(!visited[ndrP-w1+1])
						lPixels[nPixels++]=short(y-1)<<16|short(x+1), visited[ndrP-w1+1]=true;
					if(!visited[ndrP-1])
						lPixels[nPixels++]=short(y  )<<16|short(x-1), visited[ndrP-1]=true;
					if(!visited[ndrP])
						lPixels[nPixels++]=short(y  )<<16|short(x  ), visited[ndrP]=true;
					if(!visited[ndrP+1])
						lPixels[nPixels++]=short(y  )<<16|short(x+1), visited[ndrP+1]=true;
				}
				if(std::signbit(v00)==std::signbit(v10))
					Ycross[ndrP]=-1;
				else
				{
					Ycross[ndrP]=(0-v00)/(v10-v00)-.5;//zero cross
				//	Ycross[ndrP]=(0-v00)/(v10-v00);
					//12
					//34
					//56
					if(!visited[ndrP-Xplaces-1])
						lPixels[nPixels++]=short(y-1)<<16|short(x-1), visited[ndrP-Xplaces-1]=true;
					if(!visited[ndrP-Xplaces])
						lPixels[nPixels++]=short(y-1)<<16|short(x  ), visited[ndrP-Xplaces]=true;
					if(!visited[ndrP-1])
						lPixels[nPixels++]=short(y  )<<16|short(x-1), visited[ndrP-1]=true;
					if(!visited[ndrP])
						lPixels[nPixels++]=short(y  )<<16|short(x  ), visited[ndrP]=true;
					if(!visited[ndrP+Xplaces-1])
						lPixels[nPixels++]=short(y+1)<<16|short(x-1), visited[ndrP+Xplaces-1]=true;
					if(!visited[ndrP+Xplaces])
						lPixels[nPixels++]=short(y+1)<<16|short(x  ), visited[ndrP+Xplaces]=true;
				}//*/
			}
		}
		auto aa_fn_DU=[&](double Dx, double Ux)
		{
			double b=Ux-Dx;
			double d=std::abs(.5*(Ux+Dx))*inv_sqrt(1+b*b);//exact
			return d>aa_thickness?1:d*_1_aa_thickness;
		};
		auto aa_fn_LD=[&](double Lx, double Dx)
		{
			double a=Lx+.5, b=Dx+.5;
			double d=std::abs(.25-Lx*Dx)*inv_sqrt(a*a+b*b);
			return d>aa_thickness?1:d*_1_aa_thickness;
		};
		auto d_fn_UL_UR=[&](double ULx, double URx)
		{
			double a=ULx-URx, sum=ULx+URx;
			return (sum+2)*(3.23333-a*a)*.15-0.00574;//error [0, 13] /255
		};
		auto d_fn_UR_RU=[&](double URx, double RUx)
		{
			double x1=.5, y1=1+URx, x2=1+RUx, y2=.5;
			double a=.5+URx, b=.5+RUx, c=.25-x2*y1;
			double aa_bb=a*a+b*b, t0=c/aa_bb;
			if(-a*t0<x1)
				return std::sqrt(.25+y1*y1);
			if(-b*t0<y2)
				return std::sqrt(x2*x2+.25);
			return std::abs(c)*inv_sqrt(aa_bb);
		};

	//	std::vector<double> rgbPixels(rgbn);
		for(unsigned p=0;p<nPixels;++p)
		{
			auto &yx=lPixels[p];
			int x=((short*)&yx)[0], y=((short*)&yx)[1];
			auto
				&Dx=Xcross[w *y+x],	&Ux=Xcross[w *(y+1)+ x   ],
				&Lx=Ycross[w1*y+x],	&Rx=Ycross[w1* y   +(x+1)];

			double A=0;
			if(Ux!=-1)
			{
				if(Dx!=-1)
				{
					if(Lx!=-1)
					{
						if(Rx!=-1)	//udlr	>=2 lines	[//]	[\\]	[+]
							continue;//
						else		//udl	>=2 lines	[\|]	[/|]	[<]
							continue;//
					}
					else
					{
						if(Rx!=-1)	//ud r	>=2 lines	[|/]	[|\]	[>]
							continue;//
						else		//ud	>=1 line	[|]
							A=aa_fn_DU(Dx, Ux);
						//	contour[0].push_back(Int_x_y_z_Double_Z(x, y, 0, aa_fn_DU(Dx, Ux)));
					}
				}
				else
				{
					if(Lx!=-1)
					{
						if(Rx!=-1)	//u lr	>=2 lines	[_\]	[/_]	[^]
							continue;//
						else		//u l	>=1 line	[/]
							A=aa_fn_LD(-Lx, Ux);
						//	contour[0].push_back(Int_x_y_z_Double_Z(x, y, 0, aa_fn_LD(-Lx, Ux)));
					}
					else
					{
						if(Rx!=-1)	//u  r	>=1 line	[\]
							A=aa_fn_LD(-Rx, -Ux);
						//	contour[0].push_back(Int_x_y_z_Double_Z(x, y, 0, aa_fn_LD(-Rx, -Ux)));
						else		//u		>=1 line	unreachable
							continue;
					}
				}
			}
			else
			{
				if(Dx!=-1)
				{
					if(Lx!=-1)
					{
						if(Rx!=-1)	// dlr	>=2 lines	[7]		[\-]	[v]
							continue;//
						else		// dl	>=1 line	[\]
							A=aa_fn_LD(Lx, Dx);
						//	contour[0].push_back(Int_x_y_z_Double_Z(x, y, 0, aa_fn_LD(Lx, Dx)));
					}
					else
					{
						if(Rx!=-1)	// d r	>=1 line	[/]
							A=aa_fn_LD(Rx, -Dx);
						//	contour[0].push_back(Int_x_y_z_Double_Z(x, y, 0, aa_fn_LD(Rx, -Dx)));
						else		// d	>=1 line	unreachable
							continue;
					}
				}
				else
				{
					if(Lx!=-1)
					{
						if(Rx!=-1)	//  lr	>=1 line	[-]
							A=aa_fn_DU(Rx, Lx);
						//	contour[0].push_back(Int_x_y_z_Double_Z(x, y, 0, aa_fn_DU(Rx, Lx)));
						else		//  l	>=1 line	unreachable
							continue;
					}
					else
					{
						if(Rx!=-1)	//   r	>=1 line	unreachable
							continue;
						else		//		>=0 lines
						{
							if(x-1>=0&&x+1<w&&y-1>=0&&y+1<h)
							{
								auto
									&ULx=Ycross[w1*(y+1)+x  ],	&URx=Ycross[w1*(y+1)+x+1],
									&LUx=Xcross[w *(y+1)+x-1],	&RUx=Xcross[w *(y+1)+x+1],//7_|8|_9
									&LDx=Xcross[w * y   +x-1],	&RDx=Xcross[w * y   +x+1],//4_ + _6
									&DLx=Ycross[w1*(y-1)+x  ],	&DRx=Ycross[w *(y-1)+x+1];//1 |2| 3
								double d_min=_HUGE;
								if(ULx!=-1&&URx!=-1)//8
								{
									double d=d_fn_UL_UR(ULx, URx);
									if(d_min>d)
										d_min=d;
								}
								if(URx!=-1&&RUx!=-1)//9
								{
									double d=d_fn_UR_RU(URx, RUx);
									if(d_min>d)
										d_min=d;
								}
								if(RUx!=-1&&RDx!=-1)//6
								{
									double d=d_fn_UL_UR(RUx, RDx);
									if(d_min>d)
										d_min=d;
								}
								if(RDx!=-1&&DRx!=-1)//3
								{
									double d=d_fn_UR_RU(RDx, -DRx);
									if(d_min>d)
										d_min=d;
								}
								if(DRx!=-1&&DLx!=-1)//2
								{
									double d=d_fn_UL_UR(-DRx, -DLx);
									if(d_min>d)
										d_min=d;
								}
								if(DLx!=-1&&LDx!=-1)//1
								{
									double d=d_fn_UR_RU(-DLx, -LDx);
									if(d_min>d)
										d_min=d;
								}
								if(LDx!=-1&&LUx!=-1)//4
								{
									double d=d_fn_UL_UR(-LDx, -LUx);
									if(d_min>d)
										d_min=d;
								}
								if(LUx!=-1&&ULx!=-1)//7
								{
									double d=d_fn_UR_RU(-LUx, ULx);
									if(d_min>d)
										d_min=d;
								}
								if(d_min!=_HUGE)
									A=d_min>aa_thickness?1:d_min*_1_aa_thickness;
								//	contour[0].push_back(Int_x_y_z_Double_Z(x, y, 0, d_min>aa_thickness?1:d_min*_1_aa_thickness));
								else
									continue;
							}
						}
					}
				}
			}
			auto prgb=(unsigned char*)&rgb[w*y+x];
			prgb[0]=unsigned char(prgb[0]*A);
			prgb[1]=unsigned char(prgb[1]*A);
			prgb[2]=unsigned char(prgb[2]*A);
		}

	/*	for(int ys=0, pc=0, p=0;ys<h;++ys, ++pc)
		{
			for(int xs=0;xs<h;++xs, ++pc, ++p)
			{
				auto
					&h00=hit[pc   ], &h01=hit[pc   +1],
					&h10=hit[pc+w1], &h11=hit[pc+w1+1];
				if(h00)
				{
					if(h01)
					{
						if(h10)
						{
							if(h11)
							{
								auto
									&C00X=hitX[pc   ], &C01X=hitX[pc   +1],
									&C10X=hitX[pc+w1], &C11X=hitX[pc+w1+1],
									&C00Y=hitY[pc   ], &C01Y=hitY[pc   +1],
									&C10Y=hitY[pc+w1], &C11Y=hitY[pc+w1+1];
							}
							else
							{
							}
						}
						else
						{
							if(h11)
							{
							}
							else
							{
							}
						}
					}
					else
					{
						if(h10)
						{
							if(h11)
							{
							}
							else
							{
							}
						}
						else
						{
							if(h11)
							{
							}
							else
							{
							}
						}
					}
				}
				else
				{
					if(h01)
					{
						if(h10)
						{
							if(h11)
							{
							}
							else
							{
							}
						}
						else
						{
							if(h11)
							{
							}
							else
							{
							}
						}
					}
					else
					{
						if(h10)
						{
							if(h11)
							{
							}
							else
							{
							}
						}
						else
						{
							if(h11)
							{
							}
							else
							{
							}
						}
					}
				}
			}
		}//*/
	}
#endif

	//benchmarks
#if 0//QueryPerformanceCounter vs clock vs boost local_time vs GetSystemTime
	{
	//	auto LOL_1=clock();//elapsed time in milliseconds
	//	LOL_1=LOL_1;
		for(int k=0;k<rgbn;++k)
			rgb[k]=clock();//*/					//19.1 20.2		19.14 20.3ms
	/*	LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		long long frequency=li.QuadPart;
		for(int k=0;k<rgbn;++k)
		{
			QueryPerformanceCounter(&li);		//26.7ms		26.7 27.9ms
			rgb[k]=int(li.QuadPart/frequency);
		}//*/
	/*	SYSTEMTIME st;
		for(int k=0;k<rgbn;++k)
		{
			GetSystemTime(&st);					//84.53 86.13
			rgb[k]=st.wMilliseconds;
		}//*/
	}
#endif
#if 0//floor, signbit
	for(int k=0;k<rgbn;++k)
	{
		double x=rand()-rand();
		rgb[k]=x>0;					//76 83 fs
	//	rgb[k]=std::signbit(x);		//84 93

	//	rgb[k]=int(x)-(x<0);		//75 77		74 76
	//	rgb[k]=int(std::floor(x));	//119 121
	}
#endif
#if 0//container access
	{
		//buffer
	//	for(int k=0;k<rgbn;++k)
	//		rgb[k]=container[k];			//9.5 15.6 fs c
		//	rgb[k]=container[rand()%rgbn];	//47.5 50 fs c		46.4 47.5 fsc
		//vector
	//	for(int k=0;k<rgbn;++k)
	//		rgb[k]=container[k];			//10.8 15.6 fs c
		//	rgb[k]=container[rand()%rgbn];	//46.8 48 fs c
		//list
	/*	auto cIT=container.begin();
		for(int k=0;k<rgbn;++k, ++cIT)
			rgb[k]=*cIT;//*/				//15.8 16.4 fs c
	/*	if(rand()%2)
		{
			auto cIT=container.begin();
			for(int k=0;k<rgbn;++k, ++cIT)
				rgb[k]=*cIT;				//16 16.7 fs c
		}
		else
		{
			auto cIT=container.rbegin();
			for(int k=0;k<rgbn;++k, ++cIT)
				rgb[k]=*cIT;
		}//*/
	/*	for(int k=0;k<rgbn;++k)			//X
		{
			int i=rand()%rgbn;
			auto cIT=container.begin();
			for(int k2=0;k2<i;++k2)
				++cIT;
			rgb[k]=*cIT;
		}//*/
		//set
	/*	auto cIT=container.begin();
		for(int k=0;k<rgbn;++k, ++cIT)
			rgb[k]=*cIT;//*/				//131 132 fs c
		//map
		auto cIT=container.begin();
		for(int k=0;k<rgbn;++k, ++cIT)
			rgb[k]=cIT->second;//*/			//132 134 fs c
	}
#endif
#if 0//branch vs function pointer
	{
		double x, ivroot;
		for(int k=0;k<rgbn;++k)
		{
			x=rand();
			{
			//	double t0;
			//	(long long&)t0=0x5FE6EC85E7DE30DA-((long long&)x>>1);	//36.2 fs
			//	ivroot=t0*(1.5-.5*x*t0*t0);
				auto t1=0x5FE6EC85E7DE30DA-((long long&)x>>1);		//36.2 fs
				auto t2=(double&)t1;
				ivroot=t2*(1.5-.5*x*t2*t2);
			}
			rgb[k]=int(100000*ivroot);
		}
	/*	int endLevel=0, min=0;
		for(int k=0;k<rgbn;++k)
		{
			rgb[k]=' '+rand()%(127-' ');
			switch(rgb[k])
			{
			case '(':++endLevel;break;
			case ')':--endLevel;if(min>endLevel)min=endLevel;break;		//25.1 fs
			}
		//	endLevel+=(rgb[k]=='(')-(rgb[k]==')'), min=(endLevel+min-std::abs(endLevel-min))/2;//		30.2 fs
		}
		GUIPrint(hMemDC, 20, 100, "%d, %d", endLevel, min);//*/

	/*	int func=rand()%8;
		for(int k=0;k<rgbn;++k)
		{
			unsigned char r=rand(), g=rand(), b=rand();		//28.9	28.5		56.5 fs
			switch(func)
			{
			case 0:rgb[k]=r+g+b;break;
			case 1:rgb[k]=r<<16|g<<8|b;break;
			case 2:rgb[k]=b<<16|g<<8|r;break;
			case 3:rgb[k]=r*g<<8|g*b|b*r>>8;break;
			case 4:rgb[k]=b*r<<8|g*b|r*g>>8;break;
			case 5:rgb[k]=r*g*b;break;
			case 6:
				{
					int av=(r+g+b)/3;
					rgb[k]=av<<16|av<<8|av;
				}
				break;
			case 7:(r^g)<<16|(g^b)<<8|(b^r);break;
			}
		}//*/
	/*	int (*allFuncs[])(unsigned char, unsigned char, unsigned char)={func0, func1, func2, func3, func4, func5, func6, func7};
		const int allFuncLen=sizeof(allFuncs)/sizeof(allFuncs[0]);
		auto func=allFuncs[rand()%allFuncLen];
		for(int k=0;k<rgbn;++k)
			rgb[k]=func(rand(), rand(), rand());		//28.9	28.7		56.7 fs		//*/
	/*	Func *allFuncs[]={new Func0, new Func1, new Func2, new Func3, new Func4, new Func5, new Func6, new Func7};
		const int allFuncLen=sizeof(allFuncs)/sizeof(allFuncs[0]);
		auto func=allFuncs[rand()%allFuncLen];
		for(int k=0;k<rgbn;++k)
			rgb[k]=func->operator()(rand(), rand(), rand());		//30 29.8		58.6 fs
		for(int k=0;k<allFuncLen;++k)
			delete allFuncs[k];//*/
	}
//	for(int k=0;k<rgbn;++k)
	//	rgb[k]=func0(rand(), rand(), rand());//27.3
	//	rgb[k]=func1(rand(), rand(), rand());//27.3
	//	rgb[k]=func2(rand(), rand(), rand());//27.3
	//	rgb[k]=func3(rand(), rand(), rand());//27.9
	//	rgb[k]=func4(rand(), rand(), rand());//27.9
	//	rgb[k]=func5(rand(), rand(), rand());//27.4
	//	rgb[k]=func6(rand(), rand(), rand());//27.7 27.8 stops
	//	rgb[k]=func7(rand(), rand(), rand());//28.4 stops		28.5 stops
#endif
#if 0//branch
	{
		int LOL_1=0;
		for(int k=0;k<10000000;++k)
		//	LOL_1+=rand()%2;//181 192ms
			if(rand()%2)//265 263ms
				++LOL_1;
		GUIPrint(hMemDC, 0, 32, LOL_1);
	}
#endif
#if 0//method vs method pointer
	{
		LOL_class LOL_1;
		for(int k=0;k<rgbn;++k)//40 41		47.3 48.2 ms no opt
		{
			if(rand()%2)
				LOL_1.set_color(rand(), rand(), rand());
			else
				LOL_1.set(rand(), rand(), rand());
			rgb[k]=LOL_1.get_color();
		}//*/
	/*	for(int k=0;k<rgbn;++k)//39.9 40	49.7 51 ms no opt
		{
			(LOL_1.*(rand()%2?&LOL_class::set_color:&LOL_class::set))(rand(), rand(), rand());
			rgb[k]=LOL_1.get_color();
		}//*/
	}
#endif
#if 0//unicode
	{
		auto encode_utf8=[](int cp, std::string &str)
		{
			if(cp<0)
				return;
			if(cp<0x80)
				str+=cp;
			else if(cp<0x800)
				str+=0xC0|cp>>6, str+=0x80|cp&0x3F;
			else if(cp<0x10000)
				str+=0xE0|cp>>12, str+=0x80|cp>> 6&0x3F, str+=0x80|cp&0x3F;
			else if(cp<0x200000)
				str+=0xF0|cp>>18, str+=0x80|cp>>12&0x3F, str+=0x80|cp>>6&0x3F, str+=0x80|cp&0x3F;
			else if(cp<0x4000000)
				str+=0xF8|cp>>24, str+=0x80|cp>>18&0x3F, str+=0x80|cp>>12&0x3F, str+=0x80|cp>>6&0x3F, str+=0x80|cp&0x3F;
			else
				str+=0xFC|cp>>30, str+=0x80|cp>>24&0x3F, str+=0x80|cp>>18&0x3F, str+=0x80|cp>>12&0x3F, str+=0x80|cp>>6&0x3F, str+=0x80|cp&0x3F;
		};
		auto print_unicode_range=[&](int start, int end, std::string &str)
		{
			const int buf_size=1024;
			char a[buf_size];
			int linelen=256;//100
			for(int k=start;k<end;)
			{
				sprintf_s(a, "[0x%04X, %d]", k, k);
				str+=a;
				int kNext=linelen*((k+linelen)/linelen);
				for(int k2=k;k2<kNext;k2++)
					encode_utf8(k2, str);
				str+="\r\n";//notepad, notepad++
			//	str+='\r';//notepad++		X notepad
			//	str+='\n';//unix, notepad++		X notepad
				k=kNext;
			}
		};
		std::string str;
		print_unicode_range(1, 0x30000, str);
		print_unicode_range(0xE0000, 0x110000, str);

		std::ofstream file;
		file.open("D:/1.txt", std::ios::out|std::ios::binary);
		unsigned char smarker[]={0xEF, 0xBB, 0xBF, 0};//http://stackoverflow.com/questions/3973582/how-do-i-write-a-utf-8-encoded-string-to-a-file-in-windows-in-c
		file<<smarker;
		file<<str;
		file.close();
		//std::wstring str;//wchar_t, sizeof == 2U
		//std::u16string str;//char16_t == unsigned short
		//std::u16string str;//char16_t == unsigned short
		//std::u32string str;//char32_t == unsigned int
		//const auto LOL_1=sizeof(wchar_t), LOL_2=sizeof(char32_t);

	/*	const int buf_size=1024;
		wchar_t a[buf_size];
		std::wstring wstr;
		for(int k=1;k<0x10000;)
		{
			swprintf(a, buf_size, L"[0x%04X, %d]", k, k);
			wstr+=a;
			int kNext=100*((k+100)/100);
			for(int k2=k;k2<kNext;k2++)
			{
				wstr+=wchar_t(k2);
			}
			wstr+=L"\r\n";
			k=kNext;
		}
		std::wofstream f("D:/1.txt");
	//	std::wofstream f=std::wofstream("D:/1.txt");
		f<<wstr;
		f.close();//*/
	}
#endif
#if 0//expression, function, function pointer
	{
	//	memset(rgb, 0xA0A0A0, rgbn*sizeof(int));	//15.65		2.2 2.4
	//	for(int k=0;k<rgbn;++k)
	//		rgb[k]=0xA0A0A0;						//15.7		2.9 3.15
	//	for(int y=0;y<h;++y)
	//		for(int x=0;x<w;++x)
	//			rgb[w*y+x]=0xA0A0A0;				//15.6		2.8 3
	//	for(int y=0;y<h;++y)
	//		for(int x=0;x<w;++x)
	//			AssignPixel(rgb, x, y, 0xA0A0A0);	//15.6		2.8 3		no inline 4.45 4.65
	//	auto pf=AssignPixel;
	//	for(int y=0;y<h;++y)
	//		for(int x=0;x<w;++x)
	//			pf(rgb, x, y, 0xA0A0A0);			//			4.5 4.8

	//	for(int y=0;y<h;++y)
	//		for(int x=0;x<w;++x)
	//			rgb[w*y+x]=rand()<<16|rand();						//fullscreen 37.4 37.7		loop 19.3 19.5		loop no opt 20.8 21.05

	//	for(int y=0;y<h;++y)
	//		for(int x=0;x<w;++x)
	//			AssignPixel(rgb, x, y, rand()<<16|rand());			//fullscreen 37.4 37.7		loop 19.3 19.55		loop no opt 22.2 22.6

	/*	auto AssignPixel=[&](int x, int y, int c){rgb[w*y+x]=c;};	//fullscreen 37.4 37.8		loop 19.3 19.55		loop no opt 22.2 23
		for(int y=0;y<h;++y)
			for(int x=0;x<w;++x)
				AssignPixel(x, y, rand()<<16|rand());//*/
	}
#endif

#if 0//condition vs function vs method vs polymorphim vs function pointer vs method pointer vs object pointer
	{
		int x=rand()%1000;

	//	std::fill(rgb, rgb+w*h, 0xA0A0A0);
	//	memset(rgb, 0xA0A0A0, w*h*sizeof(int));
	/*	for(int k=0, kEnd=w*h;k<kEnd;++k)		//
			rgb[k]=0xA0A0A0;//*/
	/*	for(int k=0, kEnd=w*h;k<kEnd;++k)		//2.8	2.9ms			loop X			fs 8.7 15.6ms 15.6998
		{
				 if(x<500)	rgb[k]=0xA0A0A0;
			else			rgb[k]=0xB0B0B0;
		}//*/
	/*	for(int k=0, kEnd=w*h;k<kEnd;++k)		//										fs 9.3 15.6 15.838
			f_white(rgb, k);//*/
	/*	std::vector<BenchmarkColor> coco(1);
		auto it=coco.begin();
		for(int k=0, kEnd=w*h;k<kEnd;++k)
		//	it->color_inline(rgb, k);				//									fs 14.7 15.6 15.7656
			it->color(rgb, k);//*/					//									fs 14.7 15.7 15.83
	/*	auto func=x<500?::f_black: ::f_white;
		for(int k=0, kEnd=w*h;k<kEnd;++k)			//4.3	4.4 4.5ms					fs 15.25 15.37ms		15.2		-> 15.199 15.65 15.75
			func(rgb, k);//*/
	/*	BenchmarkColor1 c1, c2;//pointer method call
		auto pc=x<500?&c1:&c2;						//
		for(int k=0, kEnd=w*h;k<kEnd;++k)
			pc->color(rgb, k);//*/
	/*	BenchmarkColor1 c1, c2;//derived cast pointer method call
		BenchmarkColor *pc=x<500?&c1:&c2;			//
		for(int k=0, kEnd=w*h;k<kEnd;++k)
			((BenchmarkColor1*)pc)->color(rgb, k);//*/
	/*	BenchmarkColor1 c1;
		BenchmarkColor2 c2;
		BenchmarkColor *pc=x<500?(BenchmarkColor*)&c1:(BenchmarkColor*)&c2;//			fs 14.73 14.81ms		14.8		-> 14.9 15.7
		for(int k=0, kEnd=w*h;k<kEnd;++k)
			pc->color(rgb, k);//*/

	/*	void
			(*f_color1)(int*, int)=[](int *rgb, int k){rgb[k]=0xA0A0A0;},//				fs 15.24 15.33ms					-> 15.1 15.79
			(*f_color2)(int*, int)=[](int *rgb, int k){rgb[k]=0xB0B0B0;};
		auto f_color=x<500?f_color1:f_color2;

	//	auto f_color=x<500?							//	X
	//		(void(*)(int*, int))([](int *rgb, int k){rgb[k]=0xA0A0A0;})
	//	:	(void(*)(int*, int))([](int *rgb, int k){rgb[k]=0xB0B0B0;});
		for(int k=0, kEnd=w*h;k<kEnd;++k)
			f_color(rgb, k);//*/
		
	/*	x/=100;
		for(int k=0, kEnd=w*h;k<kEnd;++k)		//						loop X			fs 9.44 15.6ms 15.777
		{
			switch(x)
			{
			case 0:rgb[k]=0xA0A0A0;break;
			case 1:rgb[k]=0xA0A0B0;break;
			case 2:rgb[k]=0xA0B0A0;break;
			case 3:rgb[k]=0xA0B0B0;break;
			case 4:rgb[k]=0xB0A0A0;break;
			case 5:rgb[k]=0xB0A0B0;break;
			case 6:rgb[k]=0xB0B0A0;break;
			case 7:rgb[k]=0xB0B0B0;break;
			case 8:rgb[k]=0xB0B0C0;break;
			case 9:rgb[k]=0xB0C0B0;break;
			}
		}//*/
	/*	auto func=x<500?&LinearSW::f_black:&LinearSW::f_white;
		for(int k=0, kEnd=w*h;k<kEnd;++k)			//4.4	4.6ms						fs 15.44 15.55ms					-> 15.48 15.97
			(this->*func)(rgb, k);//*/
	/*	for(int k=0, kEnd=w*h;k<kEnd;++k)			//2.8	4.5ms		loop 4.3ms		fs 8.6 16.41ms
		{
				 if(x<100)	rgb[k]=0xA0A0A0;
			else if(x<200)	rgb[k]=0xA0A0B0;
			else if(x<300)	rgb[k]=0xA0B0A0;
			else if(x<400)	rgb[k]=0xA0B0B0;
			else if(x<500)	rgb[k]=0xB0A0A0;
			else if(x<600)	rgb[k]=0xB0A0B0;
			else if(x<700)	rgb[k]=0xB0B0A0;
			else if(x<800)	rgb[k]=0xB0B0B0;
			else if(x<900)	rgb[k]=0xB0B0C0;
			else			rgb[k]=0xB0C0B0;
		}//*/
	/*	std::function<void(int)> func=x<500?		//4.9	5.1ms						fs 17.51 17.65ms					-> 17.4 18.85
				std::function<void(int)>([&](int k){rgb[k]=0xA0A0A0;})
			:	std::function<void(int)>([&](int k){rgb[k]=0xB0B0B0;});
		for(int k=0, kEnd=w*h;k<kEnd;++k)//5.1 5.3ms
			func(k);//*/
	/*	int k=0;
		std::function<void()> func=x<500?			//5.1	5.3ms						fs 18.55 18.65						-> 18.49 18.9
				std::function<void()>([&]{rgb[k]=0xA0A0A0;})
			:	std::function<void()>([&]{rgb[k]=0xB0B0B0;});
		for(int kEnd=w*h;k<kEnd;++k)
			func();//*/
	}
#endif
#if 0//int vs double multiply
	for(int k=0, kEnd=w*h;k<kEnd;++k)//22.7ms
	{
		int x=rand();
		rgb[k]=x*x*x*x*x*x*x*x*x*x*x*x;
	}//*/
/*	for(int k=0, kEnd=w*h;k<kEnd;++k)//29.2ms
	{
		double x=rand();
		rgb[k]=int(x*x*x*x*x*x*x*x*x*x*x*x);
	}//*/
//	for(int k=0, kEnd=w*h;k<kEnd;++k)
//		rgb[k]=rand()*rand();//37
#endif
#if 0
	for(int k=0, kEnd=w*h;k<kEnd;++k)//177ms
	{
		auto p=(unsigned char*)&rgb[k];
		p[0]=std::exp(2.*std::log(double(rand()))), p[1]=std::exp(2.*std::log(double(rand()))), p[2]=std::exp(2.*std::log(double(rand())));
	}//*/
/*	for(int k=0, kEnd=w*h;k<kEnd;++k)//215ms
	{
		auto p=(unsigned char*)&rgb[k];
		p[0]=std::pow(double(rand()), 2.), p[1]=std::pow(double(rand()), 2.), p[2]=std::pow(double(rand()), 2.);
	}//*/
/*	for(int k=0, kEnd=w*h;k<kEnd;++k)//530 532ms
	{
		auto p=(unsigned char*)&rgb[k];
		p[0]=std::exp2(2.*std::log2(double(rand()))), p[1]=std::exp2(2.*std::log2(double(rand()))), p[2]=std::exp2(2.*std::log2(double(rand())));
	}//*/
#endif
#if 0
	double a, b;
	for(int k=0;k<rgbn;++k)
	{
		a=rand(), b=rand();
		auto p=(unsigned char*)&rgb[k];
		a=rand()%256, b=rand()%256;
	//	rgb[k]=int(a>b?a:b);				//92-93		40.4 ms
	//	rgb[k]=int((a+b+std::abs(a-b)))/2;	//77-78		36.3 ms
		rgb[k]=int((a+b+std::abs(a-b))/2);	//77-78		36.4 36.5 36.6
	//	rgb[k]=int(a<b?a:b);				//93-94
	//	rgb[k]=int((a+b-std::abs(a-b)))/2;	//77-78
	//	rgb[k]=int((a+b-std::abs(a-b))/2);	//77-78

	//	p[0]=unsigned char(a>b?a:b);				//170-171
	//	p[0]=unsigned char(a+b+std::abs(a-b))/2;	//140-156
	//	p[0]=unsigned char((a+b+std::abs(a-b))/2);	//156

	//	p[0]=unsigned char(a<b?a:b);				//171-172
	//	p[0]=unsigned char((a+b-std::abs(a-b))/2);	//156

	//	a=rand()%256, b=rand()%256;
	//	p[1]=unsigned char(a>b?a:b);
	//	p[1]=unsigned char(a+b+std::abs(a-b))/2;
	//	p[1]=unsigned char((a+b+std::abs(a-b))/2);
	//	p[1]=unsigned char(a<b?a:b);
	//	p[1]=unsigned char((a+b-std::abs(a-b))/2);

	//	a=rand()%256, b=rand()%256;
	//	p[2]=unsigned char(a>b?a:b);
	//	p[2]=unsigned char(a+b+std::abs(a-b))/2;
	//	p[2]=unsigned char((a+b+std::abs(a-b))/2);
	//	p[2]=unsigned char(a<b?a:b);
	//	p[2]=unsigned char((a+b-std::abs(a-b))/2);
	}
#endif
#if 0
//	std::vector<int> vec(w*h);
//	for(auto &x:vec)
//		x=rand();

/*	for(auto &x:vec)//46.8ms
		*vec.rbegin()+=x-rand();
	for(auto &x:vec)
		*vec.rbegin()+=x-rand();
	for(auto &x:vec)
		*vec.rbegin()+=x-rand();
	for(auto &x:vec)
		*vec.rbegin()=x-rand();//*/

/*	for(auto &x:vec)//48.9ms
		vec[vec.size()-1]+=x-rand();
	for(auto &x:vec)
		vec[vec.size()-1]+=x-rand();
	for(auto &x:vec)
		vec[vec.size()-1]+=x-rand();
	for(auto &x:vec)
		vec[vec.size()-1]+=x-rand();//*/
#endif
#if 0
	for(int k=0, kEnd=w*h;k<kEnd;++k)//26.5ms		29 29.1		29.5 29.6ms		26.5ms			52.6 53ms		26.3 26.4ms
	{
		auto p=(unsigned char*)&rgb[k];
		p[0]=rand(), p[1]=rand(), p[2]=rand();
	}//*/
//	for(int k=0, kEnd=w*h;k<kEnd;++k)//26.9ms		27			27.1 27.2ms		26-27ms			55.2 55.7ms		28.1ms
//		rgb[k]=unsigned char(rand())<<16|unsigned char(rand())<<8|unsigned char(rand());//*/
#endif

	QueryPerformanceFrequency(&li);freq=li.QuadPart;
	QueryPerformanceCounter(&li);
#if 1
		static double total_t, min_t=_HUGE;
		static int n_t=1900;//starting point
		if(++n_t>2000)
	//	if(++n_t>700)//less coherent?
	//	if(++n_t>100)
			n_t=1, total_t=0, min_t=_HUGE;
		double dt=1000.*(li.QuadPart-nticks)/freq;
		if(min_t>dt)
			min_t=dt;
		total_t+=dt;
		SetBkMode(hMemDC, OPAQUE);
		GUIPrint(hMemDC, 0, 0, "T_min=%.10f, T_av=%.10f, NT=%d", min_t, total_t/n_t, n_t);
		SetBkMode(hMemDC, TRANSPARENT);
#endif
	if(w&&h)
		linelen=sprintf_s(line, 128, "fps=%.10f, T=%.10fms, dcam=%.10f, fov=%.10f, distance=%g", freq/double(li.QuadPart-nticks), 1000.*(li.QuadPart-nticks)/freq, dcam, atan(tanfov)*720/TWOPI, 1/wbuffer[w*(h/2)+w/2]);
	else
		linelen=sprintf_s(line, 128, "fps=%.10f, T=%.10fms, dcam=%.10f, fov=%.10f", freq/double(li.QuadPart-nticks), 1000.*(li.QuadPart-nticks)/freq, dcam, atan(tanfov)*720/TWOPI);
	nticks=li.QuadPart;
	if(linelen>0)TextOutA(hMemDC, 0, h-16, line, linelen);
//	GUIPrint(hMemDC, 0,  0, "lParam=0x%08X", glParam);
//	GUIPrint(hMemDC, 0, 18, "wParam=0x%08X", gwParam);
	BitBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
#ifdef LSW_LOOP_FRAME
	}
#endif
}
void				LinearSW::draft_1behind		(double *v1, double *v2, double *v3)//1 & 2 forward		3 behind
{
	draft_start(v1, v2);
	draft_crit(v3, v1);
	draft_crit(v3, v2);
	if(v3[1]>v1[1]&&v3[1]<v2[1]||v3[1]>v2[1]&&v3[1]<v1[1]){		 if(v3[0]<(v2[0]-v1[0])*(v3[1]-v1[1])/(v2[1]-v1[1])+v1[0])	for(int k=0;k<h;++k)lfbuffer[k]=w;
															else															memset(libuffer, 0, h*sizeof(int));}
}
void				LinearSW::draft_2behind		(double *v1, double *v2, double *v3)//1 forward		2 & 3 behind
{
	draft_crit_start(v2, v1);
	draft_crit(v3, v1);
	if(v1[1]>v2[1]&&v1[1]<v3[1]||v1[1]>v3[1]&&v1[1]<v2[1]){		 if(v1[0]<(v3[0]-v2[0])*(v1[1]-v2[1])/(v3[1]-v2[1])+v2[0])	memset(libuffer, 0, h*sizeof(int));
															else															for(int k=0;k<h;++k)lfbuffer[k]=w;}
}
void				LinearSW::draft_start		(double *v1, double *v2)
{
	for(int k2=0;k2<h;++k2)libuffer[k2]=w; memset(lfbuffer, 0, h*sizeof(int));
	int k2;double k3, A=(v2[0]-v1[0])/(v2[1]-v1[1]);
		 if(v1[1]<v2[1]){	k3=v1[0]+A*((long long)(v1[1]<0?0:v1[1])+1-v1[1]);		 if(v1[0]<v2[0])
			 for(long long k=long long(v1[1])<0?	0:long long(v1[1])	+1;k<h&&k<v2[1]	;++k)
				 k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
																				else
			for(long long k=long long(v1[1])<0?	0:long long(v1[1])	+1;k<h&&k<v2[1]	;++k)
				k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
	else				{	k3=v1[0]+A*((long long)(v2[1]<0?0:v2[1])+1-v1[1]);		 if(v1[0]<v2[0])
		for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h&&k<v1[1]	;++k)
			k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
																				else
		for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h&&k<v1[1]	;++k)
			k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
//		 if(v1[1]<v2[1]){	k3=v1[0]+A*((long long)(v1[1]<0?0:v1[1])+1-v1[1]);		 if(v1[0]<v2[0])for(long long k=long long(v1[1])<0?	0:long long(v1[1])	+1;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
//																				else				for(long long k=long long(v1[1])<0?	0:long long(v1[1])	+1;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
//	else				{	k3=v1[0]+A*((long long)(v2[1]<0?0:v2[1])+1-v1[1]);		 if(v1[0]<v2[0])for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h&&k<v1[1]	;++k)	k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
//																				else				for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h&&k<v1[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
}
void				LinearSW::draft				(double *v1, double *v2)
{
	int k2;double k3, A=(v2[0]-v1[0])/(v2[1]-v1[1]);
		 if(v1[1]<v2[1]){	k3=v1[0]+A*((long long)(v1[1]<0?0:v1[1])+1-v1[1]);		 if(v1[0]<v2[0])for(long long k=long long(v1[1])<0?	0:long long(v1[1])	+1;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
																				else				for(long long k=long long(v1[1])<0?	0:long long(v1[1])	+1;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;}
	else				{	k3=v1[0]+A*((long long)(v2[1]<0?0:v2[1])+1-v1[1]);		 if(v1[0]<v2[0])for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h&&k<v1[1]	;++k)	k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
																				else				for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h&&k<v1[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;}
}
void				LinearSW::draft_crit_start	(double *v1, double *v2)
{
	for(int k2=0;k2<h;++k2)libuffer[k2]=w; memset(lfbuffer, 0, h*sizeof(int));
	int k2;double k3, A=(v2[0]-v1[0])/(v2[1]-v1[1]);
		 if(v1[1]<v2[1]){	k3=v1[0]+A*((long long)(v2[1]<0?0:v2[1])+1-v1[1]);		 if(v1[0]<v2[0])for(long long k=long long(v2[1])<0?	0:long long(v2[1])+1;k<h			;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
																				else				for(long long k=long long(v2[1])<0?	0:long long(v2[1])+1;k<h			;++k)	k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
	else				{	k3=v1[0]-A*((long long)v1[1]+1);						 if(v1[0]<v2[0])for(long long k=					0					;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
																				else				for(long long k=					0					;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
//		 if(v1[1]<v2[1]){	k3=v1[0]+A*((long long)(v2[1]<0?0:v2[1])+1-v1[1]);		 if(v1[0]<v2[0])for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h			;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
//																				else				for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h			;++k)	k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
//	else				{	k3=v1[0]-A*((long long)v1[1]+1);						 if(v1[0]<v2[0])for(long long k=					0					+1;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
//																				else				for(long long k=					0					+1;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
}
void				LinearSW::draft_crit		(double *v1, double *v2)
{
	int k2;double k3, A=(v2[0]-v1[0])/(v2[1]-v1[1]);
		 if(v1[1]<v2[1]){	k3=v1[0]+A*((long long)(v2[1]<0?0:v2[1])+1-v1[1]);		 if(v1[0]<v2[0])for(long long k=long long(v2[1])<0?	0:long long(v2[1])+1;k<h			;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
																				else				for(long long k=long long(v2[1])<0?	0:long long(v2[1])+1;k<h			;++k)	k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;}
	else				{	k3=v1[0]-A*((long long)v1[1]+1);						 if(v1[0]<v2[0])for(long long k=					0					;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
																				else				for(long long k=					0					;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;}
/*	if(v1[1]<v2[1])
	{
		k3=v1[0]+A*((long long)(v2[1]<0?0:v2[1])+1-v1[1]);
		if(v1[0]<v2[0])
			for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h			;++k)
				k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
		else
			for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h			;++k)
				k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
	}
	else
	{
		k3=v1[0]-A*((long long)v1[1]+1);
		if(v1[0]<v2[0])for(long long k=			0					+1;k<h&&k<v2[1]	;++k)
			k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
		else
			for(long long k=					0					+1;k<h&&k<v2[1]	;++k)
				k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
	}//*/
//		 if(v1[1]<v2[1]){	k3=v1[0]+A*((long long)(v2[1]<0?0:v2[1])+1-v1[1]);		 if(v1[0]<v2[0])for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h			;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
//																				else				for(long long k=long long(v2[1])<0?	0:long long(v2[1])	+1;k<h			;++k)	k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;}
//	else				{	k3=v1[0]-A*((long long)v1[1]+1);						 if(v1[0]<v2[0])for(long long k=					0					+1;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?w:k2<v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
//																				else				for(long long k=					0					+1;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?0:k2>w?v2[0]<w?int(v2[0]):w:k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;}
}
void				LinearSW::pushTexture(int *texture_)
{
	++ntx;
	texture=(int**)realloc(texture, ntx*sizeof(int*)), texture[ntx-1]=texture_;
}
void				LinearSW::enqueueTextures()
{
}
int*				LinearSW::popTexture()
{
	ntx--;
	return texture[ntx];
}
void				LinearSW::clearTextures()
{
	for(int k=0;k<ntx;++k)
		free(texture[k]);
	ntx=0;
}
void				LinearSW::finish()
{
	DeleteObject(SelectObject(hMemDC, hBitmap)), DeleteDC(hMemDC);
	for(int k=0;k<ntx;++k)free(texture[k]);
	free(texture), free(libuffer), free(lfbuffer), free(wbuffer);
//	free(rgb_r), free(rgb_g), free(rgb_b), free(rgb_n);

//	free(container);
}
struct			LinearGL:private Mode
{
	unsigned int *texture;
	HGLRC__ *hRC;
	void initiate();
	void resize();
	void changeFov();
	void render();
	void pushTexture(int *texture_);
	void enqueueTextures();
	int* popTexture();
	void clearTextures();
	void finish();
} lgl;
void				LinearGL::initiate()
{
	texture=0;
	tagPIXELFORMATDESCRIPTOR pfd={sizeof(tagPIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0};
	int PixelFormat=ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, PixelFormat, &pfd);
	hRC=wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	if(h<=0)h=1;
	changeFov();
	glShadeModel(GL_SMOOTH);
	glClearColor(1, 1, 1, 0);
	glClearDepth(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
void				LinearGL::resize()
{
	if(h<=0)h=1;
	changeFov();
	SetBkMode(hDC, OPAQUE);
}
void				LinearGL::changeFov()
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(atan(tanfov*h/w)*720/TWOPI, double(w)/h, 50, 50000);//
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void 				LinearGL::render()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0, 0, 0);
	double dx, dy, dz, cpt, v1[3], v2[3], v3[3];
	for(int k=0;k<npols;++k)
	{
		dx=env[k].x1-camx, dy=env[k].y1-camy, dz=env[k].z1-camz, cpt=dx*cax+dy*sax, v1[0]=dx*sax-dy*cax, v1[1]=cpt*say-dz*cay, v1[2]=cpt*cay+dz*say;
		dx=env[k].x2-camx, dy=env[k].y2-camy, dz=env[k].z2-camz, cpt=dx*cax+dy*sax, v2[0]=dx*sax-dy*cax, v2[1]=cpt*say-dz*cay, v2[2]=cpt*cay+dz*say;
		dx=env[k].x3-camx, dy=env[k].y3-camy, dz=env[k].z3-camz, cpt=dx*cax+dy*sax, v3[0]=dx*sax-dy*cax, v3[1]=cpt*say-dz*cay, v3[2]=cpt*cay+dz*say;
		if(env[k].tx>=0)
		{
			glEnable(GL_TEXTURE_2D);
			int txwk=txw[env[k].tx], txhk=txh[env[k].tx];
			glBindTexture(GL_TEXTURE_2D, texture[env[k].tx]);
			glBegin(GL_TRIANGLES);//glDrawArrays
			glColor3f(1, 1, 1);
			glTexCoord2f(float(env[k].tx1x/txwk), float(env[k].tx1y/txhk)), glVertex3f(float(v1[0]), float(-v1[1]), float(-v1[2]));
			glTexCoord2f(float(env[k].tx2x/txwk), float(env[k].tx2y/txhk)), glVertex3f(float(v2[0]), float(-v2[1]), float(-v2[2]));
			glTexCoord2f(float(env[k].tx3x/txwk), float(env[k].tx3y/txhk)), glVertex3f(float(v3[0]), float(-v3[1]), float(-v3[2]));
			glEnd();
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_TRIANGLES);
			glColor3f(float(unsigned char(env[k].color>>16))/255, float(unsigned char(env[k].color>>8))/255, float(unsigned char(env[k].color))/255);
			glVertex3f(float(v1[0]), float(-v1[1]), float(-v1[2]));
			glVertex3f(float(v2[0]), float(-v2[1]), float(-v2[2]));
			glVertex3f(float(v3[0]), float(-v3[1]), float(-v3[2]));
			glEnd();
		}
	}
	QueryPerformanceFrequency(&li);freq=li.QuadPart;
	QueryPerformanceCounter(&li);
	linelen=sprintf_s(line, "OpenGL, fps=%.10f, T=%.10fms, dcam=%.10f, fov=%.10f", freq/double(li.QuadPart-nticks), 1000.*(li.QuadPart-nticks)/freq, dcam, atan(tanfov)*720/TWOPI);
	nticks=li.QuadPart;

	SwapBuffers(hDC);
	if(linelen>0)TextOutA(hDC, 0, R.bottom-16, line, linelen);
}
void				LinearGL::pushTexture(int *texture_)
{
	++ntx;
	for(int k=0;k<txh[ntx-1]*txw[ntx-1];++k)texture_[k]=unsigned char(texture_[k])<<16|unsigned char(texture_[k]>>8)<<8|unsigned char(texture_[k]>>16);
	texture=(unsigned int*)realloc(texture, ntx*sizeof(unsigned int));
	glGenTextures(1, &texture[ntx-1]);
	glBindTexture(GL_TEXTURE_2D, texture[ntx-1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, txw[ntx-1], txh[ntx-1], 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_);
	free(texture_);
}
void				LinearGL::enqueueTextures()
{
}
int*				LinearGL::popTexture()
{
	int *texture_=(int*)malloc(txh[ntx-1]*txw[ntx-1]*sizeof(int));
	glBindTexture(GL_TEXTURE_2D, texture[ntx-1]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_);
	glDeleteTextures(1, &texture[ntx-1]);
	for(int k=0;k<txh[ntx-1]*txw[ntx-1];++k)texture_[k]=unsigned char(texture_[k])<<16|unsigned char(texture_[k]>>8)<<8|unsigned char(texture_[k]>>16);
	ntx--;
	return texture_;
}
void				LinearGL::clearTextures()
{
	glDeleteTextures(ntx, texture);
	ntx=0;
}
void				LinearGL::finish()
{
	wglMakeCurrent(0, 0);
	wglDeleteContext(hRC);
	free(texture);
}
struct			NonlinearSW:private Mode
{
	int **texture;
	double *wbuffer, *xbuffer, *ybuffer, *zbuffer;
	int *rgb, rgbn;
	HDC__ *hMemDC;
	HBITMAP__ *hBitmap;
	void (NonlinearSW::*rebuffer)();
	void initiate();
	void resize();
	void changeFov();
	void render();
	void			rebuffer1			();
	void			rebuffer2			();
	void			rebuffer3			();
	void			rebuffer4			();
	void pushTexture(int *texture_);
	void enqueueTextures();
	int* popTexture();
	void clearTextures();
	void finish();
} nlsw;
void				NonlinearSW::initiate()
{
	texture=0;
	rgbn=h*w;
	wbuffer=(double*)malloc(rgbn*sizeof(double)), xbuffer=(double*)malloc(rgbn*sizeof(double)), ybuffer=(double*)malloc(rgbn*sizeof(double)), zbuffer=(double*)malloc(rgbn*sizeof(double));
	hMemDC=CreateCompatibleDC(hDC);
	tagBITMAPINFO bmpInfo={{sizeof(tagBITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
	hBitmap=(HBITMAP__*)SelectObject(hMemDC, CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&rgb, 0, 0));
	SetBkMode(hMemDC, TRANSPARENT);
}
void				NonlinearSW::resize()
{
	rgbn=h*w;
	wbuffer=(double*)realloc(wbuffer, rgbn*sizeof(double)), xbuffer=(double*)realloc(xbuffer, rgbn*sizeof(double)), ybuffer=(double*)realloc(ybuffer, rgbn*sizeof(double)), zbuffer=(double*)realloc(zbuffer, rgbn*sizeof(double));
	(this->*rebuffer)();
	DeleteObject((HBITMAP__*)SelectObject(hMemDC, hBitmap));
	tagBITMAPINFO bmpInfo={{sizeof(tagBITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
	hBitmap=(HBITMAP__*)SelectObject(hMemDC, CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&rgb, 0, 0));
	SetBkMode(hMemDC, TRANSPARENT);
}
void				NonlinearSW::changeFov()
{
	(this->*rebuffer)();
}
void				NonlinearSW::render()
{
	memset(rgb, 0xFF, rgbn*sizeof(int)), memset(wbuffer, 0, rgbn*sizeof(double));
	double dx, dy, dz, cpt, v1[5], v2[5], v3[5], admittance;
	for(int k=0;k<npols;++k)
	{
		dx=env[k].x1-camx, dy=env[k].y1-camy, dz=env[k].z1-camz, cpt=dx*cax+dy*sax, v1[2]=dx*sax-dy*cax, v1[3]=cpt*say-dz*cay, v1[4]=cpt*cay+dz*say;
		dx=env[k].x2-camx, dy=env[k].y2-camy, dz=env[k].z2-camz, cpt=dx*cax+dy*sax, v2[2]=dx*sax-dy*cax, v2[3]=cpt*say-dz*cay, v2[4]=cpt*cay+dz*say;
		dx=env[k].x3-camx, dy=env[k].y3-camy, dz=env[k].z3-camz, cpt=dx*cax+dy*sax, v3[2]=dx*sax-dy*cax, v3[3]=cpt*say-dz*cay, v3[4]=cpt*cay+dz*say;
		double	au12=v2[2]-v1[2],			bu12=v2[3]-v1[3],			cu12=v2[4]-v1[4];									//u12	=<12>
		double	aux3=v3[2]-v1[2],			bux3=v3[3]-v1[3],			cux3=v3[4]-v1[4];									//ux3	=<13>
		double	a=bu12*cux3-bux3*cu12,		b=aux3*cu12-au12*cux3,		c=au12*bux3-aux3*bu12;								//abc	=<n>	=<12>x<13>
		double	t=a*v1[2]+b*v1[3]+c*v1[4], t2=t*t;
		if(!t)continue;
		double	m12=sqrt(au12*au12+bu12*bu12+cu12*cu12);			au12/=m12,			bu12/=m12,			cu12/=m12;		//u12	=<u12>	=<12>/|12|
				cpt=aux3*au12+bux3*bu12+cux3*cu12;					aux3-=cpt*au12,		bux3-=cpt*bu12,		cux3-=cpt*cu12;	//ux3	=<x3>	=<13>-<u12>.<13><u12>
				cpt=sqrt(aux3*aux3+bux3*bux3+cux3*cux3);			aux3/=cpt,			bux3/=cpt,			cux3/=cpt;		//ux3	=<ux3>	=<x3>/|x3|
		double	ux3_1=aux3*v1[2]+bux3*v1[3]+cux3*v1[4];
		double	u12_1=au12*v1[2]+bu12*v1[3]+cu12*v1[4];
		double	Xp3=aux3*v3[2]+bux3*v3[3]+cux3*v3[4]-ux3_1;
		double	Yp3=au12*v3[2]+bu12*v3[3]+cu12*v3[4]-u12_1;
		double	A1=t*aux3-a*ux3_1, A2=t*bux3-b*ux3_1, A3=t*cux3-c*ux3_1;
		double	A4=t*au12-a*u12_1, A5=t*bu12-b*u12_1, A6=t*cu12-c*u12_1;
		if(env[k].tx>=0)
		{
			double B1=env[k].txXX*A1+env[k].txXY*A4+env[k].tx1x*a, B2=env[k].txXX*A2+env[k].txXY*A5+env[k].tx1x*b, B3=env[k].txXX*A3+env[k].txXY*A6+env[k].tx1x*c;
			double B4=env[k].txYX*A1+env[k].txYY*A4+env[k].tx1y*a, B5=env[k].txYX*A2+env[k].txYY*A5+env[k].tx1y*b, B6=env[k].txYX*A3+env[k].txYY*A6+env[k].tx1y*c;
			double a12=env[k].tx1y-env[k].tx2y, b12=env[k].tx2x-env[k].tx1x, c12=-a12*env[k].tx1x-b12*env[k].tx1y, d12_3=a12*env[k].tx3x+b12*env[k].tx3y+c12;	a12/=d12_3, b12/=d12_3, c12/=d12_3;
			double a23=env[k].tx2y-env[k].tx3y, b23=env[k].tx3x-env[k].tx2x, c23=-a23*env[k].tx2x-b23*env[k].tx2y, d23_1=a23*env[k].tx1x+b23*env[k].tx1y+c23;	a23/=d23_1, b23/=d23_1, c23/=d23_1;
			double a31=env[k].tx3y-env[k].tx1y, b31=env[k].tx1x-env[k].tx3x, c31=-a31*env[k].tx3x-b31*env[k].tx3y, d31_2=a31*env[k].tx2x+b31*env[k].tx2y+c31;	a31/=d31_2, b31/=d31_2, c31/=d31_2;
			int *txk=texture[env[k].tx], txhk=txh[env[k].tx], txwk=txw[env[k].tx];
		//	b/=a, c/=a, B1/=a, B2/=a, B3/=a, B4/=a, B5/=a, B6/=a;//no: cpt
			for(int k2=0;k2<rgbn;++k2)
			{
			//	cpt=xbuffer[k2]+b*ybuffer[k2]+c*zbuffer[k2];//no: cpt
			/*	auto &ux=xbuffer[k2], &uy=ybuffer[k2], &uz=zbuffer[k2];//402ms fullscreen
				cpt=a*ux+b*uy+c*zbuffer[k2];
				double Xtx=(B1*ux+B2*uy+B3*uz)/cpt;
				double Ytx=(B4*ux+B5*uy+B6*uz)/cpt;//*/
				cpt=a*xbuffer[k2]+b*ybuffer[k2]+c*zbuffer[k2];//398ms fullscreen
				double Xtx=(B1*xbuffer[k2]+B2*ybuffer[k2]+B3*zbuffer[k2])/cpt;
				double Ytx=(B4*xbuffer[k2]+B5*ybuffer[k2]+B6*zbuffer[k2])/cpt;//*/
#if 1
				if(cpt*t>=0&&a12*Xtx+b12*Ytx+c12>0&&a23*Xtx+b23*Ytx+c23>0&&a31*Xtx+b31*Ytx+c31>0)
				{
					admittance=cpt/t;
					if(admittance>wbuffer[k2])
					{
						int Xtx2=int(Xtx)%txwk;if(Xtx2<0)Xtx2+=txwk;
						int Ytx2=int(Ytx)%txhk;if(Ytx2<0)Ytx2+=txhk;
						wbuffer[k2]=admittance, rgb[k2]=txk[Xtx2+Ytx2*txwk];
					}
				}
				//admittance=cpt/t;
				//if(admittance>=0&&a12*Xtx+b12*Ytx+c12>0&&a23*Xtx+b23*Ytx+c23>0&&a31*Xtx+b31*Ytx+c31>0&&admittance>wbuffer[k2])
				//{
				//	int Xtx2=int(Xtx)%txwk;if(Xtx2<0)Xtx2+=txwk;
				//	int Ytx2=int(Ytx)%txhk;if(Ytx2<0)Ytx2+=txhk;
				//	wbuffer[k2]=admittance, rgb[k2]=txk[Xtx2+Ytx2*txwk];
				//}
#else
				if(t*cpt>=0&&a12*Xtx+b12*Ytx+c12>0&&a23*Xtx+b23*Ytx+c23>0&&a31*Xtx+b31*Ytx+c31>0)
				{
					admittance=cpt*cpt/t2;
					if(admittance>wbuffer[k2])
					{
						int Xtx2=int(Xtx)%txwk;if(Xtx2<0)Xtx2+=txwk;
						int Ytx2=int(Ytx)%txhk;if(Ytx2<0)Ytx2+=txhk;
						wbuffer[k2]=admittance, rgb[k2]=txk[Xtx2+Ytx2*txwk];
					}
				}
#endif
			}
		}
		else
		{
			double a12=m12,											r12=a12*Xp3;		a12/=r12;
			double a23=Yp3-m12,	b23=-Xp3,	c23=-b23*m12,			r23=c23;			a23/=r23, b23/=r23, c23/=r23;
			double a31=-Yp3,	b31=Xp3,	c31=-a31*Xp3-b31*Yp3,	r31=b31*m12+c31;	a31/=r31, b31/=r31, c31/=r31;
			int color=env[k].color;
		//	b/=a, c/=a, A1/=a, A2/=a, A3/=a, A4/=a, A5/=a, A6/=a;//cpt
			for(int k2=0;k2<rgbn;++k2)
			{
			//	cpt=xbuffer[k2]+b*ybuffer[k2]+c*zbuffer[k2];//
			/*	auto &ux=xbuffer[k2], &uy=ybuffer[k2], &uz=zbuffer[k2];
				cpt=a*ux+b*uy+c*zbuffer[k2];
				double Xp=(A1*ux+A2*uy+A3*uz)/cpt;
				double Yp=(A4*ux+A5*uy+A6*uz)/cpt;//*/
				cpt=a*xbuffer[k2]+b*ybuffer[k2]+c*zbuffer[k2];
				double Xp=(A1*xbuffer[k2]+A2*ybuffer[k2]+A3*zbuffer[k2])/cpt;
				double Yp=(A4*xbuffer[k2]+A5*ybuffer[k2]+A6*zbuffer[k2])/cpt;//*/
#if 1
				if(cpt*t>=0&&a12*Xp>0&&a23*Xp+b23*Yp+c23>0&&a31*Xp+b31*Yp+c31>0)
				{
					admittance=cpt/t;
					if(admittance>wbuffer[k2])
						wbuffer[k2]=admittance, rgb[k2]=color;
				}
				//admittance=cpt/t;
				//if(admittance>=0&&a12*Xp>0&&a23*Xp+b23*Yp+c23>0&&a31*Xp+b31*Yp+c31>0&&admittance>wbuffer[k2])
				//	wbuffer[k2]=admittance, rgb[k2]=color;
#else
				if(t*cpt>=0&&a12*Xp>0&&a23*Xp+b23*Yp+c23>0&&a31*Xp+b31*Yp+c31>0)
				{
					admittance=cpt*cpt/t2;
					if(admittance>wbuffer[k2])
						wbuffer[k2]=admittance, rgb[k2]=color;
				}
#endif
			}
		}
	}
	for(int k=0;k<ntpols;++k)
	{
		dx=tenv[k].x1-camx, dy=tenv[k].y1-camy, dz=tenv[k].z1-camz, cpt=dx*cax+dy*sax, v1[2]=dx*sax-dy*cax, v1[3]=cpt*say-dz*cay, v1[4]=cpt*cay+dz*say;
		dx=tenv[k].x2-camx, dy=tenv[k].y2-camy, dz=tenv[k].z2-camz, cpt=dx*cax+dy*sax, v2[2]=dx*sax-dy*cax, v2[3]=cpt*say-dz*cay, v2[4]=cpt*cay+dz*say;
		dx=tenv[k].x3-camx, dy=tenv[k].y3-camy, dz=tenv[k].z3-camz, cpt=dx*cax+dy*sax, v3[2]=dx*sax-dy*cax, v3[3]=cpt*say-dz*cay, v3[4]=cpt*cay+dz*say;
		double	au12=v2[2]-v1[2],			bu12=v2[3]-v1[3],			cu12=v2[4]-v1[4];									//u12	=<12>
		double	aux3=v3[2]-v1[2],			bux3=v3[3]-v1[3],			cux3=v3[4]-v1[4];									//ux3	=<13>
		double	a=bu12*cux3-bux3*cu12,		b=aux3*cu12-au12*cux3,		c=au12*bux3-aux3*bu12;								//abc	=<n>	=<12>x<13>
		double	t=a*v1[2]+b*v1[3]+c*v1[4], t2=t*t;
		if(!t)continue;
		double	m12=sqrt(au12*au12+bu12*bu12+cu12*cu12);			au12/=m12,			bu12/=m12,			cu12/=m12;		//u12	=<u12>	=<12>/|12|
				cpt=aux3*au12+bux3*bu12+cux3*cu12;					aux3-=cpt*au12,		bux3-=cpt*bu12,		cux3-=cpt*cu12;	//ux3	=<x3>	=<13>-<u12>.<13><u12>
				cpt=sqrt(aux3*aux3+bux3*bux3+cux3*cux3);			aux3/=cpt,			bux3/=cpt,			cux3/=cpt;		//ux3	=<ux3>	=<x3>/|x3|
		double	ux3_1=aux3*v1[2]+bux3*v1[3]+cux3*v1[4];
		double	u12_1=au12*v1[2]+bu12*v1[3]+cu12*v1[4];
		double	Xp3=aux3*v3[2]+bux3*v3[3]+cux3*v3[4]-ux3_1;
		double	Yp3=au12*v3[2]+bu12*v3[3]+cu12*v3[4]-u12_1;
		double	A1=t*aux3-a*ux3_1, A2=t*bux3-b*ux3_1, A3=t*cux3-c*ux3_1;
		double	A4=t*au12-a*u12_1, A5=t*bu12-b*u12_1, A6=t*cu12-c*u12_1;
		if(tenv[k].tx>=0)
		{
			double B1=tenv[k].txXX*A1+tenv[k].txXY*A4+tenv[k].tx1x*a, B2=tenv[k].txXX*A2+tenv[k].txXY*A5+tenv[k].tx1x*b, B3=tenv[k].txXX*A3+tenv[k].txXY*A6+tenv[k].tx1x*c;
			double B4=tenv[k].txYX*A1+tenv[k].txYY*A4+tenv[k].tx1y*a, B5=tenv[k].txYX*A2+tenv[k].txYY*A5+tenv[k].tx1y*b, B6=tenv[k].txYX*A3+tenv[k].txYY*A6+tenv[k].tx1y*c;
			double a12=tenv[k].tx1y-tenv[k].tx2y, b12=tenv[k].tx2x-tenv[k].tx1x, c12=-a12*tenv[k].tx1x-b12*tenv[k].tx1y, d12_3=a12*tenv[k].tx3x+b12*tenv[k].tx3y+c12;	a12/=d12_3, b12/=d12_3, c12/=d12_3;
			double a23=tenv[k].tx2y-tenv[k].tx3y, b23=tenv[k].tx3x-tenv[k].tx2x, c23=-a23*tenv[k].tx2x-b23*tenv[k].tx2y, d23_1=a23*tenv[k].tx1x+b23*tenv[k].tx1y+c23;	a23/=d23_1, b23/=d23_1, c23/=d23_1;
			double a31=tenv[k].tx3y-tenv[k].tx1y, b31=tenv[k].tx1x-tenv[k].tx3x, c31=-a31*tenv[k].tx3x-b31*tenv[k].tx3y, d31_2=a31*tenv[k].tx2x+b31*tenv[k].tx2y+c31;	a31/=d31_2, b31/=d31_2, c31/=d31_2;
			int *txk=texture[tenv[k].tx], txhk=txh[tenv[k].tx], txwk=txw[tenv[k].tx];
		//	b/=a, c/=a, B1/=a, B2/=a, B3/=a, B4/=a, B5/=a, B6/=a;//cpt
			for(int k2=0;k2<rgbn;++k2)
			{
			//	cpt=a*xbuffer[k2]+b*ybuffer[k2]+c*zbuffer[k2];//
			/*	auto &ux=xbuffer[k2], &uy=ybuffer[k2], &uz=zbuffer[k2];
				cpt=a*ux+b*uy+c*zbuffer[k2];
				double Xtx=(B1*ux+B2*uy+B3*uz)/cpt;
				double Ytx=(B4*ux+B5*uy+B6*uz)/cpt;//*/
				cpt=a*xbuffer[k2]+b*ybuffer[k2]+c*zbuffer[k2];
				double Xtx=(B1*xbuffer[k2]+B2*ybuffer[k2]+B3*zbuffer[k2])/cpt;
				double Ytx=(B4*xbuffer[k2]+B5*ybuffer[k2]+B6*zbuffer[k2])/cpt;//*/
#if 1
				if(cpt*t>=0&&a12*Xtx+b12*Ytx+c12>0&&a23*Xtx+b23*Ytx+c23>0&&a31*Xtx+b31*Ytx+c31>0)
				{
					admittance=cpt/t;
					if(admittance>wbuffer[k2])
					{
						int Xtx2=int(Xtx)%txwk;if(Xtx2<0)Xtx2+=txwk;
						int Ytx2=int(Ytx)%txhk;if(Ytx2<0)Ytx2+=txhk;
						((unsigned char*)&rgb[k2])[0]=((unsigned char*)&rgb[k2])[0]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[0]>>8;//b	//f(x,y)=x*y
						((unsigned char*)&rgb[k2])[1]=((unsigned char*)&rgb[k2])[1]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[1]>>8;//g
						((unsigned char*)&rgb[k2])[2]=((unsigned char*)&rgb[k2])[2]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[2]>>8;//r
					}
				}
			//	admittance=cpt/t;
			//	if(admittance>=0&&a12*Xtx+b12*Ytx+c12>0&&a23*Xtx+b23*Ytx+c23>0&&a31*Xtx+b31*Ytx+c31>0&&admittance>wbuffer[k2])
			////	if(admittance>wbuffer[k2]&&a12*Xtx+b12*Ytx+c12>0&&a23*Xtx+b23*Ytx+c23>0&&a31*Xtx+b31*Ytx+c31>0)
			//	{
			//		int Xtx2=int(Xtx)%txwk;if(Xtx2<0)Xtx2+=txwk;
			//		int Ytx2=int(Ytx)%txhk;if(Ytx2<0)Ytx2+=txhk;
			//		((unsigned char*)&rgb[k2])[0]=((unsigned char*)&rgb[k2])[0]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[0]>>8;//b	//f(x,y)=x*y
			//		((unsigned char*)&rgb[k2])[1]=((unsigned char*)&rgb[k2])[1]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[1]>>8;//g
			//		((unsigned char*)&rgb[k2])[2]=((unsigned char*)&rgb[k2])[2]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[2]>>8;//r
			//	}
#else
				if(t*cpt>=0&&a12*Xtx+b12*Ytx+c12>0&&a23*Xtx+b23*Ytx+c23>0&&a31*Xtx+b31*Ytx+c31>0)
				{
					admittance=cpt*cpt/t2;
					if(admittance>wbuffer[k2])
					{
						int Xtx2=int(Xtx)%txwk;if(Xtx2<0)Xtx2+=txwk;
						int Ytx2=int(Ytx)%txhk;if(Ytx2<0)Ytx2+=txhk;
					//	wbuffer[k2]=admittance;
					//	rgb[k2]=txk[Xtx2+Ytx2*txwk];
						((unsigned char*)&rgb[k2])[0]=((unsigned char*)&rgb[k2])[0]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[0]>>8;//b	//f(x,y)=x*y
						((unsigned char*)&rgb[k2])[1]=((unsigned char*)&rgb[k2])[1]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[1]>>8;//g
						((unsigned char*)&rgb[k2])[2]=((unsigned char*)&rgb[k2])[2]*((unsigned char*)&txk[Xtx2+Ytx2*txwk])[2]>>8;//r
					}
				}
#endif
			}
		}
		else
		{
			double a12=m12,											r12=a12*Xp3;		a12/=r12;
			double a23=Yp3-m12,	b23=-Xp3,	c23=-b23*m12,			r23=c23;			a23/=r23, b23/=r23, c23/=r23;
			double a31=-Yp3,	b31=Xp3,	c31=-a31*Xp3-b31*Yp3,	r31=b31*m12+c31;	a31/=r31, b31/=r31, c31/=r31;
			int color=tenv[k].color;
		//	b/=a, c/=a, A1/=a, A2/=a, A3/=a, A4/=a, A5/=a, A6/=a;//cpt
			for(int k2=0;k2<rgbn;++k2)
			{
			//	cpt=xbuffer[k2]+b*ybuffer[k2]+c*zbuffer[k2];//
			/*	auto &ux=xbuffer[k2], &uy=ybuffer[k2], &uz=zbuffer[k2];
				cpt=a*ux+b*uy+c*zbuffer[k2];
				double Xp=(A1*ux+A2*uy+A3*uz)/cpt;
				double Yp=(A4*ux+A5*uy+A6*uz)/cpt;//*/
				cpt=a*xbuffer[k2]+b*ybuffer[k2]+c*zbuffer[k2];
				double Xp=(A1*xbuffer[k2]+A2*ybuffer[k2]+A3*zbuffer[k2])/cpt;
				double Yp=(A4*xbuffer[k2]+A5*ybuffer[k2]+A6*zbuffer[k2])/cpt;//*/
#if 1
				if(cpt*t>=0&&a12*Xp>0&&a23*Xp+b23*Yp+c23>0&&a31*Xp+b31*Yp+c31>0)
				{
					admittance=cpt/t;
					if(admittance>wbuffer[k2])
					{
						((unsigned char*)&rgb[k2])[0]=((unsigned char*)&rgb[k2])[0]*((unsigned char*)&color)[0]>>8;//b	//f(x,y)=x*y
						((unsigned char*)&rgb[k2])[1]=((unsigned char*)&rgb[k2])[1]*((unsigned char*)&color)[1]>>8;//g
						((unsigned char*)&rgb[k2])[2]=((unsigned char*)&rgb[k2])[2]*((unsigned char*)&color)[2]>>8;//r
					}
				}
			//	admittance=cpt/t;
			//	if(admittance>=0&&a12*Xp>0&&a23*Xp+b23*Yp+c23>0&&a31*Xp+b31*Yp+c31>0&&admittance>wbuffer[k2])
			////	if(admittance>wbuffer[k2]&&a12*Xp>0&&a23*Xp+b23*Yp+c23>0&&a31*Xp+b31*Yp+c31>0)
			//	{
			//		((unsigned char*)&rgb[k2])[0]=((unsigned char*)&rgb[k2])[0]*((unsigned char*)&color)[0]>>8;//b	//f(x,y)=x*y
			//		((unsigned char*)&rgb[k2])[1]=((unsigned char*)&rgb[k2])[1]*((unsigned char*)&color)[1]>>8;//g
			//		((unsigned char*)&rgb[k2])[2]=((unsigned char*)&rgb[k2])[2]*((unsigned char*)&color)[2]>>8;//r
			//	}
#else
				if(t*cpt>=0&&a12*Xp>0&&a23*Xp+b23*Yp+c23>0&&a31*Xp+b31*Yp+c31>0)
				{
					admittance=cpt*cpt/t2;
					if(admittance>wbuffer[k2])
					{
					//	wbuffer[k2]=admittance;
					//	rgb[k2]=color;
						((unsigned char*)&rgb[k2])[0]=((unsigned char*)&rgb[k2])[0]*((unsigned char*)&color)[0]>>8;//b	//f(x,y)=x*y
						((unsigned char*)&rgb[k2])[1]=((unsigned char*)&rgb[k2])[1]*((unsigned char*)&color)[1]>>8;//g
						((unsigned char*)&rgb[k2])[2]=((unsigned char*)&rgb[k2])[2]*((unsigned char*)&color)[2]>>8;//r
					}
				}
#endif
			}
		}
	}
	QueryPerformanceFrequency(&li);freq=li.QuadPart;
	QueryPerformanceCounter(&li);
	linelen=sprintf_s(line, "fps=%.10f, T=%.10fms, dcam=%.10f, fov=%.10f", freq/double(li.QuadPart-nticks), 1000.*(li.QuadPart-nticks)/freq, dcam, fov*720/TWOPI);
	nticks=li.QuadPart;
	if(linelen>0)TextOutA(hMemDC, 0, h-16, line, linelen);
	BitBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
}
void				NonlinearSW::rebuffer1()//F2 cylindrical panorama
{
	double Xn, Yn, *ux_y=xbuffer, *uy_y=ybuffer, *uz_y=zbuffer;
	for(int ys=0;ys<h;++ys)
	{
		Yn=(ys-Y0)*tanfov/X0;
		double th=sqrt(1+Yn*Yn), uy_yx=Yn/th;
		for(int xs=0;xs<w;++xs)
		{
			Xn=(double(xs)/X0-1)*fov;
			ux_y[xs]=sin(Xn)/th, uy_y[xs]=uy_yx, uz_y[xs]=cos(Xn)/th;
		}
		ux_y=ux_y+w, uy_y=uy_y+w, uz_y=uz_y+w;
	}
}
void				NonlinearSW::rebuffer2()//F3 sideways cylindrical panorama
{
	double Xn, Yn, *ux_y=xbuffer, *uy_y=ybuffer, *uz_y=zbuffer;
	for(int ys=0;ys<h;++ys)
	{
		Yn=(ys-Y0)*fov/X0;
		double sinYn=sin(Yn), cosYn=cos(Yn);
		for(int xs=0;xs<w;++xs)
		{
			Xn=(double(xs)/X0-1)*tanfov;
			double th=sqrt(1+Xn*Xn);
			ux_y[xs]=Xn/th, uy_y[xs]=sinYn/th, uz_y[xs]=cosYn/th;
		}
		ux_y=ux_y+w, uy_y=uy_y+w, uz_y=uz_y+w;
	}
}
void				NonlinearSW::rebuffer3()//F4 spherical
{
	double Xn, Yn, *ux_y=xbuffer, *uy_y=ybuffer, *uz_y=zbuffer;
	for(int ys=0;ys<h;++ys)
	{
		Yn=(ys-Y0)*fov/X0;
		for(int xs=0;xs<w;++xs)
		{
			Xn=(double(xs)/X0-1)*fov;
			double th=sqrt(Xn*Xn+Yn*Yn);
			if(th==0){ux_y[xs]=uy_y[xs]=0, uz_y[xs]=1;continue;}
			double sr=sin(th)/th;
			ux_y[xs]=Xn*sr, uy_y[xs]=Yn*sr, uz_y[xs]=cos(th);
		}
		ux_y=ux_y+w, uy_y=uy_y+w, uz_y=uz_y+w;
	}
}
void				NonlinearSW::rebuffer4()//F5 fuzzy
{
	double Xn, Yn, *ux_y=xbuffer, *uy_y=ybuffer, *uz_y=zbuffer;
	for(int ys=0;ys<h;++ys)
	{
		for(int xs=0;xs<w;++xs)
		{
			Yn=(ys-Y0)*tanfov/X0;
			Xn=(double(xs)/X0-1)*tanfov;
										//	Yn=exp(Yn);

										//	Yn=(0.0000005-0.000001*rand())*tanfov+exp(Yn);
										//	Xn+=(0.0000005-0.000001*rand())*tanfov;

											Yn+=(0.0000005-0.000001*rand())*tanfov;
											Xn+=(0.0000005-0.000001*rand())*tanfov;

										//	Xn+=(0.0000001-0.0000002*rand())*tanfov	+0.05*exp(10*Xn);
										//	Xn+=0.1*cos(0.00001*rand());
										//	Xn+=0.05*cos(0.1*xs);
			double mag=sqrt(1+Xn*Xn+Yn*Yn);
			ux_y[xs]=Xn/mag, uy_y[xs]=Yn/mag, uz_y[xs]=1/mag;
											uy_y[xs]+=0.01*cos(80*Xn);

			//Yn=(ys-Y0)*tanfov/X0;
			//Xn=(double(xs)/X0-1)*fov;
			//								Yn+=(0.000001-0.000002*rand())*tanfov;
			//								Xn+=(0.000001-0.000002*rand())*tanfov;
			//double r=sqrt(1+Xn*Xn+Yn*Yn);
			//ux_y[xs]=Xn/r, uy_y[xs]=Yn/r, uz_y[xs]=1/r;
		}
		ux_y=ux_y+w, uy_y=uy_y+w, uz_y=uz_y+w;
	}
}
void				NonlinearSW::pushTexture(int *texture_)
{
	++ntx;
	texture=(int**)realloc(texture, ntx*sizeof(int*)), texture[ntx-1]=texture_;
}
void				NonlinearSW::enqueueTextures()
{
}
int*				NonlinearSW::popTexture()
{
	ntx--;
	return texture[ntx];
}
void				NonlinearSW::clearTextures()
{
	for(int k=0;k<ntx;++k)
		free(texture[k]);
	ntx=0;
}
void				NonlinearSW::finish()
{
	DeleteObject(SelectObject(hMemDC, hBitmap)), DeleteDC(hMemDC);
	for(int k=0;k<ntx;++k)free(texture[k]);
	free(texture), free(wbuffer), free(xbuffer), free(ybuffer), free(zbuffer);
}
struct			NonlinearCL:private Mode
{
	int					*texture, *txi, txsize;
	int					*rgb, rgbn;
	int					err, gpu;
	_cl_context			*context[2];
	_cl_command_queue	*commandQueue[2];
	static struct CLWork
	{
		const char		*source, *kernelName, *fileName[2];
		_cl_kernel		*kernel[2];
		int				loadBinary(const char *addr, unsigned char **pBin, unsigned int *pBinLen);
		int				saveBinary(const char *addr, unsigned char *bin, unsigned int binLen);
		void			createKernelWithBinaryOrSource(_cl_device_id *deviceID, _cl_context *context, int gpu);
	} work[6];
	int					rebuffer;
	_cl_mem				*clBuffer[8];//wbuffer, xbuffer, ybuffer, zbuffer, param, rgb, texture
	HDC__				*hMemDC;
	HBITMAP__			*hBitmap;
	void				initiate();
	void				function();
	void				switchDevice(int dev_);
	void				resize();
	void				changeFov();
	void				render();
	void				rebuffer1();
	void				rebuffer2();
	void				rebuffer3();
	void				rebuffer4();
	void				pushTexture(int *texture_);
	void				enqueueTextures();
	int*				popTexture();
	void				clearTextures();
	void				finish();
} nlcl;
NonlinearCL::CLWork	NonlinearCL::work[]=
{
	{
		R"(__kernel void textured(__global float *wbuffer, __global float *xbuffer, __global float *ybuffer, __global float *zbuffer, __global float *c, __global int *rgb, __global int *texture)
{
	int k2=get_global_id(0), txik=c[20], txhk=c[21], txwk=c[22];
	float cpt=c[6]*xbuffer[k2]+c[7]*ybuffer[k2]+c[8]*zbuffer[k2];
	float Xtx=(c[0]*xbuffer[k2]+c[1]*ybuffer[k2]+c[2]*zbuffer[k2])/cpt;
	float Ytx=(c[3]*xbuffer[k2]+c[4]*ybuffer[k2]+c[5]*zbuffer[k2])/cpt;
	if(c[18]*cpt>=0&&c[9]*Xtx+c[10]*Ytx+c[11]>0&&c[12]*Xtx+c[13]*Ytx+c[14]>0&&c[15]*Xtx+c[16]*Ytx+c[17]>0)
	{
		float admittance=cpt*cpt/c[19];
		if(admittance>wbuffer[k2])
		{
			int Xtx2=(int)(Xtx)%txwk;if(Xtx2<0)Xtx2+=txwk;
			int Ytx2=(int)(Ytx)%txhk;if(Ytx2<0)Ytx2+=txhk;
			wbuffer[k2]=admittance, rgb[k2]=texture[txik+Ytx2*txwk+Xtx2];
		}
	}
})",
		"textured",		{"X-10 textured CPU.bin",	"X-10 textured GPU.bin"}
	},
	{
		R"(__kernel void solid(__global float *wbuffer, __global float *xbuffer, __global float *ybuffer, __global float *zbuffer, __global float *c, __global int *rgb)
{
	int k2=get_global_id(0), color=c[20];
	float cpt=c[6]*xbuffer[k2]+c[7]*ybuffer[k2]+c[8]*zbuffer[k2];
	float Xp=(c[0]*xbuffer[k2]+c[1]*ybuffer[k2]+c[2]*zbuffer[k2])/cpt;
	float Yp=(c[3]*xbuffer[k2]+c[4]*ybuffer[k2]+c[5]*zbuffer[k2])/cpt;
	if(c[18]*cpt>=0&&c[9]*Xp>0&&c[12]*Xp+c[13]*Yp+c[14]>0&&c[15]*Xp+c[16]*Yp+c[17]>0)
	{
		float admittance=cpt*cpt/c[19];
		if(admittance>wbuffer[k2])
			wbuffer[k2]=admittance, rgb[k2]=color;
	}
})",
		"solid",		{"X-10 solid CPU.bin",		"X-10 solid GPU.bin"}
	},
	{
		R"(__kernel void rebuffer1(__global float *xbuffer, __global float *ybuffer, __global float *zbuffer, __global float *param)//w, h, fov, tanfov
{
	int k2=get_global_id(0);
	float X0=param[0]/2, Y0=param[1]/2;
	float Xs=((k2%(int)param[0])/X0-1)*param[2], Ys=(k2/(int)param[0]-Y0)*param[3]/X0, r=sqrt(1+Ys*Ys);
	xbuffer[k2]=sin(Xs)/r, ybuffer[k2]=Ys/r, zbuffer[k2]=cos(Xs)/r;
})",
		"rebuffer1",	{"X-10 rebuffer1 CPU.bin",	"X-10 rebuffer1 GPU.bin"}
	},
	{
		R"(__kernel void rebuffer2(__global float *xbuffer, __global float *ybuffer, __global float *zbuffer, __global float *param)//w, h, fov, tanfov
{
	int k2=get_global_id(0);
	float X0=param[0]/2, Y0=param[1]/2;
	float Xs=((k2%(int)param[0])/X0-1)*param[3], Ys=(k2/(int)param[0]-Y0)*param[2]/X0, r=sqrt(1+Xs*Xs);
	xbuffer[k2]=Xs/r, ybuffer[k2]=sin(Ys)/r, zbuffer[k2]=cos(Ys)/r;
})",
		"rebuffer2",	{"X-10 rebuffer2 CPU.bin",	"X-10 rebuffer2 GPU.bin"}
	},
	{
		R"(__kernel void rebuffer3(__global float *xbuffer, __global float *ybuffer, __global float *zbuffer, __global float *param)//w, h, fov, tanfov
{
	int k2=get_global_id(0);
	float X0=param[0]/2, Y0=param[1]/2;
	float Xs=((k2%(int)param[0])/X0-1)*param[2], Ys=(k2/(int)param[0]-Y0)*param[2]/X0, r=sqrt(Xs*Xs+Ys*Ys);
	if(r==0)
		xbuffer[k2]=ybuffer[k2]=0, zbuffer[k2]=1;
	else
	{
		float sr=sin(r)/r;
		xbuffer[k2]=Xs*sr, ybuffer[k2]=Ys*sr, zbuffer[k2]=cos(r);
	}
})",
		"rebuffer3",	{"X-10 rebuffer3 CPU.bin",	"X-10 rebuffer3 GPU.bin"}
	},
	{
		R"(__kernel void rebuffer4(__global float *xbuffer, __global float *ybuffer, __global float *zbuffer, __global float *param)//w, h, fov, tanfov
{
	int k2=get_global_id(0);
	float X0=param[0]/2, Y0=param[1]/2;
	float Xs=((k2%(int)param[0])/X0-1)*param[3], Ys=(k2/(int)param[0]-Y0)*param[3]/X0;
	float r=sqrt(1+Xs*Xs+Ys*Ys);
	xbuffer[k2]=Xs/r, ybuffer[k2]=Ys/r, zbuffer[k2]=1/r;

	ybuffer[k2]+=0.01f*cos(80*Xs);
})",
		"rebuffer4",	{"X-10 rebuffer4 CPU.bin",	"X-10 rebuffer4 GPU.bin"}
	}
};
int					NonlinearCL::CLWork::loadBinary(const char *addr, unsigned char **pBin, unsigned int *pBinLen)
{
	struct stat info;
	if(!stat(addr, &info))
	{
		*pBinLen=info.st_size;
		*pBin=(unsigned char*)malloc(*pBinLen*sizeof(unsigned char));
		std::ifstream file(addr, std::ios::in|std::ios::binary);
		if(file.is_open())
		{
			file.read((char*)*pBin, *pBinLen);
			file.close();
			return 0;
		}
	}
	*pBinLen=0, *pBin=0;
	return 1;
}
int					NonlinearCL::CLWork::saveBinary(const char *addr, unsigned char *bin, unsigned int binLen)
{
	std::ofstream file(addr, std::ios::out|std::ios::binary);
	if(file.is_open())
	{
		file.write((const char*)bin, binLen);
		file.close();
		return 0;
	}
	return 1;
}
void				NonlinearCL::CLWork::createKernelWithBinaryOrSource(_cl_device_id *deviceID, _cl_context *context, int gpu)
{
	int err;
	unsigned int len;
	char buf[1024];
	_cl_program *program=0;
	unsigned int binarylen;
	unsigned char *binary;
	int alreadyHere=0;
	GetModuleFileNameA(0, buf, 1024);		len=strlen(buf);
	for(int k=len-1;k>=0;k--)
	{
		if(buf[k]=='\\')
		{
			buf[k+1]='\0';
			strcat_s(buf, 1024*sizeof(char), fileName[gpu]);
			if(!loadBinary(buf, &binary, &binarylen))
			{
				int status;
				program=clCreateProgramWithBinary(context, 1, &deviceID, &binarylen, (const unsigned char**)&binary, &status, &err);
				if(!err)alreadyHere=1;
				err=clBuildProgram(program, 0, 0, 0, 0, 0);
				if(err)alreadyHere=0;
				free(binary);
			}
			break;
		}
	}
	if(!alreadyHere)
	{
		program=clCreateProgramWithSource(context, 1, &source, 0, &err);
		err=clBuildProgram(program, 0, 0, 0, 0, 0);

		err=clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(unsigned int), &binarylen, &len);		len/=sizeof(unsigned int);
		binary=(unsigned char*)malloc(binarylen);
		err=clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(unsigned char*), &binary, &len);			len/=sizeof(unsigned char*);
		err=saveBinary(buf, binary, binarylen);
		free(binary);
	}
//	err=clGetProgramBuildInfo(program, deviceID, CL_PROGRAM_BUILD_LOG, 1024*sizeof(char), buf, &len);		len/=sizeof(unsigned char);
	kernel[gpu]=clCreateKernel(program, kernelName, &err);
	err=clReleaseProgram(program);
}
void				NonlinearCL::initiate()
{
	texture=0, txi=0, txsize=0;
	for(int k=0;k<8;++k)clBuffer[k]=0;
	rgbn=h*w;
	hMemDC=CreateCompatibleDC(hDC);
	tagBITMAPINFO bmpInfo={{sizeof(tagBITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
	hBitmap=(HBITMAP__*)SelectObject(hMemDC, CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&rgb, 0, 0));
	SetBkMode(hMemDC, TRANSPARENT);
	int properties[3]={CL_CONTEXT_PLATFORM};
	_cl_device_id *deviceID;
	err=clGetPlatformIDs(1, (_cl_platform_id**)&properties[1], 0);
	gpu=1;
	err=clGetDeviceIDs((_cl_platform_id*)properties[1], CL_DEVICE_TYPE_GPU, 1, &deviceID, 0);
	context[gpu]=clCreateContext(properties, 1, &deviceID, 0, 0, &err);
//	commandQueue[gpu]=clCreateCommandQueueWithProperties(context[gpu], deviceID, 0, &err);
	commandQueue[gpu]=clCreateCommandQueue(context[gpu], deviceID, 0, &err);
	for(int k=0;k<6;++k)
		work[k].createKernelWithBinaryOrSource(deviceID, context[gpu], gpu);

	gpu=0;
	err=clGetDeviceIDs((_cl_platform_id*)properties[1], CL_DEVICE_TYPE_CPU, 1, &deviceID, 0);
	context[gpu]=clCreateContext(properties, 1, &deviceID, 0, 0, &err);
	commandQueue[gpu]=clCreateCommandQueue(context[gpu], deviceID, 0, &err);
	for(int k=0;k<6;++k)
		work[k].createKernelWithBinaryOrSource(deviceID, context[gpu], gpu);
	function();
}
void				NonlinearCL::function()
{
	for(int k=0;k<6;++k)err=clReleaseMemObject(clBuffer[k]);
	clBuffer[0]=clCreateBuffer(context[gpu], CL_MEM_READ_WRITE,	rgbn*sizeof(float	), 0, 0);//wbuffer				//are mem flags right?
	clBuffer[1]=clCreateBuffer(context[gpu], CL_MEM_READ_ONLY,	rgbn*sizeof(float	), 0, 0);//xbuffer
	clBuffer[2]=clCreateBuffer(context[gpu], CL_MEM_READ_ONLY,	rgbn*sizeof(float	), 0, 0);//ybuffer
	clBuffer[3]=clCreateBuffer(context[gpu], CL_MEM_READ_ONLY,	rgbn*sizeof(float	), 0, 0);//zbuffer
	clBuffer[4]=clCreateBuffer(context[gpu], CL_MEM_READ_ONLY,	  23*sizeof(float	), 0, 0);//params
	clBuffer[5]=clCreateBuffer(context[gpu], CL_MEM_WRITE_ONLY,	rgbn*sizeof(int	), 0, 0);//rgb
	for(int k=0;k<2;++k)
	{
		err=clSetKernelArg(work[k].kernel[gpu], 0, sizeof(_cl_mem*), &clBuffer[0]);//textured solid
		err=clSetKernelArg(work[k].kernel[gpu], 1, sizeof(_cl_mem*), &clBuffer[1]);
		err=clSetKernelArg(work[k].kernel[gpu], 2, sizeof(_cl_mem*), &clBuffer[2]);
		err=clSetKernelArg(work[k].kernel[gpu], 3, sizeof(_cl_mem*), &clBuffer[3]);
		err=clSetKernelArg(work[k].kernel[gpu], 4, sizeof(_cl_mem*), &clBuffer[4]);
		err=clSetKernelArg(work[k].kernel[gpu], 5, sizeof(_cl_mem*), &clBuffer[5]);
	}
	for(int k=0;k<4;++k)
	{
		err=clSetKernelArg(work[2+k].kernel[gpu], 0, sizeof(_cl_mem*), &clBuffer[1]);//rebuffer1 2 3 4
		err=clSetKernelArg(work[2+k].kernel[gpu], 1, sizeof(_cl_mem*), &clBuffer[2]);
		err=clSetKernelArg(work[2+k].kernel[gpu], 2, sizeof(_cl_mem*), &clBuffer[3]);
		err=clSetKernelArg(work[2+k].kernel[gpu], 3, sizeof(_cl_mem*), &clBuffer[4]);
	}
}
void				NonlinearCL::switchDevice(int gpu_)
{
	float *buf[4];
	for(int k=0;k<4;++k)
	{
		buf[k]=(float*)malloc(rgbn*sizeof(float));
		clEnqueueReadBuffer(commandQueue[gpu], clBuffer[k], 1, 0, rgbn*sizeof(float), buf[k], 0, 0, 0);
	}
	gpu=gpu_;
	function();
	for(int k=0;k<4;++k)
	{
		clEnqueueWriteBuffer(commandQueue[gpu], clBuffer[k], 1, 0, rgbn*sizeof(float), buf[k], 0, 0, 0);
		free(buf[k]);
	}
}
void				NonlinearCL::resize()
{
	rgbn=h*w;
	function();
	changeFov();
	DeleteObject((HBITMAP__*)SelectObject(hMemDC, hBitmap));
	tagBITMAPINFO bmpInfo={{sizeof(tagBITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
	hBitmap=(HBITMAP__*)SelectObject(hMemDC, CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&rgb, 0, 0));
	SetBkMode(hMemDC, TRANSPARENT);
}
void				NonlinearCL::changeFov()
{
	float params[]={float(w), float(h), float(fov), float(tanfov)};
	clEnqueueWriteBuffer(commandQueue[gpu], clBuffer[4], 1, 0, 4*sizeof(float), params, 0, 0, 0);
	clEnqueueNDRangeKernel(commandQueue[gpu], work[2+rebuffer].kernel[gpu], 1, 0, (unsigned int*)&rgbn, 0, 0, 0, 0);//rebuffer
}
void				NonlinearCL::render()
{
	{
		float w=0;
		clEnqueueFillBuffer(commandQueue[gpu], clBuffer[0], &w, sizeof(float), 0, rgbn*sizeof(float), 0, 0, 0);
		int rgb=0x00FFFFFF;
		clEnqueueFillBuffer(commandQueue[gpu], clBuffer[5], &rgb, sizeof(int), 0, rgbn*sizeof(int), 0, 0, 0);
	}
	double dx, dy, dz, cpt, v1[5], v2[5], v3[5];
	for(int k=0;k<npols;++k)
	{
		dx=env[k].x1-camx, dy=env[k].y1-camy, dz=env[k].z1-camz, cpt=dx*cax+dy*sax, v1[2]=dx*sax-dy*cax, v1[3]=cpt*say-dz*cay, v1[4]=cpt*cay+dz*say;
		dx=env[k].x2-camx, dy=env[k].y2-camy, dz=env[k].z2-camz, cpt=dx*cax+dy*sax, v2[2]=dx*sax-dy*cax, v2[3]=cpt*say-dz*cay, v2[4]=cpt*cay+dz*say;
		dx=env[k].x3-camx, dy=env[k].y3-camy, dz=env[k].z3-camz, cpt=dx*cax+dy*sax, v3[2]=dx*sax-dy*cax, v3[3]=cpt*say-dz*cay, v3[4]=cpt*cay+dz*say;
		double	au12=v2[2]-v1[2],			bu12=v2[3]-v1[3],			cu12=v2[4]-v1[4];									//u12	=<12>
		double	aux3=v3[2]-v1[2],			bux3=v3[3]-v1[3],			cux3=v3[4]-v1[4];									//ux3	=<13>
		double	a=bu12*cux3-bux3*cu12,	b=aux3*cu12-au12*cux3,	c=au12*bux3-aux3*bu12;										//abc	=<n>	=<12>x<13>
		double t=a*v1[2]+b*v1[3]+c*v1[4], t2=t*t;
		if(!t)continue;
		double	m12=sqrt(au12*au12+bu12*bu12+cu12*cu12);			au12/=m12,			bu12/=m12,			cu12/=m12;		//u12	=<u12>	=<12>/|12|
				cpt=aux3*au12+bux3*bu12+cux3*cu12;					aux3-=cpt*au12,		bux3-=cpt*bu12,		cux3-=cpt*cu12;	//ux3	=<x3>	=<13>-<u12>.<13><u12>
				cpt=sqrt(aux3*aux3+bux3*bux3+cux3*cux3);			aux3/=cpt,			bux3/=cpt,			cux3/=cpt;		//ux3	=<ux3>	=<x3>/|x3|
		double	ux3_1=aux3*v1[2]+bux3*v1[3]+cux3*v1[4];
		double	u12_1=au12*v1[2]+bu12*v1[3]+cu12*v1[4];
		double	Xp3=aux3*v3[2]+bux3*v3[3]+cux3*v3[4]-ux3_1;
		double	Yp3=au12*v3[2]+bu12*v3[3]+cu12*v3[4]-u12_1;
		double	A1=t*aux3-a*ux3_1, A2=t*bux3-b*ux3_1, A3=t*cux3-c*ux3_1;
		double	A4=t*au12-a*u12_1, A5=t*bu12-b*u12_1, A6=t*cu12-c*u12_1;
		if(env[k].tx>=0)
		{
			double B1=env[k].txXX*A1+env[k].txXY*A4+env[k].tx1x*a,	B2=env[k].txXX*A2+env[k].txXY*A5+env[k].tx1x*b,	B3=env[k].txXX*A3+env[k].txXY*A6+env[k].tx1x*c;
			double B4=env[k].txYX*A1+env[k].txYY*A4+env[k].tx1y*a,	B5=env[k].txYX*A2+env[k].txYY*A5+env[k].tx1y*b,	B6=env[k].txYX*A3+env[k].txYY*A6+env[k].tx1y*c;
			double a12=env[k].tx1y-env[k].tx2y,	b12=env[k].tx2x-env[k].tx1x,	c12=-a12*env[k].tx1x-b12*env[k].tx1y;	double d12_3=a12*env[k].tx3x+b12*env[k].tx3y+c12;	a12/=d12_3,	b12/=d12_3,	c12/=d12_3;
			double a23=env[k].tx2y-env[k].tx3y,	b23=env[k].tx3x-env[k].tx2x,	c23=-a23*env[k].tx2x-b23*env[k].tx2y;	double d23_1=a23*env[k].tx1x+b23*env[k].tx1y+c23;	a23/=d23_1,	b23/=d23_1,	c23/=d23_1;
			double a31=env[k].tx3y-env[k].tx1y,	b31=env[k].tx1x-env[k].tx3x,	c31=-a31*env[k].tx3x-b31*env[k].tx3y;	double d31_2=a31*env[k].tx2x+b31*env[k].tx2y+c31;	a31/=d31_2,	b31/=d31_2,	c31/=d31_2;
			float params[]={float(B1), float(B2), float(B3), float(B4), float(B5), float(B6), float(a), float(b), float(c), float(a12), float(b12), float(c12), float(a23), float(b23), float(c23), float(a31), float(b31), float(c31), float(t), float(t2), float(txi[env[k].tx]), float(txh[env[k].tx]), float(txw[env[k].tx])};
			clFinish(commandQueue[gpu]);
			clEnqueueWriteBuffer(commandQueue[gpu], clBuffer[4], 1, 0, 23*sizeof(float), params, 0, 0, 0);
			clEnqueueNDRangeKernel(commandQueue[gpu], work[0].kernel[gpu], 1, 0, (unsigned int*)&rgbn, 0, 0, 0, 0);//textured
		}
		else
		{
			double a12=m12,						b12=0,							c12=0;									double d12_3=a12*Xp3;								a12/=d12_3;
			double a23=Yp3-m12,					b23=-Xp3,						c23=-b23*m12;							double d23_1=c23;									a23/=d23_1,	b23/=d23_1,	c23/=d23_1;
			double a31=-Yp3,					b31=Xp3,						c31=-a31*Xp3-b31*Yp3;					double d31_2=b31*m12+c31;							a31/=d31_2,	b31/=d31_2,	c31/=d31_2;
			float params[]={float(A1), float(A2), float(A3), float(A4), float(A5), float(A6), float(a), float(b), float(c), float(a12), float(b12), float(c12), float(a23), float(b23), float(c23), float(a31), float(b31), float(c31), float(t), float(t2), float(env[k].color)};
			clFinish(commandQueue[gpu]);
			clEnqueueWriteBuffer(commandQueue[gpu], clBuffer[4], 1, 0, 21*sizeof(float), params, 0, 0, 0);
			clEnqueueNDRangeKernel(commandQueue[gpu], work[1].kernel[gpu], 1, 0, (unsigned int*)&rgbn, 0, 0, 0, 0);//solid
		}
	}
	clFinish(commandQueue[gpu]);
	clEnqueueReadBuffer(commandQueue[gpu], clBuffer[5], 1, 0, rgbn*sizeof(int), rgb, 0, 0, 0);
			
	QueryPerformanceFrequency(&li);freq=li.QuadPart;
	QueryPerformanceCounter(&li);
	linelen=sprintf_s(line, "%s OpenCL, fps=%.02f, T=%fms, dcam=%.02f, fov=%.02f", gpu?"GPU":"CPU", freq/double(li.QuadPart-nticks), 1000.*(li.QuadPart-nticks)/freq, dcam, fov*720/TWOPI);
	nticks=li.QuadPart;
	if(linelen>0)TextOutA(hMemDC, 0, R.bottom-16, line, linelen);
	BitBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
}
void				NonlinearCL::pushTexture(int *texture_)
{
	++ntx;
	txi=(int*)realloc(txi, ntx*sizeof(int));
	txi[ntx-1]=txsize;
	txsize+=txh[ntx-1]*txw[ntx-1];
	texture=(int*)realloc(texture, txsize*sizeof(int));
	memcpy(&texture[txi[ntx-1]], texture_, txh[ntx-1]*txw[ntx-1]*sizeof(int));
	free(texture_);
}
void				NonlinearCL::enqueueTextures()
{
	clBuffer[6]=clCreateBuffer(context[0], CL_MEM_READ_ONLY, (txi[ntx-1]+txh[ntx-1]*txw[ntx-1])*sizeof(int), 0, &err);
	err=clSetKernelArg(work[0].kernel[0], 6, sizeof(_cl_mem*), &clBuffer[6]);//textured
	err=clEnqueueWriteBuffer(commandQueue[0], clBuffer[6], 1, 0, (txi[ntx-1]+txh[ntx-1]*txw[ntx-1])*sizeof(int), texture, 0, 0, 0);

	clBuffer[7]=clCreateBuffer(context[1], CL_MEM_READ_ONLY, (txi[ntx-1]+txh[ntx-1]*txw[ntx-1])*sizeof(int), 0, &err);
	err=clSetKernelArg(work[0].kernel[1], 6, sizeof(_cl_mem*), &clBuffer[7]);//textured
	err=clEnqueueWriteBuffer(commandQueue[1], clBuffer[7], 1, 0, (txi[ntx-1]+txh[ntx-1]*txw[ntx-1])*sizeof(int), texture, 0, 0, 0);
}
int*				NonlinearCL::popTexture()
{
	int *texture_=(int*)malloc(txh[ntx-1]*txw[ntx-1]*sizeof(int));
	memcpy(texture_, &texture[txi[ntx-1]], txh[ntx-1]*txw[ntx-1]*sizeof(int));
	ntx--, txsize-=txh[ntx-1]*txw[ntx-1];
	return texture_;
}
void				NonlinearCL::clearTextures()
{
	clReleaseMemObject(clBuffer[6]);
	clReleaseMemObject(clBuffer[7]);
	ntx=0, txsize=0;
}
void				NonlinearCL::finish()
{
	for(int k=0;k<8;++k)clReleaseMemObject(clBuffer[k]);
	gpu=0; for(int k=0;k<6;++k)err=clReleaseKernel(work[k].kernel[gpu]);
	gpu=1; for(int k=0;k<6;++k)err=clReleaseKernel(work[k].kernel[gpu]);
	DeleteObject(SelectObject(hMemDC, hBitmap)), DeleteDC(hMemDC);
	free(texture), free(txi);
}
struct			ParallelSW:private Mode
{
	int **texture;
	int *libuffer, *lfbuffer;
	double *wbuffer;
	int *rgb, rgbn;
	HDC__ *hMemDC;
	HBITMAP__ *hBitmap;
	void initiate();
	void resize();
	void changeFov();
	void render();
	void draft_start		(double *v1, double *v2);
	void draft				(double *v1, double *v2);
	void pushTexture(int *texture_);
	void enqueueTextures();
	int* popTexture();
	void clearTextures();
	void finish();
} psw;
void				ParallelSW::initiate()
{
	texture=0;
	rgbn=h*w;
	wbuffer=(double*)malloc(rgbn*sizeof(double)), libuffer=(int*)malloc(h*sizeof(int)), lfbuffer=(int*)malloc(h*sizeof(int));
	hMemDC=CreateCompatibleDC(hDC);
	tagBITMAPINFO bmpInfo={{sizeof(tagBITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
	hBitmap=(HBITMAP__*)SelectObject(hMemDC, CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&rgb, 0, 0));
	SetBkMode(hMemDC, TRANSPARENT);
}
void				ParallelSW::resize()
{
	rgbn=h*w;
	wbuffer=(double*)realloc(wbuffer, rgbn*sizeof(double)), libuffer=(int*)realloc(libuffer, h*sizeof(int)), lfbuffer=(int*)realloc(lfbuffer, h*sizeof(int));
	DeleteObject((HBITMAP__*)SelectObject(hMemDC, hBitmap));
	tagBITMAPINFO bmpInfo={{sizeof(tagBITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
	hBitmap=(HBITMAP__*)SelectObject(hMemDC, CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&rgb, 0, 0));
	SetBkMode(hMemDC, TRANSPARENT);
}
void				ParallelSW::changeFov()
{
}
void				ParallelSW::render()
{
	memset(rgb, 0xFF, rgbn*sizeof(int));
	for(int k=0;k<rgbn;++k)
		wbuffer[k]=_HUGE;
	double dx, dy, dz, cpt, v1[5], v2[5], v3[5], admittance;
	for(int k=0;k<npols;++k)
	{
		dx=env[k].x1-camx, dy=env[k].y1-camy, dz=env[k].z1-camz, cpt=dx*cax+dy*sax, v1[2]=dx*sax-dy*cax, v1[3]=cpt*say-dz*cay, v1[4]=cpt*cay+dz*say, cpt=X0/(1000*tanfov), v1[0]=X0+v1[2]*cpt, v1[1]=Y0+v1[3]*cpt;
		dx=env[k].x2-camx, dy=env[k].y2-camy, dz=env[k].z2-camz, cpt=dx*cax+dy*sax, v2[2]=dx*sax-dy*cax, v2[3]=cpt*say-dz*cay, v2[4]=cpt*cay+dz*say, cpt=X0/(1000*tanfov), v2[0]=X0+v2[2]*cpt, v2[1]=Y0+v2[3]*cpt;
		dx=env[k].x3-camx, dy=env[k].y3-camy, dz=env[k].z3-camz, cpt=dx*cax+dy*sax, v3[2]=dx*sax-dy*cax, v3[3]=cpt*say-dz*cay, v3[4]=cpt*cay+dz*say, cpt=X0/(1000*tanfov), v3[0]=X0+v3[2]*cpt, v3[1]=Y0+v3[3]*cpt;
		if(v1[4]<0&&v2[4]<0&&v3[4]<0)		continue;
		for(int k2=0;k2<h;++k2)libuffer[k2]=w; memset(lfbuffer, 0, h*sizeof(int));
		draft_start(v1, v2), draft(v2, v3), draft(v3, v1);
		double au12=v2[2]-v1[2],			bu12=v2[3]-v1[3],			cu12=v2[4]-v1[4];									//u12	=<12>
		double aux3=v3[2]-v1[2],			bux3=v3[3]-v1[3],			cux3=v3[4]-v1[4];									//ux3	=<13>
		double a=bu12*cux3-bux3*cu12,		b=aux3*cu12-au12*cux3,		c=au12*bux3-aux3*bu12;								//abc	=<n>	=<12>x<13>
		cpt=1000*tanfov;
		double A1=-cpt*a/c/X0, A2=-cpt*b/c/X0, A3=a/c*(cpt+v1[2])+b/c*(cpt*Y0/X0+v1[3])+v1[4];
		if(env[k].tx>=0)
		{
			double	ux3_1=sqrt(au12*au12+bu12*bu12+cu12*cu12);	au12/=ux3_1,		bu12/=ux3_1,		cu12/=ux3_1;		//u12	=<u12>	=<12>/|12|
					ux3_1=aux3*au12+bux3*bu12+cux3*cu12,		aux3-=ux3_1*au12,	bux3-=ux3_1*bu12,	cux3-=ux3_1*cu12;	//ux3	=<x3>	=<13>-<u12>.<13><u12>
					ux3_1=sqrt(aux3*aux3+bux3*bux3+cux3*cux3),	aux3/=ux3_1,		bux3/=ux3_1,		cux3/=ux3_1;		//ux3	=<ux3>	=<x3>/|x3|
					ux3_1=aux3*v1[2]+bux3*v1[3]+cux3*v1[4];																	//ux3_1			=<ux3>.<1>
			double	u12_1=au12*v1[2]+bu12*v1[3]+cu12*v1[4];																	//u12_1			=<u12>.<1>
			double	C1=aux3*env[k].txXX+au12*env[k].txXY, C2=bux3*env[k].txXX+bu12*env[k].txXY, C3=cux3*env[k].txXX+cu12*env[k].txXY;
			double	C4=aux3*env[k].txYX+au12*env[k].txYY, C5=bux3*env[k].txYX+bu12*env[k].txYY, C6=cux3*env[k].txYX+cu12*env[k].txYY;
			double	D1=C1*cpt/X0+C3*A1, D2=C2*cpt/X0+C3*A2, D3=-C1*cpt-C2*cpt*Y0/X0+C3*A3+env[k].tx1x-ux3_1*env[k].txXX-u12_1*env[k].txXY;
			double	D4=C4*cpt/X0+C6*A1, D5=C5*cpt/X0+C6*A2, D6=-C4*cpt-C5*cpt*Y0/X0+C6*A3+env[k].tx1y-ux3_1*env[k].txYX-u12_1*env[k].txYY;
			double *wbk=wbuffer;
			int libk, lfbk, *rgbk=rgb, *txk=texture[env[k].tx], txhk=txh[env[k].tx], txwk=txw[env[k].tx];
			for(int k2=0;k2<h;++k2)
			{
				libk=libuffer[k2]<0?0:libuffer[k2], lfbk=lfbuffer[k2]>w?w:lfbuffer[k2];
				admittance=A1*libk+A2*k2+A3, a=D1*libk+D2*k2+D3, b=D4*libk+D5*k2+D6;
				for(int k3=libk;k3<lfbk;++k3)
				{
					if(admittance>0&&admittance<wbk[k3])
					{
						wbk[k3]=admittance;
						int Xtx=int(a)%txwk;if(Xtx<0)Xtx+=txwk;
						int Ytx=int(b)%txhk;if(Ytx<0)Ytx+=txhk;
						rgbk[k3]=txk[Xtx+Ytx*txwk];
					}
					admittance+=A1, a+=D1, b+=D4;
				}
				wbk=wbk+w, rgbk=rgbk+w;
			}
		}
		else
		{
			double *wbk=wbuffer;
			int libk, lfbk, *rgbk=rgb, color=env[k].color;
			for(int k2=0;k2<h;++k2)
			{
				libk=libuffer[k2]<0?0:libuffer[k2], lfbk=lfbuffer[k2]>w?w:lfbuffer[k2];
				admittance=A1*libk+A2*k2+A3;
				for(int k3=libk;k3<lfbk;++k3)
				{
					if(admittance>0&&admittance<wbk[k3])
						wbk[k3]=admittance, rgbk[k3]=color;
					admittance+=A1;
				}
				wbk=wbk+w, rgbk=rgbk+w;
			}
		}
	}
	for(int k=0;k<ntpols;++k)
	{
		dx=tenv[k].x1-camx, dy=tenv[k].y1-camy, dz=tenv[k].z1-camz, cpt=dx*cax+dy*sax, v1[2]=dx*sax-dy*cax, v1[3]=cpt*say-dz*cay, v1[4]=cpt*cay+dz*say, cpt=X0/(1000*tanfov), v1[0]=X0+v1[2]*cpt, v1[1]=Y0+v1[3]*cpt;
		dx=tenv[k].x2-camx, dy=tenv[k].y2-camy, dz=tenv[k].z2-camz, cpt=dx*cax+dy*sax, v2[2]=dx*sax-dy*cax, v2[3]=cpt*say-dz*cay, v2[4]=cpt*cay+dz*say, cpt=X0/(1000*tanfov), v2[0]=X0+v2[2]*cpt, v2[1]=Y0+v2[3]*cpt;
		dx=tenv[k].x3-camx, dy=tenv[k].y3-camy, dz=tenv[k].z3-camz, cpt=dx*cax+dy*sax, v3[2]=dx*sax-dy*cax, v3[3]=cpt*say-dz*cay, v3[4]=cpt*cay+dz*say, cpt=X0/(1000*tanfov), v3[0]=X0+v3[2]*cpt, v3[1]=Y0+v3[3]*cpt;
		if(v1[4]<0&&v2[4]<0&&v3[4]<0)		continue;
		for(int k2=0;k2<h;++k2)libuffer[k2]=w; memset(lfbuffer, 0, h*sizeof(int));
		draft_start(v1, v2), draft(v2, v3), draft(v3, v1);
		double au12=v2[2]-v1[2],			bu12=v2[3]-v1[3],			cu12=v2[4]-v1[4];									//u12	=<12>
		double aux3=v3[2]-v1[2],			bux3=v3[3]-v1[3],			cux3=v3[4]-v1[4];									//ux3	=<13>
		double a=bu12*cux3-bux3*cu12,		b=aux3*cu12-au12*cux3,		c=au12*bux3-aux3*bu12;								//abc	=<n>	=<12>x<13>
		cpt=1000*tanfov;
		double A1=-cpt*a/c/X0, A2=-cpt*b/c/X0, A3=a/c*(cpt+v1[2])+b/c*(cpt*Y0/X0+v1[3])+v1[4];
		if(tenv[k].tx>=0)
		{
			double	ux3_1=sqrt(au12*au12+bu12*bu12+cu12*cu12);	au12/=ux3_1,		bu12/=ux3_1,		cu12/=ux3_1;		//u12	=<u12>	=<12>/|12|
					ux3_1=aux3*au12+bux3*bu12+cux3*cu12,		aux3-=ux3_1*au12,	bux3-=ux3_1*bu12,	cux3-=ux3_1*cu12;	//ux3	=<x3>	=<13>-<u12>.<13><u12>
					ux3_1=sqrt(aux3*aux3+bux3*bux3+cux3*cux3),	aux3/=ux3_1,		bux3/=ux3_1,		cux3/=ux3_1;		//ux3	=<ux3>	=<x3>/|x3|
					ux3_1=aux3*v1[2]+bux3*v1[3]+cux3*v1[4];																	//ux3_1			=<ux3>.<1>
			double	u12_1=au12*v1[2]+bu12*v1[3]+cu12*v1[4];																	//u12_1			=<u12>.<1>
			double	C1=aux3*tenv[k].txXX+au12*tenv[k].txXY, C2=bux3*tenv[k].txXX+bu12*tenv[k].txXY, C3=cux3*tenv[k].txXX+cu12*tenv[k].txXY;
			double	C4=aux3*tenv[k].txYX+au12*tenv[k].txYY, C5=bux3*tenv[k].txYX+bu12*tenv[k].txYY, C6=cux3*tenv[k].txYX+cu12*tenv[k].txYY;
			double	D1=C1*cpt/X0+C3*A1, D2=C2*cpt/X0+C3*A2, D3=-C1*cpt-C2*cpt*Y0/X0+C3*A3+tenv[k].tx1x-ux3_1*tenv[k].txXX-u12_1*tenv[k].txXY;
			double	D4=C4*cpt/X0+C6*A1, D5=C5*cpt/X0+C6*A2, D6=-C4*cpt-C5*cpt*Y0/X0+C6*A3+tenv[k].tx1y-ux3_1*tenv[k].txYX-u12_1*tenv[k].txYY;
			double *wbk=wbuffer;
			int libk, lfbk, *rgbk=rgb, *txk=texture[tenv[k].tx], txhk=txh[tenv[k].tx], txwk=txw[tenv[k].tx];
			for(int k2=0;k2<h;++k2)
			{
				libk=libuffer[k2]<0?0:libuffer[k2], lfbk=lfbuffer[k2]>w?w:lfbuffer[k2];
				admittance=A1*libk+A2*k2+A3, a=D1*libk+D2*k2+D3, b=D4*libk+D5*k2+D6;
				for(int k3=libk;k3<lfbk;++k3)
				{
					if(admittance>0&&admittance<wbk[k3])
					{
						wbk[k3]=admittance;
						int Xtx=int(a)%txwk;if(Xtx<0)Xtx+=txwk;
						int Ytx=int(b)%txhk;if(Ytx<0)Ytx+=txhk;
						rgbk[k3]=txk[Xtx+Ytx*txwk];
					}
					admittance+=A1, a+=D1, b+=D4;
				}
				wbk=wbk+w, rgbk=rgbk+w;
			}
		}
		else
		{
			double *wbk=wbuffer;
			int libk, lfbk, *rgbk=rgb, color=tenv[k].color;
			for(int k2=0;k2<h;++k2)
			{
				libk=libuffer[k2]<0?0:libuffer[k2], lfbk=lfbuffer[k2]>w?w:lfbuffer[k2];
				admittance=A1*libk+A2*k2+A3;
				for(int k3=libk;k3<lfbk;++k3)
				{
					if(admittance>0&&admittance<wbk[k3])
						wbk[k3]=admittance, rgbk[k3]=color;
					admittance+=A1;
				}
				wbk=wbk+w, rgbk=rgbk+w;
			}
		}
	}
	QueryPerformanceFrequency(&li);freq=li.QuadPart;
	QueryPerformanceCounter(&li);
	linelen=sprintf_s(line, 128, "fps=%.10f, T=%.10fms, dcam=%.10f, fov=%.10f", freq/double(li.QuadPart-nticks), 1000.*(li.QuadPart-nticks)/freq, dcam, atan(tanfov)*720/TWOPI);
	nticks=li.QuadPart;
	if(linelen>0)TextOutA(hMemDC, 0, h-16, line, linelen);
	BitBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
}
void				ParallelSW::draft_start		(double *v1, double *v2)
{
	int k2;double k3, A=(v2[0]-v1[0])/(v2[1]-v1[1]);
		 if(v1[1]<v2[1]){	k3=A*((v1[1]<0?0:v1[1])-v1[1])+v1[0];		 if(v1[0]<v2[0])for(long long k=long long(v1[1])<0?	0:long long(v1[1])	;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
																	else				for(long long k=long long(v1[1])<0?	0:long long(v1[1])	;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
	else				{	k3=A*((v2[1]<0?0:v2[1])-v1[1])+v1[0];		 if(v1[0]<v2[0])for(long long k=long long(v2[1])<0?	0:long long(v2[1])	;k<h&&k<v1[1]	;++k)	k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;
																	else				for(long long k=long long(v2[1])<0?	0:long long(v2[1])	;k<h&&k<v1[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=lfbuffer[k]=k2, k3+=A;}
}
void				ParallelSW::draft			(double *v1, double *v2)
{
	int k2;double k3, A=(v2[0]-v1[0])/(v2[1]-v1[1]);
		 if(v1[1]<v2[1]){	k3=A*((v1[1]<0?0:v1[1])-v1[1])+v1[0];		 if(v1[0]<v2[0])for(long long k=long long(v1[1])<0?	0:long long(v1[1])	;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
																	else				for(long long k=long long(v1[1])<0?	0:long long(v1[1])	;k<h&&k<v2[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;}
	else				{	k3=A*((v2[1]<0?0:v2[1])-v1[1])+v1[0];		 if(v1[0]<v2[0])for(long long k=long long(v2[1])<0?	0:long long(v2[1])	;k<h&&k<v1[1]	;++k)	k2=int(k3), k2=k2<0?v1[0]>0?int(v1[0]):0:k2>w?v2[0]<w?int(v2[0]):w:k2<v1[0]?int(v1[0]):k2>v2[0]?int(v2[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;
																	else				for(long long k=long long(v2[1])<0?	0:long long(v2[1])	;k<h&&k<v1[1]	;++k)	k2=int(k3), k2=k2<0?v2[0]>0?int(v2[0]):0:k2>w?v1[0]<w?int(v1[0]):w:k2<v2[0]?int(v2[0]):k2>v1[0]?int(v1[0]):k2, libuffer[k]=k2>=libuffer[k]?libuffer[k]:k2, lfbuffer[k]=k2<=lfbuffer[k]?lfbuffer[k]:k2, k3+=A;}
}
void				ParallelSW::pushTexture(int *texture_)
{
	++ntx;
	texture=(int**)realloc(texture, ntx*sizeof(int*)), texture[ntx-1]=texture_;
}
void				ParallelSW::enqueueTextures()
{
}
int*				ParallelSW::popTexture()
{
	ntx--;
	return texture[ntx];
}
void				ParallelSW::clearTextures()
{
	for(int k=0;k<ntx;++k)
		free(texture[k]);
	ntx=0;
}
void				ParallelSW::finish()
{
	DeleteObject(SelectObject(hMemDC, hBitmap)), DeleteDC(hMemDC);
	for(int k=0;k<ntx;++k)free(texture[k]);
	free(texture), free(libuffer), free(lfbuffer), free(wbuffer);
}
std::wstring	wider(char *a)
{
	auto len=strlen(a);
	auto buf=new wchar_t[len+1];
	swprintf_s(buf, len+1, L"%S", a);
	std::wstring str=buf;
	delete[] buf;
	return str;
}
std::string		narrower(std::wstring str)
{
	auto len=str.size();
	auto buf=new char[len+1];
	sprintf_s(buf, len+1, "%S", str.c_str());
	std::string str2=buf;
	delete[] buf;
	return str2;
}
bool			loadText(std::string &str)
{
	struct stat info;
	if(!stat(str.c_str(), &info))
	{
		std::ifstream file(str.c_str(), std::ios::in);
		if(file.is_open())
		{
			char *a=(char*)malloc((info.st_size+1)*sizeof(char));
			file.read(a, info.st_size), a[info.st_size]='\0';
			file.close();
			str=a;
			return true;
		}
	}
	str="";
	return false;
}
int				comtok(std::string &str, int *pI, int *pF)
{
	for(*pI=*pF;*pI==*pF;)
	{
		if(*pF<long long(str.size()))
		{
			if(str[*pF]=='/')
			{
				if(*pF+1<long long(str.size())&&str[*pF+1]=='*')
				{
					for(int k=*pF+2;;++k)
					{
						if(k==long long(str.size()))
						{
							*pI=k;
							break;
						}
						if(str[k]=='*'&&k+1<long long(str.size())&&str[k+1]=='/')
						{
							*pI=k+2;
							break;
						}
					}
				}
				else
				{
					for(int k=*pF+1;;++k)
					{
						if(k==long long(str.size()))
						{
							*pI=k;
							break;
						}
						if(str[k]=='\r'&&k+1<long long(str.size())&&str[k+1]=='\n')
						{
							*pI=k+2;
							break;
						}
						else if(str[k]=='\r'||str[k]=='\n')
						{
							*pI=k+1;
							break;
						}
					}
				}
			}
			else if(str[*pF]=='\r'&&*pF+1<long long(str.size())&&str[*pF+1]=='\n')
			{
				*pI=*pF+2;
			}
			else if(str[*pF]=='\r'||str[*pF]=='\n')
			{
				*pI=*pF+1;
			}
		}
		else
		{
			*pI=*pF;
		}
		if(*pI==long long(str.size()))
			return 0;
		for(int k=*pI;;++k)
		{
			if(k==long long(str.size())||str[k]=='\r'||str[k]=='\n'||str[k]=='/')
			{
				*pF=k;
				break;
			}
		}
	}
	return 1;
}
double			readDoubleFromBuffer(std::string &str, int i, int f, int *pF=0, int *pPrev=0)
{
	if(pF)*pF=-1;
	if(pPrev)*pPrev=0;
	int n=1;
	double value=0;
	if(str.size())
	{
		for(int k=i;k<f;++k)
		{
			switch(str[k])
			{
			case '-':
				n*=-1;
				continue;
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':case '.':
				{
					int k2=k+1, endValid=0;
					for(;k2<f;++k2)
						if((str[k2]<'0'||str[k2]>'9')&&str[k2]!='.')
						{
							endValid=1;
							break;
						}
					if(!endValid)
						k2=f;
					if(pF)*pF=k2==f?-1:k2;
					for(int k3=k;k3<k2;++k3)
					{
						if(str[k3]>='0'&&str[k3]<='9')
						{
							double p=1;
							for(int k4=k;k4<k2;++k4)
							{
								if(str[k4]=='.')
								{
									for(int k5=k4+1;k5<k2;++k5)
									{
										if(str[k5]=='.')continue;
										p/=10;
									}
									break;
								}
							}
							for(int k4=k2-1;k4>=k;k4--)
							{
								if(str[k4]=='.')continue;
								value+=(str[k4]-'0')*p, p*=10;
							}
							break;
						}
					}
				}
				break;
			case '`':
				if(pPrev)*pPrev=1;
				if(pF)*pF=k+1;
				break;
			default:
				continue;
			}
			break;
		}
	}
	return n*value;
}
void			transform(int k)
{
	double _x12=env[k].x2-env[k].x1, _y12=env[k].y2-env[k].y1, _z12=env[k].z2-env[k].z1;
	double _x13=env[k].x3-env[k].x1, _y13=env[k].y3-env[k].y1, _z13=env[k].z3-env[k].z1;
	double _r=(_x12*_x13+_y12*_y13+_z12*_z13)/(_x12*_x12+_y12*_y12+_z12*_z12);
	double _xx3=_x13-_r*_x12, _yx3=_y13-_r*_y12, _zx3=_z13-_r*_z12;
	double _x3=sqrt(_xx3*_xx3+_yx3*_yx3+_zx3*_zx3), _12=sqrt(_x12*_x12+_y12*_y12+_z12*_z12);
	env[k].txXX=(env[k].tx3x-env[k].tx1x-_r*(env[k].tx2x-env[k].tx1x))/_x3, env[k].txXY=(env[k].tx2x-env[k].tx1x)/_12;
	env[k].txYX=(env[k].tx3y-env[k].tx1y-_r*(env[k].tx2y-env[k].tx1y))/_x3, env[k].txYY=(env[k].tx2y-env[k].tx1y)/_12;
}
void			transform_transparent(int k)
{
	double _x12=tenv[k].x2-tenv[k].x1, _y12=tenv[k].y2-tenv[k].y1, _z12=tenv[k].z2-tenv[k].z1;
	double _x13=tenv[k].x3-tenv[k].x1, _y13=tenv[k].y3-tenv[k].y1, _z13=tenv[k].z3-tenv[k].z1;
	double _r=(_x12*_x13+_y12*_y13+_z12*_z13)/(_x12*_x12+_y12*_y12+_z12*_z12);
	double _xx3=_x13-_r*_x12, _yx3=_y13-_r*_y12, _zx3=_z13-_r*_z12;
	double _x3=sqrt(_xx3*_xx3+_yx3*_yx3+_zx3*_zx3), _12=sqrt(_x12*_x12+_y12*_y12+_z12*_z12);
	tenv[k].txXX=(tenv[k].tx3x-tenv[k].tx1x-_r*(tenv[k].tx2x-tenv[k].tx1x))/_x3, tenv[k].txXY=(tenv[k].tx2x-tenv[k].tx1x)/_12;
	tenv[k].txYX=(tenv[k].tx3y-tenv[k].tx1y-_r*(tenv[k].tx2y-tenv[k].tx1y))/_x3, tenv[k].txYY=(tenv[k].tx2y-tenv[k].tx1y)/_12;
}
void			antitransform(int k)
{
	auto &p=env[k];
	double _x12=p.x2-p.x1, _y12=p.y2-p.y1, _z12=p.z2-p.z1;
	double _x13=p.x3-p.x1, _y13=p.y3-p.y1, _z13=p.z3-p.z1;
	double _r=(_x12*_x13+_y12*_y13+_z12*_z13)/(_x12*_x12+_y12*_y12+_z12*_z12);
	double _xx3=_x13-_r*_x12, _yx3=_y13-_r*_y12, _zx3=_z13-_r*_z12;
	double _x3=sqrt(_xx3*_xx3+_yx3*_yx3+_zx3*_zx3), _12=sqrt(_x12*_x12+_y12*_y12+_z12*_z12);

	double	p2x=0,		p2y=_12,
			p3x=_x3,	p3y=(_x12*_x13+_y12*_y13+_z12*_z13)/_12;
	p.tx2x=p.txXX*p2x+p.txXY*p2y, p.tx2y=p.txYX*p2x+p.txYY*p2y;
	p.tx3x=p.txXX*p3x+p.txXY*p3y, p.tx3y=p.txYX*p3x+p.txYY*p3y;
//	p.tx1x=		p.tx1y=0;
//	p.tx2x=0,	p.tx2y=_12;
//	p.tx3x=_x3,	p.tx3y=(_x12*_x13+_y12*_y13+_z12*_z13)/_12;
}
void			antitransform_transparent(int k)
{
	auto &p=tenv[k];
	double _x12=p.x2-p.x1, _y12=p.y2-p.y1, _z12=p.z2-p.z1;
	double _x13=p.x3-p.x1, _y13=p.y3-p.y1, _z13=p.z3-p.z1;
	double _r=(_x12*_x13+_y12*_y13+_z12*_z13)/(_x12*_x12+_y12*_y12+_z12*_z12);
	double _xx3=_x13-_r*_x12, _yx3=_y13-_r*_y12, _zx3=_z13-_r*_z12;
	double _x3=sqrt(_xx3*_xx3+_yx3*_yx3+_zx3*_zx3), _12=sqrt(_x12*_x12+_y12*_y12+_z12*_z12);

	double	p2x=0,		p2y=_12,
			p3x=_x3,	p3y=(_x12*_x13+_y12*_y13+_z12*_z13)/_12;
	p.tx2x=p.txXX*p2x+p.txXY*p2y, p.tx2y=p.txYX*p2x+p.txYY*p2y;
	p.tx3x=p.txXX*p3x+p.txXY*p3y, p.tx3y=p.txYX*p3x+p.txYY*p3y;
//	p.tx1x=		p.tx1y=0;
//	p.tx2x=0,	p.tx2y=_12;
//	p.tx3x=_x3,	p.tx3y=(_x12*_x13+_y12*_y13+_z12*_z13)/_12;
}
void			switchMode(Mode *m_)
{
	int ntx_=0, **texture_=0;
	for(;ntx>0;)
	{
		++ntx_;
		texture_=(int**)realloc(texture_, ntx_*sizeof(int*)), texture_[ntx_-1]=m->popTexture();
	}
	m->finish();
	m=m_;
	m->initiate();
	for(int k=ntx_-1;k>=0;k--)
		m->pushTexture(texture_[k]);
	if(ntx_>0)m->enqueueTextures();
}
long			__stdcall WndProc(HWND__ *hWnd, unsigned int message, unsigned int wParam, long lParam)
{
	switch(message)
	{
	case WM_PAINT:
		GetClientRect(hWnd, &R);
		if(h!=R.bottom-R.top||w!=R.right-R.left)
		{
			h=R.bottom-R.top, w=R.right-R.left, centerP.x=X0=w/2, centerP.y=Y0=h/2;
			ClientToScreen(hWnd, &centerP);
			m->resize();
		}
		if(!timer)render();
		break;
	case WM_EXITSIZEMOVE:
		centerP.x=X0, centerP.y=Y0;ClientToScreen(hWnd, &centerP);
		return 0;
	case WM_ACTIVATE:
		if(wParam==WA_INACTIVE)
		{
			memset(kb, 0, 256*sizeof(char));
			kp=0;
			if(timer){KillTimer(hWnd, 0);timer=0;}
		}
		return 0;
	case WM_TIMER:
	//	glParam=lParam, gwParam=wParam;
		render();
		if(!kp){KillTimer(hWnd, 0);timer=0;}
		return 0;
	case WM_LBUTTONDOWN:
		drag=!drag;
		ShowCursor(!drag);
		if(drag)
		{
			mouseP0.x=short(lParam), mouseP0.y=short(lParam>>16);
			ClientToScreen(hWnd, &mouseP0);
			SetCursorPos(centerP.x, centerP.y);
		}
		else
			SetCursorPos(mouseP0.x, mouseP0.y);
		return 0;
	case WM_MOUSEMOVE:
		if(drag)
		{
			if(!d_bypass)
			{
				ax+=da_tfov*MOUSE_SENSITIVITY*(X0-short(lParam		)), cax=cos(ax), sax=sin(ax);
				ay+=da_tfov*MOUSE_SENSITIVITY*(Y0-short(lParam>>16	)), cay=cos(ay), say=sin(ay), d_bypass=1;
				for(;ax<0;)ax+=TWOPI; for(;ax>TWOPI;)ax-=TWOPI; for(;ay<0;)ay+=TWOPI; for(;ay>TWOPI;)ay-=TWOPI;
				SetCursorPos(centerP.x, centerP.y);
				if(!timer)render();
			}
			else
				d_bypass=0;
		}
#ifdef AA_MOUSEMOVE_TRIGGER
		else if(!timer)render();//
#endif
		return 0;
	case WM_RBUTTONDOWN:
		kb[VK_RBUTTON]=1, ++kp;
		if(!timer){SetTimer(hWnd, 0, TIMER_ELAPSE, 0);timer=1;}
		return 0;
	case WM_RBUTTONUP:
		kb[VK_RBUTTON]=0, kp--;
		return 0;
	case WM_MOUSEWHEEL:
		if(kb[VK_CONTROL])
		{
				 if(short(wParam>>16)>0)	dcam*=2;
			else							dcam/=2;
		}
		else
		{
				 if(short(wParam>>16)>0)	tanfov/=DTANFOV, fov/=DFOV;
			else							tanfov*=DTANFOV, fov*=DFOV;
			da_tfov=tanfov>1?1:tanfov;
			m->changeFov();
		}
		if(!timer)
			render();
		return 0;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 'W':case 'A':case 'S':case 'D':case 'T':case 'G':case VK_UP:case VK_DOWN:case VK_LEFT:case VK_RIGHT:case VK_ADD:case VK_SUBTRACT:case 'Q':
			if(!kb[wParam])kb[wParam]=1, ++kp;
			if(!timer){SetTimer(hWnd, 0, TIMER_ELAPSE, 0);timer=1;}
			return 0;
		case VK_F1://planar
			if(mode!=1)
			{
				mode=1, switchMode((Mode*)&lsw);
				kb[wParam]=1;
				if(!timer)render();
			}
			return 0;
		case VK_F2://cylinder panorama
			if(mode!=2&&mode!=7&&mode!=11)
			{
				if(m==(Mode*)&nlsw)
				{
					if(nlsw.rebuffer!=&NonlinearSW::rebuffer1)
					{
						mode=2;
						nlsw.rebuffer=&NonlinearSW::rebuffer1;
						(nlsw.*(nlsw.rebuffer))();
					}
				}
				else if(m==(Mode*)&nlcl)
				{
					if(nlcl.rebuffer!=0)
					{
						mode=nlcl.gpu?11:7;
						nlcl.rebuffer=0;
						nlcl.changeFov();
					}
				}
				else
				{
					mode=2;
					switchMode((Mode*)&nlsw);
					nlsw.rebuffer=&NonlinearSW::rebuffer1;
					(nlsw.*(nlsw.rebuffer))();
				}
				kb[wParam]=1;
				if(!timer)render();
			}
			return 0;
		case VK_F3://vertical cylinder panorama
			if(mode!=3&&mode!=8&&mode!=12)
			{
				if(m==(Mode*)&nlsw)
				{
					if(nlsw.rebuffer!=&NonlinearSW::rebuffer2)
					{
						mode=3;
						nlsw.rebuffer=&NonlinearSW::rebuffer2;
						(nlsw.*(nlsw.rebuffer))();
					}
				}
				else if(m==(Mode*)&nlcl)
				{
					if(nlcl.rebuffer!=1)
					{
						mode=nlcl.gpu?12:8;
						nlcl.rebuffer=1;
						nlcl.changeFov();
					}
				}
				else
				{
					mode=3;
					switchMode((Mode*)&nlsw);
					nlsw.rebuffer=&NonlinearSW::rebuffer2;
					(nlsw.*(nlsw.rebuffer))();
				}
				kb[wParam]=1;
				if(!timer)render();
			}
			return 0;
		case VK_F4://spherical
			if(mode!=4&&mode!=9&&mode!=13)
			{
				if(m==(Mode*)&nlsw)
				{
					if(nlsw.rebuffer!=&NonlinearSW::rebuffer3)
					{
						mode=4;
						nlsw.rebuffer=&NonlinearSW::rebuffer3;
						(nlsw.*(nlsw.rebuffer))();
					}
				}
				else if(m==(Mode*)&nlcl)
				{
					if(nlcl.rebuffer!=2)
					{
						mode=nlcl.gpu?13:9;
						nlcl.rebuffer=2;
						nlcl.changeFov();
					}
				}
				else
				{
					mode=4;
					switchMode((Mode*)&nlsw);
					nlsw.rebuffer=&NonlinearSW::rebuffer3;
					(nlsw.*(nlsw.rebuffer))();
				}
				kb[wParam]=1;
				if(!timer)render();
			}
			return 0;
		case VK_F5://fuzzy
			if(mode!=5&&mode!=10&&mode!=14)
			{
				if(m==(Mode*)&nlsw)
				{
					if(nlsw.rebuffer!=&NonlinearSW::rebuffer4)
					{
						mode=5;
						nlsw.rebuffer=&NonlinearSW::rebuffer4;
						(nlsw.*(nlsw.rebuffer))();
					}
				}
				else if(m==(Mode*)&nlcl)
				{
					if(nlcl.rebuffer!=3)
					{
						mode=nlcl.gpu?14:10;
						nlcl.rebuffer=3;
						nlcl.changeFov();
					}
				}
				else
				{
					mode=5;
					switchMode((Mode*)&nlsw);
					nlsw.rebuffer=&NonlinearSW::rebuffer4;
					(nlsw.*(nlsw.rebuffer))();
				}
				kb[wParam]=1;
				if(!timer)render();
			}
			return 0;
		case VK_F6:
			if(mode!=15)
			{
				mode=15, switchMode((Mode*)&psw);
				kb[wParam]=1;
				if(!timer)render();
			}
			return 0;
		case VK_TAB:
			switch(mode)
			{
			case 1:
				mode=6, switchMode((Mode*)&lgl);
				break;
			case 2:
				mode=7, switchMode((Mode*)&nlcl);
				nlcl.rebuffer=0;
				nlcl.changeFov();
				break;
			case 3:
				mode=8, switchMode((Mode*)&nlcl);
				nlcl.rebuffer=1;
				nlcl.changeFov();
				break;
			case 4:
				mode=9, switchMode((Mode*)&nlcl);
				nlcl.rebuffer=2;
				nlcl.changeFov();
				break;
			case 5:
				mode=10, switchMode((Mode*)&nlcl);
				nlcl.rebuffer=3;
				nlcl.changeFov();
				break;

			case 6:
				mode=1, switchMode((Mode*)&lsw);
				break;
			case 7:
				mode=11;
				nlcl.switchDevice(1);
				break;
			case 8:
				mode=12;
				nlcl.switchDevice(1);
				break;
			case 9:
				mode=13;
				nlcl.switchDevice(1);
				break;
			case 10:
				mode=14;
				nlcl.switchDevice(1);
				break;

			case 11:
				mode=2, switchMode((Mode*)&nlsw);
				nlsw.rebuffer=&NonlinearSW::rebuffer1;
				(nlsw.*(nlsw.rebuffer))();
				break;
			case 12:
				mode=3, switchMode((Mode*)&nlsw);
				nlsw.rebuffer=&NonlinearSW::rebuffer2;
				(nlsw.*(nlsw.rebuffer))();
				break;
			case 13:
				mode=4, switchMode((Mode*)&nlsw);
				nlsw.rebuffer=&NonlinearSW::rebuffer3;
				(nlsw.*(nlsw.rebuffer))();
				break;
			case 14:
				mode=5, switchMode((Mode*)&nlsw);
				nlsw.rebuffer=&NonlinearSW::rebuffer4;
				(nlsw.*(nlsw.rebuffer))();
				break;
			case 15:
				break;
			}
			if(!timer)render();
			return 0;
		case 'R':
			if(!kb[wParam])
			{
				if(kb[VK_CONTROL])
				{
					camx=0, camy=0, camz=-1.;
					ax=0, ay=-.25*TWOPI, cax=1., sax=0, cay=0, say=-1.;
					kp=0;
				//	memset(kb, 0, 256*sizeof(char));
				}
				else
				{
					camx=camx0, camy=camy0, camz=camz0, dcam=dcam0;
					ax=ax0, ay=ay0, cax=cos(ax), sax=sin(ax), cay=cos(ay), say=sin(ay), da=da0, tanfov=tanfov0, fov=fov0;
					da_tfov=tanfov;
				}
				kb[wParam]=1;
				if(!timer)render();
			}
			return 0;
		case 'Z':
			if(!kb[wParam])
			{
				ax+=TWOPI/2, ay+=TWOPI/2, cax=cos(ax), sax=sin(ax), cay=cos(ay), say=sin(ay);
				kb[wParam]=1;
				if(!timer)render();
			}
			return 0;
		case '0':
			{
				m->clearTextures();
				txh=(int*)realloc(txh, sizeof(int)), txh[ntx]=100;
				txw=(int*)realloc(txw, sizeof(int)), txw[ntx]=100;
				int *texture_=(int*)malloc(txh[ntx]*txw[ntx]*sizeof(int));
				for(int k=0;k<txh[ntx]*txw[ntx];++k)texture_[k]=unsigned char(rand())<<16|unsigned char(rand())<<8|unsigned char(rand());
				m->pushTexture(texture_);
				m->enqueueTextures();
				npols=nRandPols, env=(POL*)realloc(env, npols*sizeof(POL));
				for(int k=0;k<npols;++k)
				{
					env[k].x1=double(rand()%500), env[k].y1=double(rand()%500), env[k].z1=double(rand()%500);
					env[k].x2=double(rand()%500), env[k].y2=double(rand()%500), env[k].z2=double(rand()%500);
					env[k].x3=double(rand()%500), env[k].y3=double(rand()%500), env[k].z3=double(rand()%500);
					if(rand()%2)
					{
						double rx=double(rand())/10000, ry=double(rand())/10000, thx=double(rand())/1000, thy=double(rand())/1000;
						env[k].tx= 0, env[k].tx1x=double(rand()%500), env[k].tx1y=double(rand()%500), env[k].tx2x=double(rand()%500), env[k].tx2y=double(rand()%500), env[k].tx3x=double(rand()%500), env[k].tx3y=double(rand()%500);
						transform(k);
					}
					else
						env[k].tx=-1, env[k].color=unsigned char(rand())<<16|unsigned char(rand())<<8|unsigned char(rand());
				}
				ntpols=nRandTPols, tenv=(POL*)realloc(tenv, ntpols*sizeof(POL));
				for(int k=0;k<ntpols;++k)
				{
					tenv[k].x1=double(rand()%500), tenv[k].y1=double(rand()%500), tenv[k].z1=double(rand()%500);
					tenv[k].x2=double(rand()%500), tenv[k].y2=double(rand()%500), tenv[k].z2=double(rand()%500);
					tenv[k].x3=double(rand()%500), tenv[k].y3=double(rand()%500), tenv[k].z3=double(rand()%500);
					if(rand()%2)
					{
						double rx=double(rand())/10000, ry=double(rand())/10000, thx=double(rand())/1000, thy=double(rand())/1000;
						tenv[k].tx= 0, tenv[k].tx1x=double(rand()%500), tenv[k].tx1y=double(rand()%500), tenv[k].tx2x=double(rand()%500), tenv[k].tx2y=double(rand()%500), tenv[k].tx3x=double(rand()%500), tenv[k].tx3y=double(rand()%500);
						transform_transparent(k);
					}
					else
						tenv[k].tx=-1, tenv[k].color=unsigned char(rand())<<16|unsigned char(rand())<<8|unsigned char(rand());
				}
			}
			kb[wParam]=1;
			if(!timer)render();
			return 0;
		case '9':
			{
				npols=200, env=(POL*)realloc(env, npols*sizeof(POL));
				int k2, k3, k4=int(sqrt(double(npols)/2));
				for(int k=0;k<npols;k+=2)
				{
					k2=(k/2)%k4, k3=(k/2)/k4;
					env[k  ].x1=1000* k2   +double(rand()%100), env[k  ].y1=1000* k3   +double(rand()%100), env[k  ].z1=double(rand()%100);
					env[k  ].x2=1000*(k2+1)+double(rand()%100), env[k  ].y2=1000* k3   +double(rand()%100), env[k  ].z2=double(rand()%100);
					env[k  ].x3=1000*(k2+1)+double(rand()%100), env[k  ].y3=1000*(k3+1)+double(rand()%100), env[k  ].z3=double(rand()%100);
					env[k+1].x1=1000*(k2+1)+double(rand()%100), env[k+1].y1=1000*(k3+1)+double(rand()%100), env[k+1].z1=double(rand()%100);
					env[k+1].x2=1000* k2   +double(rand()%100), env[k+1].y2=1000*(k3+1)+double(rand()%100), env[k+1].z2=double(rand()%100);
					env[k+1].x3=1000* k2   +double(rand()%100), env[k+1].y3=1000* k3   +double(rand()%100), env[k+1].z3=double(rand()%100);
					if(rand()%3&&ntx)
					{
						env[k].tx=env[k+1].tx=rand()%ntx;
						double rx=double(rand())/1000, ry=double(rand())/1000, thx=double(rand())/1000, thy=double(rand())/1000;
						env[k  ].tx1x=double(rand()%500), env[k  ].tx1y=double(rand()%500), env[k  ].txXX=rx*cos(thx), env[k  ].txXY=ry*cos(thy), env[k  ].txYX=rx*sin(thx), env[k  ].txYY=ry*sin(thy);
						env[k+1].tx1x=double(rand()%500), env[k+1].tx1y=double(rand()%500), env[k+1].txXX=rx*cos(thx), env[k+1].txXY=ry*cos(thy), env[k+1].txYX=rx*sin(thx), env[k+1].txYY=ry*sin(thy);
					}
					else
						env[k].tx=env[k+1].tx=-1, env[k].color=env[k+1].color=unsigned char(rand())<<16|unsigned char(rand())<<8|unsigned char(rand());
				}
			}
			kb[wParam]=1;
			if(!timer)render();
			return 0;
		case '8':
			{
				npols=20, env=(POL*)realloc(env, npols*sizeof(POL));
				int k2, k3, k4=int(sqrt(double(npols)/2));
				for(int k=0;k<npols;k+=2)
				{
					k2=(k/2)%k4, k3=(k/2)/k4;
					env[k  ].x1=double(rand()%1000), env[k  ].y1=double(rand()%1000), env[k  ].z1=double(rand()%1000);
					env[k  ].x2=double(rand()%1000), env[k  ].y2=double(rand()%1000), env[k  ].z2=double(rand()%1000);
					env[k  ].x3=double(rand()%1000), env[k  ].y3=double(rand()%1000), env[k  ].z3=double(rand()%1000);
					env[k+1].x1=double(rand()%1000), env[k+1].y1=double(rand()%1000), env[k+1].z1=double(rand()%1000);
					env[k+1].x2=double(rand()%1000), env[k+1].y2=double(rand()%1000), env[k+1].z2=double(rand()%1000);
					env[k+1].x3=double(rand()%1000), env[k+1].y3=double(rand()%1000), env[k+1].z3=double(rand()%1000);
					if(rand()%3&&ntx)
					{
						env[k].tx=env[k+1].tx=rand()%ntx;
						double rx=double(rand())/1000, ry=double(rand())/1000, thx=double(rand())/1000, thy=double(rand())/1000;
						env[k  ].tx1x=double(rand()%500), env[k  ].tx1y=double(rand()%500), env[k  ].txXX=rx*cos(thx), env[k  ].txXY=ry*cos(thy), env[k  ].txYX=rx*sin(thx), env[k  ].txYY=ry*sin(thy);
						env[k+1].tx1x=double(rand()%500), env[k+1].tx1y=double(rand()%500), env[k+1].txXX=rx*cos(thx), env[k+1].txXY=ry*cos(thy), env[k+1].txYX=rx*sin(thx), env[k+1].txYY=ry*sin(thy);
					}
					else
						env[k].tx=env[k+1].tx=-1, env[k].color=env[k+1].color=unsigned char(rand())<<16|unsigned char(rand())<<8|unsigned char(rand());
				}
			}
			kb[wParam]=1;
			if(!timer)render();
			return 0;
		case '1':
			{
				int k2, k3, k4=int(sqrt(double(npols)/2));if(k4==0)k4=1;
				for(int k=0;k<npols;k+=2)
				{
					k2=(k/2)%k4, k3=(k/2)/k4;
									env[k  ].x1=1000* k2   , env[k  ].y1=1000* k3   , env[k  ].z1=10*k2*k3;
									env[k  ].x2=1000* k2   , env[k  ].y2=1000*(k3+1), env[k  ].z2=10*k2*k3;
									env[k  ].x3=1000*(k2+1), env[k  ].y3=1000*(k3+1), env[k  ].z3=10*k2*k3;
									if(env[k  ].tx>=0)env[k  ].tx1x=					env[k  ].tx1y=0,				env[k  ].tx2x=txw[env[k].tx], env[k  ].tx2y=0				, env[k  ].tx3x=txw[env[k].tx]	, env[k  ].tx3y=txh[env[k].tx]	, transform(k);
					if(k+1<npols){	env[k+1].x1=1000*(k2+1), env[k+1].y1=1000*(k3+1), env[k+1].z1=10*k2*k3;
									env[k+1].x2=1000*(k2+1), env[k+1].y2=1000* k3   , env[k+1].z2=10*k2*k3;
									env[k+1].x3=1000* k2   , env[k+1].y3=1000* k3   , env[k+1].z3=10*k2*k3;
									if(env[k+1].tx>=0)env[k+1].tx1x=txw[env[k+1].tx],	env[k+1].tx1y=txh[env[k+1].tx],	env[k+1].tx2x=0				, env[k+1].tx2y=txh[env[k+1].tx], env[k+1].tx3x=0				, env[k+1].tx3y=0				, transform(k+1);}
				}
			}
			kb[wParam]=1;
			if(!timer)render();
			return 0;
		case '2':
			for(int k=0;k<npols;++k)
				if(env[k  ].tx>=0)
				{
					env[k  ].tx1x=env[k  ].tx1y=0;
					env[k  ].tx2x=txw[env[k].tx], env[k  ].tx2y=0;
					env[k  ].tx3x=txw[env[k].tx], env[k  ].tx3y=txh[env[k].tx];
					transform(k);
				}
			kb[wParam]=1;
			if(!timer)render();
			return 0;
		case 'O':
			{
				OpenClipboard(hWnd);
				char *a=(char*)GetClipboardData(CF_OEMTEXT);
				if(!a||strlen(a)<=0){CloseClipboard();return 0;}
				std::string str=a; str+='\\';
				CloseClipboard();
				{
					std::vector<std::wstring> dir;
					std::string str2=str+'*';
					_WIN32_FIND_DATAA data;
					void *hSearch=FindFirstFileA(str2.c_str(), &data);
					for(;;)
					{
						int len=strlen(data.cFileName);
						if(len>=4&&data.cFileName[len-4]=='.'&&(	(data.cFileName[len-3]=='b'||data.cFileName[len-3]=='B')&&(data.cFileName[len-2]=='m'||data.cFileName[len-2]=='M')&&(data.cFileName[len-1]=='p'||data.cFileName[len-1]=='P')
																||	(data.cFileName[len-3]=='p'||data.cFileName[len-3]=='P')&&(data.cFileName[len-2]=='n'||data.cFileName[len-2]=='N')&&(data.cFileName[len-1]=='g'||data.cFileName[len-1]=='G')
																||	(data.cFileName[len-3]=='j'||data.cFileName[len-3]=='J')&&(data.cFileName[len-2]=='p'||data.cFileName[len-2]=='P')&&(data.cFileName[len-1]=='g'||data.cFileName[len-1]=='G')
																))
							dir.push_back(wider(data.cFileName));
						if(!FindNextFileA(hSearch, &data))break;
					}
					FindClose(hSearch);
					std::sort(dir.begin(), dir.end(), [](std::wstring const &a, std::wstring const &b){return StrCmpLogicalW(a.c_str(), b.c_str())==-1;});
					if(dir.size())
					{
						npols=0;
						m->clearTextures();
					}
					Gdiplus::GdiplusStartupInput gdiplusStartupInput;
					ULONG_PTR hgdiplusToken;
					Gdiplus::GdiplusStartup(&hgdiplusToken, &gdiplusStartupInput, 0);
					for(unsigned int k=0;k<dir.size();++k)
					{
						std::wstring str3=wider((char*)str.c_str())+dir[k];
						Gdiplus::Bitmap bitmap(str3.c_str());
						txh=(int*)realloc(txh, (ntx+1)*sizeof(int)), txh[ntx]=bitmap.GetHeight();
						txw=(int*)realloc(txw, (ntx+1)*sizeof(int)), txw[ntx]=bitmap.GetWidth();
						int *texture_=(int*)malloc(txh[ntx]*txw[ntx]*sizeof(int));
						Gdiplus::Rect rt(0, 0, txw[ntx], txh[ntx]);
						Gdiplus::BitmapData data;
						bitmap.LockBits(&rt, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data);
						memcpy(texture_, data.Scan0, txw[ntx]*txh[ntx]*sizeof(int));
						bitmap.UnlockBits(&data);
						m->pushTexture(texture_);
					}
					Gdiplus::GdiplusShutdown(hgdiplusToken);
					if(dir.size())
						m->enqueueTextures();
				}
				str+="!.txt";
				if(loadText(str))
				{
					ntpols=npols=0;
					unsigned char _c0[3]={0};
					for(int i=0, f=0;comtok(str, &i, &f);)
					{
						for(int k=i;k<f;++k)
						{
							switch(str[k])
							{
							case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '`':case '-':
								{
									k=i;
									int prev;
									env=(POL*)realloc(env, ++npols*sizeof(POL));
									memset(env+npols-1, 0, sizeof(POL));
									env[npols-1].tx=-1, env[npols-1].color=unsigned char(rand())<<16|unsigned char(rand())<<8|unsigned char(rand());
									env[npols-1].x1=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].x1=npols>1?env[npols-2].x1:0;
									if(k!=-1)
									{
										env[npols-1].y1=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].y1=npols>1?env[npols-2].y1:0;
										if(k!=-1)
										{
											env[npols-1].z1=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].z1=npols>1?env[npols-2].z1:0;
											if(k!=-1)
											{
												env[npols-1].x2=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].x2=npols>1?env[npols-2].x2:0;
												if(k!=-1)
												{
													env[npols-1].y2=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].y2=npols>1?env[npols-2].y2:0;
													if(k!=-1)
													{
														env[npols-1].z2=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].z2=npols>1?env[npols-2].z2:0;
														if(k!=-1)
														{
															env[npols-1].x3=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].x3=npols>1?env[npols-2].x3:0;
															if(k!=-1)
															{
																env[npols-1].y3=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].y3=npols>1?env[npols-2].y3:0;
																if(k!=-1)
																{
																	env[npols-1].z3=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].z3=npols>1?env[npols-2].z3:0;
																	if(k!=-1)
																	{
																		for(int k2=k;k2<f;++k2)
																		{
																			if(str[k2]=='t'||str[k2]=='T')
																			{
																				if(k2+1<f&&str[k2+1]=='f'||str[k2+1]=='F')
																				{
																					--npols;
																					tenv=(POL*)realloc(tenv, ++ntpols*sizeof(POL));
																					tenv[ntpols-1]=env[npols];

																					tenv[ntpols-1].tx=char(readDoubleFromBuffer(str, k, f, &k, &prev))-1; if(prev)tenv[ntpols-1].tx=ntpols>1?tenv[ntpols-2].tx:0;
																					if(tenv[ntpols-1].tx<0)
																						tenv[ntpols-1].tx=0;
																					if(tenv[ntpols-1].tx>=ntx)
																						tenv[ntpols-1].tx=ntx-1;
																					if(k!=-1)
																					{
																						tenv[ntpols-1].tx1x=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)tenv[ntpols-1].tx1x=ntpols>1?tenv[ntpols-2].tx1x:0;
																					}
																					if(k!=-1)//... tx1x\n		read should tell when last value found and when no value found
																					{
																						tenv[ntpols-1].tx1y=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)tenv[ntpols-1].tx1y=ntpols>1?tenv[ntpols-2].tx1y:0;
																						if(k!=-1)
																						{
																							tenv[ntpols-1].tx2x=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)tenv[ntpols-1].tx2x=ntpols>1?tenv[ntpols-2].tx2x:0;
																							if(k!=-1)
																							{
																								tenv[ntpols-1].tx2y=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)tenv[ntpols-1].tx2y=ntpols>1?tenv[ntpols-2].tx2y:0;
																								if(k!=-1)
																								{
																									tenv[ntpols-1].tx3x=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)tenv[ntpols-1].tx3x=ntpols>1?tenv[ntpols-2].tx3x:0;
																									if(k!=-1)
																									{
																										tenv[ntpols-1].tx3y=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)tenv[ntpols-1].tx3y=ntpols>1?tenv[ntpols-2].tx3y:0;
																									}
																								}
																							}
																						}
																						transform_transparent(ntpols-1);
																					}
																					else
																					{
																						tenv[ntpols-1].tx1x=tenv[ntpols-1].tx1y=0, tenv[ntpols-1].txXX=tenv[ntpols-1].txYY=1, tenv[ntpols-1].txXY=tenv[ntpols-1].txYX=0;
																						antitransform_transparent(ntpols-1);
																					}
																				}
																				else
																				{
																					env[npols-1].tx=char(readDoubleFromBuffer(str, k, f, &k, &prev))-1; if(prev)env[npols-1].tx=npols>1?env[npols-2].tx:0;
																					if(env[npols-1].tx<0)
																						env[npols-1].tx=0;
																					if(env[npols-1].tx>=ntx)
																						env[npols-1].tx=ntx-1;
																					if(k!=-1)
																					{
																						env[npols-1].tx1x=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].tx1x=npols>1?env[npols-2].tx1x:0;
																					}
																					if(k!=-1)//... tx1x\n		read should tell when last value found and when no value found
																					{
																						env[npols-1].tx1y=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].tx1y=npols>1?env[npols-2].tx1y:0;
																						if(k!=-1)
																						{
																							env[npols-1].tx2x=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].tx2x=npols>1?env[npols-2].tx2x:0;
																							if(k!=-1)
																							{
																								env[npols-1].tx2y=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].tx2y=npols>1?env[npols-2].tx2y:0;
																								if(k!=-1)
																								{
																									env[npols-1].tx3x=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].tx3x=npols>1?env[npols-2].tx3x:0;
																									if(k!=-1)
																									{
																										env[npols-1].tx3y=readDoubleFromBuffer(str, k, f, &k, &prev); if(prev)env[npols-1].tx3y=npols>1?env[npols-2].tx3y:0;
																									}
																								}
																							}
																						}
																						transform(npols-1);
																					}
																					else
																					{
																						env[npols-1].tx1x=env[npols-1].tx1y=0, env[npols-1].txXX=env[npols-1].txYY=1, env[npols-1].txXY=env[npols-1].txYX=0;
																						antitransform(npols-1);
																					}
																				}
																				break;
																			}
																			else if(str[k2]=='c'||str[k2]=='C')
																			{
																				if(k2+1<f&&str[k2+1]=='f'||str[k2+1]=='F')
																				{
																					auto _c=(unsigned char*)&env[npols-1].color;
																				//	unsigned char _c[3]={0};
																					if(k!=-1)
																					{
																						_c[2]=unsigned char(readDoubleFromBuffer(str, k2+2, f, &k, &prev)); if(prev)_c[2]=npols>1?_c0[2]:0;//r
																						if(k!=-1)
																						{
																							_c[1]=unsigned char(readDoubleFromBuffer(str, k, f, &k, &prev)); if(prev)_c[1]=npols>1?_c0[1]:0;//g
																							if(k!=-1)
																							{
																								_c[0]=unsigned char(readDoubleFromBuffer(str, k, f, &k, &prev)); if(prev)_c[0]=npols>1?_c0[0]:0;//b
																							}
																						}
																					}
																					else
																						_c[0]=rand(), _c[1]=rand(), _c[2]=rand();
																					memcpy(_c0, _c, 3*sizeof(char));
																				}
																				else
																				{
																					auto _c=(unsigned char*)&env[npols-1].color;
																				//	unsigned char _c[3]={0};
																					if(k!=-1)
																					{
																						_c[2]=unsigned char(readDoubleFromBuffer(str, k2+1, f, &k, &prev)); if(prev)_c[2]=npols>1?_c0[2]:0;//r
																						if(k!=-1)
																						{
																							_c[1]=unsigned char(readDoubleFromBuffer(str, k, f, &k, &prev)); if(prev)_c[1]=npols>1?_c0[1]:0;//g
																							if(k!=-1)
																							{
																								_c[0]=unsigned char(readDoubleFromBuffer(str, k, f, &k, &prev)); if(prev)_c[0]=npols>1?_c0[0]:0;//b
																							}
																						}
																					}
																					else
																						_c[0]=rand(), _c[1]=rand(), _c[2]=rand();
																				//	env[npols-1].color=_c[0]<<16|_c[1]<<8|_c[2];
																					memcpy(_c0, _c, 3*sizeof(char));
																				}
																				break;
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
								break;
							default:
								continue;
							}
							break;
						}
					}
				}
			}
			kb[wParam]=1;
			if(!timer)render();
			return 0;
		case 'X':
			PostQuitMessage(0);
			return 0;
		}
		kb[wParam]=1;
		return 0;
	case WM_KEYUP:
		switch(wParam)
		{
		case 'W':case 'A':case 'S':case 'D':case 'T':case 'G':case VK_UP:case VK_DOWN:case VK_LEFT:case VK_RIGHT:case VK_ADD:case VK_SUBTRACT:case 'Q':
			if(kp>0)kp--;
			break;
		}
		kb[wParam]=0;
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcA(hWnd, message, wParam, lParam);
}
int				__stdcall WinMain(HINSTANCE__ *hInstance, HINSTANCE__*, char*, int nCmdShow)
{
	tagWNDCLASSEXA wndClassEx={sizeof(tagWNDCLASSEXA), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, WndProc, 0, 0, hInstance, LoadIconA(0, (char*)0x00007F00), LoadCursorA(0, (char*)0x00007F00), (HBRUSH__*)(COLOR_WINDOW+1), 0, "New format", 0};
	RegisterClassExA(&wndClassEx);
	ghWnd=CreateWindowExA(0, wndClassEx.lpszClassName, "X-10/AWM28022012", WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, 0);
	ShowWindow(ghWnd, nCmdShow);
	
		QueryPerformanceCounter(&li), srand(li.LowPart);
		nticks=li.QuadPart;
		m=(Mode*)&lsw;
	//	nlsw.rebuffer=&NonlinearSW::rebuffer3;
	//	nlcl.rebuffer=2;
		hDC=GetDC(ghWnd), m->initiate();

	tagMSG msg;
	for(;GetMessageA(&msg, 0, 0, 0);)TranslateMessage(&msg), DispatchMessageA(&msg);
	
		m->finish(), ReleaseDC(ghWnd, hDC);
		free(env), free(txh), free(txw);

	return msg.wParam;
}
void			render()
{
	if(timer){		 if(kb['Q'])						for(int k=0;k<npols;++k)if(env[k].tx>=0)env[k].tx1x+=rand()%3-1, env[k].tx1y+=rand()%3-1, env[k].tx2x+=rand()%3-1, env[k].tx2y+=rand()%3-1, env[k].tx3x+=rand()%3-1, env[k].tx3y+=rand()%3-1, transform(k);
					 if(kb[VK_SHIFT]){	 if(kb['W'])	camx+=10.*dcam*cax*cay,	camy+=10.*dcam*sax*cay,	camz+=10.*dcam*say;
										 if(kb['A'])	camx-=10.*dcam*sax,		camy+=10.*dcam*cax;
										 if(kb['S'])	camx-=10.*dcam*cax*cay,	camy-=10.*dcam*sax*cay,	camz-=10.*dcam*say;
										 if(kb['D'])	camx+=10.*dcam*sax,		camy-=10.*dcam*cax;
									//	 if(kb['T'])	camx-=10.*dcam*cax*say,	camy-=10.*dcam*sax*say,	camz+=10.*dcam*cay;
									//	 if(kb['G'])	camx+=10.*dcam*cax*say,	camy+=10.*dcam*sax*say,	camz-=10.*dcam*cay;}
										 if(kb['T'])	camz+=10.*dcam;
										 if(kb['G'])	camz-=10.*dcam;}
				else{					 if(kb['W'])	camx+=dcam*cax*cay,		camy+=dcam*sax*cay,		camz+=dcam*say;
										 if(kb['A'])	camx-=dcam*sax,			camy+=dcam*cax;
										 if(kb['S'])	camx-=dcam*cax*cay,		camy-=dcam*sax*cay,		camz-=dcam*say;
										 if(kb['D'])	camx+=dcam*sax,			camy-=dcam*cax;
									//	 if(kb['T'])	camx-=dcam*cax*say,		camy-=dcam*sax*say,		camz+=dcam*cay;
									//	 if(kb['G'])	camx+=dcam*cax*say,		camy+=dcam*sax*say,		camz-=dcam*cay;}
										 if(kb['T'])	camz+=dcam;
										 if(kb['G'])	camz-=dcam;}
					 if(kb[VK_UP]){						ay+=da_tfov*da, cay=cos(ay), say=sin(ay);	for(;ay<0;)ay+=TWOPI;	for(;ay>TWOPI;)ay-=TWOPI;}
					 if(kb[VK_DOWN]){					ay-=da_tfov*da, cay=cos(ay), say=sin(ay);	for(;ay<0;)ay+=TWOPI;	for(;ay>TWOPI;)ay-=TWOPI;}
					 if(kb[VK_LEFT]){					ax+=da_tfov*da, cax=cos(ax), sax=sin(ax);	for(;ax<0;)ax+=TWOPI;	for(;ax>TWOPI;)ax-=TWOPI;}
					 if(kb[VK_RIGHT]){					ax-=da_tfov*da, cax=cos(ax), sax=sin(ax);	for(;ax<0;)ax+=TWOPI;	for(;ax>TWOPI;)ax-=TWOPI;}
					 if(kb[VK_ADD])						tanfov/=DTANFOV, fov/=DFOV, m->changeFov(), da_tfov=tanfov>1?1:tanfov;
					 if(kb[VK_SUBTRACT])				tanfov*=DTANFOV, fov*=DFOV, m->changeFov(), da_tfov=tanfov>1?1:tanfov;
					 if(kb[VK_RBUTTON]){				int boom=10;
														for(int k=0;k<npols;++k){	env[k].x1+=(rand()%boom)-boom/2, env[k].y1+=(rand()%boom)-boom/2, env[k].z1+=(rand()%boom)-boom/2;
																					env[k].x2+=(rand()%boom)-boom/2, env[k].y2+=(rand()%boom)-boom/2, env[k].z2+=(rand()%boom)-boom/2;
																					env[k].x3+=(rand()%boom)-boom/2, env[k].y3+=(rand()%boom)-boom/2, env[k].z3+=(rand()%boom)-boom/2, transform(k);}}}
	m->render();
}