/*
 * DrmBuffer.cpp
 *
 *  Created on: Jan 18, 2017
 *      Author: al1
 */

#include "DrmBuffer.hpp"

#include "Exception.hpp"

using std::shared_ptr;

namespace Wayland {

/*******************************************************************************
 * DrmBuffer
 ******************************************************************************/

DrmBuffer::DrmBuffer(wl_drm* wlDrm,
					 DisplayBufferPtr displayBuffer,
					 uint32_t width, uint32_t height,
					 uint32_t pixelFormat) :
	mDisplayBuffer(displayBuffer),
	mWlBuffer(nullptr),
	mWidth(width),
	mHeight(height),
	mLog("SharedBuffer")
{
	try
	{
		init(wlDrm, pixelFormat);
	}
	catch(const WlException& e)
	{
		release();

		throw;
	}
}

DrmBuffer::~DrmBuffer()
{
	release();
}

/*******************************************************************************
 * Public
 ******************************************************************************/

/*******************************************************************************
 * Private
 ******************************************************************************/

void DrmBuffer::init(wl_drm* wlDrm, uint32_t pixelFormat)
{
	mWlBuffer = wl_drm_create_buffer(wlDrm, mDisplayBuffer->readName(),
									 mWidth, mHeight,
									 mDisplayBuffer->getStride(), pixelFormat);

	if (!mWlBuffer)
	{
		throw WlException("Can't create Drm buffer");
	}

	LOG(mLog, DEBUG) << "Create, w: " << mWidth << ", h: " << mHeight
					 << ", stride: " << mDisplayBuffer->getStride()
					 << ", fd: " << mDisplayBuffer->getHandle()
					 << ", format: " << pixelFormat;
}

void DrmBuffer::release()
{
	if (mWlBuffer)
	{
		wl_buffer_destroy(mWlBuffer);

		LOG(mLog, DEBUG) << "Delete";
	}
}

}

