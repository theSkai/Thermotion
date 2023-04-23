#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <vecmath.h>
#include <ctime>
#include <unistd.h>
#include <utility>
#include <chrono>
#include <functional>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "heatmesh.hpp"
#include "customized.hpp"

using namespace std;

double Current_velocity;
double Current_Temperature;
bool Started;
bool Forward;
bool Airing;

//reset to customized setting
void ResetToCustomized()
{
    Current_velocity = Cus_WaterStartVelocity;
    Current_Temperature = Cus_WaterStartTemperature;
    Started = false;
    Forward = true;
    Airing = false;
}
//receive order from Setting
int ReceiveOrder(HeatMesh* mesh)
{
    ifstream fin("TempFile/Setting");
    if (fin.is_open())
    {
        string od;
        double par;
        for (int i = 0; i < 5; i++)
        {
            fin >> od;
            if (fin.eof())
                return 0;
            if (od == "Kill")
            {
                cerr << "KILL ";
                mesh->SayTime();
                return 'k';
            }
            if (od == "Disable")
            {
                cerr << "STOP ";
                mesh->SayTime();
                return 'd';
            }
            if (od == "Restart")
            {
                ResetToCustomized();
                cerr << "RESTART ";
                mesh->SayTime();
                return 1;
            }
            if (od == "Going" && !Started)
            {
                Started = !Started;
                cerr << "START WATER ";
                mesh->SayTime();
                return 2;
            }
            if (od == "Pause" && Started)
            {
                Started = !Started;
                cerr << "PAUSE WATER ";
                mesh->SayTime();
                return 3;
            }
            if ((od == "Forward" && !Forward) || (od == "Backward" && Forward))
            {
                cerr << "REVERSE WATER ";
                Forward = !Forward;
                mesh->SayTime();
                return 4;
            }
            if (od == "Air" && !Airing)
            {
                Airing = true;
                cerr << "LOAD AIR ";
                mesh->SayTime();
                return 7;
            }
            if (od == "Water")
            {
                Airing = false;
            }
            if (od == "Temperature")
            {
                fin >> par;
                if (par != Current_Temperature && !Airing)
                {
                    cerr << "CHANGE TEMPERATURE " << par << " ";
                    Current_Temperature = par;
                    mesh->SayTime();
                    return 5;
                }
            }
            if (od == "Waterflow")
            {
                fin >> par;
                if (par != Current_velocity && !Airing)
                {
                    cerr << "CHANGE WATERFLOW " << par << " ";
                    Current_velocity = par;
                    mesh->SayTime();
                    return 6;
                }
            }
        }
        fin.close();
        return 0;
    }
    else
        return 0;
}
//load custom settings from Simustarter
void LoadCustomizedSettings()
{
    ifstream fin;
    fin.open("TempFile/Simustarter", ios_base::in);
    if (fin.is_open())
    {
        string a;
        double c;
        fin >> a;
        if (a == "UseDecorate")
        {
            Cus_DecorateLocked = false;
            //
            fin >> a;
            assert(a == "CoeWB");
            fin >> c;
            Cus_WaterToBase = c * 1e-6;
            //
            fin >> a;
            assert(a == "CoeBD");
            fin >> c;
            Cus_BaseToDecorate = c * 1e-6;
            //
            fin >> a;
            assert(a == "CoeDA");
            fin >> c;
            Cus_DecrToCeil = c * 1e-6;
            //
            fin >> a;
            assert(a == "BaseThickness");
            fin >> c;
            Cus_BaseThickness = c;
            //
            fin >> a;
            assert(a == "DecrThickness");
            fin >> c;
            Cus_DecrThickness = c;
            //
            fin >> a;
            assert(a == "EnvironmentTemperature");
            fin >> c;
            Cus_EnvironmentTemperature = c;
            //
            fin >> a;
            assert(a == "WaterTemperature");
            fin >> c;
            Cus_WaterStartTemperature = c;
            //
            fin >> a;
            assert(a == "WaterFlow");
            fin >> c;
            Cus_WaterStartVelocity = c;
            //
            {
                fin >> a;
                assert(a == "BaseMaterial");
                double temp1, temp2, temp3;
                fin >> temp1 >> temp2 >> temp3;
                matphysic mat(temp1, temp2 * 1e-9, temp3 * 1e-3);
                Cus_Base_Mat = mat;
            }
            //
            {
                fin >> a;
                assert(a == "DecorateMaterial");
                double temp1, temp2, temp3;
                fin >> temp1 >> temp2 >> temp3;
                matphysic mat(temp1, temp2 * 1e-9, temp3 * 1e-3);
                Cus_Decr_Mat = mat;
            }
            //
            Cus_A.Load();
        }
        else if (a == "ForbidDecorate")
        {
            Cus_DecorateLocked = true;
            //
            fin >> a;
            assert(a == "CoeWB");
            fin >> c;
            Cus_WaterToBase = c * 1e-6;
            //
            fin >> a;
            assert(a == "CoeBA");
            fin >> c;
            Cus_BaseToCeil = c * 1e-6;
            //
            fin >> a;
            assert(a == "BaseThickness");
            fin >> c;
            Cus_BaseThickness = c;
            //
            fin >> a;
            assert(a == "EnvironmentTemperature");
            fin >> c;
            Cus_EnvironmentTemperature = c;
            //
            fin >> a;
            assert(a == "WaterTemperature");
            fin >> c;
            Cus_WaterStartTemperature = c;
            //
            fin >> a;
            assert(a == "WaterFlow");
            fin >> c;
            Cus_WaterStartVelocity = c;
            //
            {
                fin >> a;
                assert(a == "BaseMaterial");
                double temp1, temp2, temp3;
                fin >> temp1 >> temp2 >> temp3;
                matphysic mat(temp1, temp2 * 1e-9, temp3 * 1e-3);
                Cus_Base_Mat = mat;
            }
            //
            {
                fin >> a;
                assert(a == "DecorateMaterial");
                double temp1, temp2, temp3;
                fin >> temp1 >> temp2 >> temp3;
                matphysic mat(temp1, temp2 * 1e-9, temp3 * 1e-3);
                Cus_Decr_Mat = mat;
            }
            fin.close();
        }
        else
        {
            cerr << "Starter file format error" << endl;
            exit(0);
        }
    }
    else
    {
        cerr << "Cannot open Starter file" << endl;
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    //Logs;
    freopen("log.txt", "w", stderr);
    std::cerr << fixed << setprecision(8);
    //Load
    LoadCustomizedSettings();
    //PreCompute;
    HeatMesh mesh;
    meshstarter t = GetStarter();
    mesh.PreCompute(&t);
    mesh.LoadCheckPoints();
    //Monitor;
    ResetToCustomized();
    //Main;
    while (1)
    {
        
        //each step compute 0.2 seconds, or 0.2/
        int order = ReceiveOrder(&mesh);
        if (order == 'k')
        {
            break;
        }
        else if (order == 'd')
        {
            sleep(1);
            continue;
        }
        else if (order == 5)
        {
            mesh.ForwardIteration(0.2, order, Current_Temperature);
            mesh.OutputMesh();
        }
        else if (order == 6)
        {
            mesh.ForwardIteration(0.2, order, Current_velocity);
            mesh.OutputMesh();
        }
        else
        {
            mesh.ForwardIteration(0.2, order, 0);
            mesh.OutputMesh();
        }
        for(int i = 0; i < 4; i++){
            mesh.ForwardIteration(0.2, 0, 0);
            mesh.OutputMesh();
        }
    }
    while (access("TempFile/Temper", F_OK) != -1)
    {
        remove("TempFile/Temper");
    }
    return 0;
}