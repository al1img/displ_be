#include "catch.hpp"

#include "drm/Display.hpp"

using Drm::Display;

TEST_CASE("DRM Display")
{
	Display display("/dev/dri/card0");

	display.start();

	REQUIRE(display.getMagic() != 0);

	display.stop();
}
