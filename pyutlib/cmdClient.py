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
        self.logCB = None # lambda s, isErr:...
        return None

    #----
    def isConnected(self):
        return self.isConnected_
    
    #----
    def connect(self, sHost, port):
        self.isConnected_ = False
        s_url = sHost + ":" + str(port)
        self.log_("VRP client connect to '" +
                  s_url + "'...")
        self.sock_ = socket.socket(socket.AF_INET, 
                                   socket.SOCK_STREAM)
        self.sock_.connect((sHost, port))        
        #--- check ok
        try:
            self.sock_.getsockopt(socket.SOL_SOCKET, socket.SO_ERROR)
        except socket.error:
            self.logErr_("Failed to connect to host")
            return False
        #-----        
        print("connected")
        self.isConnected_ = True
        return True
    
    #---
    def sendCmd(self, scmd):
        try:
            s = scmd
            if s[-1] != "\n":
                s = s + "\n"
            self.sock_.sendall(bytes(s, "utf-8"))
        except:
            self.logErr_("Failed to sendCmd")
            return False
        return True
    
    #----
    def run_console(self):
        #---
        if len(sys.argv) < 3:
            print("Usage: python cmdClient.py <HOST> <PORT>")
            return False

        sHOST = sys.argv[1]
        port = int(sys.argv[2])
        print("Connect to :", sHOST, " : ",port, "...")
        ok = cc.connect(sHOST, port)
        if not ok:
            return False

        #-----
        while(True):
            ln = input(">")
            scmd = self.chk_cmd(ln)
            if scmd == "":
                continue
            
            ok = self.sendCmd(scmd)
            if not ok:
                self.logErr_("Cmd failed", 
                             ", '", scmd, "'")
                
            #---- get ack
            ok,sRes = self.getAck_()
            if not ok:
                print("Error: getAck() failed")
                continue
            
            print("  getAck() ok")

        return True
    
    #----
    def loadFile(self, sf):
        ok = False
        self.log_("Load cmd file:'" + sf + "'")
        with open(sf) as f:
            lns = f.readlines()
            i = 0
            self.log_("lines="+str(len(lns)))
            for ln in lns:
                i = i + 1
                scmd = self.chk_cmd(ln)
                if scmd == "":
                    continue

                #----- send cmd
                self.log_("[line "+str(i)+"]:'"+scmd+"'")
                ok = self.sendCmd(scmd)
                if not ok:
                    self.logErr_("Cmd fail, line ", i, 
                                 ", '", scmd, "'")
                    return False
                #---- get ack
                ok,sRes = self.getAck_()
                if not ok:
                    print("Error: getAck() failed")
                    return False
                
                print("  getAck() ok")


            return ok

        return ok

    
    #---------------private ----------
    def log_(self, s, isErr = False):
        print(s)
        if self.logCB is not None:
            self.logCB(s, isErr)
        return
    #----
    def logErr_(self, s):
        self.log_("[Error]:"+s, True)
        return
    
    #----
    def chk_cmd(self, sLn):
        s = ""
        #--- remove comment
        for c in sLn:
            if c == "#":
                break
            if c== "\n":
                break
            s = s + c
        #--- check if empty tokens
        if len(s.split(" ")) == 0:
            return ""
        return s

    #----

    #------------- private -------------
    def recvLn_(self):
        if self.sock_ is None:
            raise Exception('Not connected')
            
        s = ""
        for i in range(LN_MAX_CHARS):
            b = self.sock_.recv(1)
            c = b.decode('UTF-8')
            if c == "\n":
                return s
            #print("recv:"+c)
            s = s + c

        return s    
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
                    self.logErr_("header 'cmd_ack' not found")
                    return False,sRes
                return ok,sRes
            elif ss[0] == "cmd_ok":
                ok = True if ss[1] == "true" else False
            else:
                sRes = sRes + s +"\n"
        #-----
        s = "getAck() ACK_MAX_LNS reached, didn't recv cmd_ack_end"
        self.logErr_(s)
        return False,"ACK_MAX_LNS"
        
        
#---------------
class TestApp(object):
    def run(self):
        cc = CmdClient()
        ok = cc.connect(TEST_HOST, TEST_PORT)
        if not ok:
            return False
        return


#----------
# main
#----------
if __name__ == "__main__":
    #app = TestApp()
    #app.run()

    cc = CmdClient()
    if cc.run_console():
        exit(0)

    exit(1)

