#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// stores the points
	division = new vector3[a_nSubdivisions];

	// tip and base points
	vector3 tip(0, a_fHeight / 2.0, 0);
	vector3 base(0, -a_fHeight / 2.0, 0);

	// runs through the subdivisions calculates the points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		division[i] = vector3(cos((360 / a_nSubdivisions * i) * PI / 180) * a_fRadius, -a_fHeight / 2.0, sin((360 / a_nSubdivisions * i) * PI / 180) * a_fRadius);
	}

	// loops subdivisions
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// negative or positive
		if (i < a_nSubdivisions - 1)
		{
			// draws top tris
			AddTri(tip, division[i + 1], division[i]);

			// draws bottom tris
			AddTri(base, division[i], division[i + 1]);
		}
		else
		{
			// draws top tris
			AddTri(division[0], division[i], tip);

			// draws bottom tris
			AddTri(division[i], division[0], base);
		}
	}

	// deletes all pointers
	if (division != nullptr) 
	{
		delete[] division;
		division = nullptr;
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// stores the points
	topFace = new vector3[a_nSubdivisions];
	bottomFace = new vector3[a_nSubdivisions];

	// top and bottom middle points
	vector3 topMiddle(0, a_fHeight / 2, 0);
	vector3 bottomMiddle(0, -a_fHeight / 2, 0);

	// runs through the subdivisions calculates the top points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		topFace[i] = vector3(cos((360 / a_nSubdivisions * i) * PI / 180) * a_fRadius, a_fHeight / 2.0, sin((360 / a_nSubdivisions * i) * PI / 180) * a_fRadius);
	}

	// runs through the subdivisions calculates the bottom points
	for (int i = 0; i < a_nSubdivisions; i++) 
	{
		bottomFace[i] = vector3(cos((360 / a_nSubdivisions * i) * PI / 180) * a_fRadius, -a_fHeight / 2.0, sin((360 / a_nSubdivisions * i) * PI / 180) * a_fRadius);
	}

	// loops subdivisions
	for (int i = 0; i < a_nSubdivisions; i++) 
	{
		// negative or positive
		if (i < a_nSubdivisions - 1) 
		{
			// draws top and bottom tris
			AddTri(topFace[i + 1], topFace[i], topMiddle);
			AddTri(bottomFace[i], bottomFace[i + 1], bottomMiddle);

			// draws side quads
			AddQuad(topFace[i], topFace[i + 1], bottomFace[i], bottomFace[i + 1]);
		}
		else
		{
			// draws top and bottom tris
			AddTri(topFace[0], topFace[i], topMiddle);
			AddTri(bottomFace[i], bottomFace[0], bottomMiddle);

			// draws side quads
			AddQuad(topFace[i], topFace[0], bottomFace[i], bottomFace[0]);
		}
	}

	// deletes all pointers
	if (bottomFace != nullptr) 
	{
		delete[] bottomFace;
		bottomFace = nullptr;
	}

	if (topFace != nullptr) 
	{
		delete[] topFace;
		topFace = nullptr;
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// stores the points
	bottomOutside = new vector3[a_nSubdivisions];
	bottomInside = new vector3[a_nSubdivisions];
	topOutside = new vector3[a_nSubdivisions];
	topInside = new vector3[a_nSubdivisions];

	// top and bottom middle points
	vector3 bottomMiddle(0, -a_fHeight / 2, 0);
	vector3 topMiddle(0, a_fHeight / 2, 0);

	// runs through the subdivisions calculates the bottom points
	for (int i = 0; i < a_nSubdivisions; i++) 
	{
		bottomOutside[i] = vector3(cos((360 / a_nSubdivisions * i) * PI / 180) * a_fOuterRadius, -a_fHeight / 2.0, sin((360 / a_nSubdivisions * i) * PI / 180) * a_fOuterRadius);
		
		bottomInside[i] = vector3(cos((360 / a_nSubdivisions * i) * PI / 180) * a_fInnerRadius, -a_fHeight / 2.0, sin((360 / a_nSubdivisions * i) * PI / 180) * a_fInnerRadius);
	}

	// runs through the subdivisions calculates the top points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		topOutside[i] = vector3(cos((360 / a_nSubdivisions * i) * PI / 180) * a_fOuterRadius, a_fHeight / 2.0, sin((360 / a_nSubdivisions*i) * PI / 180) * a_fOuterRadius);
		
		topInside[i] = vector3(cos((360 / a_nSubdivisions * i) * PI / 180) * a_fInnerRadius, a_fHeight / 2.0, sin((360 / a_nSubdivisions*i) * PI / 180) * a_fInnerRadius);
	}

	// lops the subdivisions
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// negative or positive
		if (i < a_nSubdivisions - 1)
		{
			// draws top quads
			AddQuad(topOutside[i + 1], topOutside[i], topInside[i + 1], topInside[i]);

			// draws bottom quads
			AddQuad(bottomOutside[i], bottomOutside[i + 1], bottomInside[i], bottomInside[i + 1]);

			// draws inside quads
			AddQuad(topInside[i + 1], topInside[i], bottomInside[i + 1], bottomInside[i]);

			// draws outside quads
			AddQuad(topOutside[i], topOutside[i + 1], bottomOutside[i], bottomOutside[i + 1]);
		}
		else 
		{
			// draws top quads
			AddQuad(topOutside[0], topOutside[i], topInside[0], topInside[i]);

			// draws bottom quads
			AddQuad(bottomOutside[i], bottomOutside[0], bottomInside[i], bottomInside[0]);

			// draws inside quads
			AddQuad(topInside[0], topInside[i], bottomInside[0], bottomInside[i]);

			// draws outside quads
			AddQuad(topOutside[i], topOutside[0], bottomOutside[i], bottomOutside[0]);
		}
	}

	// deletes all pointers
	if (bottomOutside != nullptr) 
	{
		delete[] bottomOutside;
		bottomOutside = nullptr;
	}

	if (topOutside != nullptr)
	{
		delete[] topOutside;
		topOutside = nullptr;
	}

	if (bottomInside != nullptr)
	{
		delete[] bottomInside;
		bottomInside = nullptr;
	}

	if (topInside != nullptr)
	{
		delete[] topInside;
		topInside = nullptr;
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 10)
		a_nSubdivisions = 10;

	Release();
	Init();

	// loops subdivisions
	for (int i = 0; i < a_nSubdivisions; i++) 
	{
		// find the first 2 angles of the current subdivision
		float angle1 = PI / a_nSubdivisions * i;
		float angle2 = PI / a_nSubdivisions * (i + 1);

		for (int j = 0; j < a_nSubdivisions; j++) 
		{
			// finds the second 2 angles of the subdivision
			float angle3 = (2 * PI) / a_nSubdivisions * j;
			float angle4 = (2 * PI) / a_nSubdivisions * (j + 1);

			// caculates all 4 points of the current quad
			vector3 a(a_fRadius * cos(angle3) * sin(angle1), a_fRadius * sin(angle3) * sin(angle1), a_fRadius * cos(angle1));
			vector3 b(a_fRadius * cos(angle3) * sin(angle2), a_fRadius * sin(angle3) * sin(angle2), a_fRadius * cos(angle2));
			vector3 c(a_fRadius * cos(angle4) * sin(angle1), a_fRadius * sin(angle4) * sin(angle1), a_fRadius * cos(angle1));
			vector3 d(a_fRadius * cos(angle4) * sin(angle2), a_fRadius * sin(angle4) * sin(angle2), a_fRadius * cos(angle2));
			
			// draws the current quad
			AddQuad(a, b, c, d);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}