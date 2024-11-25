#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <vector>
#include "shader.h"
#include "shaderprogram.h"

/*=================================================================================================
	DOMAIN
=================================================================================================*/

// Window dimensions
const int InitWindowWidth = 800;
const int InitWindowHeight = 800;
int WindowWidth = InitWindowWidth;
int WindowHeight = InitWindowHeight;

// Last mouse cursor position
int LastMousePosX = 0;
int LastMousePosY = 0;

// Arrays that track which keys are currently pressed
bool key_states[256];
bool key_special_states[256];
bool mouse_states[8];

// Other parameters
bool draw_wireframe = false;

//bezier curve
int deltat = 20;

//bspline on
bool bspline = false;

//bool for editing vertices

bool one = false;
bool two = false;
bool three = false;
bool four = false;
bool five = false;
bool six = false;

void turnoff() {
	one = false;
	two = false;
	three = false;
	four = false;
	five = false;
	six = false;
}
/*=================================================================================================
	SHADERS & TRANSFORMATIONS
=================================================================================================*/

ShaderProgram PassthroughShader;
ShaderProgram PerspectiveShader;

glm::mat4 PerspProjectionMatrix(1.0f);
glm::mat4 PerspViewMatrix(1.0f);
glm::mat4 PerspModelMatrix(1.0f);

float perspZoom = 1.0f, perspSensitivity = 0.35f;
float perspRotationX = 0.0f, perspRotationY = 0.0f;

/*=================================================================================================
	OBJECTS
=================================================================================================*/

//VAO -> the object "as a whole", the collection of buffers that make up its data
//VBOs -> the individual buffers/arrays with data, for ex: one for coordinates, one for color, etc.

GLuint axis_VAO;
GLuint axis_VBO[2];

float axis_vertices[] = {
	//x axis
	-1.0f,  0.0f,  0.0f, 1.0f,
	1.0f,  0.0f,  0.0f, 1.0f,
	//y axis
	0.0f, -1.0f,  0.0f, 1.0f,
	0.0f,  1.0f,  0.0f, 1.0f,
	//z axis
	0.0f,  0.0f, -1.0f, 1.0f,
	0.0f,  0.0f,  1.0f, 1.0f
};

float axis_colors[] = {
	//x axis
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	//y axis
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	//z axis
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f
};

GLfloat bezierVerticies[] = {
	2.0f, 2.0f, 1.0f, 1.0f,

	2.0f, 7.0f, 0.0f, 1.0f,

	7.0f, 7.0f, 0.0f, 1.0f,

	7.0f, 2.0f, 0.0f, 1.0f,

	8.0f, 3.0f, 5.0f, 1.0f,

	10.0f, 10.0f, 6.0f, 1.0f,
};

GLfloat bezierColor[] = {
	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

};


GLfloat bsplineVerticies[] = {
	-2.0f, 2.0f, -1.0f, 1.0f,

	-2.0f, 7.0f, 0.0f, 1.0f,

	-6.0f, 4.0f, 3.0f, 1.0f,

	-9.0f, 2.0f, 0.0f, 1.0f,

	-7.0f, 3.0f, -3.0f, 1.0f,

	-13.0f, 5.0f, -6.0f, 1.0f,

	-9.0f, 6.0f, -4.0f, 1.0f,
};

GLfloat bsplineColor[] = {
	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,

};


GLint beizerEBO[] = {
	0,1,
	1,2,
	2,3,
	3,4,
	4,5,
};

GLint bsplineEBO[] = {
	0,1,
	1,2,
	2,3,
	3,4,
	4,5,
	5,6,
};

/*=================================================================================================
	HELPER FUNCTIONS
=================================================================================================*/

void movevertice(int index, char key) {
	if (bspline == false) {
		int axes = 0;
		if (key == 'q' or key == 'a') {
			axes = 1;
		}
		else if (key == 'e' or key == 'd') {
			axes = 2;
		}
		index = index * 4 + (axes);
		if (key == 'q') {
			bezierVerticies[index] += 1.0f;
		}
		else if (key == 'a') {
			bezierVerticies[index] -= 1.0f;
		}
		else if (key == 'w') {
			bezierVerticies[index] += 1.0f;
		}
		else if (key == 's') {
			bezierVerticies[index] -= 1.0f;
		}
		else if (key == 'e') {
			bezierVerticies[index] += 1.0f;
		}
		else if (key == 'd') {
			bezierVerticies[index] -= 1.0f;
		}
	}
	else {
		int axes = 0;
		if (key == 'q' or key == 'a') {
			axes = 1;
		}
		else if (key == 'e' or key == 'd') {
			axes = 2;
		}
		index = index * 4 + (axes);
		if (key == 'q') {
			bsplineVerticies[index] += 1.0f;
		}
		else if (key == 'a') {
			bsplineVerticies[index] -= 1.0f;
		}
		else if (key == 'w') {
			bsplineVerticies[index] += 1.0f;
		}
		else if (key == 's') {
			bsplineVerticies[index] -= 1.0f;
		}
		else if (key == 'e') {
			bsplineVerticies[index] += 1.0f;
		}
		else if (key == 'd') {
			bsplineVerticies[index] -= 1.0f;
		}
	}
	
};

std::vector<GLfloat> deCasteljauRecursive(const std::vector<GLfloat>& controlPoints, float t) {
	int numVertices = controlPoints.size() / 4;
	if (numVertices == 1) {
		// Base case: a single control point, no interpolation needed
		return controlPoints;
	}

	std::vector<GLfloat> nextLevel(4 * (numVertices - 1));
	for (int i = 0; i < numVertices - 1; ++i) {
		for (int k = 0; k < 4; ++k) {
			nextLevel[(i * 4) + k] = (1 - t) * (controlPoints[(i * 4) + k]) + t * (controlPoints[((i + 1) * 4) + k]);
		}
	}

	return deCasteljauRecursive(nextLevel, t);
}
std::vector<GLfloat> BezierCurve(const std::vector<GLfloat>& controlPoints, int numPoints) {
    std::vector<GLfloat> result;

    for (int i = 0; i <= numPoints; ++i) {
        float t = static_cast<float>(i) / numPoints;  // Calculate t in the range [0, 1]
        std::vector<GLfloat> point = deCasteljauRecursive(controlPoints, t);
        result.insert(result.end(), point.begin(), point.end());
    }

    return result;
}
std::vector<GLfloat> generateColorVector(const std::vector<GLfloat>& vertices) {
	size_t numVertices = vertices.size() / 4;
	std::vector<GLfloat> colors(numVertices * 4);

	for (size_t i = 0; i < numVertices; ++i) {
		colors[i * 4] = 1.0f;     // Red component
		colors[i * 4 + 1] = 0.0f; // Green component
		colors[i * 4 + 2] = 0.0f; // Blue component
		colors[i * 4 + 3] = 1.0f; // Alpha component
	}

	return colors;
}

std::vector<GLuint> generateEBO(const std::vector<GLfloat>& vertices) {
	std::vector<GLuint> indices;
	size_t numVertices = vertices.size() / 4;

	for (size_t i = 0; i < numVertices - 1; ++i) {
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	return indices;
}
std::vector<GLfloat> controlPoints(bezierVerticies, bezierVerticies + sizeof(bezierVerticies) / sizeof(GLfloat));

std::vector<GLfloat> Bezierpoint = BezierCurve(controlPoints, deltat);
std::vector<GLfloat> Beziercolor = generateColorVector(Bezierpoint);
std::vector<GLuint> BezierLineEBO = generateEBO(Bezierpoint);

std::vector<GLfloat> generateIntermidate(const std::vector<GLfloat>& vertices) {
	size_t numVertices = vertices.size() / 4;
	std::vector<GLfloat> intermidate(numVertices * 2 * 4 -4);

	// Calculate intermediate vertices
	for (size_t i = 0; i < (numVertices - 1); ++i) {
		intermidate[(i) *8 + 4] = 0.5f * vertices[i * 4] + 0.5f * vertices[(i + 1) * 4];
		intermidate[(i)*8 + 5] = 0.5f * vertices[i * 4 + 1] + 0.5f * vertices[(i + 1) * 4 + 1];
		intermidate[(i)*8 + 6] = 0.5f * vertices[i * 4 + 2] + 0.5f * vertices[(i + 1) * 4 + 2];
		intermidate[(i)*8 + 7] = 1.0f;

		intermidate[(i) * 8] = vertices[(i) * 4];
		intermidate[(i) * 8 + 1] = vertices[(i) * 4 + 1];
		intermidate[(i) * 8 + 2] = vertices[(i) * 4 + 2];
		intermidate[(i) * 8 + 3] = 1.0f;
	}
	intermidate.erase(intermidate.begin(), intermidate.begin() + 4);
	intermidate.erase(intermidate.end() - 4, intermidate.end());


	return intermidate;
}

std::vector<GLfloat> generateBsplinecurve(const std::vector<GLfloat>& controlPoints, int deltat) {
	size_t numVertices = controlPoints.size() / 4;
	size_t estimatedSize = numVertices * deltat;
	std::vector<GLfloat> Bsplinecurvepoints(estimatedSize * 4); // Allocate initial size
	size_t currentIndex = 0;

	std::vector<GLfloat> Bsplinequadpoints(12); // 3 vertices * 4 components

	for (size_t i = 0; i < numVertices - 2; i+=2) {
		Bsplinequadpoints[0] = controlPoints[i * 4];
		Bsplinequadpoints[1] = controlPoints[i * 4 + 1];
		Bsplinequadpoints[2] = controlPoints[i * 4 + 2];
		Bsplinequadpoints[3] = controlPoints[i * 4 + 3];

		Bsplinequadpoints[4] = controlPoints[(i + 1) * 4];
		Bsplinequadpoints[5] = controlPoints[(i + 1) * 4 + 1];
		Bsplinequadpoints[6] = controlPoints[(i + 1) * 4 + 2];
		Bsplinequadpoints[7] = controlPoints[(i + 1) * 4 + 3];

		Bsplinequadpoints[8] = controlPoints[(i + 2) * 4];
		Bsplinequadpoints[9] = controlPoints[(i + 2) * 4 + 1];
		Bsplinequadpoints[10] = controlPoints[(i + 2) * 4 + 2];
		Bsplinequadpoints[11] = controlPoints[(i + 2) * 4 + 3];

		std::vector<GLfloat> Bsplinecurvetemp = BezierCurve(Bsplinequadpoints, deltat);
		size_t tempSize = Bsplinecurvetemp.size();

		if (currentIndex + tempSize > Bsplinecurvepoints.size()) {
			Bsplinecurvepoints.resize(Bsplinecurvepoints.size() * 2); // Resize if needed
		}

		std::copy(Bsplinecurvetemp.begin(), Bsplinecurvetemp.end(), Bsplinecurvepoints.begin() + currentIndex);
		currentIndex += tempSize;
	}

	Bsplinecurvepoints.resize(currentIndex); // Shrink to actual size
	return Bsplinecurvepoints;
}

std::vector<GLfloat> BsplinecontrolPoints(bsplineVerticies, bsplineVerticies + sizeof(bsplineVerticies) / sizeof(GLfloat));
std::vector<GLfloat> Bsplineintermidates = generateIntermidate(BsplinecontrolPoints);
std::vector<GLfloat> BsplineCurve = generateBsplinecurve(Bsplineintermidates,deltat);
std::vector<GLfloat> Bsplinecolor = generateColorVector(BsplineCurve);
std::vector<GLuint> BsplineEBO = generateEBO(BsplineCurve);

// Initialization


void window_to_scene(int wx, int wy, float& sx, float& sy)
{
	sx = (2.0f * (float)wx / WindowWidth) - 1.0f;
	sy = 1.0f - (2.0f * (float)wy / WindowHeight);
}

/*=================================================================================================
	SHADERS
=================================================================================================*/

void CreateTransformationMatrices(void)
{
	// PROJECTION MATRIX
	PerspProjectionMatrix = glm::perspective<float>(glm::radians(60.0f), (float)WindowWidth / (float)WindowHeight, 0.01f, 1000.0f);

	// VIEW MATRIX
	glm::vec3 eye(0.0, 0.0, 2.0);
	glm::vec3 center(0.0, 0.0, 0.0);
	glm::vec3 up(0.0, 1.0, 0.0);

	PerspViewMatrix = glm::lookAt(eye, center, up);

	// MODEL MATRIX
	PerspModelMatrix = glm::mat4(1.0);
	PerspModelMatrix = glm::rotate(PerspModelMatrix, glm::radians(perspRotationX), glm::vec3(1.0, 0.0, 0.0));
	PerspModelMatrix = glm::rotate(PerspModelMatrix, glm::radians(perspRotationY), glm::vec3(0.0, 1.0, 0.0));
	PerspModelMatrix = glm::scale(PerspModelMatrix, glm::vec3(perspZoom));
}

void CreateShaders(void)
{
	// Renders without any transformations
	PassthroughShader.Create("./shaders/simple.vert", "./shaders/simple.frag");

	// Renders using perspective projection
	PerspectiveShader.Create("./shaders/persp.vert", "./shaders/persp.frag");

	//
	// Additional shaders would be defined here
	//
}

/*=================================================================================================
	BUFFERS
=================================================================================================*/

void CreateAxisBuffers(void)
{
	glGenVertexArrays(1, &axis_VAO); //generate 1 new VAO, its ID is returned in axis_VAO
	glBindVertexArray(axis_VAO); //bind the VAO so the subsequent commands modify it

	glGenBuffers(2, &axis_VBO[0]); //generate 2 buffers for data, their IDs are returned to the axis_VBO array

	// first buffer: vertex coordinates
	glBindBuffer(GL_ARRAY_BUFFER, axis_VBO[0]); //bind the first buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW); //send coordinate array to the GPU
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 0, made up of 4 floats
	glEnableVertexAttribArray(0);

	// second buffer: colors
	glBindBuffer(GL_ARRAY_BUFFER, axis_VBO[1]); //bind the second buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_colors), axis_colors, GL_STATIC_DRAW); //send color array to the GPU
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //let GPU know this is attribute 1, made up of 4 floats
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); //unbind when done

	//NOTE: You will probably not use an array for your own objects, as you will need to be
	//      able to dynamically resize the number of vertices. Remember that the sizeof()
	//      operator will not give an accurate answer on an entire vector. Instead, you will
	//      have to do a calculation such as sizeof(v[0]) * v.size().
}

GLuint bezier_VAO;
GLuint bezier_VBO[3];
void CreateBezierBuffers(void)
{
	glGenVertexArrays(1, &bezier_VAO); //generate 1 new VAO, its ID is returned in axis_VAO
	glBindVertexArray(bezier_VAO); //bind the VAO so the subsequent commands modify it

	glGenBuffers(3, &bezier_VBO[0]); //generate 2 buffers for data, their IDs are returned to the axis_VBO array

	// first buffer: vertex coordinates
	glBindBuffer(GL_ARRAY_BUFFER, bezier_VBO[0]); //bind the first buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(bezierVerticies), bezierVerticies, GL_STATIC_DRAW); //send coordinate array to the GPU
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0); //let GPU know this is attribute 0, made up of 4 floats
	glEnableVertexAttribArray(0);

	// second buffer: colors
	glBindBuffer(GL_ARRAY_BUFFER, bezier_VBO[1]); //bind the second buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(bezierColor), bezierColor, GL_STATIC_DRAW); //send color array to the GPU
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0); //let GPU know this is attribute 1, made up of 4 floats
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezier_VBO[2]); // Bind the third buffer using its ID
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(beizerEBO), beizerEBO, GL_STATIC_DRAW);


	glBindVertexArray(0); //unbind when done
}
GLuint bspline_VAO;
GLuint bspline_VBO[3];
void CreateBsplineBuffers(void)
{
	glGenVertexArrays(1, &bspline_VAO); //generate 1 new VAO, its ID is returned in axis_VAO
	glBindVertexArray(bspline_VAO); //bind the VAO so the subsequent commands modify it

	glGenBuffers(3, &bspline_VBO[0]); //generate 2 buffers for data, their IDs are returned to the axis_VBO array

	// first buffer: vertex coordinates
	glBindBuffer(GL_ARRAY_BUFFER, bspline_VBO[0]); //bind the first buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(bsplineVerticies), bsplineVerticies, GL_STATIC_DRAW); //send coordinate array to the GPU
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0); //let GPU know this is attribute 0, made up of 4 floats
	glEnableVertexAttribArray(0);

	// second buffer: colors
	glBindBuffer(GL_ARRAY_BUFFER, bspline_VBO[1]); //bind the second buffer using its ID
	glBufferData(GL_ARRAY_BUFFER, sizeof(bsplineColor), bsplineColor, GL_STATIC_DRAW); //send color array to the GPU
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0); //let GPU know this is attribute 1, made up of 4 floats
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bspline_VBO[2]); // Bind the third buffer using its ID
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bsplineEBO), bsplineEBO, GL_STATIC_DRAW);


	glBindVertexArray(0); //unbind when done
}
GLuint bezierLineVAO;
GLuint bezierLineVBO[3];
void BezierLineBuffers(void) {
	glGenVertexArrays(1, &bezierLineVAO);
	glBindVertexArray(bezierLineVAO);

	glGenBuffers(3, &bezierLineVBO[0]);


	glBindBuffer(GL_ARRAY_BUFFER, bezierLineVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, Bezierpoint.size() * sizeof(Bezierpoint[0]), Bezierpoint.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0); // No offset needed
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, bezierLineVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, Beziercolor.size() * sizeof(Beziercolor[0]), Beziercolor.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0); // No offset needed
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierLineVBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, BezierLineEBO.size() * sizeof(GLuint), BezierLineEBO.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint bsplineCurveVAO;
GLuint bsplineCurveVBO[3];
void bsplineCurveBuffers(void) {
	glGenVertexArrays(1, &bsplineCurveVAO);
	glBindVertexArray(bsplineCurveVAO);

	glGenBuffers(3, &bsplineCurveVBO[0]);


	glBindBuffer(GL_ARRAY_BUFFER, bsplineCurveVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, BsplineCurve.size() * sizeof(BsplineCurve[0]), BsplineCurve.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0); // No offset needed
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, bsplineCurveVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, Bsplinecolor.size() * sizeof(Bsplinecolor[0]), Bsplinecolor.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0); // No offset needed
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bsplineCurveVBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, BsplineEBO.size() * sizeof(GLuint), BsplineEBO.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}
//
//void CreateMyOwnObject( void ) ...
//

/*=================================================================================================
	CALLBACKS
=================================================================================================*/

//-----------------------------------------------------------------------------
// CALLBACK DOCUMENTATION
// https://www.opengl.org/resources/libraries/glut/spec3/node45.html
// http://freeglut.sourceforge.net/docs/api.php#WindowCallback
//-----------------------------------------------------------------------------

void idle_func()
{
	//uncomment below to repeatedly draw new frames
	glutPostRedisplay();
}

void reshape_func(int width, int height)
{
	WindowWidth = width;
	WindowHeight = height;

	glViewport(0, 0, width, height);
	glutPostRedisplay();
}
void rebuffer() {
	if (bspline == false) {
		std::vector<GLfloat> controlPoints(bezierVerticies, bezierVerticies + sizeof(bezierVerticies) / sizeof(GLfloat));

		Bezierpoint = BezierCurve(controlPoints, deltat);
		Beziercolor = generateColorVector(Bezierpoint);
		BezierLineEBO = generateEBO(Bezierpoint);


		glBindVertexArray(bezier_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, bezier_VBO[0]); //bind the first buffer using its ID
		glBufferData(GL_ARRAY_BUFFER, sizeof(bezierVerticies), bezierVerticies, GL_STATIC_DRAW); //send coordinate array to the GPU

		glBindBuffer(GL_ARRAY_BUFFER, bezier_VBO[1]); //bind the second buffer using its ID
		glBufferData(GL_ARRAY_BUFFER, sizeof(bezierColor), bezierColor, GL_STATIC_DRAW); //send color array to the GPU

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezier_VBO[2]); // Bind the third buffer using its ID
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(beizerEBO), beizerEBO, GL_STATIC_DRAW);



		glBindVertexArray(bezierLineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, bezierLineVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, Bezierpoint.size() * sizeof(Bezierpoint[0]), Bezierpoint.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, bezierLineVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, Beziercolor.size() * sizeof(Beziercolor[0]), Beziercolor.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierLineVBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, BezierLineEBO.size() * sizeof(GLuint), BezierLineEBO.data(), GL_STATIC_DRAW);
		glutPostRedisplay();
	}
	else {
		std::vector<GLfloat> BsplinecontrolPoints(bsplineVerticies, bsplineVerticies + sizeof(bsplineVerticies) / sizeof(GLfloat));
		Bsplineintermidates = generateIntermidate(BsplinecontrolPoints);
		BsplineCurve = generateBsplinecurve(Bsplineintermidates, deltat);
		Bsplinecolor = generateColorVector(BsplineCurve);
		BsplineEBO = generateEBO(BsplineCurve);

		CreateBsplineBuffers();
		bsplineCurveBuffers();
		glutPostRedisplay();
	}
}

void keyboard_func(unsigned char key, int x, int y)
{
	key_states[key] = true;

	switch (key)
	{
	case 'b':
	{
		draw_wireframe = !draw_wireframe;
		if (draw_wireframe == true)
			std::cout << "Wireframes on.\n";
		else
			std::cout << "Wireframes off.\n";
		break;
	}
	//decrease
	case 'i':
	{
		if (bspline == false) {
			if (deltat >= 5) {
				deltat -= 5;
			}
			Bezierpoint = BezierCurve(controlPoints, deltat);
			Beziercolor = generateColorVector(Bezierpoint);
			BezierLineEBO = generateEBO(Bezierpoint);


			glBindVertexArray(bezierLineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, bezierLineVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, Bezierpoint.size() * sizeof(Bezierpoint[0]), Bezierpoint.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, bezierLineVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, Beziercolor.size() * sizeof(Beziercolor[0]), Beziercolor.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierLineVBO[2]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, BezierLineEBO.size() * sizeof(GLuint), BezierLineEBO.data(), GL_STATIC_DRAW);
			glutPostRedisplay();
		}
		else {
			if (deltat >= 10) {
				deltat -= 5;
			}
			BsplineCurve = generateBsplinecurve(Bsplineintermidates, deltat);
			Bsplinecolor = generateColorVector(BsplineCurve);
			BsplineEBO = generateEBO(BsplineCurve);

			bsplineCurveBuffers();
			glutPostRedisplay();
		}

		break;
	}
	//increase
	case 'o':
	{
		if (bspline == false) {
			deltat += 5;
			Bezierpoint = BezierCurve(controlPoints, deltat);
			Beziercolor = generateColorVector(Bezierpoint);
			BezierLineEBO = generateEBO(Bezierpoint);


			glBindVertexArray(bezierLineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, bezierLineVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, Bezierpoint.size() * sizeof(Bezierpoint[0]), Bezierpoint.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, bezierLineVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, Beziercolor.size() * sizeof(Beziercolor[0]), Beziercolor.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierLineVBO[2]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, BezierLineEBO.size() * sizeof(GLuint), BezierLineEBO.data(), GL_STATIC_DRAW);
			glutPostRedisplay();
		}
		else {
			deltat += 5;
			BsplineCurve = generateBsplinecurve(Bsplineintermidates, deltat);
			Bsplinecolor = generateColorVector(BsplineCurve);
			BsplineEBO = generateEBO(BsplineCurve);
		
			bsplineCurveBuffers();
			glutPostRedisplay();
		}
		
		break;
	}
	// Exit on escape key press
	case 'p':{
		bspline = !bspline;
		break;
	}
	case 'q': {
		if (one == true) {
			movevertice(0, 'q');
			rebuffer();
		}
		else if (two == true) {
			movevertice(1, 'q');
			rebuffer();
		}
		else if (three == true) {
			movevertice(2, 'q');
			rebuffer();
		}
		else if (four == true) {
			movevertice(3, 'q');
			rebuffer();
		}
		else if (five == true) {
			movevertice(4, 'q');
			rebuffer();
		}
		else if (six == true) {
			movevertice(5, 'q');
			rebuffer();
		}
		break;
	}
	case 'a': {
		if (one == true) {
			movevertice(0, 'a');
			rebuffer();
		}
		else if (two == true) {
			movevertice(1, 'a');
			rebuffer();
		}
		else if (three == true) {
			movevertice(2, 'a');
			rebuffer();
		}
		else if (four == true) {
			movevertice(3, 'a');
			rebuffer();
		}
		else if (five == true) {
			movevertice(4, 'a');
			rebuffer();
		}
		else if (six == true) {
			movevertice(5, 'a');
			rebuffer();
		}
		break;
	}
	case 'w': {
		if (one == true) {
			movevertice(0, 'w');
			rebuffer();
		}
		else if (two == true) {
			movevertice(1, 'w');
			rebuffer();
		}
		else if (three == true) {
			movevertice(2, 'w');
			rebuffer();
		}
		else if (four == true) {
			movevertice(3, 'w');
			rebuffer();
		}
		else if (five == true) {
			movevertice(4, 'w');
			rebuffer();
		}
		else if (six == true) {
			movevertice(5, 'w');
			rebuffer();
		}
		break;
	}
	case 's': {
		if (one == true) {
			movevertice(0, 's');
			rebuffer();
		}
		else if (two == true) {
			movevertice(1, 's');
			rebuffer();
		}
		else if (three == true) {
			movevertice(2, 's');
			rebuffer();
		}
		else if (four == true) {
			movevertice(3, 's');
			rebuffer();
		}
		else if (five == true) {
			movevertice(4, 's');
			rebuffer();
		}
		else if (six == true) {
			movevertice(5, 's');
			rebuffer();
		}
		break;
	}
	case 'e': {
		if (one == true) {
			movevertice(0, 'e');
			rebuffer();
		}
		else if (two == true) {
			movevertice(1, 'e');
			rebuffer();
		}
		else if (three == true) {
			movevertice(2, 'e');
			rebuffer();
		}
		else if (four == true) {
			movevertice(3, 'e');
			rebuffer();
		}
		else if (five == true) {
			movevertice(4, 'e');
			rebuffer();
		}
		else if (six == true) {
			movevertice(5, 'e');
			rebuffer();
		}
		break;
	}
	case 'd': {
		if (one == true) {
			movevertice(0, 'd');
			rebuffer();
		}
		else if (two == true) {
			movevertice(1, 'd');
			rebuffer();
		}
		else if (three == true) {
			movevertice(2, 'd');
			rebuffer();
		}
		else if (four == true) {
			movevertice(3, 'd');
			rebuffer();
		}
		else if (five == true) {
			movevertice(4, 'd');
			rebuffer();
		}
		else if (six == true) {
			movevertice(5, 'd');
			rebuffer();
		}
		break;
	}
	case '1': {
		turnoff();
		one = true;
		break;
	}

	case '2': {
		turnoff();
		two = true;
		break;
	}

	case '3': {
		turnoff();
		three = true;
		break;
	}

	case '4': {
		turnoff();
		four = true;
		break;
	}

	case '5': {
		turnoff();
		five = true;
		break;
	}

	case '6': {
		turnoff();
		six = true;
		break;
	}

	

	case '\x1B':
	{
		exit(EXIT_SUCCESS);
		break;
	}
	}
}

void key_released(unsigned char key, int x, int y)
{
	key_states[key] = false;
}

void key_special_pressed(int key, int x, int y)
{
	key_special_states[key] = true;
}

void key_special_released(int key, int x, int y)
{
	key_special_states[key] = false;
}

void mouse_func(int button, int state, int x, int y)
{
	// Key 0: left button
	// Key 1: middle button
	// Key 2: right button
	// Key 3: scroll up
	// Key 4: scroll down

	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	if (button == 3)
	{
		perspZoom += 0.03f;
	}
	else if (button == 4)
	{
		if (perspZoom - 0.03f > 0.0f)
			perspZoom -= 0.03f;
	}

	mouse_states[button] = (state == GLUT_DOWN);

	LastMousePosX = x;
	LastMousePosY = y;
}

void passive_motion_func(int x, int y)
{
	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	LastMousePosX = x;
	LastMousePosY = y;
}

void active_motion_func(int x, int y)
{
	if (x < 0 || x > WindowWidth || y < 0 || y > WindowHeight)
		return;

	float px, py;
	window_to_scene(x, y, px, py);

	if (mouse_states[0] == true)
	{
		perspRotationY += (x - LastMousePosX) * perspSensitivity;
		perspRotationX += (y - LastMousePosY) * perspSensitivity;
	}

	LastMousePosX = x;
	LastMousePosY = y;
}

/*=================================================================================================
	RENDERING
=================================================================================================*/

void display_func(void)
{
	// Clear the contents of the back buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update transformation matrices
	CreateTransformationMatrices();


	// Choose which shader to user, and send the transformation matrix information to it
	PerspectiveShader.Use();
	PerspectiveShader.SetUniform("projectionMatrix", glm::value_ptr(PerspProjectionMatrix), 4, GL_FALSE, 1);
	PerspectiveShader.SetUniform("viewMatrix", glm::value_ptr(PerspViewMatrix), 4, GL_FALSE, 1);
	PerspectiveShader.SetUniform("modelMatrix", glm::value_ptr(PerspModelMatrix), 4, GL_FALSE, 1);

	// Drawing in wireframe?
	if (draw_wireframe == true)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Bind the axis Vertex Array Object created earlier, and draw it
	glBindVertexArray(axis_VAO);
	glDrawArrays(GL_LINES, 0, 6); // 6 = number of vertices in the object


	glBindVertexArray(bezier_VAO);
	glDrawElements(GL_LINES, sizeof(beizerEBO) / sizeof(GLint), GL_UNSIGNED_INT, 0);

	glPointSize(5.0f); // Set the point size (adjust as needed)
	glDrawElements(GL_POINTS, sizeof(beizerEBO) / sizeof(GLint), GL_UNSIGNED_INT, 0);
	//
	// Bind and draw your object here
	//
	glBindVertexArray(bezierLineVAO);
	glDrawElements(GL_LINES, BezierLineEBO.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	glBindVertexArray(bspline_VAO);
	glDrawElements(GL_LINES, sizeof(bsplineEBO) / sizeof(GLint), GL_UNSIGNED_INT, 0);
	glPointSize(5.0f); // Set the point size (adjust as needed)
	glDrawElements(GL_POINTS, sizeof(bsplineEBO) / sizeof(GLint), GL_UNSIGNED_INT, 0);


	glBindVertexArray(bsplineCurveVAO);
	glDrawElements(GL_LINES, BsplineEBO.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	// Unbind when done
	glBindVertexArray(0);

	// Swap the front and back buffers
	glutSwapBuffers();
}

/*=================================================================================================
	INIT
=================================================================================================*/

void init(void)
{
	GLfloat num = BsplineCurve[0];
	// Print some info
	std::cout << "Vendor:         " << glGetString(GL_VENDOR) << "\n";
	std::cout << "Renderer:       " << glGetString(GL_RENDERER) << "\n";
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
	std::cout << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";


	// Set OpenGL settings
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST); // enable depth test
	glEnable(GL_CULL_FACE); // enable back-face culling
	

	// Create shaders
	CreateShaders();

	// Create axis buffers
	CreateAxisBuffers();

	CreateBezierBuffers();

	BezierLineBuffers();

	CreateBsplineBuffers();

	bsplineCurveBuffers();
	//
	// Consider calling a function to create your object here
	//

	std::cout << "Finished initializing...\n\n";
}

/*=================================================================================================
	MAIN
=================================================================================================*/

int main(int argc, char** argv)
{
	// Create and initialize the OpenGL context
	glutInit(&argc, argv);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(InitWindowWidth, InitWindowHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("CSE-170 Computer Graphics");

	
	// Initialize GLEW
	GLenum ret = glewInit();
	if (ret != GLEW_OK) {
		std::cerr << "GLEW initialization error." << std::endl;
		glewGetErrorString(ret);
		return -1;
	}

	// Register callback functions
	glutDisplayFunc(display_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutKeyboardFunc(keyboard_func);
	glutKeyboardUpFunc(key_released);
	glutSpecialFunc(key_special_pressed);
	glutSpecialUpFunc(key_special_released);
	glutMouseFunc(mouse_func);
	glutMotionFunc(active_motion_func);
	glutPassiveMotionFunc(passive_motion_func);

	// Do program initialization
	init();

	// Enter the main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}
