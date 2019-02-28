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

#include "RemoteAntennaSet.h"

void RemoteAntennaSet::addRemoteAntenna(double ruX, double ruY, double ruPow)
{
    RemoteAntenna ru;
    Coord ruPos = Coord(ruX, ruY);
    ru.ruPosition_ = ruPos;
    ru.txPower_ = ruPow;
    remoteAntennaSet_.push_back(ru);
}

Coord RemoteAntennaSet::getAntennaCoord(unsigned int remote)
{
    if (remote >= remoteAntennaSet_.size())
        return Coord(0, 0);
    return remoteAntennaSet_[remote].ruPosition_;
}

double RemoteAntennaSet::getAntennaTxPower(unsigned int remote)
{
    if (remote >= remoteAntennaSet_.size())
        return 0.0;
    return remoteAntennaSet_[remote].txPower_;
}

unsigned int RemoteAntennaSet::getAntennaSetSize()
{
    return remoteAntennaSet_.size();
}

std::ostream &operator << (std::ostream &stream, const RemoteAntennaSet* ruSet)
{
    if (ruSet == NULL)
        return (stream << "Empty set");
    for (unsigned int i = 0; i < ruSet->remoteAntennaSet_.size(); i++)
    {
        stream << "RU" << i << " : " << "Pos = (" <<
            ruSet->remoteAntennaSet_[i].ruPosition_.x << "," <<
            ruSet->remoteAntennaSet_[i].ruPosition_.y << ") ; txPow = " <<
            ruSet->remoteAntennaSet_[i].txPower_ << " :: ";
    }
    return stream;
}
