/*
 * XdgShell.hpp
 *
 *  Created on: Jul 17, 2017
 *      Author: al1
 */

#ifndef SRC_WAYLAND_XDGSHELL_HPP_
#define SRC_WAYLAND_XDGSHELL_HPP_

#include <xen/be/Log.hpp>

#include "Registry.hpp"
#include "XdgSurface.hpp"

namespace Wayland {

/***************************************************************************//**
 * Wayland XDG shell class.
 * @ingroup wayland
 ******************************************************************************/
class XdgShell : public Registry
{
public:

	~XdgShell();

	/**
	 * Creates XDG surface
	 * @param surface surface
	 */
	XdgSurfacePtr createXdgSurface(SurfacePtr surface);

private:

	friend class Display;

	XdgShell(wl_registry* registry, uint32_t id, uint32_t version);

	xdg_shell* mWlXdgShell;
	xdg_shell_listener mWlListener;
	XenBackend::Log mLog;

	static void sPingHandler(void *data, xdg_shell *shell, uint32_t serial);

	void pingHandler(uint32_t serial);

	void init();
	void release();
};

typedef std::shared_ptr<XdgShell> XdgShellPtr;

}

#endif /* SRC_WAYLAND_XDGSHELL_HPP_ */
