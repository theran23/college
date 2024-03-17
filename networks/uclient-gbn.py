from socket import *
from threading import Thread
import random
import time
import numpy as np
serverIP = '127.0.0.1' # special IP for local host
serverPort = 12000
clientPort = 12001

########## 실험 조건 ################  
cc_mode = 'delay' # congestion control mode ## initial(wnd고정), loss(lossbased), delay(delaybased)
initial_win = 10 # initial window size
     
initial_ssh = 5 # cc_mode 가 'loss' 일때 초기 스레시홀드
no_pkt = 1000 # the total number of packets to send


# loss_rate = 0.01 # loss rate
###################################

win = initial_win 
ssh = initial_ssh
cc_state = 'ss' # slow start



def ssh_check(): # win 증가할때마다(ack 받아서) 체크
    global win
    global cc_state
    global ssh
    if win >= ssh:
         cc_state = 'ca' # congestion avoidance, 1씩 증가

def slowstart(): # 기본, time out 발생시
    global win
    global cc_state
    global ssh
    ssh = win / 2
    win = 1
    cc_state = 'ss' ## slow start, 2배씩 증가

def fast_recovery(): # 3 dup 발생시
    global win
    global cc_state
    global ssh
    ssh = win / 2
    win = win / 2
    cc_state = 'fr' ## fast recovery, 1씩 증가
    


send_base = 0 # oldest packet sent

seq = 0        # initial sequence number
timeout_flag = 0 # timeout trigger
tdup_flag = 0 # 3 duplicate ack trigger

end_flag = 0

NoD = 0 # number of dup

sent_time = [0 for i in range(2000)]
rtt = [0 for i in range(2000)]

nop_in_rtt = 0 # 지난 rtt 동안 보낸 패킷 개수



clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.bind(('', clientPort))
clientSocket.setblocking(0)

# thread for receiving and handling acks
def handling_ack():

    global NoD
    global tdup_flag
    # global end_flag
    global win
    global cc_state

    print("thread")
    global clientSocket
    global send_base
    global timeout_flag
    global sent_time
    global nop_in_rtt
    global rtt

    alpha = 0.125
    beta = 0.25
    timeout_interval = 10  # timeout interval

    
    pkt_delay = 0
    dev_rtt = 0
    init_rtt_flag = 1
    min_rtt = pow(10,10)
    while True:
       
        if sent_time[send_base] != 0: 
            pkt_delay = time.time() - sent_time[send_base]
            
        if timeout_flag == 0 and pkt_delay > timeout_interval:    # timeout detected
            print("timeout detected:", str(send_base), flush=True)
            print("timeout interval:", str(timeout_interval), flush=True)
            timeout_flag = 1
            if cc_mode == 'loss':
                slowstart()

        try:
            ack, serverAddress = clientSocket.recvfrom(2048)
            ack_n = int(ack.decode())
            print(ack_n, flush=True)

            if send_base == ack_n + 1: # dup ack 
                NoD += 1

            if tdup_flag == 0 and NoD >= 3: # 3 dup detected
                print("3 dup detected:", str(ack_n), flush=True)
                tdup_flag = 1
                if cc_mode == 'loss':
                    fast_recovery()
            
            if init_rtt_flag == 1:
                estimated_rtt = pkt_delay
                init_rtt_flag = 0
            else:
                estimated_rtt = (1-alpha)*estimated_rtt + alpha*pkt_delay
                dev_rtt = (1-beta)*dev_rtt + beta*abs(pkt_delay-estimated_rtt)
            timeout_interval = estimated_rtt + 4*dev_rtt

            
        except BlockingIOError:
            continue
            

        if cc_mode == 'loss':
            #win size 증가
            if cc_state == 'ss':
                win *= 2
            else:
                win += 1
            ssh_check()
        # if cc_mode == 'delay' or cc_mode =='loss':
        t1 = time.time()
        t = t1 - sent_time[send_base]
        rtt[send_base] = t

        kt = round(t,10)*pow(10,10)
        measured_rtt = int(kt)
        if measured_rtt < min_rtt:
            min_rtt = measured_rtt
        if cc_mode == 'delay':
            Tm = (nop_in_rtt*pow(10,10)) / measured_rtt
            Tuc = (win*pow(10,10)) / min_rtt

            # print(Tuc - Tm, flush=True)

            if Tuc - Tm > 300000:
                win -= 1
            else:
                win += 1
            nop_in_rtt = 0

        # window is moved upon receiving a new ack
        # window stays for cumulative ack
        send_base = ack_n + 1

        if ack_n == no_pkt - 1:
            break



# running a thread for receiving and handling acks
th_handling_ack = Thread(target = handling_ack, args = ())
th_handling_ack.start()


send_pkt_include_retransmission = 0
tdup_retransmission = 0
only_retransmission = 0
s_time, end_time = 0, 0


# 수정: 패킷을 no_pkt만큼 보냈더라도 재전송을 이 스레드는 계속 돌아가야함.
while send_base < no_pkt:
    while seq < no_pkt and seq < send_base + win: # send packets within window
        # if random.random() < 1 - loss_rate: # emulate packet loss
        clientSocket.sendto(str(seq).encode(), (serverIP, serverPort))  
        if s_time == 0:
            s_time = time.time()
        send_pkt_include_retransmission +=1
        nop_in_rtt += 1
        sent_time[seq] = time.time()    
        seq = seq + 1
    if tdup_flag == 1 or timeout_flag == 1: # retransmission
        seq = send_base 
        clientSocket.sendto(str(seq).encode(), (serverIP, serverPort))
        sent_time[seq] = time.time()
        nop_in_rtt += 1
        send_pkt_include_retransmission +=1
        only_retransmission += 1
        if tdup_flag == 1:
            tdup_retransmission += 1
        seq = seq + 1
        timeout_flag = 0
        tdup_flag = 0
        NoD = 0
    
        
th_handling_ack.join() # terminating thread

end_time = time.time()

print('------------------------------------------------------------------------')
print('mode', cc_mode)
print('#ofpkt',no_pkt)
print('initial wnd size', initial_win)
if cc_mode=='loss':
    print('initial ssh', ssh)
print('# of retransmission',only_retransmission)
print('by t.o / by 3dup',only_retransmission-tdup_retransmission,'/',tdup_retransmission)

import numpy as np

print('bandwidth:')
print('for all pkt')
print(str(send_pkt_include_retransmission/(end_time-s_time)))
print('except retransmission pkt')
print(str(no_pkt/(end_time-s_time)))


print('')
print('average rtt:')
print(np.mean(rtt))
for i in range(no_pkt):
    print(rtt[i])

print ("done")

clientSocket.close()


