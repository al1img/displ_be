/*
 * ShellSurface.cpp
 *
 *  Created on: Nov 24, 2016
 *      Author: al1
 */

#include "ShellSurface.hpp"

#include "Exception.hpp"

using std::shared_ptr;

namespace Wayland {

/*******************************************************************************
 * ShellSurface
 ******************************************************************************/

ShellSurface::ShellSurface(wl_shell* shell, shared_ptr<Surface> surface) :
	mShellSurface(nullptr),
	mSurfacePtr(surface),
	mLog("ShellSurface")
{
	try
	{
		init(shell);
	}
	catch(const WlException& e)
	{
		release();

		throw;
	}
}

ShellSurface::~ShellSurface()
{
	release();
}

/*******************************************************************************
 * Public
 ******************************************************************************/

void ShellSurface::setTopLevel()
{
	LOG(mLog, DEBUG) << "Set top level";

	wl_shell_surface_set_toplevel(mShellSurface);
}

void ShellSurface::setFullScreen()
{
	LOG(mLog, DEBUG) << "Set full screen";

	wl_shell_surface_set_fullscreen(mShellSurface,
			WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, nullptr);
}

void ShellSurface::setConfigCallback(ConfigCallback cbk)
{
	LOG(mLog, DEBUG) << "Set config callback";

	mConfigCallback = cbk;
}

/*******************************************************************************
 * Private
 ******************************************************************************/
void ShellSurface::sPingHandler(void *data, wl_shell_surface *shell_surface,
								uint32_t serial)
{
	static_cast<ShellSurface*>(data)->pingHandler(serial);
}

void ShellSurface::sConfigHandler(void *data, wl_shell_surface *shell_surface,
								  uint32_t edges, int32_t width, int32_t height)
{
	auto shellSurface = static_cast<ShellSurface*>(data);

	if (shellSurface->mConfigCallback)
	{
		shellSurface->mConfigCallback(edges, width, height);
	}
	else
	{
		shellSurface->configHandler(edges, width, height);
	}
}

void ShellSurface::sPopupDone(void *data, wl_shell_surface *shell_surface)
{
	static_cast<ShellSurface*>(data)->popupDone();
}

void ShellSurface::pingHandler(uint32_t serial)
{
	DLOG(mLog, DEBUG) << "Ping handler: " << serial;

	wl_shell_surface_pong(mShellSurface, serial);
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

void ShellSurface::init(wl_shell* shell)
{
	mShellSurface = wl_shell_get_shell_surface(shell, mSurfacePtr->mSurface);

	if (!mShellSurface)
	{
		throw WlException("Can't create shell surface");
	}

	mListener = {sPingHandler, sConfigHandler, sPopupDone};

	if (wl_shell_surface_add_listener(mShellSurface, &mListener, this) < 0)
	{
		throw WlException("Can't add listener");
	}

	LOG(mLog, DEBUG) << "Create";
}

void ShellSurface::release()
{
	if (mShellSurface)
	{
		wl_shell_surface_destroy(mShellSurface);

		LOG(mLog, DEBUG) << "Delete";
	}
}

}
