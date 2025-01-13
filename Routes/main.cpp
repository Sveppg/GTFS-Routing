//lib header
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
//custom Header
#include "network.h"
#include "CSVReader.h"
#include "types.h"
#include "scheduled_trip.h"

int main(int argc, char **argv) {
    if(argc != 2){
        std::cerr << "Usage: " << "<file-path-to-file>" << std::endl;
        return EXIT_FAILURE;
    }
    bht::Network n{argv[1]};


    for (const auto& item : n.getTravelPath("de:12054:900230999", "de:13071:12353")){
        std::cout << item.name<< ": " << item.id<< std::endl;
    }

    for (const auto& item : n.getNeighbors("de:12054:900230014::2")){
        std::cout << item << std::endl;
    }
    for(const auto& item : n.getStopsForTransfer("de:12054:900230999")){
        std::cout << item << std::endl;
    }
    // bht::NetworkScheduledTrip trip = n.getScheduledTrip("230419258");
    // for (bht::NetworkScheduledTrip::iterator iter = trip.begin(); iter != trip.end(); iter++) {
    //     std::cout << iter->stopSequence << ": " << n.stops[iter->stopId].name  << " " << iter->stopId << std::endl;
    // }
    return 0;
}


