#ifndef __CONNECTIONMAP__
#define __CONNECTIONMAP__

#include <vector>
#include <cstdlib>
#include <iostream>
using namespace std;

//Memorize all the connection in a mesh
class ConnectionMap
{
    struct node
    {
        node(int a){
            INDEX_Mesh = a;
            before = -2;
            next = -2;
        }
        int INDEX_Mesh;
        int before = -2;
        int next = -2;
    };

public:
    vector<vector<node>> connectionmap;
    ConnectionMap(){arranged = false; connectionmap.clear();};
    ~ConnectionMap(){};

    void AddConnection(const int a, const int b, const int c)
    {
        arranged = false;
        if (a <= 0 || b <= 0 || c <= 0)
        {
            exit(0);
        }
        addconnection(a, b, c);
        addconnection(b, c, a);
        addconnection(c, a, b);
    }
    void Clear()
    {
        connectionmap.clear();
        arranged = false;
    }
    void Rearrange(){
        for(int i = 1; i < connectionmap.size(); i++)
            rearrange(i);
        arranged = true;
    }
    bool IsFull(const int index){
        if(!arranged){
            exit(0);
        }
        return connectionmap[index][0].before != -2;
    }
private:
    bool arranged;
    bool issingle(const int now, const int index)
    {
        if (connectionmap[now][index].before == -2 && connectionmap[now][index].next == -2)
            return true;
        else
            return false;
    }
    void reverse(const int now, const int index)
    {
        //reverse next;
        int forwardindex = connectionmap[now][index].next;
        int backwardindex = index;
        
        while(forwardindex != -2){
            backwardindex = forwardindex;
            forwardindex = connectionmap[now][forwardindex].next;
            connectionmap[now][backwardindex].next = connectionmap[now][backwardindex].before;
            connectionmap[now][backwardindex].before = forwardindex;
        }

        forwardindex = connectionmap[now][index].before;
        backwardindex = index;

        while(forwardindex != -2){
            backwardindex = forwardindex;
            forwardindex = connectionmap[now][forwardindex].before;
            connectionmap[now][backwardindex].before = connectionmap[now][backwardindex].next;
            connectionmap[now][backwardindex].next = forwardindex;
        }

        int temp = connectionmap[now][index].next;
        connectionmap[now][index].next = connectionmap[now][index].before;
        connectionmap[now][index].before = temp;
    }
    void addconnection(const int now, const int S, const int T)
    {
        if(S < 0 || T < 0){
            exit(0);
        }
        if (connectionmap.size() <= now)
            connectionmap.resize(now + 1);
        int s_in = -1;
        int t_in = -1;
        for (int i = 0; i < connectionmap[now].size(); i++)
        {
            if (connectionmap[now][i].INDEX_Mesh == S)
                s_in = i;
            if (connectionmap[now][i].INDEX_Mesh == T)
                t_in = i;
        }

        if (s_in == -1)
        {
            s_in = connectionmap[now].size();
            connectionmap[now].push_back(node(S));
        }
        if (t_in == -1)
        {
            t_in = connectionmap[now].size();
            connectionmap[now].push_back(node(T));
        }

        if (issingle(now, s_in) && issingle(now, t_in))
        {
            connectionmap[now][s_in].next = t_in;
            connectionmap[now][t_in].before = s_in;
        }
        else if (!issingle(now, s_in) && issingle(now, t_in))
        {
            if (connectionmap[now][s_in].next == -2)
            {
                connectionmap[now][s_in].next = t_in;
                connectionmap[now][t_in].before = s_in;
            }
            else
            {
                connectionmap[now][s_in].before = t_in;
                connectionmap[now][t_in].next = s_in;
            }
        }
        else if (issingle(now, s_in) && !issingle(now, t_in))
        {
            if (connectionmap[now][t_in].next == -2)
            {
                connectionmap[now][t_in].next = s_in;
                connectionmap[now][s_in].before = t_in;
            }
            else
            {
                connectionmap[now][t_in].before = s_in;
                connectionmap[now][s_in].next = t_in;
            }
        }
        else
        {
            if (connectionmap[now][s_in].next == -2 && connectionmap[now][t_in].before == -2)
            {
                connectionmap[now][s_in].next = t_in;
                connectionmap[now][t_in].before = s_in;
            }
            else if (connectionmap[now][t_in].next == -2 && connectionmap[now][s_in].before == -2)
            {
                connectionmap[now][t_in].next = s_in;
                connectionmap[now][s_in].before = t_in;
            }
            else
            {
                reverse(now, t_in);
                if (connectionmap[now][s_in].next == -2 && connectionmap[now][t_in].before == -2)
                {
                    connectionmap[now][s_in].next = t_in;
                    connectionmap[now][t_in].before = s_in;
                }
                else if (connectionmap[now][t_in].next == -2 && connectionmap[now][s_in].before == -2)
                {
                    connectionmap[now][t_in].next = s_in;
                    connectionmap[now][s_in].before = t_in;
                }
            }
        }
    }
    void rearrange(const int now){
        int arrange_index = 0;
        bool full_marker = true;
        for(int i = 0; i < connectionmap[now].size(); i++){
            if(connectionmap[now][i].before == -2){
                arrange_index = i;
                full_marker = false;
                break;
            }
        }
        vector<node> temp;
        temp.clear();
        for(int i = 0; i < connectionmap[now].size(); i++){
            temp.push_back(connectionmap[now][arrange_index]);
            temp[i].before = 
                i == 0?
                    (full_marker?
                        connectionmap[now].size() - 1:-2)
                    :i;
            temp[i].next = 
                i == connectionmap[now].size() - 1?
                    (full_marker?
                        0:-2)
                    :i + 1;
            arrange_index = connectionmap[now][arrange_index].next;
        }
        connectionmap[now] = temp;
    }
};

#endif