/*
 * SharedBuffer.hpp
 *
 *  Created on: Nov 24, 2016
 *      Author: al1
 */

#ifndef SRC_WAYLAND_SHAREDBUFFER_HPP_
#define SRC_WAYLAND_SHAREDBUFFER_HPP_

#include <memory>

#include <wayland-client.h>

#include <xen/be/Log.hpp>

#include "SharedFile.hpp"

#include "DisplayItf.hpp"

namespace Wayland {

class SharedBuffer : public FrameBufferItf
{
public:

	~SharedBuffer();

	/**
	 * Returns pointer to the display buffer
	 */
	std::shared_ptr<DisplayBufferItf> getDisplayBuffer() override
	{
		return std::dynamic_pointer_cast<DisplayBufferItf>(mSharedFile);
	}

private:

	friend class SharedMemory;
	friend class Surface;

	SharedBuffer(wl_shm* sharedMemory,
				 std::shared_ptr<SharedFile> sharedFile,
				 uint32_t width, uint32_t height,
				 uint32_t pixelFormat);

	std::shared_ptr<SharedFile> mSharedFile;
	wl_buffer* mBuffer;
	wl_shm_pool* mPool;
	uint32_t mWidth;
	uint32_t mHeight;
	XenBackend::Log mLog;

	void init(wl_shm* sharedMemory, uint32_t pixelFormat);
	void release();
};

}

#endif /* SRC_WAYLAND_SHAREDBUFFER_HPP_ */
