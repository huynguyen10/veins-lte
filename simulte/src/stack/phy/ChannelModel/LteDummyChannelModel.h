//
//                           SimuLTE
// Copyright (C) 2012 Antonio Virdis, Daniele Migliorini, Giovanni
// Accongiagioco, Generoso Pagano, Vincenzo Pii.
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _LTE_LTEDUMMYCHANNELMODEL_H_
#define _LTE_LTEDUMMYCHANNELMODEL_H_

#include "LteChannelModel.h"

class LteDummyChannelModel : public LteChannelModel
{
  protected:
    double per_;
    double harqReduction_;
    public:
    LteDummyChannelModel(ParameterMap& params, int band);
    virtual ~LteDummyChannelModel();
    /*
     * Compute the error probability of the transmitted packet
     *
     * @param frame pointer to the packet
     * @param lteinfo pointer to the user control info
     */
    virtual bool error(LteAirFrame *frame, UserControlInfo* lteInfo);
    /*
     * Compute Attenuation caused by pathloss and shadowing (optional)
     */
    virtual double getAttenuation(MacNodeId nodeId, Direction dir, Coord coord)
    {
        return 0;
    }
    /*
     * Compute FAKE sir for each band for user nodeId according to multipath fading
     *
     * @param frame pointer to the packet
     * @param lteinfo pointer to the user control info
     */
    virtual std::vector<double> getSIR(LteAirFrame *frame, UserControlInfo* lteInfo);
    /*
     * Compute FAKE sinr for each band for user nodeId according to pathloss, shadowing (optional) and multipath fading
     *
     * @param frame pointer to the packet
     * @param lteinfo pointer to the user control info
     */
    virtual std::vector<double> getSINR(LteAirFrame *frame, UserControlInfo* lteInfo);
    //TODO
    virtual bool errorDas(LteAirFrame *frame, UserControlInfo* lteI)
    {
        opp_error("DAS PHY LAYER TO BE IMPLEMENTED");
        return false;
    }
};

#endif
