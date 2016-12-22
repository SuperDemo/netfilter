
from .connection import Connection

if __name__ == "__main__":
    NETLINK_TEST = 20

    conn = Connection(NETLINK_TEST)
    conn.send("testpyclient")
    print conn.recv()