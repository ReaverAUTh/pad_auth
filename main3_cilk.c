#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mmio.h"
#include "mmio.c"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

int NUMBER_OF_WORKERS;
int N1, nz, *CSR_I, *J;


void __cilkrts_end_cilk(void);
int __cilkrts_set_param(const char* name, const char* value);
double time_spent(struct timespec start,struct timespec end);
void setWorkers(int n);
int binarySearch(int *arr, int l, int r, int x);
void get_durations();
void read_graph();

int matrix_vector_product(float *C,float* v, int* CSR_I, int* J, int N1,int*val)
{
    for (int i = 0; i<N1; i++){
        for (int j =CSR_I[i]; j<CSR_I[i+1]; j++)
            C[i] += val[j] *v[J[j]];
    }
    return 0;
}


double v4(){

    float *c3,*e;
    c3 = (float *) malloc(N1 * sizeof(float));
    e = (float *) malloc(N1 * sizeof(float));
    cilk_for(int i=0; i<N1; i++){
        c3[i]=0;
        e[i]=0.5;
    }
    int *val;
    val = (int *) malloc(2*nz * sizeof(int));
    cilk_for(int i=0; i<2*nz; i++)
        val[i]=0;

    struct timespec start;
    clock_gettime( CLOCK_MONOTONIC, &start);

/*
    ################################################
                    Algorithm start
    ################################################
*/

    cilk_for (int i = 0; i<N1; i++){
        for (int j =CSR_I[i]; j<CSR_I[i+1]; j++){
            for (int k=CSR_I[J[j]]; k<CSR_I[J[j]+1]; k++){
                if(CSR_I[i+1] - CSR_I[i] < 5){
                    // linearSearch
                    for(int l =CSR_I[i]; l<CSR_I[i+1]; l++){
                        if(J[l] == J[k]){
                            val[j]++;
                            break;
                        }
                    }
                }
                else{
                    //binarySearch
                    if(binarySearch(J,CSR_I[i],CSR_I[i+1]-1,J[k]) != -1){
                        val[j]++;
                        break;
                    }
                }
            }
        }
    }

    matrix_vector_product(c3,e,CSR_I,J,N1,val);
/*
    ################################################
                    Algorithm end
    ################################################
*/

    struct timespec end;
    clock_gettime( CLOCK_MONOTONIC, &end);
    double duration = time_spent(start, end);
   // printf("\n\n *****************************\n  v4_cilk_time = %f sec\n *****************************\n\n",duration);

    /*
    ################################
     Prints c3 for testing purposes
    ################################
    */

    /*
    for(int i=N1-1; i>(N1-101); i--)
        printf("%d ",(int)c3[i]);
    */
    return duration;
}


int main()
{
    /*
    printf("\nEnter number of nworkers: ");
    if(scanf("%d", &NUMBER_OF_WORKERS) != 1)
        printf("\n Error.");
    setWorkers(NUMBER_OF_WORKERS);
    */

    read_graph();

    get_durations();
    //v4();
    return 0;
}

void get_durations()
{
    // txt will contain threads and respective durations
    FILE *file = fopen("v4_cilk.txt","w");
    for(NUMBER_OF_WORKERS = 2; NUMBER_OF_WORKERS<130; NUMBER_OF_WORKERS+=3){
        setWorkers(NUMBER_OF_WORKERS);
        fprintf(file, "%d,%f\n", NUMBER_OF_WORKERS,  v4());
    }
}

double time_spent(struct timespec start,struct timespec end)
{
        struct timespec temp;
        if ((end.tv_nsec - start.tv_nsec) < 0)
        {
                temp.tv_sec = end.tv_sec - start.tv_sec - 1;
                temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
        }
        else
        {
                temp.tv_sec = end.tv_sec - start.tv_sec;
                temp.tv_nsec = end.tv_nsec - start.tv_nsec;
        }
        return (double)temp.tv_sec +(double)((double)temp.tv_nsec/(double)1000000000);

}

void setWorkers(int n){
    char buf[10];
    sprintf(buf, "%d", n);

    __cilkrts_end_cilk();
    if ( 0!=__cilkrts_set_param("nworkers", buf ) )
      printf( "Error setting workers\n" );
}

void read_graph(){

    FILE *f = fopen("graph.mtx","r");
    if(fscanf(f, "%d %d\n", &N1, &nz) != 2)
        printf("error");
    CSR_I = (int *) malloc( (N1+1)* sizeof(int));
    J = (int *) malloc(2*nz * sizeof(int));
    if( (2*nz) >= (N1+1) ){ // edges more than nodes (J bigger)
        for (int i=0; i<2*nz; i++){
            if(i<=N1){
                if (fscanf(f, "%d %d\n", &CSR_I[i], &J[i]) != 2)
                    printf("error");
            }
            else{
                if (fscanf(f, "%d\n",&J[i]) != 1)
                    printf("error");
            }
        }
    }
    else{ // CSR_I bigger
        for(int i=0; i<(N1+1); i++){
            if(i<=(2*nz-1)){
                if (fscanf(f, "%d %d\n", &CSR_I[i], &J[i]) != 2)
                    printf("error");
            }
            else{
                if (fscanf(f, "%d\n",&CSR_I[i]) != 1)
                    printf("error");
            }
        }
    }
    if (f !=stdin) fclose(f);
}

int binarySearch(int *arr, int l, int r, int x)
{
    if (r >= l) {
        int mid = l + (r - l) / 2;

        // If the element is present at the middle
        // itself
        if (arr[mid] == x)
            return mid;

        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (arr[mid] > x)
            return binarySearch(arr, l, mid - 1, x);

        // Else the element can only be present
        // in right subarray
        return binarySearch(arr, mid + 1, r, x);
    }

    // We reach here when element is not
    // present in array
    return -1;
}
