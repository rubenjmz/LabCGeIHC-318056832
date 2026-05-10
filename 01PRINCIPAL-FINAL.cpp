// ====================================================================================
// PROYECTO FINAL - MOTOR GRFICO BASE
// ====================================================================================

// ====================================================================================
// 1. LIBRERAS Y DEPENDENCIAS
// Incluimos STB para cargar imgenes, GLM para matemticas de matrices y vectores,
// y las libreras estndar de OpenGL (GLEW/GLFW).
// ====================================================================================
#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <fstream>

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

// ====================================================================================
// 2. CLASES DEL MOTOR
// Cabeceras propias que encapsulan la complejidad de OpenGL.
// ====================================================================================
#include "Window.h"
#include "Camera.h"
#include "Shader_light.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"   
#include "Skybox.h" 
#include "Sphere.h"
#include "Model.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

const float toRadians = 3.14159265f / 180.0f;

// ====================================================================================
// 3. VARIABLES DE ANIMACIN Y CONTROL GLOBAL
// Variables que se actualizarn cada frame en el Game Loop (Offsets, rotaciones, etc).
// Compaeros: Agreguen aqu sus variables de fsicas o contadores.
// ====================================================================================
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0f;
float angulovaria = 0.0f;

// ==========================================
// VARIABLES AVATAR
// ==========================================
float tiempoAnimacion = 0.0f;
float velocidadAnimacion = 0.15f;
float rotacionAvatar = 0.0f;
float amplitudArticulacion = 35.0f;

// Variables de movimiento para el recorrido
float distanciaRecorrida = 0.0f;
float cuerpoPosX = 120.0f;
float cuerpoPosZ = -30.0f;
float cuerpoRotY = 0.0f; // Empezamos en 0 porque tus piezas ya tienen el 180 interno
bool estaMoviendo = true;
float cronometroEspera = 0.0f;
float velocidadTrayecto = 0.10f;

//INPUT DE KEYFRAMES
void inputKeyframes(bool* keys);
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, cicloh, ciclof, ciclot, ciclog, ciclor, contador = 0;





// Control de Tiempo (DeltaTime) para animaciones fluidas independientes de los FPS
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
// Variables para el control del Skybox animado
float temporizadorSkybox = 0.0f;
int indiceSkyboxActual = 0;

// ====================================================================================
// 4. DECLARACIN DE OBJETOS DE LA ESCENA
// Vectores, cmara, texturas, modelos y materiales que existirn en el entorno.
// ====================================================================================
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Skybox arregloSkyboxes[8];

// Materiales base
Material Material_brillante;
Material Material_opaco;

// Texturas y Modelos
Texture pisoTexture;
Texture OctaedroTexture;
Model Lampara_M;
Model Piso;
Model Prueba;
// MODELOS SIGUIENDO NUMEROS:
Model M01;	// VIAS
Model M02;	// ESTACION
Model M03;	// TREN
Model M04;	// ANDN
Model M05;	// EDIFICIO
Model M06;	// EDIFICIO
Model M07;	// --------
Model M08;	// EDIFICIO
Model M09;	// --------
Model M10;	// --------
Model M11;	// EDIFICIO
Model M12; // ESTRUCTURA
Model M13;	// --------
Model M14; // ESTRUCTURA
Model M15; // DECORACIN
Model M16; // NAVE
Model M17; // COCHE
Model M18; // DECORACIN
Model M19; // DECORACIN
Model M20; // DECORACIN
Model M21; // EDIFICIO
Model M22;	// ESTRUCTURA
Model M23;	// EDIFICIO
Model M24;	// RELOJ
Model M25; // LIBRERA
Model M26;	// VEGETACION
Model M27;
Model M28_1;
Model M28_2;
Model M28_3;
Model M28_4;
Model M29;
Model M30;
Model M31;
Model M32;
Model Nave_M;
Model Ala_D;
Model Ala_I;
// MODELOS LEAGUE OF LEGENDS:
Model LOL_00;
Model LOL_01;
//Model LOL_02;
Model LOL_03;
Model LOL_04;
Model LOL_05;
Model LOL_06;
Model LOL_07;
Model LOL_08;
Model LOL_09;
Model LOL_10;

// AVATAR
Model hw_cuerpo; 
Model hw_cabeza; 
Model hw_espada;
Model hw_PiernaDerecha;
Model hw_PiernaIzquierda;
Model hw_BrazoDerecho;
Model hw_BrazoIzquierdo;
// BANCA HW
Model hw_banca;


// ====================================================================================
// 5. SISTEMA DE ILUMINACIN
// Arreglos de luces que soporta el Shader.
// ====================================================================================
DirectionalLight arregloLucesDireccionales[8]; // Luces sincronizadas con el Skybox
PointLight pointLights[MAX_POINT_LIGHTS]; // Focos omnidireccionales
SpotLight spotLights[MAX_SPOT_LIGHTS];    // Linternas/Faros direccionales

// Rutas de Shaders
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";


// ====================================================================================
// FUNCIONES DE UTILIDAD Y GEOMETRA
// ====================================================================================

// Calcula normales dinmicas para mallas creadas a mano (Sombreado Phong suave)
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


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXavion = 2.0, posYavion = 2.0, posZavion = 0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f, rotAvion_x = 40.0f, rotAvion_y = 40.0f, rotAvion_z = -760.0f;
float giroAvion = 0;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 6;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float giroAvion;
	float giroAvionInc;
	float rotAvion_x;
	float rotAvion_y;
	float rotAvion_z;
	float rotAvion_xInc;
	float rotAvion_yInc;
	float rotAvion_zInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 7;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;
	KeyFrame[FrameIndex].rotAvion_x = rotAvion_x;
	KeyFrame[FrameIndex].rotAvion_y = rotAvion_y;
	KeyFrame[FrameIndex].rotAvion_z = rotAvion_z;
	//no volatil, agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}

void resetElements(void) //Tecla 0
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
	rotAvion_x = KeyFrame[0].rotAvion_x;
	rotAvion_y = KeyFrame[0].rotAvion_y;
	rotAvion_z = KeyFrame[0].rotAvion_z;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
	KeyFrame[playIndex].rotAvion_xInc = (KeyFrame[playIndex + 1].rotAvion_x - KeyFrame[playIndex].rotAvion_x) / i_max_steps;
	KeyFrame[playIndex].rotAvion_yInc = (KeyFrame[playIndex + 1].rotAvion_y - KeyFrame[playIndex].rotAvion_y) / i_max_steps;
	KeyFrame[playIndex].rotAvion_zInc = (KeyFrame[playIndex + 1].rotAvion_z - KeyFrame[playIndex].rotAvion_z) / i_max_steps;
}


void animate(void)
{
	//Movimiento del objeto // barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("entro aquí\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//printf("se quedó aqui\n");
			//printf("max steps: %f", i_max_steps);
			//Draw animation
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			rotAvion_x += KeyFrame[playIndex].rotAvion_xInc;
			rotAvion_y += KeyFrame[playIndex].rotAvion_yInc;
			rotAvion_z += KeyFrame[playIndex].rotAvion_zInc;
			i_curr_steps++;
		}

	}
}

// Función para vaciar todos los keyframes actuales a un TXT
void guardarKeyframesFile() {
	FILE* archivo;
	archivo = fopen("animacion_avion.txt", "w"); // "w" para sobreescribir

	if (archivo == NULL) {
		printf("Error al crear el archivo de keyframes.\n");
		return;
	}

	// Guardamos primero cuántos frames tenemos
	fprintf(archivo, "%d\n", FrameIndex);

	// Guardamos cada frame del arreglo
	for (int i = 0; i < FrameIndex; i++) {
		fprintf(archivo, "%f %f %f %f %f %f\n",
			KeyFrame[i].movAvion_x,
			KeyFrame[i].movAvion_y,
			KeyFrame[i].giroAvion,
			KeyFrame[i].rotAvion_x,
			KeyFrame[i].rotAvion_y,
			KeyFrame[i].rotAvion_z);
	}

	fclose(archivo);
	printf("Keyframes guardados exitosamente en animacion_avion.txt\n");
}

// Función para cargar los keyframes desde el TXT al iniciar
void cargarKeyframesFile() {
	FILE* archivo;
	archivo = fopen("animacion_avion.txt", "r"); // "r" para lectura

	if (archivo == NULL) {
		printf("No se encontró archivo de animación previa. Iniciando vacío.\n");
		return;
	}

	// Leemos el número de frames guardados
	fscanf(archivo, "%d\n", &FrameIndex);

	for (int i = 0; i < FrameIndex; i++) {
		fscanf(archivo, "%f %f %f %f %f %f\n",
			&KeyFrame[i].movAvion_x,
			&KeyFrame[i].movAvion_y,
			&KeyFrame[i].giroAvion,
			&KeyFrame[i].rotAvion_x,
			&KeyFrame[i].rotAvion_y,
			&KeyFrame[i].rotAvion_z);
	}

	fclose(archivo);
	printf("Se cargaron %d keyframes desde el archivo.\n", FrameIndex);
}

///////////////* FIN KEYFRAMES*////////////////////////////

// ====================================================================================
// FUNCIN PRINCIPAL (MAIN)
// ====================================================================================
int main(){
	// ====================================================================================
	// --- 1. Inicializacin de Ventana y Cmara ---
	// ====================================================================================
	mainWindow = Window(1800, 900); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CreateShaders();
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	// ====================================================================================
	// --- 2. Carga de Assets (Texturas y Modelos 3D) ---
	// ====================================================================================
	pisoTexture = Texture("Textures/piso.tga");				pisoTexture.LoadTextureA();
	//Lampara_M = Model();			Lampara_M.LoadModel("Models/LamparaOBJ.obj");
	Piso = Model();				Piso.LoadModel("Models/PISO.obj");
	
	Prueba = Model();			Prueba.LoadModel("Models/26.obj");
	//Prueba = Model();			Prueba.LoadModel("Models/09.glb");
	//Prueba = Model();			Prueba.LoadModel("Models/a.fbx");
	// Modelos segun numeros
	M01 = Model();				M01.LoadModel("Models/01.obj");
	M02 = Model();				M02.LoadModel("Models/02.obj");
	M03 = Model();				M03.LoadModel("Models/03.obj");
	M04 = Model();				M04.LoadModel("Models/04.obj");
	M05 = Model();				M05.LoadModel("Models/05.obj");
	M06 = Model();				M06.LoadModel("Models/06.obj");
	//M07 = Model();				M07.LoadModel("Models/07.obj");
	M08 = Model();				M08.LoadModel("Models/08.obj");
	//M09 = Model();				M09.LoadModel("Models/09.obj");
	//M10 = Model();				M10.LoadModel("Models/10.obj");
	//M11 = Model();				M11.LoadModel("Models/11.obj");
	M12 = Model();				M12.LoadModel("Models/12.obj");
	//M13 = Model();				M13.LoadModel("Models/13.obj");
	M14 = Model();				M14.LoadModel("Models/14.obj");
	M15 = Model();				M15.LoadModel("Models/15.obj");
	M16 = Model();				M16.LoadModel("Models/16.obj");
	M17 = Model();				M17.LoadModel("Models/17.obj");
	M18 = Model();				M18.LoadModel("Models/18.obj");
	M19 = Model();				M19.LoadModel("Models/19.obj");
	M20 = Model();				M20.LoadModel("Models/20.obj");
	M21 = Model();				M21.LoadModel("Models/21.obj");
	M22 = Model();				M22.LoadModel("Models/22.obj");
	M23 = Model();				M23.LoadModel("Models/23.obj");
	M24 = Model();				M24.LoadModel("Models/24.obj");
	M25 = Model();				M25.LoadModel("Models/25.obj");
	M26 = Model();				M26.LoadModel("Models/26.obj");
	M27 = Model();				M27.LoadModel("Models/27.obj");
	M28_1 = Model();			M28_1.LoadModel("Models/28-1.obj");
	M28_2 = Model();			M28_2.LoadModel("Models/28-2.obj");
	M28_3 = Model();			M28_3.LoadModel("Models/28-3.obj");
	M28_4 = Model();			M28_4.LoadModel("Models/28-4.obj");
	M29 = Model();				M29.LoadModel("Models/29.obj");
	M30 = Model();				M30.LoadModel("Models/30.obj");
	M31 = Model();				M31.LoadModel("Models/31.obj");
	M32 = Model();				M32.LoadModel("Models/32.obj");
	Nave_M = Model();			Nave_M.LoadModel("Models/nave.obj");
	Ala_D = Model();			Ala_D.LoadModel("Models/ala.obj");
	Ala_I = Model();			Ala_I.LoadModel("Models/ala_izq.obj");


	// Modelos League Of Legends
	LOL_00 = Model();			LOL_00.LoadModel("Models/LOL_00.obj");
	LOL_01 = Model();			LOL_01.LoadModel("Models/LOL_01.obj");
	//LOL_02 = Model();			LOL_02.LoadModel("Models/LOL_02.obj");
	LOL_03 = Model();			LOL_03.LoadModel("Models/LOL_03.obj");
	LOL_04 = Model();			LOL_04.LoadModel("Models/LOL_04.obj");
	LOL_05 = Model();			LOL_05.LoadModel("Models/LOL_05.obj");
	LOL_06 = Model();			LOL_06.LoadModel("Models/LOL_06.obj");
	LOL_07 = Model();			LOL_07.LoadModel("Models/LOL_07.obj");
	LOL_08 = Model();			LOL_08.LoadModel("Models/LOL_08.obj");
	LOL_09 = Model();			LOL_09.LoadModel("Models/LOL_09.obj");
	LOL_10 = Model();			LOL_10.LoadModel("Models/LOL_10.obj");


	//Avatar
	hw_cuerpo = Model();			hw_cuerpo.LoadModel("Models/hw_cuerpo.obj");
	hw_cabeza = Model();			hw_cabeza.LoadModel("Models/hw_cabeza.obj");
	hw_espada = Model();			hw_espada.LoadModel("Models/hw_espada.obj");
	hw_PiernaDerecha = Model();		hw_PiernaDerecha.LoadModel("Models/hw_PiernaDerecha.obj");
	hw_PiernaIzquierda = Model();	hw_PiernaIzquierda.LoadModel("Models/hw_PiernaIzquierda.obj");
	hw_BrazoDerecho = Model();		hw_BrazoDerecho.LoadModel("Models/hw_BrazoDerecho.obj");
	hw_BrazoIzquierdo = Model();	hw_BrazoIzquierdo.LoadModel("Models/hw_BrazoIzquierdo.obj");
	//Banca
	hw_banca.LoadModel("Models/hw_banca.obj");

	// ====================================================================================
	// --- 3. Carga de Skybox y Configuracin de Materiales ---
	// ====================================================================================
	std::string prefijosSkybox[8] = {
		"11Amanecer", "12Amanecer", "21Dia", "22Dia",
		"31Atardecer", "32Atardecer", "41Noche", "42Noche"
	};

	for (int i = 0; i < 8; i++) {
		std::vector<std::string> skyboxFaces;
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_px.png"); // +X Derecha
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_nx.png"); // -X Izquierda
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_ny.png"); // -Y Abajo (PRIMERO)
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_py.png"); // +Y Arriba (DESPUS)
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_pz.png"); // +Z Atrs
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_nz.png"); // -Z Frente

		arregloSkyboxes[i] = Skybox(skyboxFaces);
	}

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	// ====================================================================================
	// --- 4. Configuracin Inicial de Iluminacin ---
	// ====================================================================================
	// Luz Direccional (Global) sincronizada con los Skyboxes -----------------------------

	// 0: 11Amanecer (Amarillo claro)
	arregloLucesDireccionales[0] = DirectionalLight(1.0f, 0.9f, 0.6f, 0.1f, 0.1f, 0.0f, -1.0f, 0.0f);
	// 1: 12Amanecer (Casi blanco)
	arregloLucesDireccionales[1] = DirectionalLight(1.0f, 0.95f, 0.9f, 0.22f, 0.28f, 0.0f, -1.0f, 0.0f);
	// 2: 21Dia (Blanco)
	arregloLucesDireccionales[2] = DirectionalLight(1.0f, 1.0f, 1.0f, 0.44f, 0.44f, 0.0f, -1.0f, 0.0f);
	// 3: 22Dia (Blanco)
	arregloLucesDireccionales[3] = DirectionalLight(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.0f, -1.0f, 0.0f);
	// 4: 31Atardecer (Naranja Claro)
	arregloLucesDireccionales[4] = DirectionalLight(1.0f, 0.7f, 0.4f, 0.44f, 0.44f, 0.0f, -1.0f, 0.0f);
	// 5: 32Atardecer (Naranja)
	arregloLucesDireccionales[5] = DirectionalLight(1.0f, 0.5f, 0.1f, 0.22f, 0.28f, 0.0f, -1.0f, 0.0f);
	// 6: 41Noche (Azul oscuro)
	arregloLucesDireccionales[6] = DirectionalLight(0.1f, 0.1f, 0.5f, 0.1f, 0.1f, 0.0f, -1.0f, 0.0f);
	// 7: 42Noche (Morado oscuro)
	arregloLucesDireccionales[7] = DirectionalLight(0.3f, 0.1f, 0.4f, 0.05f, 0.05f, 0.0f, -1.0f, 0.0f);

	// 2. LUCES PUNTUALES (Point Lights) --------------------------------------------------
	unsigned int pointLightCount = 0;
	// Luz de la Lmpara Hextech (Luz puntual BLANCA)
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,	// Color Blanco (RGB)
		1.0f, 1.0f,						// Intensidad (alta para que ilumine bien)
		0.0f, 5.75f, -7.0f,				// Posicin: Misma X, Y, Z
		1.0f, 0.09f, 0.032f);			// Atenuacin para difuminado realista
	pointLightCount++;
	// LUZ DEL PEZ ABISAL (ndice 1)
	pointLights[1] = PointLight(0.0f, 0.0f, 1.0f,	// Color Azul Puro (RGB)
		1.0f, 1.0f,						// Intensidad
		0.0f, 0.0f, 0.0f,				// Posicin (Se sobreescribe en el while)
		1.0f, 0.7f, 1.8f);				// Atenuacin	
	pointLightCount++;
	// 3. LUCES SPOT (Linternas y Faros) --------------------------------------------------
	unsigned int spotLightCount = 0;
	// ndice 0: Faro AZUL (Coche) - Este ser sobreescrito en el while
	spotLights[0] = SpotLight(0.035f, 0.318f, 0.600f,	// Color Azul (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, -0.2f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		90.0f);
	spotLightCount++;
	// ndice 1: Faro AMARILLO 1 (Nave) - Este ser sobreescrito en el while 
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,		// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		30.0f);
	spotLightCount++;
	// ndice 2: Spotlight del Pez Abisal (Amarillo)
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f,	// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		40.0f);
	spotLightCount++;

	// ARREGLO AUXILIAR DE FAROS (Aqui ponemos distintas luces auxiliares para faros)
	SpotLight farosAuxiliares[8];
	// 0: Faro ROJO (Cono cerrado, llega lejos)
	farosAuxiliares[0] = SpotLight(1.0f, 0.0f, 0.0f,	// Color Rojo (RGB)
		1.0f, 1.0f,										// Intensidades
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,			// Posicin y Direccin 
		1.0f, 0.045f, 0.0075f,							// Atenuacin 
		30.0f);											// ngulo de apertura 
	// 1: Faro AMARILLO (Cono medio)
	farosAuxiliares[1] = SpotLight(1.0f, 1.0f, 0.0f,	// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		45.0f);
	// 2: Faro VERDE (Cono amplio, se atena ms rpido)
	farosAuxiliares[2] = SpotLight(0.0f, 1.0f, 0.0f,	// Color Verde (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.09f, 0.032f,
		60.0f);
	// 3: Faro CIAN 
	farosAuxiliares[3] = SpotLight(0.0f, 1.0f, 1.0f,	// Color Cian (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		75.0f);
	// 4: Faro AZUL (Original)
	farosAuxiliares[4] = SpotLight(0.0f, 0.0f, 1.0f,	// Color Azul (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		90.0f);
	// 5: Faro MAGENTA (Cono gigante)
	farosAuxiliares[5] = SpotLight(1.0f, 0.0f, 1.0f,	// Color Magenta (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f, -0.2f, 0.0f,
		1.0f, 0.022f, 0.0019f,
		120.0f);
	// 6: HELICOPTERO FARO AMARILLO PERO HACIA ADELANTE
	farosAuxiliares[6] = SpotLight(1.0f, 1.0f, 0.0f,	// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,
		1.0f, 0.09f, 0.032f,
		30.0f);
	// 7: HELICOPTERO FARO AMARILLO PERO HACIA ATRAS
	farosAuxiliares[7] = SpotLight(1.0f, 1.0f, 0.0f,	// Color Amarillo (RGB)
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		1.0f, 0.045f, 0.0075f,
		30.0f);

	// ====================================================================================
	// --- 5. Ubicaciones del Shader (Uniforms) ---
	// Estas variables se conectan directamente con la tarjeta grfica para enviarle datos de matrices, posiciones y colores.
	// ====================================================================================
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);



	// ====================================================================================
	// --- 6. Declaraciones movidas fuera del ciclo ---
	// ====================================================================================

	// Inicializacin de matrices y vectores de apoyo -------------------------------------
	GLfloat   now = 0.0f;
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 modelaux = glm::mat4(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);

	// ====================================================================================
	// VARIABLE DE CONTROL DE CMARAS
	// 0 = F (1ra Persona), 1 = G (3ra Persona), 2 = H (Area), 3 = J (Libre)
	// ====================================================================================
	int tipoCamara = 0; // Iniciamos en 0 (F) por default



	glm::vec3 posblackhawk = glm::vec3(-111.0, 100.0f, -34.19f);

	printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
	printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X");

	cargarKeyframesFile();

	// ====================================================================================
	// GAME LOOP (Ciclo Principal de Renderizado)
	// ====================================================================================
	while (!mainWindow.getShouldClose())
	{

		// Control de cuadros por segundo (Clculo del DeltaTime)
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		angulovaria += 0.5f * deltaTime;

		// --- LGICA DE CAMBIO DE SKYBOX ---
		temporizadorSkybox += deltaTime;
		if (temporizadorSkybox >= 250.0f) {
			temporizadorSkybox -= 250.0f;
			indiceSkyboxActual++;
			if (indiceSkyboxActual >= 8) {
				indiceSkyboxActual = 0;
			}
		}

		// ====================================================================================
		// FASE 0: FSICAS Y ANIMACIONES MATEMTICAS
		// Aqu deben ir todas las lgicas que modifiquen variables antes de enviarlas al modelo.
		// ====================================================================================
		/*
		// 1. Lgica Ping-Pong del Coche (Movimiento en X) ------------------------------------
		rotllanta += rotllantaOffset * deltaTime; // Rotacin continua de las llantas
		if (avanza) {
			// Si avanza, RESTAMOS para que viaje hacia la direccin correcta en X
			movCoche -= movOffset * deltaTime;
			if (movCoche <= -90.0f) {     // Lmite de 90 unidades (hacia los negativos)
				movCoche = -90.0f;        // Tope exacto
				avanza = false;           // Da la vuelta
			}
		}
		else {
			// Si retrocede, SUMAMOS para que regrese a 0
			movCoche += movOffset * deltaTime;
			if (movCoche >= 0.0f) {       // Rregresamos al punto de partida
				movCoche = 0.0f;          // Tope exacto
				avanza = true;            // Vuelve a avanzar dando la vuelta
			}
		}

		// 2. Lgica Ping-Pong de la Nave (Avanza hacia -20 en X) -----------------------------
		if (avanzaNave) {
			// Si avanza, RESTAMOS para que viaje hacia el -20
			movNave -= movOffsetNave * deltaTime;
			if (movNave <= -90.0f) {      // Lleg al lmite negativo
				movNave = -90.0f;
				avanzaNave = false;       // Da la vuelta para regresar
			}
		}
		else {
			// Si regresa, SUMAMOS para que vuelva al origen (0)
			movNave += movOffsetNave * deltaTime;
			if (movNave >= 0.0f) {        // Lleg al inicio
				movNave = 0.0f;
				avanzaNave = true;        // Vuelve a avanzar hacia el -20
			}
		}

		// 3. Lgica ZIGZAG de la Nave (Sube y Baja en Y) ------------------------------------
		if (subeNave) {
			alturaNave += velocidadAlturaNave * deltaTime;
			if (alturaNave >= 2.0f) {     // Lleg al tope superior
				alturaNave = 2.0f;
				subeNave = false;         // Ahora baja
			}
		}
		else {
			alturaNave -= velocidadAlturaNave * deltaTime;
			if (alturaNave <= -2.0f) {    // Lleg al tope inferior
				alturaNave = -2.0f;
				subeNave = true;          // Ahora sube
			}
		}

		// 4. Lgica Ping-Pong del Aleteo (Rotacin en Z) -------------------------------------
		if (aleteaSube) {
			rotAleteo += velAleteo * deltaTime;
			if (rotAleteo >= limiteAleteo) {  // Lleg al lmite superior de rotacin
				rotAleteo = limiteAleteo;
				aleteaSube = false;           // Ahora rota hacia abajo
			}
		}
		else {
			rotAleteo -= velAleteo * deltaTime;
			if (rotAleteo <= -limiteAleteo) { // Lleg al lmite inferior de rotacin
				rotAleteo = -limiteAleteo;
				aleteaSube = true;            // Ahora rota hacia arriba
			}
		}
		*/
		
		// Capturar eventos de la ventana
		glfwPollEvents();

		inputKeyframes(mainWindow.getsKeys());
		animate();

		// --- 1. ACTUALIZAR ESTADO DE CMARA ---
		if (mainWindow.getAccionF()) { tipoCamara = 0; mainWindow.apagarAccionF(); }
		if (mainWindow.getAccionG()) { tipoCamara = 1; mainWindow.apagarAccionG(); }
		if (mainWindow.getAccionH()) { tipoCamara = 2; mainWindow.apagarAccionH(); }
		if (mainWindow.getAccionJ()) { tipoCamara = 3; mainWindow.apagarAccionJ(); }

		// Leer el ratn una sola vez
		float deltaX = mainWindow.getXChange();
		float deltaY = mainWindow.getYChange();

		// --- 2. LGICA DEL RATN ---
		if (tipoCamara == 3) {
			// [J] CMARA LIBRE: Controla su propio giro
			camera.mouseControl(deltaX, deltaY);
		}
		else {
			// CMARAS ANCLADAS: El ratn gira la cintura/cuerpo del personaje
			// Ajusta el 0.3f si quieres que el ratn gire al personaje ms rpido o ms lento
			cuerpoRotY -= deltaX * 0.3f;
		}

		// --- 3. CLCULO DE VECTORES DIRECCIONALES DEL AVATAR ---
		glm::vec3 posAvatar(cuerpoPosX, 1.0f, cuerpoPosZ);
		float rotacionGlobal = 180.0f + cuerpoRotY;
		float radRot = rotacionGlobal * toRadians;

		// Vector que apunta hacia el frente del personaje
		glm::vec3 forwardAvatar(sin(radRot), 0.0f, cos(radRot));
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		// Vector que apunta hacia la derecha del personaje
		glm::vec3 rightAvatar = glm::normalize(glm::cross(up, forwardAvatar));

		// --- 4. CONTROL DE MOVIMIENTO (A, W, S, D) Y ANIMACIN ---
		bool isMoving = false;
		float velocity = 0.15f * deltaTime; //la velocidad de caminar

		if (tipoCamara == 3) {
			// Si es cmara libre, el WASD mueve a la cmara por el aire, no al avatar
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
		}
		else {
			// Si es cmara de jugador, el WASD mueve al avatar
			if (mainWindow.getsKeys()[GLFW_KEY_W]) {
				cuerpoPosX += forwardAvatar.x * velocity;
				cuerpoPosZ += forwardAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_S]) {
				cuerpoPosX -= forwardAvatar.x * velocity;
				cuerpoPosZ -= forwardAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_D]) {
				cuerpoPosX -= rightAvatar.x * velocity;
				cuerpoPosZ -= rightAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_A]) {
				cuerpoPosX += rightAvatar.x * velocity;
				cuerpoPosZ += rightAvatar.z * velocity;
				isMoving = true;
			}
		}

		// --- 5. ANIMACIN CONDICIONADA A MOVIMIENTO ---
		if (isMoving) {
			// Usamos una funcin seno basada en el tiempo para oscilar brazos/piernas de forma natural
			// 10.0f = Qu tan rpido mueve los brazos. 45.0f = Cuntos grados se levantan.
			rotacionAvatar = sin(glfwGetTime() * 10.0f) * 45.0f;
		}
		else {
			rotacionAvatar = 0.0f; // Personaje quieto con extremidades rectas
		}

		// --- 6. CLCULO FINAL DE LAS MATRICES DE VISTA ---
		glm::mat4 viewMatrix;
		glm::vec3 posCamara;
		glm::vec3 targetCamara;

		if (tipoCamara == 3) {
			viewMatrix = camera.calculateViewMatrix();
			posCamara = camera.getCameraPosition();
		}
		else {
			// Actualizamos la posicin base del avatar por si se movi
			posAvatar = glm::vec3(cuerpoPosX, 1.0f, cuerpoPosZ);

			if (tipoCamara == 0) { // F (Primera Persona)
				posCamara = posAvatar + glm::vec3(0.0f, 1.2f, 0.0f) + forwardAvatar * 1.2f;
				targetCamara = posCamara + forwardAvatar;
			}
			else if (tipoCamara == 1) { // G (Hombro / Tercera Persona)
				posCamara = posAvatar + glm::vec3(0.0f, 1.8f, 0.0f) - forwardAvatar * 5.0f + rightAvatar * 1.0f;
				targetCamara = posAvatar + glm::vec3(0.0f, 1.2f, 0.0f) + forwardAvatar * 5.0f;
			}
			else if (tipoCamara == 2) { // H (Area)
				posCamara = posAvatar + glm::vec3(0.0f, 25.0f, -0.1f);
				targetCamara = posAvatar;
			}
			viewMatrix = glm::lookAt(posCamara, targetCamara, up);
		}
		
		// Preparacin del Buffer y dibujo del fondo
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Skybox con soporte de cambio dinámico y cámaras
		arregloSkyboxes[indiceSkyboxActual].DrawSkybox(viewMatrix, projection);

		// Activacin del Shader Principal y recuperacin de sus Uniforms
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// CAMBIO AQU: Usamos viewMatrix y posCamara
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition, posCamara.x, posCamara.y, posCamara.z);

		// ====================================================================================
		// FASE 1: CLCULO DE JERARQUAS DE LUCES
		// Aqu enlazamos las posiciones de las luces direccionales (spots) a objetos dinmicos.
		// ====================================================================================



		// ====================================================================================
		// FASE 2: ENVO DE LUCES AL SHADER
		// Empaquetamos las luces calculadas y las enviamos a la GPU.
		// ====================================================================================
		shaderList[0].SetDirectionalLight(&arregloLucesDireccionales[indiceSkyboxActual]);
		unsigned int lucesPuntualesActivas = 0;
		// (La lgica para encender/apagar focos ira aqu, agregando al arreglo 'pointLights')
		shaderList[0].SetPointLights(pointLights, lucesPuntualesActivas);
		shaderList[0].SetSpotLights(spotLights, 0); // Modifica el 0 por tu contador de Spots activos
		// Limpieza de offsets y color para evitar arrastrar residuos visuales a los modelos
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// ====================================================================================
		// FASE 3: RENDERIZADO DE MODELOS OPACOS
		// Aqu se dibujan todos los elementos slidos de la escena.
		// Compaeros: Agreguen aqu sus modelos usando este formato: 
		// model = glm::translate... -> uniformMatrix -> Textura -> Material -> RenderModel()
		// ====================================================================================

		// PISO BASE GRIS
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[2]->RenderMesh();

		/*
		// PISTA ------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -2.1f, -1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Pista_M.RenderModel();

		// COCHE Y CARROCERA -----------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche, -1.0f, 0.0f));
		if (!avanza) model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CocheT.RenderModel();

		// Llantas del coche ------------------------------------------------------------------
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		// Trasera derecha
		model = glm::translate(modelaux, glm::vec3(4.3f, 0.4f, 3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT.RenderModel();
		// Trasera izquierda
		model = glm::translate(modelaux, glm::vec3(4.3f, 0.4f, -3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT.RenderModel();
		// Delantera derecha
		model = glm::translate(modelaux, glm::vec3(-6.0f, 0.4f, 3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT.RenderModel();
		// Delantera izquierda
		model = glm::translate(modelaux, glm::vec3(-6.0f, 0.4f, -3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT.RenderModel();

		// Cofre ------------------------------------------------------------------------------
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(modelaux, glm::vec3(-4.2f, 2.6f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getAperturaCofre()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CofreT.RenderModel();

		// NAVE Y SUS ALAS (Jerarqua Zigzag + Aleteo) ----------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movNave, 13.0f + alturaNave, 1.5f));
		if (!avanzaNave) {
			model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		NaveT.RenderModel();
		// ALA 1 (Costado Derecho)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f));
		model = glm::rotate(model, rotAleteo * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala1T.RenderModel();
		// ALA 2 (Costado Izquierdo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::rotate(model, -rotAleteo * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala2T.RenderModel();
		*/


		// LMPARA HEXTECH
		// ====================================================================================
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -2.0f, -7.0f));
		model = glm::scale(model, glm::vec3(0.10f, 0.10f, 0.10f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Lampara_M.RenderModel();

		// ESCENARIO BUENO
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);


		// PERSONAJES Y MODELOS LOL -------------------------------------------------------------
		// Ziggs
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-111.0, 0.0f, -34.19f));
		model = glm::rotate(model, 137 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_00.RenderModel();
		
		// Poro 1
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-89.01f, 0.96f, -41.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 2
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-49.56f, 0.91f, -33.35f));
		model = glm::rotate(model, 13 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 3
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-19.13f, 9.28f, 7.13f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 4
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(85.9f, 2.37f, -18.68f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poros Ahri -------------
		// Poro 5
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-135.0f, 0.0f, -49.0f));
		model = glm::rotate(model, -25 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 6
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-132.0f, 0.0f, -50.0f));
		model = glm::rotate(model, -15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 7
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-134.0f, 0.0f, -52.0f));
		//model = glm::rotate(model, -25 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 8
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-132.0f, 0.0f, -54.0f));
		model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 9
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-135.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 25 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();

		// Torreta 1
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-138.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_03.RenderModel();
		// Torreta 2
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-100.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_03.RenderModel();

		// Pinguim 1
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-90.0f, 0.0f, -76.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		// Pinguim 2
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-7.73f, 0.0f, -82.43f));
		model = glm::rotate(model, -211 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		// Pinguim 3
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-142.85f, 0.0f, -100.48f));
		model = glm::rotate(model, -33 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		// Pinguim 4
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -40.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		// Pinguim 5
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -45.0f));
		model = glm::rotate(model, 18 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		// Pinguim 6
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -50.0f));
		model = glm::rotate(model, -13 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();

		// Escurridizo 1
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-120.0f, 0.0f, -29.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_05.RenderModel();
		// Escurridizo 2
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-92.0f, 0.0f, -112.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_05.RenderModel();

		// Ward 1
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-89.0f, 0.0f, -47.0f));
		model = glm::rotate(model, 61 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_06.RenderModel();
		// Ward 2
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 0.0f, -19.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_06.RenderModel();

		// Maestra 1
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-104.42f, 9.65f, -34.16f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		// Maestra 2
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-107.71f, 9.65f, -30.91f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		// Maestra 3
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-104.42f, 9.65f, -27.65f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		// Maestra 4
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-101.35f, 9.65f, -30.91f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();

		// Martillo Jayce
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-114.0f, 0.0f, -25.0f));
		//model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_08.RenderModel();
		// Estatua Ahri
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 1.15f, -52.0f));
		//model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_09.RenderModel();
		// Base Ahri
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 0.0f, -52.0f));
		//model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_10.RenderModel();

		//Aquí va la nave con jerarquía de modelos, completar


		model = glm::mat4(1.0);
		posblackhawk = glm::vec3(posXavion + movAvion_x, 10.0f + posYavion + movAvion_y, posZavion);
		model = glm::translate(model, posblackhawk);
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//modelaux = model;
		model = glm::rotate(model, 180 * toRadians + (giroAvion)*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.25f));
		model = glm::rotate(model, rotAvion_x * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotAvion_y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotAvion_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_D.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::rotate(model, -rotAvion_x * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -rotAvion_y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotAvion_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_I.RenderModel();


		// PISO MODELO (.OBJ)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piso.RenderModel();
		
		// VIAS
		model = glm::mat4(1.0);
		//model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M01.RenderModel();
		
		
		// TRENES
		// Tren chico
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-6.5f, 0.0f, 7.26f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M03.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-55.0f, 0.0f, 7.26f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		
		// Tren largo
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(108.0f, 0.0f, -8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M03.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(157.0f, 0.0f, -8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(214.0f, 0.0f, -8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(270.0f, 0.0f, -8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		



		// Edificio Principa
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M02.RenderModel();

		
		// Barandal Edificio Principal
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M14.RenderModel();

		// Barandal Biblioteca
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -50.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M14.RenderModel();
		


		// BIblioteca
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M23.RenderModel();

		// Librera Aire libre
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-60.0f, 0.0f, -25.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M25.RenderModel();
		
		// Nave estacionada
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(55.0f, 0.0f, -45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M16.RenderModel();
		
		// Mini fuente
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-120.0f, 0.0f, -34.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M15.RenderModel();
		
		// VEGETACIN -----------------------------------------------------------------------
		
		// Pino 1 (Cerca de la Nave)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 2 (Cerca de la Biblioteca)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 3 (Cerca del Edificio Principal)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, 70.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();

		// Pino 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-175.0f, 0.0f, -105.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 5
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-170.0f, 0.0f, -10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 6
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-35.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 7
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(165.0f, 0.0f, 45.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Pino 8
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 0.0f, 35.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		


		
		// Cerezo 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-60.0f, 0.0f, 45.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(155.0f, 0.0f, -20.0f));
		model = glm::rotate(model, -75 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-90.0f, 0.0f, 20.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 5
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 0.0f, 75.0f));
		model = glm::rotate(model, 105 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 6
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-80.0f, 0.0f, -75.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Cerezo 7
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 105 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();

		// -----------------------------------------------------------------------------------

		
		// Coche 1 (Estacionado cerca del Edificio Principal)
		model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 55.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		// Coche 2 (Estacionado cerca de la Biblioteca)
		model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 55.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		// Coche 3 (Estacionado cerca del Edificio Principal)
		model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 55.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();



		// Coche 4 (Estacionado cerca del Edificio Principal)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 55.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		// Coche 5 (Estacionado cerca del Edificio Principal)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 55.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		// Coche 6 (Estacionado cerca de la biblioteca)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		// Coche 7 (Estacionado cerca de la biblioteca)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		
		
		// Botes de basura -----------------------------------------------------------------------
		// Bote 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.09f, 0.0f, -72.72f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		// Bote 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-87.15f, 0.0f, 19.36f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		// Bote 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.06f, 0.0f, -18.43f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		// Bote 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(87.51f, 0.0f, 73.59f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		
		
		// Bancas -----------------------------------------------------------------------
		// Banca 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-89.0f, 0.0f, -22.65f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-89.0f, 0.0f, -40.17f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();

		// Banca 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-41.42f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-68.03f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 5
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.45f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 6
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.84f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 7
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(67.01f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();

		// Banca 8
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.96f, 0.0f, -69.64f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 9
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.96f, 0.0f, -51.81f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();


		// Banca 10
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(88.41f, 0.0f, 22.38f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 11
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(88.41f, 0.0f, 40.21f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();


		// Banca 12
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.05f, 0.0f, 69.43f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 13
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.05f, 0.0f, 51.91f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();

		// Banca 14
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(41.35f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 15
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(67.95f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 16
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-40.54f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();

		// Banca 17
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.93f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// Banca 18
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-67.09f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		
		


		
		// Puesto Ziggs
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -30.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M22.RenderModel();

		// Estructura (Al lado del puesto Ziggs)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -30.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M12.RenderModel();
		
		
		// Estructura (Al lado de la nave)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, 0.0f, -30.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M11.RenderModel();
		
		
		// LINEA DE EDIFICIOS -----------------------------------------------------------------

		// Edificio No 3 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M05.RenderModel();
		// Edificio No 3 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M05.RenderModel();

		// Edificio No 5 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M06.RenderModel();
		// Edificio No 5 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M06.RenderModel();

		// Edificio No 7 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M08.RenderModel();
		// Edificio No 7 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M08.RenderModel();


		// Edificio No 1 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		// Edificio No 8 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		// Edificio No 1 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		// Edificio No 8 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, 0.0f, -120.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();


		// Edificio No 2 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_1.RenderModel();
		// Edificio No 2 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, -125.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_1.RenderModel();


		// Edificio No 6 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_3.RenderModel();
		// Edificio No 6 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, -125.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_3.RenderModel();


		// Edificio No 6 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_2.RenderModel();
		// Edificio No 6 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -125.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_4.RenderModel();
		
		// ------------------------------------------------------------------------------------




		// ====================================================================================


		// ====================================================================================
		// FASE 4: ELEMENTOS CON TRANSPARENCIA (BLENDING) Y TEXTURAS ANIMADAS
		// El "Blending" debe ocurrir AL FINAL para que las transparencias calculen
		// correctamente la profundidad sobre los modelos opacos ya dibujados.
		// ====================================================================================
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		/*
		// 2. AGAVE (Transparencia Esttica) --------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		*/

		glDisable(GL_BLEND); // Apagamos el blending al terminar esta fase
		// ====================================================================================

		// Desacoplamos el Shader para dejarlo limpio para el siguiente Frame


		// =============================================================
		// INSTANCIA HW - CUERPO (EL PADRE)
		// =============================================================
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(cuerpoPosX, 1.0f, cuerpoPosZ));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, cuerpoRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 modelCuerpo = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hw_cuerpo.RenderModel();

		// =============================================================
		// INSTANCIA HW - CABEZA (HIJO DEL CUERPO)
		// =============================================================
		model = modelCuerpo; // Partimos de la base del cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_cabeza.RenderModel();
		// =============================================================
		// INSTANCIA HW - ESPADA (HIJO DEL CUERPO)
		// =============================================================
		model = modelCuerpo; // Empezamos desde la posicin del cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -0.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_espada.RenderModel();
		// =============================================================
		// PIERNA DERECHA (Avanza 15)
		// =============================================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.18f, -0.6f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Tu rotacin base
		model = glm::rotate(model, rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // +35
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_PiernaDerecha.RenderModel();
		// =============================================================
		// PIERNA IZQUIERDA (Retrocede -15)
		// =============================================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-0.18f, -0.6f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // -35
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_PiernaIzquierda.RenderModel();
		// =============================================================
		// BRAZO DERECHO (Retrocede con pierna derecha)
		// =============================================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.37f, 0.2f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // -35
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_BrazoDerecho.RenderModel();
		// =============================================================
		// BRAZO IZQUIERDO (Avanza con pierna derecha)
		// =============================================================
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-0.37f, 0.2f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // +35
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_BrazoIzquierdo.RenderModel();

		// =============================================================
		// INSTANCIA DE LA BANCA HW
		// =============================================================
		model = glm::mat4(1.0f);
		// Posicin: X=0, Y=0 (suelo), Z=-5 (atrs del inicio del avatar)
		model = glm::translate(model, glm::vec3(120.0f, 0.0f, -25.0f));
		// Escala: Ajusta el 0.5f si la banca sale muy grande o muy pequea
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		// Rotacin: 180 grados para que mire hacia el frente si es necesario
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_banca.RenderModel();

		

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			//printf("movAvion_x es: %f\n", movAvion_x);
			//printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
		}
	}


	if (keys[GLFW_KEY_H])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += 1.0f;
			printf("movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			cicloh = 0;
			printf("Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_F])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x -= 1.0f;
			printf("movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclof = 0;
			printf("Presiona la tecla 3 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_T])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_y += 1.0f;
			printf("movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclot = 0;
			printf("Presiona la tecla 4 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_G])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_y -= 1.0f;
			printf("movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclog = 0;
			printf("Presiona la tecla 5 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_R])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			giroAvion += 180.0f;
			printf("giroAvion es: %f\n", movAvion_x);
			ciclo++;
			ciclor = 0;
			printf("Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_Z])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			rotAvion_x += 40.0f;
			printf("rotAvion_x es: %f\n", rotAvion_x);
			ciclo++;
			ciclor = 0;
			printf("Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_X])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			rotAvion_x -= 40.0f;
			printf("rotAvion_x es: %f\n", rotAvion_x);
			ciclo++;
			ciclor = 0;
			printf("Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_C])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			rotAvion_y += 40.0f;
			printf("rotAvion_y es: %f\n", rotAvion_y);
			ciclo++;
			ciclor = 0;
			printf("Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_V])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			rotAvion_y -= 40.0f;
			printf("rotAvion_y es: %f\n", rotAvion_y);
			ciclo++;
			ciclor = 0;
			printf("Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_B])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			rotAvion_z += 40.0f;
			printf("rotAvion_z es: %f\n", rotAvion_z);
			ciclo++;
			ciclor = 0;
			printf("Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_N])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			rotAvion_z -= 40.0f;
			printf("rotAvion_z es: %f\n", rotAvion_z);
			ciclo++;
			ciclor = 0;
			printf("Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (cicloh < 1)
		{
			ciclo = 0;
			printf("Ya puedes modificar tu variable\n");
		}
	}
	if (keys[GLFW_KEY_M])
	{
		guardarKeyframesFile();
	}

}

/*
		// =============================================================
		// INSTANCIA HW - CUERPO (EL PADRE)
		// =============================================================

		model = glm::mat4(1.0);
		// Volvemos a una posicin fija para pruebas (puedes usar la que tenas)
		model = glm::translate(model, glm::vec3(120.0f, 0.5f, -30.0f));

		// ESTA ES LA ROTACIN QUE VAMOS A PROBAR:
		// Prueba con 0, 90, 180 o 270 para ver cul es el "frente" de tu modelo
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 modelCuerpo = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_cuerpo.RenderModel();
		*/


		/*
				// Preparacin del Buffer y dibujo del fondo
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

				// Activacin del Shader Principal y recuperacin de sus Uniforms
				shaderList[0].UseShader();
				uniformModel = shaderList[0].GetModelLocation();
				uniformProjection = shaderList[0].GetProjectionLocation();
				uniformView = shaderList[0].GetViewLocation();
				uniformEyePosition = shaderList[0].GetEyePositionLocation();
				uniformColor = shaderList[0].getColorLocation();
				uniformTextureOffset = shaderList[0].getOffsetLocation();
				uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
				uniformShininess = shaderList[0].GetShininessLocation();

				glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
				glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
				*/
				/*
						// Capturar teclas y ratn para la cmara
						glfwPollEvents();
						camera.keyControl(mainWindow.getsKeys(), deltaTime);
						camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
						*/
