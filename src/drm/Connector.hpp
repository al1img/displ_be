/*
 *  Connector class
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Copyright (C) 2016 EPAM Systems Inc.
 *
 */

#ifndef SRC_DRM_CONNECTOR_HPP_
#define SRC_DRM_CONNECTOR_HPP_

#include <xen/be/Log.hpp>

#include "Exception.hpp"
#include "Modes.hpp"

namespace Drm {

extern const uint32_t cInvalidId;

class Device;

class Connector
{
public:

	Connector(Device& dev, int connectorId);
	~Connector();

	uint32_t getCrtcId() const { return mCrtcId; }
	uint32_t getId() const { return mConnector->connector_id; }
	bool isConnected() const { return mConnector->connection ==
									  DRM_MODE_CONNECTED;
	}

	bool isInitialized() const { return mCrtcId != cInvalidId; }

	void init(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
			  uint32_t bpp, uint32_t fbId);
	void release();

private:
	Device& mDev;
	int mFd;
	uint32_t mCrtcId;
	ModeConnector mConnector;
	drmModeCrtc* mSavedCrtc;
	XenBackend::Log mLog;

	uint32_t findCrtcId();
	uint32_t getAssignedCrtcId();
	uint32_t findMatchingCrtcId();
	bool isCrtcIdUsedByOther(uint32_t crtcId);
	drmModeModeInfoPtr findMode(uint32_t width, uint32_t height);
};

}

#endif /* SRC_DRM_CONNECTOR_HPP_ */
