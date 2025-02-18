#pragma once
#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.h"
#include "scheduled_trip.h"

namespace bht {

class Network {
public:
    explicit Network(const std::string& directoryPath);
    
    std::unordered_map<std::string, Agency> agencies;
    std::vector<CalendarDate> calendarDates;
    std::unordered_map<std::string, Calendar> calendars;
    std::unordered_map<std::string, Level> levels;
    std::unordered_map<std::string, Pathway> pathways;
    std::unordered_map<std::string, Route> routes;
    std::vector<Shape> shapes;
    std::vector<StopTime> stopTimes;
    std::unordered_map<std::string, Stop> stops;
    std::vector<Transfer> transfers;
    std::vector<Trip> trips;
    std::vector<Stop> search(std::string& query) const;
    std::vector<Route> getRoutes() const; // auch verwendet in Üb 6
    std::string getRouteDisplayName(Route route);
    std::vector<Trip> getTripsForRoute(std::string& routeId); // auch verwendet in Üb 6
    std::string getTripDisplayName(Trip trip);
    std::vector<StopTime> getStopTimesForTrip(std::string tripId)const;
    std::string castTime(GTFSTime input);
    std::string padZero(int value);
    Stop getStopById(std::string stopId);
    std::vector<StopTime> searchStopTimesForTrip(std::string query, std::string tripId);
    
    //getStopsForTransfer Input stopID -> Output all stops within the station 
    std::vector<bht::Stop> getStopsForTransfer(const std::string& stopId);

    std::unordered_set<std::string> getNeighbors(const std::string& stopId);
    std::vector<bht::Stop> getTravelPath(const std::string& fromStopId, const std::string& toStopId);
    void buildStopIdToTrips(const std::vector<StopTime> &stopTimes);
    void buildTripIdToStopTimes(const std::vector<StopTime> &stopTimes);


    NetworkScheduledTrip getScheduledTrip(const std::string& tripId) const;

    //map for conversion trip -> StopTimes StopId -> Trips
    std::unordered_map<std::string , std::vector<StopTime>> tripIdToStopTimes;
    std::unordered_map<std::string , std::vector<std::string>> stopIdToTrips;

    //for ub 6
    std::unordered_map<std::string, std::vector<Trip>> shapeIdfromtrip; // loadTrips trips load into unordered_map
    std::unordered_map<std::string, std::vector<Shape>> routeShapes;
    std::unordered_map<std::string, std::vector<Shape>> tripShapes; //Trips-Id zu Shapes load into unordered_map
    std::unordered_map<std::string, std::string> routeColors;



private:
    void loadAgencies(const std::string& filePath);
    void loadCalendarDates(const std::string& filePath);
    void loadCalendars(const std::string& filePath);
    void loadFrequencies(const std::string& filePath);
    void loadLevels(const std::string& filePath);
    void loadPathways(const std::string& filePath);
    void loadRoutes(const std::string& filePath);
    void loadShapes(const std::string& filePath);
    void loadStopTimes(const std::string& filePath);
    void loadStops(const std::string& filePath);
    void loadTransfers(const std::string& filePath);
    void loadTrips(const std::string& filePath);
};

}
#endif
