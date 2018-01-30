#include "catch.hpp"
#include "drm/Display.hpp"

using Drm::Display;

TEST_CASE("DRM Display")
{
	Display display("/dev/dri/card0");

	REQUIRE_NOTHROW(display.start());

	SECTION("Check magic")
	{
		CHECK(display.getMagic() != 0);
	}

#ifdef WITH_ZCOPY
	SECTION("Check ZCopy")
	{
		CHECK(display.isZeroCopySupported());

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
	}

	REQUIRE_NOTHROW(display.flush());

	REQUIRE_NOTHROW(display.stop());
}
