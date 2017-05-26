#include "Graph.h"


int main()
{
    Graph test;
    test.readGraph("example_.txt");
    test.getSpainingTreePrima().writeGraph("MatrixPrima.txt");
    test.getSpainingTreeKruscal().writeGraph("MatrixCruscal.txt");
    test.getSpainingTreeBoruvka().writeGraph("MatrixBoruvka.txt");
    test.transformToListOfEdges();
    test.getSpainingTreePrima().writeGraph("EdgeListPrima.txt");
    test.getSpainingTreeKruscal().writeGraph("EdgeListKruscal.txt");
    test.getSpainingTreeBoruvka().writeGraph("EdgeListBoruvka.txt");
    test.transformToAdjList();
    test.getSpainingTreeBoruvka().writeGraph("AdjListBoruvka.txt");
    test.getSpainingTreeKruscal().writeGraph("AdjListKruscal.txt");
    test.getSpainingTreePrima().writeGraph("AdjListPrima.txt");
}