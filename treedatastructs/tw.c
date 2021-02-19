// COMP2521 20T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "Dict.h"
#include "WFreq.h"
#include "stemmer.h"

#define STOPWORDS "stopwords"
#define MAXLINE 1000
#define MAXWORD 100

#define STARTING "*** START OF"
#define ENDING   "*** END OF"

#define isWordChar(c) (isalnum(c) || (c) == '\'' || (c) == '-')

/************* FUNCTION DECLARATIONS ****************/
int buildStopwordDict(Dict stopwords, char *fileName);
FILE *openWordsFile(char *fileName);
int buildWordFreqDict(Dict wfreqs, FILE *in, Dict stopwords);
void convertToLowercase(char *word);
void removeInvalidChars(char *buffer);

/************* MAIN FUNCTION *********************/
int main( int argc, char *argv[])
{
   
   FILE  *in;         // currently open file
   Dict   stopwords;  // dictionary of stopwords
   Dict   wfreqs = newDict();     // dictionary of words from book
   WFreq *results;    // array of top N (word,freq) pairs
                      // (dynamically allocated)

   char *fileName;    // name of file containing book text
   int   nWords;      // number of top frequency words to show

   // processes command-line args to obtain Nwords and File
   switch (argc) {
   case 2:
      nWords = 10;
      fileName = argv[1];
      break;
   case 3:
      nWords = atoi(argv[1]);
      if (nWords < 10) nWords = 10;
      fileName = argv[2];
      break;
   default:
      fprintf(stderr,"Usage: %s [Nwords] File\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   // builds dictionary of stopwords
   stopwords = newDict();
   if (buildStopwordDict(stopwords, STOPWORDS) == EXIT_FAILURE) {
      return EXIT_FAILURE;
   }
   
   //printf("SHOWDICT\n");


   // opens File for reading and scans up to start of text
   in = openWordsFile(fileName);
   if (in == NULL) {
      return EXIT_FAILURE;
   }
   // scan File reading words and accumualting counts

   buildWordFreqDict(wfreqs, in, stopwords);
   // TODO
   //showDict(wfreqs);
   // compute and display the top N words
   results = malloc(sizeof(WFreq) * nWords);
   int size = findTopN(wfreqs, results, nWords);
   for (int i = 0; i < size; i++) {
      printf("%7d %s\n", results[i].freq, results[i].word);
   }
   // done
   return EXIT_SUCCESS;
}

/************* FUNCTIONS *****************************/

// inputs: dictionary of stopwords, pointer to the file of stopwords
// BUILDS THE DICTIONARY OF STOPWORDS
// returns either exit failiure or exit success
int buildStopwordDict(Dict stopwords, char *fileName)
{
   // opens the stop word file
   FILE *fd = fopen(fileName, "r");
   // returns an error message if there is no file
   if (fd == NULL) {
      fprintf(stderr, "Can't open stopwords\n");
      return EXIT_FAILURE;
   }
   
   // reads words in file into a buffer string
   char buffer[MAXLINE];
   while (fgets(buffer, MAXLINE, fd) != NULL) {
      char word[MAXLINE];
      strncpy(word, buffer, strlen(buffer) - 1);
      word[strlen(buffer) - 1] = '\0'; //sets the final character to a NULL
      DictInsert(stopwords, strdup(word)); //inserts into dictionary
      //printf("%s\n", word);
   }
   return EXIT_SUCCESS;
}

// inputs: word file, buffer line
// OPENS THE WORD FILE
// returns pointer to the word file, or null
FILE *openWordsFile(char *fileName) {
   FILE *fd = fopen(fileName, "r");
   if (fd == NULL) {
      fprintf(stderr, "Can't open %s\n",fileName);
      return NULL;
   }

   char buffer[MAXLINE];
   while (fgets(buffer, MAXLINE, fd) != NULL) {  
      if (strncmp(buffer, STARTING, strlen(STARTING)) == 0) 
      return fd;
   }
   fprintf(stderr, "Not a Project Gutenberg book\n");
   return NULL;
}

// inputs: dictionary of word frequencies, pointer to word file
// , and string containing the title of the word file
// BUILDS THE WORD FREQUENCY DICTIONARY
// returns exit success
int buildWordFreqDict(Dict wfreqs, FILE *in, Dict stopwords) {
   char buffer[MAXLINE];


   // takes words in until the endline is recieved
   while (fgets(buffer, MAXLINE, in) != NULL &&
      strncmp(buffer, ENDING, strlen(ENDING)) != 0) {
 
      // removing newline and carriage return
      buffer[strlen(buffer) - 2] = '\0';
      removeInvalidChars(buffer);
      char word[MAXWORD];
      int offset = 0;
      int charsRead = 0;
      while (sscanf(buffer + offset, "%s%n", word, &charsRead) == 1) {
         convertToLowercase(word);
         if (DictFind(stopwords, word) == NULL && strlen(word) > 1) {
            int lastChar = stem(word, 0, strlen(word) - 1);
            word[lastChar + 1] = '\0';

            DictInsert(wfreqs, strdup(word)); 
            //printf("%s\n", word);
         }
         offset += charsRead;
      } 
   }
   return EXIT_SUCCESS;
}

// removes invalid characters
void removeInvalidChars(char *buffer) {
   for (int i = 0; i < strlen(buffer); i++) {
      if (!isWordChar(buffer[i])) {
         buffer[i] = ' ';
      }
   }
}

void convertToLowercase(char *word) {
   //sets all words to lowercase
   for (int i = 0; word[i] != '\0'; i++) {
      if (word[i] <= 'Z' && word[i] >= 'A') {
         word[i] = (word[i] - 'A') + 'a';
      }
   }
}
