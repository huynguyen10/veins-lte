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

#include "LteSchedulingGrant_m.h"
#include "LteCommon.h"

class UserTxParams;

class LteSchedulingGrant : public LteSchedulingGrant_Base
{
  protected:

    const UserTxParams* userTxParams;
    RbMap grantedBlocks;
    std::vector<unsigned int> grantedCwBytes;

  public:

    LteSchedulingGrant(const char *name = NULL, int kind = 0) :
        LteSchedulingGrant_Base(name, kind)
    {
        userTxParams = NULL;
        grantedCwBytes.resize(MAX_CODEWORDS);
    }

    LteSchedulingGrant(const LteSchedulingGrant& other) :
        LteSchedulingGrant_Base(other.getName())
    {
        operator=(other);
    }

    LteSchedulingGrant& operator=(const LteSchedulingGrant& other)
    {
        LteSchedulingGrant_Base::operator=(other);
	userTxParams = other.userTxParams;
	grantedBlocks = other.grantedBlocks;
	grantedCwBytes = other.grantedCwBytes;
        return *this;
    }

    virtual LteSchedulingGrant *dup() const
    {
        return new LteSchedulingGrant(*this);
    }

    void setUserTxParams(const UserTxParams* arg)
    {
        userTxParams = arg;
    }

    const UserTxParams* getUserTxParams() const
    {
        return userTxParams;
    }

    const unsigned int getBlocks(Remote antenna, Band b) const
        {
        return grantedBlocks.at(antenna).at(b);
    }

    void setBlocks(Remote antenna, Band b, const unsigned int blocks)
    {
        grantedBlocks[antenna][b] = blocks;
    }

    const RbMap& getGrantedBlocks() const
    {
        return grantedBlocks;
    }

    void setGrantedBlocks(const RbMap& rbMap)
    {
        grantedBlocks = rbMap;
    }

    virtual void setGrantedCwBytesArraySize(unsigned int size)
    {
        grantedCwBytes.resize(size);
    }
    virtual unsigned int getGrantedCwBytesArraySize() const
    {
        return grantedCwBytes.size();
    }
    virtual unsigned int getGrantedCwBytes(unsigned int k) const
    {
        return grantedCwBytes.at(k);
    }
    virtual void setGrantedCwBytes(unsigned int k, unsigned int grantedCwBytes_var)
    {
        grantedCwBytes[k] = grantedCwBytes_var;
    }
};
