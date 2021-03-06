# -*-coding:utf-8-*-
import sys
import os
import socket
import struct


NETLINK_TEST = 20
NETLINK_TEST_CONNECT = 0x10

def main():

    # 创建原始套接字，设置协议为NETLINK_TEST=20
    sock = socket.socket(socket.AF_NETLINK, socket.SOCK_RAW, NETLINK_TEST)

    # 设置nlmsghdr的信息
    nlmsg_len = 0  # 头部长度, __u32
    nlmsg_type = 0x10  # 消息主体, __u16
    nlmsg_flags = 0 # 额外参数, __u16
    nlmsg_seq = 0   # 序列号, __u32
    nlmsg_pid = os.getpid()  # 进程pid, __u32

    nlmsg_mymsg = "drop\0"    # 用户消息

    # 重新设置长度
    # nlmsg_len = sys.getsizeof(nlmsg_len) + sys.getsizeof(nlmsg_type) +\
    #     sys.getsizeof(nlmsg_flags) + sys.getsizeof(nlmsg_seq) +\
    #     sys.getsizeof(nlmsg_pid)
    # print "nlmsg_len=" + str(nlmsg_len)

    # 消息头长度为16
    nlmsg_len = 16


    # 打包参数，int or long,
    #nlmsg = struct.pack('IHHII%ds' % len(nlmsg_mymsg), nlmsg_len, nlmsg_type, nlmsg_flags, nlmsg_seq, nlmsg_pid, str(nlmsg_mymsg))
    nlmsg = struct.pack('IHHII', nlmsg_len, nlmsg_type, nlmsg_flags, nlmsg_seq, nlmsg_pid)
    #print type(nlmsg)
    #print ":".join("{:02x}".format(ord(c)) for c in nlmsg)
    sock.send(nlmsg)

    while True:
        data = sock.recv(1024)
        # data的第一位为nlmsg_len，表示消息的长度，data前16字节为nlmsg的消息头
        # print "len=" + str(ord(data[0]))
        # print ":".join("{:02x}".format(ord(c)) for c in data)
        #the following message from kernel
        kdata=data[16:ord(data[0])]
        #print data[16:ord(data[0])]
        print "k:", kdata
        hdata=handledata(kdata)
        print "h:", hdata

        nlmsg_type = 0x12  # 消息主体, __u16
        nlmsg_replay = struct.pack('IHHII%ds' % len(nlmsg_mymsg), nlmsg_len, nlmsg_type, nlmsg_flags, nlmsg_seq, nlmsg_pid, str(nlmsg_mymsg))


        if hdata is 'B':
            print "I have sent a message."
            sock.send(nlmsg_replay)

        #DEBUG:UDP:"10.108.166.130":51891 ---> "10.108.167.101":8888::
        # <?xml version='1.0' encoding='gb2312' ?><note id='3'><title>ProblemAck</title>
        # <message>A5</message></note>


def handledata(example = ''):
    """"""
    leftdex = example.find(r"<message>")
    rightdex = example.find(r"</message>")
    if leftdex < 0 or rightdex < 0:
        print "fail to parse the event message\n"
        return
    else:
        emessage = example[leftdex+9:rightdex]
        return emessage

if __name__ == '__main__':
    main()
