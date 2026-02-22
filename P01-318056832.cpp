#include <stdio.h>
#include <string.h>//para manejar cadenas de caracteres
#include <glew.h>
#include <glfw3.h>
//Librerías añadidas para cambiar el color de fondo aleatoriamente
#include <cstdlib>
#include <ctime>
//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;//creo nuevas variables de tipo entero sin signo de OpenGL que me sirven como índice de cada uno de estos tres objetos.
//VAO Arreglo de vértices de nuestro programa??
//VBO Espacio de memoria de video que están ocupando nuestros datos
//Shader Índice del programa de shader que voy a utilizar

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader. SIempre tiene entrada.
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
}";//Convierte espacios que vemos en una posición a vértices

//Fragment Shader procesa toda la información de color. SIempre tiene salida.
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(1.0f,1.0f,0.0f,1.0f);	 			\n\
}";



void CrearTriangulo() //Le voy a pasar datos con los que yo quiero que después dibuje un triángulo
{
	GLfloat vertices[] = {
		/*Letra R*/
		// t12 = Polygon(T, R, W1)
		-0.9f, 0.5f, 0.0f,     // T
		-0.9f, 0.9f, 0.0f,     // R
		-0.848f, 0.848f, 0.0f, // W1

		// t13 = Polygon(W1, R, J1)
		-0.848f, 0.848f, 0.0f, // W1
		-0.9f, 0.9f, 0.0f,     // R
		-0.74f, 0.9f, 0.0f,    // J1

		// t14 = Polygon(W1, I1, J1)
		-0.848f, 0.848f, 0.0f, // W1
		-0.82f, 0.848f, 0.0f,  // I1
		-0.74f, 0.9f, 0.0f,    // J1

		// t15 = Polygon(I1, J1, T1)
		-0.82f, 0.848f, 0.0f,  // I1
		-0.74f, 0.9f, 0.0f,    // J1
		-0.752f, 0.848f, 0.0f, // T1

		// t16 = Polygon(T1, J1, K1)
		-0.752f, 0.848f, 0.0f, // T1
		-0.74f, 0.9f, 0.0f,    // J1
		-0.72f, 0.9f, 0.0f,    // K1

		// t17 = Polygon(T1, L1, K1)
		-0.752f, 0.848f, 0.0f, // T1
		-0.71f, 0.899f, 0.0f,  // L1
		-0.72f, 0.9f, 0.0f,    // K1

		// t18 = Polygon(T1, L1, M1)
		-0.752f, 0.848f, 0.0f, // T1
		-0.71f, 0.899f, 0.0f,  // L1
		-0.704f, 0.896f, 0.0f, // M1

		// t19 = Polygon(T1, M1, N1)
		-0.752f, 0.848f, 0.0f, // T1
		-0.704f, 0.896f, 0.0f, // M1
		-0.701f, 0.89f, 0.0f,  // N1

		// t20 = Polygon(T1, N1, S)
		-0.752f, 0.848f, 0.0f, // T1
		-0.701f, 0.89f, 0.0f,  // N1
		-0.7f, 0.88f, 0.0f,    // S

		// t21 = Polygon(T1, S, O1)
		-0.752f, 0.848f, 0.0f, // T1
		-0.7f, 0.88f, 0.0f,    // S
		-0.7f, 0.72f, 0.0f,    // O1

		// t22 = Polygon(U1, T1, O1)
		-0.752f, 0.772f, 0.0f, // U1
		-0.752f, 0.848f, 0.0f, // T1
		-0.7f, 0.72f, 0.0f,    // O1

		// t23 = Polygon(U1, Q1, O1)
		-0.752f, 0.772f, 0.0f, // U1
		-0.701f, 0.71f, 0.0f,  // Q1
		-0.7f, 0.72f, 0.0f,    // O1

		// t24 = Polygon(U1, S1, Q1)
		-0.752f, 0.772f, 0.0f, // U1
		-0.704f, 0.704f, 0.0f, // S1
		-0.701f, 0.71f, 0.0f,  // Q1

		// t25 = Polygon(U1, R1, S1)
		-0.752f, 0.772f, 0.0f, // U1
		-0.71f, 0.701f, 0.0f,  // R1
		-0.704f, 0.704f, 0.0f, // S1

		// t26 = Polygon(U1, P1, R1)
		-0.752f, 0.772f, 0.0f, // U1
		-0.72f, 0.7f, 0.0f,    // P1
		-0.71f, 0.701f, 0.0f,  // R1

		// t27 = Polygon(U1, G1, P1)
		-0.752f, 0.772f, 0.0f, // U1
		-0.748f, 0.7f, 0.0f,   // G1
		-0.72f, 0.7f, 0.0f,    // P1

		// t28 = Polygon(H1, G1, U1)
		-0.82f, 0.772f, 0.0f,  // H1
		-0.748f, 0.7f, 0.0f,   // G1
		-0.752f, 0.772f, 0.0f, // U1

		// t29 = Polygon(H1, E1, G1)
		-0.82f, 0.772f, 0.0f,  // H1
		-0.8f, 0.7f, 0.0f,     // E1
		-0.748f, 0.7f, 0.0f,   // G1

		// t30 = Polygon(H1, W, E1)
		-0.82f, 0.772f, 0.0f,  // H1
		-0.82f, 0.7f, 0.0f,    // W
		-0.8f, 0.7f, 0.0f,     // E1

		// t31 = Polygon(W1, H1, T)
		-0.848f, 0.848f, 0.0f, // W1
		-0.82f, 0.772f, 0.0f,  // H1
		-0.9f, 0.5f, 0.0f,     // T

		// t32 = Polygon(H1, V, T)
		-0.82f, 0.772f, 0.0f,  // H1
		-0.82f, 0.5f, 0.0f,    // V
		-0.9f, 0.5f, 0.0f,     // T

		// t33 = Polygon(E1, F1, U)
		-0.8f, 0.7f, 0.0f,     // E1
		-0.752f, 0.5f, 0.0f,   // F1
		-0.7f, 0.5f, 0.0f,     // U

		// t34 = Polygon(E1, G1, U)
		-0.8f, 0.7f, 0.0f,     // E1
		-0.748f, 0.7f, 0.0f,   // G1
		-0.7f, 0.5f, 0.0f,     // U

		// t35 = Polygon(W1, V1, H1)
		-0.848f, 0.848f, 0.0f, // W1
		-0.82f, 0.84f, 0.0f,   // V1
		-0.82f, 0.772f, 0.0f,   // H1

		/*Letra J*/
		// t36 = Polygon(Z1, F2, Z)
		-0.02f, 0.2f, 0.0f,    // Z1
		0.06f, -0.2f, 0.0f,    // F2
		0.06f, 0.2f, 0.0f,    // Z

		// t37 = Polygon(H2, Z1, F2)
		-0.02f, -0.16f, 0.0f,    // H2
		-0.02f, 0.2f, 0.0f,    // Z1
		0.06f, -0.2f, 0.0f,    // F2

		// t38 = Polygon(E2, H2, F2)
		-0.06f, -0.2f, 0.0f,    // E2
		-0.02f, -0.16f, 0.0f,    // H2
		0.06f, -0.2f, 0.0f,    // F2

		// t39 = Polygon(G2, E2, H2)
		-0.06f, -0.16f, 0.0f,    // G2
		-0.06f, -0.2f, 0.0f,    // E2
		-0.02f, -0.16f, 0.0f,     // H2

		/*Letra E*/

		// t1 = Polygon(G, E, J)
		0.7f, -0.9f, 0.0f,      // G
		0.7f, -0.5f, 0.0f,      // E
		0.752f, -0.552f, 0.0f,  // J

		// t2 = Polygon(E, F, J)
		0.7f, -0.5f, 0.0f,      // E
		0.9f, -0.5f, 0.0f,      // F
		0.752f, -0.552f, 0.0f,  // J

		// t3 = Polygon(J, F, I)
		0.752f, -0.552f, 0.0f,  // J
		0.9f, -0.5f, 0.0f,      // F
		0.9f, -0.552f, 0.0f,    // I

		// t4 = Polygon(J, L, O)
		0.752f, -0.552f, 0.0f,  // J
		0.78f, -0.66f, 0.0f,    // L
		0.78f, -0.712f, 0.0f,   // O

		// t5 = Polygon(J, O, G)
		0.752f, -0.552f, 0.0f,  // J
		0.78f, -0.712f, 0.0f,   // O
		0.7f, -0.9f, 0.0f,      // G

		// t6 = Polygon(L, N, O)
		0.78f, -0.66f, 0.0f,    // L
		0.82f, -0.712f, 0.0f,   // N
		0.78f, -0.712f, 0.0f,   // O

		// t7 = Polygon(L, M, N)
		0.78f, -0.66f, 0.0f,    // L
		0.82f, -0.66f, 0.0f,    // M
		0.82f, -0.712f, 0.0f,   // N

		// t8 = Polygon(O, P, G)
		0.78f, -0.712f, 0.0f,   // O
		0.78f, -0.848f, 0.0f,   // P
		0.7f, -0.9f, 0.0f,      // G

		// t9 = Polygon(G, P, Q)
		0.7f, -0.9f, 0.0f,      // G
		0.78f, -0.848f, 0.0f,   // P
		0.9f, -0.848f, 0.0f,    // Q

		// t10 = Polygon(G, H, Q)
		0.7f, -0.9f, 0.0f,      // G
		0.9f, -0.9f, 0.0f,      // H
		0.9f, -0.848f, 0.0f,    // Q

		// t11 = Polygon(J, K, L)
		0.752f, -0.552f, 0.0f,  // J
		0.78f, -0.56f, 0.0f,    // K
		0.78f, -0.66f, 0.0f     // L

	};//Arreglo de vértices que me van a ayudar para dibujar.
	glGenVertexArrays(1, &VAO); //generar 1 VAO
	glBindVertexArray(VAO);//asignar VAO

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es estático pues no se modificarán los valores

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
		glEnableVertexAttribArray(0);//Una vez que agrupamos de 3 entres los flotantes, se los mandamos a vertexen su entrada vec3
		//agregar valores a vèrtices y luego declarar un nuevo vertexAttribPointer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

}
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica

//the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader,1, theCode, codeLength);//Se le asigna al shader el código
	glCompileShader(theShader);//Se compila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevención de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n",shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el código a la tarjeta gráfica
}

void CompileShaders() {
	shader= glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");//Di te sale esto, tu hardware no soporta el shader con el que quieres trabajar
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//Ya compiló el shader, ahora se linkean los shaders a la tarjeta gráfica
	 //verificaciones y prevención de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);//SI puedo validar, dibujo, si no te lo digo
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}



}
int main()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//****  LAS SIGUIENTES 4 LÍNEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSIÓN DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

 //Llamada a las funciones creadas antes del main
	CrearTriangulo();
	CompileShaders();
	
	//Se inicializan las variables utilizadas para cambiar el color aleatoriamente
	float r, g, b;
	float ultimoCambio = 0.0f;//En esta variable se guarda el tiempo en que se actualizó por última vez el color de fondo. Se inicializa en 0.0f
	srand(time(NULL));//Inicialización de la semilla para rand

	//Se asigna un primer color aleatorio a cada variable r,g,b
	//Se divide el valor aleatorio obtenido entre RAND_MAX para obtener un número aleatorio entre 0 y 1
	r = static_cast<float>(rand()) / RAND_MAX;
	g = static_cast<float>(rand()) / RAND_MAX;
	b = static_cast<float>(rand()) / RAND_MAX;

	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		float tiempo = glfwGetTime();//Devuelve el tiempo en segundos desde que se inicializó GLFW
		
		//Si la diferencia entre el tiempo en que se realizó el último cambio de color y
		//el tiempo actual es mayor o igual a 2.0f, entonces se randomiza el color
		if (tiempo - ultimoCambio >= 2.0f) {
			ultimoCambio = tiempo;
			r = static_cast<float>(rand()) / RAND_MAX;
			g = static_cast<float>(rand()) / RAND_MAX;
			b = static_cast<float>(rand()) / RAND_MAX;
		}
		
		//Actualizamos el color de fondo
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		//Recibir eventos del usuario
		glfwPollEvents();

		//Limpiar la ventana
		//glClearColor(0.0f,0.0f,0.0f,1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES,0,117);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
		 
		//NO ESCRIBIR NINGUNA LÍNEA DESPUÉS DE glfwSwapBuffers(mainWindow); 
	}


	return 0;
}