#include "csvloader.h"

Table read_csv(std::istream &stream, char delimiter) {
    Table table;

    std::stringstream elementbuff;
    Row row;

    while ( !stream.eof() ) {
        char c = stream.get();

        if (c == EOF) break;
        
        if (c != delimiter && c != '\n') {
            elementbuff << c;
            continue;
        }

        row.push_back ( elementbuff.str() );
        elementbuff.str(std::string());

        if ( c == '\n' && row.size()) {
            table.push_back( row );
            row.clear(); 
        }
    }

    elementbuff.seekg(0, std::ios::end);
    int size = elementbuff.tellg();
    if (size > 0) row.push_back(elementbuff.str());
    if (row.size() > 0) table.push_back( row );

    return table;
}

ThreatBase table_to_base(const Table &table) {
    ThreatBase tb;

    int line = 0;
    for (const Row &row: table) {
        line++;
        if (row.size() < 1) {
            // Give warning too short line
            Logger.level(LEVEL_WARNING) << "Line " << line << " is too short: expected hashstring & desctiption" << std::endl; 
            continue;
        }

        
        std::string hashtring = row[0]; 
        
        if (tb.contains(hashtring)) {
            // Give watning two lines with same hash
            Logger.level(LEVEL_WARNING) << "Line " << line << " contains dublicate hash : " << hashtring << std::endl; 
            continue;
        }
        
        ThreatDescription td;

        if (row.size() < 2) {
            Logger.level(LEVEL_WARNING) << "Line " << line << " does not contain description: standart description used" << std::endl; 
            td.name = "No desciption provided";
        } else td.name = std::string(row[1].begin(), row[1].end());
        
        tb.emplace(hashtring, td);
    }

    return tb;
}