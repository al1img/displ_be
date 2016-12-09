/*
 *  Device class
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

#include "Device.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#include <xen/be/Log.hpp>

using std::exception;
using std::dynamic_pointer_cast;
using std::lock_guard;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::thread;
using std::to_string;

namespace Drm {

const uint32_t cInvalidId = 0;

/*******************************************************************************
 * Device
 ******************************************************************************/

Device::Device(const string& name) try :
	mName(name),
	mFd(-1),
	mTerminate(true),
	mNumFlipPages(0),
	mLog("Drm")
{
	LOG(mLog, DEBUG) << "Create Drm card: " << mName;

	init();
}
catch(const exception& e)
{
	LOG(mLog, ERROR) << e.what();

	release();

	throw;
}

Device::~Device()
{
	stop();

	release();

	LOG(mLog, DEBUG) << "Delete Drm card: " << mName;
}

/*******************************************************************************
 * Public
 ******************************************************************************/
shared_ptr<DisplayBufferItf> Device::createDisplayBuffer(uint32_t width,
														 uint32_t height,
														 uint32_t bpp)
{
	lock_guard<mutex> lock(mMutex);

	LOG(mLog, DEBUG) << "Create display buffer";

	return shared_ptr<DisplayBufferItf>(new Dumb(mFd, width, height, bpp));
}

shared_ptr<FrameBufferItf> Device::createFrameBuffer(
		shared_ptr<DisplayBufferItf> displayBuffer,uint32_t width,
		uint32_t height, uint32_t pixelFormat)
{
	lock_guard<mutex> lock(mMutex);

	LOG(mLog, DEBUG) << "Create frame buffer";

	return shared_ptr<FrameBufferItf>(
			new FrameBuffer(mFd, displayBuffer, width, height, pixelFormat));
}

void Device::start()
{
	lock_guard<mutex> lock(mMutex);

	DLOG(mLog, DEBUG) << "Start";

	if (!mTerminate)
	{
		return;
	}

	mTerminate = false;

	mThread = thread(&Device::eventThread, this);
}

void Device::stop()
{
	lock_guard<mutex> lock(mMutex);

	DLOG(mLog, DEBUG) << "Stop";

	mTerminate = true;

	if (mThread.joinable())
	{
		mThread.join();
	}
}

shared_ptr<ConnectorItf> Device::getConnectorById(uint32_t id)
{
	auto iter = mConnectors.find(id);

	if (iter == mConnectors.end())
	{
		throw DrmException("Wrong connector id " + to_string(id));
	}

	return dynamic_pointer_cast<ConnectorItf>(iter->second);
}

shared_ptr<Connector> Device::getConnectorByIndex(uint32_t index)
{
	if (index >= mConnectors.size())
	{
		throw DrmException("Wrong connector index " + to_string(index));
	}

	auto iter = mConnectors.begin();

	advance(iter, index);

	return iter->second;
}

size_t Device::getConnectorsCount()
{
	return mConnectors.size();
}

/*******************************************************************************
 * Private
 ******************************************************************************/

void Device::init()
{
	mFd = open(mName.c_str(), O_RDWR | O_CLOEXEC);

	if (mFd < 0)
	{
		throw DrmException("Cannot open " + mName);
	}

	uint64_t hasDumb = false;

	if (drmGetCap(mFd, DRM_CAP_DUMB_BUFFER, &hasDumb) < 0 || !hasDumb)
	{
		throw DrmException("Drm device does not support dumb buffers");
	}

	mRes.reset(new ModeResource(mFd));

	for (int i = 0; i < (*mRes)->count_connectors; i++)
	{
		Connector* connector = new Connector(*this, (*mRes)->connectors[i]);

		mConnectors.emplace((*mRes)->connectors[i],
							shared_ptr<Connector>(connector));
	}
}

void Device::release()
{
	mRes.reset();

	mConnectors.clear();

	if (mFd >= 0)
	{
		close(mFd);
	}
}

void Device::eventThread()
{
	try
	{
		pollfd fds;

		fds.fd = mFd;
		fds.events = POLLIN;

		drmEventContext ev { 0 };

		ev.version = DRM_EVENT_CONTEXT_VERSION;
		ev.page_flip_handler = handleFlipEvent;

		while(!mTerminate)
		{
			auto ret = poll(&fds, 1, cPoolEventTimeoutMs);

			if (ret < 0)
			{
				LOG(mLog, ERROR) << "Can't poll events";
			}

			if (ret > 0)
			{
				drmHandleEvent(mFd, &ev);
			}
		}

		while(mNumFlipPages)
		{
			drmHandleEvent(mFd, &ev);
		}
	}
	catch(const exception& e)
	{
		LOG(mLog, ERROR) << e.what();
	}
}

void Device::handleFlipEvent(int fd, unsigned int sequence,
								unsigned int tv_sec, unsigned int tv_usec,
								void *user_data)
{
	if (user_data)
	{
		static_cast<Connector*>(user_data)->flipFinished();
	}
}

}
