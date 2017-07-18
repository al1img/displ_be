/*
 * XdgSurface.hpp
 *
 *  Created on: Jul 17, 2017
 *      Author: al1
 */

#ifndef SRC_WAYLAND_XDGSURFACE_HPP_
#define SRC_WAYLAND_XDGSURFACE_HPP_

#include <memory>

#include <xen/be/Log.hpp>

#include <wayland-client-core.h>

#include "wayland-xdg/xdg-shell-client-protocol.h"

#include "Surface.hpp"

namespace Wayland {

/***************************************************************************//**
 * Wayland XDG surface class.
 * @ingroup wayland
 ******************************************************************************/
class XdgSurface
{
public:

	~XdgSurface();

	/**
	 * Sets shell surface as top level
	 */
	void setTopLevel();

	/**
	 * Sets shell surface as fullscreen
	 */
	void setFullScreen();

	/**
	 * Returns associated surface
	 */
	SurfacePtr getSurface() const { return mSurface; }

private:

	friend class Display;
	friend class XdgShell;

	XdgSurface(xdg_shell* shell, SurfacePtr surface);

	xdg_surface* mWlXdgSurface;
	SurfacePtr mSurface;
	xdg_surface_listener mWlListener;

	XenBackend::Log mLog;

	static void sPingHandler(void *data, wl_shell_surface *shell_surface,
							 uint32_t serial);
	static void sConfigHandler(void *data, wl_shell_surface *shell_surface,
							   uint32_t edges, int32_t width, int32_t height);
	static void sPopupDone(void *data, wl_shell_surface *shell_surface);

	void pingHandler(uint32_t serial);
	void configHandler(uint32_t edges, int32_t width, int32_t height);
	void popupDone();

	void init(xdg_shell* shell);
	void release();
};

typedef std::shared_ptr<XdgSurface> XdgSurfacePtr;

}

#endif /* SRC_WAYLAND_XDGSURFACE_HPP_ */
