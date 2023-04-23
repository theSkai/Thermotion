#ifndef __POLYLINEWATER__
#define __POLYLINEWATER__

#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include "Vector3f.h"
using namespace std;

//Water are segmented using marker and submarker structure
class WaterManager
{
    struct marker
    {
        marker(double a, double b, double temp)
        {
            stagel = a;
            stager = b;
            temperature = temp;
        }
        double stagel; //left edge
        double stager; //right edge
        double temperature;
    };
    struct submarker
    {   
        //half marker
        submarker(const marker a, bool left)
        {
            stage = left ? a.stagel : a.stager;
            temperature = a.temperature;
        };
        submarker(const double stag, double temp)
        {
            stage = stag;
            temperature = temp;
        };
        double stage;
        double temperature;
    };

public:
    vector<submarker> returnmark;
    void LoadWaterManager(string fname)
    {
        //load polyline water
        {
            ifstream fin;
            fin.open(fname, ios::in);
            if(!fin.is_open()){
                cerr << "Cannot open water file" << endl;
                exit(0);
            }
            polylinewater.clear();
            width.clear();
            area.clear();
            //total number;
            double a;
            fin >> a;
            string ct;
            //get Water mode;
            bool Watermode = false;
            fin >> ct;
            if (ct == "PolylineWater")
            {
                Ispolyline = true;
            }
            else if (ct == "AreaWater")
            {
                Ispolyline = false;
            }
            else
            {
                cerr << "Not define water mode" << endl;
                exit(0);
            }
            //get Channnel width mode;
            bool consChannel = false;
            fin >> ct;
            if (ct == "Constant")
            {
                consChannel = true;
                double e;
                fin >> e;
                for (int i = 0; i < a; i++)
                {
                    width.push_back(e);
                }
            }
            else if (ct == "Changing")
            {
                consChannel = false;
            }
            else
            {
                cerr << "Not define channel width mode" << endl;
                exit(0);
            }
            //get Channel area mode;
            bool consArea = false;
            fin >> ct;
            if (ct == "Constant")
            {
                consArea = true;
                double e;
                fin >> e;
                for (int i = 0; i < a; i++)
                {
                    area.push_back(e);
                }
            }
            else if (ct == "Changing")
            {
                consArea = false;
            }
            else
            {
                cerr << "Not define channel area size mode" << endl;
                exit(0);
            }
            //Loading Water;
            for (int i = 0; i < a; i++)
            {
                Vector3f temp;
                fin >> temp.x() >> temp.y() >> temp.z();
                polylinewater.push_back(temp);
            }
            //Loading Changing widths;
            if (!consChannel)
            {
                for (int i = 0; i < a; i++)
                {
                    double e;
                    fin >> e;
                    width.push_back(e);
                }
            }
            if (!consArea)
            {
                for (int i = 0; i < a; i++)
                {
                    double e;
                    fin >> e;
                    area.push_back(e);
                }
            }
            fin.close();
        }
        //compute water stages
        {
            double a = polylinewater.size();
            waterstages.clear();
            waterstages.resize(polylinewater.size());
            waterlength = 0;
            for (int i = 0; i < a - 1; i++)
            {
                waterstages[i] = waterlength;
                waterlength += (polylinewater[i + 1] - polylinewater[i]).length();
            }
            waterstages[waterstages.size() - 1] = waterlength;
            for (int i = 0; i < a; i++)
            {
                waterstages[i] /= waterlength;
            }
        }
        //compute delta stages
        {
            diffstages.resize(waterstages.size() - 1);
            for (int i = 0; i < diffstages.size(); i++)
            {
                diffstages[i] = waterstages[i + 1] - waterstages[i];
            }
        }
    }
    void SetStartConditions(double temp, double flow, double tstep)
    {
        starttemperature = temp;
        startwaterflow = flow;
        constantseconds = tstep;
        __ResetAll();
    }
    int evostep()
    {
        if (!started)
            return 0;
        if (!Ispolyline)
            return -1;
        int evoret = 1;
        double newdeltastage;
        if (reversed)
        {
            for (auto i = edgemark.rbegin(); i != edgemark.rend(); i++)
            {
                if ((*i).stagel > 0.0)
                    (*i).stagel = max((*i).stagel - (deltastage / stageArea(i->stagel)), 0.0);
                if ((*i).stager < 1.0 || i != edgemark.rbegin())
                    (*i).stager = max((*i).stager - (deltastage / stageArea(i->stager)), 0.0);
            }
            while (edgemark.size() > 1)
            {
                if ((*edgemark.begin()).stager != 0)
                    break;
                edgemark.pop_front();
                evoret = 2;
            }
        }
        else
        {
            for (auto i = edgemark.begin(); i != edgemark.end(); i++)
            {
                if ((*i).stagel > 0.0 || i != edgemark.begin())
                    (*i).stagel = min((*i).stagel + (deltastage / stageArea(i->stagel)), 1.0);
                if ((*i).stager < 1.0)
                    (*i).stager = min((*i).stager + (deltastage / stageArea(i->stager)), 1.0);
            }
            while (edgemark.size() > 1)
            {
                if ((*edgemark.rbegin()).stagel != 1)
                    break;
                edgemark.pop_back();
                evoret = 2;
            }
        }
        leftguard = edgemark.begin()->temperature;
        rightguard = edgemark.rbegin()->temperature;
        return evoret;
    }
    void getallheads(int evoret)
    {
        //get the edge of water
        returnmark.clear();
        if (!Ispolyline)
        {
            if (!ExecutedIfnotPolyline)
                return;
            else
            {
                for (int i = 0; i < polylinewater.size(); i++)
                {
                    returnmark.push_back(submarker(-i - 1, temperature));
                }
                ExecutedIfnotPolyline = false;
                return;
            }
        }
        if (evoret == 0 && !started)
        {
            return;
        }
        else if (evoret == 2)
        {
            if (reversed)
            {
                for (auto i = edgemark.begin(); i != edgemark.end(); i++)
                {
                    returnmark.push_back(submarker(*i, true));
                }
            }
            else
            {
                for (auto i = edgemark.rbegin(); i != edgemark.rend(); i++)
                {
                    returnmark.push_back(submarker(*i, false));
                }
            }
        }
        else if (evoret == 1)
        {
            if (reversed)
            {
                for (auto i = ++edgemark.begin(); i != edgemark.end(); i++)
                {
                    returnmark.push_back(submarker(*i, true));
                }
            }
            else
            {
                for (auto i = ++edgemark.rbegin(); i != edgemark.rend(); i++)
                {
                    returnmark.push_back(submarker(*i, false));
                }
            }
        }
    }
    void convert(double stage, double &wid, Vector3f &position)
    {
        if (stage < 0)
        {
            position = polylinewater[(int)(-stage) - 1];
            wid = width[(int)(-stage) - 1];
        }
        else if (stage == 1)
        {
            position = polylinewater[polylinewater.size() - 1];
            wid = width[polylinewater.size() - 1];
        }
        else if (stage < 1)
        {
            int finder = stage * (polylinewater.size() - 1);
            while (waterstages[finder] < stage)
                finder++;
            while (waterstages[finder] > stage)
                finder--;
            double rate = (stage - waterstages[finder]) / (diffstages[finder]);
            position = (1 - rate) * polylinewater[finder] + rate * polylinewater[finder + 1];
            wid = (1 - rate) * width[finder] + rate * width[finder + 1];
        }
        else
        {
            cerr << "FATAL ERROR: compute something wrong" << endl;
            exit(0);
        }
    }
    void execute(int n, double param)
    {
        switch (n)
        {
        case 0:
            break;
        case 1:
            __ResetAll();
            break;
        case 2:
            __StartWater();
            break;
        case 3:
            __PauseWater();
            break;
        case 4:
            __ReverseWater();
            break;
        case 5:
            __SwitchTemperature(param);
            break;
        case 6:
            __SwitchVelocity(param);
            break;
        case 7:
            __LoadAir();
            break;
        default:
            break;
        }
    }

private:
    //7 base executions
    void __ResetAll()
    {
        edgemark.clear();
        edgemark.push_back(marker(0, 1, -274));
        temperature = starttemperature;
        waterflow = startwaterflow;
        deltastage = constantseconds * waterflow / waterlength;
        leftguard = -274;
        rightguard = -274;
        reversed = false;
        started = false;
    }
    void __StartWater()
    {
        if (started)
            return;
        if (!Ispolyline)
        {
            started = true;
            ExecutedIfnotPolyline = true;
            return;
        }
        if (reversed)
        {
            if (rightguard != temperature)
                edgemark.push_back(marker(1, 1, temperature));
        }
        else
        {
            if (leftguard != temperature)
                edgemark.push_front(marker(0, 0, temperature));
        }
        started = true;
    }
    void __PauseWater()
    {
        if (!started)
            return;
        started = false;
    }
    void __SwitchTemperature(const double temp)
    {
        if (temperature == temp)
            return;
        else
            temperature = temp;
        if (started)
        {
            if (!Ispolyline)
            {
                ExecutedIfnotPolyline = true;
                return;
            }
            if (reversed)
            {
                if (rightguard != temperature)
                    edgemark.push_back(marker(1, 1, temperature));
            }
            else
            {
                if (leftguard != temperature)
                    edgemark.push_front(marker(0, 0, temperature));
            }
        }
    }
    void __SwitchVelocity(const double flow)
    {
        if (waterflow == flow || flow <= 0)
            return;
        if (!Ispolyline)
            return;
        deltastage = (deltastage / waterflow) * flow;
        waterflow = flow;
    }
    void __ReverseWater()
    {
        if(!Ispolyline)
            return;
        reversed = !reversed;
        if (started)
        {
            if (reversed)
            {
                if (rightguard != temperature)
                    edgemark.push_back(marker(1, 1, temperature));
            }
            else
            {
                if (leftguard != temperature)
                    edgemark.push_front(marker(0, 0, temperature));
            }
        }
    }
    void __LoadAir()
    {
        __SwitchTemperature(-274);
    }
    double stageArea(double stage)
    {
        //section area when the front edge of water is at stage
        //this is a interpolate method 
        if (stage == 1)
        {
            return area[polylinewater.size() - 1];
        }
        else
        {
            int finder = stage * (polylinewater.size() - 1);
            while (waterstages[finder] < stage)
                finder++;
            while (waterstages[finder] > stage)
                finder--;
            double rate = (stage - waterstages[finder]) / (diffstages[finder]);
            return (1 - rate) * area[finder] + rate * area[finder + 1];
        }
    }
    //instructions are gotten between evostep() and getallheads()
    bool started = false;
    bool reversed = false;
    double temperature;
    double waterflow;
    double deltastage;

    double leftguard;
    double rightguard;
    double waterlength;
    list<marker> edgemark;
    
    double starttemperature;
    double startwaterflow;
    double constantseconds;
    vector<Vector3f> polylinewater;
    vector<double> width;
    vector<double> area;
    vector<double> waterstages;
    vector<double> diffstages;
    bool Ispolyline;
    bool ExecutedIfnotPolyline;
};

#endif