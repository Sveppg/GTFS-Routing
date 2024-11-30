//lib header
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
//custom Header
#include "network.h"
#include "CSVReader.h"
#include "types.h"

/*
*  Function for get Lines and Columns
*/
void getLinesnColumns(const std::string& filePath){
    std::ifstream file(filePath);
    if(!file.is_open()){
        std::cerr << "Error opening the file: " << filePath << std::endl;
            return; 
    }

    std::string line;
    int lineCount = 0; 
    int columnCount = 0;

    while(std::getline(file, line)){
        lineCount++;
        columnCount += line.length();
    }
    std::cout <<filePath << " has" << ": " << lineCount << " lines with " << columnCount << " columns " << std::endl;
        file.close();
}


int main(int argc, char **argv){
    if(argc != 2){
        std::cerr << "Usage: " << "<file-path-to-file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string gtfsDirectory = argv[1];
    bht::Network network{gtfsDirectory};    
    // //for Agencies
    for (const auto& pair : network.agencies) {
        bht::Agency agency = pair.second;
        std::cout << "Agency ID: " << agency.id 
                << ", Name: " << agency.name 
                << ", URL: " << agency.url 
                << ", Timezone: " << agency.timezone 
                << ", Language: " << agency.language 
                << ", Phone: " << agency.phone 
                << std::endl;
    }
    //for calendar
    for (const auto& pair : network.calendars) {
        const auto& calendar = pair.second;
        std::cout << "Service ID: " << calendar.serviceId 
                << ", Monday: " << static_cast<int>(calendar.monday)
                << ", Tuesday: " << static_cast<int>(calendar.tuesday)
                << ", Wednesday: " << static_cast<int>(calendar.wednesday)
                << ", Thursday: " << static_cast<int>(calendar.thursday)
                << ", Friday: " << static_cast<int>(calendar.friday)
                << ", Saturday: " << static_cast<int>(calendar.saturday)
                << ", Sunday: " << static_cast<int>(calendar.sunday)
                << ", Start Date: " << calendar.startDate.year << "-" << static_cast<int>(calendar.startDate.month) << "-" << static_cast<int>(calendar.startDate.day)
                << ", End Date: " << calendar.endDate.year << "-" << static_cast<int>(calendar.endDate.month) << "-" << static_cast<int>(calendar.endDate.day)
                << std::endl;
    }



    //for levels
    for (const auto& pair : network.levels) {
        const bht::Level& level = pair.second;
        std::cout << "ID: " << level.id 
                << ", Index: " << level.index 
                << ", Name: " << level.name 
                << std::endl;
    }

    for(const auto& pair : network.pathways){
        const bht::Pathway& pathways = pair.second;
        std::cout << "ID: " << pathways.id 
                   << " to Stop ID: " << pathways.fromStopId
                << " from StopID: " << pathways.toStopId
                << " Mode: " << pathways.mode
                << " Directional: " << static_cast<bool>(pathways.isBidirectional)
                << " Length: " << static_cast<float>(pathways.length)
                << " Traversal Time: " << static_cast<unsigned int>(pathways.traversalTime)
                << " Stair Count: " << static_cast<unsigned int>(pathways.stairCount)
                << " Max. Slope: " << static_cast<float>(pathways.maxSlope)
                << " min. Width" << static_cast<float>(pathways.minWidth)
                << " posted Ads: " << pathways.signpostedAs
                << std::endl;
    }   

    for(const auto& pair : network.routes){
        const bht::Route& route = pair.second;
        std::cout <<" Route ID: " << route.id
                << " Agency_ID: " << route.agencyId  
                << " Route_Short-Name " << route.shortName
                << " Route_Long-Name " << route.longName
                << " Route-Description " << route.description
                << " Route_Type " << route.type
                << " Route-Colour " << route.color
                << " Route                Text-Colour " << route.textColor
                << std::endl;
    }   
    //for shapes
    for(const auto& pair : network.shapes) {
        const bht::Shape& shape = pair; // Korrekt: bht::Shape ist der Typ
        std::cout << "Shape_ID: " << shape.id
              << ", Latitude: " << shape.latitude
              << ", Longitude: " << shape.longitude
              << ", Sequence: " << shape.sequence
              << std::endl;
    }
    //for StopTimes
     for(const auto& pair : network.stopTimes) {
         const bht::StopTime& stopTime = pair;
         std::cout << "Trip ID: " << stopTime.tripId 
           << ", Arrival Time: " 
           << static_cast<int>(stopTime.arrivalTime.hour) << ":"
           << static_cast<int>(stopTime.arrivalTime.minute) << ":"
           << static_cast<int>(stopTime.arrivalTime.second)
           << ", Departure Time: "
           << static_cast<int>(stopTime.departureTime.hour) << ":"
           << static_cast<int>(stopTime.departureTime.minute) << ":"
           << static_cast<int>(stopTime.departureTime.second) 
           << ", Stop ID: " << stopTime.stopId 
           << ", Stop Sequence: " << stopTime.stopSequence 
           << ", Pickup Type: " << stopTime.pickupType 
           << ", Drop-Off Type: " << stopTime.dropOffType
           << ", Stop-Headsign: " << stopTime.stopHeadsign
           << std::endl;
    }
    //for Stops
    for(const auto& pair: network.stops){
        const bht::Stop& stop = pair.second;
        std::cout << "Stop ID: " << stop.id 
                << " Stop Code: " << stop.code 
                << " Stop Name: " << stop.name
                << " Stop Description: " << stop.description
                << " Stop Latitude: " << stop.latitide
                << " Stop longitude: " << stop.longitude
                << " Location Type: " << stop.locationType
                << " Parent Station: " << stop.parentStation
                << " Wheelchair Boarding: " << stop.wheelchairBoarding
                << " Platform Code: " << stop.platformCode
                << " Zone ID: " << stop.zoneId
                << " Level ID " << stop.levelId
                << std::endl;
    }
//    "stop_id","stop_code","stop_name","stop_desc","stop_lat","stop_lon","location_type","parent_station","wheelchair_boarding","platform_code","zone_id","level_id"

    for(const auto& pair: network.transfers){
        const bht::Transfer& transfers = pair;
        std::cout << " From Stop-ID " << transfers.fromStopId
                << " To Stop-ID " << transfers.toStopId
                << " Transfer Type: " << transfers.type
                << " Min Time: " << transfers.minTransferTime
                << " from Route-ID " << transfers.fromRouteId
                << " to Route-ID " << transfers.toRouteId
                << " from Trip-ID " << transfers.fromTripId
                << " to Trip-ID " << transfers.toTripId
                << std::endl;
    }
    //"from_stop_id","to_stop_id","transfer_type","min_transfer_time","from_route_id","to_route_id","from_trip_id","to_trip_id"
    for (const auto& pair : network.trips) {
        const bht::Trip& trip = pair;  // Achte darauf, dass du den richtigen Typ verwendest
        std::cout << "Trip ID: " << trip.id  
              << " Route Id: " << trip.routeId
              << " Service ID: " << trip.serviceId
              << " Headsign: " << trip.headsign
              << " Short Name: " << trip.shortName
              << " Direction ID: " << static_cast<int>(trip.direction)  // Umwandlung von Enum in int
              << " Block ID: " << trip.blockId
              << " Shape ID: " << trip.shapeId
              << " Wheelchair Accessible: " << trip.wheelchairAccessible
              << " Bikes Allowed: " << trip.bikesAllowed
              << std::endl;
    }

    return 0; 
}
