#include "Input.h"
#include <iostream>

// Static member initialization
std::unordered_map<int, std::tuple<Input::MouseCallbackType, void*>> Input::s_MouseMovedCallbacks =
    std::unordered_map<int, std::tuple<Input::MouseCallbackType, void*>> (0);
std::unordered_map<int, std::tuple<Input::KeyCallbackType, void*>> Input::s_KeyPressedCallbacks = 
    std::unordered_map<int, std::tuple<Input::KeyCallbackType, void*>> (0);
GLFWwindow* Input::s_Window = nullptr;
std::unordered_map<int, bool> Input::s_KeysPressed;
double Input::s_MouseX = 0.0;
double Input::s_MouseY = 0.0;
bool Input::s_FirstMouse = true;
int Input::s_MousePressedCounter = 0;
int Input::s_KeyPressedCounter = 0;

void Input::Init(GLFWwindow* window) {
    s_Window = window;
    //glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSetCursorPosCallback(window, Input::MouseCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
    {
        s_KeysPressed[key] = true;
        for (auto& [_, callback] : s_KeyPressedCallbacks)
        {
            auto& [func, data] = callback;
            func(data, key, scancode, action);
        }
    }
    else if (action == GLFW_RELEASE)
    {
        s_KeysPressed[key] = false;
    }
}

void Input::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (s_FirstMouse) {
        s_MouseX = xpos;
        s_MouseY = ypos;
        s_FirstMouse = false;
    }

    float xoffset = xpos - s_MouseX;
    float yoffset = s_MouseY - ypos;
    s_MouseX = xpos;
    s_MouseY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    for (auto& [_,callback] : s_MouseMovedCallbacks) {
        auto& [func, data] = callback;
		func(data, xoffset, yoffset);
	}
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
		s_KeysPressed[button] = true;
	}
    else if (action == GLFW_RELEASE)
    {
		s_KeysPressed[button] = false;
	}
}

bool Input::IsKeyPressed(int key)
{
    auto it = s_KeysPressed.find(key);
    if (it != s_KeysPressed.end()) {
        return it->second;
    }
    return false;
}

const std::unordered_map<int, bool>& Input::GetKeysPressed()
{
    return s_KeysPressed;
}

int Input::SubscribeMouseMoved(MouseCallbackType callback, void* data)
{
    s_MouseMovedCallbacks[s_MousePressedCounter] = {callback, data};
    s_MousePressedCounter += 1;
    return s_MousePressedCounter - 1;
}

void Input::UnsubscribeMouseMoved(int id)
{   
	s_MouseMovedCallbacks.erase(id);
}

int Input::SubscribeKeyPressed(KeyCallbackType callback, void* data)
{
    s_KeyPressedCallbacks[s_KeyPressedCounter] = {callback , data};
    s_KeyPressedCounter += 1;
    return s_KeyPressedCounter - 1;
}

void Input::UnsubscribeKeyPressed(int id)
{
    s_KeyPressedCallbacks.erase(id);
}

