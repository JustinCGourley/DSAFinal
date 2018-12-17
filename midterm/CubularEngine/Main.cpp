#include "stdafx.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "GameEntity.h"
#include "Material.h"
#include "Input.h"

//TODO - maybe make some #define macro for a print if debug?
//TODO - make an Engine class with a specific Init() and Run() function such that
//       our Main.cpp is kept clean and tidy

int main()
{
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
            vs->InitFromFile("assets/shaders/vertexShader.glsl", GL_VERTEX_SHADER);
            glAttachShader(shaderProgram, vs->GetShaderLoc());

            //create FS and attach to shader program
            Shader *fs = new Shader();
            fs->InitFromFile("assets/shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
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

		int cubeCount = 20;
		 
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

        //create our mesh & material
        //TODO - maybe have a MeshManager & a MaterialManager
        Mesh* myMesh = new Mesh();
        myMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
        Material* myMaterial = new Material(shaderProgram);

		srand(time(NULL));

		std::vector<GameEntity*> gameEntities;
		int maxLoop = 50;
		for (int i = 0; i < cubeCount; i++)
		{
			bool next = true;
			float randomX, randomY;
			maxLoop = 50;
			do 
			{
				next = true;
				//get a random x and y
				randomX = -13.8f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (13.8f - (-13.8f))))));
				randomY = -9.7f + (static_cast <float> (rand() / (static_cast <float> (RAND_MAX / (9.7f - (-9.7f))))));
				if (i == 0)
				{
					randomX = -13.8f;
					randomY = -9.7f;
				}
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
				glm::vec3(randomX, randomY, 1.5f),
				glm::vec3(0.f, 0.f, 0.f),
				glm::vec3(1.f, 1.f, 1.f),
				glm::vec3((rand() % 255) / 255.f, (rand() % 255) / 255.f, (rand() % 255) / 255.f)
			);
			gameEntities.push_back(myGameEntity);
		}


		//set up list of trigger points
		std::vector<int> graph;

		graph.push_back(0);

		int nextCheck = 0;

		do 
		{

			float closest = -1;
			int index = -1;
			for (int i = 0; i < gameEntities.size(); i++)
			{
				//find closest next point to add that hasnt already been added to the list
				float distance = glm::distance(gameEntities[nextCheck]->position, gameEntities[i]->position);
				if (distance < closest || closest == -1)
				{
					bool found = false;
					for (int j = 0; j < graph.size(); j++)
					{
						if (graph[j] == i)
						{
							found = true;
						}
					}

					if (!found)
					{
						closest = distance;
						index = i;
					}
				}
			}

			graph.push_back(index);
			nextCheck = index;

		} while (graph.size() < cubeCount);

		Mesh* floorMesh = new Mesh();
		floorMesh->InitWithVertexArray(vertices, _countof(vertices), shaderProgram);
		Material* floorMat = new Material(shaderProgram);

		GameEntity* floor = new GameEntity(
			myMesh,
			myMaterial,
			glm::vec3(0.f, -10.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(100.f, 0.f, 100.f),
			glm::vec3(255.f, 255.f, 255.f)
		);

		Input::GetInstance()->Init(window);

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

		int cubeSpinCooldown = 0;
		int curSpinNum = 0;
		bool sKeyDown = false;
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
			for (int i = 0; i < cubeCount; i++)
			{
				gameEntities[i]->Update();
			}
			floor->Update();
			myCamera->Update();


            /* PRE-RENDER */
            {
                //start off with clearing the 'color buffer'
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glClearColor(0.f, 0.f, 0.f, 1.0f);
            }

            /* RENDER */
			for (int i = 0; i < cubeCount; i++)
			{
				gameEntities[i]->Render(myCamera);
			}

			floor->Render(myCamera);

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
		for (int i = 0; i < cubeCount; i++)
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