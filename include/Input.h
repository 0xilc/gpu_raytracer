#pragma once
#include <unordered_map>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <functional>

class Input
{
public:
    using MouseCallbackType = void(*)(void*, float, float);
    using KeyCallbackType = void(*)(void*, int, int, int);

    static void Init(GLFWwindow* window);
    static bool IsKeyPressed(int key);
    const std::unordered_map<int, bool>& GetKeysPressed();

    static int SubscribeMouseMoved(MouseCallbackType callback, void* data);
    static void UnsubscribeMouseMoved(int id);

    static int SubscribeKeyPressed(KeyCallbackType callback, void* data);
    static void UnsubscribeKeyPressed(int id);

    static GLFWwindow* s_Window;

private:
    static std::unordered_map<int, bool> s_KeysPressed;
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    
    static std::unordered_map<int, std::tuple<MouseCallbackType, void*>> s_MouseMovedCallbacks;
    static std::unordered_map<int, std::tuple<KeyCallbackType, void*>> s_KeyPressedCallbacks;

    static int s_MousePressedCounter;
    static int s_KeyPressedCounter;
    static double s_MouseX, s_MouseY;
    static bool s_FirstMouse;
};
