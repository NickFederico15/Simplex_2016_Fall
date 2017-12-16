#ifndef NODE_H
#define NODE_H

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <algorithm>
#include <iterator>
#include <vector>
#include <map>

class Node
{
public:
	Node(glm::vec3 pos = glm::vec3());
	Node(const Node& node);
	~Node();

	// get and set
	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	glm::mat4 getMat();
	std::vector<Node*> getAdjacentNodes();

	// node methods
	static std::vector<Node*> getPath(Node &beginning, Node &end);
	void addAdjacent(Node &node);
	void removeAdjacent(Node &node);
	void clearAdjacent();
	void reset();

	// operator methods
	void operator=(const Node& node);
	bool operator==(const Node& node);
	friend bool operator==(const Node &nodeA, const Node &nodeB);
	friend bool operator<(const Node &nodeA, const Node &nodeB);
	friend bool operator>(const Node &nodeA, const Node &nodeB);

private:
	glm::vec3 position;
	glm::mat4 transMatrix;
	std::vector<Node*> adjacentNodes;

	// scores and bool for A*
	GLfloat f_A = FLT_MAX;
	GLfloat f_B = FLT_MAX;
	bool inQueue = false;
};

// compares node pointers
struct Sort
{
	bool operator()(const Node* nodeA, const Node* nodeB) const;
};
#endif
