#include "scheduled_trip.h"
#include "network.h"

namespace bht {
    // NetworkScheduledTrip::iterator Implementierung
    NetworkScheduledTrip::iterator::iterator(pointer ptr) : current(ptr) {}

    NetworkScheduledTrip::iterator::reference NetworkScheduledTrip::iterator::operator*() const {
        return *current;
    }

    NetworkScheduledTrip::iterator::pointer NetworkScheduledTrip::iterator::operator->() const {
        return current;
    }

    NetworkScheduledTrip::iterator& NetworkScheduledTrip::iterator::operator++() {
        ++current;
        return *this;
    }

    NetworkScheduledTrip::iterator NetworkScheduledTrip::iterator::operator++(int) {
        iterator temp = *this;
        ++current;
        return temp;
    }

    NetworkScheduledTrip::iterator& NetworkScheduledTrip::iterator::operator--() {
        --current;
        return *this;
    }

    NetworkScheduledTrip::iterator NetworkScheduledTrip::iterator::operator--(int) {
        iterator temp = *this;
        --current;
        return temp;
    }

    bool NetworkScheduledTrip::iterator::operator==(const iterator& other) const {
        return current == other.current;
    }

    bool NetworkScheduledTrip::iterator::operator!=(const iterator& other) const {
        return current != other.current;
    }

    // NetworkScheduledTrip Implementierung
    NetworkScheduledTrip::NetworkScheduledTrip(const std::string& tripId, const std::vector<StopTime>& stopTimes)
        : tripId(tripId), stopTimes(stopTimes) {}
    NetworkScheduledTrip::NetworkScheduledTrip(const NetworkScheduledTrip&){
    
    };
    NetworkScheduledTrip::NetworkScheduledTrip(NetworkScheduledTrip&& ){
    };

    NetworkScheduledTrip::iterator NetworkScheduledTrip::begin() const {
        return iterator(stopTimes.data());
    }

    NetworkScheduledTrip::iterator NetworkScheduledTrip::end() const {
        return iterator(stopTimes.data() + stopTimes.size());
    }
}