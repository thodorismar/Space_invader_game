//********************************
//Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1C της OpenGL
//
//ΑΜ:5426        Όνομα:Θεόδωρος Μαρίνος
//ΑΜ:5245        Όνομα:Ευστρατιος Καρουλης

//*********************************

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

// Αποθηκευση μεταβλητων
float A_offset_x = 0.0f;
float A_step = 0.3f;
float camAngleX = 0.0f;   // περιστροφή γύρω από X
float camAngleY = 0.0f;   // περιστροφή γύρω από Y
float radius = 20.0f;     // απόσταση κάμερας από το κέντρο
float currentFOV = 60.0f;
float rotateSpeed = 0.002f;
// gia to 1-C
double lastTime = glfwGetTime();
float fallSpeed = 0.2f; // units per second

// gia ta bonus
bool paused = false;
double lastBulletTime = 0.0;
int movementPhase = 0;      
float distMovedInPhase = 0.0f; // Πόση απόσταση διανύσαμε στην τρέχουσα φάση
float horizontalSpeed = 5.0f;  // Ταχύτητα οριζόντιας κίνησης (πιο γρήγορη από την πτώση)
float dropTarget = 1.0f;       // Στόχος καθόδου (μισό μέγεθος κύβου = 1.0)

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

//= (iv) for the bullets
struct Bullet {
	glm::vec3 pos;
	bool active;
};

std::vector<Bullet> bullets;
float bulletSpeed = 10.0f;


//== (iii) ENEMY POSITIONS 
	glm::vec3 cubeB_Positions[5] = {
	glm::vec3(-9.0f, 10.0f, 0.0f),
	glm::vec3(-5.0f, 10.0f, 0.0f),
	glm::vec3(-1.0f, 10.0f, 0.0f),
	glm::vec3(3.0f, 10.0f, 0.0f),
	glm::vec3(7.0f, 10.0f, 0.0f)
};

	double lastFallTime = 0.0;
	bool gameOver = false;
	bool cubeB_Alive[5] = { true, true, true, true, true };
/////////////////////////////////////////////////

void camera_function()
{
	// Περιστροφή γύρω από τον άξονα X  (W / X)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camAngleX += rotateSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		camAngleX -= rotateSpeed;
	}

	// Περιστροφή γύρω από τον άξονα Y  (Q / Z)
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camAngleY += rotateSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		camAngleY -= rotateSpeed;
	}

	// Zoom με NumPad + και -
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
		currentFOV -= 0.05f;
		if (currentFOV < 20.0f) currentFOV = 20.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
		currentFOV += 0.05f;
		if (currentFOV > 90.0f) currentFOV = 90.0f;
	}

	// ΥΠΟΛΟΓΙΣΜΟΣ ΚΑΜΕΡΑΣ (orbit)
	float camX = radius * cos(camAngleX) * sin(camAngleY);
	float camY = radius * sin(camAngleX);
	float camZ = radius * cos(camAngleX) * cos(camAngleY);

	ViewMatrix = glm::lookAt(
		glm::vec3(camX, camY, camZ),   // θέση κάμερας
		glm::vec3(0.0f, 0.0f, 0.0f),   // το κέντρο του κόσμου
		glm::vec3(0.0f, 1.0f, 0.0f)    // up
	);

	ProjectionMatrix = glm::perspective(
		glm::radians(currentFOV),
		1.0f,        // 850/850
		0.1f,
		100.0f
	);
}

//=Draw the bullets

GLfloat bulletVertices[] = {
	// size: 0.25 x 0.5 x 0.25
	// half-sizes
	-0.125f, 0.0f,  -0.125f,
	 0.125f, 0.0f,  -0.125f,
	 0.125f, 0.5f,  -0.125f,

	-0.125f, 0.0f,  -0.125f,
	 0.125f, 0.5f,  -0.125f,
	-0.125f, 0.5f,  -0.125f,

	-0.125f, 0.0f,  0.125f,
	 0.125f, 0.0f,  0.125f,
	 0.125f, 0.5f,  0.125f,

	-0.125f, 0.0f,  0.125f,
	 0.125f, 0.5f,  0.125f,
	-0.125f, 0.5f,  0.125f,

	// sides…

	-0.125f, 0.0f, -0.125f,
	-0.125f, 0.0f,  0.125f,
	-0.125f, 0.5f,  0.125f,

	-0.125f, 0.0f, -0.125f,
	-0.125f, 0.5f,  0.125f,
	-0.125f, 0.5f, -0.125f,

	0.125f, 0.0f, -0.125f,
	0.125f, 0.0f,  0.125f,
	0.125f, 0.5f,  0.125f,

	0.125f, 0.0f, -0.125f,
	0.125f, 0.5f,  0.125f,
	0.125f, 0.5f, -0.125f
};


void addCubes(std::vector<GLfloat>& out, float x0, float y0, float z0) {
	GLfloat cubes[] = {
		// Front face
	   x0, y0, z0,
	   x0 + 2, y0, z0,
	   x0 + 2, y0 + 2, z0,

	   x0, y0, z0,
	   x0 + 2, y0 + 2, z0,
	   x0, y0 + 2, z0,

	   // Back face
	   x0, y0, z0 - 2,
	   x0 + 2, y0, z0 - 2,
	   x0 + 2, y0 + 2, z0 - 2,

	   x0, y0, z0 - 2,
	   x0 + 2, y0 + 2, z0 - 2,
	   x0, y0 + 2, z0 - 2,

	   // Left
	   x0, y0, z0,
	   x0, y0, z0 - 2,
	   x0, y0 + 2, z0 - 2,

	   x0, y0, z0,
	   x0, y0 + 2, z0 - 2,
	   x0, y0 + 2, z0,

	   // Right
	   x0 + 2, y0, z0,
	   x0 + 2, y0, z0 - 2,
	   x0 + 2, y0 + 2, z0 - 2,

	   x0 + 2, y0, z0,
	   x0 + 2, y0 + 2, z0 - 2,
	   x0 + 2, y0 + 2, z0,

	   // Top
	   x0, y0 + 2, z0,
	   x0 + 2, y0 + 2, z0,
	   x0 + 2, y0 + 2, z0 - 2,

	   x0, y0 + 2, z0,
	   x0 + 2, y0 + 2, z0 - 2,
	   x0, y0 + 2, z0 - 2,

	   // Bottom
	   x0, y0, z0,
	   x0 + 2, y0, z0,
	   x0 + 2, y0, z0 - 2,

	   x0, y0, z0,
	   x0 + 2, y0, z0 - 2,
	   x0, y0, z0 - 2,
	};
	out.insert(out.end(), cubes, cubes + sizeof(cubes) / sizeof(GLfloat));
}
/////////////////////////////////////////////////

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
///////////////////////////////////////////////////



int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(850, 850, u8"Εργασία 1Γ – 2025 – Καταστροφέας", NULL, NULL);


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// background color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// --- Create the 5 top cubes ---
	std::vector<GLfloat> cubes;

	// --- Yellow colors for the 5 cubes ---
	// One yellow RGBA per vertex (36 * 5 vertices)
	std::vector<GLfloat> cubeColors(36 * 5 * 4);

	for (int i = 0; i < 36 * 5; i++) {
		cubeColors[i * 4 + 0] = 1.0f;  // R
		cubeColors[i * 4 + 1] = 1.0f;  // G
		cubeColors[i * 4 + 2] = 0.0f;  // B
		cubeColors[i * 4 + 3] = 1.0f;  // A
	}

	for (int i = 0; i < 5; i++) {
		addCubes(cubes, 0.0f, 0.0f, 0.0f);
	}


	// Create GPU buffer for cubes
	GLuint cubeBuffer;
	glGenBuffers(1, &cubeBuffer);
	GLuint cubeColorBuffer;
	glGenBuffers(1, &cubeColorBuffer);
	// color buffer για τους κυβους
	glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, cubeColors.size() * sizeof(GLfloat), cubeColors.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
	glBufferData(GL_ARRAY_BUFFER, cubes.size() * sizeof(GLfloat), cubes.data(), GL_STATIC_DRAW);
	// buffer gia ta bullets
	GLuint bulletBuffer;
	glGenBuffers(1, &bulletBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bulletBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bulletVertices), bulletVertices, GL_STATIC_DRAW);
	// Create and compile our GLSL program from the shaders

	GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	camera_function();
	
	// Model matrix : an identity matrix (model will be at the origin)

	glm::mat4 Model = glm::mat4(1.0f);

	GLfloat len = 5.0f, wid = 2.5f, heig = 2.5f;

	static const GLfloat cube[] =
	{
		// Bottom face (z=0)
	   -1.5f, -10.0f,  0.0f,
		1.5f, -10.0f,  0.0f,
		1.5f,  -8.5f,  0.0f,

	   -1.5f, -10.0f,  0.0f,
		1.5f,  -8.5f,  0.0f,
	   -1.5f,  -8.5f,  0.0f,

	   // Top face (z=-2)
	   -1.5f, -10.0f, -2.0f,
		1.5f, -10.0f, -2.0f,
		1.5f,  -8.5f, -2.0f,

	   -1.5f, -10.0f, -2.0f,
		1.5f,  -8.5f, -2.0f,
	   -1.5f,  -8.5f, -2.0f,

	   // Front face
	   -1.5f, -10.0f,  0.0f,
		1.5f, -10.0f,  0.0f,
		1.5f, -10.0f, -2.0f,

	   -1.5f, -10.0f,  0.0f,
		1.5f, -10.0f, -2.0f,
	   -1.5f, -10.0f, -2.0f,

	   // Back face
	   -1.5f, -8.5f,  0.0f,
		1.5f, -8.5f,  0.0f,
		1.5f, -8.5f, -2.0f,

	   -1.5f, -8.5f,  0.0f,
		1.5f, -8.5f, -2.0f,
	   -1.5f, -8.5f, -2.0f,

	   // Left face
	   -1.5f, -10.0f,  0.0f,
	   -1.5f, -10.0f, -2.0f,
	   -1.5f,  -8.5f, -2.0f,

	   -1.5f, -10.0f,  0.0f,
	   -1.5f,  -8.5f, -2.0f,
	   -1.5f,  -8.5f,  0.0f,

	   // Right face
		1.5f, -10.0f,  0.0f,
		1.5f, -10.0f, -2.0f,
		1.5f,  -8.5f, -2.0f,

		1.5f, -10.0f,  0.0f,
		1.5f,  -8.5f, -2.0f,
		1.5f,  -8.5f,  0.0f,


		// --- Pyramid triangles (v3, v4, v8, v7) to v9 ---
				// v3  = ( 1.5f,  -8.5f, -2.0f)
				// v4  = (-1.5f,  -8.5f, -2.0f)
				// v7  = ( 1.5f, -8.5f, 0.0f)
				// v8  = (-1.5f, -8.5f, 0.0f)
				// v9  = ( 0.0f,  -7.75f, -1.0f)
		 // v3–v4–v9
		 1.5f,  -8.5f, -2.0f,
		-1.5f,  -8.5f, -2.0f,
		 0.0f,  -7.75f, -1.0f,

		 // v4–v8–v9
		-1.5f,  -8.5f, -2.0f,
		-1.5f, -8.5f, 0.0f,
		 0.0f,  -7.75f, -1.0f,

		 // v8–v7–v9
		 -1.5f, -8.5f, 0.0f,
		 1.5f, -8.5f, 0.0f,
		 0.0f,  -7.75f, -1.0f,

		 // v7–v3–v9
		 1.5f, -8.5f, -2.0f,
		 1.5f, -8.5f, 0.0f,
		 0.0f, -7.75f, -1.0f,
	};

	GLfloat a = 0.4f;
	static const GLfloat color[] = {
	// --- Base triangles (12 triangles, 6 colors) ---

    // Bottom face - Triangle 1 (ροδακινί)
	1.0f, 0.85f, 0.75f, 1.0f,
	1.0f, 0.85f, 0.75f, 1.0f,
	1.0f, 0.85f, 0.75f, 1.0f,

	// Bottom face - Triangle 2 (ροδακινί)
	1.0f, 0.85f, 0.75f, 1.0f,
	1.0f, 0.85f, 0.75f, 1.0f,
	1.0f, 0.85f, 0.75f, 1.0f,

	// Top face - Triangle 1 ( κόκκινο )
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	// Top face - Triangle 2 ( κόκκινο )
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	// Front - Triangle 1 ( μπλε )
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	// Front - Triangle 2 ( μπλε )
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	// Back - Triangle 1 ( πράσινο )
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	// Back - Triangle 2 ( πράσινο )
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	// Left - Triangle 1 ( πορτοκαλί )
	1.0f, 0.5f, 0.0f, 1.0f,
	1.0f, 0.5f, 0.0f, 1.0f,
	1.0f, 0.5f, 0.0f, 1.0f,

	// Left - Triangle 2 ( πορτοκαλί )
	1.0f, 0.5f, 0.0f, 1.0f,
	1.0f, 0.5f, 0.0f, 1.0f,
	1.0f, 0.5f, 0.0f, 1.0f,

	// Right - Triangle 1 ( μωβ )
	0.7f, 0.0f, 0.7f, 1.0f,
	0.7f, 0.0f, 0.7f, 1.0f,
	0.7f, 0.0f, 0.7f, 1.0f,

	// Right - Triangle 2 ( μωβ )
	0.7f, 0.0f, 0.7f, 1.0f,
	0.7f, 0.0f, 0.7f, 1.0f,
	0.7f, 0.0f, 0.7f, 1.0f,

	//Pyramid triangles (4 triangles, different colors)

	// v3–v4–v9  ( ανοιχτό μπλε )
	0.5f, 0.5f, 1.0f, 1.0f,
	0.5f, 0.5f, 1.0f, 1.0f,
	0.5f, 0.5f, 1.0f, 1.0f,

	// v4–v8–v9  ( χρυσό )
	0.8f, 0.7f, 0.2f, 1.0f,
	0.8f, 0.7f, 0.2f, 1.0f,
	0.8f, 0.7f, 0.2f, 1.0f,

	// v8–v7–v9  ( σμαραγδί )
	0.0f, 0.6f, 0.3f, 1.0f,
	0.0f, 0.6f, 0.3f, 1.0f,
	0.0f, 0.6f, 0.3f, 1.0f,

	// v7–v3–v9  ( γαλάζιο )
	0.0f, 0.7f, 1.0f, 1.0f,
	0.0f, 0.7f, 1.0f, 1.0f,
	0.0f, 0.7f, 1.0f, 1.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// to camera function

		camera_function();

		//==================== (iii) ENEMY LOGIC =====================
		
		double now = glfwGetTime();
		float delta = now - lastTime;
		lastTime = now;

		if (!gameOver) {

			float moveStep = 0.0f;

			// Επιλογή κίνησης ανάλογα με τη φάση 
			switch (movementPhase) {
			case 0: // Φάση: Μετακίνηση 5 βήματα ΔΕΞΙΑ
				moveStep = horizontalSpeed * delta;
				if (distMovedInPhase + moveStep >= 5.0f) {
					moveStep = 5.0f - distMovedInPhase; // Διόρθωση για να μην ξεπεράσουμε το 5
					movementPhase = 1; // Επόμενη φάση
					distMovedInPhase = 0.0f;
				}
				else {
					distMovedInPhase += moveStep;
				}
				for (int i = 0; i < 5; i++) cubeB_Positions[i].x += moveStep;
				break;

			case 1: // Φάση: Επιστροφή στο κέντρο (από δεξιά προς αριστερά)
				moveStep = horizontalSpeed * delta;
				if (distMovedInPhase + moveStep >= 5.0f) {
					moveStep = 5.0f - distMovedInPhase;
					movementPhase = 2;
					distMovedInPhase = 0.0f;
				}
				else {
					distMovedInPhase += moveStep;
				}
				for (int i = 0; i < 5; i++) cubeB_Positions[i].x -= moveStep;
				break;

			case 2: // Φάση: Μετακίνηση 5 βήματα ΑΡΙΣΤΕΡΑ
				moveStep = horizontalSpeed * delta;
				if (distMovedInPhase + moveStep >= 5.0f) {
					moveStep = 5.0f - distMovedInPhase;
					movementPhase = 3;
					distMovedInPhase = 0.0f;
				}
				else {
					distMovedInPhase += moveStep;
				}
				for (int i = 0; i < 5; i++) cubeB_Positions[i].x -= moveStep;
				break;

			case 3: // Φάση: Επιστροφή στο κέντρο (από αριστερά προς δεξιά)
				moveStep = horizontalSpeed * delta;
				if (distMovedInPhase + moveStep >= 5.0f) {
					moveStep = 5.0f - distMovedInPhase;
					movementPhase = 4;
					distMovedInPhase = 0.0f;
				}
				else {
					distMovedInPhase += moveStep;
				}
				for (int i = 0; i < 5; i++) cubeB_Positions[i].x += moveStep;
				break;

			case 4: // Φάση: Μετακίνηση προς τα ΚΑΤΩ (Fall) 
				// Χρησιμοποιούμε το fallSpeed που έχεις ήδη ή σταθερή κίνηση
				moveStep = fallSpeed * delta;

				// Στόχος είναι να κατέβουν μισό μέγεθος κύβου (1.0f) πριν ξαναρχίσουν τα πλάγια
				if (distMovedInPhase + moveStep >= dropTarget) {
					moveStep = dropTarget - distMovedInPhase;
					movementPhase = 0; // Επιστροφή στην αρχή του κύκλου (Δεξιά)
					distMovedInPhase = 0.0f;
				}
				else {
					distMovedInPhase += moveStep;
				}
				for (int i = 0; i < 5; i++) cubeB_Positions[i].y -= moveStep;
				break;
			}
		}
		// Move bullets
		for (auto& b : bullets) {
			if (b.active)
				b.pos.y += bulletSpeed * delta;
		}

		// PAUSE
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			paused = !paused;    
			glfwWaitEventsTimeout(0.15);  // μικρό debounce
		}

		if (paused) {
			glfwSwapBuffers(window);
			glfwPollEvents();
			continue;   // Μην κάνει update τίποτα άλλο
		}

		// SPEED UP
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			fallSpeed += 0.001f;
		}

		// SLOW DOWN
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			fallSpeed -= 0.001f;
			if (fallSpeed < 0.001f) fallSpeed = 0.001f;
		}

		// RESET
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {

			movementPhase = 0;
			distMovedInPhase = 0.0f;
			// reset character
			A_offset_x = 0.0f;

			// reset enemies
			for (int i = 0; i < 5; i++) {
				cubeB_Positions[i] = glm::vec3(-9.0f + 4.0f * i, 10.0f, 0.0f);
				cubeB_Alive[i] = true;
			}

			// reset bullets
			bullets.clear();

			// reset game flags
			gameOver = false;
			paused = false;

			// reset fall speed
			fallSpeed = 0.2f;
			glfwSetWindowTitle(window, u8"Εργασία 1Γ – 2025 – Καταστροφέας");// allagh gia na mhn sumphptei an ginei win
			glfwWaitEventsTimeout(0.2);
		}

		// for Game Over

		for (int i = 0; i < 5; i++) {
			if (cubeB_Positions[i].y <= -7.75f) {
				gameOver = true;
				std::cout << "GAME OVER" << std::endl;
			}
		}

		//================================================================
		// pros8esh tou game over gia to 1-C
		if (!gameOver) {
			// Movement of character A
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
				A_offset_x += 0.005f;  // Δεξια
			}

			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
				A_offset_x -= 0.005f;  // Αριστερα
			}
			// gia na mhn feugei ektos oriwn
			A_offset_x = glm::clamp(A_offset_x, -8.0f, 8.0f);

			// MODEL gia  A
			glm::mat4 ModelA = glm::translate(glm::mat4(1.0f), glm::vec3(A_offset_x, 0.0f, 0.0f));

			glm::mat4 MVP_A = ProjectionMatrix * ViewMatrix * ModelA;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_A[0][0]);

			//==== to bazw mesa sto mesa sto loop giati to modelA einai topikh metablhth
			// Shoot bullets
			double nowTime = glfwGetTime();
			if (!gameOver && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
				if (nowTime - lastBulletTime >= 0.5) {   // 0.5 second cooldown

					glm::vec4 start = ModelA * glm::vec4(0.0f, -7.75f, -1.0f, 1.0f);
					bullets.push_back({ glm::vec3(start), true });

					lastBulletTime = nowTime;  // reset cooldown timer
				}
			}
			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

			// Draw triangles 
			glDrawArrays(GL_TRIANGLES, 0, 48);
			
		}

		// ---------------- DRAW ENEMY CUBES (SHAPE B) -------------------
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		for (int i = 0; i < 5; i++) {

			if (!cubeB_Alive[i])
				continue;

			glm::mat4 ModelB = glm::translate(glm::mat4(1.0f), cubeB_Positions[i]);
			glm::mat4 MVP_B = ProjectionMatrix * ViewMatrix * ModelB;
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_B[0][0]);

			glDrawArrays(GL_TRIANGLES, i * 36, 36);
		}
		// ----------------------------------------------------------------
		for (auto& b : bullets) {
			if (!b.active) continue;
			
			for (int i = 0; i < 5; i++) {
				if (!cubeB_Alive[i]) continue;

				glm::vec3 c = cubeB_Positions[i];

				bool hit =
					b.pos.x >= c.x && b.pos.x <= c.x + 2.0f &&
					b.pos.y >= c.y && b.pos.y <= c.y + 2.0f &&
					b.pos.z <= c.z && b.pos.z >= c.z - 2.0f;

				if (hit) {
					cubeB_Alive[i] = false; 
					b.active = false;
				}
			}
			// === CHECK FOR WIN ===
			bool allDead = true;
			for (int i = 0; i < 5; i++) {
				if (cubeB_Alive[i]) {
					allDead = false;
					break;
				}
			}

			if (allDead && !gameOver) {
				gameOver = true;
				glfwSetWindowTitle(window, "YOU WIN!");
			}
			
			glm::mat4 MB = glm::translate(glm::mat4(1.0f), b.pos);
			glm::mat4 MVP_B = ProjectionMatrix * ViewMatrix * MB;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_B[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, bulletBuffer);  
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);    // xrhsh tou color buffer twn cubes giati einai idio
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glDrawArrays(GL_TRIANGLES, 0, 48);
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	// Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_1) != GLFW_PRESS &&
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(window));

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
