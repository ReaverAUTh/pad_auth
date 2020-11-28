#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mmio.h"
#include "mmio.c"

double time_spent(struct timespec start,struct timespec end);
int binarySearch(int *arr, int l, int r, int x);

int N = 5;

int v1(int c3[N], int A[N][N])
{
    for (int i = 0; i<N; i++){
        for (int j = 0; j<N; j++){
            for (int k = 0; k<N; k++){
                if ((A[i][j] == 1) && (A[j][k] == 1) && (A[k][i] == 1)){
                    c3[i]++;
                    c3[j]++;
                    c3[k]++;
                }
            }
        }
    }
    return 0;
}

int v2(int c3[N], int A[N][N])
{
    for (int i = 0; i<N; i++){
        for (int j = 0; j<i; j++){
            for (int k = 0; k<j; k++){ // i<j<k
                if ((A[i][j] == 1) && (A[j][k] == 1) && (A[k][i] == 1)){
                    c3[i]++;
                    c3[j]++;
                    c3[k]++;
                }
            }
        }
    }
    return 0;
}

int v3(int* CSR_I, int* J, int nz, int N1)
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

    for (int i = 0; i<N1; i++){
        for (int j =CSR_I[i]; j<CSR_I[i+1]; j++){                      // edge1 = ( i -> j )
            for (int k = j+1; k<CSR_I[i+1]; k++){                      // edge2 = ( k -> i )
                if(CSR_I[J[j]+1] - CSR_I[J[j]] < 5){
                    // linearSearch
                    for(int l =CSR_I[J[j]]; l<CSR_I[J[j]+1]; l++){
                        if(J[l] == J[k]){                              // edge3 = ( j -> k )
                            c3[i]++;                   // we have edge1, edge2 and edge3 so we have a triangle
                            c3[J[j]]++;
                            c3[J[l]]++;
                            break;       // once the node is found, stop looking
                        }
                    }
                }
                else{
                    //binarySearch
                    if(binarySearch(J,CSR_I[J[j]],CSR_I[J[j]+1]-1,J[k]) != -1){
                        c3[i]++;
                        c3[J[j]]++;
                        c3[J[k]]++;
                        break;           // once the node is found, stop looking
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
    printf("\n\n ************************\n  v3_time = %f sec\n ************************\n\n", time_spent(start, end));
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





int main(int argc, char *argv[])
{

    //    old code
    /*
    int c3[N];
    for(int i=0; i<N; i++)
        c3[i] = 0;
    int A[N][N];
    for (int i = 0; i<N; i++){
        for (int j = 0; j<N; j++){
            A[i][j] = 0;
        }
    }
    A[0][1]=1;A[1][0]=1;
    A[1][3]=1;A[3][1]=1;
    A[1][2]=1;A[2][1]=1;
    A[1][4]=1;A[4][1]=1;
    A[3][4]=1;A[4][3]=1;
    A[0][4]=1;A[4][0]=1;
    v1(c3,A);
    v2(c3,A);
    */

    // Extract info from argv[1] file

    FILE *f = fopen(argv[1],"r");
    MM_typecode matcode;
    int ret_code, M, N1, nz, *I, *J;
    if (mm_read_banner(f, &matcode) != 0)
        printf("Could not process Matrix Market banner.\n");
    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N1, &nz)) !=0)
        exit(1);
    I = (int *) malloc(nz * sizeof(int));
    J = (int *) malloc(nz * sizeof(int));
    for (int i=0; i<nz; i++){
        if(fscanf(f, "%d %d\n", &J[i], &I[i]) != 2)
            printf("error");
        I[i]--;
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


    v3(CSR_I,J,nz,N1);
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
