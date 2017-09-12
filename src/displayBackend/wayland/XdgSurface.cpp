/*
 * XdgSurface.cpp
 *
 *  Created on: Sep 12, 2017
 *      Author: al1
 */

#include "XdgSurface.hpp"

#include "Exception.hpp"

namespace Wayland {

/*******************************************************************************
 * XdgSurface
 ******************************************************************************/

XdgSurface::XdgSurface(zxdg_shell_v6* xdgShell, SurfacePtr surface) :
	mWlXdgSurface(nullptr),
	mWlTopLevel(nullptr),
	mSurface(surface),
	mLog("XdgSurface")
{
	try
	{
		init(xdgShell);
	}
	catch(const std::exception& e)
	{
		release();

		throw;
	}
}

XdgSurface::~XdgSurface()
{
	release();
}

/*******************************************************************************
 * Public
 ******************************************************************************/

/*******************************************************************************
 * Private
 ******************************************************************************/

void XdgSurface::sConfigSurfaceHandler(void *data, zxdg_surface_v6 *xdgSurface,
									   uint32_t serial)
{
	static_cast<XdgSurface*>(data)->configSurfaceHandler(serial);
}

void XdgSurface::sConfigTopLevelHandler(void *data, zxdg_toplevel_v6 *topLevel,
										int32_t width, int32_t height,
										wl_array *states)
{
	static_cast<XdgSurface*>(data)->configTopLevelHandler(width, height,
														  states);
}

void XdgSurface::configSurfaceHandler(uint32_t serial)
{
	DLOG(mLog, DEBUG) << "Config surface handler: " << serial;

	zxdg_surface_v6_ack_configure(mWlXdgSurface, serial);
}

void XdgSurface::configTopLevelHandler(int32_t width, int32_t height,
									   wl_array *states)
{
	DLOG(mLog, DEBUG) << "Config top level handler"
					  << ", w:" << width
					  << ", h:" << height;
}

void XdgSurface::closeTopLevelHandler()
{
	DLOG(mLog, DEBUG) << "Close top level handler";
}

void XdgSurface::init(zxdg_shell_v6* xdgShell)
{
	mWlXdgSurface = zxdg_shell_v6_get_xdg_surface(xdgShell,
												  mSurface->mWlSurface);

	if (!mWlXdgSurface)
	{
		throw Exception("Can't create xdg shell surface");
	}

	mWlSurfaceListener = {sConfigSurfaceHandler};

	if (zxdg_surface_v6_add_listener(mWlXdgSurface,
									 &mWlSurfaceListener, this) < 0)
	{
		throw Exception("Can't add xdg surface listener");
	}

	mWlTopLevel = zxdg_surface_v6_get_toplevel(mWlXdgSurface);

	if (!mWlTopLevel)
	{
		throw Exception("Can't create top level");
	}

	mWlTopLevelListener = {sConfigTopLevelHandler};

	if (zxdg_toplevel_v6_add_listener(mWlTopLevel,
									  &mWlTopLevelListener, this) < 0)
	{
		throw Exception("Can't add xdg toplevel listener");
	}

	mSurface->commit();

	LOG(mLog, DEBUG) << "Create";
}

void XdgSurface::release()
{
	if (mWlTopLevel)
	{
		zxdg_toplevel_v6_destroy(mWlTopLevel);
	}

	if (mWlXdgSurface)
	{
		zxdg_surface_v6_destroy(mWlXdgSurface);

		LOG(mLog, DEBUG) << "Delete";
	}
}

}
