#include "Graph.h"

int main()
{
    Graph test;
    test.readGraph("example_.txt");
    test.transformToListOfEdges();
    test.writeGraph("MatrixToEList.txt");
    test.transformToAdjList();
    test.writeGraph("ElistToAList.txt");
    test.transformToAdjMatrix();
    test.writeGraph("AlistToMatrix.txt");
    test.transformToAdjList();
    test.writeGraph("MatrixToAList.txt");
    test.transformToListOfEdges();
    test.writeGraph("AlistToEList.txt");
    test.transformToAdjMatrix();
    test.writeGraph("ElistToMatrix.txt");
}