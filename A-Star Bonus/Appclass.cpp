#include "Appclass.h"

Simplex::CameraManager* Appclass::cameraManager = nullptr;
Simplex::SystemSingleton* Appclass::system = nullptr;

std::vector<Node> Appclass::adjacenyList;
GLuint Appclass::currentNode = 0;

bool Appclass::runPath = false;
bool Appclass::getPath = false;

GLfloat Appclass::cameraSpeed = 0.5f;

// Initialize the window
GLint Appclass::init()
{
	width = 700;
	height = 500;

	std::cout << "Initializing GLFW" << std::endl;

	// checks if glfw fails
	if (!glfwInit())
	{
		std::cout << "GLFW Failed to Initialize" << std::endl;
		return -1;
	}

	// callback error
	glfwSetErrorCallback(printError);

	// GLFW window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(width, height, "A* Bonus Assignment - Nick Federico", nullptr, nullptr);

	// checks if the window fails to start
	if (window == nullptr)
	{
		std::cout << "Failed to make window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// key callback
	glfwSetKeyCallback(window, userInput);
	glfwSetCursorPos(window, this->width / 2, this->height / 2);

	std::cout << "Initializing GLEW" << std::endl;
	glewExperimental = GL_TRUE;

	//Check if glew initializes
	if (glewInit() != GLEW_OK)
	{
		std::cout << " GLEW failed to initialize" << std::endl;
		return -1;
	}

	glfwGetFramebufferSize(window, &this->width, &this->height);

	glClearColor(0.3f, 0.8f, 0.7f, 1.0f);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// sets up nodes
	adjacenyList.push_back(Node(glm::vec3(-10.0f, 0.0f, 10.0f)));
	adjacenyList.push_back(Node(glm::vec3(0.0f, 0.0f, 10.0f)));
	adjacenyList.push_back(Node(glm::vec3(10.0f, 0.0f, 10.0f)));
	adjacenyList.push_back(Node(glm::vec3(-10.0f, 0.0f, 0.0f)));
	adjacenyList.push_back(Node(glm::vec3(0.0f, 0.0f, 0.0f)));
	adjacenyList.push_back(Node(glm::vec3(10.0f, 0.0f, 0.0f)));
	adjacenyList.push_back(Node(glm::vec3(-10.0f, 0.0f, -10.0f)));
	adjacenyList.push_back(Node(glm::vec3(0.0f, 0.0f, -10.0f)));
	adjacenyList.push_back(Node(glm::vec3(10.0f, 0.0f, -10.0f)));
	adjacenyList.push_back(Node(glm::vec3(0.0f, 0.0f, -20.0f)));

	// sets up how nodes are connected
	adjacenyList[0].addAdjacent(adjacenyList[1]);
	adjacenyList[0].addAdjacent(adjacenyList[3]);

	adjacenyList[1].addAdjacent(adjacenyList[0]);
	adjacenyList[1].addAdjacent(adjacenyList[2]);
	adjacenyList[1].addAdjacent(adjacenyList[4]);

	adjacenyList[2].addAdjacent(adjacenyList[1]);
	adjacenyList[2].addAdjacent(adjacenyList[5]);

	adjacenyList[3].addAdjacent(adjacenyList[0]);
	adjacenyList[3].addAdjacent(adjacenyList[4]);
	adjacenyList[3].addAdjacent(adjacenyList[6]);

	adjacenyList[4].addAdjacent(adjacenyList[1]);
	adjacenyList[4].addAdjacent(adjacenyList[3]);
	adjacenyList[4].addAdjacent(adjacenyList[5]);
	adjacenyList[4].addAdjacent(adjacenyList[7]);

	adjacenyList[5].addAdjacent(adjacenyList[2]);
	adjacenyList[5].addAdjacent(adjacenyList[4]);
	adjacenyList[5].addAdjacent(adjacenyList[8]);

	adjacenyList[6].addAdjacent(adjacenyList[3]);
	adjacenyList[6].addAdjacent(adjacenyList[7]);
	adjacenyList[6].addAdjacent(adjacenyList[9]);

	adjacenyList[7].addAdjacent(adjacenyList[4]);
	adjacenyList[7].addAdjacent(adjacenyList[6]);
	adjacenyList[7].addAdjacent(adjacenyList[8]);
	adjacenyList[7].addAdjacent(adjacenyList[9]);

	adjacenyList[8].addAdjacent(adjacenyList[5]);
	adjacenyList[8].addAdjacent(adjacenyList[7]);
	adjacenyList[8].addAdjacent(adjacenyList[9]);

	adjacenyList[9].addAdjacent(adjacenyList[6]);
	adjacenyList[9].addAdjacent(adjacenyList[7]);
	adjacenyList[9].addAdjacent(adjacenyList[8]);

	return 0;
}

GLint Appclass::run()
{
	// setup simplex
	Simplex::MeshManager* meshManager = Simplex::MeshManager::GetInstance();
	system = Simplex::SystemSingleton::GetInstance();
	cameraManager = Simplex::CameraManager::GetInstance();

	cameraManager->SetPositionTargetAndUp(glm::vec3(0.0f, 50.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));


	GLfloat dTime = 0;
	unsigned int uClock = system->GenClock();

	// setup lerp
	GLuint route = 0;
	GLfloat counter = 0;

	glm::vec3 cubePosition = adjacenyList[0].getPosition();
	GLuint cubeIndex = 0;
	glm::mat4 cubeMatrix;

	// pointers to nodes and paths
	std::vector<Node*> adjacentNodes;
	std::vector<Node*> path;

	while (!glfwWindowShouldClose(window))
	{
		// render skybox
		meshManager->AddSkyboxToRenderList();

		cubeMatrix = glm::mat4();
		dTime += system->GetDeltaTime(uClock);

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render nodes as icospheres
		for (GLuint i = 0; i < adjacenyList.size(); i++)
		{
			if (i == currentNode) {
				meshManager->AddIcoSphereToRenderList(adjacenyList[i].getMat(), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else {
				meshManager->AddIcoSphereToRenderList(adjacenyList[i].getMat(), glm::vec3(1.0f, 0.0f, 0.0f));
			}

			// get adjacent nodes and render lines between
			adjacentNodes = adjacenyList[i].getAdjacentNodes();

			for (GLuint j = 0; j < adjacentNodes.size(); j++)
			{
				meshManager->AddLineToRenderList(glm::mat4(1.0f), adjacenyList[i].getPosition(), adjacentNodes[j]->getPosition(), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
		}

		// renders cube
		cubeMatrix = glm::translate(cubeMatrix, cubePosition);
		cubeMatrix = glm::scale(cubeMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
		meshManager->AddCubeToRenderList(cubeMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

		// uses A* to move cube
		if (runPath)
		{
			counter += 0.01f;

			// gets A* path
			if (getPath)
			{
				for (GLuint i = 0; i < adjacenyList.size(); i++) {
					// resets nodes
					adjacenyList[i].reset();
				}

				path = Node::getPath(adjacenyList[cubeIndex], adjacenyList[currentNode]);
				getPath = false;
			}

			if (route < path.size() - 1)
			{
				// lerp from current to next
				cubePosition = glm::lerp(path[route]->getPosition(), path[route + 1]->getPosition(), counter);
			}
			else // if its the last node
			{
				runPath = false;
				route = 0;
				counter = 0.0f;

				cubeIndex = currentNode;
				cubePosition = adjacenyList[cubeIndex].getPosition();
			}

			// check if counter has finished
			if (counter >= 1.0f)
			{
				route++;
				route %= path.size();

				counter = 0;
			}
		}

		// renders scene
		meshManager->Render();

		// clears render list
		meshManager->ClearRenderList();

		// swaps buffers
		glfwSwapBuffers(window);
	}
	return 0;
}

GLint Appclass::end()
{
	// releases singletons
	Simplex::ReleaseAllSingletons();
	glfwTerminate();
	return 0;
}

void Appclass::printError(GLint error, const GLchar* errorMessage)
{
	std::cout << "Error: " << error << errorMessage << std::endl;
}

void Appclass::userInput(GLFWwindow* window, GLint key, GLint scancode, GLint press, GLint mods)
{
	// switches between nodes
	switch (key)
	{
	case(GLFW_KEY_0):
		currentNode = 0;
		getPath = true;
		break;
	case(GLFW_KEY_1):
		currentNode = 1;
		getPath = true;
		break;
	case(GLFW_KEY_2):
		currentNode = 2;
		getPath = true;
		break;
	case(GLFW_KEY_3):
		currentNode = 3;
		getPath = true;
		break;
	case(GLFW_KEY_4):
		currentNode = 4;
		getPath = true;
		break;
	case(GLFW_KEY_5):
		currentNode = 5;
		getPath = true;
		break;
	case(GLFW_KEY_6):
		currentNode = 6;
		getPath = true;
		break;
	case(GLFW_KEY_7):
		currentNode = 7;
		getPath = true;
		break;
	case(GLFW_KEY_8):
		currentNode = 8;
		getPath = true;
		break;
	case(GLFW_KEY_9):
		currentNode = 9;
		getPath = true;
		break;
	default:
		break;
	}

	// moves camera
	if (key == GLFW_KEY_W && (press == GLFW_PRESS || press == GLFW_REPEAT))
		cameraManager->MoveForward(cameraSpeed);
	if (key == GLFW_KEY_S && (press == GLFW_PRESS || press == GLFW_REPEAT))
		cameraManager->MoveForward(-cameraSpeed);
	if (key == GLFW_KEY_D && (press == GLFW_PRESS || press == GLFW_REPEAT))
		cameraManager->MoveSideways(cameraSpeed);
	if (key == GLFW_KEY_A && (press == GLFW_PRESS || press == GLFW_REPEAT))
		cameraManager->MoveSideways(-cameraSpeed);
	if (key == GLFW_KEY_E && (press == GLFW_PRESS || press == GLFW_REPEAT))
		cameraManager->MoveVertical(cameraSpeed);
	if (key == GLFW_KEY_Q && (press == GLFW_PRESS || press == GLFW_REPEAT))
		cameraManager->MoveVertical(-cameraSpeed);
	if (key == GLFW_KEY_SPACE && press == GLFW_PRESS)
		runPath = !runPath;
}