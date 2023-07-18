import time
import os,sys
import socket


TEST_HOST = "127.0.0.1" 
TEST_PORT = 2468  


LN_MAX_CHARS = 2048
ACK_MAX_LNS = 100
#---------------
# CmdClient
#---------------
class CmdClient(object):
    def __init__(self):
        self.isConnected_ = False
        return None

    #----
    def isConnected(self):
        return self.isConnected_
    
    #----
    def connect(self, sHost, port):
        self.isConnected_ = False
        print("VRP client connect to '", 
                sHost, ":", port,"'...")
        self.sock_ = socket.socket(socket.AF_INET, 
                                   socket.SOCK_STREAM)
        self.sock_.connect((sHost, port))        
        #--- check ok
        try:
            self.sock_.getsockopt(socket.SOL_SOCKET, socket.SO_ERROR)
        except socket.error:
            print("[Error]:Failed to connect to ''", 
                    sHost, ":", port,"'...")
            return False
        #-----        
        print("connected")
        self.isConnected_ = True
        return True
    
    #---
    def sendCmd(self, scmd):
        try:
            self.sock_.sendall(bytes(scmd, "utf-8"))
        except:
            return False
        return True

    #-----
    def getAck_(self):
        ok = False
        sRes = ""
        bAck = False
        for i in range(ACK_MAX_LNS):
            s = self.recvLn_()
            if len(s) == 0:
                continue

            #print("[dbg]:getAck_() got s='"+s+"'")
            ss = s.split("=")
            if ss[0] == "cmd_ack":
                bAck = True
            elif ss[0] == "cmd_ack_end":
                if not bAck:
                    print("Error: header 'cmd_ack' not found")
                    return False,sRes
                return ok,sRes
            elif ss[0] == "cmd_ok":
                ok = True if ss[1] == "true" else False
            else:
                sRes = sRes + s +"\n"
        #-----
        raise("Error: getAck() ACK_MAX_LNS reached, didn't recv cmd_ack_end")
        
        
#---------------
class TestApp(object):
    def run():
        cc = CmdClient()
        ok = cc.connect(TEST_HOST, TEST_PORT)
        if not ok:
            return False
        return


#----------
# main
#----------
if __name__ == "__main__":
    app = TestApp()
    app.run()


