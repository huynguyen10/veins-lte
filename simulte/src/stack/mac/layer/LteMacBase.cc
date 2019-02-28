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


#include "LteMacBase.h"
#include "LteHarqBufferTx.h"
#include "LteHarqBufferRx.h"
#include "LteMacPdu.h"
#include "LteMacQueue.h"
#include "LteControlInfo.h"
#include "LteBinder.h"
#include "LteDeployer.h"
#include "LteHarqFeedback_m.h"
#include "LteMacBuffer.h"
#include "assert.h"

LteMacBase::LteMacBase()
{
    mbuf_.clear();
    macBuffers_.clear();
}

LteMacBase::~LteMacBase()
{
    LteMacBuffers::iterator mit;
    LteMacBufferMap::iterator vit;
    for (mit = mbuf_.begin(); mit != mbuf_.end(); mit++)
        delete mit->second;
    for (vit = macBuffers_.begin(); vit != macBuffers_.end(); vit++)
        delete vit->second;
    mbuf_.clear();
    macBuffers_.clear();
    harqTxBuffers_.clear();
    harqRxBuffers_.clear();
}

void LteMacBase::sendUpperPackets(cPacket* pkt)
{
    EV << "LteMacBase : Sending packet " << pkt->getName() << " on port MAC_to_RLC\n";
    // Send message
    send(pkt,up_[OUT]);
    emit(sentPacketToUpperLayer, pkt);
}

void LteMacBase::sendLowerPackets(cPacket* pkt)
{
    EV << "LteMacBase : Sending packet " << pkt->getName() << " on port MAC_to_PHY\n";
    // Send message
    updateUserTxParam(pkt);
    send(pkt,down_[OUT]);
    emit(sentPacketToLowerLayer, pkt);
}

/*
 * Upper layer handler
 */
void LteMacBase::fromRlc(cPacket *pkt)
{
    bufferizePacket(pkt);
}

/*
 * Lower layer handler
 */
void LteMacBase::fromPhy(cPacket *pkt)
{
    // TODO: harq test (comment fromPhy: it has only to pass pdus to proper rx buffer and
    // to manage H-ARQ feedback)

    UserControlInfo *userInfo = check_and_cast<UserControlInfo *>(pkt->getControlInfo());
    MacNodeId src = userInfo->getSourceId();

    if (userInfo->getFrameType() == HARQPKT)
    {
        // H-ARQ feedback, send it to TX buffer of source
        HarqTxBuffers::iterator htit = harqTxBuffers_.find(src);
        EV << NOW << "Mac::fromPhy: node " << nodeId_ << " Received HARQ Feeback pkt" << endl;
        if (htit == harqTxBuffers_.end())
        {
            // if a feeback arrives, a tx buffer must exists (unless it is an handover scenario
            // where the harq buffer was deleted but a feedback was in transit)
            // this case must be taken care of
            throw cRuntimeError("Mac::fromPhy(): Received feedback for an unexisting H-ARQ tx buffer");
        }
        LteHarqFeedback *hfbpkt = check_and_cast<LteHarqFeedback *>(pkt);
        htit->second->receiveHarqFeedback(hfbpkt);
    }
    else if (userInfo->getFrameType() == FEEDBACKPKT)
    {
        //Feedback pkt
        EV << NOW << "Mac::fromPhy: node " << nodeId_ << " Received feedback pkt" << endl;
        macHandleFeedbackPkt(pkt);
    }
    else if (userInfo->getFrameType()==GRANTPKT)
    {
        //Scheduling Grant
        EV << NOW << "Mac::fromPhy: node " << nodeId_ << " Received Scheduling Grant pkt" << endl;
        macHandleGrant(pkt);
    }
    else if(userInfo->getFrameType() == DATAPKT)
    {
        // data packet: insert in proper rx buffer
        EV << NOW << "Mac::fromPhy: node " << nodeId_ << " Received DATA packet" << endl;
        LteMacPdu *pdu = check_and_cast<LteMacPdu *>(pkt);
        Codeword cw = userInfo->getCw();
        HarqRxBuffers::iterator hrit = harqRxBuffers_.find(src);
        if (hrit != harqRxBuffers_.end())
        {
            hrit->second->insertPdu(cw,pdu);
        }
        else
        {
            // FIXME: possible memory leak
            LteHarqBufferRx *hrb = new LteHarqBufferRx(ENB_RX_HARQ_PROCESSES, this,src);
            harqRxBuffers_[src] = hrb;
            hrb->insertPdu(cw,pdu);
        }
    }
    else if (userInfo->getFrameType() == RACPKT)
    {
        EV << NOW << "Mac::fromPhy: node " << nodeId_ << " Received RAC packet" << endl;
        macHandleRac(pkt);
    }
    else
    {
        throw cRuntimeError("Unknown packet type %d", (int)userInfo->getFrameType());
    }
}

bool LteMacBase::bufferizePacket(cPacket* pkt)
{
    pkt->setTimestamp();        // Add timestamp with current time to packet

    FlowControlInfo* lteInfo = check_and_cast<FlowControlInfo*>(pkt->getControlInfo());

    // obtain the cid from the packet informations
    MacCid cid = ctrlInfoToMacCid(lteInfo);

    // build the virtual packet corresponding to this incoming packet
    PacketInfo vpkt(pkt->getByteLength(), pkt->getTimestamp());

    LteMacBuffers::iterator it = mbuf_.find(cid);
    if (it == mbuf_.end())
    {
        // Queue not found for this cid: create
        LteMacQueue* queue = new LteMacQueue(queueSize_);
        LteMacBuffer* vqueue = new LteMacBuffer();

        queue->pushBack(pkt);
        vqueue->pushBack(vpkt);
        mbuf_[cid] = queue;
        macBuffers_[cid] = vqueue;

        // make a copy of lte control info and store it to traffic descriptors map
        FlowControlInfo toStore(*lteInfo);
        connDesc_[cid] = toStore;
        // register connection to lcg map.
        LteTrafficClass tClass = (LteTrafficClass) lteInfo->getTraffic();

        lcgMap_.insert(LcgPair(tClass, CidBufferPair(cid, macBuffers_[cid])));

        EV << "LteMacBuffers : Using new buffer on node: " <<
        MacCidToNodeId(cid) << " for Lcid: " << MacCidToLcid(cid) << ", Space left in the Queue: " <<
        queue->getQueueSize() - queue->getByteLength() << "\n";
    }
    else
    {
        // Found
        LteMacQueue* queue = it->second;
        LteMacBuffer* vqueue = macBuffers_.find(cid)->second;
        if (!queue->pushBack(pkt))
        {
            tSample_->id_=nodeId_;
            tSample_->sample_=pkt->getByteLength();
            if (lteInfo->getDirection()==DL)
            {
                emit(macBufferOverflowDl_,tSample_);
            }
            else
            {
                emit(macBufferOverflowUl_,tSample_);
            }

            EV << "LteMacBuffers : Dropped packet: queue" << cid << " is full\n";
            delete pkt;
            return false;
        }
        vqueue->pushBack(vpkt);

        EV << "LteMacBuffers : Using old buffer on node: " <<
        MacCidToNodeId(cid) << " for Lcid: " << MacCidToLcid(cid) << ", Space left in the Queue: " <<
        queue->getQueueSize() - queue->getByteLength() << "\n";
    }
        /// After bufferization buffers must be synchronized
    assert(mbuf_[cid]->getQueueLength() == macBuffers_[cid]->getQueueLength());
    return true;
}

void LteMacBase::deleteQueues(MacNodeId nodeId)
{
    LteMacBuffers::iterator mit;
    LteMacBufferMap::iterator vit;
    for (mit = mbuf_.begin(); mit != mbuf_.end(); )
    {
        if (MacCidToNodeId(mit->first) == nodeId)
        {
            delete mit->second;        // Delete Queue
            mbuf_.erase(mit++);        // Delete Elem
        } else {
            mit++;
        }
    }
    for (vit = macBuffers_.begin(); vit != macBuffers_.end(); )
    {
        if (MacCidToNodeId(vit->first) == nodeId)
        {
            delete vit->second;        // Delete Queue
            macBuffers_.erase(vit++);        // Delete Elem
        } else {
            vit++;
        }
    }

    // TODO remove traffic descriptor and lcg entry
}


/*
 * Main functions
 */
void LteMacBase::initialize(int stage)
{
    if (stage == 0)
    {
        /* Gates initialization */
        up_[IN] = gate("RLC_to_MAC");
        up_[OUT] = gate("MAC_to_RLC");
        down_[IN] = gate("PHY_to_MAC");
        down_[OUT] = gate("MAC_to_PHY");

        /* Create buffers */
        queueSize_ = par("queueSize");
        maxBytesPerTti_ = par("maxBytesPerTti");

        nodeId_ = getAncestorPar("macNodeId");
        cellId_ = getAncestorPar("macCellId");

        /* Get reference to binder */
        binder_ = getBinder();

        /* Set The MAC MIB */

        muMimo_ = par("muMimo");

        harqProcesses_ = par("harqProcesses");

        /* Start TTI tick */
        ttiTick_ = new cMessage("ttiTick_");
        ttiTick_->setSchedulingPriority(1);        // TTI TICK after other messages
        scheduleAt(NOW + TTI, ttiTick_);
        macBufferOverflowDl_ = registerSignal("macBufferOverflowDl");
        macBufferOverflowUl_ = registerSignal("macBufferOverflowUl");
        receivedPacketFromUpperLayer = registerSignal("receivedPacketFromUpperLayer");
        receivedPacketFromLowerLayer = registerSignal("receivedPacketFromLowerLayer");
        sentPacketToUpperLayer = registerSignal("sentPacketToUpperLayer");
        sentPacketToLowerLayer = registerSignal("sentPacketToLowerLayer");
        tSample_ = new TaggedSample();
        tSample_->module_ = this;

        measuredItbs_ = registerSignal("measuredItbs");
        measuredItbs_lte_ = registerSignal("measuredItbs_lte");
        WATCH(queueSize_);
        WATCH(maxBytesPerTti_);
        WATCH(nodeId_);
        WATCH_MAP(mbuf_);
        WATCH_MAP(macBuffers_);
    }
}

void LteMacBase::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage())
    {
        handleSelfMessage();
        scheduleAt(NOW + TTI, msg);
        return;
    }

    cPacket* pkt = check_and_cast<cPacket *>(msg);
    EV << "LteMacBase : Received packet " << pkt->getName() <<
    " from port " << pkt->getArrivalGate()->getName() << endl;

    cGate* incoming = pkt->getArrivalGate();

    if (incoming == down_[IN])
    {
        // message from PHY_to_MAC gate (from lower layer)
        emit(receivedPacketFromLowerLayer, pkt);
        fromPhy(pkt);
    }
    else
    {
        // message from RLC_to_MAC gate (from upper layer)
        emit(receivedPacketFromUpperLayer, pkt);
        fromRlc(pkt);
    }
    return;
}

void LteMacBase::finish()
{
    // TODO make-finish
}

