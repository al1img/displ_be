/*
 * IviSurface.cpp
 *
 *  Created on: Dec 8, 2016
 *      Author: al1
 */

#include "IviSurface.hpp"

#include <ilm/ilm_client.h>

#include "Exception.hpp"

namespace Wayland {

/*******************************************************************************
 * IviSurface
 ******************************************************************************/

IviSurface::IviSurface(ivi_application* iviApplication, SurfacePtr surface,
					   uint32_t surfaceId) :
	mWlIviSurface(nullptr),
	mIlmSurfaceId(surfaceId),
	mSurface(surface),
	mLog("IviSurface")
{
	try
	{
		init(iviApplication);
	}
	catch(const std::exception& e)
	{
		release();

		throw;
	}
}

IviSurface::~IviSurface()
{
	release();
}

/*******************************************************************************
 * Public
 ******************************************************************************/

/*******************************************************************************
 * Private
 ******************************************************************************/
void IviSurface::sOnConfigure(void *data, ivi_surface *ivi_surface,
							  int32_t width, int32_t height)
{
	static_cast<IviSurface*>(data)->onConfigure(width, height);
}

void IviSurface::onConfigure(int32_t width, int32_t height)
{
	LOG(mLog, DEBUG) << "On configure, width: " << width
					 << ", height: " << height;
}

void IviSurface::init(ivi_application* iviApplication)
{
	mWlIviSurface = ivi_application_surface_create(iviApplication,
												   mIlmSurfaceId,
												   mSurface->mWlSurface);

	if (!mWlIviSurface)
	{
		throw Exception("Can't create IVI surface", errno);
	}

	mWlListener = {sOnConfigure};

	if (ivi_surface_add_listener(mWlIviSurface, &mWlListener, this) < 0)
	{
		throw Exception("Can't add listener", errno);
	}

	LOG(mLog, DEBUG) << "Create, surface id: " << mIlmSurfaceId;
}

void IviSurface::release()
{
	if (mWlIviSurface)
	{
		ivi_surface_destroy(mWlIviSurface);

		LOG(mLog, DEBUG) << "Delete";
	}
}

}
