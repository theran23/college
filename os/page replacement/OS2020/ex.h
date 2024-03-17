#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdio.h>

#include <stdlib.h>


typedef struct _process{
    int pid;
    int AT;
    int cycle;
    int TT;
    int BT;
    int WT;
    int * BTlist;
    int idx;
    struct _process * next;
}Process;

typedef struct _queue{
    int qid;
    int pnum; // 현재 push된 process 갯수
    Process * front;
}LQueue;

typedef LQueue Queue;

int hash[10];



Process* CreateProcess(Queue *pq,int pid, int AT,int cycle){
    //printf("im in");
    Process*newProcess=(Process*)malloc(sizeof(Process));
    newProcess->next=NULL;
    newProcess->pid=pid;
    newProcess->AT=AT;
    newProcess->TT=(AT*-1);
    newProcess->cycle=cycle;
    newProcess->idx=0;
    if(pq->pnum==0){
        pq->front=newProcess;
        newProcess->next=NULL;
    }
    else{
        Process *n=pq->front;
        int k=0;
        for(int i=0; i< pq->pnum;i++){
            if(n->AT<=newProcess->AT){
                n=n->next;
                k++;
            }
            else{
                break;
            }
        }
        if(k==0){
            newProcess->next=pq->front;
            pq->front=newProcess;
        }
        else{
            n=pq->front;
            for(int i=0;i<k-1;i++){
                n=n->next;
            }
            newProcess->next=n->next;
            n->next=newProcess;
        }
    }
    pq->pnum++;
    return newProcess;
}


int getProcesses(FILE*fp,Queue*q0,Queue*q1,Queue*q2,Queue*q3){
    for(int i=0;i< 10;i++){
        hash[i]=0;
    }
    int NofP;
    int pid;
    int AT;
    int Qid;
    int cycle;


    
    fscanf(fp,"%d",&NofP);
    if(NofP>10){
        printf("프로세스는 %d개 이하로 받을 수 있습니다. \n",10);
        return -1;
    }
    Process*newProcess;
    int c;
    for(int i=0;i<NofP;i++){
        c=fscanf(fp,"%d",&pid);
        if (c==EOF){
            printf("pid에 인자가 부족합니다.\n");
            return -1;
        }
        else if(hash[pid]==0){
            hash[pid]++;
        }
        else{
            printf("중복되는 pid입니다. \n");
            return -1;
        }
        c=fscanf(fp,"%d",&AT);
        if (c==EOF){
            printf("AT에 인자가 부족합니다.\n");
            return -1;
        }
        c=fscanf(fp,"%d",&Qid);
        if (c==EOF){
            printf("qid에 인자가 부족합니다.\n");
            return -1;
        }
        else if((Qid<0)||(Qid>3)){
            printf("Qid는 0,1,2,3 만 가능합니다. \n");
            return -1;
        }
        //printf("qid:%d\n",Qid);
       c=fscanf(fp,"%d",&cycle);
        if(c==EOF){
            printf("cycle에 인자가 부족합니다.\n");
            return -1;
        }
        else if((Qid<0)&&(Qid>3)){
            printf("cycle은 자연수여야합니다. \n");
            return -1;
        }
        switch(Qid){
            case 0:
                newProcess=CreateProcess(q0,pid,AT,cycle);
                break;
            case 1:
                newProcess=CreateProcess(q1,pid,AT,cycle);
                break;
            case 2:
                newProcess=CreateProcess(q2,pid,AT,cycle);
                break;
            case 3:
                newProcess=CreateProcess(q3,pid,AT,cycle);
                break;
        }
        newProcess->BTlist=(int*)malloc(sizeof(int)*(cycle*2-1));
        
        int T;
        newProcess->BT=0;
        //printf("cycle:%d\n",cycle);
        for(int j=0;j<(cycle*2-1);j++){
            c=fscanf(fp,"%d",&T);
            //printf("%d:T\n",j,T);
            if(c==EOF){
                printf("BTlist에 인자가 부족합니다.\n");
                return -1;
            }
            newProcess->BTlist[j]=T;
            newProcess->BT+=T;
        }
        if(i==NofP-1){
            c=fscanf(fp,"%d",&T);
            if(c!=EOF){
                printf("입력을 초과하였습니다.\n");
                return -1;
            }
        }
    }
    //printf("done\n");
    return 0;

}

void printProcesses(Queue *q0,Queue *q1,Queue *q2,Queue *q3){
    printf("queue0:\n");
    Process * p=q0->front;
    for(int i=0;i<q0->pnum;i++){
        printf("pid:%d (%2d)  TT:%d  ",p->pid,p->AT,p->TT);
        printf("BT:");
        for(int j=0;j<(p->cycle*2-1);j++){
            printf("%d",p->BTlist[j]);
            if(j!=(p->cycle*2-2)){
                printf(",");
            }
            else{
                printf(" ");
            }
        }
        puts("");
        p=p->next;
    }
    puts("");
    printf("queue1:   \n");

    p=q1->front;
    for(int i=0;i<q1->pnum;i++){
        printf("pid:%d (%2d)  TT:%d  ",p->pid,p->AT,p->TT);
        printf("BT:");
        for(int j=0;j<(p->cycle*2-1);j++){
            printf("%d",p->BTlist[j]);
            if(j!=(p->cycle*2-2)){
                printf(",");
            }
            else{
                printf(" ");
            }
        }
        puts("");
        p=p->next;
    }
    puts("");
    printf("queue2:\n");
    p=q2->front;
    for(int i=0;i<q2->pnum;i++){
        printf("pid:%d (%2d)  TT:%d  ",p->pid,p->AT,p->TT);
        printf("BT:");
        for(int j=0;j<(p->cycle*2-1);j++){
            printf("%d",p->BTlist[j]);
            if(j!=(p->cycle*2-2)){
                printf(",");
            }
            else{
                printf(" ");
            }
        }
        puts("");
        p=p->next;
    }
    puts("");
    printf("queue3:\n");
    p=q3->front;
    for(int i=0;i<q3->pnum;i++){
        printf("pid:%d (%2d)  TT:%d  ",p->pid,p->AT,p->TT);
        printf("BT:");
        for(int j=0;j<(p->cycle*2-1);j++){
            printf("%d",p->BTlist[j]);
            if(j!=(p->cycle*2-2)){
                printf(",");
            }
            else{
                printf(" ");
            }
        }
        puts("");
        p=p->next;
    }
    printf("------------------------------------------------------------------------------------------------------\n");
}



void Enqueue(Queue*pq,Process*p){
    if(pq->pnum==0){
        pq->front=p;
        p->next=NULL;
    }
    else{
        Process*n=pq->front;
        int k=0;
        for(int i=0;i<pq->pnum;i++){
            if(n->AT<=p->AT){
                n=n->next;
                k++;
            }
            else{
                break;
            }
        }
        if(k==0){
            p->next=pq->front;
            pq->front=p;
        }
        else{
            n=pq->front;
            for(int i=0;i<k-1;i++){
                n=n->next;
            }
            p->next=n->next;
            n->next=p;
        }
    }
    pq->pnum++;
}

void QueueInit(Queue*pq,int qid){
    pq->qid=qid;
    pq->pnum=0;
    pq->front = NULL;
}
void Dequeue(Queue* pq){
    Process* dqProcess;
    dqProcess=pq->front;
    pq->front=pq->front->next;
    pq->pnum--;
}
int QPeek(Queue*pq){
    return pq->front->pid;
}

int schedule(Queue*q0,Queue*q1,Queue*q2,Queue*q3,int*TIME){
    int end=0;
    int top=-1;
    int wait=-1;
    int min=2147483645;
    int go;
    int ATlist[4];
    for(int i=0;i<4;i++){
        ATlist[i]=-1;
    }
    if(q0->pnum!=0){ATlist[0]=q0->front->AT;}
    if(q1->pnum!=0){ATlist[1]=q1->front->AT;}
    if(q2->pnum!=0){ATlist[2]=q2->front->AT;}
    if(q3->pnum!=0){ATlist[3]=q3->front->AT;}

    for(int i=3;i>=0;i--){
        if(ATlist[i]==-1){
            end++;
        }
        else{
            if(ATlist[i]<=*TIME){
                top=i;
            }
            else{
                if(min>ATlist[i]){
                    min=ATlist[i];
                    wait=i;
                }
            }
        }
    }
    if(end==4){
        go=4;
    }
    else if(top>=0){
        go=top;
    }
    else{
        go=wait;
        *TIME=min;
    }
    return go;
}


#endif