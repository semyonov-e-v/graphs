#include "Graph.h"


int main()
{
    Graph test;
    test.readGraph("lab3.txt");

    for(const auto &el : test.getEuleranTourEffective())
        std::cout<<el<<' ';

    std::cout<<'\n';
    for(const auto &el : test.getEuleranTourFleri())
        std::cout<<el<<' ';
    test.transformToAdjList();
    std::cout<<'\n';
    for(const auto &el : test.getEuleranTourEffective())
        std::cout<<el<<' ';
    std::cout<<'\n';
    for(const auto &el : test.getEuleranTourFleri())
        std::cout<<el<<' ';
    test.transformToAdjMatrix();
    std::cout<<'\n';
    for(const auto &el : test.getEuleranTourEffective())
        std::cout<<el<<' ';
    std::cout<<'\n';
    for(const auto &el : test.getEuleranTourFleri())
        std::cout<<el<<' ';
    std::cout<<'\n';
    std::cin.get();
    std::cin.get();

}