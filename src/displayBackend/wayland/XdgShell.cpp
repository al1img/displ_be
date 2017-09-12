/*
 * XdgShell.cpp
 *
 *  Created on: Sep 12, 2017
 *      Author: al1
 */

#include "XdgShell.hpp"

#include "Exception.hpp"

namespace Wayland {

/*******************************************************************************
 * XdgShell
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
	LOG(mLog, DEBUG) << "Create xdg surface";

	return XdgSurfacePtr(new XdgSurface(mWlXdgShell, surface));
}

/*******************************************************************************
 * Private
 ******************************************************************************/

void XdgShell::sPingHandler(void *data, zxdg_shell_v6 *xdgShell,
							uint32_t serial)
{
	static_cast<XdgShell*>(data)->pingHandler(serial);
}

void XdgShell::pingHandler(uint32_t serial)
{
	LOG(mLog, DEBUG) << "Ping handler: " << serial;

	zxdg_shell_v6_pong(mWlXdgShell, serial);
}

void XdgShell::init()
{
	mWlXdgShell = static_cast<zxdg_shell_v6*>(
			wl_registry_bind(getRegistry(), getId(),
							 &zxdg_shell_v6_interface, getVersion()));

	if (!mWlXdgShell)
	{
		throw Exception("Can't bind xdg shell");
	}

	mWlListener = {sPingHandler};

	if (zxdg_shell_v6_add_listener(mWlXdgShell, &mWlListener, this) < 0)
	{
		throw Exception("Can't add listener");
	}

	LOG(mLog, DEBUG) << "Create";
}

void XdgShell::release()
{
	if (mWlXdgShell)
	{
		zxdg_shell_v6_destroy(mWlXdgShell);

		LOG(mLog, DEBUG) << "Delete";
	}
}

}
