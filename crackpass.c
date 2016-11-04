#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"
#include <omp.h>

void MD5_HashString(char *src, char *dest){
  MD5_CTX ctx;
  unsigned char digest[16];
  int i;
  MD5_Init(&ctx);
  MD5_Update(&ctx, src, strlen(src));
  MD5_Final(digest,&ctx);
  for (i = 0; i < 16; i++)  {
    sprintf(&dest[i*2],"%02x", digest[i]);
  }
}

// Trim string buffer
void trim(char *str){
  int len=strlen(str);
  if(str[len-1] == '\n'){
    str[len-1] = '\0';
  }
}

// Return number of lines in a file (lines must be newline "\n" terminated)
int file_nlines(FILE *f){
  size_t nread = 0;
  char *buffer;
  int nlines = 0;

  while (getline(&buffer, &nread, f) != -1) {
    nlines++;
  }
  return nlines;
}


int main (int argc, char *argv[]){
  int i;
  char *dictionary, *hash, *output;
  int BUF_SIZE = 255;
  int verbose;

  char hash_md5[32];

  dictionary = malloc(1*sizeof(char));
  hash = malloc(1*sizeof(char));
  output = malloc(1*sizeof(char));
  verbose = 0;

  // Extract command line arguments:
  // -d  -- dictionary to read from (lines separated with a newline "\n" character)
  // -h  -- hash list to read from (lines separated with a newline "\n" character)
  // -o  -- name of file to output results
  // -v  -- show threads processing
  if (argc > 1){
    for (i = 1; i < argc; i++){
      if (strcmp(argv[i],"-d")==0 && i+1<argc){
        dictionary = realloc(dictionary, strlen(argv[i+1]));
        strcpy(dictionary, argv[i+1]);
      }else if (strcmp(argv[i],"-h")==0 && i+1<argc){
        hash = realloc(hash, strlen(argv[i+1]));
        strcpy(hash, argv[i+1]);
      }else if (strcmp(argv[i],"-o")==0 && i+1<argc){
        output = realloc(output, strlen(argv[i+1]));
        strcpy(output, argv[i+1]);
      }else if (strcmp(argv[i],"-v")==0){
        verbose = 1;
      }
    }
  }

  printf("\n");
  printf("Dictionary: %s\n", dictionary);
  printf("Hash: %s\n", hash);
  // printf("Output: %s\n", output);
  printf("\n");


  FILE *dictionary_file;
  dictionary_file = fopen(dictionary,"r");
  if(dictionary_file==NULL){
    fprintf(stderr, "Can't open file \"%s\"!\n", dictionary);
      exit(1);
  }

  printf("\nAnalizing file...\n");
  int number_of_words = file_nlines(dictionary_file);

  char word[BUF_SIZE], word_cpy[BUF_SIZE], word_md5[32];

  if(fseek(dictionary_file, 0, SEEK_SET)==-1){
    exit(1);
  }

  int j, found=0;
  double execution_time_start = omp_get_wtime();
  // Assign one word for each thread
  printf("\nSearching...\n");
  #pragma omp parallel for private (word, word_cpy, word_md5, hash_md5) shared(found) schedule(dynamic)
  for(j=0;j<number_of_words;j++){
    if(fgets(word,BUF_SIZE,dictionary_file)!= NULL){
      if(found){
        fclose(dictionary_file);
        exit(0);
      }
      strcpy(word_cpy,word);
      trim(word_cpy);

      if(verbose){
        printf("Thread: %d - Trying: %s\n", omp_get_thread_num(), word_cpy);
      }

      MD5_HashString(word_cpy,word_md5);

      hash[32]='\0';
      if(strcmp(hash, word_md5)==0){
        if(verbose){
          printf("Thread: %d - Found: %s\n", omp_get_thread_num(), word_cpy);
        }else{
          printf("Password Found: %s\n\n", word_cpy);
          printf("Search Execution time: %f seconds\n\n", omp_get_wtime() - execution_time_start);
        }
        found=1;
      }

    }
  }

  fclose(dictionary_file);

  if(!found){
    printf("Password not found.\n\n");
    printf("Search Execution time: %f seconds\n\n", omp_get_wtime() - execution_time_start);
  }

  exit(0);
}