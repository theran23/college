#include <stdio.h>
#include <stdlib.h>


int main () {


    int** req_m; //request matrix
    int** alloc_m; //allocation matrix

    int NofP; //process 개수
    int NofR; //resource type 개수
    int* Runits; //resource unit 개수
    int* Available; //resource별 available unit 개수
    int* p_state; //graph reduction 과정을 진행하면서 프로세스의 현재 상태를 의미. 
    //'b'(blocked), 'u'(unblocked), 'd'(done)의 세가지 상태로 나눈다. 

    //graph reduction 기법으로 deadlock detection
    //가정: multiple  resource types & multiple resource units
    

    //input.txt 에서 읽어오기 
    FILE* fp= fopen("input.txt","r");
    if(fp==NULL){
        printf("Wrong input file\n");
        return 0;
    }

    fscanf(fp,"%d",&NofP);
    req_m=(int**)malloc(sizeof(int*)*NofP);
    alloc_m=(int**)malloc(sizeof(int*)*NofP);
    p_state=(int*)malloc(sizeof(int)*NofP);

    fscanf(fp,"%d",&NofR);
    Runits=(int*)malloc(sizeof(int)*NofR);
    Available=(int*)malloc(sizeof(int)*NofR);
    for(int i=0;i<NofP;i++){
        req_m[i]=(int*)malloc(sizeof(int)*NofR);
        alloc_m[i]=(int*)malloc(sizeof(int*)*NofR);
    }
    for(int i=0;i<NofR;i++){
        fscanf(fp,"%d",&Runits[i]);
    }
    

    for(int i=0;i<NofP;i++){
        for(int j=0;j<NofR;j++){
            fscanf(fp,"%d",&alloc_m[i][j]);
        }
    }
    for(int i=0;i<NofP;i++){
        for(int j=0;j<NofR;j++){
            fscanf(fp,"%d",&req_m[i][j]);
        }
    }
    
    
    
   
    //초기화
    for(int i=0;i<NofR;i++){
        Available[i]=Runits[i];  
        p_state[i]=-1;    
    }
    
    while(1){
        //available한 resource unit 개수 구하기
        for(int i=0;i<NofR;i++){
            Available[i]=0;
        }
        for(int i=0;i<NofP;i++){
            for(int j=0;j<NofR;j++){
                Available[j]+=alloc_m[i][j];
            }
        }
        
        for(int i=0;i<NofR;i++){
            Available[i]=Runits[i]-Available[i];
        }
        /*
        printf("available: ");
        for(int i=0;i<NofR;i++){
            printf("%d ",Available[i]);
        }
        printf("\n");
        */
       
        
        // unblocked/blocked 구하기
        for(int i=0;i<NofP;i++){ 
            if(p_state[i]=='d'){
                continue;
            }
            for(int j=0;j<NofR;j++){
                if(req_m[i][j]>Available[j]){
                    p_state[i]='b';
                    break;
                }
                p_state[i]='u';
            }
        }
        
        /*
        for(int i=0;i<NofP;i++){
            printf("i:%d , %c  \n",i+1,p_state[i]);
        }
        printf("----------------------------------------\n");
        */

        //unblocked 인 process가 2개 이상이면 
        //하나만 ublocked인채로 남겨두고 나머지는 우선 blocked로 바꾼다.
        int u_count=0;//u_count는 u 상태인 프로세스 개수이다.
        int d_count=0;// d_count는 d 상태인 프로세스 개수이다.
        int unblocked_pid=-1;
        for(int i=0;i<NofP;i++){
            if(p_state[i]=='u'){
                u_count++;
                unblocked_pid=i;
            }
            else if(p_state[i]=='d'){
                d_count++;
            }
        }
        for(int i=0;i<NofP;i++){
            if((p_state[i]=='u')&&(i!=unblocked_pid)){
                p_state[i]='b';
            }
        }


        //deadelock detection
        if(d_count==NofP){
            printf("Non Deadlock state\n");
            break;
        }
        else if(u_count==0){
            printf("Deadlock state\n");
            printf("Deadlocked process(es):");
            for(int i=0;i<NofP;i++){
                if(p_state[i]=='b'){
                    printf("Process #%d. ",i+1);
                }
            }
            printf("\n");
            break;
        }
        


        //graph reducing
        for(int j=0;j<NofR;j++){
            req_m[unblocked_pid][j]=0;
            Available[j]=alloc_m[unblocked_pid][j];
            alloc_m[unblocked_pid][j]=0;
        }
        p_state[unblocked_pid]='d';
       
    }
    free(req_m);
    free(alloc_m);
    free(p_state);
    free(Runits);
    free(Available);
}