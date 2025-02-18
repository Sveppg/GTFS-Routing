//lib header
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
//custom Header
#include "network.h"
#include "CSVReader.h"
#include "types.h"
//#include "renderarea.h"

//#include "mainwindow.h"


int main(int argc, char **argv){
    bht::Network n(argv[1]);
    for(const auto& [routeShapeId,routeShapes] : n.routeShapes){
        std::cout << routeShapeId << std::endl;
        for (const auto& n : routeShapes){
            std::cout << n.id << std::endl;
            std::cout << n.latitude << "-- ---- --" << std::endl;
            std::cout << n.longitude << std::endl;
        }
    }

    return 0;
}
