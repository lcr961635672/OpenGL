#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// 当用户改变窗口大小时，视口也应该被调整，我们对窗口注册 一个回调函数，在窗口大小被调整的时候调用
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "out vec3 ourColor;\n"
                                 "void main() \n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos, 1.0);\n"
                                 "   ourColor = aColor;\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   //  "uniform vec4 ourColor;\n" // 在OpenGL程序代码中设定这个变量
                                   "in vec3 ourColor;\n"
                                   "void main() \n"
                                   "{\n"
                                   "  FragColor = vec4(ourColor, 1.0f);\n"
                                   "}";

int main()
{

  // 首先要创建窗口
  // 初始化GLFW
  glfwInit();
  // 配置GLFW ，我们将主版本跟次版本号都设为3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 创建一个窗口对象，
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "创建窗口失败了" << std::endl;
    glfwTerminate();
    return -1;
  }
  //通知GLFW将我们的窗口的上下文设置为当前线程的主上下文
  glfwMakeContextCurrent(window);
  // 注册窗口变化监听函数
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // 初始化GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // 创建一个着色器对象
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  // 把着色器源码附加到着色器对象上，然后编译它
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "顶点着色器编译发生了错误\n"
              << infoLog << std::endl;
  }

  //创建片段着色器
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  ;
  glCompileShader(fragmentShader);

  //创建着色器程序对象
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  // 把之前编译的着色器附加到程序对象上
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  // 链接起来
  glLinkProgram(shaderProgram);

  // 删除着色器对象
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "着色器程序链接发生了错误\n"
              << infoLog << std::endl;
  }
  float vertices[] = {
      // 位置              // 颜色
      0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // 右下
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 左下
      0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // 顶部
  };

  // 1、 绑定VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  // 2、把顶点数组复制到缓冲中供OpenGL使用
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // 3、设置顶点属性指针
  // 第一个参数指定我们要配置的顶点属性。还记得我们在顶点着色器使用layout(location=0)定义了position顶点属性的位置值(Location)吗？
  // 它可以把顶点属性的位置设置为0.因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0.
  // 第二个参数指定顶点属性的大小。顶点属性是一个vec3，它由三个值组成，所以大小是3.
  // 第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
  // 第四个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为true，所有的数据都会被映射到0(对于有符号型signed数据是-1)到1之间。
  // 第五个参数叫做步长, 它告诉我们在连续的顶点属性组之间的间隔。由于下个组位置数据在3个float之后，我们把步长设置为3*sizeof(float)。
  // 最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在缓冲中起始位置的偏移量。由于位置在数据的开头，这里是0.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glUseProgram(shaderProgram);
  // 创建渲染循环
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);
    // float timeValue = glfwGetTime(); // 获取运行秒数
    // float greenValue = sin(timeValue) / 2.0f + 0.5f;// sin函数让颜色在0.0到1.0之间改变
    // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); // 查询变量位置
    // glUseProgram(shaderProgram);
    // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    std::cout << "已经按下esc键" << std::endl;
    glfwSetWindowShouldClose(window, true);
  }
}
