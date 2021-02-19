// COMP2521 20T2 Assignment 1
// Dict.c ... implementation of Dictionary ADT
// Written by Joe Vu Nguyen z5120839 4/7/20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Dict.h"
#include "WFreq.h"
///////////////////////////////////////////////////////
/* DICTIONARY DECLARATIONS */
// An AVL tree is used to implement the Dictionaries

// pointer to node of dictionary
typedef struct _DictNode *Link;

// A node of the dictionary
typedef struct  _DictNode {
   WFreq  data; //data containing frequency and string
   int    height; //current height of the node
   Link   left; //left node   
   Link   right; //right node
} DictNode;

// representative of the dictionary
struct _DictRep {
   Link tree; // pointer to node
   Link last; //pointer to last inserted node
};

////////////////////////////////////////////////////////

/* FUNCTIONS */ 

void printDict(Link root);
Link RecursiveInsertDict(Dict d, char *w, Link root);
int updateHeight(Link root);
int max(int a, int b);
Link reBalance(Link root, int dir);
Link DictSearch(Link root, char *w);
char *duplicate(char *word);
Link rotateLeft(Link root);
Link rotateRight(Link root);
int heightNode(Link root);

////////////////////////////////////////////////////////

/* HEAP CODE */
// A Min Heap is used to implement FindTopN

typedef struct _HeapRep {
   int currSize;
   int maxSize;
   WFreq *heap; //an array pointed to first element
} HeapRep;

typedef HeapRep *HeapRepPtr;

// Formulas used to compute relative positioning in array
#define heapParent(i) (i - 1) / 2
#define heapLeftChild(i) (2*i) + 1
#define heapRightChild(i) (2*i) + 2

// Creates a new heap
HeapRepPtr newHeap(int maxSize) {
   HeapRepPtr h = malloc(sizeof(HeapRep));
   h -> currSize = 0;
   h -> maxSize = maxSize;
   h -> heap = malloc(sizeof(WFreq) * maxSize);
   return h;
}

// Compares two elements in the array
// Returns 1 if b has larger freq or alphanumerically higher
// Else 0
int compareWfreq(WFreq a, WFreq b) {
   if (a.freq == b.freq) {
      int result = strcmp(a.word, b.word);
      return result == 0 ? 0 : result > 0;
   }
   return a.freq < b.freq;
}
// swaps two elements in an array
void swapHeapElements(HeapRepPtr h, int a, int b) {
   WFreq temp = h -> heap[a];
   h -> heap[a] = h -> heap[b];
   h -> heap[b] = temp;
}

// inserts an element into the heap
void insertHeap(HeapRepPtr h, WFreq input) {
   if (h -> currSize >= h -> maxSize) {
      fprintf(stderr, "heap size exceeded\n");
      exit(1);
   }

   int pos = h -> currSize;
   h -> heap[pos] = input;
   h -> currSize++;

   while (compareWfreq(h -> heap[pos], h -> heap[heapParent(pos)])) {
      swapHeapElements(h, pos, heapParent(pos));
      pos = heapParent(pos);
   }
}

// rearranges the heap to maintain the correct min heap structure
void minHeapify(HeapRepPtr h, int pos) {

   int minpos = pos;
   if (heapLeftChild(pos) < h -> currSize && 
      compareWfreq(h -> heap[heapLeftChild(pos)], h -> heap[minpos])) {
      minpos = heapLeftChild(pos);
   }
   if (heapRightChild(pos) < h -> currSize && 
      compareWfreq(h -> heap[heapRightChild(pos)], h -> heap[minpos])) {
      minpos = heapRightChild(pos);
   }
   if (minpos != pos) {
      swapHeapElements(h, minpos, pos);
      minHeapify(h, minpos);
   }

}

// adjusts min
WFreq *removeMin(HeapRepPtr h) {
   if (h -> currSize == 0) {
      return NULL;
   }
   WFreq *min = malloc(sizeof(HeapRep));
   *min = h -> heap[0];
   h -> heap[0] = h -> heap[h -> currSize - 1];
   h -> currSize -= 1;
   minHeapify(h, 0);
   return min;
}

// builds the min heap
void buildMinHeap(HeapRepPtr h, Link root) {
   if (root == NULL) {
      return;
   }

   if (h -> currSize < h -> maxSize || compareWfreq(h -> heap[0], root -> data)) {
      if (h -> currSize == h -> maxSize)
         removeMin(h);
      insertHeap(h, root -> data);
   }
   buildMinHeap(h, root -> left);
   buildMinHeap(h, root -> right);
}

///////////////////////////////////////////////////////////////////////////

/* DICTIONARY CODE */
// Stopwords and Input file are both stored in binary trees...
// ..later to be convert in an array using a min heap 

// creates a new node in the dictionary and
// sets all values to NULL or 0
// returns a pointer to the node
Link newNode(char *word) {
   Link new = malloc(sizeof(*new));
   new -> left = NULL;
   new -> right = NULL;
   new -> height = 0;
   new -> data.word = word;
   new -> data.freq = 1;
   return new;
}


// creates a new empty Dictionary
// returns a pointer to the dictionary rep
Dict newDict() {
   // TODO
   Dict new = malloc(sizeof(*new));
   new -> tree = NULL;
   new -> last = NULL;
   return new;
} 

// inputs: dictionary, input word
// INSERTS WORD INTO DICTIONARY
// returns pointer to the (word,freq) pair for that word
WFreq *DictInsert(Dict d, char *w) {
   d -> tree = RecursiveInsertDict(d, w, d -> tree);
   return &(d -> last -> data);
}

// finds a Word in Dictionary
// return pointer to (word,freq) pair, if found
// otherwise return NULL
WFreq *DictFind(Dict d, char *w) {
   Link node = DictSearch(d -> tree, w);
   return &(node -> data);
}

// find top N frequently occurring words in Dict
// input: Dictionary, array of WFreqs, size of array
// returns: #WFreqs in array, modified array
int findTopN(Dict d, WFreq *wfs, int n) {
   HeapRepPtr h = newHeap(n);
   buildMinHeap(h, d -> tree);
   int size = h -> currSize;

   for (int i = size - 1; i >= 0; i--) {
      wfs[i] = *removeMin(h);
   }
   return size;
}

// shows the dictionary for the purpose of debugging
void showDict(Dict d) {
   printf("SHOW ROOT = %s\n", d-> tree -> data.word);
   printDict(d -> tree);
   return;
}

// helper function for showDict
void printDict(Link root) {
   if (root == NULL) {
      return;
   }

   printDict(root -> left);
   printf("%s\n", root -> data.word);
   printDict(root -> right);
}

// Helper function for DictInsert
Link RecursiveInsertDict(Dict d, char *w, Link root) {
   if (root == NULL) {     
      d -> last = newNode(w);
      return d -> last;
   }
   // traverses the tree based on alphabetical order
   int alphaorder = strcmp(root -> data.word, w);

   // updates the frequency of the word is found
   int dir = 0;
   // continues left if word is below alphabetically
   if (alphaorder > 0){
      root -> left = RecursiveInsertDict(d, w, root -> left);
      dir = strcmp(root -> left -> data.word, w);
   }
   // countinues right if word is in front alphabetically
   else if (alphaorder < 0) {
      root -> right = RecursiveInsertDict(d, w, root -> right);
      dir = strcmp(root -> right -> data.word, w);
   } else {
      root -> data.freq++;
      d -> last = root;
      return root;
   }
   return reBalance(root, dir); //rebalances and updates the root
}

// reBalances AVL tree
// used during insertions to maintain balance
Link reBalance(Link root, int dir) {
   int Lheight = heightNode(root->left);
   int Rheight = heightNode(root->right);
   root -> height = heightNode(root);

   // rebalance right heavy branches
   if (Rheight - Lheight > 1){
      if (dir > 0) {
         root -> right = rotateRight(root -> right);
      }
      root = rotateLeft(root);
   }

   //rebalance left heavy branches
   else if (Lheight - Rheight > 1) {
      if (dir < 0) {
         root -> left = rotateLeft(root -> left);
      }
      root = rotateRight(root);
   }
   return root;
}

// inputs: pointer to a node, word searched, a double pointer to data
// SEARCHES THE DICTIONARY FOR THE INPUT WORD
// AND UPDATES THE FREQUENCY IF FOUND, OR CREATES A NEW NODE
// returns the root of the tree
Link DictSearch(Link root, char *w) {  
   // creates a new node if the word is not found
   if (root == NULL){     
      return NULL;
   }

   // traverses the tree based on alphabetical order
   int alphaorder = strcmp(root -> data.word, w);

   // continues left if word is below alphabetically
   if (alphaorder > 0){
      return DictSearch(root -> left, w);
   }
   // countinues right if word is in front alphabetically
   else if (alphaorder < 0){
      return DictSearch(root -> right, w);
   }
   return root;
}

char *duplicate(char *word) {
   char *copy = malloc((strlen(word) + 1)*sizeof(*copy));
   strcpy(copy, word);

   return copy;

}
// rotates left
// used in rebalancing AVL tree during insertions
Link rotateLeft(Link root) {
   if (root == NULL || root -> right == NULL) {
      return root;
   }

   Link temp = root -> right;
   root -> right = temp -> left;
   temp -> left = root;
   updateHeight(temp);
   return temp;
}

//rotates right
// used in rebalancing AVL tree during insertions
Link rotateRight(Link root) {
   if (root == NULL || root -> left == NULL) {
      return root;
   }
   Link temp = root -> left;
   root -> left = temp -> right;
   temp -> right = root;
   updateHeight(temp);
   return temp;
}

// finds the maximum to two integers
// used in updateHeight function below to update the height
int max(int a, int b) {
   return a > b ? a : b;
}

// updates the height during inserts and rebalancing
int updateHeight(Link root) {
   if (root == NULL)
      return -1;
   root -> height = 1 + max(updateHeight(root->left), updateHeight(root->right));
   return root->height;
}

//gives the height of the node, in order to rebalance and maintain AVL tree
int heightNode(Link root) {
   int Rheight = -1;
   int Lheight = -1;
   if (root == NULL) {
      return -1;
   }
   if (root -> left != NULL){
      Lheight = root -> left -> height;
   }
   if (root -> right != NULL){
      Rheight = root -> right -> height;
   }
   return max(Lheight, Rheight) + 1;
}


