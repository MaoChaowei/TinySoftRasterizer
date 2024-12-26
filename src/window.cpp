#include"window.h"
#include"softrender/render.h"

/* 一些回调函数  */ 
void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
} 

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance && instance->render_) {
        instance->render_->handleKeyboardInput(key, action);
    }

}

void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!instance || !instance->render_) return;

    int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    if (leftButtonState == GLFW_PRESS) {
        // 如果是第一次检测到左键按下，需要重置 lastX_ 和 lastY_，避免镜头跳动
        if (instance->firstMouse_) {
            instance->lastX_ = xpos;
            instance->lastY_ = ypos;
            instance->firstMouse_ = false;
        }

        // 计算偏移量
        float xoffset = static_cast<float>(xpos - instance->lastX_);
        float yoffset = static_cast<float>(instance->lastY_ - ypos); // 注意 y 方向相反
        instance->lastX_ = static_cast<float>(xpos);
        instance->lastY_ = static_cast<float>(ypos);

        // 将偏移量传给渲染器
        instance->render_->handleMouseInput(xoffset, yoffset);
    }
    else {
        // 如果左键没有按下，可以把 firstMouse_ 重置，以便下次按下时重新设置 (可选)
        instance->firstMouse_ = true;
    }

}

// 初始化glfw窗口
int Window::init(const char* name,int width,int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_ = glfwCreateWindow(width, height, name, NULL, NULL);
	if (!window_) {
		std::cout << "fail to create the window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window_);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

    // 设置用户指针，将 `this` 绑定到 `GLFWwindow`
    glfwSetWindowUserPointer(window_, this);

	glViewport(0, 0, width, height);
    width_=width;
    height_=height;

    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwSetKeyCallback(window_, keyCallback);  
    glfwSetCursorPosCallback(window_, mouseCallback);
    // glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 捕获鼠标

    this->initData();
    this->initShaderProgram();    // init shader program

	return 0;
}

// 初始化着色器
void Window::initShaderProgram() {
    // 编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 检查编译是否成功
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // 检查编译是否成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 创建着色器程序并链接
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 检查链接是否成功
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    shaderProgram_=shaderProgram;
}

void Window::initData()
{
    // 顶点数据和纹理坐标
    float vertices[] = {
        // positions     // texCoords
        -1.0f,  1.0f,    0.0f, 1.0f,  // 左上角
        -1.0f, -1.0f,    0.0f, 0.0f,  // 左下角
         1.0f, -1.0f,    1.0f, 0.0f,  // 右下角
         1.0f,  1.0f,    1.0f, 1.0f   // 右上角
    };
    unsigned int indices[] = {
        0, 1, 2,  // 第一三角形
        0, 2, 3   // 第二三角形
    };

    // 创建 VAO、VBO 和 EBO
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 绑定 VAO
    glBindVertexArray(VAO_);

    // 绑定并设置 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定并设置 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 设置顶点属性
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    // 设置纹理环绕和过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Window::updateFrame(unsigned char* addr){
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE,addr);// 更新纹理数据
    // clear
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // 绘制纹理
    glUseProgram(shaderProgram_);
    glBindVertexArray(VAO_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}