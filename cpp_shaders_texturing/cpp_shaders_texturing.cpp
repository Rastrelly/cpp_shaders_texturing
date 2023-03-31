#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <ctime>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "shader.h"
#include "stb_image.h"

using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::to_string;

typedef vector<float> flarr;
typedef vector<int> intarr;

Shader * shad;
unsigned int ourTexture;

flarr vertices = {
	// positions          // colors           // texture coords
	 0.4f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   // top right
	 0.4f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // bottom right
	-0.4f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   // bottom left
	-0.4f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f    // top left 
};

intarr indices =
{
	0, 1, 3,
	1, 2, 3
};


void drawRectangle(flarr verts, intarr inds, unsigned int tex)
{

	//buffers
	unsigned int VBO; //vertex buffer
	unsigned int EBO; //elemnt buffer
	unsigned int VAO; //vertex array

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO); //generate vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //assign data type to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*verts.size(), verts.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO); //generate element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //assgn data type
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*inds.size(), inds.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
}



unsigned int makeTexture(string fileName)
{
	unsigned int tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int iw, ih, nChan;
	unsigned char *data = stbi_load(fileName.c_str(),&iw,&ih,&nChan,0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iw, ih, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Texture load failed " << fileName << endl;
	}

	stbi_image_free(data);

	return tex;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void prepShaders()
{
	std::cout << "Preparing shaders..." << std::endl;
	shad = new Shader("shader_vert.gls", "shader_frag.gls");
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Main Window", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//INITIALIZTION OF GLAD
	bool gladLoaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!gladLoaded)
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Renderer setup
	//main loop screen refresh callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//ready shaders for use
	prepShaders();	

	//load texture
	ourTexture = makeTexture("motoko_setup.jpg");

	//MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shad->use();

		shad->setInt("ourTexture",0);

		drawRectangle(vertices, indices, ourTexture);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//kill GLFW
	glfwTerminate();

	return 0;
}

