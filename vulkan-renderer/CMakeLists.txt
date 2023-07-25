# CMakeList.txt : CMake project for vksandbox, include source and define project specific logic here.

include_directories(
	"C:/Projects/Vulkan-Headers/include"
	"E:/C/a/Vulkan-Headers/include"
)
link_directories(
	"C:/Projects/Vulkan-Headers"
	"E:/C/a/Vulkan-Headers"
)
add_executable(vksandbox WIN32
	"console.c"
	"files.c"
	"text.c"
	"util.c"
	"vksandbox.c"
	"vkwrapper.c"
	"window.c"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET vksandbox PROPERTY CXX_STANDARD 20)
endif()

