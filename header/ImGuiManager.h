#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class UI_MANAGER {

	GLFWwindow* window;
public:
	UI_MANAGER(GLFWwindow* window) { window = window; }


	~UI_MANAGER() { void DestroyAll(); }
};