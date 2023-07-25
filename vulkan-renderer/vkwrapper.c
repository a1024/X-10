#include"vksandbox.h"
#include<stdio.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<tmmintrin.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include<vulkan/vulkan.h>
#pragma comment(lib, "vulkan-1.lib")
static const char file[]=__FILE__;


	#define ENABLE_VALIDATION
	#define COMPILE_SHADERS		//compilation requires Vulkan SDK to run

//	#define ENABLE_COMPUTE
//	#define DISABLE_TEXT


#define NSHADERS 5
#define MAXFRAMESINFLIGHT 2

#define printf console_log

static Region region={0};
Region const *const currentregion=&region;

static VkInstance instance=0;
#ifdef ENABLE_VALIDATION
static const int messageseverity=VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
static VkDebugUtilsMessengerEXT messenger=0;
#endif
static VkPhysicalDevice physicaldevice=0;
static VkPhysicalDeviceProperties physicaldeviceproperties={0};
static VkPhysicalDeviceMemoryProperties physicaldevicememproperties={0};
static VkDevice device=0;
static int graphicsqueuefamily=0, presentqueuefamily=0;
static VkQueue graphicsqueue=0, presentqueue=0;
#ifdef ENABLE_COMPUTE
static int graphicsandcomputequeuefamily=0;
static VkQueue computequeue=0;
#endif
static VkSurfaceKHR surface=0;
typedef struct SwapChainSupportDetailsStruct
{
	VkSurfaceCapabilitiesKHR capabilities;
	ArrayHandle formats;//VkSurfaceFormatKHR
	ArrayHandle presentmodes;//VkPresentModeKHR
} SwapChainSupportDetails;
static SwapChainSupportDetails swapsupport={0};
static VkSurfaceFormatKHR surfaceformat={VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
static VkPresentModeKHR presentmode=VK_PRESENT_MODE_FIFO_KHR;
static VkSwapchainKHR swapchain=0;
static VkExtent2D extent={0, 0};
static VkViewport viewport={0};
static VkRect2D scissor={0};
static ArrayHandle images=0;//VkImage
static ArrayHandle imageviews=0;//VkImageViews
static VkShaderModule shaders[NSHADERS]={0};//{vertexshader, fragmentshader, computeshader}
static VkRenderPass renderpass=0;
static VkDescriptorSetLayout descriptorsetlayout=0;
static VkPipelineLayout pipelinelayout=0;
static VkPipeline graphicspipelines[2]={0};//{3D, sdftext}
static ArrayHandle swapchainframebuffers=0;//VkFramebuffer
static VkCommandPool commandpool=0;
static unsigned currentframe=0;
static VkCommandBuffer commandbuffers[MAXFRAMESINFLIGHT]={0};
static VkSemaphore imageavailablesemaphores[MAXFRAMESINFLIGHT]={0}, renderfinishedsemaphores[MAXFRAMESINFLIGHT]={0};
static VkFence inflightfences[MAXFRAMESINFLIGHT]={0};
static int resized=0;
static VkBuffer vertexbuffer=0;
static VkDeviceMemory vertexbuffermemory=0;

static VkBuffer indexbuffer=0;
static VkDeviceMemory indexbuffermemory=0;

static VkBuffer uniformbuffers[MAXFRAMESINFLIGHT]={0};
static VkDeviceMemory uniformbuffersmemory[MAXFRAMESINFLIGHT]={0};
static void *uniformbuffersmapped[MAXFRAMESINFLIGHT]={0};

static VkBuffer sdftext_uniformbuffers[MAXFRAMESINFLIGHT]={0};
static VkDeviceMemory sdftext_uniformmemory[MAXFRAMESINFLIGHT]={0};
static void *sdftext_uniformmap[MAXFRAMESINFLIGHT]={0};

static VkDescriptorPool descriptorpool=0;
static VkDescriptorSet descriptorsets[MAXFRAMESINFLIGHT]={0};

static VkFormat depthformat=0;
static VkImage depthimage=0;
static VkDeviceMemory depthmemory=0;
static VkImageView depthimageview=0;

static VkSampler texturesampler=0;

static int miplevels=0;
static VkImage textureimage=0;
static VkDeviceMemory texturememory=0;
static VkImageView textureimageview=0;

static int indexcount=0;

static int maxAAsamples=1, frameAAsamples=1;//must be nonzero POT
static VkImage colorimage=0;
static VkDeviceMemory colorimagememory=0;
static VkImageView colorimageview=0;

#ifdef ENABLE_COMPUTE
static int nparticles=0;
static VkPipelineLayout computepipelinelayout=0;
static VkPipeline computepipeline=0;
static VkBuffer shaderstoragebuffers[MAXFRAMESINFLIGHT]={0};
static VkDeviceMemory shaderstoragememory[MAXFRAMESINFLIGHT]={0};
static VkCommandBuffer computecommandbuffers[MAXFRAMESINFLIGHT]={0};
static VkFence computeinflightfences[MAXFRAMESINFLIGHT]={0};
static VkSemaphore computefinishedsemaphores[MAXFRAMESINFLIGHT]={0};
#endif

#ifndef DISABLE_TEXT
static VkImage sdftext_atlas=0;
static VkDeviceMemory sdftext_atlasmemory=0;
static VkImageView sdftext_atlasview=0;

static size_t sdftext_bufsize=0, sdftext_memsize=0;
static VkBuffer sdftext_vertexbuffer=0;
static VkDeviceMemory sdftext_vertexmemory=0;
static void *sdftext_vertexmap=0;
#endif


const char* vkerror2str(int error)
{
#define CASE(E) case E:return #E
	switch(error)
	{
	CASE(VK_SUCCESS);
	CASE(VK_NOT_READY);
	CASE(VK_TIMEOUT);
	CASE(VK_EVENT_SET);
	CASE(VK_EVENT_RESET);
	CASE(VK_INCOMPLETE);
	CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
	CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
	CASE(VK_ERROR_INITIALIZATION_FAILED);
	CASE(VK_ERROR_DEVICE_LOST);
	CASE(VK_ERROR_MEMORY_MAP_FAILED);
	CASE(VK_ERROR_LAYER_NOT_PRESENT);
	CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
	CASE(VK_ERROR_FEATURE_NOT_PRESENT);
	CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
	CASE(VK_ERROR_TOO_MANY_OBJECTS);
	CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
	CASE(VK_ERROR_FRAGMENTED_POOL);
	CASE(VK_ERROR_UNKNOWN);
	CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
	CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
	CASE(VK_ERROR_FRAGMENTATION);
	CASE(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
	CASE(VK_PIPELINE_COMPILE_REQUIRED);
	CASE(VK_ERROR_SURFACE_LOST_KHR);
	CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
	CASE(VK_SUBOPTIMAL_KHR);
	CASE(VK_ERROR_OUT_OF_DATE_KHR);
	CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
	CASE(VK_ERROR_VALIDATION_FAILED_EXT);
	CASE(VK_ERROR_INVALID_SHADER_NV);
	CASE(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
	CASE(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
	CASE(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
	CASE(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
	CASE(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
	CASE(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
	CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
	CASE(VK_ERROR_NOT_PERMITTED_KHR);
	CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
	CASE(VK_THREAD_IDLE_KHR);
	CASE(VK_THREAD_DONE_KHR);
	CASE(VK_OPERATION_DEFERRED_KHR);
	CASE(VK_OPERATION_NOT_DEFERRED_KHR);
	CASE(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
	CASE(VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT);
	}
	return "Unknown VkResult";
}
#define CHECKVK(E) (!(E)||LOG_ERROR(vkerror2str(E)))
static VkBool32 VKAPI_CALL debugcallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *callbackdata, void *pUserData)
{
	//if(messageSeverity>=messageseverity)
	{
		const char *severity=0, *type=0;
		switch(messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:	severity="Verbose";	break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:		severity="Info";	break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:	severity="Warning";	break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:		severity="Error";	break;
		}
		switch(messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:		type="General";		break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:	type="Violation";	break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:	type="Performance";	break;
		}
		console_start();
		printf("Validation %s %s:\t", type, severity);
		printf("%s\n", callbackdata->pMessage);

		if(messageSeverity==VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			LOG_ERROR("Aborting\n\n%s", callbackdata->pMessage);
	}
	return VK_FALSE;
}
#define LOAD_FUNC(FUNC) PFN_##FUNC FUNC=(PFN_##FUNC)vkGetInstanceProcAddr(instance, #FUNC)


typedef struct UniformBufferObjectStruct
{
	float model[16], view[16], proj[16], delta, pointsize;
} UniformBufferObject;

typedef struct SdftextUBOStruct
{
	float txt[4], bk[4], zoom;
} SdftextUBO;
typedef struct SdftextHostStyleStruct
{
	int txt, bk;
	float zoom;
} SdftextHostStyle;
static int sdftext_pendingupdate=0;
static SdftextUBO sdftextubo={0};
static SdftextHostStyle sdftextstyle={0};
float const *const font_zoom=&sdftextstyle.zoom;
int set_textcolor(int color)
{
	if(color!=sdftextstyle.txt)
	{
		int temp;
		SWAPVAR(sdftextstyle.txt, color, temp);

		unsigned char *p=(unsigned char*)&sdftextstyle.txt;
		const float gain=1.f/255;

		sdftextubo.txt[0]=p[0]*gain;
		sdftextubo.txt[1]=p[1]*gain;
		sdftextubo.txt[2]=p[2]*gain;
		sdftextubo.txt[3]=p[3]*gain;
		sdftext_pendingupdate=1;
	}
	return color;
}
int set_textbk(int color)
{
	if(color!=sdftextstyle.bk)
	{
		int temp;
		SWAPVAR(sdftextstyle.bk, color, temp);

		unsigned char *p=(unsigned char*)&sdftextstyle.bk;
		const float gain=1.f/255;

		sdftextubo.bk[0]=p[0]*gain;
		sdftextubo.bk[1]=p[1]*gain;
		sdftextubo.bk[2]=p[2]*gain;
		sdftextubo.bk[3]=p[3]*gain;
		sdftext_pendingupdate=1;
	}
	return color;
}
float set_textzoom(float zoom)
{
	zoom=CLAMP(0.25f, zoom, 64);
	if(zoom!=sdftextstyle.zoom)
	{
		extern float sdf_txh, sdf_slope;
		float temp;
		SWAPVAR(sdftextstyle.zoom, zoom, temp);
		
		sdftextubo.zoom=sdftextstyle.zoom*16.f/(sdf_txh*sdf_slope);
		sdftext_pendingupdate=1;
	}
	return zoom;
}
void set_region(Region *r)
{
	//printf("Setting Region\n");//
	//viewport.x=r->rx0;
	//viewport.y=r->ry0;
	//viewport.width=r->rdx;
	//viewport.height=r->rdy;
	//scissor.offset.x=r->cx0;
	//scissor.offset.y=r->cy0;
	//scissor.extent.width=r->cdx;
	//scissor.extent.height=r->cdy;
	memcpy(&region, r, sizeof(region));
}


#if 0
typedef struct VertexStruct
{
	float
		x, y, z,
		r, g, b,
		tx, ty;
} Vertex;
static const Vertex vertexdata[]=
{
	{-0.5f, -0.5f, 0,		1, 0, 0,		1, 0},//0
	{ 0.5f, -0.5f, 0,		0, 1, 0,		0, 0},//1
	{ 0.5f,  0.5f, 0,		0, 0, 1,		0, 1},//2
	{-0.5f,  0.5f, 0,		1, 1, 1,		1, 1},//3

	{-0.5f, -0.5f, -0.5,	1, 0, 0,		1, 0},//4
	{ 0.5f, -0.5f, -0.5,	0, 1, 0,		0, 0},//5
	{ 0.5f,  0.5f, -0.5,	0, 0, 1,		0, 1},//6
	{-0.5f,  0.5f, -0.5,	1, 1, 1,		1, 1},//7

	//{ 0   , -0.5f,		1, 0, 0},
	//{ 0.5f,  0.5f,		0, 1, 0},
	//{-0.5f,  0.5f,		0, 0, 1},
};
static const unsigned short indexdata[]=
{
	0, 1, 2,
	2, 3, 0,

	4, 5, 6,
	6, 7, 4,
};
static void getbindingdescription(VkVertexInputBindingDescription *desc)
{
	desc->binding=0;
	desc->stride=sizeof(Vertex);
	desc->inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
}
static void getattributedescriptions(VkVertexInputAttributeDescription *desc)
{
	desc->binding=0;
	desc->location=0;
	desc->format=VK_FORMAT_R32G32B32_SFLOAT;
	desc->offset=offsetof(Vertex, x);
	++desc;

#if 1
	desc->binding=0;
	desc->location=1;
	desc->format=VK_FORMAT_R32G32B32_SFLOAT;
	desc->offset=offsetof(Vertex, r);
	++desc;
#endif

	desc->binding=0;
	desc->location=2;
	desc->format=VK_FORMAT_R32G32_SFLOAT;
	desc->offset=offsetof(Vertex, tx);
	++desc;
}
#endif
unsigned findMemoryType(unsigned typeFilter, VkMemoryPropertyFlags properties)
{
	static int initialized=0;
	if(!initialized)
	{
		vkGetPhysicalDeviceMemoryProperties(physicaldevice, &physicaldevicememproperties);
		initialized=1;
	}
	for (int k=0;k<(int)physicaldevicememproperties.memoryTypeCount;++k)
	{
		if((typeFilter&1<<k) && (physicaldevicememproperties.memoryTypes[k].propertyFlags&properties)==properties)
			return k;
	}
	LOG_ERROR("Could not find memory type");
	return -1;
}
VkFormat findsupportedformat(VkFormat *candidates, int count, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for(int k=0;k<count;++k)
	{
		VkFormat format=candidates[k];
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicaldevice, format, &props);
		if(tiling==VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures&features)==features)
			return format;
		if(tiling==VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures&features)==features)
			return format;
	}
	LOG_ERROR("No supported format");
	return VK_FORMAT_UNDEFINED;
}
VkSampleCountFlagBits getMaxUsableSampleCount()
{
	VkSampleCountFlags counts=physicaldeviceproperties.limits.framebufferColorSampleCounts&physicaldeviceproperties.limits.framebufferDepthSampleCounts;
	if(counts&VK_SAMPLE_COUNT_64_BIT)return VK_SAMPLE_COUNT_64_BIT;
	if(counts&VK_SAMPLE_COUNT_32_BIT)return VK_SAMPLE_COUNT_32_BIT;
	if(counts&VK_SAMPLE_COUNT_16_BIT)return VK_SAMPLE_COUNT_16_BIT;
	if(counts&VK_SAMPLE_COUNT_8_BIT )return VK_SAMPLE_COUNT_8_BIT;
	if(counts&VK_SAMPLE_COUNT_4_BIT )return VK_SAMPLE_COUNT_4_BIT;
	if(counts&VK_SAMPLE_COUNT_2_BIT )return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
}
VkCommandBuffer begin_singletimecommands()
{
	VkCommandBuffer commandbuffer=0;
	VkCommandBufferAllocateInfo allocinfo=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, 0,
		commandpool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1,//command buffer count
	};
	vkAllocateCommandBuffers(device, &allocinfo, &commandbuffer);
	VkCommandBufferBeginInfo begininfo=
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		0,
	};
	vkBeginCommandBuffer(commandbuffer, &begininfo);
	return commandbuffer;
}
void end_singletimecommands(VkCommandBuffer commandbuffer)
{
	vkEndCommandBuffer(commandbuffer);
	VkSubmitInfo submitinfo=
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO, 0,
		0, 0, 0,
		1, &commandbuffer,
		0, 0,
	};
	vkQueueSubmit(graphicsqueue, 1, &submitinfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsqueue);
	vkFreeCommandBuffers(device, commandpool, 1, &commandbuffer);
}
void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffermemory)
{
	VkResult error;
	VkBufferCreateInfo bufferinfo=
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, 0, 0,
		size,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
	};
	error=vkCreateBuffer(device, &bufferinfo, 0, buffer);	CHECKVK(error);
	VkMemoryRequirements memrequirements;
	vkGetBufferMemoryRequirements(device, *buffer, &memrequirements);
	VkMemoryAllocateInfo memallocinfo=
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, 0,
		memrequirements.size,
		findMemoryType(memrequirements.memoryTypeBits, properties),
	};
	error=vkAllocateMemory(device, &memallocinfo, 0, buffermemory);	CHECKVK(error);
	error=vkBindBufferMemory(device, *buffer, *buffermemory, 0);	CHECKVK(error);
}
void copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize srcoffset, VkDeviceSize dstoffset, VkDeviceSize size)
{
	//VkResult error;
	VkCommandBuffer commandbuffer=begin_singletimecommands();
	VkBufferCopy region=
	{
		srcoffset,
		dstoffset,
		size,
	};
	vkCmdCopyBuffer(commandbuffer, src, dst, 1, &region);
	end_singletimecommands(commandbuffer);
}
void create_stagingbuffer_immediate(const void *src, size_t size, VkBuffer *stagingbuffer, VkDeviceMemory *stagingmemory)
{
	VkResult error;
	create_buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingbuffer, stagingmemory);
		
	//fill the staging buffer
	void *dst;
	error=vkMapMemory(device, *stagingmemory, 0, size, 0, &dst);	CHECKVK(error);
	memcpy(dst, src, size);
	//vkFlushMappedMemoryRanges
	vkUnmapMemory(device, *stagingmemory);
}
void destroy_buffer(VkBuffer *buffer, VkDeviceMemory *memory)//FIXME make buffers share memory
{
	vkDestroyBuffer(device, *buffer, 0);	*buffer=0;
	vkFreeMemory(device, *memory, 0);	*memory=0;
}
void destroy_texture(VkImage *image, VkDeviceMemory *memory, VkImageView *view)
{
	vkDestroyImageView(device, *view, 0);	*view=0;
	vkDestroyImage(device, *image, 0);		*image=0;
	vkFreeMemory(device, *memory, 0);		*memory=0;
}
void create_buffer_immediate(const void *src, size_t size, VkBufferUsageFlagBits usage, VkBuffer *buffer, VkDeviceMemory *buffermemory)
{
	VkBuffer stagingbuffer=0;
	VkDeviceMemory stagingbuffermemory=0;
	create_stagingbuffer_immediate(src, size, &stagingbuffer, &stagingbuffermemory);

	create_buffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT|usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, buffermemory);

	copy_buffer(stagingbuffer, *buffer, 0, 0, size);

	destroy_buffer(&stagingbuffer, &stagingbuffermemory);
}
VkImageView create_imageview(VkImage image, VkFormat format, VkImageAspectFlags aspectflags, int miplevels)
{
	VkImageView imageview=0;
	VkImageViewCreateInfo viewinfo=
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, 0, 0,
		image,
		VK_IMAGE_VIEW_TYPE_2D,
		format,
		{//components
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
		},
		{//subresource range
			aspectflags,
			0,//base mip level
			miplevels,//level count
			0,//base array layer
			1,//layer count
		},
	};
	vkCreateImageView(device, &viewinfo, 0, &imageview);
	return imageview;
}
ArrayHandle create_image(const void *data, int tw, int th, int miplevels, VkSampleCountFlagBits numSamples, VkFormat *format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryproperties, VkImage *textureimage, VkDeviceMemory *texturememory)
{
	VkResult error;
	VkImageCreateInfo imageinfo=
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, 0, 0,
		VK_IMAGE_TYPE_2D, *format, {tw, th, 1},
		miplevels,
		1,//array layers
		numSamples,
		tiling,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0, 0,//queueFamilies
		VK_IMAGE_LAYOUT_UNDEFINED,
	};
	if(miplevels>1)
		imageinfo.usage|=VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	//check if format is supported
	ArrayHandle auxbuf=0;
	//VkFormatProperties formatproperties;
	VkImageFormatProperties formatproperties;
	do
	{
		//vkGetPhysicalDeviceFormatProperties(physicaldevice, *format, &formatproperties);
		error=vkGetPhysicalDeviceImageFormatProperties(physicaldevice, imageinfo.format, imageinfo.imageType, imageinfo.tiling, imageinfo.usage, imageinfo.flags, &formatproperties);//no error checking
		if(error==VK_ERROR_FORMAT_NOT_SUPPORTED)//try to resolve the error
		{
			if(*format==VK_FORMAT_R8_SRGB&&data)//convert to float
			{
				ARRAY_ALLOC(float, auxbuf, 0, (size_t)tw*th, 0, 0);
				float *ptr=(float*)auxbuf->data;
				float gain=1.f/255;
				for(int k=0;k<(int)auxbuf->count;++k)
					ptr[k]=((unsigned char*)data)[k]*gain;
				*format=imageinfo.format=VK_FORMAT_R32_SFLOAT;
			}
			//else if(*format==VK_FORMAT_B8G8R8A8_SRGB&&!data)
			//{
			//	VkFormatProperties properties;
			//	vkGetPhysicalDeviceFormatProperties(physicaldevice, *format, &properties);
			//	printf("Format %d is currently not supported:\n", *format);
			//	printf("  linearTilingFeatures  0x%08X\n", properties.linearTilingFeatures);
			//	printf("  optimalTilingFeatures 0x%08X\n", properties.optimalTilingFeatures);
			//	printf("  bufferFeatures        0x%08X\n", properties.bufferFeatures);
			//}
			//else if(*format==VK_FORMAT_B8G8R8A8_SRGB&&!data)*format=imageinfo.format=VK_FORMAT_B8G8R8A8_UNORM;
			//else if(*format==VK_FORMAT_B8G8R8A8_UNORM&&!data)*format=imageinfo.format=VK_FORMAT_B8G8R8A8_UINT;
			//else if(*format==VK_FORMAT_B8G8R8A8_UINT&&!data)*format=imageinfo.format=VK_FORMAT_R8G8B8A8_SNORM;
			else
				LOG_ERROR("Unsupported texture format %d", *format);
		}
	}while(error);

	error=vkCreateImage(device, &imageinfo, 0, textureimage);	CHECKVK(error);
	VkMemoryRequirements memrequirements;
	vkGetImageMemoryRequirements(device, *textureimage, &memrequirements);
	VkMemoryAllocateInfo allocinfo=
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, 0,
		memrequirements.size,
		findMemoryType(memrequirements.memoryTypeBits, memoryproperties),//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	};
	error=vkAllocateMemory(device, &allocinfo, 0, texturememory);	CHECKVK(error);
	error=vkBindImageMemory(device, *textureimage, *texturememory, 0);	CHECKVK(error);
	return auxbuf;
}
void transitionimagelayout(VkImage image, VkFormat format, VkImageLayout oldlayout, VkImageLayout newlayout, int miplevels)
{
	VkCommandBuffer commandbuffer=begin_singletimecommands();
	
	VkPipelineStageFlags srcstage=0, dststage=0;
	VkImageMemoryBarrier barrier=
	{
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, 0,
		0,//srcAccessMask
		0,//dstAccessMask
		oldlayout,
		newlayout,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		image,
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,//base mip level
			miplevels,//level count
			0,//base array layer
			1,//layer count
		},
	};
	
	if(newlayout==VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask=VK_IMAGE_ASPECT_DEPTH_BIT;
		if(format==VK_FORMAT_D32_SFLOAT_S8_UINT || format==VK_FORMAT_D24_UNORM_S8_UINT)
			barrier.subresourceRange.aspectMask|=VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	if(oldlayout==VK_IMAGE_LAYOUT_UNDEFINED && newlayout==VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask=0;
		barrier.dstAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;

		srcstage=VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dststage=VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if(oldlayout==VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newlayout==VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask=VK_ACCESS_SHADER_READ_BIT;

		srcstage=VK_PIPELINE_STAGE_TRANSFER_BIT;
		dststage=VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldlayout==VK_IMAGE_LAYOUT_UNDEFINED && newlayout==VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask=0;
		barrier.dstAccessMask=VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		srcstage=VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dststage=VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
		LOG_ERROR("Unsupported layout transition");

	vkCmdPipelineBarrier(commandbuffer, srcstage, dststage, 0,    0, 0,    0, 0,    1, &barrier);

	end_singletimecommands(commandbuffer);
}
void copybuffertoimage(VkBuffer buffer, VkImage image, unsigned width, unsigned height)
{
	VkCommandBuffer commandbuffer=begin_singletimecommands();

	VkBufferImageCopy region=
	{
		0,//offset
		0,//row length (tightly packed)
		0,//buffer image height (tightly packed)
		{//image subresource
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,//mip level
			0,//base array layer
			1,//layer count
		},
		{0, 0, 0},//offset
		{width, height, 1},//extent
	};
	vkCmdCopyBufferToImage(commandbuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	end_singletimecommands(commandbuffer);
}
void generate_mipmaps(VkImage image, VkFormat format, int iw, int ih, int miplevels)
{
	VkFormatProperties properties;
	vkGetPhysicalDeviceFormatProperties(physicaldevice, format, &properties);
	if(!(properties.optimalTilingFeatures&VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		LOG_ERROR("Texture format does not support linear blitting");

	VkCommandBuffer commandbuffer=begin_singletimecommands();

	VkImageMemoryBarrier barrier=
	{
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, 0,
		0,//srcAccessMask
		0,//dstAccessMask
		0,//oldLayout
		0,//newLayout
		VK_QUEUE_FAMILY_IGNORED,//src
		VK_QUEUE_FAMILY_IGNORED,//dst
		image,
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,//baseMipLevel
			1,//levelCount
			0,//baseArrayLayer
			1,//layerCount
		},
	};
	int mipWidth=iw, mipHeight=ih;
	for(int k=1;k<miplevels;++k)
	{
		barrier.subresourceRange.baseMipLevel=k-1;
		barrier.oldLayout=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout=VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask=VK_ACCESS_TRANSFER_READ_BIT;
		vkCmdPipelineBarrier(commandbuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,    0, 0,    0, 0,    1, &barrier);

		VkImageBlit blit=
		{
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				k-1,
				0,
				1,
			},
			{
				{0, 0, 0},
				{mipWidth, mipHeight, 1},
			},
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				k,
				0,
				1,
			},
			{
				{0, 0, 0},
				{mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1},
			},
		};
		vkCmdBlitImage(commandbuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

		barrier.oldLayout=VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask=VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask=VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(commandbuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,    0, 0,    0, 0,    1, &barrier);

		mipWidth>>=mipWidth>1;
		mipHeight>>=mipHeight>1;
	}

	barrier.subresourceRange.baseMipLevel=miplevels-1;
	barrier.oldLayout=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask=VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(commandbuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,    0, 0,    0, 0,    1, &barrier);
	
	end_singletimecommands(commandbuffer);
}
void create_texture_immediate(Texture *tx, VkFormat *format, int miplevels, VkImage *textureimage, VkDeviceMemory *texturememory)
{
	VkResult error;
	VkBuffer stagingbuffer=0;
	VkDeviceMemory stagingbuffermemory=0;
	size_t size;
	
	ArrayHandle auxbuf=create_image(tx->data, tx->iw, tx->ih, miplevels, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureimage, texturememory);
	if(auxbuf)
		size=auxbuf->count*auxbuf->esize;
	else
		size=(size_t)tx->iw*tx->ih*tx->ch;

	//create staging buffer
	create_buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingbuffer, &stagingbuffermemory);
		
	//fill the staging buffer
	void *dst;
	error=vkMapMemory(device, stagingbuffermemory, 0, size, 0, &dst);	CHECKVK(error);
	if(auxbuf)
		memcpy(dst, auxbuf->data, size);
	else
		memcpy(dst, tx->data, size);
	//vkFlushMappedMemoryRanges
	vkUnmapMemory(device, stagingbuffermemory);

	transitionimagelayout(*textureimage, *format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, miplevels);
	copybuffertoimage(stagingbuffer, *textureimage, tx->iw, tx->ih);
	if(miplevels==1)
		transitionimagelayout(*textureimage, *format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, miplevels);
	else
		generate_mipmaps(*textureimage, *format, tx->iw, tx->ih, miplevels);
	
	destroy_buffer(&stagingbuffer, &stagingbuffermemory);
	if(auxbuf)
		array_free(&auxbuf);
}

//3D
void mat4_lookAt(float *dst, const float *cam, const float *center, const float *up)
{
	float f[3], t1, u[3], s[3];
	vec3_sub(f, center, cam);
	vec3_normalize(f, f, t1);
	vec3_normalize(u, up, t1);
	vec3_cross(s, f, u);
	vec3_normalize(s, s, t1);
	vec3_cross(u, s, f);
	dst[0]=s[0], dst[4]=s[1], dst[8]=s[2], dst[12]=-vec3_dot(s, cam);
	dst[1]=u[0], dst[5]=u[1], dst[9]=u[2], dst[13]=-vec3_dot(u, cam);
	dst[2]=f[0], dst[6]=f[1], dst[10]=f[2], dst[14]=-vec3_dot(f, cam);
	dst[3]=0, dst[7]=0, dst[11]=0, dst[15]=1;
}
void mat4_FPSView(float *dst, const float *campos, float yaw, float pitch)
{
	float _cam[]={-campos[1], campos[2], -campos[0]};
	//float _cam[]={campos[0], campos[1], campos[2]};
	float cos_p=cosf(pitch), sin_p=sinf(pitch), cos_y=cosf(yaw), sin_y=sinf(yaw);
	float
		xaxis[]={cos_y, 0, -sin_y},
		yaxis[]={sin_y*sin_p, cos_p, cos_y*sin_p},
		zaxis[]={sin_y*cos_p, -sin_p, cos_p*cos_y};

	dst[0]=xaxis[2], dst[1]=yaxis[2], dst[2]=zaxis[2], dst[3]=0;
	dst[4]=xaxis[0], dst[5]=yaxis[0], dst[6]=zaxis[0], dst[7]=0;
	dst[8]=xaxis[1], dst[9]=yaxis[1], dst[10]=zaxis[1], dst[11]=0;

	//dst[0]=xaxis[0], dst[1]=yaxis[0], dst[2]=zaxis[0], dst[3]=0;
	//dst[4]=xaxis[1], dst[5]=yaxis[1], dst[6]=zaxis[1], dst[7]=0;
	//dst[8]=xaxis[2], dst[9]=yaxis[2], dst[10]=zaxis[2], dst[11]=0;

	dst[12]=-vec3_dot(xaxis, _cam), dst[13]=-vec3_dot(yaxis, _cam), dst[14]=-vec3_dot(zaxis, _cam), dst[15]=1;
}
void mat4_perspective(float *dst, float tanfov, float w_by_h, float znear, float zfar)
{
	dst[0]=1/tanfov,	dst[1]=0,				dst[2]=0,							dst[3]=0;
	dst[4]=0,			dst[5]=w_by_h/tanfov,	dst[6]=0,							dst[7]=0;
	dst[8]=0,			dst[9]=0,				dst[10]=-(zfar+znear)/(zfar-znear), dst[11]=-1;
	dst[12]=0,			dst[13]=0,				dst[14]=-2*zfar*znear/(zfar-znear), dst[15]=0;
}
void GetTransformInverseNoScale(float *dst, const float *src)// Requires this matrix to be transform matrix, NoScale version requires this matrix be of scale 1
{
	//https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
#define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)        VecSwizzleMask(vec, MakeShuffleMask(x,y,z,w))
#define VecSwizzle1(vec, x)                VecSwizzleMask(vec, MakeShuffleMask(x,x,x,x))
// special swizzle
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)
	__m128 inM[4], ret[4];
	
	inM[0]=_mm_loadu_ps(src);
	inM[1]=_mm_loadu_ps(src+4);
	inM[2]=_mm_loadu_ps(src+8);
	inM[3]=_mm_loadu_ps(src+12);

	// transpose 3x3, we know m03 = m13 = m23 = 0
	__m128 t0 = VecShuffle_0101(inM[0], inM[1]); // 00, 01, 10, 11
	__m128 t1 = VecShuffle_2323(inM[0], inM[1]); // 02, 03, 12, 13
	ret[0] = VecShuffle(t0, inM[2], 0,2,0,3); // 00, 10, 20, 23(=0)
	ret[1] = VecShuffle(t0, inM[2], 1,3,1,3); // 01, 11, 21, 23(=0)
	ret[2] = VecShuffle(t1, inM[2], 0,2,2,3); // 02, 12, 22, 23(=0)

	// last line
	ret[3] =                    _mm_mul_ps(ret[0], VecSwizzle1(inM[3], 0));
	ret[3] = _mm_add_ps(ret[3], _mm_mul_ps(ret[1], VecSwizzle1(inM[3], 1)));
	ret[3] = _mm_add_ps(ret[3], _mm_mul_ps(ret[2], VecSwizzle1(inM[3], 2)));
	ret[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), ret[3]);

	_mm_storeu_ps(dst, ret[0]);
	_mm_storeu_ps(dst+4, ret[1]);
	_mm_storeu_ps(dst+8, ret[2]);
	_mm_storeu_ps(dst+12, ret[3]);
#undef MakeShuffleMask
#undef VecSwizzleMask
#undef VecSwizzle
#undef VecSwizzle1
#undef VecSwizzle_0022
#undef VecSwizzle_1133
#undef VecShuffle
#undef VecShuffle_0101
#undef VecShuffle_2323
}
void mat4_normalmat3(float *dst, float *m4)//inverse transpose of top left 3x3 submatrix
{
	float temp[16];
	GetTransformInverseNoScale(temp, m4);
	vec3_copy(dst, temp);
	vec3_copy(dst+3, temp+4);
	vec3_copy(dst+6, temp+8);
}

void update_uniforms(unsigned currentframe)
{
	static double t_start=0, t_prev=0;
	double t=time_ms();
	if(!t_start)
		t_start=t;
	if(!t_prev)
		t_prev=t;
	//double time=(time_ms()-t_start)*0.001;

	UniformBufferObject ubo;

	mat4_identity(ubo.model, 1);

	//float temp[16], angle=(float)(time*(M_PI*0.5)), dir[]={0, 0, 1}, temp2[2], temp3[3], temp4[3];
	//mat4_identity(temp, 1);
	//mat4_rotate(ubo.model, temp, angle, dir, temp2, temp3, temp4);

	mat4_FPSView(ubo.view, &cam.x, cam.ax, cam.ay);
	mat4_perspective(ubo.proj, cam.tanfov, (float)w/h, 0.1f, 1000.f);

	memcpy(uniformbuffersmapped[currentframe], &ubo, sizeof(ubo));
	ubo.delta=0.00001f;
	ubo.pointsize=14.f;
	//ubo.delta=t-t_prev;

	//if(sdftext_pendingupdate)
	{
		memcpy(sdftext_uniformmap[currentframe], &sdftextubo, sizeof(sdftextubo));
		sdftext_pendingupdate=0;
	}

	t_prev=t;
}


int check_layerssupport(const char **reqlayers, int nreqlayers)
{
	ArrayHandle layers;
	unsigned count=0;
	vkEnumerateInstanceLayerProperties(&count, 0);
	ARRAY_ALLOC(VkLayerProperties, layers, 0, count, 0, 0);
	vkEnumerateInstanceLayerProperties(&count, (VkLayerProperties*)layers->data);
#if 0
	printf("Available device layers:\n");
	for(int kl=0;kl<(int)layers->count;++kl)
	{
		VkLayerProperties *layer=(VkLayerProperties*)array_at(&layers, kl);
		printf("%s:\t%s\n", layer->layerName, layer->description);
	}
	printf("\n");
#endif
	for(int kr=0;kr<nreqlayers;++kr)
	{
		int found=0;
		for(int kl=0;kl<(int)layers->count;++kl)
		{
			VkLayerProperties *layer=(VkLayerProperties*)array_at(&layers, kl);
			if(!strcmp(layer->layerName, reqlayers[kr]))
			{
				found=1;
				break;
			}
		}
		if(!found)
		{
			LOG_ERROR("%s not supported", reqlayers[kr]);
			return 0;
		}
	}
	return 1;
}
void query_swapchainsupport(VkPhysicalDevice physicaldevice, SwapChainSupportDetails *details)
{
	VkResult error;
	unsigned count;

	error=vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicaldevice, surface, &details->capabilities);	CHECKVK(error);
	
	count=0;
	error=vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &count, 0);	CHECKVK(error);
	if(count)
	{
		if(details->formats)
			array_free(&details->formats);
		ARRAY_ALLOC(VkSurfaceFormatKHR, details->formats, 0, count, 0, 0);
		error=vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &count, (VkSurfaceFormatKHR*)details->formats->data);	CHECKVK(error);

		//printf("Available surface formats:\n");//
		//for(int k=0;k<(int)details->formats->count;++k)
		//{
		//	VkSurfaceFormatKHR *format=(VkSurfaceFormatKHR*)array_at(&details->formats, k);
		//	printf("\t%d\n", format->format);
		//}
		//printf("End\n\n");//
	}

	count=0;
	error=vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &count, 0);	CHECKVK(error);
	if(count)
	{
		if(details->presentmodes)
			array_free(&details->presentmodes);
		ARRAY_ALLOC(VkPresentModeKHR, details->presentmodes, 0, count, 0, 0);
		error=vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &count, (VkPresentModeKHR*)details->presentmodes->data);	CHECKVK(error);
	}
}

int compile_shader(const char *searchpath, const char *srcname, const char *dstname)//requires Vulkan SDK to run
{
	ArrayHandle filepath;
	STR_COPY(filepath, srcname, strlen(srcname));
	ptrdiff_t size=get_filesize((char*)filepath->data);
	if(size==-1)
	{
		array_insert(&filepath, 0, searchpath, strlen(searchpath), 1, 1);
		size=get_filesize((char*)filepath->data);
		if(size==-1)
		{
			LOG_ERROR("Couldn't find %s", srcname);
			return 0;
		}
	}
	snprintf(g_buf, G_BUF_SIZE, "glslc \"%s\" -o %s", (char*)filepath->data, dstname);
	int result=system(g_buf);
	if(result)
	{
		LOG_ERROR("Compilation of %s failed", srcname);
		return 0;
	}
	return 1;
}
VkShaderModule create_shadermodule(ArrayHandle spv, const char *name)
{
	unsigned error;
	VkShaderModule shader=0;
	if(!spv)
	{
		LOG_ERROR("Missing shader %s", name);
		return 0;
	}
	VkShaderModuleCreateInfo info=
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, 0, 0,
		spv->count,
		(unsigned*)spv->data,
	};
	error=vkCreateShaderModule(device, &info, 0, &shader);	CHECKVK(error);
	return shader;
}

static int create_swapchain(int w, int h)//also creates image views
{
	VkResult error;
	unsigned count;
	{
		ArrayHandle arr=swapsupport.formats;
		if(!arr||!arr->count)
		{
			LOG_ERROR("No available formats");
			return 0;
		}
		for(int k=0;k<(int)arr->count;++k)
		{
			VkSurfaceFormatKHR *fmt=(VkSurfaceFormatKHR*)array_at(&arr, k);
			if(fmt->format==VK_FORMAT_B8G8R8A8_SRGB&&fmt->colorSpace==VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				surfaceformat=*fmt;
				break;
			}
		}
		if(surfaceformat.format==VK_FORMAT_UNDEFINED)//if not selected, select first option
			surfaceformat=*(VkSurfaceFormatKHR*)array_at(&arr, 0);

		arr=swapsupport.presentmodes;
		if(!arr||!arr->count)
		{
			LOG_ERROR("No present modes");
			return 0;
		}
		for(int k=0;k<(int)arr->count;++k)
		{
			VkPresentModeKHR *mode=(VkPresentModeKHR*)array_at(&arr, k);
			if(*mode==VK_PRESENT_MODE_MAILBOX_KHR)
			{
				presentmode=*mode;
				break;
			}
		}
		if(presentmode!=VK_PRESENT_MODE_MAILBOX_KHR)//redundant assignment
			presentmode=VK_PRESENT_MODE_FIFO_KHR;
	}

	if(swapsupport.capabilities.currentExtent.width!=(unsigned)-1)
		extent=swapsupport.capabilities.currentExtent;
	else
	{
		extent.width=CLAMP(swapsupport.capabilities.minImageExtent.width, (unsigned)w, swapsupport.capabilities.maxImageExtent.width);
		extent.height=CLAMP(swapsupport.capabilities.minImageExtent.height, (unsigned)h, swapsupport.capabilities.maxImageExtent.height);
	}
	unsigned imagecount=swapsupport.capabilities.minImageCount+1;
	if(swapsupport.capabilities.maxImageCount&&imagecount>swapsupport.capabilities.maxImageCount)
		imagecount=swapsupport.capabilities.maxImageCount;
	unsigned indices[]=
	{
		graphicsqueuefamily,
		presentqueuefamily,
	};
	VkSwapchainCreateInfoKHR swapchaincreateinfo=
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, 0, 0,
		surface,
		imagecount,
		surfaceformat.format,
		surfaceformat.colorSpace,
		extent,
		1,//array layers
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		graphicsqueuefamily!=presentqueuefamily?VK_SHARING_MODE_CONCURRENT:VK_SHARING_MODE_EXCLUSIVE,
		(graphicsqueuefamily!=presentqueuefamily)<<1,//queue family index count		optional if indices are equal
		graphicsqueuefamily!=presentqueuefamily?indices:0,
		swapsupport.capabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		presentmode,
		VK_TRUE,//clipped
		VK_NULL_HANDLE,//old swapchain
	};
	error=vkCreateSwapchainKHR(device, &swapchaincreateinfo, 0, &swapchain);	CHECKVK(error);

	vkGetSwapchainImagesKHR(device, swapchain, &count, 0);
	if(images)
		array_free(&images);
	ARRAY_ALLOC(VkImage, images, 0, count, 0, 0);
	vkGetSwapchainImagesKHR(device, swapchain, &count, (VkImage*)images->data);

	
	//create imageviews
	if(imageviews)
		array_free(&imageviews);
	ARRAY_ALLOC(VkImageView, imageviews, 0, images->count, 0, 0);
	for(int k=0;k<(int)imageviews->count;++k)
	{
		VkImageView *imageview=(VkImageView*)array_at(&imageviews, k);
		VkImage *image=(VkImage*)array_at(&images, k);
		*imageview=create_imageview(*image, surfaceformat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
#if 0
		VkImageViewCreateInfo createinfo=
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, 0, 0,
			*image,
			VK_IMAGE_VIEW_TYPE_2D,
			surfaceformat.format,
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
			},
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,//baseMipLevel
				1,//levelCount
				0,//baseArrayLayer
				1,//layerCount
			},
		};
		error=vkCreateImageView(device, &createinfo, 0, imageview);
#endif
	}
	return 1;
}
static void create_framebuffers()//also creates color resources & depth resources
{
	VkResult error;

	//create color resources
	VkFormat colorformat=surfaceformat.format;
	if(frameAAsamples>1)
	{
		//printf("colorimage format %d\n", colorformat);//
		create_image(0, extent.width, extent.height, 1, frameAAsamples, &colorformat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &colorimage, &colorimagememory);
		colorimageview=create_imageview(colorimage, colorformat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}


	//create depth resources

	//printf("depth format %d\n", depthformat);//
	create_image(0, extent.width, extent.height, 1, frameAAsamples, &depthformat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthimage, &depthmemory);
	depthimageview=create_imageview(depthimage, depthformat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	transitionimagelayout(depthimage, depthformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);


	//create framebuffers
	ARRAY_ALLOC(VkFramebuffer, swapchainframebuffers, 0, imageviews->count, 0, 0);
	int enableMSAA=frameAAsamples>1;
	for(int k=0;k<(int)swapchainframebuffers->count;++k)
	{
		VkFramebuffer *fb=(VkFramebuffer*)array_at(&swapchainframebuffers, k);
		VkImageView *imageview=(VkImageView*)array_at(&imageviews, k);
		VkImageView attachements[]=
		{
			colorimageview,
			depthimageview,
			*imageview,
		};
		if(!enableMSAA)
			memreverse(attachements+1, 2, sizeof(VkImageView));
		VkFramebufferCreateInfo info=
		{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, 0, 0,
			renderpass,
			_countof(attachements)-!enableMSAA, attachements+!enableMSAA,
			extent.width,
			extent.height,
			1,//layers
		};
		error=vkCreateFramebuffer(device, &info, 0, fb);	CHECKVK(error);
	}
}
static void cleanup_swapchain()
{
	vkDestroyImageView(device, colorimageview, 0);
    vkDestroyImage(device, colorimage, 0);
    vkFreeMemory(device, colorimagememory, 0);

	vkDestroyImageView(device, depthimageview, 0);
	vkDestroyImage(device, depthimage, 0);
	vkFreeMemory(device, depthmemory, 0);

	for(int k=0;k<(int)swapchainframebuffers->count;++k)
	{
		VkFramebuffer *fb=(VkFramebuffer*)array_at(&swapchainframebuffers, k);
		vkDestroyFramebuffer(device, *fb, 0);	*fb=0;
	}
	for(int k=0;k<(int)imageviews->count;++k)
	{
		VkImageView *imageview=(VkImageView*)array_at(&imageviews, k);
		vkDestroyImageView(device, *imageview, 0);	*imageview=0;
	}
	vkDestroySwapchainKHR(device, swapchain, 0);
}
static void recreate_swapchain()
{
	//printf("Recreating swapchain...\n");
	vkDeviceWaitIdle(device);

	cleanup_swapchain();

	create_swapchain(w, h);
	create_framebuffers();
}

void print_text(VkCommandBuffer cmdbuf)
{
#ifndef DISABLE_TEXT
	VkResult error;
	if(text_vertices&&text_vertices->count)//if there is text to print
	{
		size_t size=text_vertices->count*text_vertices->esize;
		if(!sdftext_vertexbuffer||sdftext_bufsize<size)//grow text buffer if doesn't exist or too small
		{
			if(sdftext_vertexbuffer)
			{
				vkDestroyBuffer(device, sdftext_vertexbuffer, 0);	sdftext_vertexbuffer=0;
			}
			VkBufferCreateInfo bufferinfo=
			{
				VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, 0, 0,
				size,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_SHARING_MODE_EXCLUSIVE,
			};
			error=vkCreateBuffer(device, &bufferinfo, 0, &sdftext_vertexbuffer);	CHECKVK(error);

			VkMemoryRequirements memrequirements;
			vkGetBufferMemoryRequirements(device, sdftext_vertexbuffer, &memrequirements);
			if(!sdftext_vertexmemory||sdftext_memsize<memrequirements.size)//grow text memory if not allocated or too small
			{
				if(sdftext_vertexmemory)
				{
					vkFreeMemory(device, sdftext_vertexmemory, 0);
					sdftext_vertexmemory=0;
				}
				VkMemoryAllocateInfo memallocinfo=
				{
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, 0,
					memrequirements.size,
					findMemoryType(memrequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
				};
				error=vkAllocateMemory(device, &memallocinfo, 0, &sdftext_vertexmemory);	CHECKVK(error);
				sdftext_memsize=memrequirements.size;
			}
			error=vkBindBufferMemory(device, sdftext_vertexbuffer, sdftext_vertexmemory, 0);	CHECKVK(error);
			error=vkMapMemory(device, sdftext_vertexmemory, 0, size, 0, &sdftext_vertexmap);	CHECKVK(error);
			sdftext_bufsize=size;
		}
		memcpy(sdftext_vertexmap, text_vertices->data, size);

		vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipelines[1]);
		VkDeviceSize offset=0;
		vkCmdBindVertexBuffers(cmdbuf, 0, 1, &sdftext_vertexbuffer, &offset);
		vkCmdDraw(cmdbuf, (int)text_vertices->count, 1, 0, 0);
	}
#endif
}
int vkw_init(const char *searchpath, Texture *atlas, Texture *texture, ArrayHandle vertices, ArrayHandle indices, ArrayHandle particles, int anisotropyenable, int miplevels, int enableMSAA)
{
	VkResult error;
	unsigned count;

#if 0
	{
		console_start();
		count=0;
		error=vkEnumerateInstanceExtensionProperties(0, &count, 0);	CHECKVK(error);
		ArrayHandle extensions;
		ARRAY_ALLOC(VkExtensionProperties, extensions, 0, count, 0, 0);
		error=vkEnumerateInstanceExtensionProperties(0, &count, (VkExtensionProperties*)extensions->data);	CHECKVK(error);
		printf("Available Vulkan extensions:\n");
		for(unsigned k=0;k<count;++k)
		{
			VkExtensionProperties *ext=(VkExtensionProperties*)array_at(&extensions, k);
			printf("%s\t%d.%d.%d\n", ext->extensionName, VK_VERSION_MAJOR(ext->specVersion), VK_VERSION_MINOR(ext->specVersion), VK_VERSION_PATCH(ext->specVersion));
		}
		printf("\n");
		array_free(&extensions);
	}
#endif

	//step 1: create Vulkan instance

	//https://harrylovescode.gitbooks.io/vulkan-api/content/chap05/chap05-windows.html
	VkApplicationInfo appinfo=
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO, 0,
		"vksandbox", VK_MAKE_VERSION(0, 0, 1),
		"enginenamehere", VK_MAKE_VERSION(0, 0, 1),
		VK_API_VERSION_1_3,
	};
	const char *layernames[]=
	{
#ifdef ENABLE_VALIDATION
		"VK_LAYER_KHRONOS_validation",
#endif
		0,
	};
	const char *extensionnames[]=
	{
		"VK_KHR_surface",
		"VK_KHR_win32_surface",
#ifdef ENABLE_VALIDATION
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
		0,
	};
#ifdef ENABLE_VALIDATION
	if(!check_layerssupport(layernames, _countof(layernames)-1))
		return 0;
	VkDebugUtilsMessengerCreateInfoEXT debugutilsmessengercreateinfo=
	{
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT, 0, 0,
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
		debugcallback, 0,
	};
#endif
	VkInstanceCreateInfo instancecreateinfo=
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifdef ENABLE_VALIDATION
		&debugutilsmessengercreateinfo,
#else
		0,
#endif
		0,
		&appinfo,
		_countof(layernames)-1, layernames,
		_countof(extensionnames)-1, extensionnames,
	};
	error=vkCreateInstance(&instancecreateinfo, 0, &instance);	CHECKVK(error);
#ifdef ENABLE_VALIDATION
	{
		LOAD_FUNC(vkCreateDebugUtilsMessengerEXT);
		if(!vkCreateDebugUtilsMessengerEXT)
		{
			LOG_ERROR("vkCreateDebugUtilsMessengerEXT() not loaded");
			return 0;
		}
		error=vkCreateDebugUtilsMessengerEXT(instance, &debugutilsmessengercreateinfo, 0, &messenger);	CHECKVK(error);
	}
#endif

	
	//step 2: create render surface		https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Window_surface
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo=
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, 0, 0,
		(HINSTANCE)get_hinstance(),//hinstance
		(HWND)get_hwnd(),//hwnd
	};
	error=vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, 0, &surface);	CHECKVK(error);


	//step 3: setup device			step 3.1: select physical device
	const char *deviceexts[]=
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		0,
	};
	count=0;
	error=vkEnumeratePhysicalDevices(instance, &count, 0);	CHECKVK(error);
	if(!count)
	{
		LOG_ERROR("No physical devices");
		return 0;
	}
	{
		ArrayHandle physicaldevices;
		ARRAY_ALLOC(VkPhysicalDevice, physicaldevices, 0, count, 0, 0);
		error=vkEnumeratePhysicalDevices(instance, &count, (VkPhysicalDevice*)physicaldevices->data);	CHECKVK(error);
		for(int k=0;k<(int)physicaldevices->count;++k)//physical device selection loop
		{
			int valid=1;
			VkPhysicalDevice *pd=(VkPhysicalDevice*)array_at(&physicaldevices, k);

			//VkPhysicalDeviceFeatures features;
			//vkGetPhysicalDeviceFeatures(*pd, &features);

			unsigned extcount=0;
			vkEnumerateDeviceExtensionProperties(*pd, 0, &extcount, 0);
			ArrayHandle availableexts;
			ARRAY_ALLOC(VkExtensionProperties, availableexts, 0, extcount, 0, 0);
			vkEnumerateDeviceExtensionProperties(*pd, 0, &extcount, (VkExtensionProperties*)availableexts->data);
#if 0
			printf("Device extensions:\n");
			for(int k3=0;k3<(int)availableexts->count;++k3)
			{
				VkExtensionProperties *ep=(VkExtensionProperties*)array_at(&availableexts, k3);
				printf("%s\t\t%d.%d.%d\n", ep->extensionName, VK_VERSION_MAJOR(ep->specVersion), VK_VERSION_MINOR(ep->specVersion), VK_VERSION_PATCH(ep->specVersion));
			}
			printf("\n");
#endif
			for(int k2=0;k2<_countof(deviceexts)-1;++k2)
			{
				int found=0;
				for(int k3=0;k3<(int)availableexts->count;++k3)
				{
					VkExtensionProperties *ep=(VkExtensionProperties*)array_at(&availableexts, k3);
					if(!strcmp(ep->extensionName, deviceexts[k2]))
					{
						found=1;
						break;
					}
				}
				if(!found)
				{
					console_start();
					printf("Device extension %s not found", deviceexts[k2]);
					valid=0;
				}
				//	LOG_ERROR("Device extension %s not found", deviceexts[k2]);
			}
			array_free(&availableexts);
			if(!valid)
				continue;

			query_swapchainsupport(*pd, &swapsupport);
			if(!swapsupport.formats||!swapsupport.presentmodes)
				continue;

			vkGetPhysicalDeviceProperties(*pd, &physicaldeviceproperties);
			if(physicaldeviceproperties.deviceType==VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU||physicaldeviceproperties.deviceType==VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			{
				count=k;
				break;
			}
		}
		if(count>=physicaldevices->count)
		{
			LOG_ERROR("No physical devices");
			return 0;
		}
		physicaldevice=*(VkPhysicalDevice*)array_at(&physicaldevices, count);
		array_free(&physicaldevices);
		maxAAsamples=getMaxUsableSampleCount();
		frameAAsamples=enableMSAA?maxAAsamples:1;
	}
	
	//step 3.1: select queue family index/indices
	count=0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicaldevice, &count, 0);
	if(!count)
	{
		LOG_ERROR("No queue families");
		return 0;
	}
	{
		ArrayHandle queuefamilies;
		ARRAY_ALLOC(VkQueueFamilyProperties, queuefamilies, 0, count, 0, 0);
		vkGetPhysicalDeviceQueueFamilyProperties(physicaldevice, &count, (VkQueueFamilyProperties*)queuefamilies->data);
		graphicsqueuefamily=-1;
		presentqueuefamily=-1;
		for(int k=0;k<(int)queuefamilies->count;++k)
		{
			VkQueueFamilyProperties *qf=(VkQueueFamilyProperties*)array_at(&queuefamilies, k);
			if(qf->queueFlags&VK_QUEUE_GRAPHICS_BIT)
				graphicsqueuefamily=k;
#ifdef ENABLE_COMPUTE
			if((qf->queueFlags&VK_QUEUE_GRAPHICS_BIT)&&(qf->queueFlags&VK_QUEUE_COMPUTE_BIT))
				graphicsandcomputequeuefamily=k;
#endif

			VkBool32 supportspresent=0;
			error=vkGetPhysicalDeviceSurfaceSupportKHR(physicaldevice, k, surface, &supportspresent);	CHECKVK(error);
			if(supportspresent)
				presentqueuefamily=k;
		}
		if(graphicsqueuefamily==-1||presentqueuefamily==-1)
		{
			LOG_ERROR("queue indices: graphics=%d, present=%d", graphicsqueuefamily, presentqueuefamily);
			return 0;
		}
		array_free(&queuefamilies);
	}

	//step 3.2: create (logical) device
	float priority=1;
	VkDeviceQueueCreateInfo queuecreateinfo[]=
	{
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, 0, 0,
			graphicsqueuefamily, 1, &priority,
		},
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, 0, 0,
			presentqueuefamily, 1, &priority,
		},
	};
	VkPhysicalDeviceFeatures devicefeatures={0};

	if(anisotropyenable)
		devicefeatures.samplerAnisotropy=VK_TRUE;
	if(enableMSAA)
		devicefeatures.sampleRateShading=VK_TRUE;

	VkDeviceCreateInfo devicecreateinfo=
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, 0, 0,
		1+(graphicsqueuefamily!=presentqueuefamily), queuecreateinfo,//pass two structs if queue indices are different
		_countof(layernames)-1, layernames,//device layers (ignored in modern Vulkan)
		_countof(deviceexts)-1, deviceexts,//device extensions
		&devicefeatures,
	};
	error=vkCreateDevice(physicaldevice, &devicecreateinfo, 0, &device);	CHECKVK(error);
	if(!device)
	{
		LOG_ERROR("Couldn't create logical device");
		return 0;
	}
	vkGetDeviceQueue(device, graphicsqueuefamily, 0, &graphicsqueue);
	vkGetDeviceQueue(device, presentqueuefamily, 0, &presentqueue);
#ifdef ENABLE_COMPUTE
	vkGetDeviceQueue(device, graphicsandcomputequeuefamily, 0, &computequeue);
#endif


	//step 4: create swapchain & imageviews
	create_swapchain(w, h);


	//step 5: create render pass
	{
		VkFormat candidates[]=
		{
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
		};
		depthformat=findsupportedformat(candidates, _countof(candidates), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}
	VkAttachmentDescription attachements[]=
	{
		{//color attachement
			0,
			surfaceformat.format,
			frameAAsamples,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,

			enableMSAA?VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		},
		{//depth attachement
			0,
			depthformat,
			frameAAsamples,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		},
		{//color attachement (resolves MSAA to regular image)
			0,
			surfaceformat.format,
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		},
	};

	VkAttachmentReference colorattachementref=
	{
		0,//attachement
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,//layout
	};
	VkAttachmentReference depthattachementref=
	{
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};
	VkAttachmentReference resolveattachementref=
	{
		2,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	VkSubpassDescription subpass=
	{
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0, 0,//input attachements
		1, &colorattachementref, enableMSAA?&resolveattachementref:0, &depthattachementref,//color attachements
		0, 0,//preserve attachements
	};

	VkSubpassDependency dependency=
	{
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		0,
	};

	VkRenderPassCreateInfo renderpassinfo=
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, 0, 0,
		_countof(attachements)-!enableMSAA, attachements,
		1, &subpass,
		1, &dependency,
	};
	error=vkCreateRenderPass(device, &renderpassinfo, 0, &renderpass);	CHECKVK(error);


	//step 6: create descriptor set layout
	{
		VkDescriptorSetLayoutBinding layoutbindings[]=				//uniform info
		{
			{//3D uniform
				0,//binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,//descriptor count
				VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_COMPUTE_BIT,
				0,//immutable samplers
			},
			{//3D texture
				1,//binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,//descriptor count
				VK_SHADER_STAGE_FRAGMENT_BIT,
				0,//immutable samplers
			},

#ifdef ENABLE_COMPUTE
			{//compute particles
				2,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				0,
			},
			{//compute particles
				3,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				0,
			},
#endif

#ifndef DISABLE_TEXT
			{//sdftext uniform
				4,//binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,//descriptor count
				VK_SHADER_STAGE_FRAGMENT_BIT,
				0,//immutable samplers
			},
			{//sdftext atlas
				5,//binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				1,//descriptor count
				VK_SHADER_STAGE_FRAGMENT_BIT,
				0,//immutable samplers
			},
#endif
		};
		VkDescriptorSetLayoutCreateInfo layoutinfo=
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, 0, 0,
			_countof(layoutbindings), layoutbindings,
		};
		error=vkCreateDescriptorSetLayout(device, &layoutinfo, 0, &descriptorsetlayout);	CHECKVK(error);
	}


	//step 7: create graphics & compute pipelines	(pipelinelayout then pipeline)
	VkPipelineLayoutCreateInfo pipelinelayoutinfo=
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, 0, 0,
		1, &descriptorsetlayout,//set layouts
		0, 0,//push constant ranges
	};
	error=vkCreatePipelineLayout(device, &pipelinelayoutinfo, 0, &pipelinelayout);	CHECKVK(error);
	
	//load shaders
	const char *shadernames[]=
	{
		"bin_3D_vert.spv",
		"bin_3D_frag.spv",
		"bin_comp.spv",
		"bin_sdftext_vert.spv",
		"bin_sdftext_frag.spv",
	};
#ifdef COMPILE_SHADERS
	const char *srcnames[]=
	{
		"shader_3D.vert",
		"shader_3D.frag",
		"shader.comp",
		"shader_sdftext.vert",
		"shader_sdftext.frag",
	};
	for(int k=0;k<_countof(shaders);++k)
		compile_shader(searchpath, srcnames[k], shadernames[k]);
#endif
	for(int k=0;k<_countof(shaders);++k)
	{
		ArrayHandle spv;

		spv=load_file(shadernames[k], 1, 0, 0);
		shaders[k]=create_shadermodule(spv, shadernames[k]);
		array_free(&spv);
	}
	VkPipelineShaderStageCreateInfo shaderstageinfo[]=
	{
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, 0, 0,
			VK_SHADER_STAGE_VERTEX_BIT, shaders[0], "main", 0,
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, 0, 0,
			VK_SHADER_STAGE_FRAGMENT_BIT, shaders[1], "main", 0,
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, 0, 0,
			VK_SHADER_STAGE_COMPUTE_BIT, shaders[2], "main", 0,
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, 0, 0,
			VK_SHADER_STAGE_VERTEX_BIT, shaders[3], "main", 0,
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, 0, 0,
			VK_SHADER_STAGE_FRAGMENT_BIT, shaders[4], "main", 0,
		},
	};
	VkVertexInputBindingDescription bindingdescriptions[]=			//attribute info
	{
		{//3D
			0,//binding
			sizeof(Vertex),//stride
			VK_VERTEX_INPUT_RATE_VERTEX,
		},
#ifndef DISABLE_TEXT
		{//sdftext
			0,//binding
			sizeof(float[4]),//stride
			VK_VERTEX_INPUT_RATE_VERTEX,
		},
#endif
	};
	VkVertexInputAttributeDescription attributedescriptions_3D[]=
	{
		{//3D position
			0,//location
			0,//binding
			VK_FORMAT_R32G32B32_SFLOAT,//format
			offsetof(Vertex, x),//offset
		},
		{//3D vertex color (or normal)
			1,//location
			0,//binding
			VK_FORMAT_R32G32B32_SFLOAT,//format
			offsetof(Vertex, r),//offset
		},
		{//3D texcoord
			2,//location
			0,//binding
			VK_FORMAT_R32G32_SFLOAT,//format
			offsetof(Vertex, tx),//offset
		},
	};
#ifndef DISABLE_TEXT
	VkVertexInputAttributeDescription attributedescriptions_sdftext[]=
	{
		{//sdftext position
			0,//location
			0,//binding
			VK_FORMAT_R32G32_SFLOAT,//format
			0,//offset
		},
		{//sdftext texcoord
			1,//location
			0,//binding
			VK_FORMAT_R32G32_SFLOAT,//format
			sizeof(float[2]),//offset
		},
	};
#endif
	VkPipelineVertexInputStateCreateInfo vertexinputinfos[]=
	{
		{//3D
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, 0, 0,
			1, bindingdescriptions+0,
			_countof(attributedescriptions_3D), attributedescriptions_3D,
		},
#ifndef DISABLE_TEXT
		{//sdftext
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, 0, 0,
			1, bindingdescriptions+1,
			_countof(attributedescriptions_sdftext), attributedescriptions_sdftext,
		},
#endif
	};
	VkPipelineInputAssemblyStateCreateInfo inputassemblyinfo=
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, 0, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE,//primitive restart enable
	};
	viewport.x=0;
	viewport.y=0;
	viewport.width=(float)extent.width;
	viewport.height=(float)extent.height;
	viewport.minDepth=0;
	viewport.maxDepth=1;
	scissor.offset.x=0;
	scissor.offset.y=0;
	scissor.extent=extent;

	memcpy(&region, &viewport, sizeof(float[4]));
	memcpy((float*)&region+4, &scissor, sizeof(scissor));
	
	VkPipelineViewportStateCreateInfo viewportstate=
	{
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, 0, 0,
		1, 0,//viewports (specified later)
		1, 0,//scissors (specified later)
	};
	VkDynamicState dynamicstates[]=
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};
	VkPipelineRasterizationStateCreateInfo rasterizer=
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, 0, 0,
		VK_FALSE,//depth clamp enable
		VK_FALSE,//rasterizer discard enable
		VK_POLYGON_MODE_FILL,//changing this requires enabling a GPU feature
		
		VK_CULL_MODE_NONE,
		//VK_CULL_MODE_BACK_BIT,
		
		//VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FRONT_FACE_CLOCKWISE,

		VK_FALSE,//depth bias enable
		0, 0, 0,//depth bias constant/clamp/slope
		1,//line width
	};
	VkPipelineMultisampleStateCreateInfo multisampling=
	{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, 0, 0,
		frameAAsamples,
		enableMSAA,//sample shading enable
		enableMSAA?0.2f:1,//min sample shading
		0,//sample mask
		VK_FALSE,//alpha to coverage enable
		VK_FALSE,//alpha to one enable
	};
	VkPipelineDepthStencilStateCreateInfo depthstencil=
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, 0, 0,
		VK_TRUE,//depth test enable
		VK_TRUE,//depth write enable
		VK_COMPARE_OP_LESS,
		VK_FALSE,//depth bounds test enable
		VK_FALSE,//stencil test enable
		{//front (optional)
			VK_STENCIL_OP_KEEP,
			VK_STENCIL_OP_KEEP,
			VK_STENCIL_OP_KEEP,
			VK_COMPARE_OP_NEVER,
			0, 0, 0,
		},
		{//back (optional)
			VK_STENCIL_OP_KEEP,
			VK_STENCIL_OP_KEEP,
			VK_STENCIL_OP_KEEP,
			VK_COMPARE_OP_NEVER,
			0, 0, 0,
		},
		0,//min depth bounds
		1,//max depth bounds
	};
	VkPipelineColorBlendAttachmentState colorblendattachement=
	{
		VK_TRUE,//blend enable
		VK_BLEND_FACTOR_SRC_ALPHA,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT,
	};
	VkPipelineColorBlendStateCreateInfo colorblending=
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, 0, 0,
		VK_FALSE,//logicOpEnable
		VK_LOGIC_OP_COPY,
		1, &colorblendattachement,
		{0, 0, 0, 0},//blend constants
	};
	VkPipelineDynamicStateCreateInfo dynamicstate=
	{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, 0, 0,
		_countof(dynamicstates), dynamicstates,
	};
	VkGraphicsPipelineCreateInfo pipelineinfos[]=
	{
		{//3D
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, 0, 0,
			2, shaderstageinfo,
			vertexinputinfos+0,
			&inputassemblyinfo,
			0,//tesselationState
			&viewportstate,
			&rasterizer,
			&multisampling,
			&depthstencil,
			&colorblending,
			&dynamicstate,
			pipelinelayout,
			renderpass,
			0,//subpass
			VK_NULL_HANDLE,//basePipelineHandle
			0,//basePipelineIndex
		},
#ifndef DISABLE_TEXT
		{//sdftext
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, 0, 0,
			2, shaderstageinfo+3,
			vertexinputinfos+1,
			&inputassemblyinfo,
			0,//tesselationState
			&viewportstate,
			&rasterizer,
			&multisampling,
			&depthstencil,
			&colorblending,
			&dynamicstate,
			pipelinelayout,
			renderpass,
			0,//subpass
			VK_NULL_HANDLE,//basePipelineHandle
			0,//basePipelineIndex
		},
#endif
	};
	error=vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, _countof(pipelineinfos), pipelineinfos, 0, graphicspipelines);	CHECKVK(error);

	//create compute pipeline
#ifdef ENABLE_COMPUTE
	{
		VkPipelineLayoutCreateInfo pipelinelayoutinfo=
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, 0, 0,
			1, &descriptorsetlayout,
			0, 0,//push constant ranges
		};
		error=vkCreatePipelineLayout(device, &pipelinelayoutinfo, 0, &computepipelinelayout);	CHECKVK(error);
		VkComputePipelineCreateInfo pipelineinfo=
		{
			VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, 0, 0,
			shaderstageinfo[2],
			computepipelinelayout,
		};
		vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineinfo, 0, &computepipeline);
	}
#endif

	//destroy shader modules
	for(int k=0;k<_countof(shaders);++k)
	{
		vkDestroyShaderModule(device, shaders[k], 0);
		shaders[k]=0;
	}


	//step 8: create command pool
	VkCommandPoolCreateInfo poolinfo=
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, 0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,//flags: VK_COMMAND_POOL_CREATE_TRANSIENT_BIT / VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
		graphicsqueuefamily,
	};
	error=vkCreateCommandPool(device, &poolinfo, 0, &commandpool);	CHECKVK(error);


	//step 9: create framebuffers & depth resouces
	create_framebuffers();


	//step 10: send texture(s): create sampler & image/memory/view for each texture
	{
		VkSamplerCreateInfo samplerinfo=
		{
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, 0, 0,
			VK_FILTER_LINEAR,//magnification filter
			VK_FILTER_LINEAR,//minification filter
			VK_SAMPLER_MIPMAP_MODE_LINEAR,//mipmapmode
			VK_SAMPLER_ADDRESS_MODE_REPEAT,//u
			VK_SAMPLER_ADDRESS_MODE_REPEAT,//v
			VK_SAMPLER_ADDRESS_MODE_REPEAT,//w
			0,//miplodbias
			anisotropyenable,
			physicaldeviceproperties.limits.maxSamplerAnisotropy,
			0,//compareEnable
			0,//compareOp
			0,//minLod
			miplevels>1?(float)miplevels:0,//maxLod
			VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			VK_FALSE,//unnormalized coordinates
		};
		vkCreateSampler(device, &samplerinfo, 0, &texturesampler);
	}
	{
		VkFormat format=VK_FORMAT_R8G8B8A8_SRGB;
		create_texture_immediate(texture, &format, miplevels, &textureimage, &texturememory);
		textureimageview=create_imageview(textureimage, format, VK_IMAGE_ASPECT_COLOR_BIT, miplevels);

#ifndef DISABLE_TEXT
		format=VK_FORMAT_R8_SRGB;
		create_texture_immediate(atlas, &format, miplevels, &sdftext_atlas, &sdftext_atlasmemory);
		sdftext_atlasview=create_imageview(sdftext_atlas, format, VK_IMAGE_ASPECT_COLOR_BIT, miplevels);
#endif
	}


	//step 11: create vertex, index & uniform buffers
	create_buffer_immediate(vertices->data, vertices->count*vertices->esize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vertexbuffer, &vertexbuffermemory);
	create_buffer_immediate(indices->data, indices->count*indices->esize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &indexbuffer, &indexbuffermemory);
	indexcount=(int)indices->count;
	{
		size_t size=sizeof(UniformBufferObject);
		for(int k=0;k<MAXFRAMESINFLIGHT;++k)
		{
			create_buffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformbuffers+k, uniformbuffersmemory+k);
			vkMapMemory(device, uniformbuffersmemory[k], 0, size, 0, uniformbuffersmapped+k);
		}
		
		size=sizeof(SdftextUBO);
		for(int k=0;k<MAXFRAMESINFLIGHT;++k)
		{
			create_buffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sdftext_uniformbuffers+k, sdftext_uniformmemory+k);
			vkMapMemory(device, sdftext_uniformmemory[k], 0, size, 0, sdftext_uniformmap+k);
		}
		
#ifdef ENABLE_COMPUTE
		size=particles->count*particles->esize;
		VkBuffer stagingbuffer=0;
		VkDeviceMemory stagingmemory=0;
		create_stagingbuffer_immediate(particles->data, size, &stagingbuffer, &stagingmemory);
		for(int k=0;k<MAXFRAMESINFLIGHT;++k)
		{
			create_buffer(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shaderstoragebuffers+k, shaderstoragememory+k);
			copy_buffer(stagingbuffer, shaderstoragebuffers[k], 0, 0, size);
		}
		destroy_buffer(&stagingbuffer, &stagingmemory);
		nparticles=(int)particles->count;
#endif
	}


	//step 12: create descriptor pool & update descriptor sets		(like setKernelArg)
	{
		VkDescriptorPoolSize poolsizes[]=
		{
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAXFRAMESINFLIGHT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAXFRAMESINFLIGHT},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAXFRAMESINFLIGHT*2},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAXFRAMESINFLIGHT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAXFRAMESINFLIGHT},
		};
		VkDescriptorPoolCreateInfo poolinfo=
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, 0, 0,
			MAXFRAMESINFLIGHT,
			_countof(poolsizes), poolsizes,
		};
		error=vkCreateDescriptorPool(device, &poolinfo, 0, &descriptorpool);	CHECKVK(error);

		VkDescriptorSetLayout layouts[MAXFRAMESINFLIGHT];
		memfill(layouts, &descriptorsetlayout, sizeof(layouts), sizeof(descriptorsetlayout));
		VkDescriptorSetAllocateInfo allocinfo=
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, 0,
			descriptorpool,
			MAXFRAMESINFLIGHT, layouts,//descriptor set layouts
		};
		error=vkAllocateDescriptorSets(device, &allocinfo, descriptorsets);	CHECKVK(error);
		for(int k=0;k<MAXFRAMESINFLIGHT;++k)
		{
			int previdx=k-1;
			MODVAR(previdx, previdx, MAXFRAMESINFLIGHT);
			VkDescriptorBufferInfo bufferinfos_3D[]=
			{
				{
					uniformbuffers[k],
					0,//offset
					sizeof(UniformBufferObject),//range
				},
#ifdef ENABLE_COMPUTE
				{
					shaderstoragebuffers[previdx],
					0,
					nparticles*sizeof(Vertex),
				},
				{
					shaderstoragebuffers[k],
					0,
					nparticles*sizeof(Vertex),
				},
#endif
			};
			VkDescriptorImageInfo imageinfo_3D=
			{
				texturesampler,
				textureimageview,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			};

#ifndef DISABLE_TEXT
			VkDescriptorBufferInfo bufferinfo_sdftext=
			{
				sdftext_uniformbuffers[k],
				0,//offset
				sizeof(SdftextUBO),//range
			};
			VkDescriptorImageInfo imageinfo_sdftext=
			{
				texturesampler,
				sdftext_atlasview,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			};
#endif
			VkWriteDescriptorSet descriptorwrites[]=
			{
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
					descriptorsets[k],//dstSet
					0,//uniform dst binding
					0,//array element index (unused)
					1,//descriptor count
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					0,//image info
					bufferinfos_3D+0,
					0,//texel buffer view
				},
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
					descriptorsets[k],
					1,
					0,
					1,
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					&imageinfo_3D,
					0,
					0,
				},
				
#ifdef ENABLE_COMPUTE
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
					descriptorsets[k],
					2,
					0,
					1,
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					0,
					bufferinfos_3D+1,
					0,
				},
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
					descriptorsets[k],
					3,
					0,
					1,
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					0,
					bufferinfos_3D+2,
					0,
				},
#endif

#ifndef DISABLE_TEXT
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
					descriptorsets[k],//dstSet
					4,//uniform dst binding
					0,//array element index (unused)
					1,//descriptor count
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					0,//image info
					&bufferinfo_sdftext,
					0,//texel buffer view
				},
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0,
					descriptorsets[k],
					5,
					0,
					1,
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					&imageinfo_sdftext,
					0,
					0,
				},
#endif
			};
			vkUpdateDescriptorSets(device, _countof(descriptorwrites), descriptorwrites, 0, 0);
		}
	}


	//step 13: create command buffers
	{
		VkCommandBufferAllocateInfo commandbufferinfo=
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, 0,
			commandpool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,//secondary cannot be submitted directly, but can be called by other command buffers
			MAXFRAMESINFLIGHT,//command buffer count
		};
		error=vkAllocateCommandBuffers(device, &commandbufferinfo, commandbuffers);	CHECKVK(error);
#ifdef ENABLE_COMPUTE
		error=vkAllocateCommandBuffers(device, &commandbufferinfo, computecommandbuffers);	CHECKVK(error);
#endif
	}


	//step 14: create sync objects
	VkSemaphoreCreateInfo semaphoreinfo=
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, 0, 0,
	};
	VkFenceCreateInfo fenceinfo=
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, 0, VK_FENCE_CREATE_SIGNALED_BIT,//to avoid waiting forever on first frame
	};
	for(int k=0;k<MAXFRAMESINFLIGHT;++k)
	{
		error=vkCreateSemaphore(device, &semaphoreinfo, 0, imageavailablesemaphores+k);	CHECKVK(error);
		error=vkCreateSemaphore(device, &semaphoreinfo, 0, renderfinishedsemaphores+k);	CHECKVK(error);
		error=vkCreateFence(device, &fenceinfo, 0, inflightfences+k);	CHECKVK(error);
#ifdef ENABLE_COMPUTE
		error=vkCreateSemaphore(device, &semaphoreinfo, 0, computefinishedsemaphores+k);	CHECKVK(error);
		error=vkCreateFence(device, &fenceinfo, 0, computeinflightfences+k);	CHECKVK(error);
#endif
	}

	//initialize text style
	set_textcolor(0xFF000000);
	set_textbk(0xFFFFFFFF);
	set_textzoom(1);

	//GUIPrint(0, 100, 100, "Hello");

	return 1;
}
int vkw_render()
{
	//if(text_vertices)//
	//	printf("Text vertex count: %d\n", (int)text_vertices->count);
	//printf("Index count %d\n", indexcount);//
	VkResult error;
	VkCommandBuffer cmdbuf;
	unsigned long long timeout_ns=(long long)-1;


	//compute submission
#ifdef ENABLE_COMPUTE
	{
		vkWaitForFences(device, 1, computeinflightfences+currentframe, VK_TRUE, timeout_ns);

		vkResetFences(device, 1, computeinflightfences+currentframe);
		cmdbuf=computecommandbuffers[currentframe];
		vkResetCommandBuffer(cmdbuf, 0);

		//record compute command buffer
		VkCommandBufferBeginInfo begininfo=
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0,
			0,//inheritance info for secondary command buffers
		};
		error=vkBeginCommandBuffer(cmdbuf, &begininfo);
		vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_COMPUTE, computepipeline);
		vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_COMPUTE, computepipelinelayout, 0, 1, descriptorsets+currentframe, 0, 0);
		vkCmdDispatch(cmdbuf, nparticles/256, 1, 1);
		error=vkEndCommandBuffer(cmdbuf);
		
		VkSubmitInfo submitinfo=
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO, 0,
			0, 0, 0,
			1, &cmdbuf,
			1, computefinishedsemaphores+currentframe,
		};
		vkQueueSubmit(computequeue, 1, &submitinfo, computeinflightfences[currentframe]);
	}
#endif


	//graphics submission
	{
		vkWaitForFences(device, 1, inflightfences+currentframe, VK_TRUE, timeout_ns);
		unsigned imageidx=0;
		error=vkAcquireNextImageKHR(device, swapchain, timeout_ns, imageavailablesemaphores[currentframe], VK_NULL_HANDLE, &imageidx);//timeout is in nanoseconds
		if(error==VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreate_swapchain();
			return 1;
		}
		if(error!=VK_SUBOPTIMAL_KHR)
			CHECKVK(error);
		
#ifndef ENABLE_COMPUTE
		update_uniforms(currentframe);
#endif

		vkResetFences(device, 1, inflightfences+currentframe);//reset fences only if work is submitted

		cmdbuf=commandbuffers[currentframe];
		error=vkResetCommandBuffer(cmdbuf, 0);	CHECKVK(error);
	

		//record command buffer
		VkClearValue clearvalues[2]=//order is as in the attachements
		{
			{
				{{1, 1, 1, 1}},		//<- clear color
			},
		};
		clearvalues[1].depthStencil.depth=1;//1 is farthest, 0 is nearest
		clearvalues[1].depthStencil.stencil=0;

		VkCommandBufferBeginInfo begininfo=
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0, 0,
			0,//inheritanceInfo
		};
		error=vkBeginCommandBuffer(cmdbuf, &begininfo);	CHECKVK(error);
		VkRenderPassBeginInfo renderpassinfo=
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 0,
			renderpass,
			*(VkFramebuffer*)array_at(&swapchainframebuffers, imageidx),
			{{0, 0}, extent},
			2, clearvalues,
		};
		vkCmdBeginRenderPass(cmdbuf, &renderpassinfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipelines[0]);

		VkDeviceSize offset=0;
		vkCmdBindVertexBuffers(cmdbuf, 0, 1, &vertexbuffer, &offset);
		vkCmdBindIndexBuffer(cmdbuf, indexbuffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdBindIndexBuffer(cmdbuf, indexbuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelinelayout, 0, 1, descriptorsets+currentframe, 0, 0);

		vkCmdSetViewport(cmdbuf, 0, 1, &viewport);
		vkCmdSetScissor(cmdbuf, 0, 1, &scissor);
		vkCmdDrawIndexed(cmdbuf, indexcount, 1, 0, 0, 0);
		//vkCmdDrawIndexed(cmdbuf, _countof(indexdata), 1, 0, 0, 0);
		//vkCmdDraw(cmdbuf, _countof(vertexdata), 1, 0, 0);

        //vkCmdBindVertexBuffers(cmdbuf, 0, 1, &shaderstoragebuffers[currentframe], &offset);
        //vkCmdDraw(cmdbuf, nparticles, 1, 0, 0);

		print_text(cmdbuf);

		vkCmdEndRenderPass(cmdbuf);

		error=vkEndCommandBuffer(cmdbuf);	CHECKVK(error);


		//submit command buffer
        VkSemaphore waitsemaphores[]=
		{
#ifdef ENABLE_COMPUTE
			computefinishedsemaphores[currentframe],
#endif
			imageavailablesemaphores[currentframe],
		};
        VkPipelineStageFlags waitstages[]=
		{
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		};
		VkSubmitInfo submitinfo=
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO, 0,
			_countof(waitsemaphores), waitsemaphores, waitstages,//wait semaphores
			1, &cmdbuf,
			1, renderfinishedsemaphores+currentframe,//signal semaphores
		};
		error=vkQueueSubmit(graphicsqueue, 1, &submitinfo, inflightfences[currentframe]);	CHECKVK(error);


		//presentation
		VkPresentInfoKHR presentinfo=
		{
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, 0,
			1, renderfinishedsemaphores+currentframe,
			1, &swapchain, &imageidx,
			0,//check value returned by vkQueuePresentKHR() instead
		};
		error=vkQueuePresentKHR(presentqueue, &presentinfo);
		if(error==VK_ERROR_OUT_OF_DATE_KHR||error==VK_SUBOPTIMAL_KHR||resized)
		{
			//printf("vkQueuePresentKHR(): %s, resized %d\n", vkerror2str(error), resized);
			resized=0;
			recreate_swapchain();
		}
		else
			CHECKVK(error);//error handling not necessary here
	}

	currentframe=(currentframe+1)%MAXFRAMESINFLIGHT;

	return !error;
}
int vkw_resize(int w, int h)
{
	resized=1;
	//recreate_swapchain();
	return 1;
}
int vkw_cleanup()
{
	vkDeviceWaitIdle(device);

	cleanup_swapchain();

	destroy_texture(&textureimage, &texturememory, &textureimageview);
#ifndef DISABLE_TEXT
	destroy_texture(&sdftext_atlas, &sdftext_atlasmemory, &sdftext_atlasview);
#endif
	vkDestroySampler(device, texturesampler, 0);

	for (int k=0;k<MAXFRAMESINFLIGHT;++k)
	{
		destroy_buffer(uniformbuffers+k, uniformbuffersmemory+k);
#ifdef ENABLE_COMPUTE
		destroy_buffer(shaderstoragebuffers+k, shaderstoragememory+k);
#endif
		destroy_buffer(sdftext_uniformbuffers+k, sdftext_uniformmemory+k);
    }
	vkDestroyDescriptorPool(device, descriptorpool, 0);		descriptorpool=0;
	vkDestroyDescriptorSetLayout(device, descriptorsetlayout, 0);	descriptorsetlayout=0;
	
#ifndef DISABLE_TEXT
	if(sdftext_vertexbuffer)
	{
		vkDestroyBuffer(device, sdftext_vertexbuffer, 0);	sdftext_vertexbuffer=0;	sdftext_vertexmap=0;
	}
	if(sdftext_vertexmemory)
	{
		vkFreeMemory(device, sdftext_vertexmemory, 0);	sdftext_vertexmemory=0;
	}
#endif

	destroy_buffer(&vertexbuffer, &vertexbuffermemory);
	destroy_buffer(&indexbuffer, &indexbuffermemory);

	for(int k=0;k<MAXFRAMESINFLIGHT;++k)
	{
		vkDestroySemaphore(device, imageavailablesemaphores[k], 0);	imageavailablesemaphores[k]=0;
		vkDestroySemaphore(device, renderfinishedsemaphores[k], 0);	renderfinishedsemaphores[k]=0;
		vkDestroyFence(device, inflightfences[k], 0);	inflightfences[k]=0;
#ifdef ENABLE_COMPUTE
		vkDestroySemaphore(device, computefinishedsemaphores[k], 0);	computefinishedsemaphores[k]=0;
		vkDestroyFence(device, computeinflightfences[k], 0);	computeinflightfences[k]=0;
#endif
	}

	vkDestroyCommandPool(device, commandpool, 0);	commandpool=0;
	
#ifdef ENABLE_COMPUTE
	vkDestroyPipeline(device, computepipeline, 0);	computepipeline=0;
	vkDestroyPipelineLayout(device, computepipelinelayout, 0);	computepipelinelayout=0;
#endif

	for(int k=0;k<_countof(graphicspipelines);++k)
	{
		vkDestroyPipeline(device, graphicspipelines[k], 0);	graphicspipelines[k]=0;
	}
	vkDestroyPipelineLayout(device, pipelinelayout, 0);	pipelinelayout=0;

	vkDestroyRenderPass(device, renderpass, 0);	renderpass=0;

	vkDestroySurfaceKHR(instance, surface, 0);
	vkDestroyDevice(device, 0);
#ifdef ENABLE_VALIDATION
	{
		LOAD_FUNC(vkDestroyDebugUtilsMessengerEXT);
		if(!vkDestroyDebugUtilsMessengerEXT)
		{
			LOG_ERROR("");
			return 0;
		}
		vkDestroyDebugUtilsMessengerEXT(instance, messenger, 0);
	}
#endif
	vkDestroyInstance(instance, 0);	instance=0;
	return 1;
}