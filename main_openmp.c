#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mmio.h"
#include "mmio.c"

double time_spent(struct timespec start,struct timespec end);
void get_durations(int *CSR_I,int *J,int nz,int N1);
int binarySearch(int *arr, int l, int r, int x);
int NUMBER_OF_THREADS;

double v3(int* CSR_I, int* J, int nz, int N1)
{
    int *c3;
    c3 = (int *) malloc(N1 * sizeof(int));
    for(int i=0; i<N1; i++)
        c3[i]=0;

    struct timespec start;
    clock_gettime( CLOCK_MONOTONIC, &start);

/*
    ################################################
                    Algorithm start
    ################################################
*/

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i<N1; i++){ //i=0,...,N1-1
        for (int j =CSR_I[i]; j<CSR_I[i+1]; j++){ // edges till then to edges of current row
            for (int k = j+1; k<CSR_I[i+1]; k++){
                if(CSR_I[J[j]+1] - CSR_I[J[j]] < 5){
                    // linearSearch
                    for(int l =CSR_I[J[j]]; l<CSR_I[J[j]+1]; l++){
                        if(J[l] == J[k]){
                            #pragma omp atomic
                            c3[i]++;
                            #pragma omp atomic
                            c3[J[j]]++;
                            #pragma omp atomic
                            c3[J[l]]++;
                            break;
                        }
                    }
                }
                else{
                    //binarySearch
                    if(binarySearch(J,CSR_I[J[j]],CSR_I[J[j]+1]-1,J[k]) != -1){
                        #pragma omp atomic
                        c3[i]++;
                        #pragma omp atomic
                        c3[J[j]]++;
                        #pragma omp atomic
                        c3[J[k]]++;
                        break;
                    }

                }
            }
        }
    }

/*
    ################################################
                    Algorithm end
    ################################################
*/
    struct timespec end;
    clock_gettime( CLOCK_MONOTONIC, &end);
    double duration = time_spent(start, end);
    //printf("\n\n *******************************\n  v3_openmp_time = %f sec\n *******************************\n\n", duration);
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





int main(int argc, char *argv[])
{
    /*
    printf("\nEnter number of threads: ");
    if(scanf("%d", &NUMBER_OF_THREADS) != 1)
        printf("\n Error.");
    omp_set_num_threads(NUMBER_OF_THREADS);
    */

    // Extract info from argv[1] file

    FILE *f = fopen(argv[1],"r");
    MM_typecode matcode;
    int M, N1, nz, *I, *J, ret_code;
    if (mm_read_banner(f, &matcode) != 0)
        printf("Could not process Matrix Market banner.\n");
    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N1, &nz)) !=0) // M -> rows , N1 -> cols , nz -> non-zeros
        exit(1);
    I = (int *) malloc(nz * sizeof(int));
    J = (int *) malloc(nz * sizeof(int));
    for (int i=0; i<nz; i++){
        if(fscanf(f, "%d %d\n", &J[i], &I[i]) != 2)
            printf("error");
        I[i]--;  /* adjust from 1-based to 0-based */
        J[i]--;
    }
    if (f !=stdin) fclose(f);

    // COO format -> CSR format

    int *CSR_I;
    CSR_I = (int *) malloc((N1+1) * sizeof(int));
    for(int i=0; i<(N1+1); i++)
        CSR_I[i] = 0;
    for (int i = 0; i <nz; i++)
        CSR_I[I[i] + 1]++;
    for (int i = 0; i <N1; i++)
        CSR_I[i + 1] += CSR_I[i];


    get_durations(CSR_I,J,nz,N1);
    //v3(CSR_I,J,nz,N1);
    return 0;
}

void get_durations(int *CSR_I,int *J,int nz,int N1)
{
    // txt will contain threads and respective durations
    FILE *file = fopen("v3_openmp.txt","w");
    for(NUMBER_OF_THREADS = 2; NUMBER_OF_THREADS<130; NUMBER_OF_THREADS+=3){
        omp_set_num_threads(NUMBER_OF_THREADS);
        fprintf(file, "%d,%f\n", NUMBER_OF_THREADS,  v3(CSR_I,J,nz,N1));
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
