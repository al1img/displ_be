#include "catch.hpp"

#include "drm/Display.hpp"

#include "mocks/drm/DrmMock.hpp"

using Drm::Display;

TEST_CASE("DRM Display")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	REQUIRE_NOTHROW(display.start());

	SECTION("Check magic")
	{
		CHECK(display.getMagic() != 0);
	}

#ifdef WITH_ZCOPY
	SECTION("Check ZCopy")
	{
		REQUIRE(display.isZeroCopySupported());

		DisplayItf::GrantRefs refs;

		auto buffer1 = display.createZCopyBuffer(800, 600, 32, 0, refs, true);

		CHECK(buffer1 != nullptr);

		auto buffer2 = display.createZCopyBuffer(800, 600, 32, 0, refs, false);

		CHECK(buffer2 != nullptr);
	}
#endif

	SECTION("Check connectors")
	{
		auto connector1 = display.createConnector("HDMI-A-0");

		CHECK(connector1 != nullptr);

		auto connector2 = display.createConnector("HDMI-A-1");

		CHECK(connector2 != nullptr);

		CHECK_THROWS_AS(display.createConnector("Invalid"), Drm::Exception);
	}

	SECTION("Check buffers")
	{
		DisplayItf::GrantRefs refs;

		auto dumb1 = display.createDisplayBuffer(800, 600, 32);

		CHECK(dumb1 != nullptr);

		auto dumb2 = display.createDisplayBuffer(800, 600, 32, 0, refs, true);

		CHECK(dumb2 != nullptr);

		auto dumb3 = display.createDisplayBuffer(800, 600, 32, 0, refs, false);

		CHECK(dumb3 != nullptr);

		auto fb = display.createFrameBuffer(dumb1, 800, 600, 0);
	}

	REQUIRE_NOTHROW(display.flush());

	REQUIRE_NOTHROW(display.stop());
}

TEST_CASE("DRM Display without ZCopy")
{
	DrmMock::reset();
	DrmMock::setDisableZCopy(true);

	Display display("/dev/dri/card0");

	REQUIRE_NOTHROW(display.start());

	SECTION("Check buffers")
	{
		DisplayItf::GrantRefs refs;

		CHECK_THROWS_AS(display.createDisplayBuffer(800, 600, 32, 0, refs, true),
						Drm::Exception);

		auto dumb1 = display.createDisplayBuffer(800, 600, 32, 0, refs, false);

		CHECK(dumb1 != nullptr);
	}

	REQUIRE_NOTHROW(display.flush());

	REQUIRE_NOTHROW(display.stop());
}

TEST_CASE("DRM Display error path")
{
	DrmMock::reset();

	SECTION("Check second start")
	{
		Display display("/dev/dri/card0");

		CHECK_NOTHROW(display.start());
		CHECK_NOTHROW(display.start());
	}

	SECTION("Check second stop")
	{
		Display display("/dev/dri/card0");

		CHECK_NOTHROW(display.stop());
		CHECK_NOTHROW(display.stop());
	}

	SECTION("Check create error")
	{
		DrmMock::setErrorMode(true);

		CHECK_THROWS_AS([](){ Display display("/dev/dri/card0"); }(),
							  Drm::Exception);
	}
}
