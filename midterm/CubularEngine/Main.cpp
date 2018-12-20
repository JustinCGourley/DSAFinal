#include "stdafx.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "GameEntity.h"
#include "Material.h"
#include "Input.h"
#include "BezierCurve.h"
#include "Interpolate.h"


//methods
void CreateManyCubes(Mesh*, Material*);
void CreateBezierExample(Mesh*, Material*, BezierCurve*);
void UpdateBezierExample(BezierCurve *, GameEntity *);
void UpdateScaleExample(GameEntity *);
void UpdateSheerExample(GameEntity *);
void UpdateLERPExample(GameEntity *);
void UpdateSLERPExample(GameEntity *);
void SetupLERPExample(Mesh *, Material *);
void SetupSLERPExample(Mesh *, Material *);
Camera* CreateCamera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up, int width, int height, GLFWwindow *window, bool controllable);
void CreatePhysicsExample1(Mesh *mesh, Material *mat);
void CheckUpdateCameras();
void UpdateGravityExample(GameEntity* gameObj);
void QuadTree(std::vector<GameEntity*> quadify, GameEntity* floor, glm::vec3 center, irrklang::ISoundEngine* sound);

std::vector<GameEntity*> gameEntities;
std::vector<GameEntity*> staticEntities;
std::vector<GameEntity*> octreeEntities;


//bezier cube example vars
float bezierCubeTime = 0;
float bezierCubeStep = 1.f / 500.f;
bool bezierDirForward = true;

//scaling example
int scalingDir = 0;
float scaleAmount = 1.f;

//shear example
int shearDir = 0;
float shearAmount = 0.f;

//interpolation declaration
Interpolate interpolate;

//LERP example
glm::vec3 lerpStart = glm::vec3(50.f, 10.f, 5.f);
glm::vec3 lerpEnd = glm::vec3(60.f, 5.f, 10.f);
float lerpTime = 0;
float lerpStep = 1.f / 100.f;
bool lerpDirForward = true;

//SLERP example
glm::vec3 slerpStart = glm::vec3(70.f, 10.f, 5.f);
glm::vec3 slerpEnd = glm::vec3(80.f, 5.f, 10.f);
float slerpTime = 0;
float slerpStep = 1.f / 100.f;
bool slerpDirForward = true;

std::vector<Camera*> cameras;
int curCamera = 0;
bool cameraSwap = false;

int main()
{

	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

	engine->setSoundVolume(0.1f);

	//engine->play2D("../libraries/irrKlang-1.5.0/media/explosion.wav", false);

    {
        //init GLFW
        {
            if (glfwInit() == GLFW_FALSE)
            {
#ifdef _DEBUG
                std::cout << "GLFW failed to initialize" << std::endl;
                _CrtDumpMemoryLeaks();
                std::cin.get();
#endif
                return 1;
            }
        }
#ifdef _DEBUG
        std::cout << "GLFW successfully initialized!" << std::endl;
#endif // _DEBUG

        //create & init window, set viewport
        int width = 1200;
        int height = 800;
        GLFWwindow* window = glfwCreateWindow(width, height, "Babby's First Cube?", nullptr, nullptr);
        {
            if (window == nullptr)
            {
#ifdef _DEBUG
                std::cout << "GLFW failed to create window" << std::endl;
                _CrtDumpMemoryLeaks();
                std::cin.get();
#endif
                glfwTerminate();
                return 1;
            }

            //tells OpenGL to use this window for this thread
            //(this would be more important for multi-threaded apps)
            glfwMakeContextCurrent(window);

            //gets the width & height of the window and specify it to the viewport
            int windowWidth, windowHeight;
            glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            glViewport(0, 0, windowWidth, windowHeight);
        }
#ifdef _DEBUG
        std::cout << "Window successfully initialized!" << std::endl;
#endif // _DEBUG

        //init GLEW
        {
            if (glewInit() != GLEW_OK)
            {
#ifdef _DEBUG
                std::cout << "GLEW failed to initialize" << std::endl;
                _CrtDumpMemoryLeaks();
                std::cin.get();
#endif
                glfwTerminate();
                return 1;
            }
        }
#ifdef _DEBUG
        std::cout << "GLEW successfully initialized!" << std::endl;
#endif // _DEBUG

        //init the shader program
        //TODO - this seems like a better job for a shader manager
        //       perhaps the Shader class can be refactored to fit a shader program
        //       rather than be a thing for vs and fs
        GLuint shaderProgram = glCreateProgram();
        {

            //create vS and attach to shader program
            Shader *vs = new Shader();
            vs->InitFromFile("../assets/shaders/vertexShader.glsl", GL_VERTEX_SHADER);
            glAttachShader(shaderProgram, vs->GetShaderLoc());

            //create FS and attach to shader program
            Shader *fs = new Shader();
            fs->InitFromFile("../assets/shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
            glAttachShader(shaderProgram, fs->GetShaderLoc());

            //link everything that's attached together
            glLinkProgram(shaderProgram);

            GLint isLinked;
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
            if (!isLinked)
            {
                char infolog[1024];
                glGetProgramInfoLog(shaderProgram, 1024, NULL, infolog);
#ifdef _DEBUG
                std::cout << "Shader Program linking failed with error: " << infolog << std::endl;
                std::cin.get();
#endif

                // Delete the shader, and set the index to zero so that this object knows it doesn't have a shader.
                glDeleteProgram(shaderProgram);
                glfwTerminate();
                _CrtDumpMemoryLeaks();
                return 1;
            }

            //everything's in the program, we don't need this
            delete fs;
            delete vs;
        }

#ifdef _DEBUG
        std::cout << "Shaders compiled attached, and linked!" << std::endl;
#endif // _DEBUG

		 
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //init the mesh (a cube)
        //TODO - replace this with model loading
        GLfloat vertices[] = {
            -1.0f,-1.0f,-1.0f, // triangle 1 : begin
            -1.0f,-1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, // triangle 1 : end
            1.0f, 1.0f,-1.0f, // triangle 2 : begin
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f, // triangle 2 : end
            1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f,-1.0f, 1.0f
        };

		//=================== create a bunch of cubes=============================
		Mesh* myMesh = new Mesh();
		myMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* myMaterial = new Material(shaderProgram);
		CreateManyCubes(myMesh, myMaterial);

		//==================== create bezier cubes==================================
		Mesh* bMesh = new Mesh();
		bMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* bMat = new Material(shaderProgram);


		glm::vec2 curveStart = glm::vec2(20.f, 10.f);
		BezierCurve* bezierCurve = new BezierCurve(curveStart, glm::vec2(10, 10), glm::vec2(5, 20), glm::vec2(25, 20));
		CreateBezierExample(bMesh, bMat, bezierCurve);

		GameEntity* bezierCube = new GameEntity(
			bMesh,
			bMat,
			glm::vec3(curveStart.x, curveStart.y, 5),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.13f, 0.73f, 0.27f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);

		staticEntities.push_back(bezierCube);

		//============ create scaling example=============================
		GameEntity* scaleExample = new GameEntity(
			bMesh,
			bMat,
			glm::vec3(40, 5, 5),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(1.f, 1.f, 1.f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);

		staticEntities.push_back(scaleExample);

		//============ create shearing example=============================
		GameEntity* shearingExample = new GameEntity(
			bMesh,
			bMat,
			glm::vec3(90, 5, 5),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(1.f, 1.f, 1.f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);

		staticEntities.push_back(shearingExample);

		//================== create lerp example ========================

		SetupLERPExample(bMesh, bMat);

		//moving var
		GameEntity* lerpExample = new GameEntity(
			bMesh,
			bMat,
			lerpStart,
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);

		staticEntities.push_back(lerpExample);

		//================== create slerp example ========================

		SetupSLERPExample(bMesh, bMat);

		//moving var
		GameEntity* slerpExample = new GameEntity(
			bMesh,
			bMat,
			slerpStart,
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);

		staticEntities.push_back(slerpExample);

		//create floor 
		Mesh* floorMesh = new Mesh();
		floorMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* floorMat = new Material(shaderProgram);

		GameEntity* floor = new GameEntity(
			floorMesh,
			floorMat,
			glm::vec3(0.f, -10.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(100.f, 1.f, 100.f),
			glm::vec3(0.2f, 0.2f, 0.2f),
			false,
			glm::vec3(100.f, 1.f, 100.f),
			1,
			"Floor",
			glm::vec3(0.f, 0.f, 0.f)
		);

		gameEntities.push_back(floor);
		staticEntities.push_back(floor);
		octreeEntities.push_back(floor);

		//======================================= create no gravity linear momentum example =======================

		CreatePhysicsExample1(floorMesh, floorMat);

		Input::GetInstance()->Init(window);

		//============================================= create gravity example =================================
		GameEntity* gravityExample = new GameEntity(
			bMesh,
			bMat,
			glm::vec3(40.f, 50.f, -70.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(2.f, 2.f, 2.f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			true,
			glm::vec3(2.f, 2.f, 2.f),
			1,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);
		gameEntities.push_back(gravityExample);

		//=====================================setup cameras==========================================
		Camera* freeCam = CreateCamera(
			glm::vec3(0.0f, 0.0f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			true);

		cameras.push_back(freeCam);

		Camera* camBezier = CreateCamera(
			glm::vec3(15.f, 15.f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		Camera* camScale = CreateCamera(
			glm::vec3(40.f, 5.f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		Camera* camLERP = CreateCamera(
			glm::vec3(55.f, 7.5f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		Camera* camSLERP = CreateCamera(
			glm::vec3(70.f, 10.f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		Camera* camShear = CreateCamera(
			glm::vec3(90.f, 5.f, -20.f),
			glm::vec3(0.0f, 0.0f, 1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		Camera* camLinearM = CreateCamera(
			glm::vec3(0.f, 40.f, -70.f),
			glm::vec3(0.0f, -1.0f, 0.f),
			glm::vec3(0.0f, 0.f, 1.0f),
			width,
			height,
			window,
			false);

		Camera* camGravity = CreateCamera(
			glm::vec3(40.f, 20.f, 0.f),
			glm::vec3(0.0f, 0.f, -1.f),
			glm::vec3(0.0f, 1.f, 0.0f),
			width,
			height,
			window,
			false);

		cameras.push_back(camBezier);
		cameras.push_back(camScale);
		cameras.push_back(camLERP);
		cameras.push_back(camSLERP);
		cameras.push_back(camShear);
		cameras.push_back(camLinearM);
		cameras.push_back(camGravity);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);


        //--------------------================================start main loop========================----------------------------
        while (!glfwWindowShouldClose(window))
        {
            /* INPUT */
            {
                //checks events to see if there are pending input
                glfwPollEvents();

				
                //breaks out of the loop if user presses ESC
                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                {
                    break;
                }
            }

            /* GAMEPLAY UPDATE */

			for (int i = 0; i < gameEntities.size(); i++)
			{
				gameEntities[i]->Update(gameEntities, i, engine);
			}

			for (int i = 0; i < staticEntities.size(); i++)
			{
				staticEntities[i]->Update(staticEntities, i, engine);
			}

			/*for (int i = 0; i < octreeEntities.size(); i++)
			{
				octreeEntities[i]->Update(octreeEntities, i, engine);
			}*/

			QuadTree(octreeEntities, floor, glm::vec3(0.f, -7.f, -70.f), engine);

			cameras[curCamera]->Update();

			//update bezier example
			UpdateBezierExample(bezierCurve, bezierCube);

			//update scaling example
			UpdateScaleExample(scaleExample);

			//update shearing example
			UpdateSheerExample(shearingExample);

			//update lerp example
			UpdateLERPExample(lerpExample);

			//update slerp example
			UpdateSLERPExample(slerpExample);

			//update gravity example
			UpdateGravityExample(gravityExample);

			//update cameras
			CheckUpdateCameras();

            /* PRE-RENDER */
            {
                //start off with clearing the 'color buffer'
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glClearColor(0.f, 0.f, 0.f, 1.0f);
            }

            /* RENDER */
			for (int i = 0; i < gameEntities.size(); i++)
			{
				gameEntities[i]->Render(cameras[curCamera]);
			}
			for (int i = 0; i < staticEntities.size(); i++)
			{
				staticEntities[i]->Render(cameras[curCamera]);
			}
			for (int i = 0; i < octreeEntities.size(); i++)
			{
				octreeEntities[i]->Render(cameras[curCamera]);
			}


            /* POST-RENDER */
            {
                //'clear' for next draw call
                glBindVertexArray(0);
                glUseProgram(0);
                //swaps the front buffer with the back buffer
                glfwSwapBuffers(window);
            }
        }

		//Delete Sound engine
		engine->drop();

        //de-allocate our mesh!
        delete myMesh;
        delete myMaterial;

		delete floorMesh;
		delete floorMat;

		delete bMesh;
		delete bMat;

		for (int i = 0; i < gameEntities.size(); i++)
		{
			delete gameEntities[i];
		}
		//cubeGraph.clear();
		for (int i = 0; i < cameras.size(); i++)
		{
			delete cameras[i];
		}
        Input::Release();
    }

    //clean up
    glfwTerminate();
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif // _DEBUG
    return 0;
}

// ========================================================== TODO: remove this later
void CreateManyCubes(Mesh* myMesh, Material* myMaterial)
{
	srand(time(NULL));

	int cubeCount = 5;

	int maxLoop = 50;
	for (int i = 0; i < cubeCount; i++)
	{
		bool next = true;
		float randomX, randomY, randomZ;
		maxLoop = 50;
		do
		{
			next = true;
			//get a random x and y
			randomX = -13.8f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (13.8f - (-13.8f))))));
			//randomX = 0;
			//randomY = (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (9.7f - (-9.7f))))));
			randomY = 0;
			//randomZ = -13.8f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (13.8f - (-13.8f))))));
			randomZ = 0;

			//see if this position will intersect with another cube | if so, remake the position
			for (int j = 0; j < i; j++)
			{
				if (abs(gameEntities[j]->position.x - randomX) < 2.f && abs(gameEntities[j]->position.y - randomY) < 2.f)
				{
					next = false;
					maxLoop--;
					if (maxLoop <= 0)
					{
						next = true;
					}
				}
			}
		} while (!next);

		//create the entity, also giving it a random color
		GameEntity* myGameEntity = new GameEntity(
			myMesh,
			myMaterial,
			glm::vec3(randomX, randomY, randomZ),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(1.f, 1.f, 1.f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			true,
			glm::vec3(1.f, 1.f, 1.f),
			1,
			"Cube",
			glm::vec3(0.f, 0.f, 0.f)
		);
		gameEntities.push_back(myGameEntity);
	}
}

// ========================================================== create bezier curve example
void CreateBezierExample(Mesh* bMesh, Material* bMat, BezierCurve* bezierCurve)
{
	int pointCount = 100;

	float interval = (float)(1.f / pointCount);
	for (int i = 0; i < pointCount; i++)
	{
		float t = interval * i;
		glm::vec2 pos = bezierCurve->GetPoint(t);
		GameEntity* myGameEntity = new GameEntity(
			bMesh,
			bMat,
			glm::vec3(pos.x, pos.y, 5),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.02f, 0.02f, 0.02f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);
		staticEntities.push_back(myGameEntity);
	}

	glm::vec2 pos = bezierCurve->GetPoint(0);
	GameEntity* start = new GameEntity(
		bMesh,
		bMat,
		glm::vec3(pos.x, pos.y, 5),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(1.0f, 0.f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object",
		glm::vec3(0.f, 0.f, 0.f)
	);

	pos = bezierCurve->GetPoint(1);
	GameEntity* end = new GameEntity(
		bMesh,
		bMat,
		glm::vec3(pos.x, pos.y, 5),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.0f, 1.f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object",
		glm::vec3(0.f, 0.f, 0.f)
	);

	staticEntities.push_back(start);
	staticEntities.push_back(end);
}

// ========================================================== update bezier curve example
void UpdateBezierExample(BezierCurve *bezierCurve, GameEntity *gameObj)
{
	bezierCubeTime += (bezierDirForward) ? bezierCubeStep : -bezierCubeStep;

	if (bezierCubeTime >= 1.f) { bezierCubeTime = 1.f; bezierDirForward = false; }
	else if (bezierCubeTime <= 0) { bezierCubeTime = 0.f; bezierDirForward = true; }


	glm::vec2 newPos = bezierCurve->GetPoint(bezierCubeTime);

	gameObj->position.x = newPos.x;
	gameObj->position.y = newPos.y;
	gameObj->position.z = 5;
}

// ========================================================== update Scale example
void UpdateScaleExample(GameEntity *gameObj)
{
	glm::vec3 scaleSet = glm::vec3(1.f, 1.f, 1.f);
	scaleAmount += (scalingDir % 2 == 0) ? 0.01f : -0.01f;

	if (scaleAmount >= 2.f || scaleAmount <= 1.f)
	{
		scaleAmount = (scaleAmount >= 2.f) ? 2.f : 1.f;
		scalingDir++;
		if (scalingDir == 6) { scalingDir = 0; }
	}

	if (scalingDir == 0 || scalingDir == 1)
	{
		scaleSet.x = scaleAmount;
	}
	else if (scalingDir == 2 || scalingDir == 3)
	{
		scaleSet.y = scaleAmount;
	}
	else
	{
		scaleSet.z = scaleAmount;
	}

	gameObj->scale = scaleSet;
	gameObj->eulerAngles.y += 0.009f;
	gameObj->eulerAngles.x += 0.006f;
	gameObj->eulerAngles.z += 0.003f;
}

// ========================================================== update Sheer example
void UpdateSheerExample(GameEntity *gameObj)
{
	glm::vec3 shearSet = glm::vec3(0.f, 0.f, 0.f);
	shearAmount += (shearDir % 2 == 0) ? 0.01f : -0.01f;
	if (shearAmount >= 1.f || shearAmount <= 0.f)
	{
		shearAmount = (shearAmount >= 1.f) ? 1.f : 0.f;
		shearDir++;
		if (shearDir == 6) { shearDir = 0; }
	}

	if (shearDir == 0 || shearDir == 1)
	{
		shearSet.x = shearAmount;
	}
	else if (shearDir == 2 || shearDir == 3)
	{
		shearSet.y = shearAmount;
	}
	else
	{
		shearSet.z = shearAmount;
	}

	gameObj->shear = shearSet;
}

// ========================================================== create LERP example
void SetupLERPExample(Mesh *bMesh, Material *bMat)
{
	int pointCount = 100;
	float step = 1.f / pointCount;

	for (int i = 0; i < pointCount; i++)
	{
		float t = step * i;

		GameEntity* obj = new GameEntity(
			bMesh,
			bMat,
			interpolate.LERP(lerpStart, lerpEnd, t),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(.02f, .02f, .02f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);

		staticEntities.push_back(obj);
	}

	//start / end pos
	GameEntity* lerpStartObj = new GameEntity(
		bMesh,
		bMat,
		lerpStart,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.1f, .1f, .1f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object",
		glm::vec3(0.f, 0.f, 0.f)
	);
	GameEntity* lerpEndObj = new GameEntity(
		bMesh,
		bMat,
		lerpEnd,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.1f, .1f, .1f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object",
		glm::vec3(0.f, 0.f, 0.f)
	);

	staticEntities.push_back(lerpStartObj);
	staticEntities.push_back(lerpEndObj);
}

// ========================================================== create SLERP example
void SetupSLERPExample(Mesh *bMesh, Material *bMat)
{
	int pointCount = 100;
	float step = 1.f / pointCount;

	for (int i = 0; i < pointCount; i++)
	{
		float t = step * i;
		glm::vec3 pos = interpolate.SLERP(slerpStart, slerpEnd, t);
		GameEntity* obj = new GameEntity(
			bMesh,
			bMat,
			interpolate.SLERP(slerpStart, slerpEnd, t),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(.02f, .02f, .02f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(0.f, 0.f, 0.f),
			0,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);

		staticEntities.push_back(obj);
	}

	//start / end pos
	GameEntity* slerpStartObj = new GameEntity(
		bMesh,
		bMat,
		slerpStart,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.1f, .1f, .1f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object",
		glm::vec3(0.f, 0.f, 0.f)
	);
	GameEntity* slerpEndObj = new GameEntity(
		bMesh,
		bMat,
		slerpEnd,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(.1f, .1f, .1f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		false,
		glm::vec3(0.f, 0.f, 0.f),
		0,
		"Object",
		glm::vec3(0.f, 0.f, 0.f)
	);

	staticEntities.push_back(slerpStartObj);
	staticEntities.push_back(slerpEndObj);
}

// ========================================================== update LERP example
void UpdateLERPExample(GameEntity *gameObj)
{
	lerpTime += (lerpDirForward) ? lerpStep : -lerpStep;
	if (lerpTime >= 1.f || lerpTime <= 0.f) { lerpDirForward = !lerpDirForward; }

	glm::vec3 pos = interpolate.LERP(lerpStart, lerpEnd, lerpTime);

	gameObj->position = pos;
}

// ========================================================== update SLERP rotation example
//TODO: update this SLERP to be rotations instead of movement (oops)
void UpdateSLERPExample(GameEntity *gameObj)
{
	slerpTime += (slerpDirForward) ? slerpStep : -slerpStep;
	if (slerpTime >= 1.f || slerpTime <= 0.f) { slerpDirForward = !slerpDirForward; }

	//glm::vec3 pos = interpolate.SLERP(slerpStart, slerpEnd, slerpTime);

	//gameObj->position = pos;
}

// ========================================================== creates a camera based on given params
Camera* CreateCamera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up, int width, int height, GLFWwindow *window, bool control)
{
	Camera* camera = new Camera(
		pos,    //position of camera
		forward,     //the 'forward' of the camera
		up,     //what 'up' is for the camera
		60.0f,                          //the field of view in radians
		(float)width,                   //the width of the window in float
		(float)height,                  //the height of the window in float
		0.01f,                          //the near Z-plane
		500.f,							//far z-plane
		window,
		control
	);

	return camera;
}

// ========================================================== create linear momentum example 1 (box)
void CreatePhysicsExample1(Mesh *mesh, Material *mat)
{
	GameEntity* wall1 = new GameEntity(
		mesh,
		mat,
		glm::vec3(0.f, -7.f, -50.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(20.f, 2.f, 2.0f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		false,
		glm::vec3(20.f, 2.f, 2.f),
		1,
		"Wall",
		glm::vec3(0.f, 0.f, 0.f)
	);

	GameEntity* wall2 = new GameEntity(
		mesh,
		mat,
		glm::vec3(0.f, -7.f, -90.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(20.f, 2.f, 2.f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		false,
		glm::vec3(20.f, 2.f, 2.f),
		1,
		"Wall",
		glm::vec3(0.f, 0.f, 0.f)
	);

	GameEntity* wall3 = new GameEntity(
		mesh,
		mat,
		glm::vec3(-20.f, -7.f, -70.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(2.f, 2.f, 20.f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		false,
		glm::vec3(2.f, 2.f, 20.f),
		1,
		"Wall",
		glm::vec3(0.f, 0.f, 0.f)
	);

	GameEntity* wall4 = new GameEntity(
		mesh,
		mat,
		glm::vec3(20.f, -7.f, -70.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(2.f, 2.f, 20.f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		false,
		glm::vec3(2.f, 2.f, 20.f),
		1,
		"Wall",
		glm::vec3(0.f, 0.f, 0.f)
	);

	octreeEntities.push_back(wall1);
	octreeEntities.push_back(wall2);
	octreeEntities.push_back(wall3);
	octreeEntities.push_back(wall4);

	//create a bunch of entities to move around (and apply random forces to each one)
	int cubeCount = 35;
	srand(time(NULL));

	for (int i = 0; i < cubeCount; i++)
	{
		float randomZ = -85.f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (-55.f - (-85.f))))));
		
		glm::vec3 pos = glm::vec3((0.7f * i) + 1.f - 18.f, -6.f, randomZ);
		GameEntity* cube = new GameEntity(
			mesh,
			mat,
			pos,
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(0.7f, 0.7f, 0.7f),
			true,
			glm::vec3(0.5f, 0.5f, 0.5f),
			1,
			"Object",
			glm::vec3(0.f, 0.f, 0.f)
		);

		float dirX = -0.5f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (0.5f - (-0.5f))))));
		float dirZ = -0.5f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (0.5f - (-0.5f))))));

		glm::vec3 randomForce = glm::vec3(dirX, 0.0f, 0.0f);

		cube->ApplyForce(randomForce);

		octreeEntities.push_back(cube);
	}
}

//QuadTree
void QuadTree(std::vector<GameEntity*> quadify, GameEntity* floor, glm::vec3 center, irrklang::ISoundEngine* sound) {

	std::vector<GameEntity*> topRight;
	std::vector<GameEntity*> bottomRight;
	std::vector<GameEntity*> bottomLeft;
	std::vector<GameEntity*> topLeft;

	/*topRight.push_back(floor);
	bottomRight.push_back(floor);
	bottomLeft.push_back(floor);
	topLeft.push_back(floor);*/

	for (int i = 0; i < quadify.size(); i++)
	{
		if (quadify[i]->tag == std::string("Floor")  || quadify[i]->tag == std::string("Wall")) {
			topRight.push_back(quadify[i]);
			bottomRight.push_back(quadify[i]);
			bottomLeft.push_back(quadify[i]);
			topLeft.push_back(quadify[i]);
		}

		if (quadify[i]->position.x >= center.x && quadify[i]->position.z >= center.z && quadify[i]->tag != std::string("Floor") && quadify[i]->tag != std::string("Wall")) {
			topRight.push_back(quadify[i]);
		}
		else if (quadify[i]->position.x >= center.x && quadify[i]->position.z <= center.z && quadify[i]->tag != std::string("Floor") && quadify[i]->tag != std::string("Wall")) {
			bottomRight.push_back(quadify[i]);
		}
		else if (quadify[i]->position.x <= center.x && quadify[i]->position.z <= center.z && quadify[i]->tag != std::string("Floor") && quadify[i]->tag != std::string("Wall")) {
			bottomLeft.push_back(quadify[i]);
		}
		else if (quadify[i]->position.x <= center.x && quadify[i]->position.z >= center.z && quadify[i]->tag != std::string("Floor") && quadify[i]->tag != std::string("Wall")) {
			topLeft.push_back(quadify[i]);
		}
	}

	for (int i = 0; i < topRight.size(); i++)
	{
		topRight[i]->Update(topRight, i, sound);
	}

	for (int i = 0; i < bottomRight.size(); i++)
	{
		bottomRight[i]->Update(bottomRight, i, sound);
	}

	for (int i = 0; i < bottomLeft.size(); i++)
	{
		bottomLeft[i]->Update(bottomLeft, i, sound);
	}

	for (int i = 0; i < topLeft.size(); i++)
	{
		topLeft[i]->Update(topLeft, i, sound);
	}
}

void UpdateGravityExample(GameEntity* gameObj)
{
	if (gameObj->position.y <= -7.f)
	{
		gameObj->position.y = -7.f;
		gameObj->ApplyForce(glm::vec3(0.f, 2.f, 0.f));
	}
}

// ========================================================== Update input based on cameras
void CheckUpdateCameras()
{
	if (Input::GetInstance()->IsKeyDown(GLFW_KEY_RIGHT) && !cameraSwap)
	{
		curCamera++;
		if (curCamera >= cameras.size())
		{
			curCamera = 0;
		}
		cameraSwap = true;
	}
	else if (Input::GetInstance()->IsKeyDown(GLFW_KEY_LEFT) && !cameraSwap)
	{
		curCamera--;
		if (curCamera < 0)
		{
			curCamera = cameras.size() - 1;
		}
		cameraSwap = true;
	}

	if (Input::GetInstance()->IsKeyDown(GLFW_KEY_LEFT) == false && Input::GetInstance()->IsKeyDown(GLFW_KEY_RIGHT) == false && cameraSwap)
	{
		cameraSwap = false;
	}

	//do all the changing of tests in here
	
	if (curCamera == 1)//bezier curve
	{

	}
	else if (curCamera == 2)//scaling cube
	{

	}
	else if (curCamera == 3)//lerp
	{

	}
}