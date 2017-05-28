#pragma once
#include <vector>

class DSU
{
public:
    DSU(int n)
    {
        forest.reserve(n);
        for (int i=0; i<n; i++)
            forest.push_back(disjoint_set(i));
    }
    int find(int i)
    {
        if (forest[i].parent == i)
            return i;
        else
        {
            forest[i].parent = find(forest[i].parent);
            return forest[i].parent;
        }
    }
    void unite(int x, int y)
    {
        int root_x = find(x);
        int root_y = find(y);
        if (root_x != root_y)
        {
            if (forest[root_x].rank < forest[root_y].rank)
                forest[root_x].parent = root_y;
            else if (forest[root_x].rank > forest[root_y].rank)
                forest[root_y].parent = root_x;
            else
            {
                forest[root_x].parent = root_y;
                forest[root_y].rank += 1;
            }
        }
    }



    static int find(const std::vector<int> &parent, int i)
    {
        if (parent[i] == -1)
            return i;
        return find(parent, parent[i]);
    }

    static void Union( std::vector<int> &parent, int x, int y)
    {
        int xset = find(parent, x);
        int yset = find(parent, y);
        parent[xset] = yset;
    }

private:
    struct disjoint_set
    {
        int parent;
        unsigned rank;
        disjoint_set(int i) : parent(i), rank(0) { }
    };
    std::vector<disjoint_set> forest;
};


