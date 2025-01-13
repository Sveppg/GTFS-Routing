#pragma once
#ifndef SCHEDULED_HPP
#define SCHEDULED_HPP

#include <vector>
#include <string>
#include "types.h"  // Enthält StopTime, Trip, etc.

namespace bht {
    class  NetworkScheduledTrip {
    public:
        class iterator {
        public:
            using value_type = StopTime;
            using pointer = const StopTime*;
            using reference = const StopTime&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            iterator(pointer ptr);

            reference operator*() const;
            pointer operator->() const;

            iterator& operator++();    // Prefix-Inkrement
            iterator operator++(int); // Postfix-Inkrement
            iterator& operator--();    // Prefix-Dekrement
            iterator operator--(int); // Postfix-Dekrement

            bool operator==(const iterator& other) const;
            bool operator!=(const iterator& other) const;

        private:
            pointer current;
        };

        NetworkScheduledTrip(const std::string& tripId, const std::vector<StopTime>& stopTimes);
        NetworkScheduledTrip(const NetworkScheduledTrip& other);
        NetworkScheduledTrip(NetworkScheduledTrip&& other); 

        iterator begin() const;
        iterator end() const;

    private:
        std::string tripId;
        std::vector<StopTime> stopTimes;
    };
}
#endif