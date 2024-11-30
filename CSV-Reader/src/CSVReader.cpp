#include "CSVReader.h"
#include "types.h"
#include <sstream>
#include <iostream> 
#include <fstream>

CSVReader::CSVReader(const std::string& filename) : file(filename) {
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file"+  filename);
    }
    parseHeader();
}


void CSVReader::parseHeader() {
    std::string value= ""; 
    getline(file,currentLine);
        bool inBrackets = false;
    for (char c : currentLine){ //von der muster lösung inspiriert TODO bitte kommentar löschen
        if (c == '\"'){
            inBrackets =!inBrackets;
        }else if ((c==',') && inBrackets==false ){ 
            header.push_back(value)  ;
            value="";
        } else if (c != '\n' && c != '\r'){
            value = value + c;
        }
    }
    header.push_back(value);
}

bool CSVReader::parseLine() { //Better
    currentRow.clear();
    if (!std::getline(file, currentLine)) {
        return false; // Keine weitere Zeile vorhanden
    }

    bool inBrackets = false;
    std::string value = ""; 

    for (char c : currentLine) {
        if (c == '\"') {
            inBrackets = !inBrackets;
        } else if ((c == ',') && !inBrackets) { 
            currentRow.push_back(value);
            value = "";
        } else if (c != '\n' && c != '\r') {
            value += c;
        }
    }
    currentRow.push_back(value);
    return true;
}

/*
bool CSVReader::parseLine() {
    currentRow.clear();
    getline(file,currentLine);
        bool inBrackets = true;
    //std::cout << currentLine<<std::endl;
    std::string value= ""; 

    for (char c : currentLine){ //von der muster lösung inspiriert TODO bitte kommentar löschen
        if (c == '\"'){
            inBrackets =!inBrackets;
        }else if ((c==',') && inBrackets ){ 
            // std::cout <<value<<std::endl;
            currentRow.push_back(value);
            value= "";
        } else if (c != '\n' && c != '\r'){
            value = value + c;
        }
    }
    currentRow.push_back(value);
    // for (int i = 0; i < currentRow.size() ; i++){
    //     std::cout << currentRow[i]<< std::endl;
    // }
    return true;
}*/
std::string CSVReader::getField(const std::string key, const std::string& defaultValue) {
    // int it = header.find(key);
    // if (it != header.end() && it < currentRow.size() && !currentRow[it->second].empty()) {
    //     return currentRow[it->second];
    // }
    // return defaultValue;
    //huy TODO lesen
    std::string result=""; 
    for (size_t i =0 ; i < header.size(); i++){
        if(key == header[i]){
            result = currentRow[i];
        }
    }
    if (result == ""){
        return defaultValue;
    }else {
        // for (int i =0 ; i <<result.size();i++){
        // std::cout<<result[i]<<std::endl; 
        // }
        return result;
    }
}


std::string CSVReader::getField(const std::string key) {
    return getField(key, "");
}

bool CSVReader::next() { // Better
    if (!hasNext()) {
        return false;
    }
    return parseLine();
}

/*
bool CSVReader::next() {
    if (!file.good()){
        return false;
    }
    return parseLine(); 
}*/


bool CSVReader::hasNext() const { // Better
    return file && !file.eof();
}
/*
bool CSVReader::hasNext() const {
    //std::cout << "has Next geht" << std::endl; // Debugging
    //return !file.eof();
    return file.good();
}*/

void CSVReader::reset() {
    file.clear();
    file.seekg(0);
    parseHeader();
}
