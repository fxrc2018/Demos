#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;

void init(GLFWwindow *window){

}

void display(GLFWwindow *window, double currentTime){
    glClearColor(1.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char const *argv[])
{
    if(!glfwInit()){
        exit(EXIT_FAILURE);
    }
    //指定机器兼容的目标版本
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow *window = glfwCreateWindow(600,600,"Main",NULL,NULL);
    // if(window == NULL){
    //     cout<<"window == NULL"<<endl;
    //     return 0;
    // }
    glfwMakeContextCurrent(window);
    if(glewInit() != GLEW_OK){
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);
    init(window);
    while(!glfwWindowShouldClose(window)){
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
