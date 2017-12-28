/*
 *  Connector class
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

#include "Connector.hpp"

#include "Exception.hpp"
#include "SurfaceManager.hpp"

using DisplayItf::FrameBufferPtr;

namespace Wayland {

/*******************************************************************************
 * Connector
 ******************************************************************************/

Connector::Connector(const std::string& name, CompositorPtr compositor, wl_display *wlDisplay) :
	mCompositor(compositor),
	mName(name),
	mWlDisplay(wlDisplay),
	mLog("Connector"),
	mWlEglWindow(nullptr),
	mEglSurface(nullptr)
{
	LOG(mLog, DEBUG) << "Create, name: "  << mName;
}

Connector::~Connector()
{
	LOG(mLog, DEBUG) << "Delete, name: " << mName;
}

/*******************************************************************************
 * Public
 ******************************************************************************/

void Connector::init(uint32_t width, uint32_t height,
					 FrameBufferPtr frameBuffer)
{
	onInit(mCompositor->createSurface(), frameBuffer, width, height);
}

void Connector::release()
{
	onRelease();
}

void Connector::pageFlip(FrameBufferPtr frameBuffer, FlipCallback cbk)
{
	DLOG(mLog, DEBUG) << "Page flip, name: " << mName;

	mSurface->draw(frameBuffer, cbk);
}

/*******************************************************************************
 * Protected
 ******************************************************************************/

void Connector::onInit(SurfacePtr surface, FrameBufferPtr frameBuffer,
					   uint32_t width, uint32_t height)
{
	LOG(mLog, DEBUG) << "Init, name: " << mName;

	if (isInitialized())
	{
		throw Exception("Connector already initialized", EPERM);
	}

	mSurface = surface;

	createPlatformSurface(width, height);

	SurfaceManager::getInstance().createSurface(mName, mSurface->mWlSurface);

	mSurface->draw(frameBuffer);
}

void Connector::onRelease()
{
	LOG(mLog, DEBUG) << "Release, name: " << mName;

	SurfaceManager::getInstance().deleteSurface(mName, mSurface->mWlSurface);

	if (mWlEglWindow)
	{
		wl_egl_window_destroy(mWlEglWindow);
	}

	if (mEglSurface)
	{
		eglDestroySurface(mEglDisplay, mEglSurface);
	}

	eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(mEglDisplay);

	mSurface.reset();
}

/*******************************************************************************
 * Private
 ******************************************************************************/

void Connector::createPlatformSurface(uint32_t width, uint32_t height)
{
	EGLint major, minor;
	int nConfig;
	EGLint configAttribs[] = {
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_RED_SIZE,   8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE,  8,
			EGL_ALPHA_SIZE, 8,
			//EGL_SAMPLE_BUFFERS, 1,
			//EGL_SAMPLES,        2,
			EGL_NONE,
	};
	EGLint contextAttribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE,
	};

	mWlEglWindow = wl_egl_window_create(mSurface->mWlSurface, width, height);

	mEglDisplay = eglGetDisplay((EGLNativeDisplayType)mWlDisplay);

	if (eglGetError() != EGL_SUCCESS)
	{
		throw Exception("Can't init EGL", EINVAL);
	}

	if (!eglInitialize(mEglDisplay, &major, &minor))
	{
		throw Exception("Can't init EGL", EINVAL);
	}

	eglBindAPI(EGL_OPENGL_ES_API);

	if (!eglChooseConfig(mEglDisplay, configAttribs, &mEglConfig, 1, &nConfig)
			|| (nConfig != 1)){
		throw Exception("Can't init EGL", EINVAL);
	}

	mEglSurface = eglCreateWindowSurface(mEglDisplay, mEglConfig,
										 (EGLNativeWindowType)mWlEglWindow, nullptr);

	mEglContext = eglCreateContext(mEglDisplay, mEglConfig, nullptr, contextAttribs);

	eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext);
}

}
