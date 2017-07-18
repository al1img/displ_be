/*
 * XdgShell.cpp
 *
 *  Created on: Jul 17, 2017
 *      Author: al1
 */

#include "XdgShell.hpp"

#include "Exception.hpp"

namespace Wayland {

/*******************************************************************************
 * Shell
 ******************************************************************************/

XdgShell::XdgShell(wl_registry* registry, uint32_t id, uint32_t version) :
	Registry(registry, id, version),
	mWlXdgShell(nullptr),
	mLog("XdgShell")
{
	try
	{
		init();
	}
	catch(const std::exception& e)
	{
		release();

		throw;
	}
}

XdgShell::~XdgShell()
{
	release();
}

/*******************************************************************************
 * Public
 ******************************************************************************/
XdgSurfacePtr XdgShell::createXdgSurface(SurfacePtr surface)
{
	LOG(mLog, DEBUG) << "Create XDG surface";

	return XdgSurfacePtr(new XdgSurface(mWlXdgShell, surface));
}

/*******************************************************************************
 * Private
 ******************************************************************************/
void XdgShell::sPingHandler(void *data, xdg_shell *shell, uint32_t serial)
{
	static_cast<XdgShell*>(data)->pingHandler(serial);
}

void XdgShell::pingHandler(uint32_t serial)
{
	DLOG(mLog, DEBUG) << "Ping handler: " << serial;

	xdg_shell_pong(mWlXdgShell, serial);
}

void XdgShell::init()
{
	mWlXdgShell = static_cast<xdg_shell*>(
			wl_registry_bind(getRegistry(), getId(),
							 &xdg_shell_interface, getVersion()));

	if (!mWlXdgShell)
	{
		throw Exception("Can't bind XDG shell");
	}

	xdg_shell_use_unstable_version(mWlXdgShell, 5);

	mWlListener = { sPingHandler };

	if (xdg_shell_add_listener(mWlXdgShell, &mWlListener, this) < 0)
	{
		throw Exception("Can't add listener");
	}

	LOG(mLog, DEBUG) << "Create";
}

void XdgShell::release()
{
	if (mWlXdgShell)
	{
		xdg_shell_destroy(mWlXdgShell);

		LOG(mLog, DEBUG) << "Delete";
	}
}

}
