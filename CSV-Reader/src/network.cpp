#include "network.h"
#include "CSVReader.h"
#include "types.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>

using namespace bht ;

Network::Network(const std::string& directoryPath) {
    // Laden der Daten aus den Dateien
    std::cout << "load Agency" << std::endl;
    loadAgencies(directoryPath + "/agency.txt");
    std::cout << "load CalendarDates" << std::endl;
    loadCalendarDates(directoryPath + "/calendar_dates.txt");
    std::cout << "load Calendar" << std::endl;
    loadCalendars(directoryPath + "/calendar.txt");
    std::cout << "load Frequencie" << std::endl;
    loadFrequencies(directoryPath + "/frequencies.txt");
    std::cout << "load Level" << std::endl;
    loadLevels(directoryPath + "/levels.txt");
    std::cout << "load Pathway" << std::endl;
    loadPathways(directoryPath + "/pathways.txt"); 
    std::cout << "load Routes" << std::endl;
    loadRoutes(directoryPath + "/routes.txt");
    std::cout << "load Shapes" << std::endl;
    loadShapes(directoryPath +"/shapes.txt");
    std::cout << "load StopTimes" << std::endl;
    loadStopTimes(directoryPath + "/stop_times.txt");
    std::cout << "load Stops" << std::endl;
    loadStops(directoryPath + "/stops.txt");
    std::cout << "load transfer" << std::endl;
    loadTransfers(directoryPath + "/transfers.txt");
    std::cout << "load trips" << std::endl;
    loadTrips(directoryPath + "/trips.txt");

}

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

/*
//Method for CalendarDates
void Network::loadCalendarDates(const std::string& filePath){
    CSVReader calendarDatesReader(filePath);
    while(calendarDatesReader.hasNext()){
        if(calendarDatesReader.next()){
            CalendarDate calendarDate;
            calendarDate.serviceId = calendarDatesReader.getField("service_ID");
            std::string dateStr = calendarDatesReader.getField("date");  // Ersetze "date_column_name" durch den tatsächlichen Spaltennamen.
            if (dateStr.length() == 8) {  // Annahme: Datum im Format YYYYMMDD
                calendarDate.date.year = std::stoi(dateStr.substr(0, 4));    // Jahr (die ersten 4 Zeichen)
                calendarDate.date.month = std::stoi(dateStr.substr(4, 2));   // Monat (die nächsten 2 Zeichen)
                calendarDate.date.day = std::stoi(dateStr.substr(6, 2));     // Tag (die letzten 2 Zeichen)
            } else { 
                std::cerr << "Invalid date format in CSV: " << dateStr << std::endl;
            }
        }
    }
}
*/
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
	std::cout << filePath << std::endl;
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

