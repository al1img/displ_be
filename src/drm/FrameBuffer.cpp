/*
 *  FrameBuffer class
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

#include "FrameBuffer.hpp"

#include <xen/be/Log.hpp>

#include "Device.hpp"
#include "Dumb.hpp"

using std::chrono::milliseconds;
using std::string;
using std::this_thread::sleep_for;

namespace Drm {

extern const uint32_t cInvalidId;

/***************************************************************************//**
 * FrameBuffer
 ******************************************************************************/

FrameBuffer::FrameBuffer(Device& drm, Dumb& dumb, uint32_t width,
						 uint32_t height, uint32_t pixelFormat,
						 uint32_t pitch) :
	mDrm(drm),
	mDumb(dumb),
	mId(cInvalidId),
	mFlipPending(false)
{
	uint32_t handles[4], pitches[4], offsets[4] = {0};

	handles[0] = mDumb.getHandle();
	pitches[0] = pitch;

	auto ret = drmModeAddFB2(mDrm.getFd(), width, height, pixelFormat, handles,
							 pitches, offsets, &mId, 0);

	DLOG("FrameBuffer", DEBUG) << "Create frame buffer, handle: " << handles[0]
							  << ", id: " << mId;

	if (ret)
	{
		throw DrmException ("Cannot create frame buffer: " +
							string(strerror(errno)));
	}

}

FrameBuffer::~FrameBuffer()
{
	if (mFlipPending)
	{
		sleep_for(milliseconds(100));
	}

	if (mFlipPending)
	{
		LOG("FrameBuffer", ERROR) << "Delete frame buffer on pending flip";
	}

	if (mId != cInvalidId)
	{
		DLOG("FrameBuffer", DEBUG) << "Delete frame buffer, handle: "
								  << getHandle() << ", id: " << mId;

		drmModeRmFB(mDrm.getFd(), mId);
	}
}

/***************************************************************************//**
 * Public
 ******************************************************************************/

uint32_t FrameBuffer::getHandle() const
{
	return mDumb.getHandle();
}

bool FrameBuffer::pageFlip(uint32_t crtc, FlipCallback cbk)
{
	if (mDrm.isStopped())
	{
		DLOG("FrameBuffer", WARNING) << "Page flip when DRM is stopped";

		return false;
	}

	auto ret = drmModePageFlip(mDrm.getFd(), crtc, mId,
							   DRM_MODE_PAGE_FLIP_EVENT, this);

	if (ret)
	{
		throw DrmException("Cannot flip CRTC: " + mId);
	}

	mDrm.pageFlipScheduled();
	mFlipPending = true;
	mFlipCallback = cbk;

	return true;
}

/***************************************************************************//**
 * Private
 ******************************************************************************/

void FrameBuffer::flipFinished()
{
	mDrm.pageFlipDone();

	if (!mFlipPending)
	{
		LOG("FrameBuffer", ERROR) << "Not expected flip event";

		return;
	}

	mFlipPending = false;

	if (mFlipCallback)
	{
		mFlipCallback();
	}
}

}

