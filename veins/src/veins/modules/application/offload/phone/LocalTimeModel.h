//
// Copyright (C) 2018 Quang-Huy Nguyen <nguyen@ccs-labs.org>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef __LOCALTIMEMODEL_H_
#define __LOCALTIMEMODEL_H_

#include <map>
#include <string>
#include <omnetpp.h>

/**
 * @brief
 * Models for local processing time
 *     localTime = mean +/- sd
 *
 * @author Quang-Huy Nguyen
 */

/**
 * @brief Used at initialisation to pass the parameters
 * to the LocalTimeModel
 */
typedef std::map<std::string, cMsgPar> ParameterMap;

/** @brief Used to store the LocalTimeModels to be used.*/
typedef std::map<std::string, ParameterMap> ConfigMap;

class LocalTimeModel {

protected:
	/** @brief Model name. **/
	std::string name;

    /** @brief List of the local time models to use.*/
    ConfigMap configs;

public:
    LocalTimeModel(std::string name, ConfigMap& configs);

    std::string getName() {
    	return this->name;
    }

    ConfigMap getConfigMap() {
    	return this->configs;
    }

    virtual ~LocalTimeModel() {}

    /**
     * @brief Return the local processing corresponding to
     * sampling frequency and window length.
     */
	virtual double getLocalTime(double frequency, double windowLength);
};

#endif //__LOCALTIMEMODEL_H_
