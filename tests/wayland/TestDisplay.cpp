#include "catch.hpp"

#include "wayland/Display.hpp"
#include "wayland/Exception.hpp"

#include "mocks/wayland/WaylandMock.hpp"

#ifdef WITH_ZCOPY
#include "mocks/drm/DrmMock.hpp"
#endif

#ifdef WITH_INPUT
#include "InputItf.hpp"
#endif

using Wayland::Display;
using Wayland::DisplayPtr;

TEST_CASE("Wayland Display")
{
	wl_display::reset();
	wl_registry::reset();
	wl_seat::reset();

	DisplayPtr display;

	SECTION("Check start stop")
	{
		REQUIRE_NOTHROW(display.reset(new Display()));

		CHECK_NOTHROW(display->start());

		CHECK_NOTHROW(display->flush());

		CHECK_NOTHROW(display->stop());
	}

	SECTION("Check shell connector")
	{
		wl_registry::enableShell(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		auto connector = display->createConnector("1");

		CHECK(connector != nullptr);
	}

#ifdef WITH_IVI_EXTENSION

	SECTION("Check ivi connector")
	{
		wl_registry::enableIviApplication(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		auto connector = display->createConnector("1");

		CHECK(connector != nullptr);
	}

#endif

	SECTION("Check compositor connector")
	{
		REQUIRE_NOTHROW(display.reset(new Display()));

		auto connector = display->createConnector("1");

		CHECK(connector != nullptr);
	}

	SECTION("Check buffers")
	{
		wl_registry::enableShmem(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		auto dumb = display->createDisplayBuffer(800, 600, 32);

		CHECK(dumb != nullptr);

		DisplayItf::GrantRefs refs;

		auto dumbRefs = display->createDisplayBuffer(800, 600, 32, 0, refs, false);

		REQUIRE(dumbRefs != nullptr);

		auto frameBuffer = display->createFrameBuffer(dumbRefs, 800, 600, 0);

		CHECK(frameBuffer != nullptr);
	}

#ifdef WITH_ZCOPY

	SECTION("Check DRM buffers")
	{
		DrmMock::reset();
		wl_registry::enableDrm(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		DisplayItf::GrantRefs refs;

		auto dumb = display->createDisplayBuffer(800, 600, 32, 0, refs, false);

		REQUIRE(dumb != nullptr);

		auto frameBuffer = display->createFrameBuffer(dumb, 800, 600, 0);

		CHECK(frameBuffer != nullptr);
	}

	SECTION("Check KMS buffers")
	{
		DrmMock::reset();
		wl_registry::enableKms(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		DisplayItf::GrantRefs refs;

		auto dumb = display->createDisplayBuffer(800, 600, 32, 0, refs, false);

		REQUIRE(dumb != nullptr);

		auto frameBuffer = display->createFrameBuffer(dumb, 800, 600, 0);

		CHECK(frameBuffer != nullptr);
	}

#endif

#ifdef WITH_INPUT

	SECTION("Check inputs")
	{
		wl_registry::enableSeat(true);
		wl_seat::setCapabilities(WL_SEAT_CAPABILITY_KEYBOARD |
								 WL_SEAT_CAPABILITY_POINTER |
								 WL_SEAT_CAPABILITY_TOUCH);

		REQUIRE_NOTHROW(display.reset(new Display()));

		InputItf::KeyboardCallbacks keyboardCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", keyboardCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::KeyboardCallbacks>("1"));

		InputItf::PointerCallbacks pointerCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", pointerCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::PointerCallbacks>("1"));

		InputItf::TouchCallbacks touchCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", touchCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::TouchCallbacks>("1"));
	}

#endif
}

TEST_CASE("Wayland Display error path")
{
	wl_display::reset();
	wl_registry::reset();
	wl_seat::reset();

	DisplayPtr display;

	SECTION("Check create error")
	{
		wl_display::setErrorMode(true);

		CHECK_THROWS_AS(display.reset(new Display()), Wayland::Exception);
	}

	SECTION("Check no shmem")
	{
		REQUIRE_NOTHROW(display.reset(new Display()));

		CHECK_THROWS_AS(display->createDisplayBuffer(800, 600, 32), Wayland::Exception);

		DisplayItf::GrantRefs refs;

		CHECK_THROWS_AS(display->createDisplayBuffer(800, 600, 32, 0, refs, false), Wayland::Exception);
	}

#ifdef WITH_IVI_EXTENSION

	SECTION("Check wrong ivi id")
	{
		wl_registry::enableIviApplication(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		CHECK_THROWS_AS(display->createConnector("Wrong"), Wayland::Exception);
	}

#endif

#ifdef WITH_INPUT

	SECTION("Check no seat")
	{
		REQUIRE_NOTHROW(display.reset(new Display()));

		InputItf::KeyboardCallbacks keyboardCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", keyboardCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::KeyboardCallbacks>("1"));

		InputItf::PointerCallbacks pointerCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", pointerCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::PointerCallbacks>("1"));

		InputItf::TouchCallbacks touchCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", touchCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::TouchCallbacks>("1"));
	}

	SECTION("Check no capabilities")
	{
		wl_registry::enableSeat(true);

		REQUIRE_NOTHROW(display.reset(new Display()));

		InputItf::KeyboardCallbacks keyboardCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", keyboardCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::KeyboardCallbacks>("1"));

		InputItf::PointerCallbacks pointerCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", pointerCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::PointerCallbacks>("1"));

		InputItf::TouchCallbacks touchCbks;

		CHECK_NOTHROW(display->setInputCallbacks("1", touchCbks));
		CHECK_NOTHROW(display->clearInputCallbacks<InputItf::TouchCallbacks>("1"));
	}

#endif

}
