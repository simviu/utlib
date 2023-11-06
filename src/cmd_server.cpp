/*
   Author: Sherman Chen
   Create Time: 2023-11-06
   Email: schen@simviu.com
   Copyright(c): Simviu Inc.
   Website: https://www.simviu.com
 */

#include "ut/cutil.h"

using namespace ut;

//----- arm server
bool Cmd::run_server(CStrs& args)
{
    StrTbl kv; parseKV(args, kv);
    string s_port = lookup(kv, string("port"));
    int port=0; 
    if(!s2d(s_port, port))
    {
        log_e(" failed to get para 'port'");
        return false;
    }
   
    return run_server(port);
}
//----
bool Cmd::run_server(int port)
{
    log_i("Cmd::run_server() on port:"+to_string(port));
    socket::Server svr;
    //-----
    bool ok = svr.start(port);
    if(!ok)
    {
        log_e("Failed to start server at port:"+to_string(port));
        return false;
    }
    
    //---- server started
    while(svr.isRunning())
    {
        if(!svr.isConnected())
        {
            sys::sleep(0.2);
            continue;
        } 

        //----
        log_i("Cmd server wait...");
        string srcv;
        if(!svr.recvLn(srcv)) 
        {
            sys::sleep(0.2);
            continue;
        }
        //-----
        string sr2 = rm_comment(srcv);
        string sln = ut::remove(sr2, '\n');
        if(sln=="")continue;
        //---
        //---- run cmd
        log_i("Cmd server recv and run:'"+sln+"'");

        //---- run session
        Ack ack;
        ack.run_ok = this->runln(sln);
        ack.s_res = sRes_;
        string sr = ack.enc();
        
        //---
        log_d("  send ack...");
        svr.send(sr);
        sys::sleep(0.2);
        log_d("done\n");
    }
    log_i("Server shutdown");
    //---- 
    return true;
}