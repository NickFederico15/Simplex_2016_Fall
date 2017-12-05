#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount;
uint MyOctant::m_uLeafCount;
uint MyOctant::m_uMaxLevel;
uint MyOctant::m_uIdealEntityCount;

// constructor for the root
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	// makes root octant
	Init();

	m_pRoot = this;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	// sets the max and min to the center
	m_v3Max = m_v3Min = m_pEntityMngr->GetRigidBody()->GetCenterGlobal();

	// gets the count
	m_uCurrEntityCount = m_pEntityMngr->GetEntityCount();

	for (uint i = 0; i < m_uCurrEntityCount; ++i) {
		m_lEntityList.push_back(i);

		// get the min and max
		vector3 rb_min = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal();
		vector3 rb_max = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal();

		// set octants min and max
		if (rb_min.x < m_v3Min.x) 
			m_v3Min.x = rb_min.x;

		if (rb_max.x > m_v3Max.x)
			m_v3Max.x = rb_max.x;

		if (rb_min.y < m_v3Min.y) 
			m_v3Min.y = rb_min.y;

		if (rb_max.y > m_v3Max.y)
			m_v3Max.y = rb_max.y;

		if (rb_min.z < m_v3Min.z) 
			m_v3Min.z = rb_min.z;

		if (rb_max.z > m_v3Max.z) 
			m_v3Max.z = rb_max.z;
	}

	// gets the center and the size
	m_v3Center = (m_v3Min + m_v3Max) / 2.f;
	m_v3Size = m_v3Max - m_v3Min;

	// creates children
	Subdivide();

	// adds dimensions to leaf
	ConfigureDimensions();
}

// constructor for the branch and leaf
MyOctant::MyOctant(vector3 a_v3Center, vector3 a_v3Size)
{
	Init();

	m_v3Center = a_v3Center;
	m_v3Size = a_v3Size;
	m_v3Max = a_v3Center + m_v3Size / 2.f;
	m_v3Min = a_v3Center - m_v3Size / 2.f;
}

// copy constructor
MyOctant::MyOctant(MyOctant const & other)
{
	Init();

	// copying data
	m_uLevel = other.m_uLevel;
	m_v3Size = other.m_v3Size;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	m_pParent = other.m_pParent;

	// loops to create nodes
	m_uChildren = other.m_uChildren;

	for (uint i = 0; i < m_uChildren; ++i) {
		m_pChild[i] = new MyOctant(*other.m_pChild[i]);
	}

	// creates a new entity list
	m_uCurrEntityCount = other.m_uCurrEntityCount;

	for (uint i = 0; i < m_uCurrEntityCount; ++i) {
		m_lEntityList.push_back(other.m_lEntityList[i]);
	}

	// if its a root, copy the children into the list
	m_pRoot = other.m_pRoot;

	if (this == m_pRoot) {
		float fChildren = other.m_lChildren.size();

		for (uint i = 0; i < fChildren; ++i) {
			m_lChildren.push_back(other.m_lChildren[i]);
		}
	}
}

// copy assignment operator
MyOctant & MyOctant::operator=(MyOctant const & other)
{
	if (&other == this)
		return *this;

	// release initial value and creates error if you try to make the child node the parent node
	Release();
	Init();

	// copying data
	m_uLevel = other.m_uLevel;
	m_v3Size = other.m_v3Size;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	m_pParent = other.m_pParent;

	// loops to create nodes
	m_uChildren = other.m_uChildren;

	for (uint i = 0; i < m_uChildren; ++i) {
		m_pChild[i] = new MyOctant(*other.m_pChild[i]);
	}

	// creates a new entity list
	m_uCurrEntityCount = other.m_uCurrEntityCount;

	for (uint i = 0; i < m_uCurrEntityCount; ++i) {
		m_lEntityList.push_back(other.m_lEntityList[i]);
	}

	// if its a root, copy children
	m_pRoot = other.m_pRoot;

	if (this == m_pRoot) {
		float fChildCount = other.m_lChildren.size();

		for (uint i = 0; i < fChildCount; ++i) {
			m_lChildren.push_back(other.m_lChildren[i]);
		}
	}

	return *this;
}

// destructor
MyOctant::~MyOctant(void)
{
	Release();
}

void MyOctant::Swap(MyOctant & other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_v3Size, other.m_v3Size);
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_pParent, other.m_pParent);
	std::swap(m_pChild, other.m_pChild);
	std::swap(m_lEntityList, other.m_lEntityList);
	std::swap(m_uCurrEntityCount, other.m_uCurrEntityCount);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChildren, other.m_lChildren);
}

// getters
vector3 MyOctant::GetSize(void) { return m_v3Size; }
vector3 MyOctant::GetCenterGlobal(void) { return m_v3Center; }
vector3 MyOctant::GetMinGlobal(void) { return m_v3Min; }
vector3 MyOctant::GetMaxGlobal(void) { return m_v3Max; }

uint MyOctant::GetOctantCount(void) { return m_uOctantCount; }
uint Simplex::MyOctant::GetLeafCount(void) { return m_uLeafCount; }

MyOctant * MyOctant::GetParent(void) { return m_pParent; }
MyOctant * MyOctant::GetChild(uint a_nChild)
{
	if (m_uChildren == 0)
		return nullptr;
	else return m_pChild[a_nChild];
}


// helper functions
bool MyOctant::IsLeaf(void) { 
	return m_uChildren == 0; 
}

bool MyOctant::ContainsMoreThan(uint a_nEntities) { 
	return m_uCurrEntityCount > a_nEntities; 
}


// checks if a rigidbody collides with the octant
bool MyOctant::IsColliding(uint a_uRBIndex)
{
	MyRigidBody* rb = m_pEntityMngr->GetRigidBody(a_uRBIndex);

	vector3 rb_max = rb->GetMaxGlobal();
	vector3 rb_min = rb->GetMinGlobal();

	if (rb_max.x > m_v3Min.x && rb_min.x < m_v3Max.x && rb_max.y > m_v3Min.y && rb_min.y < m_v3Max.y && rb_max.z > m_v3Min.z && rb_min.z < m_v3Max.z) {
		return true;
	}
	else 
		return false;
}

// function to show selected octants
void Simplex::MyOctant::Display(uint a_uIndex, vector3 a_v3Color)
{
	if (a_uIndex >= m_uOctantCount) {
		DisplayAll();
		return;
	}

	m_lChildren[a_uIndex]->DisplayCurrent(a_v3Color);
}

// helper function that displays only an octant
void MyOctant::DisplayCurrent(vector3 a_v3Color)
{
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(IDENTITY_M4, m_v3Size), a_v3Color);
}

// helper function to display every octant
void Simplex::MyOctant::DisplayAll(vector3 a_v3Color)
{
	if (IsLeaf()) {
		DisplayCurrent(a_v3Color);
	}
	else {
		for (uint i = 0; i < m_uChildren; ++i) {
			m_pChild[i]->DisplayAll(a_v3Color);
		}
	}
}

// clears child nodes
void MyOctant::ClearEntityList(void)
{
	for (uint i = 0; i < m_uChildren; ++i) {
		m_pChild[i]->ClearEntityList();
	}

	m_lEntityList.clear();
}

// divides octants
void MyOctant::Subdivide(void)
{
	// stops subdividing if level reaches of 5 or less
	if (m_uLevel >= m_uMaxLevel || !ContainsMoreThan(m_uIdealEntityCount)) {
		m_pRoot->m_lChildren.push_back(this);
		m_uLeafCount += 1;
		return;
	}

	if (m_uChildren == 8) {
		std::cout << "Octant already subdivided" << std::endl;
		return;
	}

	// creates each octant at a position
	m_pChild[0] = new MyOctant(m_v3Center + vector3(-m_v3Size.x / 4, m_v3Size.y / 4, -m_v3Size.z / 4), m_v3Size / 2.f);
	m_pChild[1] = new MyOctant(m_v3Center + vector3(-m_v3Size.x / 4, m_v3Size.y / 4, m_v3Size.z / 4), m_v3Size / 2.f);
	m_pChild[2] = new MyOctant(m_v3Center + vector3(-m_v3Size.x / 4, -m_v3Size.y / 4, -m_v3Size.z / 4), m_v3Size / 2.f);
	m_pChild[3] = new MyOctant(m_v3Center + vector3(-m_v3Size.x / 4, -m_v3Size.y / 4, m_v3Size.z / 4), m_v3Size / 2.f);
	m_pChild[4] = new MyOctant(m_v3Center + vector3(m_v3Size.x / 4, -m_v3Size.y / 4, -m_v3Size.z / 4), m_v3Size / 2.f);
	m_pChild[5] = new MyOctant(m_v3Center + vector3(m_v3Size.x / 4, -m_v3Size.y / 4, m_v3Size.z / 4), m_v3Size / 2.f);
	m_pChild[6] = new MyOctant(m_v3Center + vector3(m_v3Size.x / 4, m_v3Size.y / 4, -m_v3Size.z / 4), m_v3Size / 2.f);
	m_pChild[7] = new MyOctant(m_v3Center + vector3(m_v3Size.x / 4, m_v3Size.y / 4, m_v3Size.z / 4), m_v3Size / 2.f);
	m_uChildren = 8;

	// loops and initializes children
	for (uint i = 0; i < m_uChildren; ++i) {
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_pRoot = m_pRoot;

		// loops and adds rigidbodies
		for (uint j = 0; j < m_uCurrEntityCount; ++j) {
			if (m_pChild[i]->IsColliding(m_lEntityList[j]))
				m_pChild[i]->m_lEntityList.push_back(m_lEntityList[j]);
		}

		// updates entity count
		m_pChild[i]->m_uCurrEntityCount = m_pChild[i]->m_lEntityList.size();

		// recursive
		m_pChild[i]->Subdivide();
	}
}

// deletes all nodes except the root
void MyOctant::DeleteBranches(void)
{
	if (IsLeaf()) {
		return;
	}
	else {
		for (uint i = 0; i < m_uChildren; ++i) {
			m_pChild[i]->DeleteBranches();
			SafeDelete(m_pChild[i]);
		}
	}
}

// configures dimensions for leaves
void Simplex::MyOctant::ConfigureDimensions()
{
	if (IsLeaf()) {
		for (uint i = 0; i < m_uCurrEntityCount; ++i) {
			m_pEntityMngr->AddDimension(m_lEntityList[i], m_uID);
		}
	}
	else {
		for (uint i = 0; i < m_uChildren; ++i) {
			m_pChild[i]->ConfigureDimensions();
		}
	}
}

// release
void MyOctant::Release(void)
{
	if (this == m_pRoot) {
		DeleteBranches();
	}
}

// initilization
void MyOctant::Init(void)
{
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	m_uID = m_uOctantCount;

	m_uOctantCount += 1;
}