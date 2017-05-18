#pragma once
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>
#include <iterator>
#include <sstream>

std::vector<int> Split(const std::string& subject)
{
    std::istringstream ss{subject};
    using StrIt = std::istream_iterator<int>;
    std::vector<int> container{StrIt{ss}, StrIt{}};
    return container;
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
    virtual ~View () {}

    bool is_weighted;
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
    virtual ~AdjMatrix () {}

    Matrix data;
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
                key = vec[j];
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
                    out<<el.first<<' '<<el.second<<' ';
                else
                    out<<el.first<<' ';
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
    virtual ~AdjLists () {}

    Lists data;
    bool is_directed;
};

typedef std::unordered_multimap<int,std::pair<int,int>> Edges;

class ListOfEdges : public View
{
public:

    ListOfEdges() : View('E') {}
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
                out<<el.first<<' '<<value.first<<' '<<value.second;
            else
                out<<el.first<<' '<<value.first;
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
    virtual ~ListOfEdges () {}

    Edges data;
    bool is_directed;
    std::size_t NTops;

private:

    void erase(int from,int to)
    {
        auto range = data.equal_range(from);
        for (auto it = range.first; it != range.second;)
        {
            if (it->second.first == to)
                it = data.erase(it);
            else
                ++it;
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

        for(const auto &el : lists->data)
            if(lists->is_weighted)
                R->data[el.first][el.second.first]=el.second.second;
            else
                R->data[el.first][el.second.first]=1;

        return R;
    };
    static AdjLists* AListFromMatrix(const AdjMatrix *matrix)
    {
        auto R = new AdjLists();
        auto Size = matrix->data.size();
        R->data = Lists(Size,List());
        R->is_weighted = matrix->is_weighted;
        bool flag=true;
        for(std::size_t i = 0; i<Size; i++)
        {
            for(std::size_t j = 0; j<Size; j++)
            {
                auto el = matrix->data[i][j];
                if(el)
                    R->data[i][j]=el;
                if(flag && el!=matrix->data[j][i])
                    flag = false;
            }
        }
        R->is_directed = flag;
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
            if(lists->is_weighted)
                R->data[el.first][el.second.first]=el.second.second;
            else
                R->data[el.first][el.second.first]=1;
        return R;
    };
    static ListOfEdges* EListFromMatrix(const AdjMatrix *matrix)
    {
        auto R = new ListOfEdges();
        auto Size = matrix->data.size();
        R->data = Edges();
        R->is_weighted = matrix->is_weighted;
        R->NTops = Size;
        bool flag=true;
        for(std::size_t i = 0; i<Size; i++)
        {
            for(std::size_t j = 0; j<Size; j++)
            {
                auto el = matrix->data[i][j];
                if(el)
                    R->data.insert(std::make_pair(i,
                                                  std::make_pair(j,el)));
                if(flag && el!=matrix->data[j][i])
                    flag = false;
            }
        }
        R->is_directed = flag;
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

    ~Graph()
    {
        delete data;
    }
private:
    View* data;

};