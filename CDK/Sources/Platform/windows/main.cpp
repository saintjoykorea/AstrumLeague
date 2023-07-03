#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSDeviceConfig.h"
#include "CSOpenGL.h"
#include "CSFile.h"
#include "CSThread.h"
#include "CSGraphicsImpl.h"
#include "CSAudio.h"
#include "CSApplication.h"
#include "CSSignal.h"
#include "CSDiagnostics.h"
#include "CSView.h"
#include "CSViewBridge.h"
#include "CSFontImpl.h"
#include "CSTextFieldImpl.h"
#include "CSURLConnectionImpl.h"

#include <direct.h>
#include <windows.h>
#include <GLFW/glfw3.h>
#include <FreeImage.h>

struct Context {
	GLFWwindow* window;
	CSView* view;
	CSGraphics* graphics;
	int width;
	int height;
	bool alive;
	CSPlatformTouch* touch;
	CSString* clipboard;
};
static Context __context = {};

static void onError(int error, const char* description);
static void onResizeFramebuffer(GLFWwindow* window, int width, int height);
static void onMouseEvent(GLFWwindow* window, int button, int action, int mods);
static void onScrollEvent(GLFWwindow* window, double xoffset, double yoffset);
static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onTextInput(GLFWwindow *window, uint codepoint, int mods);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
#ifdef CS_CONSOLE_DEBUG
	if (AllocConsole()) {
		freopen("CONIN$", "rb", stdin);
		freopen("CONOUT$", "wb", stdout);
		freopen("CONOUT$", "wb", stderr);

		//std::ios::sync_with_stdio();
	}
#endif

	const char* app = __argv[0];
	int e = strlen(app) - 1;
	while (e >= 0 && app[e] != '\\') e--;
	if (e > 0) {
		char* cwd = (char*)alloca(e + 1);
		strncpy(cwd, app, e);
		cwd[e] = 0;
		chdir(cwd);
		app += e;
	}

	glfwSetErrorCallback(onError);

	if (!glfwInit()) {
		MessageBox(NULL, "GLFW Init fail", "Error", MB_OK);
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

	deviceConfigInitialize();

	int width = deviceConfigWidth();
	int height = deviceConfigHeight();

	int profile = CSFile::initialize();

	GLFWwindow* window;
	{
		char title[64];
		sprintf(title, "%s #%d", app, profile);
		window = glfwCreateWindow(width, height, title, NULL, NULL);
	}
		
	if (!window) {
		CSFile::finalize();

		MessageBox(NULL, "Window Init fail", "Error", MB_OK);

		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, onResizeFramebuffer);
	glfwSetMouseButtonCallback(window, onMouseEvent);
	glfwSetScrollCallback(window, onScrollEvent);
	glfwSetKeyCallback(window, onKeyEvent);
	glfwSetCharModsCallback(window, onTextInput);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		CSFile::finalize();

		MessageBox(NULL, "GLEW Init fail", "Error", MB_OK);

		glfwTerminate();
		exit(-1);
	}

	int extensionCount = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
	for (int i = 0; i < extensionCount; i++) {
		CSLog((const char*)glGetStringi(GL_EXTENSIONS, i));
	}

	FreeImage_Initialise(TRUE);

	CSThread::initialize();
	CSSignal::initialize();
	CSFontImpl::initialize();
	CSGraphicsImpl::initialize();
	CSAudio::initialize();
	CSTextFieldHandleManager::initialize();
	CSURLConnectionHandleManager::initialize();

	glfwGetFramebufferSize(window, &width, &height);

	__context.graphics = new CSGraphics(CSRenderTarget::target(width, height, true));
	__context.window = window;
	__context.view = new CSView(NULL);
	__context.width = width;
	__context.height = height;
	__context.alive = true;

	onStart(__context.view);

	while (!glfwWindowShouldClose(window) && __context.alive) {
		glfwPollEvents();

		CSTextFieldHandle* handle = CSTextFieldHandleManager::sharedManager()->focusedHandle();

		if (handle) {
			handle->timeout();
		}

		if (__context.touch) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			if (x < 0 || x > __context.width || y < 0 || y > __context.height) {
				__context.view->touchesCancelled(__context.touch, 1);
				delete __context.touch;
				__context.touch = NULL;
			}
			else if (x != __context.touch->x || y != __context.touch->y) {
				__context.touch->x = x;
				__context.touch->y = y;
				__context.view->touchesMoved(__context.touch, 1);
			}
		}
		CSThread::mainThread()->execute();
	}
	
	if (__context.clipboard) __context.clipboard->release();
	if (__context.touch) delete __context.touch;
	__context.graphics->release();
	__context.view->release();

	CSApplication::sharedApplication()->OnDestroy();
	onDestroy();

	FreeImage_DeInitialise();

	CSFile::finalize();
	CSURLConnectionHandleManager::finalize();
	CSTextFieldHandleManager::finalize();
	CSGraphicsImpl::finalize();
	CSAudio::finalize();
	CSFontImpl::finalize();
	CSSignal::finalize();
	CSThread::finalize();

	glfwDestroyWindow(__context.window);
	
	glfwTerminate();

#ifdef CS_CONSOLE_DEBUG
	FreeConsole();
#endif

	return 0;
}

static void onError(int error, const char* description) {
	CSErrorLog("Error:%s", description);
}

static void onResizeFramebuffer(GLFWwindow* window, int width, int height) {
	if (width && height) {
		__context.width = width;
		__context.height = height;
		__context.graphics->target()->resize(width, height);
	}
}

static void onMouseEvent(GLFWwindow* window, int button, int action, int mods) {
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	switch (action) {
		case GLFW_PRESS:
			if (button == GLFW_MOUSE_BUTTON_LEFT && CSTextFieldHandleManager::sharedManager()->touchHandle(CSVector2(x, y))) {
				break;
			}
			if (!__context.touch) {
				__context.touch = new CSPlatformTouch();
				__context.touch->key = 0;
				__context.touch->button = (CSTouchButton)button;
				__context.touch->x = x;
				__context.touch->y = y;
				__context.view->touchesBegan(__context.touch, 1);
			}
			break;
		case GLFW_RELEASE:
			if (__context.touch) {
				__context.view->touchesEnded(__context.touch, 1);
				delete __context.touch;
				__context.touch = NULL;
			}
			break;
	}
}

static void onScrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
	__context.view->wheel(yoffset);
}

static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CSTextFieldHandle* handle = CSTextFieldHandleManager::sharedManager()->focusedHandle();

	if (handle) {
		if (action == GLFW_PRESS) {
			switch (key) {
				case GLFW_KEY_LEFT:
					handle->moveLeftCursor(false);
					break;
				case GLFW_KEY_RIGHT:
					handle->moveRightCursor(false);
					break;
				case GLFW_KEY_BACKSPACE:
					handle->moveLeftCursor(true);
					break;
				case GLFW_KEY_DELETE:
					handle->moveRightCursor(true);
					break;
				case GLFW_KEY_HOME:
					handle->moveStartCursor();
					break;
				case GLFW_KEY_END:
					handle->moveEndCursor();
					break;
				case GLFW_KEY_ENTER:
					handle->complete();
					break;
			}
		}
	}
	else {
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_ESCAPE) __context.view->backKey();
			else __context.view->keyDown(key);
		}
		else if (action == GLFW_RELEASE) {
			if (key != GLFW_KEY_ESCAPE) __context.view->keyUp(key);
		}
	}
}

static void onTextInput(GLFWwindow *window, uint codepoint, int mods) {
	CSTextFieldHandle* handle = CSTextFieldHandleManager::sharedManager()->focusedHandle();
	
	if (handle) {
		handle->inputCharacter(codepoint);
	}
}

uint CSViewBridge::bufferWidth(void* handle) {
	return __context.width;
}

uint CSViewBridge::bufferHeight(void* handle) {
	return __context.height;
}

void CSViewBridge::render(void* handle, bool refresh) {
	if (refresh) {
		__context.view->draw(__context.graphics);

		CSTextFieldHandleManager::sharedManager()->draw(__context.graphics);

		__context.graphics->flush();

		glDisableCS(GL_STENCIL_TEST);
		glStencilOpCS(GL_KEEP, GL_KEEP, GL_KEEP);
		glColorMaskCS(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glBindFramebufferCS(GL_FRAMEBUFFER, 0);
		glViewportCS(0, 0, __context.width, __context.height);
		CSGraphicsImpl::sharedImpl()->fillShader()->draw(__context.graphics->target()->screen(false), CSRenderBounds::Full);

		glFlushCS();

		CSGraphics::invalidate(CSGraphicsValidationTarget | CSGraphicsValidationStencil);

		glfwSwapBuffers(__context.window);
	}
	else {
		Sleep(10);
	}
}

CSRect CSViewBridge::frame(void* handle) {
	return CSRect(0, 0, __context.width, __context.height);
}

CSRect CSViewBridge::bounds(void* handle) {
	return CSRect(0, 0, __context.width, __context.height);
}

CSEdgeInsets CSViewBridge::edgeInsets(void* handle) {
	CSEdgeInsets rtn = {};

	return rtn;
}

bool CSViewBridge::screenshot(void* handle, const char* path) {
	return __context.graphics->target()->screenshot(path);
}

//==============================================================
void CSApplication::setVersion(const char* version) {
    if (version) {
        if (!_version) _version = new CSString(version);
        else _version->set(version);
    }
    else {
        CSObject::release(_version);
    }
}

void CSApplication::setResolution(CSResolution resolution) {
	CSLog("setResolution not supported on windows:%d", resolution);
}

CSResolution CSApplication::resolution() const {
	return CSResolutionFit;
}

void CSApplication::openURL(const char* url) {
	CSLog("open url:%s", url);
}

const CSString* CSApplication::clipboard() {
	return __context.clipboard;
}

void CSApplication::setClipboard(const char* text) {
	if (text) {
		if (!__context.clipboard) __context.clipboard = new CSString(text);
		else __context.clipboard->set(text);
	}
	else {
		CSObject::release(__context.clipboard);
	}
}

void CSApplication::shareUrl(const char* title, const char* message, const char* url) {
    
}

void CSApplication::finish() {
	__context.alive = false;
}

#endif
