#ifndef __COSTOMIZED_HPP__
#define __COSTOMIZED_HPP__
#include "structs.hpp"
#include "materials.hpp"
#include "area.hpp"
using namespace std;

bool Cus_DecorateLocked = true;
string Cus_MeshName = "TempFile/Mesh";
string Cus_WaterName = "TempFile/Water";
matphysic *Cus_Base = &Cus_Base_Mat;
matphysic *Cus_Decorate = &Cus_Decr_Mat; //if DecorateLocked == false, this will not be used
double Cus_WaterToBase = 5e-5;              // W / mm^2 * K
double Cus_BaseToDecorate = 2e-3;           // W / mm^2 * K
double Cus_BaseToCeil = 5e-6;               // W / mm^2 * K
double Cus_DecrToCeil = 5e-6;               // W / mm^2 * K
double Cus_BaseThickness = 0.5;             //mm
double Cus_DecrThickness = 0.05;            //mm
double Cus_EnvironmentTemperature = 26.1;   //
double Cus_WaterStartTemperature = 60;      //
double Cus_WaterStartVelocity = 4444;
MeshedArea Cus_A;
meshstarter GetStarter()
{
    meshstarter S;
    S.DecorateLocked = Cus_DecorateLocked;
    S.DecorateArea = &Cus_A;
    S.MeshName = Cus_MeshName;
    S.WaterName = Cus_WaterName;
    meshphysics P(Cus_Base, Cus_Decorate, Cus_WaterToBase, Cus_BaseToDecorate, Cus_BaseToCeil, Cus_DecrToCeil, Cus_BaseThickness, Cus_DecrThickness);
    S.Physics = P;
    S.EnvironmentTemperature = Cus_EnvironmentTemperature;
    S.WaterStartTemperature = Cus_WaterStartTemperature;
    S.WaterStartVelocity = Cus_WaterStartVelocity;
    return S;
}
#endif