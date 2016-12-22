# import sys
# import os
# import socket
# import struct
# from impacket import ImpactDecoder, ImpactPacket
#
# NLMSG_NOOP = 0x1
#
# def main():
#
#     # 创建原始套接字，设置协议为NETLINK_TEST=20
#     sock = socket.socket(socket.AF_NETLINK, socket.SOCK_RAW, 20)
#
#     sock.bind((os.getpid(), 0))
#
#     # 设置nlmsghdr的信息
#     nlmsg_len = 0  # 头部长度, __u32
#     nlmsg_type = NLMSG_NOOP  # 消息主体, __u16
#     nlmsg_flags = 0 # 额外参数, __u32
#     nlmsg_seq = 0   # 序列号, __u32
#     nlmsg_pid = os.getpid()   # 进程pid, __u32
#
#     nlmsg_mymsg = "testpyclient"    # 用户消息
#
#     # 重新设置长度
#     nlmsg_len = sys.getsizeof(nlmsg_len) + sys.getsizeof(nlmsg_type) +\
#         sys.getsizeof(nlmsg_flags) + sys.getsizeof(nlmsg_seq) +\
#         sys.getsizeof(nlmsg_pid) + len(nlmsg_mymsg)
#
#
#     # 打包参数，int or long,
#     nlmsg = struct.pack('isiiis', nlmsg_len, nlmsg_type, nlmsg_flags, nlmsg_seq,\
#                         nlmsg_pid, nlmsg_mymsg)
#
#     ret = socket.send(nlmsg)
#
# if __name__ == '__main__':
#     main()


import os
import socket
import struct
from .constants import *
from .message import Message

class Connection(object):
    """
    Object representing Netlink socket connection to the kernel.
    """
    def __init__(self, nlservice=socket.NETLINK_ROUTE, groups=0):
        # nlservice = Netlink IP service
        self.fd = socket.socket(socket.AF_NETLINK, socket.SOCK_RAW, nlservice)
        self.fd.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 65536)
        self.fd.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 65536)
        self.fd.bind((0, groups)) # pid=0 lets kernel assign socket PID
        self.pid, self.groups = self.fd.getsockname()
        self._seq = 0

    def send(self, msg):
        if isinstance(msg, Message):
            if msg.seq == -1:
                msg.seq = self.seq()
            msg.pid = self.pid
            msg = msg.pack()
        self.fd.send(msg)

    def recv(self):
        contents, (nlpid, nlgrps) = self.fd.recvfrom(16384)
        msg = Message.unpack(contents)

        if msg.type == NLMSG_ERROR:
            errno = -msg.service_template.error_code
            #errno = -struct.unpack("i", msg.payload[:4])[0]
            if errno != 0:
                err = OSError("Netlink error: %s (%d) | msg: %s" % (
                    os.strerror(errno), errno, msg.service_template.old_message))
                err.errno = errno
                raise err
        return msg

    def seq(self):
        self._seq += 1
        return self._seq