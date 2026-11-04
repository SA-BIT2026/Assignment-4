/**
 * A5Lib.cpp
 * @author kisslune 
 */

#include "A5Header.h"

void Andersen::dumpResult()
{
    std::string fname = SVF::PAG::getPAG()->getModuleIdentifier() + ".res.txt";
    std::ofstream outFile(fname, std::ios::out);
    if (!outFile)
    {
        std::cout << "error opening " + fname + "!!\n";
        return;
    }

    // Write S-edges
    for (auto &pointerIt : pts)
    {
        outFile << pointerIt.first << " points to: {";
        for (auto pointee : pointerIt.second)
        {
            outFile << pointee << ", ";
        }
        outFile << "}\n";
    }
}