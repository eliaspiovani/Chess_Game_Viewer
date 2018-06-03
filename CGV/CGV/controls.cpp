#include <stdio.h>

// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in main.cpp.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
    return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
    return ProjectionMatrix;
}


// Initial position : on +Z
float radius = 35;
//glm::vec3 position = glm::vec3( 7, 7, radius );
glm::vec3 position = glm::vec3( 0, 0, 0 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f; //3.14f;
// Initial vertical angle : none
float verticalAngle = 3.14f/4.0f;
// Initial Field of View
float initialFoV = 25.0f;
float FoV = initialFoV;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (FoV >= 1.0f && FoV <= 160.0f)
        FoV -= yoffset;
    if (FoV <= 1.0f)
        FoV = 1.0f;
    if (FoV >= 160.0f)
        FoV = 160.0f;
}

glm::vec3 computeMatricesFromInputs(const int &mouseButton){
    
    glfwSetScrollCallback(window, scroll_callback);
    
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    //    float deltaTime = float(currentTime - lastTime);
    
    // Get mouse position
    double xpos = 0, ypos = 0;
    
    // Get mouse click, left click GLFW_MOUSE_BUTTON_LEFT, for middle click GLFW_MOUSE_BUTTON_MIDDLE
    int state = glfwGetMouseButton(window, mouseButton);
    
    static bool click_down = false;
    static double oldXpos, oldYpos;
    
    // Compute the MVP matrix from keyboard and mouse input
    if (state == GLFW_PRESS) {
        // border down sensor for the button, when click starts put mouse in the center to better calculations
        if (click_down == false) {
            // save the position to return it after the button is released
            glfwGetCursorPos(window, &oldXpos, &oldYpos);
            // Reset mouse position for initial frame
            glfwSetCursorPos(window, 1024/2, 768/2);
            click_down = true;
            
            // Hide the mouse and enable unlimited mouvement
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Reset mouse position for next frame
        glfwSetCursorPos(window, 1024/2, 768/2);
        
        
        // Compute new orientation
        horizontalAngle += mouseSpeed * float(1024/2 - xpos );
        verticalAngle   += mouseSpeed * float( 768/2 - ypos );
        
    } else {
        // set mouse to normal again
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        
        // return mouse to the position when the click started
        if (click_down == true) {
            glfwSetCursorPos(window, float(oldXpos), float(oldYpos));
            //            printf("%f %f\n", float(oldXpos), float(oldYpos));
            click_down = false;
        }
    }
    
    glm::vec3 direction(
                        7 + radius * sin(horizontalAngle),
                        7 + radius * verticalAngle,
                        7 + radius * cos(horizontalAngle)
                        );
    
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 300.0f);
    // Camera matrix
    //ViewMatrix       = glm::lookAt(position, position+direction, up);
    ViewMatrix       = glm::lookAt(position+direction, glm::vec3(7,0,7), glm::vec3(0,1,0));
    // Camera is here
    // and looks here : at the same position, plus "direction"
    // Head is up (set to 0,-1,0 to look upside-down)
    
    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
    
    //printf("Camera Position %f", direction.x);
    return direction;
}

