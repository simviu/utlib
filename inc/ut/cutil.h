/*
   Author: Sherman Chen
   Create Time: 2022-05-12
   Email: schen@simviu.com
   Copyright(c): Simviu Inc.
   Website: https://www.simviu.com
 */

#pragma once

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <cmath>

#include <vector>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <assert.h>
#include <atomic>

namespace ut
{
    
    using namespace std;

    //------------
    // string utils
    //------------
    using CStr = const string;
    using Strs = vector<string>;
    using CStrs = const vector<string>;

    //-----   TODO: to be deprecated, replace by KeyVals
    //--- parse key/value table, e.g.:     
    // file=a.txt n=10 ...
    using StrTbl = map<string, string>;
    using CStrTbl = const StrTbl;   
    inline string lookup(CStrTbl& m, CStr& sk)
    {  
        auto it=m.find(sk); 
        if(it==m.end()) 
            return ""; 
        return it->second; 
    }
    extern bool parseKV(CStrs& ss, StrTbl& kv);
    inline bool has(CStrTbl& m, CStr& sk)
    { auto it=m.find(sk); return it!=m.end(); }

    //----
    extern string remove(const string& s, const char c);
    extern vector<string> tokens(const string& s, char c_deli=' ');
    //---- if width -1, unset
    extern string f2s(double d, int width=3);

    //----
    template<typename T>
        bool s2d(const string& s, T& d)
        { stringstream t(s); t>>d; return !t.fail(); }
    // TODO: template this by s2d()
    extern bool s2data(const string& s, vector<double>& ds, char c_deli=',');
    extern bool s2data(const string& s, vector<int>& ds,    char c_deli=',');
    // hex string must with prefix of "0x"
    extern bool s2hex(const string& s, uint32_t& d);
    //---- err exception
    class ErrExcept : std::exception{
    public:
        using exception::exception;
        ErrExcept(const string& s):s_(s){}
        virtual string str()const{ return s_; }
    protected:
        string s_;
    };
    //-------
    // KeyVals
    //-------
    class KeyVals{
    public:
        KeyVals(){}
        KeyVals(const string& s, char c_sep=' ')
            { parse(s, c_sep); }
        KeyVals(CStrs& ss){ parse(ss); };

        //---
        bool parse(CStrs& ss){ return parseKV(ss, items);  };
        bool parse(const string& s, char c_sep=' ');

        //----
        bool has(const string& sKey)const;
        string operator[] (const string& skey) { return get(skey); }
        string operator[] (const string& skey) const{ return get(skey); }
        string get(const string& skey)const;

        bool get(const string& skey, double& d)const;
        bool get(const string& skey, int& d)const;
        bool get(const string& skey, bool& d)const;
        bool get(const string& skey, string& s)const;

        string query(const string& skey)const
        { return (has(skey))?get(skey) :""; }
        //----
        map<string, string> items;
    };
    //-----------------------------
    //	Aliase for std::shared_ptr
    //-----------------------------
    //---- aliase shared_ptr<T> to Sp<T>
    template<class T> using Sp = std::shared_ptr<T>;
    
    //---- aliase make_shared<T> to makeSp<T>
    template<class T, class ..._Args>
    inline static std::shared_ptr<T> mkSp(_Args&& ...__args)
    { return std::make_shared<T>(__args...); };

    //---- namespace fn
    /*
    namespace fn
    {
        extern string nopath(const string& s);
    }
    */
   // file path decode
    
    //-----------
    // container utils
    //-----------
    template<class T>
    inline Sp<T> lookup(map<string, Sp<T>>& m, CStr& s)
    { auto it = m.find(s); 
        return(it==m.end())?nullptr:it->second; }
    

    //-----------
    // math
    //-----------
    inline double toRad(double d){ return d*M_PI/180.0;  };
    inline double toDgr(double d){ return d*180.0/M_PI;  };
    template<typename T>
    T dgrIn180(const T& d)
    {   return d - ((int)(d/360.0+(d>0?0.5:-0.5)))*360.0; }
    inline bool isValid(double d)
    { return !(std::isnan(d)||std::isinf(d)); }
    //-----
    class PolyCurve{
    public:
        PolyCurve(){}
        PolyCurve(const vector<double>& k):k_(k){};
        float calc(double t)const;    
        void setk(const vector<double>& k){ k_ = k; }
    protected:
        vector<double> k_;
    };    
    //-----------
    // log
    //-----------
    namespace utlog{
        extern bool openFile(CStr& sFile);
        extern void dbg(CStr& s);
        extern void inf(CStr& s);
        extern void err(CStr& s);
        extern void errf(CStr& s);
        extern void str(CStr& s);
        using FuncCbk=std::function<void(CStr& s)>;
        extern void setCallbk(FuncCbk cbk);
    }
    // shortcuts
    const auto log_s = utlog::str;
    const auto log_i = utlog::inf;
    const auto log_d = utlog::dbg;
    const auto log_e = utlog::err;
    const auto log_ef = utlog::errf;
    //----
    namespace sys{
        struct FPath{
            FPath(const string& sf);
            string path, base, ext;
        };
        extern string pwd();
        inline void sleepMS(int ms){
            this_thread::sleep_for(chrono::milliseconds(ms) );
        }
        inline void sleep(double s){
            size_t ms = s * 1000;
            this_thread::sleep_for(chrono::milliseconds(ms) );
        }
        //--- time functions
        using Time = std::chrono::high_resolution_clock::time_point;
        inline Time now()
        { return std::chrono::high_resolution_clock::now(); }
        inline double elapse(const Time t1, const Time t2)
        { std::chrono::duration<double> e=t2-t1; return e.count(); }
        //----
        class FPS{
        public:
            struct Cfg{ int N_avg=10; };
            Cfg cfg_;
            // check positive as valid
            void tick();
            double fps()const{ return fps_; }
        protected:
            double fps_=-1;
            Time t_ = now();
            list<double> dts;
        };
        //----
        extern bool exists(const string& s);
        extern bool mkdir(const string& s);
    }
    
    //----------------
    // multiple thread
    //----------------
    namespace mth{
        //-----------
        // Pipe
        //-----------
        // Cross thread data delivery
        //   with mutex / conditiona var
        template<class T>
        struct Pipe{
            void push(T d){
                std::unique_lock<std::mutex> ul(m_);  
                que_.push(d);
                cv_.notify_one(); 
            }
            T wait()
            {
                std::unique_lock<std::mutex> ul(m_);  
                cv_.wait(ul,[&] {return que_.size()!=0;});  
                auto p = que_.front();
                que_.pop();
                return p;
            }
            void clear(){
                std::unique_lock<std::mutex> ul(m_);  
                while(!que_.empty())
                    que_.pop();
            }
            size_t size(){
                std::unique_lock<std::mutex> ul(m_);  
                return que_.size();
            }
            T pop(){
                std::unique_lock<std::mutex> ul(m_);  
                auto d = que_.front();
                que_.pop();
                return d;
            }
        protected:
            queue<T> que_;
            std::condition_variable cv_;
            std::mutex m_;
        };   
    }
    //----------
    // Elements
    //----------
    struct Px{
        Px(){}
        Px(int x, int y):x(x),y(y){}
        int x=0;int y=0;
        Px operator - (const Px& d)const
        { return {x - d.x, y - d.y}; }
        Px operator + (const Px& d)const
        { return {x + d.x, y + d.y}; }
        void operator -= (const Px& d)
        {  x -= d.x; y -= d.y; }
        void operator += (const Px& d)
        {  x += d.x; y += d.y; }
        string str()const 
        { stringstream s; 
          s << x <<","<< y; return s.str(); } 
        bool set(const string& s, char c_deli=',');
    };
    //-----
    struct Color{
        uint8_t r=0;
        uint8_t g=0; 
        uint8_t b=0;
        uint8_t a=255;
        //Color(){}
        //Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
        //    r(r),g(g),b(b),a(a){}
        bool set(const string& s, char c_deli=',');
        string str()const 
        { stringstream s; 
          s << (int)r <<","<< (int)g << ","
             << (int)b << "," << (int)a; 
          return s.str(); } 
        bool isBlack()const 
        { return r==0 && g==0 && b==0; }
        bool operator == (const Color& c)const
        { return r==c.r && g==c.g && b==c.b && a==c.a; }
        bool operator != (const Color& c)const
        { return !(*this == c); }
    };
    inline ostream& operator << (ostream& s, const Px& px)
    {  s << px.str(); return s; }
    inline istream& operator >> (istream& s, Px& px)
    {  s >> px.x; s >> px.y; return s; }
    inline Color operator - (const Color& c1, const Color& c2)
        { return {  (uint8_t)(c1.r-c2.r), (uint8_t)(c1.g-c2.g), 
                    (uint8_t)(c1.b-c2.b), (uint8_t)(c1.a-c2.a)}; }
    //----
    struct Sz{
        Sz(){}
        Sz(int w, int h):w(w), h(h){}
        int w=0;
        int h=0;
        bool isIn(const Px& px)
        { return (px.x>=0) && (px.y>=0) && (px.x<w) && (px.y<h); }
        string str()const
        {  stringstream s; s << w << "," << h << endl; return s.str(); }
        bool set(const string& s, char c_deli=',');
        void operator *= (float s){ w*=s; h*=s; }
        bool operator ==(const Sz& b)const
            { return (w==b.w)&&(h==b.h); }
        bool operator !=(const Sz& b)const
            { return (w!=b.w)||(h!=b.h); }
    };
    inline ostream& operator << (ostream& s, const Sz& sz)
    {  s << sz.w << ", " << sz.h; return s; }
    //-----    
    // TODO: rename Rctn
    struct Rect{
        Rect(){}
        Rect(const Px& c, const Sz& sz):cntr(c), sz(sz){}
        Px cntr;
        Sz sz;
        Px p0()const { return Px(cntr.x - sz.w*0.5, cntr.y - sz.h*0.5); }
        Px p1()const { return Px(cntr.x + sz.w*0.5, cntr.y + sz.h*0.5); }
        bool is_in(const Px& p)const 
        {  Px q = p; q -= cntr; q += Px(sz.w/2, sz.h/2); 
           return (q.x<=sz.w)&&(q.y<=sz.h)&&(q.x>=0)&&(q.y>=0); }
        string str()const 
        { return "{ c:\"" + cntr.str() + "\", sz:\""+ 
                sz.str()+"\"}"; }

    };
    //-------------
    // util stuct
    //-------------
    template<typename T=double>
    struct Rng{
        Rng(){};
        Rng(const T& d0, const T& d1):d0(d0),d1(d1){}

        T cut(const T& d)const 
        { return (d<d0)?d0 : (d>d1)?d1 : d; }

        bool isIn(const T& d)const
        { return (d>=d0)&&(d<=d1); }
        void upd(const T& d)
        {   if(!val()) d0=d1=d; 
            else if(d>d1)d1=d; else if(d<d0)d0=d; }
        void upd(const Rng<T>& r)
        { upd(r.d0); upd(r.d1); }
        T len()const{ return fabs(d1-d0); }
        T mid()const{ return (d0+d1)*0.5; }
        void scale(T s)
        {   T l = len(); T c = mid(); 
            d0 = c-l*0.5*s; d1 = c + l*0.5*s; }
        bool val()const{ return d1>=d0; }
        string str()const
        { stringstream s; s << d0 <<"," << d1; return s.str(); }
        T d0=1.0; 
        T d1=0; 
    };
    //-------------
    //  utils
    //-------------
    //bool fexist(CStr& sf);

    //---- Binary Buf
    struct Buf{
        Buf(){}
        Buf(uint8_t* p, size_t n):
        p(p), n(n){}
        Buf(size_t n) { resize(n); }
        void resize(size_t nn)
        { 
            if(p!=nullptr) delete p;
            p = new uint8_t[nn];
            n = nn;
            bDel = true;
        }
        ~Buf()
        { if(bDel && p!=nullptr) 
            delete p; }

        uint8_t operator[](int i)
        {  if(i>=n) return 0xee; return p[i]; }
        
        //--- to C vector buf
        void to(vector<uint8_t>& d)const
        { for(int i=0;i<n;i++)d.push_back(*(p+i)); }

        //----
        uint8_t* p = nullptr; 
        size_t n = 0;
    protected:
        bool bDel = false;
    };

    //---- Legacy C file handler helper
    // for streamming, such as Serial/ Socket
    // TODO: local
    class CStream
    {
    public:
        struct TCfg{
            double timeout = 1;
            // query interval 1ms
            double t_query_int = 0.001;
        }; TCfg cfg_;
        struct Status{
            bool eTimeout = false;
        }; Status st_;
        
        CStream(int fd):fd_(fd){}
        bool readln(string& sln);
        void flush();
        bool readFrm(const string& sHeader, 
                     Buf& buf);
    protected:
        int fd_ = -1;
    };
    //-------------
    // Test
    //-------------
    class Test{
    public:
        Test(){};
        Test(map<string, Sp<Test>>& ts):tests_(ts){}
        virtual bool run();
        virtual bool run(const string& s);
        int run(int argc, char ** argv);

        void add(const string& s, Sp<Test> p)
        { tests_[s] = p; }
        string getTestsStr()const;

        // sub tests
        map<string, Sp<Test>> tests_;

    };
    //-------------
    // Packet
    //-------------
    // simple commnunication data packet, 
    //   for socket or serial link.
    class Packet{
    public:
        struct Head{
            // note:include '\0'
            unsigned char sPrefix[8]{"$utpack"};

            // Payload length, 
            //   len =L0 + L1*256 + L2*256*256 + L3*256*256*256
            uint8_t L[4];
            uint8_t idx=0;    // sequence idx
            uint8_t chksum=0; // checksum of payload only
        };

    };

    //-------------
    // socket
    //-------------
    // TODO: multiple connections
    namespace socket{
        //-----
        class Node{
        public:
            struct Cntx{
                int port = -1;
                string sHost = "undef";
                int cur_socket = -1;
                atomic<bool> bConnected{false};
                atomic<bool> isRunning{false};
            }; Cntx cntx_;

            bool send(const Buf& buf);
            bool send(const string& s);

            // TODO: discard, change to isConnected.
            bool isRunning()const
            { return cntx_.isRunning; }

            bool recvLn(string& sln);
            bool recv(Buf& buf);
            bool isConnected()const 
            { return cntx_.bConnected; }
        protected:
            std::mutex rd_mtx_;
            std::mutex wr_mtx_;
            void onDisconnect();
        };
        
        
        //---
        class Server : public Node{
        public:
            struct Cfg{
            }; Cfg cfg_;
            ~Server(){ close(); }
            bool start(int port);
            void close();
        protected:
           // bool run_thd();
            std::thread thd_;

        };
        class Client : public Node{
        public:
            bool connect(const string& sHost, int port);
        protected:
            void run_thd();
            std::thread thd_;
        };
    }

    //-------------
    // CmdBase
    //-------------
    // string cmd line handler
    class CmdBase{
    public:
        using Fun=function<bool(CStrs& args)>;
        CmdBase(){}
        CmdBase(CStr& sHelp):sHelp_(sHelp){}
        CmdBase(CStr& sHelp, Fun f):sHelp_(sHelp), f_(f){}
        void add(CStr& s, Sp<CmdBase> p) { cmds_[s]=p; }
        void add(CStr& s, CStr& sH, Fun f) { add(s, mkSp<CmdBase>(sH, f)); }
        virtual bool runln(const string& sLn);

        string help(const string& s_prefix="")const;
        auto& cmds(){ return cmds_; }
        auto& cmds()const{ return cmds_; }

        //---- Simple protocol for Server ack
        struct Ack{
            string enc()const;
            bool dec(CStrs& ss);
            //string s_log;
            string s_res;
            bool run_ok = false;
            string str()const;

        };
        //----
        string sHelp_;
        const string& getRes()const{ return sRes_; }
        bool run_console();
    protected:
        string sRes_; // result string, can be filled and retrieved.
        bool run_args(CStrs& args);
        bool run_func(CStrs& args);
        Fun f_=nullptr;
        map<string, Sp<CmdBase>> cmds_;
        string rm_comment(CStr & s)const;
    };
    //-------------
    // Cmd
    //-------------
    // string cmd line handler
    class Cmd : public CmdBase{
    public:
        using CmdBase::CmdBase;

        bool runFile(CStr& sf);
        bool run(int argc, char ** argv);

        bool run_server(CStrs& args);
        bool run_server(int port);

    protected:
        bool run_core(int argc, char ** argv);
        string usage()const;
    };
}
