#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: pattern_gen <num>\n");
    exit(1);
  }
 
  int N = atoi(argv[1]);
  int i, j, cell;
  time_t t;
 
  srand((unsigned int) time(&t));
 
  printf("%d\n", N);
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++)
      printf("%d ", rand() % 2);
    printf("\n");
  }
}