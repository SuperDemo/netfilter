import sys
import os
import socket
import struct
from impacket import ImpactDecoder, ImpactPacket

NLMSG_NOOP = 0x1

def main():

    # 创建原始套接字，设置协议为NETLINK_TEST=20
    sock = socket.socket(socket.AF_NETLINK, socket.SOCK_RAW, 20)

    sock.bind((os.getpid(), 0))

    # 设置nlmsghdr的信息
    nlmsg_len = 0  # 头部长度, __u32
    nlmsg_type = NLMSG_NOOP  # 消息主体, __u16
    nlmsg_flags = 0 # 额外参数, __u16
    nlmsg_seq = 0   # 序列号, __u32
    nlmsg_pid = os.getpid()   # 进程pid, __u32

    nlmsg_mymsg = "testpyclient"    # 用户消息

    # 重新设置长度
    nlmsg_len = sys.getsizeof(nlmsg_len) + sys.getsizeof(nlmsg_type) +\
        sys.getsizeof(nlmsg_flags) + sys.getsizeof(nlmsg_seq) +\
        sys.getsizeof(nlmsg_pid) + len(nlmsg_mymsg)


    # 打包参数，int or long,
    nlmsg = struct.pack('IHHII%ds' % len(nlmsg_mymsg), nlmsg_len, nlmsg_type, nlmsg_flags, nlmsg_seq, nlmsg_pid, nlmsg_mymsg)
    print type(nlmsg)
    print ":".join("{:02x}".format(ord(c)) for c in nlmsg)

    socket.send(nlmsg)

if __name__ == '__main__':
    main()


