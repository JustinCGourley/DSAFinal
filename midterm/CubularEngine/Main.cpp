#include "stdafx.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "GameEntity.h"
#include "Material.h"
#include "Input.h"
#include "irrKlang.h"
#include "BezierCurve.h"

//TODO - maybe make some #define macro for a print if debug?
//TODO - make an Engine class with a specific Init() and Run() function such that
//       our Main.cpp is kept clean and tidy

void CreateManyCubes(Mesh*, Material*);
void CreateBezierExample(Mesh*, Material*);
std::vector<GameEntity*> gameEntities;

int main()
{

	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

	//engine->play2D("../libraries/irrKlang-1.5.0/media/getout.ogg", true);

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

		//create a bunch of cubes
		Mesh* myMesh = new Mesh();
		myMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* myMaterial = new Material(shaderProgram);
		CreateManyCubes(myMesh, myMaterial);

		//create bezier cubes
		Mesh* bMesh = new Mesh();
		bMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* bMat = new Material(shaderProgram);
		CreateBezierExample(bMesh, bMat);

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
			std::string("ground")
		);

		gameEntities.push_back(floor);


		Input::GetInstance()->Init(window);

		//setup and create camera
        //TODO - maybe a CameraManager?
        Camera* myCamera = new Camera(
            glm::vec3(0.0f, 0.0f, -20.f),    //position of camera
            glm::vec3(0.0f, 0.0f, 1.f),     //the 'forward' of the camera
            glm::vec3(0.0f, 1.f, 0.0f),     //what 'up' is for the camera
            60.0f,                          //the field of view in radians
            (float)width,                   //the width of the window in float
            (float)height,                  //the height of the window in float
            0.01f,                          //the near Z-plane
            500.f,							//far z-plane
			window
        );


        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);


        //main loop
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
				gameEntities[i]->Update(gameEntities, i);
			}

			gameEntities[0]->ApplyForce(glm::vec3(0.001f, 0.0f, 0.0f));

			myCamera->Update();


            /* PRE-RENDER */
            {
                //start off with clearing the 'color buffer'
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glClearColor(0.f, 0.f, 0.f, 1.0f);
            }

            /* RENDER */
			for (int i = 0; i < gameEntities.size(); i++)
			{
				gameEntities[i]->Render(myCamera);
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
        delete myCamera;
        Input::Release();
    }

    //clean up
    glfwTerminate();
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif // _DEBUG
    return 0;
}

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
			//randomY = (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (9.7f - (-9.7f))))));
			randomY = 0;
			//randomZ = 5.f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (20.f - (5.f))))));
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
			std::string("cube")
		);
		gameEntities.push_back(myGameEntity);
	}
}

void CreateBezierExample(Mesh* bMesh, Material* bMat)
{
	int pointCount = 100;
	BezierCurve* bezierCurve = new BezierCurve(glm::vec2(20, 10), glm::vec2(10, 10), glm::vec2(5, 20), glm::vec2(25, 20));

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
			glm::vec3(0.01f, 0.01f, 0.01f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			false,
			glm::vec3(1.f, 1.f, 1.f),
			0,
			std::string("curve")
		);
		gameEntities.push_back(myGameEntity);
	}
}