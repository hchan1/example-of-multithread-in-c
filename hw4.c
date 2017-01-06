#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#define Buffer_Size 65536
#define ASCII_SIZE 128
const unsigned int NUM_PRINTS = 5;
const unsigned int NUM_THREADS = 8;
static char BufferA[Buffer_Size];
static int CountA[ASCII_SIZE];
int EndofBuffer = 0;
pthread_mutex_t lock; // lock for EndofBuffer
pthread_mutex_t lock2;// lock for CountA
void cleanBuffer(int *buf) {
    int i;

    for (i = 0; i < ASCII_SIZE; i++) {
        buf[i] = 0;
    }
}

void *thread_func(void *data) {
int CountLocal[ASCII_SIZE];
int start, end,i;
	cleanBuffer(CountLocal);
    pthread_mutex_lock(&lock);
   start = EndofBuffer;
   end= EndofBuffer +Buffer_Size/NUM_THREADS;
   EndofBuffer = end; // renew the num
    pthread_mutex_unlock(&lock);
for( start;start < end;start++)
{
i = (int)BufferA[start];
CountLocal[i]++;
}
pthread_mutex_lock(&lock2);
for (i = 0; i < ASCII_SIZE; i++) {
        CountA[i]= CountA[i]+CountLocal[i];
    }
pthread_mutex_unlock(&lock2);
}

int main(int argc, char **argv) {
    int k = 0;
    int bytes_read;
    FILE *outfile;
    pthread_t threads[NUM_THREADS];

    cleanBuffer(CountA);
    if (argc <= 2) {
        printf("no argument \n");
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("Unable to open file %s\n", argv[1]);
        exit(1);
    }
    //printf(" file %s already exist \n", argv[2]);
    //exit(1);

    /*   if (access(argv[2], F_OK) != -1) {
           // file exist
           printf(" file %s already exist \n", argv[2]);
           exit(1);
       }
       else {*/
    outfile = fopen(argv[2], "w");
    //  }
	printf(" with %i NUM_THREADS\n",NUM_THREADS);
    do {
        bytes_read = read(fd, BufferA, Buffer_Size);
        //printf("%zu", bytes_read);


        if (bytes_read <= 0) { // if no byte is read
            if (bytes_read < 0) {
                printf(" having a error when reading file %s \n", argv[1]);

                exit(1);
            }
        } else {
            unsigned long i;
	EndofBuffer = 0; // restart it
            for (i = 0; i < NUM_THREADS; i++) {
               
                pthread_create(&threads[i], NULL, thread_func, (void *) (i + 1));
            }
            unsigned int j;
            for (j = 0; j < NUM_THREADS; j++) {
                pthread_join(threads[j], NULL);
            }
        }
    } while (bytes_read != 0);
    for (k = 0; k < ASCII_SIZE; k++) {
        fprintf(outfile, " %i occurrences of Ox%x\n", CountA[k], k);
    }



    return 0;
}
