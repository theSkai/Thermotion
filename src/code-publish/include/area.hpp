#ifndef __AREA_HPP__
#define __AREA_HPP__

#include "Vector3f.h"
#include <vector>
using namespace std;

//Virtual base class
class Area{
public:
    virtual bool InArea(Vector3f a){
        return false;
    };
    virtual void OutputPhysics(){
        cerr << " Area: Null" << endl;
    };
};
//Not used, represent surface that in a box space
class BoxArea:public Area{
public:
    BoxArea(const Vector3f v1, const Vector3f v2){
        h.x() = max(v1.x(), v2.x());
        h.y() = max(v1.y(), v2.y());
        h.z() = max(v1.z(), v2.z());
        l.x() = min(v1.x(), v2.x());
        l.y() = min(v1.y(), v2.y());
        l.z() = min(v1.z(), v2.z());
    }
    bool InArea(Vector3f a) override{
        if(a.x() <= h.x() && a.x() >= l.x() && a.y() <= h.y() && a.y() >= l.y() && a.z() <= h.z() && a.z() >= l.z()) return true;
        else return false;
    }
    void OutputPhysics() override{
        cerr << " Area: BoxArea, ";
        cerr << "[" << l.x() << ":" << h.x() << "] ";
        cerr << "[" << l.y() << ":" << h.y() << "] ";
        cerr << "[" << l.z() << ":" << h.z() << "] ";
        cerr << endl;
    }
private:
    Vector3f h;
    Vector3f l;
};
//Not used, represent all surface
class TotalArea:public Area{
public:
    bool InArea(Vector3f a) override{
        return true;
    };
    void OutputPhysics(){
        cerr << " Area: Total Area" << endl;
    }
};
//Represent surface using a mesh
class MeshedArea:public Area{
    vector<Vector3f> Vertices;
public:
    MeshedArea(){
        Vertices.clear();
    }
    void Load(){
        ifstream fin;
        fin.open("TempFile/Decr");
        if(fin.is_open()){
            Vertices.clear();
            while(!fin.eof()){
                Vector3f a;
                fin >> a.x() >> a.y() >> a.z();
                Vertices.push_back(a);
            }
            fin.close();
        }
    }
    bool InArea(Vector3f a) override{
        for(int i = 0; i < Vertices.size(); i++){
            if((a - Vertices[i]).length() < 1e-4) return true;
        }
        return false;
    }
    void OutputPhysics() override{
        cerr << " Area: Meshed Area" << endl;
    }
};

#endif