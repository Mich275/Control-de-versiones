/*
Práctica 6: Texturizado
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dado_10carasTexture; 
Texture logofiTexture;
Texture discoTexture;
Texture ruedaTexture;
Texture cofreTexture;
Texture parabrisasTexture;

Model carro_M; 
Model Llanta_M;
Model Dado_M;
Model parabrisas_M;
Model cofre_M;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";




//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}



void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);


	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		//caras arriba
		// front
		0, 1, 2,

		// back
		3, 4, 5,

		// left
		6, 7 , 8,
		
		// top
		9, 10, 11,

		// right
		12, 13, 14,

		//caras abajo
		// front
		15, 16, 17,

		// back
		18, 19, 20,

		// left
		21, 22, 23,

		// top
		24, 25, 26,

		// right
		27, 28, 29,

		// right

	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {

		//caras arriba
		
		// front
		//x		  y		  z		       S		T			NX		NY		NZ
		0.0f,     2.0f,   0.0f,	       0.25f,   0.85f,		0.0f,	0.0f,	-1.0f,	//0
		0.0f,     0.0f,   -2.0f,	   0.34f,	0.65f,		0.0f,	0.0f,	-1.0f,	//1
		1.902f,   0.0f,   -0.618f,	   0.15f,	0.65f,		0.0f,	0.0f,	-1.0f,	//2
			
		// right
		//x		  y		  z		       S		T
		0.0f,     2.0f,   0.0f,	       0.39f,   0.85f,		-1.0f,	0.0f,	0.0f,
		1.902f,   0.0f,   -0.618f,	   0.51f,	0.65f,		-1.0f,	0.0f,	0.0f,
		1.176f,   0.0f,   1.618f,	   0.28f,	0.65f,		-1.0f,	0.0f,	0.0f,
		
		// back
		//x		  y		  z		       S		T
		0.0f,     2.0f,   0.0f,	       0.57f,   0.85f,		0.0f,	0.0f,	1.0f,
		1.176f,   0.0f,   1.618f,	   0.68f,	0.65f,		0.0f,	0.0f,	1.0f,
		-1.176f,  0.0f,   1.618f,	   0.47f,	0.65f,		0.0f,	0.0f,	1.0f,
		

		// left
		//x		  y		  z		       S		T
		0.0f,     2.0f,   0.0f,	       0.74f,   0.86f,		1.0f,	0.0f,	0.0f,
		-1.176f,  0.0f,   1.618f,	   0.85f,	0.65f,		1.0f,	0.0f,	0.0f,
		-1.902f,  0.0f,   -0.618f,	   0.62f,	0.65f,		1.0f,	0.0f,	0.0f,
		

		// bottom
		//x		  y		  z		       S		T
		0.0f,     2.0f,   0.0f,	       0.25f,   0.65f,		0.0f,	1.0f,	0.0f,
		-1.902f,  0.0f,   -0.618f,	   0.37f,	0.45f,		0.0f,	1.0f,	0.0f,
		 0.0f,    0.0f,   -2.0f,	   0.14f,	0.45f,		0.0f,	1.0f,	0.0f,
		
		 //caras abajo

		// front
		//x		  y		   z		   S		T			NX		NY		NZ
		0.0f,     -2.0f,   0.0f,	   0.4f,    0.65f,		0.0f,	0.0f,	-1.0f,	//0
		0.0f,     0.0f,   -2.0f,	   0.51f,	0.45f,		0.0f,	0.0f,	-1.0f,	//1
		1.902f,   0.0f,   -0.618f,	   0.3f,	0.45f,		0.0f,	0.0f,	-1.0f,	//2

		// right
		//x		  y		   z		   S		T
		0.0f,     -2.0f,   0.0f,	   0.57f,   0.65f,   	-1.0f,	0.0f,	0.0f,
		1.902f,   0.0f,    -0.618f,	   0.44f,   0.45f,		-1.0f,	0.0f,	0.0f,
		1.176f,   0.0f,    1.618f,	   0.68f,   0.45f,		-1.0f,	0.0f,	0.0f,

		// back
		//x		  y		   z		   S		T
		0.0f,     -2.0f,   0.0f,	   0.73f,   0.65f,		0.0f,	0.0f,	1.0f,
		1.176f,   0.0f,    1.618f,	   0.84f,	0.45f,		0.0f,	0.0f,	1.0f,
		-1.176f,  0.0f,    1.618f,	   0.63f,	0.45f,		0.0f,	0.0f,	1.0f,


		// left
		//x		  y		   z		   S		T
		0.0f,     -2.0f,   0.0f,	   0.392f,  0.438f,		1.0f,	0.0f,	0.0f,
		-1.176f,  0.0f,    1.618f,	   0.5f,	0.25f,		1.0f,	0.0f,	0.0f,
		-1.902f,  0.0f,    -0.618f,	   0.28f,	0.25f,		1.0f,	0.0f,	0.0f,


		// bottom
		//x		  y		   z		   S		T
		0.0f,     -2.0f,   0.0f,	   0.55f,   0.44f,		0.0f,	1.0f,	0.0f,
		-1.902f,  0.0f,    -0.618f,	   0.75f,	0.25f,		0.0f,	1.0f,	0.0f,
		 0.0f,    0.0f,    -2.0f,	   0.39f,	0.25f,		0.0f,	1.0f,	0.0f,
		
		

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 240, 30);
	meshList.push_back(dado);

}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png"); 
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();
	dado_10carasTexture = Texture("Textures/numeros-1-al-10.png");
	dado_10carasTexture.LoadTextureA(); 
	discoTexture = Texture("Textures/discoR.png");
	discoTexture.LoadTextureA();
	ruedaTexture = Texture("Textures/rueda.png");
	ruedaTexture.LoadTextureA();
	cofreTexture = Texture("Textures/cofre.png");
	cofreTexture.LoadTextureA();
	parabrisasTexture = Texture("Textures/cruz_ojos.png");
	
	carro_M = Model();
	carro_M.LoadModel("Models/carro.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta.obj");
	parabrisas_M = Model();
	parabrisas_M.LoadModel("Models/parabrisas.obj");
	cofre_M = Model();
	cofre_M.LoadModel("Models/cofre.obj"); 

	
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();

		/*Reporte de práctica :
		Ejercicio 1: Crear un dado dec 10 caras y texturizarlo por medio de código
		Ejercicio 2: Importar el modelo de su coche con sus 4 llantas acomodadas y 
		tener texturizadas las 4 llantas (diferenciar caucho y rin) 
		Ejercicio 3: Texturizar la cara del personaje de la imagen tipo cars en el 
		espejo (ojos) y detalles en cofre y parrilla de su propio modelo de coche

		*/

		//Dado de Opengl
		//Ejercicio 1: Texturizar dado de 10 caras
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dado_10carasTexture.UseTexture();
		meshList[4]->RenderMesh();
		
		//Ejercicio 2
		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex() , -0.5f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		carro_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.0f, -0.5f, 1.5f));
		model = glm::rotate(model,-270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ruedaTexture.UseTexture();
		discoTexture.UseTexture();
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.4f, -0.5f, 1.5f));
		model = glm::rotate(model, -270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ruedaTexture.UseTexture();
		discoTexture.UseTexture();
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.0f, -0.5f, -1.5f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ruedaTexture.UseTexture();
		discoTexture.UseTexture();
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.4f, -0.5f, -1.5f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ruedaTexture.UseTexture();
		discoTexture.UseTexture();
		Llanta_M.RenderModel();

		//cofre 
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.6f, 0.90f, -1.45f)); 
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.54f, 0.54f, 0.52f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cofreTexture.UseTexture();
		cofre_M.RenderModel();

		//parabrisas
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.1f, 1.42f, -0.15f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		parabrisasTexture.UseTexture();
		parabrisas_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/