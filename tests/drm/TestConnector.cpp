#include "catch.hpp"

#include "drm/Display.hpp"
#include "drm/Connector.hpp"

#include "mocks/drm/DrmMock.hpp"

using Drm::Display;

TEST_CASE("DRM Connector")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	REQUIRE_NOTHROW(display.start());

	auto connector = display.createConnector("HDMI-A-0");

	REQUIRE(connector != nullptr);

	SECTION("Check Init")
	{
		CHECK(connector->getName() == "HDMI-A-0");

		CHECK(connector->isConnected());

		CHECK_FALSE(connector->isInitialized());

		auto dumb = display.createDisplayBuffer(800, 600, 32);
		auto fb = display.createFrameBuffer(dumb, 800, 600, 0);

		CHECK_NOTHROW(connector->init(800, 600, fb));

		CHECK(connector->isInitialized());

		CHECK_NOTHROW(connector->release());

		CHECK_FALSE(connector->isInitialized());
	}

	SECTION("Check Used crtc")
	{
		auto dumb1 = display.createDisplayBuffer(800, 600, 32);
		auto fb1 = display.createFrameBuffer(dumb1, 800, 600, 0);

		CHECK_NOTHROW(connector->init(800, 600, fb1));

		DrmMock::getDrmMock(display.getFd())->setEncoderCrtcId(2, 1);

		auto connector2 = display.createConnector("HDMI-A-1");

		auto dumb2 = display.createDisplayBuffer(800, 600, 32);
		auto fb2 = display.createFrameBuffer(dumb2, 800, 600, 0);

		CHECK_NOTHROW(connector2->init(800, 600, fb2));
	}

	REQUIRE_NOTHROW(display.flush());

	REQUIRE_NOTHROW(display.stop());
}

TEST_CASE("DRM Page flip")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	REQUIRE_NOTHROW(display.start());

	auto connector = display.createConnector("HDMI-A-0");

	REQUIRE(connector != nullptr);

	auto dumb1 = display.createDisplayBuffer(800, 600, 32);
	auto fb1 = display.createFrameBuffer(dumb1, 800, 600, 0);

	auto dumb2 = display.createDisplayBuffer(800, 600, 32);
	auto fb2 = display.createFrameBuffer(dumb2, 800, 600, 0);

	CHECK_NOTHROW(connector->init(800, 600, fb1));

	CHECK_NOTHROW(connector->pageFlip(fb2, nullptr));

	REQUIRE_NOTHROW(display.flush());

	REQUIRE_NOTHROW(display.stop());
}

TEST_CASE("DRM Connector error path")
{
	DrmMock::reset();

	Display display("/dev/dri/card0");

	REQUIRE_NOTHROW(display.start());

	auto connector = display.createConnector("HDMI-A-0");

	REQUIRE(connector != nullptr);

	SECTION("Check second init")
	{
		auto dumb = display.createDisplayBuffer(800, 600, 32);
		auto fb = display.createFrameBuffer(dumb, 800, 600, 0);

		CHECK_NOTHROW(connector->init(800, 600, fb));

		CHECK_THROWS_AS(connector->init(800, 600, fb), Drm::Exception);
	}

	SECTION("Check second release")
	{
		auto dumb = display.createDisplayBuffer(800, 600, 32);
		auto fb = display.createFrameBuffer(dumb, 800, 600, 0);

		CHECK_NOTHROW(connector->init(800, 600, fb));

		CHECK_NOTHROW(connector->release());

		CHECK_NOTHROW(connector->release());
	}

	SECTION("Check unsupported mode")
	{
		auto dumb = display.createDisplayBuffer(320, 200, 32);
		auto fb = display.createFrameBuffer(dumb, 320, 200, 0);

		CHECK_THROWS_AS(connector->init(320, 200, fb), Drm::Exception);
	}

	SECTION("Check error set mode")
	{
		auto dumb = display.createDisplayBuffer(800, 600, 32);
		auto fb = display.createFrameBuffer(dumb, 800, 600, 0);

		DrmMock::setErrorMode(true);

		CHECK_THROWS_AS(connector->init(800, 600, fb), Drm::Exception);
	}

	SECTION("Check disconnected")
	{
		auto dumb = display.createDisplayBuffer(800, 600, 32);
		auto fb = display.createFrameBuffer(dumb, 800, 600, 0);

		DrmMock::getDrmMock(display.getFd())->setConnected(1, false);

		CHECK_THROWS_AS(connector->init(800, 600, fb), Drm::Exception);
	}

	REQUIRE_NOTHROW(display.flush());

	REQUIRE_NOTHROW(display.stop());

}
