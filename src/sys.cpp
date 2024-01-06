/*
   Author: Sherman Chen
   Create Time: 2024-01-06
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

    //--------------------
    // sys
    //--------------------
    namespace sys
    {

        //-----
        FPath::FPath(const string& sf)
        {
            std::filesystem::path p(sf);
            path = string(p.parent_path()) + "/";
            base = p.stem();
            ext = p.extension();
        }

        extern string pwd()
        {
            char s[PATH_BUF_LEN];
            s[0]=0;
            getcwd(s, sizeof(s));
            return string(s);
        }
        
        //-----
        void FPS::tick()
        {
            Time t = now();
            double dt = elapse(t_, t);
            t_ = t;
            //--- wait for 2nd tick()
            if(fps_<0) 
            { fps_ =0; return; }

            //---- sliding window average
            dts.push_back(dt);
            int n = dts.size();
            if(n > cfg_.N_avg)
            {
                dts.pop_front();
                n--;
            }

            //---- update
            double dt_sum = 0;
            for(auto& dt : dts)
                dt_sum += dt;
            if(dt_sum==0) return;
            fps_ = n / dt_sum ;

        }
        //----
        extern bool exists(const string& s)
        {
            return filesystem::exists(s);
        }
        //----
        extern bool mkdir(const string& s)
        {
            if(exists(s))return true;
            bool ok = std::filesystem::create_directory(s);
            if(ok) log_i("mkdir:'"+s+"'");
            else log_e("fail to mkdir:'"+s+"'");
            return ok;
        }

    }
    //--------------------
    // Elements
    //--------------------
    bool Color::set(const string& s, char c_deli)
    {
        vector<int> ds;
        if( (!s2data(s, ds, c_deli)) || ds.size()<3 )
            return false;
        r = ds[0]; g=ds[1]; b=ds[2];
        if(ds.size()>3) a=ds[3];
        return true; 
    }

    //----
    bool Px::set(const string& s, char c_deli)
    {   
        vector<double> ds;
        if( (!s2data(s, ds, c_deli)) ||
            ds.size()<2 )
            return false;
        x = ds[0]; y=ds[1]; 
        return true; 
    }

    //-----
    bool Sz::set(const string& s, char c_deli)
    {
        vector<int> ds;
        if( (!s2data(s, ds, c_deli)) ||
            ds.size()<2 )
            return false;
        w = ds[0]; h=ds[1]; 
        return true; 

    } 

}