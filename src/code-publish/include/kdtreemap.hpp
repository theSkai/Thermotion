#ifndef __KD_TREE_MAP__
#define __KD_TREE_MAP__

#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>
#include "Vector3f.h"
using namespace std;

//A k-d tree structure, 3-d tree actually
class KdTreeMap
{
    struct node
    {
        node(int id, Vector3f pos, int dim)
        {
            INDEX_Mesh = id;
            position = pos;
            l_child = -2;
            r_child = -2;
            dvd_dimension = dim;
        }
        int INDEX_Mesh;
        Vector3f position;
        int l_child;
        int r_child;
        int dvd_dimension; //1 for x, 2 for y, 3 for z;
    };
public:    
    KdTreeMap() { kdtree.clear(); };

    void AddPoint(int Index_in_mesh, Vector3f position)
    {
        if (kdtree.size() == 0)
            kdtree.push_back(node(Index_in_mesh, position, 1));
        else
        {
            int doubleer = 0;
            while (doubleer < kdtree.size())
            {
                if(kdtree[doubleer].dvd_dimension == 1){
                    if(position.x() <= kdtree[doubleer].position.x()){
                        if(have_l_child(doubleer)) doubleer = kdtree[doubleer].l_child;
                        else{
                            kdtree[doubleer].l_child = kdtree.size();
                            kdtree.push_back(node(Index_in_mesh, position, 2));
                            return;
                        }
                    }
                    else{
                        if(have_r_child(doubleer)) doubleer = kdtree[doubleer].r_child;
                        else{
                            kdtree[doubleer].r_child = kdtree.size();
                            kdtree.push_back(node(Index_in_mesh, position, 2));
                            return;
                        }
                    }
                }
                else if(kdtree[doubleer].dvd_dimension == 2){
                    if(position.y() <= kdtree[doubleer].position.y()){
                        if(have_l_child(doubleer)) doubleer = kdtree[doubleer].l_child;
                        else{
                            kdtree[doubleer].l_child = kdtree.size();
                            kdtree.push_back(node(Index_in_mesh, position, 3));
                            return;
                        }
                    }
                    else{
                        if(have_r_child(doubleer)) doubleer = kdtree[doubleer].r_child;
                        else{
                            kdtree[doubleer].r_child = kdtree.size();
                            kdtree.push_back(node(Index_in_mesh, position, 3));
                            return;
                        }
                    }
                }
                else{
                    if(position.z() <= kdtree[doubleer].position.z()){
                        if(have_l_child(doubleer)) doubleer = kdtree[doubleer].l_child;
                        else{
                            kdtree[doubleer].l_child = kdtree.size();
                            kdtree.push_back(node(Index_in_mesh, position, 1));
                            return;
                        }
                    }
                    else{
                        if(have_r_child(doubleer)) doubleer = kdtree[doubleer].r_child;
                        else{
                            kdtree[doubleer].r_child = kdtree.size();
                            kdtree.push_back(node(Index_in_mesh, position, 1));
                            return;
                        }
                    }
                }
            }
        }
    }
    //search all points in a sphere, using a stack structure(std::list)
    bool SearchSphere(const double radius, const Vector3f position){
        if(kdtree.size() == 0) return false;
        buffer.clear();

        stack.clear();
        stack.push_back(0);
        while(stack.size() > 0){
            int point = *stack.begin();
            stack.pop_front();
            int ret = pointin(point, radius, position);
            if(ret == 0){
                buffer.push_back(kdtree[point].INDEX_Mesh);
                if(have_l_child(point)) stack.push_back(kdtree[point].l_child);
                if(have_r_child(point)) stack.push_back(kdtree[point].r_child);
            }
            else if(ret == -1 || ret == -2 || ret == -3){
                if(have_r_child(point)) stack.push_back(kdtree[point].r_child);
            }
            else if(ret == 1 || ret == 2 || ret == 3){
                if(have_l_child(point)) stack.push_back(kdtree[point].l_child);
            }
            else{
                if(have_l_child(point)) stack.push_back(kdtree[point].l_child);
                if(have_r_child(point)) stack.push_back(kdtree[point].r_child);
            }
        }
        return true;
    }
    void ClearTree(){
        kdtree.clear();
    }
    vector<int> buffer;//record the index in Mesh, not kdtree
private:
    vector<node> kdtree;
    list<int> stack;
    bool have_l_child(int index){return kdtree[index].l_child != -2;};
    bool have_r_child(int index){return kdtree[index].r_child != -2;};
        // 0 for in;; 1\2\3 for x\y\z higher, and -1\-2\-3 for lower;; 5 for possibly in;
    int pointin(int index, const double radius, Vector3f position){
        Vector3f rele = (kdtree[index].position - position);
        if(rele.length() < radius) return 0;
        else{
            if(kdtree[index].dvd_dimension == 1){
                if(rele.x() <= -radius){
                    return -1;
                }
                if(rele.x() >= radius){
                    return 1;
                }
            }
            else if(kdtree[index].dvd_dimension == 2){
                if(rele.y() <= -radius){
                    return -2;
                }
                if(rele.y() >= radius) return 2;
            }
            else if(kdtree[index].dvd_dimension == 3){
                if(rele.z() <= -radius){
                    return -3;
                }
                if(rele.z() >= radius){
                    return 3;
                }
            }
            return 5;
        }
    }
};

#endif