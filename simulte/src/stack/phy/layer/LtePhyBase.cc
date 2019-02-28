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

#include "LtePhyBase.h"
#include "LteCommon.h"

short LtePhyBase::airFramePriority_ = 10;

LtePhyBase::LtePhyBase()
{
    channelModel_ = NULL;
}

LtePhyBase::~LtePhyBase()
{
    delete channelModel_;
}

void LtePhyBase::initialize(int stage)
{
    ChannelAccess::initialize(stage);

    EV << "LtePhyBase initialize, stage: " << stage << endl;

    if (stage == 0)
    {
        binder_ = getBinder();
        // get gate ids
        upperGateIn_ = findGate("upperGateIn");
        upperGateOut_ = findGate("upperGateOut");
        radioInGate_ = findGate("radioIn");

        // get local id
        nodeId_ = getAncestorPar("macNodeId");
        EV << "Local MacNodeId: " << nodeId_ << endl;
        deployer_ = getDeployer(nodeId_);
        // Initialize and watch statistics
        numAirFrameReceived_ = numAirFrameNotReceived_ = 0;
        ueTxPower_ = par("ueTxPower");
        eNodeBtxPower_ = par("eNodeBTxPower");
        microTxPower_ = par("microTxPower");
        relayTxPower_ = par("relayTxPower");
        deployer_->channelUpdate(nodeId_,
            intuniform(1, binder_->phyPisaData.maxChannel2()));
        averageCqiDl_ = registerSignal("averageCqiDl");
        averageCqiUl_ = registerSignal("averageCqiUl");

        averageCqiDlvect_ = registerSignal("averageCqiDlvect");
        averageCqiUlvect_ = registerSignal("averageCqiUlvect");

        if (!hasListeners(averageCqiDl_))
            error("no phy listeners");

        tSample_ = new TaggedSample();
        tSample_->module_ = check_and_cast<cComponent*>(this);
        carrierFrequency_ = 2.1e+9;
        WATCH(numAirFrameReceived_);
        WATCH(numAirFrameNotReceived_);
    }
    else if (stage == 2)
    {
        initializeChannelModel(par("channelModel").xmlValue());
    }
}

void LtePhyBase::handleMessage(cMessage* msg)
{
    EV << " LtePhyBase::handleMessage - new message received" << endl;

    if (msg->isSelfMessage())
    {
        handleSelfMessage(msg);
    }
    // AirFrame
    else if (msg->getArrivalGate()->getId() == radioInGate_)
    {
        handleAirFrame(msg);
    }

    // message from stack
    else if (msg->getArrivalGate()->getId() == upperGateIn_)
    {
        handleUpperMessage(msg);
    }
    // unknown message
    else
    {
        EV << "Unknown message received." << endl;
        delete msg;
    }
}

void LtePhyBase::handleControlMsg(LteAirFrame *frame,
    UserControlInfo *userInfo)
{
    cPacket *pkt = frame->decapsulate();
    delete frame;
    pkt->setControlInfo(userInfo);
    send(pkt, upperGateOut_);
    return;
}

LteAirFrame *LtePhyBase::createHandoverMessage()
{
    // broadcast airframe
    LteAirFrame *bdcAirFrame = new LteAirFrame("handoverFrame");
    UserControlInfo *cInfo = new UserControlInfo();
    cInfo->setIsBroadcast(true);
    cInfo->setIsCorruptible(false);
    cInfo->setSourceId(nodeId_);
    cInfo->setFrameType(HANDOVERPKT);
    cInfo->setTxPower(txPower_);
    bdcAirFrame->setControlInfo(cInfo);

    bdcAirFrame->setDuration(0);
    bdcAirFrame->setSchedulingPriority(airFramePriority_);
    // current position
    cInfo->setCoord(getRadioPosition());
    return bdcAirFrame;
}

void LtePhyBase::handleUpperMessage(cMessage* msg)
{
    EV << "LtePhy: message from stack" << endl;

    UserControlInfo* lteInfo = check_and_cast<UserControlInfo*>(
        msg->removeControlInfo());

    LteAirFrame* frame = NULL;

    if (lteInfo->getFrameType() == HARQPKT
        || lteInfo->getFrameType() == GRANTPKT
        || lteInfo->getFrameType() == RACPKT)
    {
        frame = new LteAirFrame("harqFeedback-grant");
    }
    else
    {
        // create LteAirFrame and encapsulate the received packet
        frame = new LteAirFrame("airframe");
    }

    frame->encapsulate(check_and_cast<cPacket*>(msg));

    // initialize frame fields

    frame->setSchedulingPriority(airFramePriority_);
    frame->setDuration(TTI);
    // set current position
    lteInfo->setCoord(getRadioPosition());

    lteInfo->setTxPower(txPower_);
    frame->setControlInfo(lteInfo);

    EV << "LtePhy: " << nodeTypeToA(nodeType_) << " with id " << nodeId_
       << " sending message to the air channel. Dest=" << lteInfo->getDestId() << endl;
    sendUnicast(frame);
}

void LtePhyBase::initializeChannelModel(cXMLElement* xmlConfig)
{
    if (xmlConfig == 0)
        throw cRuntimeError("No channel models configuration file specified");

    // Get channel Model field which contains parameters fields
    cXMLElementList channelModelList = xmlConfig->getElementsByTagName("ChannelModel");

    if (channelModelList.empty())
        throw cRuntimeError("No channel models configuration found in configuration file");

    if (channelModelList.size() > 1)
        throw cRuntimeError("More than one channel configuration found in configuration file.");

    cXMLElement* channelModelData = channelModelList.front();

    const char* name = channelModelData->getAttribute("type");

    if (name == 0)
        throw cRuntimeError("Could not read name of channel model");

    ParameterMap params;
    getParametersFromXML(channelModelData, params);

    LteChannelModel* newChannelModel = getChannelModelFromName(name, params);

    if (newChannelModel == 0)
        throw cRuntimeError("Could not find a channel model with the name \"%s\" ", name);

    // attach the new AnalogueModel to the AnalogueModelList
    channelModel_ = newChannelModel;

    EV << "ChannelModel \"" << name << "\" loaded." << endl;
    return;
}

LteChannelModel* LtePhyBase::getChannelModelFromName(std::string name, ParameterMap& params)
{
    if (name == "DUMMY")
        return initializeDummyChannelModel(params);
    else if (name == "REAL")
        return initializeChannelModel(params);
    else
        return 0;
}

LteChannelModel* LtePhyBase::initializeChannelModel(ParameterMap& params)
{
    return new LteRealisticChannelModel(params, getRadioPosition(), deployer_->getNumBands());
}

LteChannelModel* LtePhyBase::initializeDummyChannelModel(ParameterMap& params)
{
    return new LteDummyChannelModel(params, deployer_->getNumBands());
}

void LtePhyBase::updateDisplayString()
{
    char buf[80] = "";
    if (numAirFrameReceived_ > 0)
        sprintf(buf + strlen(buf), "af_ok:%d ", numAirFrameReceived_);
    if (numAirFrameNotReceived_ > 0)
        sprintf(buf + strlen(buf), "af_no:%d ", numAirFrameNotReceived_);
    getDisplayString().setTagArg("t", 0, buf);
}

void LtePhyBase::sendBroadcast(LteAirFrame *airFrame)
{
    //TODO

//    const ChannelControl::RadioRefVector& gateList = cc->getNeighbors(myRadioRef);
//    ChannelControl::radioRefVector::const_iterator i = gateList.begin();
//    UserControlInfo *ci = check_and_cast<UserControlInfo *>(
//            airFrame->getControlInfo());
//
//    if (i != gateList.end()) {
//        EV << "Sending broadcast message to " << gateList.size() << " NICs"
//                << endl;
// all gates
//        for(; i != gateList.end(); ++i){
//            // gate ID of the NIC
//            int radioStart = (*i)->host->gate("radioIn",0);  //       ->second->getId();
//            int radioEnd = radioStart + (*i)->host->gateSize("radioIn");
//            // all gates' indexes (if it is a gate array)
//            for (int g = radioStart; g != radioEnd; ++g) {
//                LteAirFrame *af = airFrame->dup();
//                af->setControlInfo(ci->dup());
//                sendDirect(af, i->second->getOwnerModule(), g);
//            }
//        }

//        // last receiving nic
//        int radioStart = (*i)->host->gate("radioIn",0);  //       ->second->getId();
//        int radioEnd = radioStart + (*i)->host->gateSize("radioIn");
//        // send until last - 1 gate, or doesn't enter if it is not a gate array
//        for (int g = radioStart; g != --radioEnd; ++g){
//            LteAirFrame *af = airFrame->dup();
//            af->setControlInfo(ci->dup());
//            sendDirect(af, i->second->getOwnerModule(), g);
//        }
//
//        // send the original message to the last gate of the last NIC
//        sendDirect(airFrame, i->second->getOwnerModule(), radioEnd);
//    } else {
//        EV << "NIC is not connected to any gates!" << endl;
//        delete airFrame;
//    }
    error("sendBroadcast is not implemented!");
}

LteAmc *LtePhyBase::getAmcModule(MacNodeId id)
{
    LteAmc *amc = NULL;
    OmnetId omid = binder_->getOmnetId(id);
    amc = check_and_cast<LteMacEnb *>(
        simulation.getModule(omid)->getSubmodule("nic")->getSubmodule(
            "mac"))->getAmc();
    return amc;
}

void LtePhyBase::sendUnicast(LteAirFrame *frame)
{
    UserControlInfo *ci = check_and_cast<UserControlInfo *>(
        frame->getControlInfo());
    // dest MacNodeId from control info
    MacNodeId dest = ci->getDestId();
    // destination node (UE, RELAY or ENODEB) omnet id
    try {
        binder_->getOmnetId(dest);
    } catch (std::out_of_range& e) {
        delete frame;
        return;         // FIXME HACK make sure that nodes that left the simulation do not send
    }
    OmnetId destOmnetId = binder_->getOmnetId(dest);
    if (destOmnetId == 0){
        delete frame;
        return;		// HACK
    }
    // get a pointer to receiving module
    cModule *receiver = simulation.getModule(destOmnetId);
    // receiver's gate
    sendDirect(frame, 0, frame->getDuration(), receiver, "radioIn");

    return;
}
