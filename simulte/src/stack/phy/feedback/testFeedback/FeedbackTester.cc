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

#include "FeedbackTester.h"

Define_Module(FeedbackTester);

void FeedbackTester::initialize()
{
    interval_ = par("interval");
    generator_ = check_and_cast<LteDlFeedbackGenerator*>(simulation.getModuleByPath("lteDlFbGenerator"));
    aperiodic_ = new cMessage("aperiodic");
    scheduleAt(simTime(), aperiodic_);
}

void FeedbackTester::handleMessage(cMessage *msg)
{
    if (msg == aperiodic_)
    {
        scheduleAt(simTime() + interval_, aperiodic_);
        generator_->aperiodicRequest();
    }
}
