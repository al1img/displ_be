/*
 * XdgSurface.cpp
 *
 *  Created on: Jul 17, 2017
 *      Author: al1
 */

/*
 * ShellSurface.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: al1
 */

#include "XdgSurface.hpp"

#include "Exception.hpp"

namespace Wayland {

/*******************************************************************************
 * XdgSurface
 ******************************************************************************/

XdgSurface::XdgSurface(xdg_shell* shell, SurfacePtr surface) :
	mWlXdgSurface(nullptr),
	mSurface(surface),
	mLog("XdgSurface")
{
	try
	{
		init(shell);
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

void XdgSurface::setTopLevel()
{
	LOG(mLog, DEBUG) << "Set top level";

#if 0
	wl_shell_surface_set_toplevel(mWlShellSurface);
#endif
}

void XdgSurface::setFullScreen()
{
	LOG(mLog, DEBUG) << "Set full screen";
#if 0
	wl_shell_surface_set_fullscreen(mWlShellSurface,
			WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, nullptr);
#endif
}

/*******************************************************************************
 * Private
 ******************************************************************************/
#if 0
void ShellSurface::sPingHandler(void *data, wl_shell_surface *shell_surface,
								uint32_t serial)
{
	static_cast<ShellSurface*>(data)->pingHandler(serial);
}

void ShellSurface::sConfigHandler(void *data, wl_shell_surface *shell_surface,
								  uint32_t edges, int32_t width, int32_t height)
{
	static_cast<ShellSurface*>(data)->configHandler(edges, width, height);
}

void ShellSurface::sPopupDone(void *data, wl_shell_surface *shell_surface)
{
	static_cast<ShellSurface*>(data)->popupDone();
}

void ShellSurface::pingHandler(uint32_t serial)
{
	DLOG(mLog, DEBUG) << "Ping handler: " << serial;

	wl_shell_surface_pong(mWlShellSurface, serial);
}

void ShellSurface::configHandler(uint32_t edges, int32_t width, int32_t height)
{
	DLOG(mLog, DEBUG) << "Config handler, edges: " << edges
					  << ", width: " << width << ", height: " << height;
}

void ShellSurface::popupDone()
{
	DLOG(mLog, DEBUG) << "Popup done";
}
#endif

void XdgSurface::init(xdg_shell* shell)
{
	mWlXdgSurface = xdg_shell_get_xdg_surface(shell, mSurface->mWlSurface);

	if (!mWlXdgSurface)
	{
		throw Exception("Can't create xdg surface");
	}
#if 0
	mWlListener = {sPingHandler, sConfigHandler, sPopupDone};

	if (wl_shell_surface_add_listener(mWlShellSurface, &mWlListener, this) < 0)
	{
		throw Exception("Can't add listener");
	}
#endif
	LOG(mLog, DEBUG) << "Create";
}

void XdgSurface::release()
{
	if (mWlXdgSurface)
	{
		xdg_surface_destroy(mWlXdgSurface);

		LOG(mLog, DEBUG) << "Delete";
	}
}

}
