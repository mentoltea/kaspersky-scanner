#ifndef CSVLOADER_H
#define CSVLOADER_H

#ifndef PREFIX
    #define PREFIX
#endif

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "logger.h"

typedef std::string Element;
typedef std::vector<Element> Row;
typedef std::vector<Row> Table;

PREFIX Table read_csv(std::istream &stream, char delimiter = ';');

struct ThreatDescription {
    std::string name;
    // Возможно, другие поля описания
    // уровень угрозы
    // ...
};

typedef std::map<std::string, ThreatDescription> ThreatBase; // hashstring to description

PREFIX ThreatBase table_to_base(const Table &table);

#endif // CSVLOADER_H