#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mmio.h"
#include "mmio.c"
int N1, nz, *CSR_I, *J;
double time_spent(struct timespec start,struct timespec end);
int binarySearch(int *arr, int l, int r, int x);
void read_graph();

int matrix_vector_product(float *C,float* v, int* CSR_I, int* J, int N1,int*val)
{
    for (int i = 0; i<N1; i++){
        for (int j =CSR_I[i]; j<CSR_I[i+1]; j++)
            C[i] += val[j] *v[J[j]];
    }
    return 0;
}


int v4(){

    float *c3,*e;
    c3 = (float *) malloc(N1 * sizeof(float));
    e = (float *) malloc(N1 * sizeof(float));
    for(int i=0; i<N1; i++){
        c3[i]=0;
        e[i]=0.5;
    }
    int *val; // result of A@(AxA) is val
    val = (int *) malloc(2*nz * sizeof(int));
    for(int i=0; i<2*nz; i++)
        val[i]=0;

    struct timespec start;
    clock_gettime( CLOCK_MONOTONIC, &start);
/*
    ################################################
                    Algorithm start
    ################################################
*/

    for (int i = 0; i<N1; i++){                       // all rows
        for (int j =CSR_I[i]; j<CSR_I[i+1]; j++){     // non-zeros of row i
            for (int k=CSR_I[J[j]]; k<CSR_I[J[j]+1]; k++){
                if(CSR_I[i+1] - CSR_I[i] < 5){
                    // linearSearch
                    for(int l =CSR_I[i]; l<CSR_I[i+1]; l++){
                        if(J[l] == J[k]){
                            val[j]++;
                            break;  // once the node is found, stop looking
                        }
                    }
                }
                else{
                    //binarySearch
                    if(binarySearch(J,CSR_I[i],CSR_I[i+1]-1,J[k]) != -1){
                        val[j]++;
                        break;     // once the node is found, stop looking
                    }
                }
            }
        }
    }


    matrix_vector_product(c3,e,CSR_I,J,N1,val); // c3 = val * e , where val = A@(AA) and e = 1/2
/*
    ################################################
                    Algorithm end
    ################################################
*/
    struct timespec end;
    clock_gettime( CLOCK_MONOTONIC, &end);
    printf("\n\n ************************\n  v4_time = %f sec\n ************************\n\n", time_spent(start, end));

    /*
    ################################
     Prints c3 for testing purposes
    ################################
    */
    /*
    for(int i=N1-1; i>(N1-101); i--)
    printf("%d ",(int)c3[i]);
    */
    return 0;
}



int main()
{
    // We read the "graph.mtx" file that contains CSR_I and J
    read_graph();
    // And we run once the v4 algorithm
    v4();

    return 0;
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
