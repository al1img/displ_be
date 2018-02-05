#include "catch.hpp"

#include "drm/Display.hpp"
#include "drm/Dumb.hpp"
#include "drm/FrameBuffer.hpp"

#include "mocks/drm/DrmMock.hpp"

using Drm::Display;
using Drm::DumbDrm;
using Drm::FrameBuffer;

/*******************************************************************************
 * Static variables
 ******************************************************************************/

/*******************************************************************************
 * Static functions
 ******************************************************************************/

/*******************************************************************************
 * Test cases
 ******************************************************************************/

TEST_CASE("DRM FrameBuffer")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	auto dumb = display.createDisplayBuffer(800, 600, 32);

	FrameBuffer frameBuffer(display.getFd(), dumb, 800, 600, 0);

	CHECK(frameBuffer.getHandle() != 0);
	CHECK(frameBuffer.getWidth() == 800);
	CHECK(frameBuffer.getHeight() == 600);
	CHECK(frameBuffer.getDisplayBuffer() == dumb);
}

TEST_CASE("DRM FrameBuffer error path")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	auto dumb = display.createDisplayBuffer(800, 600, 32);

	DrmMock::setErrorMode(true);

	CHECK_THROWS_AS([&]()
					{ FrameBuffer frameBuffer(display.getFd(), dumb, 800, 600, 0); }(),
					Drm::Exception);
}
