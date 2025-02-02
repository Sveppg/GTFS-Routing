#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <queue>
#include<limits>
// custom include
#include "scheduled_trip.h"
#include "CSVReader.h"
#include "types.h"
#include "network.h"
#include "renderarea.h"


namespace bht {

Network::Network(const std::string& directoryPath) {

    std::cout << "loading Data..." << std::endl;
   loadAgencies(directoryPath + "/agency.txt");
   loadCalendarDates(directoryPath + "/calendar_dates.txt");
   loadCalendars(directoryPath + "/calendar.txt");
   loadFrequencies(directoryPath + "/frequencies.txt");
   loadLevels(directoryPath + "/levels.txt");
   loadPathways(directoryPath + "/pathways.txt");
   loadRoutes(directoryPath + "/routes.txt");
   loadShapes(directoryPath +"/shapes.txt");
   loadStopTimes(directoryPath + "/stop_times.txt");
   loadStops(directoryPath + "/stops.txt");
   loadTransfers(directoryPath + "/transfers.txt");
   loadTrips(directoryPath + "/trips.txt");
    std::cout << "Data loaded: ->" << std::endl;

   buildStopIdToTrips(stopTimes);
   buildTripIdToStopTimes(stopTimes);
}



NetworkScheduledTrip Network::getScheduledTrip(const std::string& tripId) const {
    auto stopTimesForTrip = getStopTimesForTrip(tripId);
    return NetworkScheduledTrip(tripId, stopTimesForTrip);
}


void Network::getShapesForRoute() {
    std::unordered_map<std::string, std::vector<Shape>> shapesMap;
    for (const auto& shape : shapes) {
        shapesMap[shape.id].push_back(shape);
    }

    std::unordered_map<std::string, std::vector<std::string>> routeToShapes;
    for (const auto& trip : trips) {
        routeToShapes[trip.routeId].push_back(trip.shapeId);
    }

    for (const auto& [routeId, shapeIds] : routeToShapes) {
        std::vector<std::string> allShapesForRoute;

        for (const auto& shapeId : shapeIds) {
            if (shapesMap.find(shapeId) != shapesMap.end()) {
                const auto& shapesForId = shapesMap[shapeId];
                allShapesForRoute.push_back(shapeId);

                // Korrektur: Füge nur die IDs der Shapes hinzu
                for (const auto& shape : shapesForId) {
                    allShapesForRoute.push_back(shape.id);
                }
            }
        }

        routeToShapes[routeId] = allShapesForRoute;
    }
}

void Network::buildStopIdToTrips(const std::vector<bht::StopTime> &stopTimes){
    // Clear the map
    stopIdToTrips.clear();
    for (const auto &stopTime : stopTimes){
    // Add the trip ID to the corresponding stop ID in the map
        stopIdToTrips[stopTime.stopId].push_back(stopTime.tripId);
    }
}

void Network::buildTripIdToStopTimes(const std::vector<bht::StopTime> &stopTimes){
    tripIdToStopTimes.clear();
    for (const auto &stopTime : stopTimes){
        tripIdToStopTimes[stopTime.tripId].push_back(stopTime);
    }
    // sort with stopSequence (faster lookup later) /
    for (auto &[tripId, stopTimesVector] : tripIdToStopTimes){
            /*
            Parameters
            first, last    -    the range of elements to sort
            policy        -    the execution policy to use. See execution policy for details.
            comp        -    comparison function object (i.e. an object that satisfies the requirements of Compare) which returns ​true if the first argument is less than (i.e. is ordered before) the second.
            // we create a lambda (throw away func) to sort it easy via < */
        std::sort(stopTimesVector.begin(), stopTimesVector.end(),
            [](const bht::StopTime &first,
            const bht::StopTime &second){
                return first.stopSequence < second.stopSequence;
            });
        }
}


std::unordered_set<std::string> Network::getNeighbors(const std::string& stopId) {

    std::unordered_set<std::string> neighbors;

    if (stopIdToTrips.find(stopId) == stopIdToTrips.end()) {
        //std::cout<<"Kein nachbar gefunden !!!"<< stopId << std::endl;
        return neighbors; // Kein Nachbar gefunden
    }

    // Iteriere über alle Trips, die mit diesem Stop verknüpft sind
    for (const auto& tripId : stopIdToTrips[stopId]) {
        // Prüfen, ob der Trip in der Map existiert
        if (tripIdToStopTimes.find(tripId) == tripIdToStopTimes.end()) {
        std::cout<<"no Trip found"<<std::endl;
            continue;
        }

        const auto& stopTimesList = tripIdToStopTimes.at(tripId);
        // Suche die aktuelle Position des Stops im StopTimes-Array
        auto it = std::find_if(stopTimesList.begin(), stopTimesList.end(),
            [&stopId](const bht::StopTime& stopTime) {
                return stopTime.stopId == stopId;
            });
        if (it != stopTimesList.end()) {
            // Nachbarn hinzufügen (vorheriger und nächster Stop im Trip)
            if (it != stopTimesList.begin()) {
                neighbors.insert((it - 1)->stopId); // Vorheriger Stop
                // print(neighbors);
            }
            if (it + 1 != stopTimesList.end()) {
                neighbors.insert((it + 1)->stopId); // Nächster Stop
            }
        }
    }
    for (const auto& inserted: getStopsForTransfer(stopId)){
        if(inserted.locationType == LocationType_Stop){
        neighbors.insert(inserted.id);
        }
    }
    return neighbors;
}


std::vector<Stop> Network::getTravelPath(const std::string& fromStopId, const std::string& toStopId) {
    std::vector<bht::Stop> vekStop = Network::getStopsForTransfer(fromStopId);

    // for (const auto& stop_vek :vekStop){
    //     std::cout << "stop sollte immer der gleiche sein " <<stop_vek.name << "unterschiedliche ID: " <<stop_vek.id<<std::endl;
    // }

    // Distanzen und Vorgänger
    std::unordered_map<std::string, double> distances;
    std::unordered_map<std::string, std::string> previous;
    std::unordered_set<std::string> visited;
    std::vector<Stop> path;

    // Prioritätswarteschlange (Min-Heap)
    std::priority_queue<std::pair<double, std::string>,
                        std::vector<std::pair<double,
                        std::string>>,
                        std::greater<>> pq;

    // Initialisierung
    for (const auto& stop : stops) {
        distances[stop.second.id] = std::numeric_limits<double>::max();
    }


    distances[fromStopId] = 0.0;
    pq.emplace(0.0, fromStopId);
    for (const auto& stop:vekStop){
        distances[stop.id] = 0.0;
        pq.emplace(0.0, stop.id);
    }

    std::cout << "from: " << fromStopId << std::endl;
    std::cout << "to: " << toStopId << std::endl;

    // Dijkstra-Algorithmus
    while (!pq.empty()) {
        auto [currentDistance, currentStopId] = pq.top();
        pq.pop();

        if (visited.count(currentStopId)) continue;
        visited.insert(currentStopId);


        // Ziel erreicht
        if (currentStopId == toStopId) break;

        // Verarbeite Nachbarn
        for (const auto& neighbor : Network::getNeighbors(currentStopId)) {
            if (visited.count(neighbor)) continue;

            double newDistance = currentDistance + 1.0; // Annahme: Kosten pro Verbindung = 1
            if (newDistance < distances[neighbor]) {
                distances[neighbor] = newDistance;
                previous[neighbor] = currentStopId;
                pq.emplace(newDistance, neighbor);
            }
        }
    }

    // Kein Weg gefunden
    if (distances[toStopId] == std::numeric_limits<double>::max()) {
        std::cerr << "No path found from " << fromStopId << " to " << toStopId << std::endl;
        return path;
    }

    // Pfad rekonstruieren
    for (std::string at = toStopId; at != fromStopId;) {
        auto it = stops.find(at);
        if (it != stops.end()) {
            path.push_back(it->second);
        } else {
            std::cerr << "Error: Stop ID not found in stops map: " << at << std::endl;
            break; // Vermeidet Endlosschleife
        }

        // Überprüfen, ob 'at' im vorherigen Knoten existiert
        auto prevIt = previous.find(at);
        if (prevIt == previous.end()) {
            break; // Vermeidet Endlosschleife
        }
        // Zum nächsten Knoten wechseln
        at = prevIt->second;
    }

    // Startknoten hinzufügen
    auto startIt = stops.find(fromStopId);
    if (startIt != stops.end()) {
        path.push_back(startIt->second);
    }

    // Pfad umkehren
    std::reverse(path.begin(), path.end());
    // std::reverse(path.begin(), path.end());
    return path;
}

std::vector<bht::Stop> Network::getStopsForTransfer(const std::string& stopId){
    std::vector<bht::Stop> result;
    bht::Stop check_stop = getStopById(stopId);
    // std::cout<<"Wahl gefunden"<< sto_p.name <<std::endl;
    if ((check_stop.parentStation.empty()) && (check_stop.locationType == LocationType_Station ) ){
        result.push_back(getStopById(check_stop.id));
        // std::cout<<"1 eine stations gefunden"<<std::endl;
        for (const auto &[stopid,stop]:stops){
            if (stop.parentStation == stopId ){
                    result.push_back(stop);
            }
        }
    }else if (check_stop.locationType ==LocationType_Stop){
        result.push_back(getStopById(check_stop.parentStation));
        // std::cout<<"2 eine stations gefunden"<<std::endl;
        for (const auto &[stopid,stop]:stops){
            if (stop.parentStation == check_stop.parentStation){
                    result.push_back(stop);
            }
        }
    }


    // Sortiere die Stop nach Stop-ID
    std::sort(result.begin(), result.end(), [](const Stop& a, const Stop& b) {
        return a.id < b.id;
    });
    return result;
}
// // for 4a
// std::vector<bht::Stop> Network::getStopsForTransfer(const std::string& stopId){
//     std::vector<bht::Stop> result;
//     bht::Stop sto_p = getStopById(stopId);
//     // std::cout<<"Wahl gefunden"<< sto_p.name <<std::endl;
//     if ((sto_p.parentStation.empty()) && (sto_p.locationType == LocationType_Station ) ){
//         // std::cout<<"1 eine stations gefunden"<<std::endl;
//     for (const auto &[stopid,stop]:stops){
//         if (stopid.find(stopId) != std::string::npos){
//                 result.push_back(stop);
//         }
//     }
//     }else if (sto_p.locationType ==LocationType_Stop){
//         // std::cout<<"2 eine stations gefunden"<<std::endl;
//     // mogel-Funktion
//         for (const auto &[stopid,stop]:stops){
//             if ((stopId!= stop.id) &&(stopId.find(stop.id) != std::string::npos)){
//                 // std::cout<<stop.id<<std::endl;
//                 result = Network::getStopsForTransfer(stop.id);
//                 // result.push_back(stop);
//             }
//         }
//     }

//     // Sortiere die Stop nach Stop-ID
//     std::sort(result.begin(), result.end(), [](const Stop& a, const Stop& b) {
//         return a.id < b.id;
//     });
//     return result;
// }





//For Search from labor 03
//seach from the filtered stop
std::vector<StopTime> Network::searchStopTimesForTrip(std::string query, std::string tripId){
    std::vector<StopTime> result;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    // Suche alle StopTimes für die angegebene Trip-ID
    for (const auto& stopTime : getStopTimesForTrip(tripId)) {
        // Durchlaufe alle Stops und prüfe, ob der Name (oder ein anderes Attribut) den Query enthält
        std::string lowerName = getStopById(stopTime.stopId).name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        if ((lowerName.find(lowerQuery) != std::string::npos)) {
            result.push_back(stopTime);
        }
    }

    // Sortiere die StopTimes nach Stop-Sequenz
    std::sort(result.begin(), result.end(), [](const StopTime& a, const StopTime& b) {
        return a.stopSequence < b.stopSequence;
    });
    return result;

}

//For get stops from stop id
Stop Network::getStopById(std::string stopId){
    Stop result;
    for (const auto& [stopid, stop] : stops){

        if(stopid == stopId){
            result = stop;
        };
    };
    return result;
}

//format HH:MM:SS
std::string Network::padZero(int value) {
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << value;
    return oss.str();
}
//cast GTFStime to string
std::string Network::castTime(GTFSTime input) {
    return padZero((int)input.hour) + ":"
        + padZero((int)input.minute) + ":"
        +padZero((int)input.second);
}

//For stoptime
//get get stoptime from trip ID
std::vector<StopTime> Network::getStopTimesForTrip(std::string tripId) const {
    std::vector<StopTime> result;

    // Suche alle StopTimes für die angegebene Trip-ID
    for (const auto& stopTime : stopTimes) {
        if (stopTime.tripId == tripId) {
            result.push_back(stopTime);
        }
    }

    // Sortiere die StopTimes nach Stop-Sequenz
    std::sort(result.begin(), result.end(), [](const StopTime& a, const StopTime& b) {
        return a.stopSequence < b.stopSequence;
    });

    return result;
}

// For recieving Routes
std::string Network::getRouteDisplayName(Route route){

  if (!route.longName.empty()) {
    return route.shortName + " - " + route.longName;
  }
  // Falls longName leer ist, nur shortName zurückgeben
  return route.shortName;
}
// For display routes
std::vector<Route> Network::getRoutes() const{
    std::vector<bht::Route> results;
    for (const auto& [routeId, route]:routes){
        results.push_back(route);
    }
    return results;
}
// For get Trips for Routes
std::vector<Trip> Network::getTripsForRoute(std::string& routeId){
    std::vector<Trip> result;
    for (const auto& trip : trips) {
        if (trip.routeId == routeId) {  // Filter-Bedingung
            result.push_back(trip);
        }
    }
    return result;
}
//For display trips
std::string Network::getTripDisplayName(Trip trip){
  if(!trip.shortName.empty()){
    return trip.shortName +" - "+ trip.headsign;
  }

  return trip.headsign;

}
//For Search //labor2
std::vector<Stop> Network::search(std::string& query) const {
    std::vector<Stop> results;

    // Transformiere den Query-String in Kleinbuchstaben zur fallunabhängigen Suche
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    // Durchlaufe alle Stops und prüfe, ob der Name (oder ein anderes Attribut) den Query enthält
    for (const auto& [stopId, stop] : stops) {
        std::string lowerName = stop.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        if (lowerName.find(lowerQuery) != std::string::npos) {
            results.push_back(stop);
        }
    }
    return results;
}


//einbringung der GTSF DATEIEN ins Script
// Method for Agencies
void Network::loadAgencies(const std::string& filePath) {
    CSVReader agencyReader{filePath};
    while (agencyReader.hasNext()) {
        if (agencyReader.next()) {
            // std::cout<<filePath<<std::endl;
            Agency agency={
                .id = agencyReader.getField("agency_id",""),
                .name = agencyReader.getField("agency_name",""),
                .url = agencyReader.getField("agency_url",""),
                .timezone = agencyReader.getField("agency_timezone",""),
                .language = agencyReader.getField("agency_lang",""),
                .phone = agencyReader.getField("agency_phone","")
            };
            // std::cout<<agency.id<<std::endl; // Debug output
            agencies[agency.id] = agency;
        }
    }
}

void Network::loadCalendarDates(const std::string& filePath) {
    CSVReader reader{filePath};
    while (reader.hasNext()) {
        if (reader.next()) {
            try {
                // Lese die Felder aus der CSV-Datei
                std::string serviceId = reader.getField("service_id");
                std::string dateStr = reader.getField("date");
                std::string exceptionStr = reader.getField("exception_type", "1");

                // Überprüfe, ob das Datum die erwartete Länge hat
                if (dateStr.length() != 8) {
                    std::cerr << "Invalid date format: " << dateStr << std::endl;
                    continue; // Überspringe ungültige Zeilen
                }

                // Korrekte Typumwandlung für Datum
                unsigned short year = static_cast<unsigned short>(std::stoi(dateStr.substr(0, 4)));
                unsigned char month = static_cast<unsigned char>(std::stoi(dateStr.substr(4, 2)));
                unsigned char day = static_cast<unsigned char>(std::stoi(dateStr.substr(6, 2)));

                // Erstelle den CalendarDate-Eintrag
                CalendarDate calendarDate = {
                    serviceId,
                    {day, month, year}, // Korrekte Initialisierung des Datums
                    static_cast<CalendarDateException>(std::stoi(exceptionStr))
                };

                // Füge den Eintrag zur Liste hinzu
                calendarDates.push_back(calendarDate);

            } catch (const std::exception& e) {
                std::cerr << "Error processing line: " << e.what() << std::endl;
            }
        }
    }
}


// Method for Calendars
void Network::loadCalendars(const std::string& filePath) {
    CSVReader calendarReader(filePath);  // CSVReader ist ein Parser, der CSV-Dateien liest.
    while (calendarReader.hasNext()) {
        if (calendarReader.next()) {
            Calendar calendar;  // Erstelle eine neue Calendar-Instanz.
                // Lese die Felder aus der CSV-Datei aus und weise sie den Membern der Struktur zu.
            calendar.serviceId = calendarReader.getField("service_id");
            calendar.monday = static_cast<CalendarAvailability>(std::stoi(calendarReader.getField("monday")));
            calendar.tuesday = static_cast<CalendarAvailability>(std::stoi(calendarReader.getField("tuesday")));
            calendar.wednesday = static_cast<CalendarAvailability>(std::stoi(calendarReader.getField("wednesday")));
            calendar.thursday = static_cast<CalendarAvailability>(std::stoi(calendarReader.getField("thursday")));
            calendar.friday = static_cast<CalendarAvailability>(std::stoi(calendarReader.getField("friday")));
            calendar.saturday = static_cast<CalendarAvailability>(std::stoi(calendarReader.getField("saturday")));
            calendar.sunday = static_cast<CalendarAvailability>(std::stoi(calendarReader.getField("sunday")));

            // Konvertiere das Datum aus dem Format YYYYMMDD in eine GTFSDate-Struktur.
            calendar.startDate = {
                static_cast<unsigned char>(std::stoi(calendarReader.getField("start_date").substr(6, 2))),
                static_cast<unsigned char>(std::stoi(calendarReader.getField("start_date").substr(4, 2))),
                static_cast<unsigned short>(std::stoi(calendarReader.getField("start_date").substr(0, 4)))
            };
            calendar.endDate = {
                static_cast<unsigned char>(std::stoi(calendarReader.getField("end_date").substr(6, 2))),
                static_cast<unsigned char>(std::stoi(calendarReader.getField("end_date").substr(4, 2))),
                static_cast<unsigned short>(std::stoi(calendarReader.getField("end_date").substr(0, 4)))
            };
            // Füge den Kalender in die `calendars`-Map ein, wobei die `serviceId` als Schlüssel dient.
            calendars[calendar.serviceId] = calendar;
        }
    }
}

void Network::loadFrequencies(const std::string& filePath){
    (void)filePath;
}


// Method for Levels
void Network::loadLevels(const std::string& filePath) {
    CSVReader levelReader(filePath);
    while (levelReader.hasNext()) {
        if (levelReader.next()) {
            Level level;
            // Überprüfen, ob das Feld existiert und nicht leer ist
            try {
                level.id = levelReader.getField("level_id");
                if (level.id.empty()) {
                    std::cerr << "Fehlendes Level ID in Datei: " << filePath << std::endl;
                    continue;
                }

                std::string indexStr = levelReader.getField("level_index");
                if (!indexStr.empty()) {
                    level.index = std::stoi(indexStr);
                } else {
                    std::cerr << "Fehlender Level-Index für ID: " << level.id << std::endl;
                    continue;
                }

                level.name = levelReader.getField("level_name"); // Optional, falls leer akzeptabel ist
                levels[level.id] = level;

            } catch (const std::invalid_argument& e) {
                std::cerr << "Ungültiger Indexwert für Level-ID: " << level.id << " in Datei " << filePath << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Indexwert außerhalb des Bereichs für Level-ID: " << level.id << " in Datei " << filePath << std::endl;
            }
        }
    }
}

// Method for Pathways
void Network::loadPathways(const std::string& filePath) {
    CSVReader pathwaysReader(filePath);
    while (pathwaysReader.hasNext()) {
        if (pathwaysReader.next()) {
            Pathway pathway;
            try {
                pathway.id = pathwaysReader.getField("pathway_id");
                pathway.fromStopId = pathwaysReader.getField("from_stop_id");
                pathway.toStopId = pathwaysReader.getField("to_stop_id");

                std::string modeStr = pathwaysReader.getField("pathway_mode");
                if (!modeStr.empty()) {
                    pathway.mode = static_cast<PathwayMode>(std::stoi(modeStr));
                } else {
                    std::cerr << "Missing pathway_mode for pathway_id: " << pathway.id << std::endl;
                    continue;  // Überspringe Eintrag, wenn wesentliche Daten fehlen
                }

                std::string bidirectionalStr = pathwaysReader.getField("is_bidirectional");
                pathway.isBidirectional = !bidirectionalStr.empty() && std::stoi(bidirectionalStr) != 0;

                std::string lengthStr = pathwaysReader.getField("length");
                pathway.length = !lengthStr.empty() ? std::stof(lengthStr) : 0.0f;

                std::string traversalTimeStr = pathwaysReader.getField("traversal_time");
                pathway.traversalTime = !traversalTimeStr.empty() ? static_cast<unsigned int>(std::stoi(traversalTimeStr)) : 0;

                std::string stairCountStr = pathwaysReader.getField("stair_count");
                pathway.stairCount = !stairCountStr.empty() ? static_cast<unsigned int>(std::stoi(stairCountStr)) : 0;

                std::string maxSlopeStr = pathwaysReader.getField("max_slope");
                pathway.maxSlope = !maxSlopeStr.empty() ? std::stof(maxSlopeStr) : 0.0f;

                std::string minWidthStr = pathwaysReader.getField("min_width");
                pathway.minWidth = !minWidthStr.empty() ? std::stof(minWidthStr) : 0.0f;

                pathway.signpostedAs = pathwaysReader.getField("signposted_as");

                pathways[pathway.id] = pathway;

            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid numeric value in pathway data for pathway_id: " << pathway.id << " - " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Numeric value out of range in pathway data for pathway_id: " << pathway.id << " - " << e.what() << std::endl;
            }
        }
    }
}

// Method for Routes
void Network::loadRoutes(const std::string& filePath){
    CSVReader routesReader(filePath);
    while(routesReader.hasNext()){
        if(routesReader.next()){
            Route route;
            route.id = routesReader.getField("route_id");
            route.agencyId = routesReader.getField("agency_id");
            route.shortName = routesReader.getField("route_short_name");
            route.longName = routesReader.getField("route_long_name");
            route.description = routesReader.getField("route_desc");
            route.type = static_cast<RouteType>(std::stoi(routesReader.getField("route_type")));
            route.color = routesReader.getField("route_color");
            route.textColor = routesReader.getField("route_text_color");
            routes[route.id] = route;
        }
    }
}

//Method for Shapes
void Network::loadShapes(const std::string& filePath){
    CSVReader shapesReader(filePath);
    while(shapesReader.hasNext()){
        if(shapesReader.next()){
            Shape shape;
            shape.id = shapesReader.getField("shape_id");
            shape.latitude = std::stod(shapesReader.getField("shape_pt_lat"));
            shape.longitude = std::stod(shapesReader.getField("shape_pt_lon"));
            shape.sequence = std::stoi(shapesReader.getField("shape_pt_sequence"));
            shapes.push_back(shape);
        }
    }
}

//Method for StopTimes
void Network::loadStopTimes(const std::string& filePath){
    CSVReader stopTimesReader(filePath);
    while (stopTimesReader.hasNext()) {
        if (stopTimesReader.next()) {
            StopTime stopTime;
            stopTime.tripId = stopTimesReader.getField("trip_id");
            std::string arrivalStr = stopTimesReader.getField("arrival_time");
            stopTime.arrivalTime = {
                static_cast<unsigned char>(std::stoi(arrivalStr.substr(0, 2))),
                static_cast<unsigned char>(std::stoi(arrivalStr.substr(3, 2))),
                static_cast<unsigned char>(std::stoi(arrivalStr.substr(6, 2)))
            };
            std::string departureStr = stopTimesReader.getField("departure_time");
            stopTime.departureTime = {                 static_cast<unsigned char>(std::stoi(departureStr.substr(0, 2))),
                static_cast<unsigned char>(std::stoi(departureStr.substr(3, 2))),
                static_cast<unsigned char>(std::stoi(departureStr.substr(6, 2)))
            };

            stopTime.stopId = stopTimesReader.getField("stop_id");
            stopTime.stopSequence = static_cast<unsigned int>(std::stoi(stopTimesReader.getField("stop_sequence")));
            stopTime.pickupType = static_cast<PickupType>(std::stoi(stopTimesReader.getField("pickup_type")));
            stopTime.dropOffType = static_cast<EDropOffType>(std::stoi(stopTimesReader.getField("drop_off_type")));
            stopTime.stopHeadsign = stopTimesReader.getField("stop_headsign");
            stopTimes.push_back(stopTime);
        }
    }
}

//Method for Stops
void Network::loadStops(const std::string& filePath) {
    CSVReader stopsReader{filePath};
    while (stopsReader.hasNext()) {
        if (stopsReader.next()) {
            try {
                Stop stop = {
                    stopsReader.getField("stop_id"),
                    stopsReader.getField("stop_code"),
                    stopsReader.getField("stop_name"),
                    stopsReader.getField("stop_desc"),
                    std::stod(stopsReader.getField("stop_lat")),
                    std::stod(stopsReader.getField("stop_lon")),
                    static_cast<LocationType>(std::stoi(stopsReader.getField("location_type"))),
                    stopsReader.getField("parent_station"),
                    static_cast<WheelchairAccessibility>(std::stoi(stopsReader.getField("wheelchair_boarding"))),
                    stopsReader.getField("platform_code"),
                    stopsReader.getField("level_id"),
                    stopsReader.getField("zone_id")
                };
                // Einfügen des erstellten Stop in die Map mit stop_id als Schlüssel
                stops[stop.id] = stop;

            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid data in stops file: " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range error in stops file: " << e.what() << std::endl;
            }
        }
    }
}


// Method for Transfer
void Network::loadTransfers(const std::string& filePath) {
    CSVReader transferReader{filePath};
    while (transferReader.hasNext()) {
        // Fetch the next line
        if (transferReader.next()) {
            Transfer item = {
                transferReader.getField("from_stop_id"),
                transferReader.getField("to_stop_id"),
                transferReader.getField("from_route_id"),
                transferReader.getField("to_route_id"),
                transferReader.getField("from_trip_id"),
                transferReader.getField("to_trip_id"),
                (TransferType)std::stoi(transferReader.getField("transfer_type")),
                (unsigned int)std::stoi(transferReader.getField("min_transfer_time", "0"))
            };
            transfers.push_back(item);
        }
    }
}

//Method for Trips
void Network::loadTrips(const std::string& path) {
    CSVReader reader{path};
    while (reader.hasNext()) {
        // Fetch the next line
        if (reader.next()) {
            //Trip item = {
            trips.emplace_back(Trip{
                reader.getField("trip_id"),
                reader.getField("route_id"),
                reader.getField("service_id"),
                reader.getField("trip_headsign"),
                reader.getField("trip_short_name"),
                static_cast<TripDirection>(std::stoi(reader.getField("direction_id", "0"))),
                reader.getField("block_id"),
                reader.getField("shape_id"),
                static_cast<WheelchairAccessibility>(std::stoi(reader.getField("wheelchair_accessible", "0"))),
                reader.getField("bikes_allowed", "0") == "1"
            });
            // Convert the bikes_allowed field to a bool explicitly
            //static_cast<bool>(static_cast<BikesAllowed>(std::stoi(reader.getField("bikes_allowed", "0")) == 1))
            //};
            //trips.push_back(item);
        }
    }
}

} // for network bht
