#include"vksandbox.h"
#include<stdlib.h>
#include<string.h>
#define _USE_MATH_DEFINES
#include<math.h>
static const char file[]=__FILE__;


#define NPARTICLES 8192
static const char *searchpath=SEARCHPATH;


char drag=0;
int mx0=0, my0=0;
float
	mouse_sensitivity=0.003f,
	key_turn_speed=0.03f;
Camera cam=
{
	2, 2, 2,
	(float)(225*M_PI/180), (float)(324.7356103172454f*M_PI/180),
	1,
	0.04f, (float)(2*M_PI/180),
}, cam0;
#define ACTIVE_KEY_LIST\
	AK('W') AK('A') AK('S') AK('D') AK('T') AK('G')\
	AK(KEY_LEFT) AK(KEY_RIGHT) AK(KEY_UP) AK(KEY_DOWN)\
	AK(KEY_ENTER) AK(KEY_BKSP)
int active_keys_pressed=0;

static void count_active_keys(IOKey upkey)
{
	keyboard[upkey]=0;
	active_keys_pressed=0;
#define		AK(KEY)		active_keys_pressed+=keyboard[KEY];
	ACTIVE_KEY_LIST
#undef		AK
	if(!active_keys_pressed)
		timer_stop();
}

int io_init(int argc, char **argv)//return false to abort
{
	set_window_title("VkSandbox");

	cam_zoomIn(cam, 1);
	cam_turnMouse(cam, 0, 0, mouse_sensitivity);
	memcpy(&cam0, &cam, sizeof(cam));

	Texture atlas;
	load_texture(&atlas, searchpath, "font_sdf.PNG", 1);
	init_atlas_coords(&atlas);


	Texture texture;
	load_texture(&texture, searchpath, "viking_room.png", 4);
	//load_texture(&texture, searchpath, "texture.jpg", 4);

	int enableanisotropy=0;
	int miplevels=1;
	//int miplevels=floor_log2(MAXVAR(texture.iw, texture.ih))+1;
	int enableMSAA=0;

	ArrayHandle vertices=0, indices=0;
	load_model(searchpath, "viking_room.txt", &vertices, &indices);

	ArrayHandle particles;
	ARRAY_ALLOC(Vertex, particles, 0, NPARTICLES, 0, 0);
	for(int k=0;k<(int)particles->count;++k)
	{
		Vertex *p=(Vertex*)array_at(&particles, k);
		float r=0.25f*sqrtf((float)rand()/RAND_MAX);
		float theta=(float)(rand()*(2*M_PI/RAND_MAX)), phi=(float)(rand()*(M_PI/RAND_MAX));
		float cos_phi=cosf(phi);
		p->x=r*cos_phi*cosf(theta);
		p->y=r*cos_phi*sinf(theta);
		p->z=r*sinf(phi);
		p->r=p->x*0.00025f;
		p->g=p->y*0.00025f;
		p->b=p->z*0.00025f;
		p->tx=(float)rand()/RAND_MAX;
		p->ty=(float)rand()/RAND_MAX;

		//p->r=(float)rand()/RAND_MAX;
		//p->g=(float)rand()/RAND_MAX;
		//p->b=(float)rand()/RAND_MAX;
		//p->a=1;
	}

	vkw_init(searchpath, &atlas, &texture, vertices, indices, particles, enableanisotropy, miplevels, enableMSAA);

	free(atlas.data);
	free(texture.data);
	array_free(&vertices);
	array_free(&indices);

	return 1;
}
void io_resize()
{
	vkw_resize(w, h);
}
int io_mousemove()//return true to redraw
{
	if(drag)
	{
		int X0=w>>1, Y0=h>>1;
		cam_turnMouse(cam, mx-X0, my-Y0, mouse_sensitivity);
		set_mouse(X0, Y0);
		return !timer;
	}
	return 0;
}
int io_mousewheel(int forward)
{
	if(keyboard[KEY_SHIFT])//shift wheel		change cam speed
	{
			 if(forward>0)	cam.move_speed*=2;
		else				cam.move_speed*=0.5f;
	}
	else
	{
			 if(forward>0)	cam_zoomIn(cam, 1.1f);
		else				cam_zoomOut(cam, 1.1f);
	}
	return !timer;
}
int io_keydn(IOKey key, char c)
{
	switch(key)
	{
	case KEY_LBUTTON:
	case KEY_ESC:
		show_mouse(drag);
		drag=!drag;
		if(drag)//enter mouse control
		{
			mx0=mx, my0=my;
			set_mouse(w>>1, h>>1);
		}
		else//leave mouse control
			set_mouse(mx0, my0);
		break;
#define AK(KEY) case KEY:
	ACTIVE_KEY_LIST
#undef  AK
		timer_start(10);
		break;
	case 'R':
		memcpy(&cam, &cam0, sizeof(cam));
		return 1;
	}
	return 0;
}
int io_keyup(IOKey key, char c)
{
	switch(key)
	{
#define AK(KEY) case KEY:
	ACTIVE_KEY_LIST
#undef  AK
		count_active_keys(key);
		break;
	}
	return 0;
	return 0;
}
void io_timer()
{
	float move_speed=keyboard[KEY_SHIFT]?10*cam.move_speed:cam.move_speed;
	if(keyboard['W'])		cam_moveForward(cam, move_speed);
	if(keyboard['A'])		cam_moveLeft(cam, move_speed);
	if(keyboard['S'])		cam_moveBack(cam, move_speed);
	if(keyboard['D'])		cam_moveRight(cam, move_speed);
	if(keyboard['T'])		cam_moveUp(cam, move_speed);
	if(keyboard['G'])		cam_moveDown(cam, move_speed);
	if(keyboard[KEY_UP])	cam_turnUp(cam, key_turn_speed);
	if(keyboard[KEY_DOWN])	cam_turnDown(cam, key_turn_speed);
	if(keyboard[KEY_LEFT])	cam_turnLeft(cam, key_turn_speed);
	if(keyboard[KEY_RIGHT])	cam_turnRight(cam, key_turn_speed);
	if(keyboard[KEY_ENTER])	cam_zoomIn(cam, 1.1f);
	if(keyboard[KEY_BKSP])	cam_zoomOut(cam, 1.1f);
}
void io_render()
{
	GUIPrint(0, 0, 0, "Hello");

	vkw_render();
	text_vertices->count=0;
	
	static double t0=0;
	double t=time_ms();
	set_window_title("VkSandbox  fps %lf", 1000/(t-t0));
	t0=t;
}
int io_quit_request()//return 1 to exit
{
	return 1;
}
void io_cleanup()//cleanup
{
	vkw_cleanup();
}