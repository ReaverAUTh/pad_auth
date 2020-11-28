# -*- coding: utf-8 -*-


def validity(n):
    c3 =[0 for _ in range(n)]
    e =[1/2 for _ in range(n)]
    A = [[0]*n for _ in range(n)]
    A[0][1]=1
    A[1][0]=1
    A[0][4]=1
    A[4][0]=1
    A[1][4]=1
    A[4][1]=1
    A[4][3]=1
    A[3][4]=1
    A[1][2]=1
    A[2][1]=1
    A[1][3]=1
    A[3][1]=1
    A_A = [[0]*n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            for k in range(n):
                A_A[i][j]  += A[i][k] * A[k][j]
    for i in range(n):
        for j in range(n):
            A[i][j] = A[i][j]*A_A[i][j]
    # 5x5 * 5x1 = 5x1
    for i in range(n):
        for j in range(n):
            c3[i] += A[i][j] * e[j]
    print(c3)
    
validity(5)