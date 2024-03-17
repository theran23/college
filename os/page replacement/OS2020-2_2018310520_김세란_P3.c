#include "functions.h"
#include <stdio.h>
#include <stdlib.h>


int main () {
   
    int N; // process가 갖는 page 개수, 최대 100개
    int M; // 할당 page frame 개수, 최대 20개, WS 기법에서는 비사용 
    int W; // window size, 최대 100개, WS 기법에서만 사용
    int K; // page reference string 길이 , 최대 100000
    //int page[100]; // 0 ~ 99 까지의 페이지 번호를 가질 수 있음, 참조된 횟수 기록하는 해쉬 배열


    FILE* fp= fopen("inputt.txt","r");
    fscanf(fp,"%d",&N);
    fscanf(fp,"%d",&M);
    Frame ** f = (Frame**) malloc(sizeof(Frame*)*M);
    for(int i=0;i<M;i++){
        f[i]=(Frame*) malloc(sizeof(Frame));
        frame_init(f[i]);
    }
    fscanf(fp,"%d",&W);
    fscanf(fp,"%d",&K);
    int * RS=(int*)malloc(sizeof(int)*K);
    for(int i=0;i<K;i++){
        fscanf(fp,"%d",&RS[i]);
    }

    printInput(N,M,W,K,RS);
    int rt=getInput(N,M,W,K,RS); 
    // 예외처리, 비정상적인 input을 받으면 에러 종류를 출력하고 프로그램 종료
    if(rt==-1){
    return 0;
    }
    

    
    doMIN(N,M,K,f,RS);
    for(int i=0;i<M;i++){
        frame_init(f[i]);
    }
    doFIFO(N,M,K,f,RS);
    for(int i=0;i<M;i++){
        frame_init(f[i]);
    }
    doLRU(N,M,K,f,RS);
    for(int i=0;i<M;i++){
        frame_init(f[i]);
    }
    doLFU(N,M,K,f,RS);
    for(int i=0;i<M;i++){
        frame_init(f[i]);
    }
    doCLOCK(N,M,K,f,RS);
    for(int i=0;i<M;i++){
        frame_init(f[i]);
    }
    doWS(N,W,K,RS);
    

    free(f);
    free(RS);
    fclose(fp);
}