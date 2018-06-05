//
//  read_pgn.cpp
//  CGV
//
//  Created by Elias Piovani on 23/05/2018.
//  Copyright © 2018 Elias Piovani. All rights reserved.
//

#include "read_pgn.hpp"

bool openPGN(char *path, std::string &header, std::vector<std::string> &turns)
{
    char headerLine[100];   // buffer to save one line
    char c = '\0';  // character of the body of the file
//    int plyCount = 0;   // counts how many turns the file contains
    std::string buffer; // to append the characters for white and black moves
    
    // open file with the received path
    FILE * file = std::fopen(path, "r");
    // if file is not found print error message
    if( file == NULL ){
        printf("Impossible to open the file !n");
        getchar();
        return false;
    }
    // read all the header lines and save it in header
//    while (strcmp(fgets(headerLine, 100, file), "\n"))
//        //        printf("%s", headerLine);
//        header += headerLine;
    for (int i = 0; i < 13; i++) {
        fgets(headerLine, 100, file);
        header += headerLine;
    }
    
    // reads all the file until the end
    while (c != EOF)
    {
        buffer = "\0";
        
        // ignore the number that inform the what turn is
        while (c != '.') {
            c = fgetc(file);
            if (c == EOF) break;
        }
        
        // ignore '.' and ' '
        c = fgetc(file);
        if (c == EOF) break;
        if (c == ' ' || c == '\n') c = fgetc(file);
        if (c == EOF) break;
        
//        plyCount++;
        //        printf("%i ", plyCount);
        
        // read white move
        while (c != ' ' && c != '\n') {
            //            printf("%c", c);
            buffer += c;
            c = fgetc(file);
            if (c == EOF) break;
        }
        
        //        printf("%s ", buffer.c_str());
        // jump " " between moves
        turns.push_back(buffer);
        buffer = "\0";
        if (c == ' ' || c == '\n') c = fgetc(file);
        if (c == EOF) break;
        
        // read black move
        while (c != ' ' && c != '\n') {
            //            printf("%c", c);
            buffer += c;
            c = fgetc(file);
            if (c == EOF) break;
        }
        //        printf("%s \n", buffer.c_str());
        turns.push_back(buffer);
        //        printf("\n");
    }
    
    if (turns.back()[0] == '0' || turns.back()[0] == '1') {
        turns.pop_back();
    }
    
    printf("Loaded %lu turns\n", turns.size());
//    for (std::vector<std::string>::iterator it = turns.begin() ; it != turns.end(); ++it) {
//        printf("%s ", it->c_str());
//    }
    
    return true;
}

