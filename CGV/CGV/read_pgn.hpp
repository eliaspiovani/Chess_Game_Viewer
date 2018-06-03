//
//  read_pgn.hpp
//  CGV
//
//  Created by Elias Piovani on 23/05/2018.
//  Copyright © 2018 Elias Piovani. All rights reserved.
//

#ifndef read_pgn_hpp
#define read_pgn_hpp

#include <stdio.h>
#include <vector>
#include <string>

#include "Object.hpp"

bool openPGN(char *path, std::string &header, std::vector<std::string> &turns);

#endif /* read_pgn_hpp */
