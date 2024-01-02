
/*
   Author: Sherman Chen
   Create Time: 2023-01-17
   Email: schen@simviu.com
   Copyright(c): Simviu Inc.
   Website: https://www.simviu.com
 */

#include "ut/cutil.h"



using namespace ut;

namespace
{
    // internal function
    class CmdFunc : public Cmd{
    public:
        CmdFunc()
        {
           
            add(":halt", "(system halt loop)",
            [&](CStrs& args)->bool{  
                while(1) sys::sleep(0.01);
                return true;
            });

            //----
            add(":sleep", "t=<SEC>",
            [&](CStrs& args)->bool{  
                double t=0;
                if(!KeyVals(args).get("t", t))
                    return false;
                sys::sleep(t);
                return true;
            });
        }
    };  CmdFunc cmdFunc_;

}


//--------------------
// Cmd
//--------------------
string CmdBase::rm_comment(CStr & s)const
{
    auto n = s.find('#');
    return s.substr(0, n);
}

//----
bool CmdBase::run_func(CStrs& args)
{
    assert(args.size()!=0);
    string scmd=args[0];
    assert(scmd!="");
    KeyVals kvs(args);
    return cmdFunc_.run_args(args);
}

//----
bool CmdBase::runln(const string& sLn_in)
{
    string sLn = ut::remove(sLn_in, '\n');
    auto sLns = tokens(sLn, ';');
    for(auto& s : sLns)
    {
        if(s=="")continue;
        auto args = tokens(s, ' ');
        if(args.size()==0)
            continue;
        //---- check embedded functions
        string scmd = args[0];
        if(scmd=="") continue;
        bool ok = true;
        if(scmd[0]==':')
            ok = run_func(args);
        //---- run this cmd
        else ok = run_args(args);
        //--- exit loop
        if(!ok) return false;
    }
    return true;
}

//---- Cmd server ack
string CmdBase::Ack::str()const 
{
    stringstream s;
    s << "run_ok=" << run_ok << endl;
    s << "res:\n";
    s << s_res << endl;
    return s.str();
}
//---
string CmdBase::Ack::enc()const
{
    //---- A simple protocol wrap output
    // into multiple lines:
    //  'cmd_ack\n' 
    //  'cmd_ok=[true|false]\n'
    //  '[LOG_LINE\n | ...]'
    //  'cmd_ack_end\n'
    string s;
    s += "cmd_ack\n";
    string s_ok = "cmd_ok=";
    s_ok += run_ok?"true":"false" ;
    s_ok += "\n";

    s += s_ok + s_res + "\n"; 
    s += "cmd_ack_end\n";
    return s;
}
//----
bool CmdBase::Ack::dec(CStrs& ss)
{
 //   auto ss = tokens(sLns, '\n');
    if(ss.size()<3)
    {
        log_e("CmdBase::Ack::dec() expect at least 3 lines");
        return false;
    }
    //----
    if( (ss[0]!="cmd_ack") ||
        (ss[ss.size()-1]!="cmd_ack_end") )
    {
        log_e("CmdBase::Ack::dec() expect 'cmd_ack' and 'cmd_ack_end'");
        return false;        
    }
    //----
    KeyVals kvs(ss);
    //----
    string s_ok = kvs["cmd_ok"];
    if(s_ok=="true") run_ok = true;
    else if(s_ok=="false") run_ok = false;
    else {
        log_e("CmdBase::Ack::dec() expect 'true' or 'false'");
        return false;
    } 
    //----
    s_res = "";
    for(int i=2;i<ss.size()-1;i++)
        s_res += ss[i];
    return true;
}


//-----
bool CmdBase::run_args(CStrs& args)
{
    //--- run actual cmd
    if(f_!=nullptr)
    {
        if(f_(args)) return true;
        log_e("  Cmd failed, usage: '"+sHelp_ +"'");
        return false;
    }

    // check subcmds
    if(cmds_.size()==0) 
    {
        log_e("Cmd not init and no subcmds");
        return false;
    }
    //-------
    string sc = args[0];
    if(sc=="help")
    {
        log_i(help());
        return true;

    }
    
    //----
    auto p = lookup(cmds_, sc);
    if(p==nullptr)
    {
        log_e("can't find subcmd:'"+sc+"'");
        return false;
        
    }
    //---- run subcmd
    Strs ss = args;
    ss.erase(ss.begin());
    return p->run_args(ss);

}
//-----
string CmdBase::help(const string& s_prefix)const
{
    string s= s_prefix + " "+ sHelp_ + "\n";
    for(auto& it : cmds_)
    {
        auto& sCmd = it.first;
        auto p = it.second;
        s += p->help("  "+s_prefix + " " + sCmd);        
    }
   
    return s;
}


//----
bool CmdBase::run_console()
{
    log_i("Cmd console, 'help' for help, 'quit' to exit.\n");
    while(1)
    {
        log_s("> ") ;
        string sln;
        std::getline(std::cin, sln);
        
        //--- check quit
        if(sln=="quit") break;

        
        //--- run
        runln(sln);
    }
    return true;
}


