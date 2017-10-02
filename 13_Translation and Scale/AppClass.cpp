#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Nick Federico - njf1994@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	// init the mesh with 11 columns and 8 rows, filling only 
	// the spaces that contain cubes
	const int columns = 11;
	const int rows = 8;

	// creates the shape of the alien
	bool alienCubes[rows][columns] = 
	{
		{ false, false, false, true, true, false, true, true, false, false, false },
		{ true, false, true, false, false, false, false, false, true, false, true },
		{ true, false, true, true, true, true, true, true, true, false, true },
		{ true, true, true, true, true, true, true, true, true, true, true },
		{ false, true, true, false, true, true, true, false, true, true, false },
		{ false, false, true, true, true, true, true, true, true, false, false },
		{ false, false, false, true, false, false, false, true, false, false, false },
		{ false, false, true, false, false, false, false, false, true, false, false },
	};

	// runs through all rows and columns
	for (int i = 0; i < rows; i++) 
	{
		for (int j = 0; j < columns; j++) 
		{
			if (alienCubes[i][j]) 
			{
				// creates temp mesh
				MyMesh* temp = new MyMesh();

				// uses the temp mesh to create a cube
				temp->GenerateCube(1.0f, C_BLACK);

				temp->x = j;
				temp->y = i;

				// adds the temp cube to the alienMesh vector
				alienMesh.push_back(temp);
			}
		}
	}
	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f,2.0f,2.0f));
	static float value = 0.0f;
	value += 0.01f;

	// translates cubes in the mesh to proper position and moves entire alien mesh
	for (int i = 0; i < alienMesh.size(); i++) 
	{
		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value + alienMesh[i]->x, alienMesh[i]->y, 3.0f));
		matrix4 m4Model = m4Scale * m4Translate;
		alienMesh[i]->Render(m4Projection, m4View, m4Model);
	}
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	for (int i = 0; i < alienMesh.size(); i++) 
	{
		SafeDelete(alienMesh[i]);
	}

	//release GUI
	ShutdownGUI();
}