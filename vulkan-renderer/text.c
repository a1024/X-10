#include"vksandbox.h"
#include<stdio.h>
#include<string.h>
#include<stdarg.h>
static const char file[]=__FILE__;

short tab_count=8;
float fontW=0, fontH=0;//non-tab character dimensions (mono)
float sdf_txh=0, sdf_slope=0.062023f;
extern float const *const font_zoom;

ArrayHandle text_vertices=0;

typedef struct		QuadCoordsStruct
{
	float x1, x2, y1, y2;
} QuadCoords;
QuadCoords atlas_coords[128-32]={0};
typedef struct SDFTextureHeaderStruct
{
	double slope;
	char
		grid_start_x, grid_start_y,
		cell_size_x, cell_size_y,
		csize_x, csize_y,
		reserved[2];
} SDFTextureHeader;
void init_atlas_coords(Texture const *atlas)
{
	SDFTextureHeader header;
	memcpy(&header, atlas->data, sizeof(header));
	fontW=header.csize_x;
	fontH=header.csize_y;

	sdf_slope=(float)header.slope;
	for(int c=32;c<127;++c)
	{
		QuadCoords *rect=atlas_coords+c-32;
		int px=header.grid_start_x+header.cell_size_x*(c&7),
			py=header.grid_start_y+header.cell_size_y*((c>>3)-4);
		rect->x1=(float)px/atlas->iw;
		rect->x2=(float)(px+fontW)/atlas->iw;
		rect->y1=(float)py/atlas->ih;
		rect->y2=(float)(py+fontH)/atlas->ih;
	}
	sdf_txh=fontH;
	fontW*=16.f/fontH;
	fontH=16;
}
float print_line(float tab_origin, float x, float y, const char *msg, int msg_length, int req_cols, int *ret_idx, int *ret_cols)
{
	if(msg_length<1)
		return 0;
	if(!text_vertices)
		ARRAY_ALLOC(float[4], text_vertices, 0, 0, 256, 0);
	float rect[4]={0};
	QuadCoords *txc=0;
	//if(sdf_active)
	//	zoom*=16.f/sdf_txh;
	float width=fontW**font_zoom, height=fontH**font_zoom;
	int tab_origin_cols=(int)(tab_origin/width), printable_count=0, cursor_cols=ret_cols?*ret_cols:0, advance_cols;
	if(y+height<currentregion->ry0||y>=currentregion->ry0+currentregion->rdy)//off-screen optimization
		return 0;//no need to do anything, this line is outside the window
	//	return idx2col(msg, msg_length, (int)(tab_origin/width))*width;
	float CX1=2.f/currentregion->rdx, CX0=CX1*(x-currentregion->rx0)-1;//delta optimization
	rect[1]=1-(y-currentregion->ry0)*2.f/currentregion->rdy;
	rect[3]=1-(y+height-currentregion->ry0)*2.f/currentregion->rdy;
	//vrtx_resize(msg_length<<2, 4);//vx, vy, txx, txy		x4 vertices/char
	int k=ret_idx?*ret_idx:0;
	if(req_cols<0||cursor_cols<req_cols)
	{
		CX1*=width;
		for(;k<msg_length;++k)
		{
			char c=msg[k];
			if(c>=32&&c<0xFF)
				advance_cols=1;
			else if(c=='\t')
			{
				MODVAR(advance_cols, cursor_cols-tab_origin_cols, tab_count);
				advance_cols=tab_count-advance_cols;
				c=' ';
				//advance_cols=tab_count-mod(cursor_cols-tab_origin_cols, tab_count), c=' ';
			}
			else
				advance_cols=0;
			if(advance_cols)
			{
				if(x+(cursor_cols+advance_cols)*width>=currentregion->rx0&&x+cursor_cols*width<currentregion->rx0+currentregion->rdx)//off-screen optimization
				{
					rect[0]=CX1*cursor_cols+CX0;//xn1
					cursor_cols+=advance_cols;
					rect[2]=CX1*cursor_cols+CX0;//xn2

					//rect[0]=(x+msg_width-rx0)*2.f/rdx-1;//xn1
					//rect[1]=1-(y-ry0)*2.f/rdy;//yn1
					//rect[2]=(x+msg_width+width-rx0)*2.f/rdx-1;//xn2
					//rect[3]=1-(y+height-ry0)*2.f/rdy;//yn2

					//toNDC_nobias(float(x+msg_width		), float(y			), rect[0], rect[1]);
					//toNDC_nobias(float(x+msg_width+width	), float(y+height	), rect[2], rect[3]);//y2<y1

					txc=atlas_coords+c-32;
					float *vrtx=ARRAY_APPEND(text_vertices, 0, 6, 1, 0);
					vrtx[ 0]=rect[0], vrtx[ 1]=rect[1],		vrtx[ 2]=txc->x1, vrtx[ 3]=txc->y1;//top left
					vrtx[ 4]=rect[0], vrtx[ 5]=rect[3],		vrtx[ 6]=txc->x1, vrtx[ 7]=txc->y2;//bottom left
					vrtx[ 8]=rect[2], vrtx[ 9]=rect[3],		vrtx[10]=txc->x2, vrtx[11]=txc->y2;//bottom right
					vrtx[12]=rect[2], vrtx[13]=rect[3],		vrtx[14]=txc->x2, vrtx[15]=txc->y2;//bottom right
					vrtx[16]=rect[2], vrtx[17]=rect[1],		vrtx[18]=txc->x2, vrtx[19]=txc->y1;//top right
					vrtx[20]=rect[0], vrtx[21]=rect[1],		vrtx[22]=txc->x1, vrtx[23]=txc->y1;//top left

					++printable_count;
				}
				else
					cursor_cols+=advance_cols;
				if(req_cols>=0&&cursor_cols>=req_cols)
				{
					++k;
					break;
				}
			}
		}
#if 0
		if(printable_count)
		{
//#ifndef NO_3D
//			glDisable(GL_DEPTH_TEST);
//#endif
			if(sdf_active)
			{
				setGLProgram(shader_sdftext.program);
				glUniform1f(u_sdftext_zoom, zoom*16.f/(sdf_txh*sdf_slope));
				select_texture(sdf_atlas_txid, u_sdftext_atlas);

				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);							GL_CHECK(error);
				glBufferData(GL_ARRAY_BUFFER, printable_count<<6, vrtx, GL_STATIC_DRAW);GL_CHECK(error);
				glVertexAttribPointer(a_sdftext_coords, 4, GL_FLOAT, GL_TRUE, 0, 0);	GL_CHECK(error);

			//	glBindBuffer(GL_ARRAY_BUFFER, 0);										GL_CHECK(error);
			//	glVertexAttribPointer(a_sdftext_coords, 4, GL_FLOAT, GL_TRUE, 0, vrtx);	GL_CHECK(error);

				glEnableVertexAttribArray(a_sdftext_coords);	GL_CHECK(error);
				glDrawArrays(GL_QUADS, 0, printable_count<<2);	GL_CHECK(error);
				glDisableVertexAttribArray(a_sdftext_coords);	GL_CHECK(error);
			}
			else
			{
				setGLProgram(shader_text.program);
				select_texture(font_txid, u_text_atlas);

				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);							GL_CHECK(error);
				glBufferData(GL_ARRAY_BUFFER, printable_count<<6, vrtx, GL_STATIC_DRAW);GL_CHECK(error);//set vertices & texcoords
				glVertexAttribPointer(a_text_coords, 4, GL_FLOAT, GL_TRUE, 0, 0);		GL_CHECK(error);

			//	glBindBuffer(GL_ARRAY_BUFFER, 0);									GL_CHECK(error);
			//	glVertexAttribPointer(a_text_coords, 4, GL_FLOAT, GL_TRUE, 0, vrtx);GL_CHECK(error);

				glEnableVertexAttribArray(a_text_coords);		GL_CHECK(error);
				glDrawArrays(GL_QUADS, 0, printable_count<<2);	GL_CHECK(error);//draw the quads: 4 vertices per character quad
				glDisableVertexAttribArray(a_text_coords);		GL_CHECK(error);
			}
//#ifndef NO_3D
//			glEnable(GL_DEPTH_TEST);
//#endif
		}
#endif
	}
	if(ret_idx)
		*ret_idx=k;
	if(ret_cols)
		*ret_cols=cursor_cols;
	return cursor_cols*width;
}
float GUIPrint(float tab_origin, float x, float y, const char *format, ...)
{
	int len;
	va_list args;

	va_start(args, format);
	len=vsnprintf(g_buf, G_BUF_SIZE, format, args);
	va_end(args);
	return print_line(tab_origin, x, y, g_buf, len, -1, 0, 0);
}