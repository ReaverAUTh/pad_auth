#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mmio.h"
#include "mmio.c"
#include <omp.h>

void merge(int *arr, int l, int m, int r,int *mirror);
void mergeSort(int *arr, int l, int r,int *mirror);
void mergeSort_J(int *arr, int l, int r);
void merge_J(int *arr, int l, int m, int r);

/*
    make_graph reads a COO format of MatrixMarket
    and for each edge (i,j), it adds an (j,i) edge.

    Then it sorts the pointer I, mirrored by J,
    so they will have (2 * non-zeros) edges.


*/

int main(int argc, char *argv[])
{
    FILE *f = fopen(argv[1],"r");
    MM_typecode matcode;
    int M, N1, nz, ret_code, *I, *J;
    if (mm_read_banner(f, &matcode) != 0)
        printf("Could not process Matrix Market banner.\n");
    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N1, &nz)) !=0)
        exit(1);
    I = (int *) malloc(2*nz * sizeof(int));
    J = (int *) malloc(2*nz * sizeof(int));


    for (int i=0; i<nz; i++){
        //(i -> j)
        if(fscanf(f, "%d %d\n", &J[i], &I[i]) != 2)
            printf("error");
        I[i]--;
        J[i]--;

        //(j -> i)
        I[i+nz]=J[i];
        J[i+nz]=I[i];
    }
    if (f !=stdin) fclose(f);

    mergeSort(I,0,2*nz-1,J);

    // COO format -> CSR format

    int *CSR_I;
    CSR_I = (int *) malloc((N1+1) * sizeof(int));
    # pragma omp parallel for
    for(int i=0; i<(N1+1); i++)
        CSR_I[i] = 0;
    # pragma omp parallel for
    for (int i = 0; i <2*nz; i++)
        CSR_I[I[i] + 1]++;
    for (int i = 0; i <N1; i++)
        CSR_I[i + 1] += CSR_I[i];



    // sort the adjacent nodes of the i-node
    # pragma omp parallel for
    for (int i = 0; i<N1; i++)
        mergeSort_J(J,CSR_I[i],CSR_I[i+1]-1);




    FILE *f2 = fopen("graph.mtx","w");
    fprintf(f2, "%d %d\n", N1, nz);

    if( (2*nz) >= (N1+1) ){ // edges more than nodes (J bigger)
        for (int i=0; i<2*nz; i++){
            if(i<=N1)
                fprintf(f2, "%d %d\n", CSR_I[i], J[i]);
            else
                fprintf(f2, "%d\n",J[i]);
        }
    }
    else{ // CSR_I bigger
        for(int i=0; i<(N1+1); i++){
            if(i<=(2*nz-1))
                fprintf(f2, "%d %d\n", CSR_I[i], J[i]);
            else
                fprintf(f2, "%d\n", CSR_I[i]);
        }
    }


    return 0;
}


// methods to sort I, mirrored by J

void merge(int *arr, int l, int m, int r,int *mirror){
    int k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L,*R,*L_mirror,*R_mirror;
    L=(int *) malloc(n1 * sizeof(int));
    R =(int *) malloc(n2 * sizeof(int));
    L_mirror=(int *) malloc(n1 * sizeof(int));
    R_mirror =(int *) malloc(n2 * sizeof(int));


    for (int i = 0; i < n1; i++){
        L[i] = arr[l + i];
        L_mirror[i] = mirror[l+i];
    }

    for (int j = 0; j < n2; j++){
        R[j] = arr[m + 1 + j];
        R_mirror[j] = mirror[m+1+j];
    }

    int i = 0;
    int j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            mirror[k] = L_mirror[i];
            i++;
        }
        else {
            arr[k] = R[j];
            mirror[k] = R_mirror[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        mirror[k] = L_mirror[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        mirror[k] = R_mirror[j];
        j++;
        k++;
    }
}


void mergeSort(int *arr, int l, int r,int *mirror){
    if (l < r) {
        int m = l + (r - l) / 2;

        #pragma omp task
        mergeSort(arr, l, m, mirror);

        mergeSort(arr, m + 1, r,mirror);

        #pragma omp taskwait
        merge(arr, l, m, r,mirror);
    }
}

void merge_J(int *arr, int l, int m, int r){
    int k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L,*R;
    L=(int *) malloc(n1 * sizeof(int));
    R =(int *) malloc(n2 * sizeof(int));



    for (int i = 0; i < n1; i++){
        L[i] = arr[l + i];
    }

    for (int j = 0; j < n2; j++){
        R[j] = arr[m + 1 + j];
    }

    int i = 0;
    int j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}


void mergeSort_J(int *arr, int l, int r){
    if (l < r) {
        int m = l + (r - l) / 2;

        #pragma omp task
        mergeSort_J(arr, l, m);

        mergeSort_J(arr, m + 1, r);

        #pragma omp taskwait
        merge_J(arr, l, m, r);
    }
}
