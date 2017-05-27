#include "Graph.h"


int main()
{
    Graph test;
    test.readGraph("new_.txt");

    test.getSpainingTreePrima().writeGraph("EdgeListPrima.txt");
    test.getSpainingTreeKruscal().writeGraph("EdgeListKruscal.txt");
    test.getSpainingTreeBoruvka().writeGraph("EdgeListBoruvka.txt");

    test.transformToAdjList();
    test.getSpainingTreeBoruvka().writeGraph("AdjListBoruvka.txt");
    test.getSpainingTreeKruscal().writeGraph("AdjListKruscal.txt");
    test.getSpainingTreePrima().writeGraph("AdjListPrima2.txt");

    test.transformToAdjMatrix();
    test.getSpainingTreePrima().writeGraph("MatrixPrima.txt");
    test.getSpainingTreeKruscal().writeGraph("MatrixCruscal.txt");
    test.getSpainingTreeBoruvka().writeGraph("MatrixBoruvka.txt");



}