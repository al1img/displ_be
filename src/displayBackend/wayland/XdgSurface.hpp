/*
 * XdgSurface.hpp
 *
 *  Created on: Sep 12, 2017
 *      Author: al1
 */

#ifndef SRC_WAYLAND_XDGSURFACE_HPP_
#define SRC_WAYLAND_XDGSURFACE_HPP_

#include <memory>

#include <xen/be/Log.hpp>

#include "xdg-shell-unstable-v6.h"

#include "Surface.hpp"

namespace Wayland {

/***************************************************************************//**
 * Wayland xdg shell surface class.
 * @ingroup wayland
 ******************************************************************************/
class XdgSurface
{
public:

	~XdgSurface();

	/**
	 * Returns associated surface
	 */
	SurfacePtr getSurface() const { return mSurface; }

private:

	friend class Display;
	friend class XdgShell;

	XdgSurface(zxdg_shell_v6* xdgShell, SurfacePtr surface);

	zxdg_surface_v6* mWlXdgSurface;
	zxdg_toplevel_v6* mWlTopLevel;
	SurfacePtr mSurface;
	zxdg_surface_v6_listener mWlSurfaceListener;
	zxdg_toplevel_v6_listener mWlTopLevelListener;

	XenBackend::Log mLog;

	static void sConfigSurfaceHandler(void *data, zxdg_surface_v6 *xdgSurface,
									  uint32_t serial);
	static void sConfigTopLevelHandler(void *data, zxdg_toplevel_v6 *topLevel,
									   int32_t width, int32_t height,
									   wl_array *states);
	static void sCloseTopLevelHandler(void *data, zxdg_toplevel_v6 *topLevel);
	void configSurfaceHandler(uint32_t serial);
	void configTopLevelHandler(int32_t width, int32_t height, wl_array *states);
	void closeTopLevelHandler();

	void init(zxdg_shell_v6* xdgShell);
	void release();
};

typedef std::shared_ptr<XdgSurface> XdgSurfacePtr;

}

#endif /* SRC_WAYLAND_XDGSURFACE_HPP_ */
