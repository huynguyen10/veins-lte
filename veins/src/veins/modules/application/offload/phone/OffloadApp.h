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

#ifndef __VEINS_OFFLOADAPP_H_
#define __VEINS_OFFLOADAPP_H_

#include <map>
#include <vector>
#include <string>

#include <omnetpp.h>
#include "veins/modules/application/heterogeneous/IHeterogeneousApp.h"
#include "veins/modules/application/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/base/utils/SimpleLogger.h"
#include "LocalTimeModel.h"

/**
 * @brief
 * A simple application that sends messages via the DONTCARE channel to other cars and sometimes a
 * message to the server via LTE.
 *
 * @author Quang-Huy Nguyen
 */

class LocalTimeModel;

class OffloadApp: public IHeterogeneousApp {

private:
	cMessage *dataReadyEvent;
	cMessage *sendAfterEvent;

	HeterogeneousMessage* resultMsg;

protected:
    std::string mode;
    std::string classifier;
    int resultMsgLength;

	double samplingFreq;
	double windowLength;

	/** @brief Used to store the LocalTimeModels to be used.*/
	typedef std::vector<LocalTimeModel*> LocalTimeModelList;

	LocalTimeModel* localTimeModel;

	/** @brief List of the local time models to use.*/
	LocalTimeModelList localTimeModels;

	/** Variables for statistic collection */
	simsignal_t localTimeSignal;
	simsignal_t serverToPhoneDelaySignal;
	simsignal_t offloadDelaySignal;
	long numSentSignal;
	long numReceivedSignal;

protected:
	virtual void initialize(int stage);
	virtual void handleMessage(cMessage *msg);
	virtual void finish();

	/**
	 * @brief Creates and returns an instance of the LocalTimeModel with the
	 * specified name.
	 *
	 * The returned LocalTimeModel has to be
	 * generated with the "new" command. The OffloadApp
	 * keeps the ownership of the returned LocalTimeModel.
	 *
	 * This method is used by the OffloadApp during
	 * initialization to load the LocalTimeModels which
	 * has been specified in the ned file.
	 *
	 * This method has to be overridden if you want to be
	 * able to load your own LocalTimeModels.
	 */
	virtual LocalTimeModel* getLocalTimeModelFromName(std::string name);

private:
	/**
	 * @brief Utility function. Reads the configs of a XML element
	 * and stores them in the passed ClsConfigMap reference.
	 */
	void getConfigsFromXML(cXMLElement* xmlData, ConfigMap& outputMap);

    /**
     * @brief Utility function. Reads the parameters of a XML element
     * and stores them in the passed ParameterMap reference.
     */
    void getParametersFromXML(cXMLElement* xmlData, ParameterMap& outputMap);

	/**
	 * Initializes the localTimeModel with the data from the
	 * passed XML-config element.
	 *
	 * @param xmlConfig The XML element specified in the ini file (ie the
	 *                  XML local time models configuration file)
	 */
	void initializeLocalTimeModel(cXMLElement* xmlConfig);
};

#endif /* __VEINS_OFFLOADAPP_H_ */
