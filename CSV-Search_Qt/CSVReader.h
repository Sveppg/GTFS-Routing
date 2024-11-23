#pragma once
#ifndef CSVREADER_HPP
    #define CSVREADER_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <vector>

class CSVReader{
    public:
        explicit CSVReader(const std::string& filename);
        std::string getField(const std::string key, const std::string& defaultValue );
        std::string getField(const std::string key);
        bool next();
        bool hasNext() const;
        void reset();
    private:
        mutable std::ifstream file;
        std::vector<std::string> currentRow; // Speichert die aktuellen Zeilenwerte
        std::vector<std::string> header; // // Zuordnung von Spaltennamen
        // std::unordered_map<std::string, size_t> header; // Zuordnung von Spaltennamen zu Indizes
        std::string currentLine;

        void parseHeader();
        bool parseLine();
};
#endif