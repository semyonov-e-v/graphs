#pragma once
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>
#include <iterator>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <limits>
#include <algorithm>
#include <set>
#include <stack>

#include "DSU.h"

typedef std::tuple<int,int,int> EdgeTuple;
typedef std::vector<EdgeTuple> SpainingTree;

std::vector<int> Split(const std::string& subject)
{
    std::istringstream ss{subject};
    using StrIt = std::istream_iterator<int>;
    std::vector<int> container{StrIt{ss}, StrIt{}};
    return container;
}

int minKey (const std::vector<int> &key,
            const std::vector<bool> &mstSet)
{
    int min = std::numeric_limits<int>::max(), min_index;
    int Size = key.size();

    for (int v = 0; v < Size; v++)
        if (mstSet[v] == false && key[v] < min)
            min = key[v], min_index = v;

    return min_index;
}

class View
{
public:
    View (char pt) :type(pt) {}
    virtual void Fill(std::ifstream &in) =0;
    virtual void addEdge(int from,int to, int weight) =0;
    virtual void removeEdge(int from,int to) =0;
    virtual int changeEdge(int from,int to,int newWeight)=0;
    virtual void Write(std::ostream &out) =0;
    virtual std::pair<SpainingTree,int> getSpainingTreePrima() =0;
    virtual std::pair<SpainingTree,int> getSpainingTreeKruscal() =0;
    virtual std::pair<SpainingTree,int> getSpainingTreeBoruvka() =0;
    virtual int CheckEuler(bool &circleExist) =0;
    virtual std::vector<int> getEuleranTourEffective() =0;
    virtual std::vector<int> getEuleranTourFleri() =0;
    virtual ~View () {}

    bool is_weighted;
    bool is_directed;
    char type;
};

typedef std::vector<int> IntVec;
typedef std::vector<IntVec> Matrix;

class AdjMatrix : public View
{
public:

    AdjMatrix() : View('C') {}

    virtual void Fill(std::ifstream &in)
    {
        std::size_t Size;
        in>>Size;
        in>>is_directed;
        in>>is_weighted;

        data = Matrix(Size,IntVec(Size,0));
        for(std::size_t i = 0; i<Size; i++)
            for(std::size_t j = 0; j<Size; j++)
                in>>data[i][j];
    }
    virtual void Write(std::ostream &out)
    {
        std::size_t Size = data.size();
        out<<Size<<' ';
        out<<is_directed<<'\n';
        out<<is_weighted<<'\n';
        for(std::size_t i = 0; i<Size; i++)
        {
            for(std::size_t j = 0; j<Size; j++)
                out<<data[i][j]<<' ';
            out<<'\n';
        }
    }

    virtual void addEdge(int from,int to, int weight)
    {
        data[from][to] = weight;
        data[to][from] = weight;
    }
    virtual void removeEdge(int from,int to)
    {
        data[from][to] = 0;
        data[to][from] = 0;
    }
    virtual int changeEdge(int from,int to,int newWeight)
    {
        auto oldWeight = data[from][to];
        data[from][to] = newWeight;
        data[to][from] = newWeight;
        return oldWeight;
    }
    virtual std::pair<SpainingTree,int> getSpainingTreePrima()
    {
        int V = data.size();
        std::vector<int> key(V,std::numeric_limits<int>::max());
        std::vector<int> mstSet(V,-1);

        SpainingTree R(V-1);
        std::set<std::pair<int,int>> q;
        q.insert (std::make_pair (0, 0));
        std::vector<bool> used(V,false);
        for (int i=0; i<V; ++i)
        {
            if (q.empty())
            {
                std::cout << "No MST!";
                exit(0);
            }
            int v = q.begin()->second;

            q.erase (q.begin());

            if (mstSet[v] != -1)
                used[v] = true;

            for (int j=0; j<V; ++j)
            {
                int cost = data[v][j];
                if (cost && !used[j] &&  cost < key[j])
                {
                    q.erase (std::make_pair(key[j], j));
                    key[j] = cost;
                    mstSet[j] = v;
                    q.insert (std::make_pair (key[j], j));
                }
            }
        }

        for (int i = 1; i < V; i++)
            R[i-1] = std::make_tuple(i, mstSet[i], key[i]);
        return std::make_pair(R,V);
    }

    virtual std::pair<SpainingTree,int> getSpainingTreeKruscal()
    {
        SpainingTree SortedEdges;
        int V = data.size();
        for(int i = 0; i<V; i++)
        {
            for(int j = 0; j<V; j++)
            {
                auto el = data[i][j];
                if(el)
                    SortedEdges.push_back(std::make_tuple(i,j,el));
            }
        }

        SpainingTree result(V-1);
        std::sort(SortedEdges.begin(),SortedEdges.end(),
                  [](EdgeTuple &a,EdgeTuple &b)
        {
            return std::get<2>(a)<std::get<2>(b);
        });
        DSU subsets = DSU(V);

        std::size_t j = 0;
        for(const auto &el : SortedEdges)
        {
            int x = subsets.find(std::get<0>(el));
            int y = subsets.find(std::get<1>(el));

            if (x != y)
            {
                result[j++]=el;
                subsets.unite(x, y);
            }
        }

        return std::make_pair(result,V);
    }
    virtual std::pair<SpainingTree,int> getSpainingTreeBoruvka()
    {
        SpainingTree edges;
        int V = data.size();
        for(int i = 0; i<V; i++)
        {
            for(int j = 0; j<V; j++)
            {
                auto el = data[i][j];
                if(el)
                    edges.push_back(std::make_tuple(i,j,el));
            }
        }

        SpainingTree result(V-1);
        std::sort(edges.begin(),edges.end(),
                  [](EdgeTuple &a,EdgeTuple &b)
        {
            return std::get<2>(a)<std::get<2>(b);
        });
        DSU subsets = DSU(V);

        int numTrees = V;
        auto index = 0;
        int E = edges.size();
        for (int t = 1; t < V && numTrees > 1; t = t + t)
        {
            std::vector<int> cheapest(V,-1);
            for (int i=0; i<E; i++)
            {
                int set1 = subsets.find(std::get<0>(edges[i]));
                int set2 = subsets.find(std::get<1>(edges[i]));
                if (set1 == set2)
                    continue;

                else
                {
                    if (cheapest[set1] == -1 ||
                            std::get<2>(edges[cheapest[set1]]) > std::get<2>(edges[i]))
                        cheapest[set1] = i;

                    if (cheapest[set2] == -1 ||
                            std::get<2>(edges[cheapest[set2]]) > std::get<2>(edges[i]))
                        cheapest[set2] = i;
                }
            }
            for (int i=0; i<V; i++)
            {
                // Check if cheapest for current set exists
                if (cheapest[i] != -1)
                {
                    int set1 = subsets.find(std::get<0>(edges[cheapest[i]]));
                    int set2 = subsets.find(std::get<1>(edges[cheapest[i]]));
                    if (set1 == set2)
                        continue;
                    //MSTweight += std::get<2>(edge[cheapest[i]]);
                    result[index++] = edges[cheapest[i]];
                    subsets.unite(set1,set2);
                    numTrees--;
                }
            }

        }
        return std::make_pair(result,V);
    }
    int CheckEuler(bool &circleExist)
    {
        //проверка на 2-ой критерий
        int NTops = data.size();
        std::vector<int> parent(NTops,-1);
        circleExist = false;
        for(int i=0; i<NTops; i++)
            for(int j=0; j<NTops; j++)
            {
                if(!data[i][j])
                    continue;
                int x = DSU::find(parent, i);
                int y = DSU::find(parent, j);

                if (x == y)
                {
                    circleExist = true;
                    break;
                }

                DSU::Union(parent, x, y);
            }
        if(!circleExist)
            return 0;

        int count = 0,top = 1;

        for(int i=0; i<NTops; i++)
        {
            auto temp = NTops-std::count(data[i].cbegin(),data[i].cend(),0);
            if(temp%2!=0)
            {
                top = i+1;
                count++;
            }
        }
        if(count>2)
            return 0;
        else if(count == 0)
            circleExist = true;
        else
            circleExist = false;

        return top;
    }
    std::vector<int> getEuleranTourFleri()
    {
        bool is_circle;
        auto v = CheckEuler(is_circle);
        if(!v)
            return std::vector<int> {};
        v--;
        std::vector<int> R;
        R.reserve(2*data.size());
        auto copy = data;
        R.push_back(v+1);
        proc(v,copy,R);
        return R;
    }
    std::vector<int> getEuleranTourEffective()
    {
        bool is_circle;
        auto v = CheckEuler(is_circle);
        if(!v)
            return std::vector<int> {};
        v--;
        std::vector<int> R;
        R.reserve(2*data.size());
        auto copy = data;
        std::stack<int> S;
        S.push(v);
        int Size = data.size();
        while (!S.empty())
        {
            auto w = S.top();
            for(int u=0; u<Size; u++)
                if(copy[w][u])
                {
                    S.push(u);
                    copy[w][u]=0;
                    if(!is_directed)
                        copy[u][w]=0;
                    break;
                }

            if(w==S.top())
            {
                S.pop();
                R.push_back(w+1);
            }
        }
        return R;
    }
    virtual ~AdjMatrix () {}

    Matrix data;

private:

    int DFSCount (int v, std::vector<bool> &visited,
                  const Matrix &copy)
    {
        visited[v] = true;
        int count = 1;

        for(auto j=0; j<int(copy.size()); j++)
            if (!visited[j])
                count += DFSCount(j, visited,copy);
        return count;
    }
    bool isValidNextEdge(int u, int v,Matrix &copy)
    {

        if((copy.size()-std::count(data[u].cbegin(),data[u].cend(),0))==1)
            return true;

        std::vector<bool> visited(copy.size(),false);
        int count1 = DFSCount(u, visited,copy);

        copy[u][v] = 0;
        if(!is_directed)
            copy[v][u] = 0;

        std::fill(visited.begin(),visited.end(),false);
        int count2 = DFSCount(u, visited,copy);
        copy[u][v] = 1;
        if(!is_directed)
            copy[v][u] = 1;

        return (count1 > count2)? false: true;
    }
    void proc (int u,Matrix &copy,std::vector<int> &R)
    {
        for(auto v=0; v<int(copy.size()); v++)
        {
            if(copy[u][v] && isValidNextEdge(u, v,copy) )
            {
                R.push_back(v+1);
                copy[u][v] = 0;
                if(!is_directed)
                    copy[v][u] = 0;
                proc(v,copy,R);
            }
        }
    }
};
typedef std::unordered_map<int,int> List;
typedef std::vector<List> Lists;
class AdjLists : public View
{
public:

    AdjLists() : View('L') {}
    virtual void Fill(std::ifstream &in)
    {
        std::size_t Size;
        in>>Size;
        in>>is_directed;
        in>>is_weighted;

        data = Lists(Size,List());
        int key;
        std::string str;
        std::getline(in,str);
        for(std::size_t i = 0; i<Size; i++)
        {
            std::getline(in,str);
            auto vec = Split(str);
            for(std::size_t j = 0; j<vec.size(); j++)
            {
                key = vec[j]-1;
                if(is_weighted)
                {
                    data[i][key] = vec[++j];
                }
                else
                    data[i][key] = 1;
            }
        }
    }
    virtual void Write(std::ostream &out)
    {
        std::size_t Size = data.size();
        out<<Size<<'\n';
        out<<is_directed<<' '<<is_weighted<<'\n';
        for(std::size_t i = 0; i<Size; i++)
        {
            for(const auto &el : data[i])
                if(is_weighted)
                    out<<el.first+1<<' '<<el.second<<' ';
                else
                    out<<el.first+1<<' ';
            out<<'\n';
        }
    }

    virtual void addEdge(int from,int to, int weight)
    {
        data[from][to] = weight;
        if(!is_directed)
            data[to][from] = weight;
    }
    virtual void removeEdge(int from,int to)
    {
        data[from].erase(to);
        if(!is_directed)
            data[to].erase(from);
    }
    virtual int changeEdge(int from,int to,int newWeight)
    {
        auto oldWeight = data[from][to];
        data[from][to] = newWeight;
        if(!is_directed)
            data[to][from] = newWeight;
        return oldWeight;
    }
    std::pair<SpainingTree,int> getSpainingTreePrima()
    {
        int V = data.size();
        std::vector<int> key(V,std::numeric_limits<int>::max());
        std::vector<int> mstSet(V,-1);

        SpainingTree R(V-1);
        std::set<std::pair<int,int>> q;
        q.insert (std::make_pair (0, 0));
        std::vector<bool> used(V,false);
        for (int i=0; i<V; ++i)
        {
            if (q.empty())
            {
                std::cout << "No MST!";
                exit(0);
            }
            int v = q.begin()->second;

            q.erase (q.begin());

            if (mstSet[v] != -1)
                used[v] = true;

            for (const auto &el : data[v])
            {
                int to = el.first;
                int cost = el.second;
                if (!used[to] &&  cost < key[to])
                {
                    q.erase (std::make_pair(key[to], to));
                    key[to] = cost;
                    mstSet[to] = v;
                    q.insert (std::make_pair (key[to], to));
                }
            }
        }

        for (int i = 1; i < V; i++)
            R[i-1] = std::make_tuple(i, mstSet[i], key[i]);

        return std::make_pair(R,V);
    }
    std::pair<SpainingTree,int> getSpainingTreeKruscal()
    {
        SpainingTree SortedEdges;
        int V = data.size();
        for(int i = 0; i<V; i++)
        {
            for(const auto &el : data[i])
                SortedEdges.push_back(std::make_tuple(i,el.first,el.second));
        }

        SpainingTree result(V-1);
        std::sort(SortedEdges.begin(),SortedEdges.end(),
                  [](EdgeTuple &a,EdgeTuple &b)
        {
            return std::get<2>(a)<std::get<2>(b);
        });
        DSU subsets = DSU(V);

        std::size_t j = 0;
        for(const auto &el : SortedEdges)
        {
            int x = subsets.find(std::get<0>(el));
            int y = subsets.find(std::get<1>(el));

            if (x != y)
            {
                result[j++]=el;
                subsets.unite(x, y);
            }
        }

        return std::make_pair(result,V);
    }
    std::pair<SpainingTree,int> getSpainingTreeBoruvka()
    {
        SpainingTree edges;
        int V = data.size();
        for(int i = 0; i<V; i++)
        {
            for(const auto &el : data[i])
                edges.push_back(std::make_tuple(i,el.first,el.second));
        }

        SpainingTree result(V-1);
        std::sort(edges.begin(),edges.end(),
                  [](EdgeTuple &a,EdgeTuple &b)
        {
            return std::get<2>(a)<std::get<2>(b);
        });
        DSU subsets = DSU(V);

        int numTrees = V;
        auto index = 0;
        int E = edges.size();
        for (int t = 1; t < V && numTrees > 1; t = t + t)
        {
            std::vector<int> cheapest(V,-1);
            for (int i=0; i<E; i++)
            {
                int set1 = subsets.find(std::get<0>(edges[i]));
                int set2 = subsets.find(std::get<1>(edges[i]));
                if (set1 == set2)
                    continue;

                else
                {
                    if (cheapest[set1] == -1 ||
                            std::get<2>(edges[cheapest[set1]]) > std::get<2>(edges[i]))
                        cheapest[set1] = i;

                    if (cheapest[set2] == -1 ||
                            std::get<2>(edges[cheapest[set2]]) > std::get<2>(edges[i]))
                        cheapest[set2] = i;
                }
            }
            for (int i=0; i<V; i++)
            {
                // Check if cheapest for current set exists
                if (cheapest[i] != -1)
                {
                    int set1 = subsets.find(std::get<0>(edges[cheapest[i]]));
                    int set2 = subsets.find(std::get<1>(edges[cheapest[i]]));
                    if (set1 == set2)
                        continue;
                    //MSTweight += std::get<2>(edge[cheapest[i]]);
                    result[index++] = edges[cheapest[i]];
                    subsets.unite(set1,set2);
                    numTrees--;
                }
            }

        }
        return std::make_pair(result,V);
    }
    int CheckEuler(bool &circleExist)
    {
        //проверка на 2-ой критерий
        int NTops = data.size();
        std::vector<int> parent(NTops,-1);
        circleExist = false;
        for(int i=0; i<NTops; i++)
            for(const auto &el : data[i])
            {
                int x = DSU::find(parent, i);
                int y = DSU::find(parent, el.first);

                if (x == y)
                {
                    circleExist = true;
                    break;
                }
                DSU::Union(parent, x, y);
            }
        if(!circleExist)
            return 0;

        int count = 0,top = 1;
        for(int i=0; i<NTops; i++)
        {
            if(data[i].size()%2!=0)
            {
                top = i+1;
                count++;
            }
        }
        if(count>2)
            return 0;
        else if(count == 0)
            circleExist = true;
        else
            circleExist = false;

        return top;
    }
    std::vector<int> getEuleranTourEffective()
    {
        bool is_circle;
        auto v = CheckEuler(is_circle);
        if(!v)
            return std::vector<int> {};
        v--;
        std::vector<int> R;
        R.reserve(2*data.size());
        auto copy = data;
        std::stack<int> S;
        S.push(v);
        while (!S.empty())
        {
            auto w = S.top();
            auto map = copy[w];
            if(!map.empty())
            {
                auto u = map.begin()->first;
                S.push(u);
                copy[w].erase(u);
                if(!is_directed)
                    copy[u].erase(w);
            }

            if(w==S.top())
            {
                S.pop();
                R.push_back(w+1);
            }
        }
        return R;
    }
    std::vector<int> getEuleranTourFleri()
    {
        bool is_circle;
        auto v = CheckEuler(is_circle);
        if(!v)
            return std::vector<int> {};
        v--;
        std::vector<int> R;
        R.reserve(2*data.size());
        auto copy = data;
        R.push_back(v+1);
        proc(v,copy,R);
        return R;
    }

    virtual ~AdjLists () {}

    Lists data;

private:
    int DFSCount (int v, std::vector<bool> &visited,
                  const Lists &copy)
    {
        visited[v] = true;
        int count = 1;

        for (auto it = copy[v].begin(); it != copy[v].end(); ++it)
            if (!visited[it->first])
                count += DFSCount(it->first, visited,copy);
        return count;
    }
    bool isValidNextEdge(int u, int v,Lists &copy)
    {
        if(copy[u].size()==1)
            return true;

        std::vector<bool> visited(copy.size(),false);
        int count1 = DFSCount(u, visited,copy);

        copy[u][v] = 0;
        if(!is_directed)
            copy[v][u] = 0;

        std::fill(visited.begin(),visited.end(),false);
        int count2 = DFSCount(u, visited,copy);
        copy[u][v] = 1;
        if(!is_directed)
            copy[v][u] = 1;

        return (count1 > count2)? false: true;
    }
    void proc (int u,Lists &copy,std::vector<int> &R)
    {
        for(const auto &el : copy[u])
        {
            auto v = el.first;
            auto t = el.second;
            if(t && isValidNextEdge(u, v,copy) )
            {
                R.push_back(v+1);
                copy[u][v] = 0;
                if(!is_directed)
                    copy[v][u] = 0;
                proc(v,copy,R);
            }
        }
    }
};

typedef std::unordered_multimap<int,std::pair<int,int>> Edges;

class ListOfEdges : public View
{
public:

    ListOfEdges() : View('E') {}
    ListOfEdges(const std::pair<SpainingTree,int> &set) : View('E')
    {
        NTops = set.second;
        auto edges = set.first;
        auto Size = edges.size();
        is_directed = false;
        is_weighted = true;
        data = Edges();
        for(std::size_t i = 0; i<Size; i++)
        {
            int from,to,w;
            std::tie(from,to,w) = edges[i];
            data.insert(std::make_pair(from,std::make_pair(to,w)));
        }
    }

    virtual void Fill(std::ifstream &in)
    {
        std::size_t Size;
        in>>NTops;
        in>>Size;
        in>>is_directed;
        in>>is_weighted;
        data = Edges();
        for(std::size_t i = 0; i<Size; i++)
        {
            int from;
            int to;
            in>>from;
            in>>to;
            from--;
            to--;
            if(is_weighted)
            {
                int w;
                in>>w;
                data.insert(std::make_pair(from,
                                           std::make_pair(to,w)));
            }
            else
                data.insert(std::make_pair(from,
                                           std::make_pair(to,1)));
        }
    }
    virtual void Write(std::ostream &out)
    {
        out<<NTops<<' '<<data.size()<<'\n';
        out<<is_directed<<' '<<is_weighted<<'\n';
        for(const auto &el : data)
        {
            auto value = el.second;
            if(is_weighted)
                out<<el.first+1<<' '<<value.first+1<<' '<<value.second;
            else
                out<<el.first+1<<' '<<value.first+1;
            out<<'\n';
        }
    }

    virtual void addEdge(int from,int to, int weight)
    {
        data.insert(std::make_pair(from,std::make_pair(to,weight)));
        if(!is_directed)
            data.insert(
                std::make_pair(to,std::make_pair(from,weight)));
    }
    virtual void removeEdge(int from,int to)
    {
        erase(from,to);
        if(!is_directed)
            erase(to,from);
    }
    virtual int changeEdge(int from,int to,int newWeight)
    {
        int oldWeight;
        auto range = data.equal_range(from);
        for (auto it = range.first; it != range.second;)
        {
            if (it->second.first == to)
            {
                oldWeight = it->second.second;
                it->second.second = newWeight;
                break;
            }
            else
                ++it;
        }
        if(is_directed)
            return oldWeight;
        range = data.equal_range(to);
        for (auto it = range.first; it != range.second;)
        {
            if (it->second.first == from)
            {
                it->second.second = newWeight;
                return oldWeight;
            }
            else
                ++it;
        }
        return oldWeight;
    }
    virtual std::pair<SpainingTree,int> getSpainingTreePrima()
    {
        int V = NTops;
        std::vector<int> key(V,std::numeric_limits<int>::max());
        std::vector<int> mstSet(V,-1);

        SpainingTree R(V-1);
        std::set<std::pair<int,int>> q;
        q.insert (std::make_pair (0, 0));
        std::vector<bool> used(V,false);
        auto copy = data;
        for(const auto &el : data)
        {
            auto from = el.first;
            auto to = el.second.first;
            auto w = el.second.second;
            copy.insert(
                std::make_pair(to,std::make_pair(from,w)));
        }

        for (int i=0; i<V; ++i)
        {
            if (q.empty())
            {
                std::cout << "No MST!";
                exit(0);
            }
            int v = q.begin()->second;

            q.erase (q.begin());

            if (mstSet[v] != -1)
                used[v] = true;

            auto range = copy.equal_range(v);
            for (auto it = range.first; it != range.second; ++it)
            {
                auto j = it->second.first;
                auto w = it->second.second;
                if (!used[j] && w <  key[j])
                {
                    q.erase (std::make_pair(key[j], j));
                    key[j] = w;
                    mstSet[j] = v;
                    q.insert (std::make_pair (key[j], j));
                }
            }
        }

        for (int i = 1; i < V; i++)
            R[i-1] = std::make_tuple(i, mstSet[i], key[i]);

        return std::make_pair(R,V);
    }
    std::pair<SpainingTree,int> getSpainingTreeKruscal()
    {
        int V = data.size();
        SpainingTree result;

        SpainingTree SortedEdges(V);
        std::size_t j = 0;
        for(const auto &el : data)
            SortedEdges[j++] =
                std::make_tuple(el.first,el.second.first,el.second.second);

        std::sort(SortedEdges.begin(),SortedEdges.end(),
                  [](EdgeTuple &a,EdgeTuple &b)
        {
            return std::get<2>(a)<std::get<2>(b);
        });

        DSU subsets = DSU(V);


        for(const auto &el : SortedEdges)
        {
            int x = subsets.find(std::get<0>(el));
            int y = subsets.find(std::get<1>(el));

            if (x != y)
            {
                result.push_back(el);
                subsets.unite(x, y);
            }
        }
        return std::make_pair(result,NTops);
    }

    std::pair<SpainingTree,int> getSpainingTreeBoruvka()
    {
        int E = data.size();
        SpainingTree edges(E);
        std::size_t j = 0;
        SpainingTree result(NTops-1);
        for(const auto &el : data)
            edges[j++] =
                std::make_tuple(el.first,el.second.first,el.second.second);

        std::sort(edges.begin(),edges.end(),
                  [](EdgeTuple &a,EdgeTuple &b)
        {
            return std::get<2>(a)<std::get<2>(b);
        });

        DSU subsets = DSU(NTops);

        int numTrees = NTops;
        auto index = 0;

        for (std::size_t t = 1; t < NTops && numTrees > 1; t = t + t)
        {
            std::vector<int> cheapest(NTops,-1);
            for (int i=0; i<E; i++)
            {
                int set1 = subsets.find(std::get<0>(edges[i]));
                int set2 = subsets.find(std::get<1>(edges[i]));
                if (set1 == set2)
                    continue;

                else
                {
                    if (cheapest[set1] == -1 ||
                            std::get<2>(edges[cheapest[set1]]) > std::get<2>(edges[i]))
                        cheapest[set1] = i;

                    if (cheapest[set2] == -1 ||
                            std::get<2>(edges[cheapest[set2]]) > std::get<2>(edges[i]))
                        cheapest[set2] = i;
                }
            }
            for (std::size_t i=0; i<NTops; i++)
            {
                // Check if cheapest for current set exists
                if (cheapest[i] != -1)
                {
                    int set1 = subsets.find(std::get<0>(edges[cheapest[i]]));
                    int set2 = subsets.find(std::get<1>(edges[cheapest[i]]));
                    if (set1 == set2)
                        continue;
                    //MSTweight += std::get<2>(edge[cheapest[i]]);
                    result[index++] = edges[cheapest[i]];
                    subsets.unite(set1,set2);
                    numTrees--;
                }
            }

        }
        return std::make_pair(result,NTops);
    }
    int CheckEuler(bool &circleExist)
    {
        //проверка на 2-ой критерий
        std::vector<int> parent(NTops,-1);
        circleExist = false;
        for(const auto &el : data)
        {
            int x = DSU::find(parent,el.first);
            int y = DSU::find(parent, el.second.first);

            if (x == y)
            {
                circleExist = true;
                break;
            }

            DSU::Union(parent, x, y);
        }
        if(!circleExist)
            return 0;
        auto copy = data;
        for(const auto &el : data)
        {
            auto from = el.first;
            auto to = el.second.first;
            auto w = el.second.second;
            copy.insert(
                std::make_pair(to,std::make_pair(from,w)));
        }
        int count = 0,top = 1;
        for(int i =0; i<int(NTops); i++)
        {
            if(copy.count(i)%2!=0)
            {
                top = i+1;
                count++;
            }
        }
        if(count>2)
            return 0;
        else if(count == 0)
            circleExist = true;
        else
            circleExist = false;

        return top;
    }
    std::vector<int> getEuleranTourFleri()
    {
        bool is_circle;
        auto v = CheckEuler(is_circle);
        if(!v)
            return std::vector<int> {};
        v--;
        std::vector<int> R(data.size()+1);
        auto copy = data;
        for(const auto &el : data)
        {
            auto from = el.first;
            auto to = el.second.first;
            auto w = el.second.second;
            copy.insert(
                std::make_pair(to,std::make_pair(from,w)));
        }
        R[0] = v+1;
        proc(v,copy,R);
        return R;
    }
    std::vector<int> getEuleranTourEffective()
    {
        bool is_circle;
        auto v = CheckEuler(is_circle);
        if(!v)
            return std::vector<int> {};
        v--;
        std::vector<int> R(data.size()+1,0);
        auto copy = data;
        for(const auto &el : data)
        {
            auto from = el.first;
            auto to = el.second.first;
            auto w = el.second.second;
            copy.insert(
                std::make_pair(to,std::make_pair(from,w)));
        }
        std::stack<int> S;
        S.push(v);
        std::size_t count = 0;
        while (!S.empty())
        {
            auto w = S.top();
            auto it = copy.find(w);
            if(it != copy.cend())
            {
                auto u = it->second.first;
                S.push(u);
                erase(copy,w,u);
                if(!is_directed)
                    erase(copy,u,w);
            }

            if(w==S.top())
            {
                S.pop();
                R[count++]=w+1;
            }
        }
        return R;
    }
    virtual ~ListOfEdges () {}

    Edges data;
    bool is_directed;
    std::size_t NTops;

private:

    void erase(int from,int to)
    {
        erase(data,from,to);
    }
    void erase(Edges &d, int from,int to)
    {
        auto range = d.equal_range(from);
        for (auto it = range.first; it != range.second;)
        {
            if (it->second.first == to)
                it = d.erase(it);
            else
                ++it;
        }
    }

    int DFSCount (int v, std::vector<bool> &visited,
                  const Edges &copy)
    {
        visited[v] = true;
        int count = 1;

        auto range = copy.equal_range(v);
        for (auto it = range.first; it != range.second; it++)
        {
            auto j = it->first;
            if (!visited[j])
                count += DFSCount(j, visited,copy);
        }
        return count;
    }
    bool isValidNextEdge(int u, int v,Edges &copy)
    {
        int count1 = 0;
        auto range = copy.equal_range(u);
        for (auto it = range.first; it != range.second; it++)
            if (it->second.second)
                count1++;

        if(count1==1)
            return true;

        std::vector<bool> visited(copy.size(),false);
        count1 = DFSCount(u, visited,copy);

        auto r = copy.equal_range(v);
        for (auto it = range.first; it != range.second; it++)
            if (it->second.first == v)
                it->second.second = 0;
        if(!is_directed)
        {
            for (auto it = r.first; it != r.second; it++)
                if (it->second.first == u)
                    it->second.second = 0;
        }

        std::fill(visited.begin(),visited.end(),false);
        int count2 = DFSCount(u, visited,copy);
        for (auto it = range.first; it != range.second; it++)
            if (it->second.first == v)
                it->second.second = 1;
        if(!is_directed)
        {
            for (auto it = r.first; it != r.second; it++)
                if (it->second.first == u)
                    it->second.second = 1;
        }

        return (count1 > count2)? false: true;
    }
    void proc (int u,Edges &copy,std::vector<int> &R)
    {
        static int index = 0;
        auto range = copy.equal_range(u);
        for (auto it = range.first; it != range.second; it++)
        {
            auto v = it->second.first;
            if(it->second.second && isValidNextEdge(u, v,copy) )
            {
                R[++index]=v+1;
                it->second.second = 0;
                if(!is_directed)
                {
                    auto r = copy.equal_range(v);
                    for (auto z = r.first; z != r.second; z++)
                        if (z->second.first == u)
                            z->second.second = 0;

                }
                proc(v,copy,R);
            }
        }

    }

};

struct TransformView
{
    static AdjMatrix* MatrixFromLists(const AdjLists *lists)
    {
        auto R = new AdjMatrix();
        auto Size = lists->data.size();
        R->data = Matrix(Size,IntVec(Size,0));
        R->is_weighted = lists->is_weighted;
        R->is_directed = lists->is_directed;
        for(std::size_t i = 0; i<Size; i++)
        {
            for(const auto &el : lists->data[i])
                if(lists->is_weighted)
                    R->data[i][el.first]=el.second;
                else
                    R->data[i][el.first]=1;
        }
        return R;
    };
    static AdjMatrix* MatrixFromEList(const ListOfEdges *lists)
    {
        auto R = new AdjMatrix();
        auto Size = lists->NTops;

        R->data = Matrix(Size,IntVec(Size,0));
        R->is_weighted = lists->is_weighted;
        R->is_directed = lists->is_directed;

        for(const auto &el : lists->data)
        {
            if(lists->is_weighted)
                R->data[el.first][el.second.first]=el.second.second;
            else
                R->data[el.first][el.second.first]=1;
            if(!lists->is_directed)
            {
                if(lists->is_weighted)
                    R->data[el.second.first][el.first]=el.second.second;
                else
                    R->data[el.second.first][el.first]=1;
            }
        }
        return R;
    };
    static AdjLists* AListFromMatrix(const AdjMatrix *matrix)
    {
        auto R = new AdjLists();
        auto Size = matrix->data.size();
        R->data = Lists(Size,List());
        R->is_weighted = matrix->is_weighted;
        R->is_directed = matrix->is_directed;

        for(std::size_t i = 0; i<Size; i++)
        {
            for(std::size_t j = 0; j<Size; j++)
            {
                auto el = matrix->data[i][j];
                if(el)
                    R->data[i][j]=el;
            }
        }
        return R;
    };
    static AdjLists* AListFromEList(const ListOfEdges *lists)
    {
        auto R = new AdjLists();
        auto Size = lists->NTops;
        R->data = Lists(Size,List());
        R->is_weighted = lists->is_weighted;
        R->is_directed = lists->is_directed;

        for(const auto &el : lists->data)
        {
            if(lists->is_weighted)
                R->data[el.first][el.second.first]=el.second.second;
            else
                R->data[el.first][el.second.first]=1;

            if(!lists->is_directed)
            {
                if(lists->is_weighted)
                    R->data[el.second.first][el.first]=el.second.second;
                else
                    R->data[el.second.first][el.first]=1;
            }
        }
        return R;
    };
    static ListOfEdges* EListFromMatrix(const AdjMatrix *matrix)
    {
        auto R = new ListOfEdges();
        auto Size = matrix->data.size();
        R->data = Edges();
        R->is_weighted = matrix->is_weighted;
        R->is_directed = matrix->is_directed;
        R->NTops = Size;

        for(std::size_t i = 0; i<Size; i++)
        {
            for(std::size_t j = i+1; j<Size; j++)
            {
                auto el = matrix->data[i][j];
                if(el)
                    R->data.insert(std::make_pair(i,
                                                  std::make_pair(j,el)));
            }
        }
        if(!R->is_directed)
            return R;
        for(std::size_t i = 0; i<Size; i++)
        {
            for(std::size_t j = i+1; j<Size; j++)
            {
                auto el = matrix->data[j][i];
                if(el)
                    R->data.insert(std::make_pair(j,
                                                  std::make_pair(i,el)));
            }
        }
        return R;
    };
    static ListOfEdges* EListFromLists(const AdjLists *lists)
    {
        auto R = new ListOfEdges();
        auto Size = lists->data.size();
        R->data = Edges();
        R->is_weighted = lists->is_weighted;
        R->is_directed = lists->is_directed;
        R->NTops = Size;

        for(std::size_t i = 0; i<Size; i++)
        {
            for(const auto &el : lists->data[i])
                if(lists->is_weighted)
                    R->data.insert(
                        std::make_pair(i,std::make_pair(
                                           el.first,el.second)));
                else
                    R->data.insert(
                        std::make_pair(i,std::make_pair(
                                           el.first,1)));
        }
        return R;
    };
};


class Graph
{
public:
    Graph() : data(nullptr) {};
    void readGraph(std::string fileName)
    {
        std::ifstream in(fileName);
        if(!in)
            return;
        char type;
        in>>type;
        if(type=='C')
        {
            data = new AdjMatrix();
            data->Fill(in);
        }
        else if(type=='L')
        {
            data = new AdjLists();
            data->Fill(in);
        }
        else if(type=='E')
        {
            data = new ListOfEdges();
            data->Fill(in);
        }
    }
    void addEdge(int from,int to, int weight)
    {
        data->addEdge(from,to,weight);
    }
    void removeEdge(int from,int to)
    {
        data->removeEdge(from,to);
    }
    int changeEdge(int from,int to,int newWeight)
    {
        return data->changeEdge(from,to,newWeight);
    }
    void transformToAdjList()
    {
        if(data->type=='C')
        {
            View* update = TransformView::AListFromMatrix(dynamic_cast<AdjMatrix*>(data));
            delete data;
            data = update;
        }
        else if(data->type=='L')
        {
            return;
        }
        else if(data->type=='E')
        {
            View* update = TransformView::AListFromEList(dynamic_cast<ListOfEdges*>(data));
            delete data;
            data = update;
        }
    }
    void transformToAdjMatrix()
    {

        if(data->type=='C')
        {
            return;
        }
        else if(data->type=='L')
        {
            View* update = TransformView::MatrixFromLists(dynamic_cast<AdjLists*>(data));
            delete data;
            data = update;
        }
        else if(data->type=='E')
        {
            View* update = TransformView::MatrixFromEList(dynamic_cast<ListOfEdges*>(data));
            delete data;
            data = update;
        }
    }
    void transformToListOfEdges()
    {
        if(data->type=='C')
        {
            View* update = TransformView::EListFromMatrix(dynamic_cast<AdjMatrix*>(data));
            delete data;
            data = update;
        }
        else if(data->type=='L')
        {
            View* update = TransformView::EListFromLists(dynamic_cast<AdjLists*>(data));
            delete data;
            data = update;
        }
        else if(data->type=='E')
        {
            return;
        }
    }
    void writeGraph(std::string fileName)
    {
        std::ofstream out(fileName);
        if(!out)
            return;

        out<<data->type<<' ';
        data->Write(out);
    }

    Graph getSpainingTreePrima() const
    {
        std::pair<SpainingTree,int> R;
        //матрица
        if(data->type=='C')
        {
            R = data->getSpainingTreePrima();
        }
        //списки смежности
        else if(data->type=='L')
        {
            R = data->getSpainingTreePrima();
        }
        //списки ребер
        else if(data->type=='E')
        {
            R = data->getSpainingTreePrima();
        }
        Graph g;
        g.data = new ListOfEdges(R);
        return g;
    }
    Graph getSpainingTreeKruscal() const
    {
        std::pair<SpainingTree,int> R;
        //матрица
        if(data->type=='C')
        {
            R = data->getSpainingTreeKruscal();
        }
        //списки смежности
        else if(data->type=='L')
        {
            R = data->getSpainingTreeKruscal();
        }
        //списки ребер
        else if(data->type=='E')
        {
            R = data->getSpainingTreeKruscal();
        }
        Graph g;
        g.data = new ListOfEdges(R);
        return g;
    }

    Graph getSpainingTreeBoruvka() const
    {
        std::pair<SpainingTree,int> R;
        //матрица
        if(data->type=='C')
        {
            R = data->getSpainingTreeBoruvka();
        }
        //списки смежности
        else if(data->type=='L')
        {
            R = data->getSpainingTreeBoruvka();
        }
        //списки ребер
        else if(data->type=='E')
        {
            R = data->getSpainingTreeBoruvka();
        }
        Graph g;
        g.data = new ListOfEdges(R);
        return g;
    }
    int CheckEuler(bool &circleExist)
    {
        return data->CheckEuler(circleExist);
    }
    std::vector<int> getEuleranTourFleri()
    {
        return data->getEuleranTourFleri();
    }
    std::vector<int> getEuleranTourEffective()
    {
        return data->getEuleranTourEffective();
    }
    ~Graph()
    {
        delete data;
    }
private:
    View* data;

};