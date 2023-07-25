#include"vksandbox.h"
#include<ctype.h>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
//#define TINYOBJ_LOADER_C_IMPLEMENTATION
//#include"tinyobj_loader_c.h"
static const char file[]=__FILE__;


void load_texture(Texture *texture, const char *searchpath, const char *filetitle, int ch)
{
	ArrayHandle filename;
	STR_COPY(filename, filetitle, strlen(filetitle));
	texture->data=stbi_load((char*)filename->data, &texture->iw, &texture->ih, &texture->ch0, ch);
	if(!texture->data)
	{
		array_insert(&filename, 0, searchpath, strlen(searchpath), 1, 1);
		texture->data=stbi_load((char*)filename->data, &texture->iw, &texture->ih, &texture->ch0, ch);
		if(!texture->data)
		{
			LOG_ERROR("Couldn't find %s", filetitle);
			return;
		}
	}
	texture->ch=ch;
}


typedef struct vec2Struct
{
	float x, y;
} vec2;
typedef struct vec3Struct
{
	float x, y, z;
} vec3;
typedef struct VertRefStruct
{
	int v, vt, vn;
} VertRef;
typedef struct VertRef2Struct
{
	int v, vt, vn, idx;
} VertRef2;
//typedef struct FaceStruct
//{
//	int v[3], t[3], n[3];
//} Face;
typedef struct VertexIdxStruct
{
	Vertex v;
	int idx;
} VertexIdx;
CmpRes cmp_vertex(const void *key, const void *candidate)
{
	const float tolerance=1e-6f;
	Vertex const *l, *r;

	l=(Vertex const*)key;
	r=(Vertex const*)candidate;
	if(fabsf(l->x-r->x)>tolerance)
		return (l->x>r->x)-(l->x<r->x);
	if(fabsf(l->y-r->y)>tolerance)
		return (l->y>r->y)-(l->y<r->y);
	if(fabsf(l->z-r->z)>tolerance)
		return (l->z>r->z)-(l->z<r->z);
	
	if(fabsf(l->tx-r->tx)>tolerance)
		return (l->tx>r->tx)-(l->tx<r->tx);
	if(fabsf(l->ty-r->ty)>tolerance)
		return (l->ty>r->ty)-(l->ty<r->ty);
	return RESULT_EQUAL;
}
CmpRes cmp_vertref2(const void *key, const void *candidate)
{
	VertRef2 const *l, *r;
	int result;

	l=(VertRef2 const*)key;
	r=(VertRef2 const*)candidate;

	result=(l->v>r->v)-(l->v<r->v);
	if(result)
		return result;
	result=(l->vt>r->vt)-(l->vt<r->vt);
	if(result)
		return result;
	return (l->vn>r->vn)-(l->vn<r->vn);
}
void parse_obj(const char *text, ptrdiff_t len, ArrayHandle *vertices, ArrayHandle *indices)//https://stackoverflow.com/questions/52824956/how-can-i-parse-a-simple-obj-file-into-triangles
{
	ptrdiff_t idx=0, lineno=0;

	DList positions, texcoords, normals, faces;
	dlist_init(&positions, sizeof(vec3), 256, 0);
	dlist_init(&texcoords, sizeof(vec2), 256, 0);
	dlist_init(&normals, sizeof(vec3), 256, 0);
	dlist_init(&faces, sizeof(VertRef[3]), 256, 0);

	//ArrayHandle positions, texcoords, normals;
	//ARRAY_ALLOC(vec3, positions, 0, 0, 0, 0);
	//ARRAY_ALLOC(vec2, texcoords, 0, 0, 0, 0);
	//ARRAY_ALLOC(vec3, normals, 0, 0, 0, 0);

	for(;idx<len;++lineno)
	{
		for(;idx<len&&isspace(text[idx]);++idx);//skip space
		if(idx>=len)
			break;

		if(text[idx]=='#')
		{
			for(;idx<len&&text[idx]!='\n';++idx);//skip comment
			++idx;//skip newline
			continue;
		}

		ptrdiff_t start=idx;
		for(;idx<len&&!isspace(text[idx]);++idx);
		if(idx-start>2)
		{
			for(;idx<len&&text[idx]!='\n';++idx);//skip line
			++idx;//skip newline
			continue;
		}
		//	LOG_ERROR("Line %d: unrecognized token", lineno);
		int tag=0;
		memcpy(&tag, text+start, (size_t)idx-start);

		for(;idx<len&&isspace(text[idx]);++idx);//skip space

		float data[4];
		VertRef face[3];
		char *end=0;
		switch(tag&0xDFDFDFDF)
		{
		case 'V'://vertex (position)
			for(int k=0;k<3;++k)
			{
				data[k]=strtof(text+idx, &end);
				idx=end-text;
				for(;idx<len&&isspace(text[idx]);++idx);//skip space
			}
			dlist_push_back1(&positions, data);
			break;
		case 'V'|'T'<<8://texcoord
			for(int k=0;k<2;++k)
			{
				data[k]=strtof(text+idx, &end);
				idx=end-text;
				for(;idx<len&&isspace(text[idx]);++idx);//skip space
			}
			dlist_push_back1(&texcoords, data);
			break;
		case 'V'|'N'<<8://normal
			for(int k=0;k<3;++k)
			{
				data[k]=strtof(text+idx, &end);
				idx=end-text;
				for(;idx<len&&isspace(text[idx]);++idx);//skip space
			}
			dlist_push_back1(&normals, data);
			break;
		case 'F'://face
			for(int k=0;k<9;++k)
			{
				((int*)face)[k]=(int)_strtoi64(text+idx, &end, 10);
				idx=end-text;
				for(;idx<len&&(isspace(text[idx])||text[idx]=='/');++idx);//skip space or slashes
			}
			dlist_push_back1(&faces, face);
			break;
		default:
			for(;idx<len&&text[idx]!='\n';++idx);//skip line
			++idx;//skip newline

			//LOG_ERROR("Line %d: Unrecognized token");
			continue;
		}

#if 0
		// vertex
		if( lineType == "v" )
		{
			float x = 0, y = 0, z = 0, w = 1;
			lineSS >> x >> y >> z >> w;
			positions.push_back( glm::vec4( x, y, z, w ) );
		}

		// texture
		if( lineType == "vt" )
		{
			float u = 0, v = 0, w = 0;
			lineSS >> u >> v >> w;
			texcoords.push_back( glm::vec3( u, v, w ) );
		}

		// normal
		if( lineType == "vn" )
		{
			float i = 0, j = 0, k = 0;
			lineSS >> i >> j >> k;
			normals.push_back( glm::normalize( glm::vec3( i, j, k ) ) );
		}

		// polygon
		if( lineType == "f" )
		{
			std::vector< VertRef > refs;
			std::string refStr;
			while( lineSS >> refStr )
			{
				std::istringstream ref( refStr );
				std::string vStr, vtStr, vnStr;
				std::getline( ref, vStr, '/' );
				std::getline( ref, vtStr, '/' );
				std::getline( ref, vnStr, '/' );
				int v = atoi( vStr.c_str() );
				int vt = atoi( vtStr.c_str() );
				int vn = atoi( vnStr.c_str() );
				v  = (  v >= 0 ?  v : positions.size() +  v );
				vt = ( vt >= 0 ? vt : texcoords.size() + vt );
				vn = ( vn >= 0 ? vn : normals.size()   + vn );
				refs.push_back( VertRef( v, vt, vn ) );
			}

			// triangulate, assuming n>3-gons are convex and coplanar
			for( size_t i = 1; i+1 < refs.size(); ++i )
			{
				const VertRef* p[3] = { &refs[0], &refs[i], &refs[i+1] };

				// http://www.opengl.org/wiki/Calculating_a_Surface_Normal
				glm::vec3 U( positions[ p[1]->v ] - positions[ p[0]->v ] );
				glm::vec3 V( positions[ p[2]->v ] - positions[ p[0]->v ] );
				glm::vec3 faceNormal = glm::normalize( glm::cross( U, V ) );

				for( size_t j = 0; j < 3; ++j )
				{
					Vertex vert;
					vert.position = glm::vec3( positions[ p[j]->v ] );
					vert.texcoord = glm::vec2( texcoords[ p[j]->vt ] );
					vert.normal = ( p[j]->vn != 0 ? normals[ p[j]->vn ] : faceNormal );
					verts.push_back( vert );
				}
			}
		}
#endif
	}

	ArrayHandle pos=0, tex=0, nor=0, fac=0;
	dlist_appendtoarray(&positions, &pos);
	dlist_clear(&positions);

	dlist_appendtoarray(&texcoords, &tex);
	dlist_clear(&texcoords);

	dlist_appendtoarray(&normals, &nor);
	dlist_clear(&normals);

	dlist_appendtoarray(&faces, &fac);
	dlist_clear(&faces);

	DList vlist, ilist;
	dlist_init(&vlist, sizeof(Vertex), 256, 0);
	dlist_init(&ilist, sizeof(int[3]), 256, 0);
	Map verts, vertrefs;
	map_init(&verts, sizeof(VertexIdx), cmp_vertex, 0);
	map_init(&vertrefs, sizeof(VertRef2), cmp_vertref2, 0);
	for(ptrdiff_t k=0;k<(ptrdiff_t)fac->count;++k)
	{
		VertRef *face=(VertRef*)array_at(&fac, k);

		int *idx=dlist_push_back1(&ilist, 0);
		for(int kv=0;kv<3;++kv)
		{
			int found=0;
			RBNodeHandle *hnode=map_insert(&vertrefs, face+kv, &found);
			VertRef2 *ref2=(VertRef2*)hnode[0]->data;
			if(!found)
			{
				memcpy(ref2, face+kv, sizeof(VertRef));

				vec3 *p3=(vec3*)array_at(&pos, ref2->v-1);
				vec2 *p2=(vec2*)array_at(&tex, ref2->vt-1);
				Vertex v=
				{
					p3->x, p3->y, p3->z,
					1, 1, 1,
					p2->x, 1-p2->y,
				};
				hnode=map_insert(&verts, &v, &found);
				VertexIdx *v2=(VertexIdx*)hnode[0]->data;
				if(!found)
				{
					memcpy(v2, &v, sizeof(Vertex));
					v2->idx=(int)vlist.nobj;

					dlist_push_back1(&vlist, &v);
				}
				ref2->idx=v2->idx;
			}
			idx[kv]=ref2->idx;
		}
	}
	array_free(&pos);
	array_free(&tex);
	array_free(&nor);
	array_free(&fac);
	MAP_CLEAR(&vertrefs);
	dlist_appendtoarray(&vlist, vertices);
	dlist_clear(&vlist);

	dlist_appendtoarray(&ilist, indices);
	dlist_clear(&ilist);
	indices[0]->count*=3;
	indices[0]->esize=sizeof(int);
}
#if 0
typedef struct FileCallbackInfoStruct
{
	ArrayHandle text;
} FileCallbackInfo;
void file_callback(void *ctx, const char *filename, int is_mtl, const char *obj_filename, char **buf, size_t *len)
{
	FileCallbackInfo *info=(FileCallbackInfo*)ctx;
	info->text=load_file(filename, 0, 0, 1);
	*buf=info->text->data;
	*len=info->text->count;
}
#endif
void load_model(const char *searchpath, const char *filetitle, ArrayHandle *vertices, ArrayHandle *indices)
{
	ArrayHandle filename;
	STR_COPY(filename, filetitle, strlen(filetitle));
	ptrdiff_t size=get_filesize((char*)filename->data);
	if(size==-1)
	{
		array_insert(&filename, 0, searchpath, strlen(searchpath), 1, 1);
		size=get_filesize((char*)filename->data);
		if(size==-1)
		{
			LOG_ERROR("Couldn't find %s", filetitle);
			return;
		}
	}

	ArrayHandle text=load_file((char*)filename->data, 0, 16, 1);
	parse_obj((char*)text->data, text->count, vertices, indices);
	array_free(&text);
#if 0
	tinyobj_attrib_t attrib;
	tinyobj_shape_t *shapes=0;
	size_t nshapes=0;
	tinyobj_material_t *materials=0;
	size_t nmaterials=0;

	tinyobj_attrib_init(&attrib);

	FileCallbackInfo ctx={0};

	int error=tinyobj_parse_obj(&attrib, &shapes, &nshapes, &materials, &nmaterials, (char*)filename->data, file_callback, &ctx, TINYOBJ_FLAG_TRIANGULATE);
	if(error)
		LOG_ERROR("Failed to load %s", filetitle);

	for(int ks=0;ks<(int)nshapes;++ks)
	{
		tinyobj_shape_t *shape=shapes+ks;
		for(int ki=0;ki<shape->
	}

	tinyobj_attrib_free(&attrib);
	tinyobj_shapes_free(shapes, nshapes);
	tinyobj_materials_free(materials, nmaterials);
#endif
}
#ifdef DEBUG_TEST
#include<stdio.h>
int main(int argc, char **argv)
{
	ArrayHandle vertices=0, indices=0;
	load_model(SEARCHPATH, "viking_room.txt", vertices, indices);

	printf("Loaded %d vertices  %d indices\n", (int)vertices->count, (int)indices->count);
	array_free(&vertices);
	array_free(&indices);
	return 0;
}
#endif