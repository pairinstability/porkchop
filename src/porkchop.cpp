/**
 * porkchop.cpp
 * generate the porkchop plot data and write it to a file so it can be
 * plotted (with a python script and matplotlib)
 * 
 * alexander 
 * 2022-06-05
 * 
 */

#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <porkchop.hpp>

/* constants for solving lamberts problem */
#define MULTI_REVS              (0) 
#define MU                      (ASTRO_MU_SUN)


void Porkchop::generate()
{
    /* For each depature epoch, to solve the lambert problem for each
     * time of flight where the ordinate is the departure epoch + tof.
     * for each solution, compute the C3 and plot it as a contour.
     */
    
    /* number of days between the upper and lower departure dates */
    double departure_range = m_departure_upper.mjd2000() -
                                m_departure_lower.mjd2000();

    /* increment by 1 so we get the full range */
    departure_range += 1;
    m_tof_upper += 1;

    /* plotting */
    double x_size = departure_range;
    double y_size = m_tof_upper - m_tof_lower;

    /* 2D array to save the data to so we can write it to a file */
    /* its bounded by departure range and tof range */
    std::vector<std::vector<double>> c30_t1, c3f_t1, c30_t2, c3f_t2,
                                        dep_epoch, arr_epoch;

    /* C3 values used for plotting */
    c30_t1.resize(x_size);
    c3f_t1.resize(x_size);
    c30_t2.resize(x_size);
    c3f_t2.resize(x_size);
    dep_epoch.resize(x_size);
    arr_epoch.resize(x_size);

    
    for (int i=0; i < x_size; i++)
    {
        c30_t1[i].resize(y_size);
        c3f_t1[i].resize(y_size);
        c30_t2[i].resize(y_size);
        c3f_t2[i].resize(y_size);
        dep_epoch[i].resize(y_size);
        arr_epoch[i].resize(y_size);
    }

    kep_toolbox::epoch base_epoch(1970,1,1);

    /* departure loop */
    for (int i = 0; i < departure_range; i++)
    {
        /* arrival position and velocity vectors */
        kep_toolbox::array3D r0, v0;
        /* time of departure in MJD2000 */
        double dep_mjd2000;

        dep_mjd2000 = m_departure_lower.mjd2000() + i;

        /* get departure ephemeris */
        /* jpl_lp::eph_impl wants epoch in MJD2000 and returns r0 as m and v0
         * as m/s */
        m_departure_body.eph(dep_mjd2000, r0, v0);

        /* tof loop */
        for (int j = m_tof_lower; j < m_tof_upper; j++)
        {
            /* departure position and velocity vectors */
            kep_toolbox::array3D r1, v1;
            kep_toolbox::array3D tv0, tv1;

            /* delta V */
            kep_toolbox::array3D dv0, dvf;

            kep_toolbox::array3D ir1, ir2, ih;
            bool flip_type = false;

            double dv0_norm, dvf_norm;
            /* time of flight in seconds */
            double tof_s;
            /* time of arrival in MJD2000 */
            double arr_mjd2000;

            tof_s = j * ASTRO_DAY2SEC;

            arr_mjd2000 = dep_mjd2000 + j;

            /* get arrival ephemeris */
            /* jpl_lp::eph_impl wants epoch in MJD2000 and returns r1 as m and
             * v1 and m/s */
            m_arrival_body.eph(arr_mjd2000, r1, v1);

            kep_toolbox::vers(ir1,r0);
            kep_toolbox::vers(ir2,r1);
            kep_toolbox::cross(ih, ir1, ir2);
            kep_toolbox::vers(ih,ih);

            /* transfer angle is > 180deg as seen from above z axis, so
             * we need to un-flip the type1 and type2 C3 values. the pykep
             * lambert solver is really weird and it makes no sense why it
             * would flip them in the first place */
            if (ih[2] < 0.0)
            {
                flip_type = true;
            }

            /* ============================================================= */
            /* solve for type 1 (prograde), the short way */
            kep_toolbox::lambert_problem lamb1(r0, r1, tof_s, MU, false, MULTI_REVS);

            tv0 = lamb1.get_v1()[0];
            tv1 = lamb1.get_v2()[0];

            kep_toolbox::diff(dv0, tv0, v0);
            kep_toolbox::diff(dvf, v1, tv1);

            /* div by 1000 to go from m/s -> km/s */
            dv0_norm = kep_toolbox::norm(dv0)/1000;
            dvf_norm = kep_toolbox::norm(dvf)/1000;

            /* save as C3 */
            if (flip_type)
            {
                c30_t2[i][j-m_tof_lower] = std::pow(dv0_norm, 2);
                c3f_t2[i][j-m_tof_lower] = std::pow(dvf_norm, 2);
            }
            else
            {
                c30_t1[i][j-m_tof_lower] = std::pow(dv0_norm, 2);
                c3f_t1[i][j-m_tof_lower] = std::pow(dvf_norm, 2);
            }


            /* ============================================================= */
            /* solve for type 2 (retrograde), the long way */
            kep_toolbox::lambert_problem lamb2(r0, r1, tof_s, MU, true, MULTI_REVS);
            
            tv0 = lamb2.get_v1()[0];
            tv1 = lamb2.get_v2()[0];

            kep_toolbox::diff(dv0, tv0, v0);
            kep_toolbox::diff(dvf, v1, tv1);

            /* div by 1000 to go from m/s -> km/s */
            dv0_norm = kep_toolbox::norm(dv0)/1000;
            dvf_norm = kep_toolbox::norm(dvf)/1000;

            /* save as C3 */
            if (flip_type)
            {
                c30_t1[i][j-m_tof_lower] = std::pow(dv0_norm, 2);
                c3f_t1[i][j-m_tof_lower] = std::pow(dvf_norm, 2);
            }
            else
            {
                c30_t2[i][j-m_tof_lower] = std::pow(dv0_norm, 2);
                c3f_t2[i][j-m_tof_lower] = std::pow(dvf_norm, 2);
            }

            /* converting to matplotlib dates number */
            dep_epoch[i][j-m_tof_lower] =
                (dep_mjd2000 - base_epoch.mjd2000());
            arr_epoch[i][j-m_tof_lower] =
                (arr_mjd2000 - base_epoch.mjd2000());

        }
    }


    /* write the data to a file, because plotting in C++ is a little jank
     * and I already have the code to plot a pretty slick plot with
     * matplotlib */
    std::ofstream filestream("porkchop_data", std::ios::out);

    for (int x = 0; x < x_size; x++)
    {
        for (int y = 0; y < y_size; y++)
        {
            /* c30_t1, c3f_t1, c30_t2, c3f_t2, dep_epoch, arr_epoch */
            filestream << c30_t1[x][y] << "," <<
                c3f_t1[x][y] << "," << c30_t2[x][y] << "," << c3f_t2[x][y] <<
                "," << dep_epoch[x][y] << "," << arr_epoch[x][y] << std::endl;
        }
    }
}

Porkchop::~Porkchop()
{
    // destruct
}
