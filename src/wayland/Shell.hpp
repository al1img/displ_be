/*
 * Shell.hpp
 *
 *  Created on: Nov 24, 2016
 *      Author: al1
 */

#ifndef SRC_WAYLAND_SHELL_HPP_
#define SRC_WAYLAND_SHELL_HPP_

#include <xen/be/Log.hpp>

#include "Registry.hpp"
#include "ShellSurface.hpp"

namespace Wayland {

class Shell : public Registry
{
public:

	~Shell();

	std::shared_ptr<ShellSurface> getShellSurface(std::shared_ptr<Surface>
												  surface);

private:

	friend class Display;

	Shell(wl_registry* registry, uint32_t id, uint32_t version);

	wl_shell* mShell;
	XenBackend::Log mLog;

	void init();
	void release();
};

}

#endif /* SRC_WAYLAND_SHELL_HPP_ */
