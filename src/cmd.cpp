/*
   Author: Sherman Chen
   Create Time: 2023-11-06
   Email: schen@simviu.com
   Copyright(c): Simviu Inc.
   Website: https://www.simviu.com
 */

#include "ut/cutil.h"

using namespace ut;
//---- 
string Cmd::usage()const
{
    string s;
    s += "  Command line options:\n";
    s += "    --help  : Help\n";
    s += "    --console  : Console\n";
    s += "    --file <CMD_FILE> : Run cmd file\n";
    s += "    --server port=<PORT> : TCP server\n";
    s += "    <CMD> [ARGS] \n";
    return s;
}

//--------
bool Cmd::run(int argc, char ** argv)
{
    bool ok = true;
    
#ifdef CUTIL_NO_EXCEPTION //----
    ok = run_core(argc, argv);
#else //-----
    try{
        ok = run_core(argc, argv);
    }
    catch(const std::exception& e)
    {
        stringstream s;
        s << "Cmd::run() exception :'";
        s << e.what() << "'";
        log_e(s.str());
        ok = false;
    }
#endif //---- CUTIL_NO_EXCEPTION
    return ok;
}

//--------
bool Cmd::run_core(int argc, char ** argv)
{

    //---- no arg
    if(argc==1)
    {
        log_i(usage());
        return true;
    }

    //---- run with args
    Strs args;
    for(int i=1;i<argc;i++)
        args.push_back(argv[i]);
    
    //--- check script -f
    string scmd = args[0];
    if(scmd=="--help")
    {
        log_i(help());
        return true;
    }
    else if(scmd=="--file")
    {
        if(args.size()<2)
        {
            log_e("Expect <FILE> after '-f'");
            return false;
        }
        //----
        return runFile(args[1]);
    }
    else if(scmd=="--server")
        return run_server(args);
    else if(scmd=="--console")
        return run_console();
    else if(args.size()==1)// TODO: ?
        return runln(args[0]);
    else 
        return run_args(args);
    return true;
}
//----
bool Cmd::runFile(CStr& sf)
{
    log_i("Cmd::runFile() :'"+sf+"'...");
    ifstream f;
    f.open(sf);
    if(!f.is_open())
    {
        log_ef(sf);
        return false;
    }
    //
    int i=0;
    bool ok = true;
    while(!f.eof())
    {
        i++;
        string si;
        getline(f, si);
        string si2 = rm_comment(si);
        string s = ut::remove(si2, '\n');
        if(s=="")continue;
        log_i("Run cmd (line "+to_string(i)+"):'"+s+"'");
        ok = runln(s);
        if(!ok)
        {
            log_e("Line "+to_string(i)+" in '"+sf+"':");
            log_e("  Cmd fail:'"+s+"'");
            break;
        }
    }
    //----
    if(ok)
        log_i("Cmd::runFile() done OK");
    else
        log_e("Cmd::runFile() stopped with Error");
    return ok;

}

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
        log_i("Cmd server wait...('q' to quit)");
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
        sRes_ = "";
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