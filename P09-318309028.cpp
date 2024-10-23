/*
Semestre 2024-1
Animación:
Sesión 1:
Simple o básica:Por banderas y condicionales (más de 1 transforomación geométrica se ve modificada
Sesión 2
Compleja: Por medio de funciones y algoritmos.
Adicional.- ,Textura Animada
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float movimientocartel;
float rotacioncartel;
float offsetMovCartel;
float offsetRotCartel; 

//dado
float movimientoD;
float movimientoDOffset;
float girarX;
float girarY;
float girarZ;
bool cae;

//moto
bool avanzaMoto;
float movimientoMoto;

float movimientoMoto2;
float movimientoMotoOffset;
float rotllantamoto;
float rotllantamotoOffset;
float moviMoto;

int generarNumeroAletorio;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;

//Model arco_M; 
//Model cartel_M;
Model dado10_M;
Model cuerpomoto_M;
Model llantamoto_M;
Model mapa_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función de calculo de normales por promedio de vértices 
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

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{

	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();



	/*arco_M = Model();
	arco_M.LoadModel("Models/arco.obj");
	cartel_M = Model();
	cartel_M.LoadModel("Models/cartel.obj");*/
	dado10_M = Model();
	dado10_M.LoadModel("Models/dado10.obj");
	cuerpomoto_M = Model();
	cuerpomoto_M.LoadModel("Models/cuerpomoto.obj");
	llantamoto_M = Model();
	llantamoto_M.LoadModel("Models/llantamoto.obj");
	mapa_M = Model();
	mapa_M.LoadModel("Models/mapa.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	movCoche = 0.0f;
	movOffset = 0.3f;
	rotllanta = 0.0f;
	rotllantaOffset = 5.0f;
	avanza = true;
	offsetMovCartel = 0.1f;
	offsetRotCartel = 3.0f;
	float prevTime = glfwGetTime(); //tiempo en que se tarda el ejecutar

	//dad0 
	movimientoD = 0.0;
	movimientoDOffset = 0.4;
	girarX = 1.5;
	girarY = 1.5;
	girarZ = 1.5;
	cae = false;
	float cae2 = false;

	//moto
	rotllantamoto = 1.0f;
	rotllantamotoOffset = 5.0f;
	movimientoMoto = 1.0f;
	movimientoMotoOffset = 0.3;
	avanzaMoto = true;
	moviMoto = 0.3f;

	
	generarNumeroAletorio = 0.0;
	srand(time(NULL));
	//int generarNumeroAletorio = 9;
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//para los num aleatorios
		if (mainWindow.getsKeys()[GLFW_KEY_R]) { //con R se gira el dado
			generarNumeroAletorio = rand() % 10 + 1;
			girarX = girarY = girarZ = movimientoD = 0.0;
			// Mostrar el número generado
			printf("Número entre 1 y 10: %d\n", generarNumeroAletorio);
		}

		//casos para caras del dado mediante numero aleatorio
		if (glfwGetTime() > 25) {
			if (movimientoD > -53.8) {  //donde inicia dado (arriba)
				cae = true;
				movimientoD -= movimientoDOffset * deltaTime;
			}
		}
		switch (generarNumeroAletorio) {
		case 1:
			if (glfwGetTime() > 25) {
				if (cae) {
					if (girarX <= 46 && girarY <= 53) {
						girarX += 3.0 * deltaTime;
						girarY += 3.44 * deltaTime;
					}
				}
			}
			break;
		case 2:
			if (glfwGetTime() > 25) {
				if (cae) {
					if (girarX <= 46) {
						girarX += 2.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarZ >= -20) {
								girarZ -= 2.0 * deltaTime;
							}
						}
					}
				}
			}/*
			girarX = 46.0;
			girarY = -18.0;
			girarZ = 0.0;*/
			break;

		case 3:
			if (glfwGetTime() > 25) {
				if (cae) {
					if (girarY <= 35) {
						girarY += 2.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarZ >= -48) {
								girarZ -= 8.0 * deltaTime;
							}
						}
					}
				}
			}/*
			girarY = 18.0;
			girarZ = -48.0;*/
			break;

		case 4:
			if (glfwGetTime() > 25) {
				if (cae) {
					if (girarX >= -47) {
						girarX -= 2.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarY <= 20) {
								girarY += 2.0 * deltaTime;
							}
						}
					}
				}
			}
			/*girarX = -47.0;
			girarY = 20.0;*/
			break;

		case 5:
			if (glfwGetTime() > 25) {
				if (cae) {
					if (girarX >= -45) {
						girarX -= 2.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarY >= -54) {
								girarY -= 2.5 * deltaTime;
							}
						}
					}
				}
			}/*
			girarX = -45.0;
			girarY = -54.0;*/
			break;

		case 6:
			/*if (glfwGetTime() > 25) {
				if (cae) {
					if (girarX <= 134) {
						girarX += 6.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarY <= 20) {
								girarY += 1.0 * deltaTime;
							}
						}
					}
				}
			}*/
			girarX = 134.0;
			girarY = 20.0;
			break;

		case 7:
			/*if (glfwGetTime() > 25) {
				if (cae) {
					if (girarX <= 137) {
						girarX += 6.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarY >= -55) {
								girarY -= 2.0 * deltaTime;
							}
						}
					}
				}
			}*/
			girarX = 137.0;
			girarY = -55.0;
			break;

		case 8:
			/*if (glfwGetTime() > 25) {
				if (cae) {
					if (girarZ >= -141) {
						girarZ -= 6.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarX >= -55) {
								girarX -= 2.0 * deltaTime;
							}
						}
					}
				}
			}*/
			girarZ = -141.0;
			girarX = -25.0;
			break;

		case 9:
			/*if (glfwGetTime() > 25) {
				if (cae) {
					if (girarZ >= -135) {
						girarZ -= 6.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarX >= -20) {
								girarX -= 2.0 * deltaTime;
							}
						}
					}
				}
			}*/
			girarX = -135.0;
			girarZ = -20.0;
			break;

		case 10:
			if (glfwGetTime() > 25) {
				if (cae) {
					if (girarZ <= 134) {
						girarZ += 6.0 * deltaTime;
						cae2 = true;
						if (cae2) {
							if (girarY <= 25) {
								girarY += 2.0 * deltaTime;
							}
						}
					}
				}
			}
			/*girarZ = 134.0;
			girarY = 25.0;*/
			break;
		}



		//para movimiento de la moto en el tablero empezando desde -x, -z
		if (glfwGetTime() > 20) {
			if (avanzaMoto)
			{
				if (movimientoMoto < 140.0f)
				{
					movimientoMoto += movimientoMotoOffset * deltaTime;
					rotllantamoto += rotllantamotoOffset * deltaTime;

				}

				else
				{
					avanzaMoto = !avanzaMoto;
				}
			}


			else
			{

				avanzaMoto = !avanzaMoto;
			}
		}
			

			//para cartel

			/*if (glfwGetTime() > 5) {
				if (avanza) {
					if (movimientocartel > -6.5) {
						movimientocartel -= offsetMovCartel * deltaTime;
					}
					else {
						if (rotacioncartel < 361) {
							rotacioncartel += offsetRotCartel * deltaTime;
						}
						else {
							avanza = !avanza;
						}
					}
				}
				else {
					if (movimientocartel < -0.1) {
						movimientocartel += offsetMovCartel * deltaTime;
					}
					else {
						if (rotacioncartel < 361) {
							rotacioncartel += offsetRotCartel * deltaTime;
						}
						else {
							avanza = !avanza;
						}
					}
				}
			}*/


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
			uniformEyePosition = shaderList[0].GetEyePositionLocation();
			uniformColor = shaderList[0].getColorLocation();

			//información en el shader de intensidad especular y brillo
			uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
			uniformShininess = shaderList[0].GetShininessLocation();

			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

			// luz ligada a la cámara de tipo flash
			//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
			glm::vec3 lowerLight = camera.getCameraPosition();
			lowerLight.y -= 0.3f;
			spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

			//información al shader de fuentes de iluminación
			shaderList[0].SetDirectionalLight(&mainLight);
			shaderList[0].SetPointLights(pointLights, pointLightCount);
			shaderList[0].SetSpotLights(spotLights, spotLightCount);



			glm::mat4 model(1.0);
			glm::mat4 modelaux(1.0);
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

			/*model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));

			pisoTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			*//*meshList[2]->RenderMesh();*/

			//Practica 9 
			//1.  Su dado de 10 caras cae sobre el piso, gira y cae en un número "random", se repite la tirada al presionar una tecla.
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, movimientoD + 60.5f, 0.0f));
			model = glm::rotate(model, girarX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, girarY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, girarZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			dado10_M.RenderModel();



			/*2. Por integrante del equipo elegirán un tipo de vehículo: terrestre o aéreo . Cada integrante del equipo creará un
			recorrido en el cual el vehículo se moverá alrededor de su tablero de Monopoly. Cada vehículo iniciará su recorrido a
			partir de una esquina diferente. (el vehículo terrestre no puede ser un carro o vehículo similar
			motorizado de 4 ruedas, se debe de tener movimiento de llantas o de hélices en sus vehículos.)
			*/

			//tablero monopoly
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			modelaux = model;
			model = glm::scale(model, glm::vec3(5.5f, 5.5f, 5.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			mapa_M.RenderModel();

			//moto 
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(movimientoMoto2 - 70.0f, 1.5f, movimientoMoto - 70.0f));
			modelaux = model;
			model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			cuerpomoto_M.RenderModel();

			//llanta moto delante 
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
			model = glm::rotate(model, rotllantamoto * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
			//color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			llantamoto_M.RenderModel();

			//llanta moto delante 
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.9f));
			model = glm::rotate(model, rotllantamoto * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
			//color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			llantamoto_M.RenderModel();



			//blending: transparencia o traslucidez
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			/*AgaveTexture.UseTexture();*/
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[3]->RenderMesh();
			glDisable(GL_BLEND);

			glUseProgram(0);

			mainWindow.swapBuffers();
		}

		return 0;
}
