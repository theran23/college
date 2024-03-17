#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__
#include <stdio.h>
#include <stdlib.h>
#define INT_MAX 2147483647


typedef struct _frame{
    int page_number; // 로드된 페이지 번호
    int load_time; // 페이지가 해당 프레임에 로드된 시각
    int reference_time; // 페이지가 프레임에 있는 동안 참조된 가장 최근 시각
    int reference_frequecy; // 페이지가 "현재 시각 이전까지 모두" 참조된 횟수
    int reference_bit; // clock 기법에서 필요한 reference bit
}Frame;

void frame_init (Frame* f){
    f->page_number=-1; // 페이지가 로드되지 않음
    f->load_time=-1;
    f->reference_time=-1;
    f->reference_frequecy=-1;
    f->reference_bit=-1;
}
void print_before (int M){
    for(int j=0;j<M;j++){
        printf("-----------");
    }
    printf("\n");
    printf("FRAME NUM:");
    for(int j=0;j<M;j++){
        printf(" %6d ",j);
    }
    printf("\n");
    for(int j=0;j<M;j++){
        printf("-----------");
    }
    printf("\n");
}

void print_before_ws(int W){
    for(int j=0;j<W;j++){
        printf("------------------------");
    }
    printf("\n");
    printf("FRAME NUM:");
    for(int j=0;j<W;j++){
        if(j==W/2){
            printf("MEMORY STATE");
        }
        printf("          ");
    }
    printf("\n");
    for(int j=0;j<W;j++){
        printf("------------------------");
    }
    printf("\n");
}

void print_before_clock (int M){
    for(int j=0;j<M;j++){
        printf("-------------");
    }
    printf("\n");
    printf("FRAME NUM:");
    for(int j=0;j<M;j++){
        printf("   %6d ",j);
    }
    printf("\n");
    for(int j=0;j<M;j++){
        printf("-------------");
    }
    printf("\n");
}

void print_state_clock(int T, int M, int PF, Frame**f, int victim,int pointer){
    if(T>M){
    printf("T=%6d: ",T);
        for(int j=0;j<M;j++){
            if(j==pointer){
                printf(" ->");
            }
            else{
                printf("   ");
            }
            if(j==PF){
                printf("(%6d/%d)",f[j]->page_number,f[j]->reference_bit);
            }
            else if(f[j]->page_number==-1){
                printf("     --/- ");
            }
            else{
                printf(" %6d/%d ",f[j]->page_number,f[j]->reference_bit);
            }
        }
        printf("   ");
        if(PF==-1){
            printf("NO PAGE FAULT\n");
        }
        else{
            if(victim==-1){
                printf("  PAGE FAULT: displaced: --, loaded: %d AT [#%d]\n",f[PF]->page_number,PF);
            }
            else{
                printf("  PAGE FAULT: displaced :%d, loaded: %d AT [#%d]\n",victim,f[PF]->page_number,PF);
            }
        }
    }
}

void print_state(int T, int M, int PF, Frame**f, int victim){
    printf("T=%6d: ",T);
        for(int j=0;j<M;j++){
            if(j==PF){
                printf("(%6d)",f[j]->page_number);
            }
            else if(f[j]->page_number==-1){
                printf("     -- ");
            }
            else{
                printf(" %6d ",f[j]->page_number);
            }
        }
        printf("   ");
        if(PF==-1){
            printf("NO PAGE FAULT\n");
        }
        else{
            if(victim==-1){
                printf("  PAGE FAULT: (-- -> %d) AT [#%d]\n",f[PF]->page_number,PF);
            }
            else{
                printf("  PAGE FAULT: (%d -> %d) AT [#%d]\n",victim,f[PF]->page_number,PF);
            }
        }
}

void print_state_ws(int in, int out){
    printf("OUT: ");
        if(out==-1){
            printf("--, ");
        }
        else{
            printf("%2d, ",out);
        }
        printf("IN: ");
        if(in==-1){
            printf("--");
        }
        else{
            printf("%2d",in);
        }
        printf("\n");
}

void doWS(int N,int W,int K,int* RS){//M이 프레임 개수, N이 페이지 개수, K는 스트링 길이
    int T=1; // 현재 시각[초], 1초에서 시작, string 하나 지날 때 마다 1 초씩 증가
    int NofE=0; // 페이지를 가진 프레임 개수
    int PF=-1; // Page Fault 발생한 페이지 번호, -1 이면 없다는 의미
    int PAGEFAULT=0; //page fault 횟수
    int out;
    int in;
    int NofFA=0;
    int SUM=0;

    int mem[W+1];
    for(int i=0;i<W+1;i++){
        mem[i]=-1;
    }

    printf("WS\n");
    print_before_ws(W);
    for(int i=0;i<K;i++){
        out=-1;
        in =-1;
        

        //삭제 : 1) 현재 페이지 번호 아닌 것. 2) 앞의 W 스트링 만큼에 존재하지 않는 것
        for(int j=0;j<W+1;j++){
            if(mem[j]==-1){
                continue;
            }
            else{
                int d=0;
                for(int k=0;k<=W;k++){
                    if(mem[j]==RS[i-k]){
                        d=1;
                        break;
                    }
                }
                if(d==0){
                    out=mem[j];
                    mem[j]=-1;
                    NofFA--;
                }
            }
        }
        //추가
        int x=0;
        for(int j=0;j<W+1;j++){
            if(mem[j]==RS[i]){
                x=1;
                break;
            }
        }
        if(x==0){//PAGE FAULT, 추가
            PAGEFAULT++;
            int j;

            for( j=0;j<W+1;j++){
                if(mem[j]==-1){
                    break;
                }
            }
            in=RS[i];
            mem[j]=RS[i];
            NofFA++;
        }
        
        
        for(int j=0;j<W+1;j++){
            if(mem[j]==-1){
                printf("      -- ");
            }
            else if(mem[j]==RS[i]){
                printf("  (%6d)",mem[j]);
            }
            else{
                printf("   %6d",mem[j]);
            }
        }
        printf("   ");
        if(x==0){
            printf("   PAGE FAULT  ");
        }
        else{
            printf("NO PAGE FAULT  ");
        }
        printf("IN:  ");
        if(in==-1){
            printf("--  ");
        }
        else{
            printf("%2d  ",in);
        }
         printf("OUT:  ");
        if(out==-1){
            printf("--");
        }
        else{
            printf("%2d",out);
        }
        printf("\n");
        
        T++;
        SUM+=NofFA;
    }
    
    printf("PAGEFAULT:%5d MEAN NUM OF FRAMES ALLOCATED:%.2f\n\n",PAGEFAULT,(double)SUM/(double)T);


}

void doCLOCK(int N,int M,int K,Frame ** f,int* RS){//M이 프레임 개수, N이 페이지 개수, K는 스트링 길이
    int T=1; // 현재 시각[초], 1초에서 시작, string 하나 지날 때 마다 1 초씩 증가
    int NofE=0; // 페이지를 가진 프레임 개수
    int PF=-1; // Page Fault 발생한 페이지 번호, -1 이면 없다는 의미
    int victim; //victim이되는 프레임의 교체 전 페이지 번호
    int PAGEFAULT=0; //page fault 횟수
    int pointer=0;
    printf("CLOCK\n");
    print_before_clock(M);
    for(int i=0;i<K;i++){
        for(int j=0;j<M;j++){ // Page Fault 여부 체크
            if(f[j]->page_number==RS[i]){ 
                PF=-1;
                //CLOCK: reference bit 1
                f[j]->reference_bit=1;
                break;
            }
            PF=0;
        }
        if(PF!=-1){ // Page Fault 가 존재
            PAGEFAULT++;
            if(NofE<M){ // 빈자리가 존재할 때
                PF=NofE; // 빈자리 중 가장 작은 번호의 페이지에 Page Fault 발생
                f[PF]->page_number=RS[i];
                //CLOCK: reference bit 1
                f[PF]->reference_bit=1;
                NofE++;
                victim=-1;
            }
            else{ // 빈자리가 존재하지 않음, CLOCK기법에 맞게 페이지 교체
                //교체할 프레임 찾기 (CLOCK: reference bit이 0인 것)
                while(1){
                    if(f[pointer]->reference_bit==0){
                        PF=pointer;
                        pointer=(pointer+1)%M;
                        break;
                    }
                    f[pointer]->reference_bit=0;
                    pointer=(pointer+1)%M;
                }
                //페이지 교체
                victim=f[PF]->page_number;
                f[PF]->page_number=RS[i];
                //CLOCK: reference bit 1
                f[PF]->reference_bit=1;
            }
        }
        print_state_clock(T,M,PF,f,victim,pointer);
        T++;
    }
    printf("PAGEFAULT:%5d\n\n",PAGEFAULT);
}

void doLFU(int N,int M,int K,Frame ** f,int* RS){//M이 프레임 개수, N이 페이지 개수, K는 스트링 길이
    int T=1; // 현재 시각[초], 1초에서 시작, string 하나 지날 때 마다 1 초씩 증가
    int NofE=0; // 페이지를 가진 프레임 개수
    int PF=-1; // Page Fault 발생한 페이지 번호, -1 이면 없다는 의미
    int victim; //victim이되는 프레임의 교체 전 페이지 번호
    int PAGEFAULT=0;
    int reference[100]; //page 번호 별 참조된 횟수
    int r_time[100]; // page 번호 별 참조된 가장 최근 시각
    for(int i=0;i<100;i++){
        reference[i]=0;
        r_time[i]=0;
    }

    printf("LFU\n");
    print_before (M);
    for(int i=0;i<K;i++){
        for(int j=0;j<M;j++){ // Page Fault 여부 체크
            if(f[j]->page_number==RS[i]){ 
                PF=-1;
                break;
            }
            PF=0;
        }
        if(PF!=-1){ // Page Fault 가 존재
            PAGEFAULT++;
            if(NofE<M){ // 빈자리가 존재할 때
                PF=NofE; // 빈자리 중 가장 작은 번호의 페이지에 Page Fault 발생
                f[PF]->page_number=RS[i];
                NofE++;
                victim=-1;
            }
            else{ // 빈자리가 존재하지 않음, LUR기법에 맞게 페이지 교체
                //교체할 프레임 찾기 (LFU: 각 프레임 별 페이지의 참조 횟수 가장 작은 것,
                // 횟수가 같은 것 중에는 참조 시각이 가장 이른 것)
                int min = reference[f[0]->page_number];
                int min_fid=0;
                int min_time=r_time[f[0]->page_number];
                
                for(int j=1;j<M;j++){
                    if(min>reference[f[j]->page_number]){
                        min=reference[f[j]->page_number];
                        min_time=r_time[f[j]->page_number];
                        min_fid=j;
                    }
                    else if (min==reference[f[j]->page_number]){
                        if (min_time > r_time[f[j]->page_number]){
                            min_time=r_time[f[j]->page_number];
                            min_fid=j;
                        }
                    }
                }
                //페이지 교체
                PF=min_fid;
                victim=f[PF]->page_number;
                f[PF]->page_number=RS[i];
            }
        }
        //LFU: 프레임에 페이지 참조 횟수 및 시각 증가
        reference[RS[i]]++;
        r_time[RS[i]]=T;
        print_state(T,M,PF,f,victim);
        T++;
    }
    printf("PAGEFAULT:%5d\n\n",PAGEFAULT);
}

void doLRU(int N,int M,int K,Frame ** f,int* RS){//M이 프레임 개수, N이 페이지 개수, K는 스트링 길이
    int T=1; // 현재 시각[초], 1초에서 시작, string 하나 지날 때 마다 1 초씩 증가
    int NofE=0; // 페이지를 가진 프레임 개수
    int PF=-1; // Page Fault 발생한 페이지 번호, -1 이면 없다는 의미
    int victim; //victim이되는 프레임의 교체 전 페이지 번호
    int PAGEFAULT=0;
    printf("LRU\n");
    print_before (M);
    for(int i=0;i<K;i++){
        for(int j=0;j<M;j++){ // Page Fault 여부 체크
            if(f[j]->page_number==RS[i]){ 
                //LRU: 프레임에 페이지 새로 로드 로딩/참조 시각 업데이트
                f[j]->reference_time=T;
                PF=-1;
                break;
            }
            PF=0;
        }
        if(PF!=-1){ // Page Fault 가 존재
            PAGEFAULT++;
            if(NofE<M){ // 빈자리가 존재할 때
                PF=NofE; // 빈자리 중 가장 작은 번호의 페이지에 Page Fault 발생
                f[PF]->page_number=RS[i];
                NofE++;
                victim=-1;
            }
            else{ // 빈자리가 존재하지 않음, LUR기법에 맞게 페이지 교체
                //교체할 프레임 찾기 (LRU: 각 프레임 별 페이지의 참조 시각 가장 작은 것)
                int min = f[0]->reference_time;
                int min_fid=0;
                for(int j=1;j<M;j++){
                    if(min>f[j]->reference_time){
                        min=f[j]->reference_time;
                        min_fid=j;
                    }
                }
                //페이지 교체
                PF=min_fid;
                victim=f[PF]->page_number;
                f[PF]->page_number=RS[i];
            }
            //LRU: 프레임에 페이지 새로 로드 로딩/참조 시각 업데이트
            f[PF]->reference_time=T;
        }
        print_state(T,M,PF,f,victim);
        T++;
    }
    printf("PAGEFAULT:%5d\n\n",PAGEFAULT);
}

void doFIFO(int N,int M,int K,Frame ** f,int* RS){//M이 프레임 개수, N이 페이지 개수, K는 스트링 길이
    int T=1; // 현재 시각[초], 1초에서 시작, string 하나 지날 때 마다 1 초씩 증가
    int NofE=0; // 페이지를 가진 프레임 개수
    int PF=-1; // Page Fault 발생한 페이지 번호, -1 이면 없다는 의미
    int victim; //victim이되는 프레임의 교체 전 페이지 번호
    int PAGEFAULT=0;
    printf("FIFO\n");
    print_before (M);
    for(int i=0;i<K;i++){
        for(int j=0;j<M;j++){ // Page Fault 여부 체크
            if(f[j]->page_number==RS[i]){ 
                PF=-1;
                break;
            }
            PF=0;
        }
        if(PF!=-1){ // Page Fault 가 존재
            PAGEFAULT++;
            if(NofE<M){ // 빈자리가 존재할 때
                PF=NofE; // 빈자리 중 가장 작은 번호의 페이지에 Page Fault 발생
                f[PF]->page_number=RS[i];
                NofE++;
                victim=-1;
            }
            else{ // 빈자리가 존재하지 않음, FIFO기법에 맞게 페이지 교체
                //교체할 프레임 찾기 (FIFO: 각 프레임 별 페이지의 로딩 시각 가장 작은 것)
                int min = f[0]->load_time;
                int min_fid;
                min_fid=0;
                for(int j=1;j<M;j++){
                    if(min>f[j]->load_time){
                        min=f[j]->load_time;
                        min_fid=j;
                    }
                }
                //페이지 교체
                PF=min_fid;
                victim=f[PF]->page_number;
                f[PF]->page_number=RS[i];
            }
            //FIFO: 프레임에 페이지 새로 로드 로딩 시각 업데이트
            f[PF]->load_time=T;
        }
        print_state(T,M,PF,f,victim);
        T++;
    }
    printf("PAGEFAULT:%5d\n\n",PAGEFAULT);
}

void doMIN(int N,int M,int K,Frame ** f,int* RS){//M이 프레임 개수, N이 페이지 개수, K는 스트링 길이
    int T=1; // 현재 시각[초], 1초에서 시작, string 하나 지날 때 마다 1 초씩 증가
    int NofE=0; // 페이지를 가진 프레임 개수
    int PF=-1; // Page Fault 발생한 페이지 번호, -1 이면 없다는 의미
    int victim; //victim이되는 프레임의 교체 전 페이지 번호
    int PAGEFAULT=0;
    printf("MIN\n");
    print_before (M);
    for(int i=0;i<K;i++){
        for(int j=0;j<M;j++){ // Page Fault 여부 체크
            if(f[j]->page_number==RS[i]){ 
                PF=-1;
                break;
            }
            PF=0;
        }
        if(PF!=-1){ // Page Fault 가 존재
            PAGEFAULT++;
            if(NofE<M){ // 빈자리가 존재할 때
                PF=NofE; // 빈자리 중 가장 작은 번호의 페이지 프레임에 Page Fault 발생
                f[PF]->page_number=RS[i];
                NofE++;
                victim=-1;
            }
            else{ // 빈자리가 존재하지 않음, MIN기법에 맞게 페이지 교체
                //victim이 있는 frame 선택
                int max_fid,max_time,x;
                for(int j=0;j<M;j++){ //존재하는 모든 프레임에 대해
                    for(int t=T;t<K;t++){ // 변수 x 에 이후에 다시 등장하는 시각을 대입
                        if(f[j]->page_number==RS[t]){
                            x=t;
                            break;
                        }
                        x=INT_MAX; // 이후에 다시 등장하지 않으면 무한대
                    }
                    
                    if(j==0){
                        max_time=x;
                        max_fid=j;
                    }
                    else{
                        if(max_time<x){
                            max_time=x;
                            max_fid=j;
                        }
                    }
                }
                //페이지 교체
                PF=max_fid;
                victim=f[PF]->page_number;
                f[PF]->page_number=RS[i];
            }
        }
        print_state(T,M,PF,f,victim);
        T++;
    }
    printf("PAGEFAULT:%5d\n\n",PAGEFAULT);
}

int getInput(int N,int M, int W, int K,int* RS){
    
    int rt=0;
    if((N>100)||(N<1)){
        printf("Process가  갖는 page 개수는 1 이상 100 이하의 수이어야 합니다.\n");
        rt=-1;
    }
    if((M>20)||(M<1)){
        printf("Page Frame 개수는 1 이상 20 이하의 수이어야 합니다.\n");
        rt=-1;
    }
    if((W>100)||(W<1)){
        printf("Window Size는 1 이상 100 이하의 수이어야 합니다.\n");
        rt=-1;
    }
    if((K>100000)||(K<1)){
        printf("Page Reference String 길이는 1 이상 100000 이하의 수이어야 합니다.\n");
        rt=-1;
    }
   
    for(int i=0;i<K;i++){
        if((RS[i]>100)||(RS[i]<0)){
            printf("Page 번호는 0이상 %d이하의 수 이어야 합니다.\n",N);
            rt=-1;
            break;
        }
    }  
    return rt;
}

void printInput(int N,int M, int W, int K,int* RS){
    printf("N: %d,",N);
    printf("M: %d,",M);
    printf("W: %d,",W);
    printf("K: %d\n",K);
    for(int i=0;i<K;i++){
        printf("%d",RS[i]);
        if((i%10==9)||(i==K-1)){
            printf("\n");
        }
        else{
            printf(" ");
        }
    }

}
#endif