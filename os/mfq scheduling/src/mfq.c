#include <stdio.h>
#include "queue.h"
#include <stdlib.h>


int main () {
    int*TIME=(int*)malloc(sizeof(int));
    *TIME=0;

    //ready queue 0,1,2,3 선언    
    Queue* q0 = (Queue*) malloc(sizeof(Queue));
    QueueInit(q0,0);
    Queue* q1=(Queue*) malloc(sizeof(Queue));;
    QueueInit(q1,1);
    Queue* q2=(Queue*) malloc(sizeof(Queue));;
    QueueInit(q2,2);
    Queue* q3=(Queue*) malloc(sizeof(Queue));;
    QueueInit(q3,3);
    
    Queue*q=(Queue*) malloc(sizeof(Queue));
    QueueInit(q3,-1);

    //input
    FILE* fp= fopen("input.txt","r");
    
    int rt=getProcesses(fp,q0,q1,q2,q3); // 예외처리, 비정상적인 input을 받으면 에러 종류를 출력하고 프로그램 종료
        if(rt==-1){
        return 0;
    }

    int timeslice;
    
    Process * runningP;

    int cs[11];
    for(int i=0;i<12;i++){
        cs[i]=0;
    }
    
    int go;
    
    //CPU실행
    printf("=============================Gantt Chart=============================\n");
    printf("|CLOCK| PID |\n");
    while(1){
        //printProcesses(q0,q1,q2,q3);

        //go=0;
        go=schedule(q0,q1,q2,q3,TIME);
        //printf("from q%d:\n",go);


        if(go==0){
            cs[0]=1;
            timeslice=2;
            runningP=q0->front;
            if(runningP->BTlist[runningP->idx]>timeslice){ 
                //PREMPTION
                printf("| %-4d| %3d |",*TIME,runningP->pid);
                for(int k=0;k<timeslice;k++){
                    printf("--");
                }
                //CPU BURST
                *TIME+=timeslice;
                runningP->BTlist[runningP->idx]-=timeslice;
                //SCHEDULING
                Dequeue(q0);
                runningP->AT=*TIME;
                Enqueue(q1,runningP);
            }
            else {
                //ASLEEP OR TERMINATE
                printf("| %-4d| %3d |",*TIME,runningP->pid);
                for(int k=0;k<runningP->BTlist[runningP->idx];k++){
                    printf("--"); 
                }
                //CPU BURST
                *TIME+=runningP->BTlist[runningP->idx];
                runningP->BTlist[runningP->idx]=0;
                runningP->idx++;
                if((runningP->cycle*2-1)<=runningP->idx){
                    cs[1]=1;
                    //PROCESS TERMINATE
                    runningP->TT+=*TIME;
                    runningP->WT=runningP->TT-runningP->BT;
                    runningP->AT=runningP->pid;
                    Dequeue(q0);
                    Enqueue(q,runningP);
                }
                else{
                    cs[2]=1;
                    //IO BURST
                    Dequeue(q0);
                    runningP->AT=*TIME+runningP->BTlist[runningP->idx];
                    runningP->BTlist[runningP->idx]=0;
                    runningP->idx++;
                    //SHCHEDULING
                    Enqueue(q0,runningP);
                }
            }
        }
        else if(go==1){
            timeslice=4;
            runningP=q1->front;
            if(runningP->BTlist[runningP->idx]>timeslice){ 
                cs[3]=1;
                //PREMPTION
                printf("| %-4d| %3d |",*TIME,runningP->pid);
                for(int k=0;k<timeslice;k++){
                    printf("--");
                }
                //CPU BURST
                *TIME+=timeslice;
                runningP->BTlist[runningP->idx]-=timeslice;
                //SCHEDULING
                Dequeue(q1);
                runningP->AT=*TIME;
                Enqueue(q2,runningP);
            }
            else { 
                //ASLEEP OR TERMINATE
                //CPU BURST
                printf("| %-4d| %3d |",*TIME,runningP->pid);
                for(int k=0;k<runningP->BTlist[runningP->idx];k++){
                    printf("--");
                }
                *TIME+=runningP->BTlist[runningP->idx];
                runningP->BTlist[runningP->idx]=0;
                runningP->idx++;
                if((runningP->cycle*2-1)<=runningP->idx){
                    cs[4]=1;
                    //PROCESS TERMINATE
                    runningP->TT+=*TIME;
                    runningP->WT=runningP->TT-runningP->BT;
                    runningP->AT=runningP->pid;
                    Dequeue(q1);
                    Enqueue(q,runningP);
                }
                else{
                    //IO BURST
                    cs[5]=1;
                    Dequeue(q1);
                    runningP->AT=*TIME+runningP->BTlist[runningP->idx];
                    runningP->BTlist[runningP->idx]=0;
                    runningP->idx++;
                    //SCHEDULING
                    Enqueue(q0,runningP);
                }
            }
        }
        else if(go==2){
            timeslice=8;
            runningP=q2->front;
            if(runningP->BTlist[runningP->idx]>timeslice){ 
                cs[6]=1;
                //PREMPTION
                printf("| %-4d| %3d |",*TIME,runningP->pid);
                for(int k=0;k<timeslice;k++){
                    printf("--");
                }
                //CPU BURST
                *TIME+=timeslice;
                runningP->BTlist[runningP->idx]-=timeslice;
                //SCHEDULING
                Dequeue(q2);
                runningP->AT=*TIME;
                Enqueue(q3,runningP);
            }
            else { 
                //ASLEEP OR TERMINATE
                printf("| %-4d| %3d |",*TIME,runningP->pid);
                for(int k=0;k<runningP->BTlist[runningP->idx];k++){
                    printf("--");
                }
                //CPU BURST
                *TIME+=runningP->BTlist[runningP->idx];
                runningP->BTlist[runningP->idx]=0;
                runningP->idx++;
                if((runningP->cycle*2-1)<=runningP->idx){
                    cs[7]=1;
                    //PROCESS TERMINATE
                    *TIME+=runningP->BTlist[runningP->idx];
                    runningP->TT+=*TIME;
                    runningP->WT=runningP->TT-runningP->BT;
                    runningP->AT=runningP->pid;
                    Dequeue(q2);
                    Enqueue(q,runningP);
                }
                else{
                    cs[8]=1;
                    //IO BURST
                    Dequeue(q2);
                    runningP->AT=*TIME+runningP->BTlist[runningP->idx];
                    runningP->BTlist[runningP->idx]=0;
                    runningP->idx++;
                    //SCHEDULING
                    Enqueue(q1,runningP);
                }
            }
        }
        else if(go==3){
            runningP=q3->front;
            //NO PREEMPTION
            //ASLEEP OR TERMINATE
            printf("| %-4d| %3d |",*TIME,runningP->pid);
            for(int k=0;k<runningP->BTlist[runningP->idx];k++){
                printf("--");
            }
            //CPU BURST
            *TIME+=runningP->BTlist[runningP->idx];
            runningP->BTlist[runningP->idx]=0;
            runningP->idx++;
            if((runningP->cycle*2-1)<=runningP->idx){
                    cs[9]=1;
                    //PROCESS TERMINATE
                    *TIME+=runningP->BTlist[runningP->idx];
                    runningP->TT+=*TIME;
                    runningP->WT=runningP->TT-runningP->BT;
                    runningP->AT=runningP->pid;
                    Dequeue(q3);
                    Enqueue(q,runningP);
            }
            else{
                cs[10]=1;
                //IO BURST
                Dequeue(q3);
                runningP->AT=*TIME+runningP->BTlist[runningP->idx];
                runningP->BTlist[runningP->idx]=0;
                runningP->idx++;
                Enqueue(q2,runningP);
            }
        } 
        else if(go==4){
            break;
        } 
        printf(">|\n");
    }
    printf("| %-4d| DONE|\n",*TIME);
    
    double TTmean=0;
    double WTmean=0;

    printf("\n==============================THE RESULT===============================\n");
    printf("                  PID            TT            WT       \n");
    printf("-----------------------------------------------------------------------\n");
    
    Process* p=q->front;
    for(int i=0;i<q->pnum;i++){
        printf("                 %4d          %4d          %4d       \n",p->pid,p->TT,p->WT);
        TTmean+=p->TT;
        WTmean+=p->WT;
        p=p->next;
    }
    printf("\n");
    printf("                                                 TT mean: %.2f\n",TTmean/q->pnum);
    printf("                                                 WT mean: %.2f\n",WTmean/q->pnum);

    printf("=======================================================================\n");
   /*
     printf("case number:\n");
    for(int i=0;i<11;i++){
        printf("%d  ",cs[i]);
    }
    printf("\n");
   */
    return 0;
}


