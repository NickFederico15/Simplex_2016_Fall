#include "Node.h"

// constructor
Node::Node(glm::vec3 pos)
{
	this->position = pos;

	// sets up matrix
	transMatrix = glm::translate(transMatrix, pos);
}

// copy constructor
Node::Node(const Node& node)
{
	this->position = node.position;
	this->transMatrix = node.transMatrix;
	this->adjacentNodes = node.adjacentNodes;
	this->f_A = node.f_A;
	this->f_B = node.f_B;
}

// gets the node position
glm::vec3 Node::getPosition()
{
	return this->position;
}

// sets the node position
void Node::setPosition(glm::vec3 pos)
{
	this->position = pos;
}

// gets the transform matrix
glm::mat4 Node::getMat()
{
	return transMatrix;
}

// gets the adjacent nodes
std::vector<Node*> Node::getAdjacentNodes()
{
	return adjacentNodes;
}

// A* (shortest path)
std::vector<Node*> Node::getPath(Node &beginning, Node &end)
{
	std::vector<Node*> incomplete; // vector of nodes that haven't been searched
	std::vector<Node*> complete; // vector of nodes that have been searched
	std::map<Node*, Node*> origin; // map of nodes and their origin
	std::map<Node*, GLfloat> cost; // map and cost of nodes

	bool skip;
	Node* current = nullptr;

	// set beginnings
	beginning.f_A = 0;
	beginning.f_B = glm::distance(beginning.position, end.position);

	// push beginnings to incomplete
	incomplete.push_back(&beginning);

	// set the cost to 0
	cost[&beginning] = 0;

	beginning.inQueue = true;

	// loop list
	while (!incomplete.empty())
	{
		// get top node
		current = incomplete.front();

		// check if current node is the end one
		if (*current == end)
		{
			// nodes on path
			std::vector<Node*> pathNodes;

			// adds end node to path
			pathNodes.push_back(&end);

			Node* temp = &end;

			// if temp isn't beginning, loop
			while (temp != &beginning)
			{
				// get temps origin and add it to the nodes path
				temp = origin[temp];
				pathNodes.push_back(temp);
			}

			// reverse vector
			std::reverse(pathNodes.begin(), pathNodes.end());

			return pathNodes;
		}

		// remove node from incomplete and add it to completes
		incomplete.erase(incomplete.begin());
		complete.push_back(current);

		// loops adjacent nodes
		for (GLuint i = 0; i < current->adjacentNodes.size(); i++)
		{
			skip = false;

			// checks if node has been completed
			for (GLuint j = 0; j < complete.size(); j++)
			{
				if (current->adjacentNodes[i] == complete[j])
				{
					skip = true;
					break;
				}
			}

			// skip if checked
			if (skip)
				continue;

			// if node isn't in incomplete, add it
			if (!current->adjacentNodes[i]->inQueue)
			{
				incomplete.push_back(current->adjacentNodes[i]);
				current->adjacentNodes[i]->inQueue = true;

				// sort the vector
				std::sort(std::begin(incomplete), std::end(incomplete), NodeCompare());
			}

			// get a new float for adjacent node
			GLfloat f_New = cost[current] + glm::distance(current->position, current->adjacentNodes[i]->position);

			// Check if the float is less than the current float for the node
			if (f_New < current->adjacentNodes[i]->f_A)
			{
				// adds adjacent node to origins list with the current node
				origin[current->adjacentNodes[i]] = current;
				current->adjacentNodes[i]->f_A = f_New;
				current->adjacentNodes[i]->f_B = glm::abs(glm::distance(current->adjacentNodes[i]->position, end.position));

				// adds cost to map
				cost[current->adjacentNodes[i]] = f_New + current->adjacentNodes[i]->f_B;
			}


		}
	}
}

// Adds an adjacent node to the current
void Node::addAdjacent(Node &node)
{
	adjacentNodes.push_back(&node);
}

// Removes an adjacent node from the current
void Node::removeAdjacent(Node &node)
{
	for (GLuint i = 0; i < adjacentNodes.size(); i++)
	{
		if (adjacentNodes[i] == &node)
			adjacentNodes.erase(adjacentNodes.begin() + i);
	}
}

// Clears all the adjacent nodes
void Node::clearAdjacent()
{
	adjacentNodes.clear();
}

// resets the queue value
void Node::reset()
{
	inQueue = false;
	f_A = FLT_MAX;
	f_B = FLT_MAX;
}

// assignment operator
void Node::operator=(const Node& node)
{
	this->position = node.position;
	this->transMatrix = node.transMatrix;
	this->adjacentNodes = node.adjacentNodes;
	this->f_A = node.f_A;
	this->f_B = node.f_B;
}

// equality operator
bool Node::operator==(const Node& node)
{
	if (this->position == node.position && this->adjacentNodes == node.adjacentNodes)
		return true;

	return false;
}

// equality operator
bool operator==(const Node& nodeA, const Node& nodeB)
{
	if (nodeA.position == nodeB.position)
		return true;

	return false;
}

// less than operator
bool operator<(const Node& nodeA, const Node& nodeB)
{
	return ((nodeA.f_B + nodeA.f_A) < (nodeB.f_B + nodeB.f_A));
}

// greater than operator
bool operator>(const Node& nodeA, const Node& nodeB)
{
	return ((nodeA.f_B + nodeA.f_A) > (nodeB.f_B + nodeB.f_A));
}

// destructor
Node::~Node()
{
	adjacentNodes.clear();
}

// sorts pointers
bool NodeCompare::operator()(const Node* nodeA, const Node* nodeB) const
{
	return *nodeA < *nodeB;
}