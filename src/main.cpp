/**
 * main.cpp
 * porkchop plotting tool
 * 
 * Parses JSON input to generate a porkchop plot.
 * 
 * alexander 
 * 2022-05-29
 * 
 */

#include <iostream>
#include <fstream>
#include <json.hpp>
#include <porkchop.hpp>


int main(int argc, char* argv[])
{
    double tof_lower;
    double tof_upper;
    std::string departure_body, arrival_body;

    /* parse the json file */
    std::fstream input("input.json");
    nlohmann::json jf = nlohmann::json::parse(input);

    /* upper and lower departure upper and lower bounds epochs as
     * gregorian dates */
    kep_toolbox::epoch departure_lower(
            (double)jf["window"]["departure_lower"]["y"],
            (double)jf["window"]["departure_lower"]["m"],
            (double)jf["window"]["departure_lower"]["d"]);

    kep_toolbox::epoch departure_upper(
            (double)jf["window"]["departure_upper"]["y"],
            (double)jf["window"]["departure_upper"]["m"],
            (double)jf["window"]["departure_upper"]["d"]);

    tof_lower = jf["window"]["tof_lower"];
    tof_upper = jf["window"]["tof_upper"];

    departure_body = jf["body"]["departure"];
    arrival_body = jf["body"]["arrival"];


    /* this will return the posix time in seconds for the epochs */
    std::cout << "departure lower bound: " << departure_lower << std::endl;
    std::cout << "departure upper bound: " << departure_upper << std::endl;

    std::cout << "time of flight lower bound [days]: " << tof_lower << std::endl;
    std::cout << "time of flight upper bound [days]: " << tof_upper << std::endl;

    Porkchop porkchop(
        kep_toolbox::planet::jpl_lp(departure_body),
        kep_toolbox::planet::jpl_lp(arrival_body),
        departure_lower,
        departure_upper,
        tof_lower,
        tof_upper);
    
    porkchop.generate();
    
    return 0;
}
