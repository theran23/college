from socket import *
from threading import Thread
import time

serverPort = 12000  

serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind(('', serverPort))

########## 실험 조건 ################
queue_size = 50 # 큐가 가지고 있을 수 있는 pkt 개수를 의미한다. 
bandwidth = 800 # 1초에 packt 100개 속도로 큐에서 내보냄을 의미한다.
no_pkt = 1000 # the total number of packets to send
###################################

pkt_queue = []
dequeued_pkt = []

rcv_flag = 0
end_flag = 0

clientAddress = -1

num_of_drop = 0

def enqueing():
    global num_of_drop

    global clientAddress
    global end_flag
    while True:
        if end_flag == 1:
            return
        try:
            message, clientAddress = serverSocket.recvfrom(2048)
            pkt = int(message.decode()) # extract sequence number
            # print("", 'q:'+str(pkt), flush=True)
            
            # drop policy: tail drop
            if len(pkt_queue) == queue_size:
                # print('pk dropped',flush=True)
                num_of_drop += 1
                continue
            else:
                pkt_queue.append(pkt)
                # print("pkt_queue", str(pkt_queue), flush=True)

        except:
            continue

    


def dequeing():
    global end_flag
    while True:
        if end_flag == 1:
            return
        # 해당 두 줄 코드에 대한 실행 시간은 무시
        if len(pkt_queue) != 0:
            pkt = pkt_queue.pop(0)
            # print("pkt_queue-t2", str(pkt_queue), flush=True)
            dequeued_pkt.append(pkt)
            # print("out", 'q1:'+str(dequeued_pkt), flush=True)

            time.sleep(1/bandwidth)
        

print('The server is ready to receive')

rcv_base = 0  # next sequence number we wait for


th_enqueing = Thread(target = enqueing, args = ())
th_enqueing.start()

th_dequeing = Thread(target = dequeing, args = ())
th_dequeing.start()

while True:
    # message, clientAddress = serverSocket.recvfrom(2048)
    # seq_n = int(message.decode()) # extract sequence number
    # print(seq_n)

    if len(dequeued_pkt) !=0 :
        seq_n = dequeued_pkt.pop(0)
        print(seq_n, flush=True)
        # print("seq:", str(seq_n), flush=True)
        if seq_n == rcv_base: # in order delivery
            rcv_base = seq_n + 1 

        serverSocket.sendto(str(rcv_base-1).encode(), clientAddress) # send cumulative ack
        # print("send", 'ack:'+str(rcv_base), flush=True)
        

        #수정: rcv_ base가 되어야 함.
        if rcv_base == no_pkt:
            end_flag = 1
            break


print('------------------------------------------------------------------------')


print('queue size: ',queue_size)
print('bandwidth: ', bandwidth)

print("# of drop(packet loss)",num_of_drop)

serverSocket.close()
th_enqueing.join()

th_dequeing.join()





