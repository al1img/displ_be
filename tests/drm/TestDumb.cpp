#include "catch.hpp"

#include "drm/Display.hpp"
#include "drm/Dumb.hpp"

#include "mocks/drm/DrmMock.hpp"

using Drm::Display;
using Drm::DumbDrm;
using Drm::DumbZCopyFront;
using Drm::DumbZCopyFrontDrm;
using Drm::DumbZCopyBack;

/*******************************************************************************
 * Static variables
 ******************************************************************************/

/*******************************************************************************
 * Static functions
 ******************************************************************************/

/*******************************************************************************
 * Test cases
 ******************************************************************************/

TEST_CASE("DRM Dumb")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	SECTION("Without refs")
	{
		DumbDrm dumb(display.getFd(), 800, 600, 32);

		CHECK(dumb.getFd() != -1);

		CHECK(dumb.getHandle() != 0);

		CHECK(DrmMock::isDumbExist(dumb.getHandle()));

		CHECK(dumb.getBuffer() != nullptr);

		CHECK_FALSE(dumb.needsCopy());

		CHECK_NOTHROW(dumb.readName());
	}

	SECTION("With refs")
	{
		DisplayItf::GrantRefs refs(512);

		DumbDrm dumb(display.getFd(), 800, 600, 32, 0, refs);

		CHECK(dumb.getFd() != -1);

		CHECK(dumb.getHandle() != 0);

		CHECK(DrmMock::isDumbExist(dumb.getHandle()));

		CHECK(dumb.getBuffer() != nullptr);

		CHECK(dumb.needsCopy());

		CHECK_NOTHROW(dumb.copy());
	}
}

TEST_CASE("DRM Dumb error path")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	SECTION("Check error on create")
	{
		DrmMock::setErrorMode(true);

		CHECK_THROWS_AS([&display]()
						{ DumbDrm dumb(display.getFd(), 800, 600, 32); }(),
						Drm::Exception);
	}

	SECTION("Check error on copy")
	{
		DumbDrm dumb(display.getFd(), 800, 600, 32);

		CHECK_THROWS_AS(dumb.copy(), Drm::Exception);
	}
}

#ifdef WITH_ZCOPY

TEST_CASE("DRM Front ZCopy Dumb")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	DisplayItf::GrantRefs refs(512);

	DumbZCopyFront dumb(display.getFd(), display.getZCopyFd(), 800, 600, 32, 0, refs);

	CHECK(dumb.getFd() != -1);
	CHECK(DrmMock::isPrimeFdExist(dumb.getFd()));

	CHECK(dumb.getHandle() != 0);
	CHECK(DrmMock::isDumbExist(dumb.getHandle()));
}

TEST_CASE("DRM Front ZCopy Dumb DRM")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	DisplayItf::GrantRefs refs(512);

	DumbZCopyFrontDrm dumb(display.getFd(), display.getZCopyFd(), 800, 600, 32, 0, refs);

	CHECK(dumb.getFd() != -1);

	CHECK(dumb.getHandle() != 0);
	CHECK(DrmMock::isDumbExist(dumb.getHandle()));
}

TEST_CASE("DRM Back ZCopy Dumb")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	DisplayItf::GrantRefs refs;

	DumbZCopyBack dumb(display.getFd(), display.getZCopyFd(), 800, 600, 32, 0, refs);

	CHECK(dumb.getFd() != -1);
	CHECK(DrmMock::isPrimeFdExist(dumb.getFd()));

	CHECK(dumb.getHandle() != 0);
	CHECK(DrmMock::isDumbExist(dumb.getHandle()));

	CHECK((refs.size() * 4096) >= dumb.getSize());
}

#endif
