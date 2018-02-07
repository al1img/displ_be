#include "catch.hpp"

#include "wayland/Display.hpp"
#include "wayland/Connector.hpp"
#include "wayland/Exception.hpp"

#include "mocks/wayland/WaylandMock.hpp"

using Wayland::Display;
using DisplayItf::DisplayPtr;
using DisplayItf::ConnectorPtr;

TEST_CASE("Wayland Connector")
{
	wl_display::reset();
	wl_registry::reset();
	wl_seat::reset();

	std::string name("1");

	DisplayPtr display;
	ConnectorPtr connector;

	SECTION("Shell connector")
	{
		wl_registry::enableShell(true);
		wl_registry::enableShmem(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		connector = display->createConnector(name);
	}

#ifdef WITH_IVI_EXTENSION

	SECTION("Ivi connector")
	{
		wl_registry::enableIviApplication(true);
		wl_registry::enableShmem(true);


		REQUIRE_NOTHROW(display.reset(new Display()));

		connector = display->createConnector("1");
	}

#endif

	SECTION("Compositor connector")
	{
		wl_registry::enableShmem(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		connector = display->createConnector("1");
	}

	REQUIRE(connector != nullptr);

	CHECK(connector->isConnected());

	CHECK(connector->getName() == name);

	DisplayItf::GrantRefs refs;

	auto dumb1 = display->createDisplayBuffer(800, 600, 32, 0, refs, false);

	REQUIRE(dumb1 != nullptr);

	auto frameBuffer1 = display->createFrameBuffer(dumb1, 800, 600, 0);

	REQUIRE(frameBuffer1 != nullptr);

	CHECK_NOTHROW(connector->init(800, 600, frameBuffer1));

	auto dumb2 = display->createDisplayBuffer(800, 600, 32, 0, refs, false);

	REQUIRE(dumb2 != nullptr);

	auto frameBuffer2 = display->createFrameBuffer(dumb2, 800, 600, 0);

	REQUIRE(frameBuffer2 != nullptr);

	CHECK_NOTHROW(connector->pageFlip(frameBuffer2, nullptr));

	CHECK_NOTHROW(connector->release());

	CHECK_NOTHROW(connector->release());

	CHECK_NOTHROW(connector->init(800, 600, frameBuffer1));
}

TEST_CASE("Wayland Connector error path")
{
	wl_display::reset();
	wl_registry::reset();
	wl_seat::reset();

	wl_registry::enableShell(true);
	wl_registry::enableShmem(true);

	DisplayPtr display;
	ConnectorPtr connector;

	REQUIRE_NOTHROW(display.reset(new Display()));

	connector = display->createConnector("1");

	REQUIRE(connector != nullptr);

	DisplayItf::GrantRefs refs;

	auto dumb = display->createDisplayBuffer(800, 600, 32, 0, refs, false);

	REQUIRE(dumb != nullptr);

	auto frameBuffer = display->createFrameBuffer(dumb, 800, 600, 0);

	REQUIRE(frameBuffer != nullptr);

	CHECK_THROWS_AS(connector->pageFlip(frameBuffer, nullptr), Wayland::Exception);

	CHECK_NOTHROW(connector->init(800, 600, frameBuffer));

	CHECK_THROWS_AS(connector->init(800, 600, frameBuffer), Wayland::Exception);
}
