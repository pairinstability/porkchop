/**
 * porkchop.hpp
 * generate the porkchop plot 
 * 
 * alexander 
 * 2022-05-29
 * 
 */

#pragma once

#include <string>

#include <keplerian_toolbox/keplerian_toolbox.hpp>


class Porkchop {
public:
    Porkchop(
        kep_toolbox::planet::jpl_lp departure_body,
        kep_toolbox::planet::jpl_lp arrival_body, 
        kep_toolbox::epoch departure_lower,
        kep_toolbox::epoch departure_upper,
        double tof_lower,
        double tof_upper)
    :
    m_departure_body(departure_body),
    m_arrival_body(arrival_body),
    m_departure_lower(departure_lower),
    m_departure_upper(departure_upper),
    m_tof_lower(tof_lower),
    m_tof_upper(tof_upper) {}
    
    void generate(void);
    virtual ~Porkchop();

    kep_toolbox::planet::jpl_lp m_departure_body, m_arrival_body;
    kep_toolbox::epoch m_departure_lower, m_departure_upper;
    double m_tof_lower, m_tof_upper;
};
