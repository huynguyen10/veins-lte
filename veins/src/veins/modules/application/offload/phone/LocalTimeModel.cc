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


#include "LocalTimeModel.h"

LocalTimeModel::LocalTimeModel(std::string name, ConfigMap& configs) {
	this->name = name;
	this->configs = configs;
}

double LocalTimeModel::getLocalTime(double frequency, double windowLength) {
	// pattern: "f_10.0,wl_0.2", "f_50.0,wl_1.5"
    char buf[15];
    std::sprintf(buf, "f_%.1f,wl_%.1f", frequency, windowLength);
    std::string config(buf);

    ParameterMap paras = configs[config];
    double sd = paras["sd"];
    double mean = paras["mean"];

    return (mean + uniform(-sd, sd)) / 1000;
}
