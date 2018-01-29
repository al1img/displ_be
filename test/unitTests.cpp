#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_COLOUR_NONE

#include <catch.hpp>

#include "xen/be/Log.hpp"

int main( int argc, char* argv[] )
{
	XenBackend::Log::setLogMask("*:Disable");

	int result = Catch::Session().run( argc, argv );

	return ( result < 0xff ? result : 0xff );
}
