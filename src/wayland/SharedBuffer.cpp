/*
 * SharedBuffer.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: al1
 */

#include "SharedBuffer.hpp"

#include <drm_fourcc.h>

#include "Exception.hpp"

using std::shared_ptr;

namespace Wayland {

/*******************************************************************************
 * SharedBuffer
 ******************************************************************************/
SharedBuffer::SharedBuffer(wl_shm* sharedMemory,
						   shared_ptr<SharedFile> sharedFile,
						   uint32_t width, uint32_t height,
						   uint32_t pixelFormat) :
	mSharedFile(sharedFile),
	mWlBuffer(nullptr),
	mWlPool(nullptr),
	mWidth(width),
	mHeight(height),
	mLog("SharedBuffer")
{
	try
	{
		init(sharedMemory, pixelFormat);
	}
	catch(const WlException& e)
	{
		release();

		throw;
	}
}

SharedBuffer::~SharedBuffer()
{
	release();
}

/*******************************************************************************
 * Public
 ******************************************************************************/

/*******************************************************************************
 * Private
 ******************************************************************************/

uint32_t SharedBuffer::convertPixelFormat(uint32_t format)
{
	if (format == DRM_FORMAT_ARGB8888)
	{
		return WL_SHM_FORMAT_ARGB8888;
	}

	if (format == DRM_FORMAT_XRGB8888)
	{
		return WL_SHM_FORMAT_XRGB8888;
	}

	return format;
}

void SharedBuffer::init(wl_shm* sharedMemory, uint32_t pixelFormat)
{
	mWlPool = wl_shm_create_pool(sharedMemory, mSharedFile->mFd,
							   mHeight * mSharedFile->mStride);

	if (!mWlPool)
	{
		throw WlException("Can't create pool");
	}

	mWlBuffer = wl_shm_pool_create_buffer(mWlPool, 0, mWidth, mHeight,
										mSharedFile->mStride,
										convertPixelFormat(pixelFormat));

	if (!mWlBuffer)
	{
		throw WlException("Can't create shared buffer");
	}

	wl_shm_pool_destroy(mWlPool);

	mWlPool = nullptr;

	LOG(mLog, DEBUG) << "Create, w: " << mWidth << ", h: " << mHeight
					 << ", stride: " << mSharedFile->mStride
					 << ", fd: " << mSharedFile->mFd
					 << ", format: " << pixelFormat;
}

void SharedBuffer::release()
{
	if (mWlPool)
	{
		wl_shm_pool_destroy(mWlPool);
	}

	if (mWlBuffer)
	{
		wl_buffer_destroy(mWlBuffer);

		LOG(mLog, DEBUG) << "Delete";
	}
}

}
