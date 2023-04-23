#ifndef __MESH__
#define __MESH__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include "Vector3f.h"
#include "Vector4f.h"
#include "connectionmap.hpp"
#include "kdtreemap.hpp"
#include "watermanager.hpp"
#include "area.hpp"
#include "structs.hpp"
#include "materials.hpp"

using namespace std;
class HeatMesh
{
    struct triangle
    {
        int i[3];
    };

public:
    HeatMesh() {
        FOUT.open("Checkpoints.txt");
        if(!FOUT.is_open()){
            cerr << "Something error when opening checkpoints file" << endl;
            exit(0);
        }
    }
    ~HeatMesh() {
        FOUT.close();
    }
    void SayTime(){
        cerr << "at: " << current_stepnum * timestep << endl;
    }
    void PreCompute(meshstarter *s)
    {
        starter = s;
        InitializeData();
        SetWater();
        ConstructMesh();
        ReconnectMesh();
        TreeManageMesh();
        ComputeGeometry();
        LoadPhysics();
        LoadBoundaryConditions();
    }
    void ForwardIteration(double seconds, int executenum, double executeparam)
    {
        //Only one instruction is allowed in one iteration step
        double time = timestep * current_stepnum;
        double endtime = time + seconds;
        double waterwidth;
        Vector3f position;
        bool first = true;
        while (time < endtime)
        {
            //iterate water;
            {
                int ret = Water.evostep();
                if (first)
                    Execute(executenum, executeparam);
                Water.getallheads(ret);
                for (auto k = Water.returnmark.begin(); k != Water.returnmark.end(); k++)
                {
                    Water.convert(k->stage, waterwidth, position);
                    MeshKdTree.SearchSphere(waterwidth / 2, position);
                    for (int w = 0; w < MeshKdTree.buffer.size(); w++)
                    {
                        WaterHeat[MeshKdTree.buffer[w]] = k->temperature;
                    }
                }
            }
            //iterate heat distribution;
            {
                double heat1 = 0;
                double heat2 = 0;
                if (current_side == 1)
                {
                    for (int i = 1; i < MeshVertices.size(); i++)
                    {
                        //vertical
                        heat1 = WaterHeat[i] < -273 ? 0 : VerticesComplexes[i].complex1 * (WaterHeat[i] - MeshHeat[i][1]);
                        if (VerticesDecorated[i])
                        {
                            double conv = VerticesComplexes[i].complex3 * (MeshHeat[i][1] - MeshHeat[i][3]);
                            heat2 = conv - VerticesComplexes[i].complex_decr_air * (MeshHeat[i][3] - starter->EnvironmentTemperature);
                            heat1 += conv;
                        }
                        else
                        {
                            heat1 -= VerticesComplexes[i].complex_base_air * (MeshHeat[i][1] - starter->EnvironmentTemperature);
                        }
                        //horizontial
                        if (VerticesDecorated[i])
                        {
                            for (int j = 0; j < MeshConnection.connectionmap[i].size(); j++)
                            {
                                heat1 += MeshContactComplexes[i][j].complex2 == 0 ? 0 : MeshContactComplexes[i][j].complex2 * (MeshHeat[MeshConnection.connectionmap[i][j].INDEX_Mesh][1] - MeshHeat[i][1]);
                                heat2 += MeshContactComplexes[i][j].complex4 == 0 ? 0 : MeshContactComplexes[i][j].complex4 * (MeshHeat[MeshConnection.connectionmap[i][j].INDEX_Mesh][3] - MeshHeat[i][3]);
                            }
                        }
                        else
                        {
                            for (int j = 0; j < MeshConnection.connectionmap[i].size(); j++)
                            {
                                heat1 += MeshContactComplexes[i][j].complex2 == 0 ? 0 : MeshContactComplexes[i][j].complex2 * (MeshHeat[MeshConnection.connectionmap[i][j].INDEX_Mesh][1] - MeshHeat[i][1]);
                            }
                        }
                        MeshHeat[i][0] = MeshHeat[i][1] + (heat1 / VerticesComplexes[i].complex5);
                        MeshHeat[i][2] = MeshHeat[i][3] + (heat2 / VerticesComplexes[i].complex6);
                    }
                    current_side = !current_side;
                }
                else
                {
                    for (int i = 1; i < MeshVertices.size(); i++)
                    {
                        //vertical
                        heat1 = WaterHeat[i] < -273 ? 0 : VerticesComplexes[i].complex1 * (WaterHeat[i] - MeshHeat[i][0]);
                        if (VerticesDecorated[i])
                        {
                            double conv = VerticesComplexes[i].complex3 * (MeshHeat[i][0] - MeshHeat[i][2]);
                            heat2 = conv - VerticesComplexes[i].complex_decr_air * (MeshHeat[i][2] - starter->EnvironmentTemperature);
                            heat1 -= conv;
                        }
                        else
                        {
                            heat1 -= VerticesComplexes[i].complex_base_air * (MeshHeat[i][0] - starter->EnvironmentTemperature);
                        }
                        //horizontial
                        if (VerticesDecorated[i])
                        {
                            for (int j = 0; j < MeshConnection.connectionmap[i].size(); j++)
                            {
                                heat1 += MeshContactComplexes[i][j].complex2 == 0 ? 0 : MeshContactComplexes[i][j].complex2 * (MeshHeat[MeshConnection.connectionmap[i][j].INDEX_Mesh][0] - MeshHeat[i][0]);
                                heat2 += MeshContactComplexes[i][j].complex4 == 0 ? 0 : MeshContactComplexes[i][j].complex4 * (MeshHeat[MeshConnection.connectionmap[i][j].INDEX_Mesh][2] - MeshHeat[i][2]);
                            }
                        }
                        else
                        {
                            for (int j = 0; j < MeshConnection.connectionmap[i].size(); j++)
                            {
                                heat1 += MeshContactComplexes[i][j].complex2 == 0 ? 0 : MeshContactComplexes[i][j].complex2 * (MeshHeat[MeshConnection.connectionmap[i][j].INDEX_Mesh][0] - MeshHeat[i][0]);
                            }
                        }
                        MeshHeat[i][1] = MeshHeat[i][0] + (heat1 / VerticesComplexes[i].complex5);
                        MeshHeat[i][3] = MeshHeat[i][2] + (heat2 / VerticesComplexes[i].complex6);
                    }
                    current_side = !current_side;
                }
            }
            current_stepnum++;
            output(time);
            time += timestep;
            first = false;
        }
    }
    void LoadCheckPoints(){
        ifstream fin;
        fin.open("TempFile/CheckPoints");
        ChkPoints.clear();
        if(!fin.is_open()) return;
        while(1){
            int a;
            fin >> a;
            if(fin.eof()) break;
            ChkPoints.push_back(a);
        }
        fin.close();
    }
    void OutputMesh()
    {
        output_temperature("TempFile/Temper", timestep * current_stepnum);
    }
private:
    void Execute(int num, double param)
    {
        Water.execute(num, param);
        if (num == 1)
        {
            MeshHeat.clear();
            MeshHeat.resize(MeshVertices.size(), Vector4f(starter->EnvironmentTemperature));
            WaterHeat.clear();
            WaterHeat.resize(MeshVertices.size(), -274);
        }
    }
    //starter
    meshstarter *starter;
    //base data
    vector<Vector3f> MeshVertices;  //vertices, 0 is not used
    vector<triangle> MeshTriangles; //triangles
    //geometry data and data for algorithm
    ConnectionMap MeshConnection;                    //connectionmap[i][j].Index_Mesh
    KdTreeMap MeshKdTree;                            //Kd tree map
    vector<vector<double>> MeshContactLength;        //Contact Length
    vector<vector<double>> MeshBaseContactArea;      //Base Contact Area
    vector<vector<double>> MeshDecrContactArea;      //Decorate Contact Area
    vector<vector<double>> MeshLaplacianCoefficient; //(cotα + cotβ)/2 for inner edges and 0 for boundary
    vector<double> MeshArea;                         //Area size
    vector<double> LaplacianArea;
    vector<vector<Vector3f>> MeshContactNormal;      //Contact normal (outer direction)
    vector<vector<Vector3f>> MeshContactParallel;    //Contact boundary vector
    vector<double> MeshBaseVolume;                   //Base volume
    vector<double> MeshDecrVolume;                   //Decorate volume
    //physics
    double basethickness;                            //basethickness
    double decoratethickness;                        //decoratethickness
    vector<int> VerticesDecorated;                   //mark the physics of each vertice
    vector<vercomplex> VerticesComplexes;            //record the complexes value of each vertice
    vector<vector<horcomplex>> MeshContactComplexes; //record the complexes value of each edge
    WaterManager Water;                              //water controller
    //heat compute data
    vector<Vector4f> MeshHeat;    //0 for baseheat1, 1 for baseheat2, 2 for decheat1, 3 for decheat2
    vector<double> WaterHeat;     //heat of water layer
    vector<double> AirHeat;       //heat of Air layer
    bool decr_locked;             //whether if there's decorate
    bool current_side;            //current side of MeshHeat(0 for [0] and [2], 1 for [1] and [3])
    int current_stepnum;          //time count
    const double timestep = 1e-3; //time step
    const int _count = 1;         //the timestep count per iteration of heat
    const int water_count = 1;    //the timestep count per iteration of water
    double get_temperature(int index)
    {
        if (VerticesDecorated[index])
            return current_side ? MeshHeat[index][3] : MeshHeat[index][2];
        else
            return current_side ? MeshHeat[index][1] : MeshHeat[index][0];
    }
    ofstream FOUT;
    vector<int> ChkPoints;
    //load (precompute)
    void InitializeData()
    {
        current_stepnum = 0;
        current_side = 0;
        decr_locked = starter->DecorateLocked;
    }
    void SetWater()
    {
        Water.LoadWaterManager(starter->WaterName);
        Water.SetStartConditions(starter->WaterStartTemperature, starter->WaterStartVelocity, water_count * timestep);
    }
    void ConstructMesh()
    {
        ifstream fin;
        MeshVertices.resize(1); //0 not use
        MeshTriangles.clear();
        fin.open(starter->MeshName, ios::in);
        string line;
        string tok;
        while (true)
        {
            getline(fin, line);
            if (fin.eof())
                break;
            if (line.at(0) == '#')
                continue;
            stringstream ss(line);
            ss >> tok;
            if (tok == "v")
            {
                Vector3f temp;
                ss >> temp[0] >> temp[1] >> temp[2];
                MeshVertices.push_back(temp);
            }
            else if (tok == "vn")
            {
                continue;
            }
            else if (tok == "vt")
            {
                continue;
            }
            else if (tok == "f")
            {
                triangle temp;
                ss >> temp.i[0] >> temp.i[1] >> temp.i[2];
                MeshTriangles.push_back(temp);
            }
        }
        fin.close();
    }
    void ReconnectMesh()
    {
        MeshConnection.Clear();
        for (int i = 0; i < MeshTriangles.size(); i++)
        {
            MeshConnection.AddConnection(
                MeshTriangles[i].i[0],
                MeshTriangles[i].i[1],
                MeshTriangles[i].i[2]);
        }
        MeshConnection.Rearrange();
    }
    void TreeManageMesh()
    {
        MeshKdTree.ClearTree();
        for (int i = 1; i < MeshVertices.size(); i++)
        {
            MeshKdTree.AddPoint(i, MeshVertices[i]);
        }
    }
    void ComputeGeometry()
    {
        //explode thickness from starter
        {
            basethickness = starter->Physics.BaseThickness;
            decoratethickness = starter->Physics.DecrThickness;
        }
        //Compute Contact length and parallel vector
        {
            MeshContactLength.clear();
            MeshContactParallel.clear();
            MeshContactLength.resize(MeshVertices.size());
            MeshContactParallel.resize(MeshVertices.size());

            for (int i = 1; i < MeshContactLength.size(); i++)
            {
                MeshContactLength[i].clear();
                MeshContactParallel[i].clear();
                int N = MeshConnection.connectionmap[i].size() - 1;
                if (MeshConnection.IsFull(i))
                {
                    //compute [N,0,1]
                    MeshContactLength[i].push_back(
                        ((MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh]) / 3.0).length());
                    MeshContactParallel[i].push_back(
                        (MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh]) / 3.0);
                    for (int j = 1; j < N; j++)
                    {
                        //compute [0,1,2], [1,2,3] to [N-2,N-1,N]
                        MeshContactLength[i].push_back(
                            ((MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh]) / 3.0).length());
                        MeshContactParallel[i].push_back(
                            (MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh]) / 3.0);
                    }
                    //compute[N-1,N,0]
                    MeshContactLength[i].push_back(
                        ((MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh]) / 3.0).length());
                    MeshContactParallel[i].push_back(
                        (MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh]) / 3.0);
                }
                else
                {
                    MeshContactLength[i].push_back(
                        ((MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] + MeshVertices[i]) / 2.0 -
                         (MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] + MeshVertices[i]) / 3.0)
                            .length());
                    MeshContactParallel[i].push_back(
                        (MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] + MeshVertices[i]) / 2.0 -
                        (MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] + MeshVertices[i]) / 3.0);
                    for (int j = 1; j < N; j++)
                    {
                        //compute [0,1,2], [1,2,3] to [N-2,N-1,N]
                        MeshContactLength[i].push_back(
                            ((MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh]) / 3.0).length());
                        MeshContactParallel[i].push_back(
                            (MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh]) / 3.0);
                    }
                    MeshContactLength[i].push_back(
                        ((MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] + MeshVertices[i]) / 2.0 -
                         (MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] + MeshVertices[i]) / 3.0)
                            .length());
                    MeshContactParallel[i].push_back(
                        (MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] + MeshVertices[i]) / 2.0 -
                        (MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] + MeshVertices[i]) / 3.0);
                    //two fix edge
                    MeshContactParallel[i].push_back(
                        (MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[i]) / 2);
                    MeshContactLength[i].push_back(
                        ((MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[i]) / 2).length());
                    MeshContactParallel[i].push_back(
                        (MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[i]) / 2);
                    MeshContactLength[i].push_back(
                        ((MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[i]) / 2).length());
                }
            }
        }
        //Compute bottom Area size
        {
            MeshArea.clear();
            MeshArea.resize(MeshVertices.size());
            for (int i = 1; i < MeshArea.size(); i++)
            {
                int N = MeshConnection.connectionmap[i].size() - 1;
                if (MeshConnection.IsFull(i))
                {
                    //compute [N,0,1]
                    MeshArea[i] = 0;
                    MeshArea[i] +=
                        Vector3f::cross(
                            (MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0,
                            (MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0)
                            .length() /
                        2;
                    for (int j = 1; j < N; j++)
                    {
                        //compute [0,1,2], [1,2,3] to [N-2,N-1,N]
                        MeshArea[i] +=
                            Vector3f::cross(
                                (MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0,
                                (MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0)
                                .length() /
                            2;
                    }
                    //compute[N-1,N,0]
                    MeshArea[i] +=
                        Vector3f::cross(
                            (MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0,
                            (MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0)
                            .length() /
                        2;
                }
                else
                {
                    MeshArea[i] = 0;
                    MeshArea[i] +=
                        Vector3f::cross(
                            (MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[i]) / 2.0,
                            (MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0)
                            .length() /
                        2;
                    for (int j = 1; j < N; j++)
                    {
                        //compute [0,1,2], [1,2,3] to [N-2,N-1,N]
                        MeshArea[i] +=
                            Vector3f::cross(
                                (MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0,
                                (MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0)
                                .length() /
                            2;
                    }
                    //compute[N-1,N,0]
                    MeshArea[i] +=
                        Vector3f::cross(
                            (MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - 2 * MeshVertices[i]) / 3.0,
                            (MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[i]) / 2.0)
                            .length() /
                        2;
                }
            }
        }
        //Compute MeshLaplacian size
        {
            LaplacianArea.clear();
            LaplacianArea.resize(MeshVertices.size());
            for(int i = 1; i < LaplacianArea.size(); i++)
            {
                int N = MeshConnection.connectionmap[i].size() - 1;
                if(MeshConnection.IsFull(i)){
                    //compute [N,0]
                    LaplacianArea[i] = 0;
                    LaplacianArea[i] +=
                        Vector3f::cross(
                            MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[i],
                            MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[i])
                            .length() /
                        6;
                    for (int j = 1; j <= N; j++)
                    {
                        //compute [0,1], [1,2] to [N-2,N-1]
                        LaplacianArea[i] +=
                            Vector3f::cross(
                                MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[i],
                                MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh] - MeshVertices[i])
                                .length() /
                            6;
                    }
                }
                else{
                    LaplacianArea[i] = 0;
                    for(int j = 1; j <= N; j++){
                        LaplacianArea[i] +=
                            Vector3f::cross(
                                MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[i],
                                MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh] - MeshVertices[i])
                                .length() /
                            6;
                    }
                }
            }
        }
        //Compute Contact normal
        {
            MeshContactNormal.clear();
            MeshContactNormal.resize(MeshVertices.size());
            for (int i = 1; i < MeshContactNormal.size(); i++)
            {
                MeshContactNormal[i].clear();
                int N = MeshConnection.connectionmap[i].size() - 1;
                Vector3f normal;
                Vector3f parallel;
                Vector3f vertical;
                //[N,0,1]
                parallel = MeshConnection.IsFull(i) ? (MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh]) : ((MeshVertices[i] + MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh]) / 2.0 - (MeshVertices[i] + MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh]) / 3.0);
                vertical = Vector3f::cross(parallel, MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[i]);
                normal = Vector3f::cross(parallel, vertical).normalized();
                normal = Vector3f::dot(normal, MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[i]) > 0 ? normal : -normal;
                MeshContactNormal[i].push_back(normal);
                for (int j = 1; j < N; j++)
                {
                    parallel = MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh];
                    vertical = Vector3f::cross(parallel, MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh] - MeshVertices[i]);
                    normal = Vector3f::cross(parallel, vertical).normalized();
                    normal = Vector3f::dot(normal, MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh] - MeshVertices[i]) > 0 ? normal : -normal;
                    MeshContactNormal[i].push_back(normal);
                }
                parallel = MeshConnection.IsFull(i) ? (MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh]) : ((MeshVertices[i] + MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh]) / 2.0 - (MeshVertices[i] + MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] + MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh]) / 3.0);
                vertical = Vector3f::cross(parallel, MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[i]);
                normal = Vector3f::cross(parallel, vertical).normalized();
                normal = Vector3f::dot(normal, MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[i]) > 0 ? normal : -normal;
                MeshContactNormal[i].push_back(normal);

                if (!MeshConnection.IsFull(i))
                {
                    parallel = MeshContactParallel[i][N + 1];
                    normal = Vector3f::cross(parallel, vertical).normalized();
                    normal = Vector3f::dot(normal, (MeshVertices[i] - MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh])) > 0 ? normal : -normal;
                    MeshContactNormal[i].push_back(normal);

                    parallel = MeshContactParallel[i][N + 2];
                    normal = Vector3f::cross(parallel, vertical).normalized();
                    normal = Vector3f::dot(normal, (MeshVertices[i] - MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh])) > 0 ? normal : -normal;
                    MeshContactNormal[i].push_back(normal);
                }
            }
        }
        //Compute Contact Area size
        {
            MeshBaseContactArea.clear();
            MeshDecrContactArea.clear();
            MeshBaseContactArea.resize(MeshVertices.size());
            MeshDecrContactArea.resize(MeshVertices.size());
            for (int i = 1; i < MeshVertices.size(); i++)
            {
                MeshBaseContactArea[i].clear();
                MeshDecrContactArea[i].clear();
                for (int j = 0; j < MeshConnection.connectionmap[i].size(); j++)
                {
                    MeshBaseContactArea[i].push_back(MeshContactLength[i][j] * basethickness);
                    MeshDecrContactArea[i].push_back(MeshContactLength[i][j] * decoratethickness);
                }
            }
        }
        //Compute Volume
        {
            MeshBaseVolume.clear();
            MeshDecrVolume.clear();
            MeshBaseVolume.resize(MeshVertices.size());
            MeshDecrVolume.resize(MeshVertices.size());
            for (int i = 1; i < MeshBaseVolume.size(); i++)
            {
                MeshBaseVolume[i] = MeshArea[i] * basethickness;
                MeshDecrVolume[i] = MeshArea[i] * decoratethickness;
            }
        }
        //Compute Laplacian coefficient
        {
            MeshLaplacianCoefficient.clear();
            MeshLaplacianCoefficient.resize(MeshVertices.size());
            for (int i = 1; i < MeshVertices.size(); i++)
            {
                MeshLaplacianCoefficient[i].clear();
                if (!MeshConnection.IsFull(i))
                {
                    int N = MeshConnection.connectionmap[i].size() - 1;
                    double ef;
                    Vector3f v1u;
                    Vector3f v1d;
                    Vector3f v2u;
                    Vector3f v2d;
                    //try
                    v1u = MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] - MeshVertices[i];
                    v1d = MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh];
                    v2u = MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] - MeshVertices[i];
                    v2d = MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh];
                    ef = (Vector3f::dot(v1u, v1d) / (Vector3f::cross(v1u, v1d).length()) + Vector3f::dot(v2u, v2d) / (Vector3f::cross(v2u, v2d).length())) / 4;
                    MeshLaplacianCoefficient[i].push_back(ef);
                    for (int j = 1; j < N; j++)
                    {
                        v1u = MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[i];
                        v1d = MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh];
                        v2u = MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh] - MeshVertices[i];
                        v2d = MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh];
                        ef = (Vector3f::dot(v1u, v1d) / (Vector3f::cross(v1u, v1d).length()) + Vector3f::dot(v2u, v2d) / (Vector3f::cross(v2u, v2d).length())) / 2;
                        MeshLaplacianCoefficient[i].push_back(ef);
                    }
                    v1u = MeshVertices[MeshConnection.connectionmap[i][N-1].INDEX_Mesh] - MeshVertices[i];
                    v1d = MeshVertices[MeshConnection.connectionmap[i][N-1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh];
                    v2u = MeshVertices[MeshConnection.connectionmap[i][N-1].INDEX_Mesh] - MeshVertices[i];
                    v2d = MeshVertices[MeshConnection.connectionmap[i][N-1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh];
                    ef = (Vector3f::dot(v1u, v1d) / (Vector3f::cross(v1u, v1d).length()) + Vector3f::dot(v2u, v2d) / (Vector3f::cross(v2u, v2d).length())) / 4;
                    MeshLaplacianCoefficient[i].push_back(ef);
                }
                else
                {
                    int N = MeshConnection.connectionmap[i].size() - 1;
                    double ef;
                    Vector3f v1u;
                    Vector3f v1d;
                    Vector3f v2u;
                    Vector3f v2d;
                    //N 0 1;
                    v1u = MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[i];
                    v1d = MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh];
                    v2u = MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] - MeshVertices[i];
                    v2d = MeshVertices[MeshConnection.connectionmap[i][1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh];
                    ef = (Vector3f::dot(v1u, v1d) / (Vector3f::cross(v1u, v1d).length()) + Vector3f::dot(v2u, v2d) / (Vector3f::cross(v2u, v2d).length())) / 2;
                    MeshLaplacianCoefficient[i].push_back(ef);
                    for (int j = 1; j < N; j++)
                    {
                        v1u = MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[i];
                        v1d = MeshVertices[MeshConnection.connectionmap[i][j - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh];
                        v2u = MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh] - MeshVertices[i];
                        v2d = MeshVertices[MeshConnection.connectionmap[i][j + 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][j].INDEX_Mesh];
                        ef = (Vector3f::dot(v1u, v1d) / (Vector3f::cross(v1u, v1d).length()) + Vector3f::dot(v2u, v2d) / (Vector3f::cross(v2u, v2d).length())) / 2;
                        MeshLaplacianCoefficient[i].push_back(ef);
                    }
                    //N-1 N 0
                    v1u = MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] - MeshVertices[i];
                    v1d = MeshVertices[MeshConnection.connectionmap[i][N - 1].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh];
                    v2u = MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[i];
                    v2d = MeshVertices[MeshConnection.connectionmap[i][0].INDEX_Mesh] - MeshVertices[MeshConnection.connectionmap[i][N].INDEX_Mesh];
                    ef = (Vector3f::dot(v1u, v1d) / (Vector3f::cross(v1u, v1d).length()) + Vector3f::dot(v2u, v2d) / (Vector3f::cross(v2u, v2d).length())) / 2;
                    MeshLaplacianCoefficient[i].push_back(ef);
                }
            }
        }
    }
    void LoadPhysics()
    {
        //mark decorate layer and load physics
        {
            Area *a = starter->DecorateArea;
            VerticesDecorated.resize(MeshVertices.size());
            for (int i = 1; i < MeshVertices.size(); i++)
            {
                if (!decr_locked && a->InArea(MeshVertices[i]))
                    VerticesDecorated[i] = true;
                else
                    VerticesDecorated[i] = false;
            }
        }
        //compute complexes
        {
            VerticesComplexes.resize(MeshVertices.size());
            MeshContactComplexes.resize(MeshConnection.connectionmap.size());
            for (int i = 1; i < MeshVertices.size(); i++)
            {
                MeshContactComplexes[i].resize(MeshConnection.connectionmap[i].size());
                //complex1 = water2base * S * Δt(base)
                VerticesComplexes[i].complex1 =
                    (starter->Physics).Coefficient4water2base *
                    MeshArea[i] *
                    timestep *
                    _count;
                // complex3 = base2decr * S * Δt(decr)
                if (VerticesDecorated[i])
                {
                    VerticesComplexes[i].complex3 =
                        (starter->Physics).Coefficient4base2decr *
                        MeshArea[i] *
                        timestep *
                        _count;
                }
                // complex5 = C * ρ * S * h
                VerticesComplexes[i].complex5 =
                    starter->Physics.basephysic->heatCapacity *
                    starter->Physics.basephysic->Density *
                    MeshBaseVolume[i];
                // complex6 =
                if (VerticesDecorated[i])
                {
                    VerticesComplexes[i].complex6 =
                        starter->Physics.decrphysic->heatCapacity *
                        starter->Physics.decrphysic->Density *
                        MeshDecrVolume[i];
                }
                VerticesComplexes[i].complex_base_air =
                    starter->Physics.Coefficient4base2ceil *
                    MeshArea[i] *
                    timestep *
                    _count;
                if (VerticesDecorated[i])
                {
                    VerticesComplexes[i].complex_decr_air =
                        starter->Physics.Coefficient4decr2ceil *
                        MeshArea[i] *
                        timestep *
                        _count;
                }
                //complex2 = baseCoefficient * base_thickness * contactlength * Δt(base)
                for (int j = 0; j < MeshConnection.connectionmap[i].size(); j++)
                {
                    MeshContactComplexes[i][j].complex2 =
                        starter->Physics.basephysic->heatCoefficient *
                        MeshLaplacianCoefficient[i][j] *
                        (MeshArea[i]/LaplacianArea[i]) *
                        basethickness *
                        _count *
                        timestep;
                    if (VerticesDecorated[i] &&
                        VerticesDecorated[MeshConnection.connectionmap[i][j].INDEX_Mesh])
                    {
                        MeshContactComplexes[i][j].complex4 =
                            starter->Physics.decrphysic->heatCoefficient *
                            MeshLaplacianCoefficient[i][j] *
                            (MeshArea[i]/LaplacianArea[i]) *
                            decoratethickness *
                            _count *
                            timestep;
                    }
                    else
                        MeshContactComplexes[i][j].complex4 = 0;
                }
            }
        }
    }
    void LoadBoundaryConditions()
    {
        double ENVIRONMENT_TEMPERATURE = starter->EnvironmentTemperature;
        MeshHeat.clear();
        MeshHeat.resize(MeshVertices.size(), Vector4f(ENVIRONMENT_TEMPERATURE));

        AirHeat.clear();
        AirHeat.resize(MeshVertices.size(), ENVIRONMENT_TEMPERATURE);

        WaterHeat.clear();
        WaterHeat.resize(MeshVertices.size(), -274);
    }
    //output
    int timecounter4output = 0;
    void output(double time)
    {
        if (time > timecounter4output * 0.5)
        {
            if (access("Tempers", 0) != 0)
            {
                mkdir("Tempers", 0755);
            }
            output_temperature("Tempers/Temper" + to_string(timecounter4output), time);
            for(int i = 0; i < ChkPoints.size(); i++){
                FOUT << get_temperature(ChkPoints[i]) << " ";
            }
            FOUT << endl;
            timecounter4output++;
        }
    }
    void output_temperature(string Outputname, double time)
    {
        ofstream fout;
        fout.open(Outputname);
        fout << time << endl;
        double maxtemp = -275;
        double mintemp = 1000;
        double x;
        for (int i = 1; i < MeshVertices.size(); i++)
        {
            x = get_temperature(i);
            fout << x << endl;
            mintemp = min(x, mintemp);
            maxtemp = max(x, maxtemp);
        }
        fout << maxtemp << endl;
        fout << mintemp << endl;
        fout.close();
    }
};
#endif