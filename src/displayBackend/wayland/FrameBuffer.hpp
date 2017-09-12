/*
 * FrameBuffer.hpp
 *
 *  Created on: Sep 4, 2017
 *      Author: al1
 */

#ifndef SRC_WAYLAND_FRAMEBUFFER_HPP_
#define SRC_WAYLAND_FRAMEBUFFER_HPP_

#include <wayland-client.h>

#include "DisplayItf.hpp"

namespace Wayland {

class FrameBuffer
{
public:
	FrameBuffer();

protected:
	wl_buffer* mWlBuffer;

};

}

#endif /* SRC_DISPLAYBACKEND_WAYLAND_FRAMEBUFFER_HPP_ */
