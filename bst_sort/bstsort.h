#ifndef _BstNode_h
#define _BstNode_h

#endif

// Binary search tree definition
typedef struct BstNode
{
	// Pointer to a string
	char* data;
	// Pointer to left child
	struct BstNode* left;
	// Pointer to right child
	struct BstNode* right;
	// Counts occurences of data
	int count;

}BstNode;

BstNode* getNewNode(char*);
BstNode* insert(BstNode*, char*, int);
int stringCaseCompare(const char*, const char*);
int stringNoCaseCompare(const char*, const char*);
void printTree(BstNode*);
BstNode* deleteTree(BstNode*, int);
BstNode* deleteNode(BstNode*, char*, int);
BstNode* findMin(BstNode*);
int findNode(BstNode*, char*, int);