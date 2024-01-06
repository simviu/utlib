/*
   Author: Sherman Chen
   Create Time: 2022-05-12
   Email: schen@simviu.com
   Copyright(c): Simviu Inc.
   Website: https://www.simviu.com
 */

#include "ut/cutil.h"
#include <unistd.h>
#include <stdio.h>
#include <filesystem>

#define PATH_BUF_LEN 1024

namespace ut{
   
    //--- file name/ path util
    namespace fn
    {
        extern string nopath(const string& s) 
        {
        // ref : https://btechgeeks.com/how-to-get-filename-from-a-path-with-or-without-extension-in-cpp/
            char sep = '/';
            #ifdef _WIN32
            sep = '\\';
            #endif
            size_t i = s.rfind(sep, s.length());
            if (i != string::npos) 
            {
                string filename = s.substr(i+1, s.length() - i);
                string rawname = filename.substr(0, s.length()); 
                return(rawname);
            }
            return("");
        }
    }
    //----
    string remove(const string& s, const char c)
    {
        string s1 = s;
        s1.erase(remove(s1.begin(), s1.end(), c), s1.end()); 
        return s1;
    }

    //---- TODO: deprecated and put in KeyVals
    extern bool parseKV(CStrs& ss, StrTbl& kv)
    {
        for(auto& si : ss)
        {
            string s = si;
            s.erase(remove(s.begin(), s.end(), ' '), s.end());

            if(s=="")continue;
            size_t e = s.find('=');
            //---- standalong string take as option
            if(e==string::npos)
            { kv[s] =""; continue; }
            //--- split k/v
            string sk = s.substr(0,e);
            string sv = s.substr(e+1);
            kv[sk] = sv;
        }
        return true;
    }
    //----
    extern vector<string> tokens(
        const string& s, char c_deli)
    {
        vector<string> ts;
        stringstream ss(s);
        string t;
        while(getline(ss, t, c_deli))
            if(t!="")
                ts.push_back(t);
        
        return ts;
    }

    //-------------- -----
    //-----
    bool KeyVals::parse(const string& s, char c_sep)
    { 
        string s1 = s;
        std::replace(s1.begin(), s1.end(), '\n', c_sep);         
        return parse(tokens(s1, c_sep)); 
    }

    //----
    bool KeyVals::has(const string& skey)const
    {
        return items.find(skey)!=items.end();
    }
    string KeyVals::get(const string& skey)const
    {
        auto it = items.find(skey);
        if(it==items.end()) {
            log_e("  key '"+skey+"' not found");
            return "";
        }
        return it->second;
    }
    //--------
    //------
    namespace{
        //----- Help func for KeyVals::get()
        template<typename T> bool get_val(
            const KeyVals& kvs, 
            const string& skey,
            const string& sType,
            T& d)
        {
            string sv = kvs.get(skey);
            if(sv=="") return false;
            if(s2d<T>(sv, d)) return true; 
            
            log_e("  For key '"+skey+"' fail to parse '"+
                    sv+"' to "+sType);
            return false;             
        }
    }
    //--------
    bool KeyVals::get(const string& skey, double& d)const
        {  return get_val(*this, skey, "'double'", d); }
    bool KeyVals::get(const string& skey, int& d)const
        {  return get_val(*this, skey, "'int'", d); }
    bool KeyVals::get(const string& skey, bool& d)const
        {  return get_val(*this, skey, "'bool'", d); }
    //--------
    bool KeyVals::get(const string& skey, string& s)const
    {   
        auto it = items.find(skey);
        if(it==items.end()) {
            log_e("  key '"+skey+"' not found");
            return false;
        }
        s = it->second;
        return true;
    }
    //-------
    extern string f2s(double d, int width)
    {
        stringstream s;
        s << std::setw(width);
        s << std::fixed << d;
        return s.str();
    }

    //-----------------
    extern bool s2data(const string& s, vector<double>& ds, char c_deli)
    {
        auto ts = tokens(s, c_deli);
        for(auto& t : ts)
        {
            double d=0;
            int r = sscanf(t.c_str(), "%lf", &d);
            if(r!=1) return false;
            ds.push_back(d);
        }
        return true;
    }
    //-----
    extern bool s2data(const string& s, vector<int>& ds, char c_deli)
    {
        auto ts = tokens(s, c_deli);
        for(auto& t : ts)
        {
            int d=0;
            int r = sscanf(t.c_str(), "%d", &d);
            if(r!=1) return false;
            ds.push_back(d);
        }
        return true;
    }

    //-----
    extern bool s2hex(const string& s, uint32_t& d)
    {
        if( s.length()<2) return false;
        if( (s[0]!='0') || (s[1]!='x') ) return false;
        string sx = s.substr(2, s.length()-2);
        stringstream ss;
        ss >> std::hex >> d;
        return (!ss.fail()); 
    }




}// namespace ut
