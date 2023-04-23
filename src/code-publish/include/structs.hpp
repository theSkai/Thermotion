#ifndef __STRUCTS_HPP__
#define __STRUCTS_HPP__
#include <string>
using namespace std;

struct matphysic
{
    matphysic(double heatcapacity, double density, double heatcoefficient)
    {
        heatCapacity = heatcapacity;
        Density = density;
        heatCoefficient = heatcoefficient;
        heatCapvolume = heatcapacity * density;
        thermalDiffusivity = heatCoefficient / heatCapvolume;
    }
    double heatCapacity;    //J/(kg * K)
    double Density;         //kg/m^3 kg/mm^3
    double heatCapvolume;   //
    double heatCoefficient; //J/(m * s * K) J/mm*s*K
    double thermalDiffusivity;
};
struct vercomplex
{
    double complex1 = 0; //heat conduction per step per temperature difference from water to base material

    double complex3 = 0; //heat conduction per step per temperature difference from base material to decorate material

    double complex5 = 0; //coefficient of temperature change of base material caused by unit heat

    double complex6 = 0; //coefficient of temperature change of decorate material caused by unit heat

    double complex_base_air = 0;

    double complex_decr_air = 0;
};
struct horcomplex
{
    double complex2 = 0; //heat conduction per step per temperature difference between small amounts of base material

    double complex4 = 0; //heat conduction per step per temperature difference between small amounts of decorate material
};
struct meshphysics
{
    meshphysics(){};
    meshphysics(const matphysic* b, const matphysic* d, double cwb, double cbd, double cba, double cda, double bthick, double dthick){
        basephysic = b;
        decrphysic = d;
        Coefficient4water2base = cwb;
        Coefficient4base2decr = cbd;
        Coefficient4base2ceil = cba;
        Coefficient4decr2ceil = cda;
        BaseThickness = bthick;
        DecrThickness = dthick;
    }
    const matphysic *basephysic;
    const matphysic *decrphysic;
    double Coefficient4water2base;
    double Coefficient4base2decr;
    double Coefficient4base2ceil;
    double Coefficient4decr2ceil;
    double BaseThickness;
    double DecrThickness;
};
struct meshstarter
{
    bool DecorateLocked;
    string MeshName;
    string WaterName;
    meshphysics Physics;
    Area* DecorateArea;
    double WaterStartTemperature;
    double WaterStartVelocity;
    double EnvironmentTemperature;
};

#endif