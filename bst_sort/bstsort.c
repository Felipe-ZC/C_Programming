
// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "bstsort.h"

// --------------------------------- BST operations ---------------------------------

// Returns a new BstNode with the specified argument as the data field
BstNode* getNewNode(char* str)
{
	// Allocate memory for a new node in heap
	BstNode* newNode = (BstNode*)malloc(sizeof(BstNode));
	// Allocate memory for a new string in heap
	newNode->data = (char*)malloc(sizeof(BstNode));
	// Copy contents of str into datafield
	strcpy(newNode->data,str);
	// Set left and right pointers
	newNode->left = NULL;
	newNode->right = NULL;
	// Set str count
	newNode->count = 1;
	return newNode;
}

/*
Takes in address of the root node along with a pointer to a string 
and inserts a new node into the tree. Also takes in an integer, case flag, 
that determines how the strings should be compared (case sensitive/insensitive)
*/
BstNode* insert(BstNode* rootNode, char* data, int caseFlag)
{	
	// Check for past occurences...
	if(findNode(rootNode,data,caseFlag) == 1)
	{
		rootNode->count += 1;
		return rootNode;
	}
	// If the tree is empty...
	if(rootNode == NULL)
	{
		rootNode = getNewNode(data);
	}
	// Tree is not empty, place new node in the correct subtree
	// Case sensitive
	else if(caseFlag == 1)
	{
		// Insert into right subtree
		if(stringCaseCompare(data, rootNode->data) > 0)
			rootNode->right = insert(rootNode->right,data,caseFlag);
		// Insert into left subtree
		else
			rootNode->left = insert(rootNode->left,data,caseFlag);
	}
	// Case insensitive
	else
	{
		// Insert into right subtree
		if(stringNoCaseCompare(data, rootNode->data) > 0)
			rootNode->right = insert(rootNode->right,data,caseFlag);
		// Insert into left subtree
		else
			rootNode->left = insert(rootNode->left,data,caseFlag);
	}
	return rootNode;
} 

// Returns 1 if the specified value is in the BST, returns 0 otherwise
int findNode(BstNode* rootNode, char* data, int caseFlag)
{	
	// Base case
	if(rootNode == NULL)
		return 0;

	// Node has been found!
	else if(caseFlag == 1 && stringCaseCompare(data, rootNode->data) == 0) 
		return 1;
	else if (caseFlag == 1 && stringNoCaseCompare(data, rootNode->data) == 0) 
		return 1;

	// Look for node in left subtree, CASE SENSITIVE 
	else if(caseFlag == 1 && stringCaseCompare(data, rootNode->data) < 0) 
		return findNode(rootNode->left,data,caseFlag);
	// Look for node in right subtree, CASE SENSITIVE 
	else if(caseFlag == 1 && stringCaseCompare(data, rootNode->data) > 0) 
		return findNode(rootNode->right,data,caseFlag);

	// Look for node in left subtree, CASE INSENSITIVE 
	else if(caseFlag == 0 && stringNoCaseCompare(data, rootNode->data) < 0) 
		return findNode(rootNode->left,data,caseFlag);
	// Look for node in right subtree, CASE INSENSITIVE 
	else if(caseFlag == 0 && stringNoCaseCompare(data, rootNode->data) > 0) 
		return findNode(rootNode->right,data,caseFlag);

}

/*
Returns a pointer to the smallest value in a BST,
takes in a pointer to the root node of the BST.
*/
BstNode* findMin(BstNode* rootNode)
{	
	// Visit left nodes until the leftmost leaf of the tree has been found
	if(rootNode->left == NULL)
		// Return address of min value
		return rootNode;
	else 
		findMin(rootNode->left);
}

/*
Delete specified data value from BST, returns address to new 
root node. Takes in the current root node of a BST along with
the value to be deleted and an integer that determines comparisons
(case sensitive vs case insensitive)
*/
BstNode* deleteNode(BstNode* rootNode, char* data, int caseFlag)
{
	// Search for node with specified value
	if(rootNode == NULL) return rootNode;

	// Look for node in left subtree, CASE SENSITIVE 
	if(caseFlag == 1 && stringCaseCompare(data, rootNode->data) < 0) 
		rootNode->left = deleteNode(rootNode->left,data,caseFlag);
	// Look for node in right subtree, CASE SENSITIVE 
	else if(caseFlag == 1 && stringCaseCompare(data, rootNode->data) > 0) 
		rootNode->right = deleteNode(rootNode->right,data,caseFlag);

	// Look for node in left subtree, CASE INSENSITIVE 
	else if(caseFlag == 0 && stringNoCaseCompare(data, rootNode->data) < 0) 
		rootNode->left = deleteNode(rootNode->left,data,caseFlag);
	// Look for node in right subtree, CASE INSENSITIVE 
	else if(caseFlag == 0 && stringNoCaseCompare(data, rootNode->data) > 0) 
		rootNode->right = deleteNode(rootNode->right,data,caseFlag);

	// Node has been found!
	else
	{
		// Case 1: Node has no children
		if(rootNode->left == NULL && rootNode->right == NULL)
		{
			// Reclaim memory from Node's data field
			free(rootNode->data);
			// Recalim memory from Node
			free(rootNode);
			// Detach from tree
			rootNode = NULL;
		}

		// Case 2: Node only has one left or right child!
		else if(rootNode->left == NULL)
		{
			// Save current root node in a temporary pointer
			BstNode* temp = rootNode;
			// Link rootNode to its only child
			rootNode = rootNode->right;
			// Reclaim memory from Node's data field
			free(temp->data);
			// Recalim memory from Node
			free(temp);
		}
		else if(rootNode->right == NULL)
		{
			// Save current root node in a temporary pointer
			BstNode* temp = rootNode;
			// Link rootNode to its only child
			rootNode = rootNode->left;
			// Reclaim memory from Node's data field
			free(temp->data);
			// Reclaim memory from Node
			free(temp);
		}

		// Case 3: Node has 2 children
		else
		{
			// Find minimum node in right subtree
			BstNode* temp = findMin(rootNode->right);
			// Replace node we want to delete with min value of right subtree
			strcpy(rootNode->data,temp->data);
			// Delete min value in right subtree
			rootNode->right = deleteNode(rootNode->right,temp->data,caseFlag);
		}
	}
	return rootNode;
}

/*
Iterates through a BST using post order traversal,
frees all memory allocated for the tree structure 
including the string data field. Returns a NULL
pointer
*/
BstNode* deleteTree(BstNode* rootNode, int caseFlag)
{
	// If the current node is a leaf...
	if(rootNode != NULL && (rootNode->left == NULL && rootNode->right == NULL))
	{	
		rootNode = deleteNode(rootNode,rootNode->data,caseFlag);
	}
	// If the current node is a root...
	else if(rootNode != NULL && (rootNode->left != NULL || rootNode->right != NULL))
	{	
		// Traverse left subtree
		rootNode->left = deleteTree(rootNode->left,caseFlag);
		// Traverse right subtree
		rootNode->right = deleteTree(rootNode->right,caseFlag);
		// Delete root node
		rootNode = deleteTree(rootNode,caseFlag);
	}
	return rootNode;
}
// --------------------------------- Helper functions ---------------------------------

/*
Compares two strings s1 and s2, CASE SENSITIVE!
returns 1 if s1 > s2, this means s1 comes after s2 in the dictionary
returns 0 if s1 == s2, s1 and s2 are in the same place in the dictionary
returns -1 if s1 < s1, this means s2 comes before s2 in the dictionary
*/
int stringCaseCompare(const char* s1,  const char* s2)
{
	// Convert both strings to char array
	int str1Size = strlen(s1) + 1;
	int str2Size = strlen(s2) + 1;
	char str1[str1Size];
	char str2[str2Size];
	strcpy(str1,s1);
	strcpy(str2,s2);

	// Compare each char in str1 with each char in str2, convert all chars to lowercase
	int i = 0;
	int j = 0;

	// Iterate through strings
	while(i < str1Size || j < str2Size)
	{
		// Get characters
		char ch1 = str1[i];
		char ch2 = str2[j];

		// Compare individual characters
		if(ch1 > ch2)
			return 1;
		else if(ch1 < ch2)
			return -1;

		// Advance counters
		if(i < str1Size)	
			++i;
		if(j < str2Size)
			++j;
	}
	/*
	Both strings has been completely iterated through
	and function has not returned yet, strings must
	be equal to each other.
	*/
	return 0;
}

/*
Compares two strings s1 and s2, CASE INSENSITIVE!
returns 1 if s1 > s2, this means s1 comes after s2 in the dictionary
returns 0 if s1 == s2, s1 and s2 are in the same place in the dictionary
returns -1 if s1 < s2, this means s1 comes before s2 in the dictionary
*/
int stringNoCaseCompare(const char* s1,  const char* s2)
{
	// Convert both strings to char array
	int str1Size = strlen(s1) + 1;
	int str2Size = strlen(s2) + 1;
	char str1[str1Size];
	char str2[str2Size];
	strcpy(str1,s1);
	strcpy(str2,s2);

	// Compare each char in str1 with each char in str2, convert all chars to lowercase
	int i = 0;
	int j = 0;

	// Iterate through strings
	while(i < str1Size || j < str2Size)
	{	
		// Convert to lowercase characters
		char ch1 = tolower(str1[i]);
		char ch2 = tolower(str2[j]);

		// Compare individual characters
		if(ch1 > ch2)
			return 1;
		else if(ch1 < ch2)
			return -1;

		// Advance counters
		if(i < str1Size)	
			++i;
		if(j < str2Size)
			++j;
	}
	/*
	Both strings has been completely iterated through
	and function has not returned yet, strings must
	be equal to each other.
	*/
	return 0;
}

// Prints a BST using inorder traversal to stdin
void printTree(BstNode* rootNode)
{	
	// If the current node is a leaf...
	if(rootNode != NULL && (rootNode->left == NULL && rootNode->right == NULL))
	{	
		// Print Node, count times and return
		int cntr = 0;
		while(cntr < rootNode->count)
		{
			printf("%s\n", rootNode->data);
			++cntr;
		}
		return;
	}
	// If the current node is a root...
	else if(rootNode != NULL && (rootNode->left != NULL || rootNode->right != NULL))
	{	
		// Traverse left subtree
		printTree(rootNode->left);
		// Print rootNode's data field
		int cntr = 0;
		while(cntr < rootNode->count)
		{
			printf("%s\n", rootNode->data);
			++cntr;
		}
		// Traverse right subtree 
		printTree(rootNode->right);
	}
}

// Prints a BST using inorder traversal to an output file
void fprintTree(BstNode* rootNode, FILE* oFile)
{	
	// If the current node is a leaf...
	if(rootNode != NULL && (rootNode->left == NULL && rootNode->right == NULL))
	{	
		int cntr = 0;
		// Print Node and return
		while(cntr < rootNode->count)
		{
			fprintf(oFile,"%s\n", rootNode->data);
			++cntr;
		}
		return;
	}
	// If the current node is a root...
	else if(rootNode != NULL && (rootNode->left != NULL || rootNode->right != NULL))
	{	
		// Traverse left subtree
		fprintTree(rootNode->left,oFile);
		int cntr = 0;
		// Print Node and return
		while(cntr < rootNode->count)
		{
			fprintf(oFile,"%s\n", rootNode->data);
			++cntr;
		}	
		// Traverse right subtree 
		fprintTree(rootNode->right,oFile);
	}
}

// Flushes stdin
void clearBuffer()
{
    int ch; 
    while ( (ch = getchar()) != EOF && ch != '\n' ) {} 
}

// --------------------------------- Main function ---------------------------------

int main(int argc, char **argv)
{	
	extern char *optarg;
	extern int optind;
	int c, err = 0;
	int cflag = 0, oflag = 0, qflag = 0;
	// Output file name
	char* output_file_name = "";
	// Holds default arguments
	char* defaultArg = "";
	static char usage[] = "usage: %s bstsort [-c] [-o output_file_name] [input_file_name]\n";

	while ((c = getopt(argc, argv, "co:")) != -1){
		switch (c) {
		case 'c':
			cflag = 1;
			break;
		case 'o':
			output_file_name = optarg;
			oflag = 1;
			break;
		default:
			break;
		}
	}	
	// Output help message
	if (stringCaseCompare(argv[argc-1],"?") == 0) {	
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}	
	else
	{	
		// Check for file
		FILE* inputFile = NULL;
		// Do not open ./bstsort
		if(stringCaseCompare(argv[argc-1],"./bstsort") != 0)
			inputFile = fopen(argv[argc-1],"r");
		// No file found, get all input from stdin
		if(inputFile == NULL)
		{	
			// Create a new BST
			BstNode* root = NULL;
			// Read until EOF
			char line[4096] = "";
			while(scanf("%[^\n]%*c",line) == 1)
			{	
				// Insert each line to the bst
				root = insert(root,line,cflag);
			}
			// Output to the specified source
			if(oflag == 1)
			{	
				// Create a file named output_file_name
				FILE* outputFile = fopen(output_file_name, "w");
				// Output to outputFile
				fprintTree(root,outputFile);
				// Close outputFile
				fclose(outputFile);
			}
			else
			{
				// Separator 
				printf("\nOrdered:\n");
				//Output to stdout
				printTree(root);
			}
			// Reclaim tree	
			root = deleteTree(root,cflag);	
		}
		// File opened succesfully, get all input from file
		else
		{	
			// Creates a new empty bst
			BstNode* root = NULL;
			// Holds each line in the inputFile
			char line[4096] = "";
			// Get each line in the inputFile
			while(fgets(line,sizeof(line),inputFile) != NULL)
			{
				// Insert lines into bst
				root = insert(root,line,cflag);
			}
			// Output sorted lines to an ouput file
			if(oflag == 1)
			{	
				// Create a new output file
				FILE* outputFile = fopen(output_file_name,"w");	
				// Print sorted lines to outputfile
				fprintTree(root, outputFile);
				fclose(outputFile);
			}
			// Output sorted lines to stdout
			else
			{	
				// Separator 
				printf("\nOrdered:\n");
				printTree(root);
			}

			// Reclaim tree
			root = deleteTree(root,cflag);
			// Close input file
			fclose(inputFile);
		}
	}
	return 0;
}
