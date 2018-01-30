#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_COLOUR_NONE

#include <xen/be/Log.hpp>

#include "catch.hpp"

#include "drm/Display.hpp"

int main( int argc, char* argv[] )
{
	XenBackend::Log::setLogMask("*:Debug");

	int result = Catch::Session().run( argc, argv );

	return result;
}
