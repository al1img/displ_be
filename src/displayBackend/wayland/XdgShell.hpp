/*
 * XdgShell.hpp
 *
 *  Created on: Sep 12, 2017
 *      Author: al1
 */

#ifndef SRC_WAYLAND_XDGSHELL_HPP_
#define SRC_WAYLAND_XDGSHELL_HPP_

#include <xen/be/Log.hpp>

#include "Registry.hpp"
#include "XdgSurface.hpp"

namespace Wayland {

/***************************************************************************//**
 * Wayland xdg shell class.
 * @ingroup wayland
 ******************************************************************************/
class XdgShell : public Registry
{
public:

	~XdgShell();

	/**
	 * Creates xdg shell surface
	 * @param surface surface
	 */
	XdgSurfacePtr createXdgSurface(SurfacePtr surface);

private:

	friend class Display;

	XdgShell(wl_registry* registry, uint32_t id, uint32_t version);

	zxdg_shell_v6* mWlXdgShell;
	zxdg_shell_v6_listener mWlListener;

	XenBackend::Log mLog;

	static void sPingHandler(void *data, zxdg_shell_v6 *xdgShell,
							 uint32_t serial);
	void pingHandler(uint32_t serial);

	void init();
	void release();
};

typedef std::shared_ptr<XdgShell> XdgShellPtr;
}

#endif /* SRC_WAYLAND_XDGSHELL_HPP_ */
