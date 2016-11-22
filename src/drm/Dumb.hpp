/*
 *  Dumb class
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Copyright (C) 2016 EPAM Systems Inc.
 *
 */

#ifndef SRC_DRM_DUMB_HPP_
#define SRC_DRM_DUMB_HPP_

#include <cstdint>

namespace Drm {

class Device;

class Dumb
{
public:
	Dumb(Device& drm, uint32_t width, uint32_t height, uint32_t bpp);
	~Dumb();

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }
	uint32_t getHandle() const { return mHandle; }
	uint32_t getPitch() const { return mPitch; }
	uint32_t getSize() const { return mSize; }
	void* getBuffer() const { return mBuffer; }

private:
	Device& mDrm;
	uint32_t mHandle;
	uint32_t mPitch;
	uint32_t mWidth;
	uint32_t mHeight;
	uint64_t mSize;
	void* mBuffer;

	void release();
};

}

#endif /* SRC_DRM_DUMB_HPP_ */
