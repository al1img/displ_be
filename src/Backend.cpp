/*
 *  Backend
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Copyright (C) 2016 EPAM Systems Inc.
 */

#include <algorithm>
#include <exception>
#include <iostream>

#include <csignal>
#include <execinfo.h>
#include <getopt.h>

#include <xen/be/Log.hpp>

#include "DisplayBackend.hpp"
#include "InputBackend.hpp"

using std::chrono::milliseconds;
using std::atomic_bool;
using std::cout;
using std::dynamic_pointer_cast;
using std::endl;
using std::exception;
using std::string;
using std::this_thread::sleep_for;
using std::toupper;
using std::transform;

using XenBackend::Log;

atomic_bool gTerminate(false);
DisplayMode gDisplayMode = DisplayMode::WAYLAND;

/*******************************************************************************
 *
 ******************************************************************************/

void terminateHandler(int signal)
{
	gTerminate = true;
}

void segmentationHandler(int sig)
{
	void *array[20];
	size_t size;

	LOG("Main", ERROR) << "Segmentation fault!";

	size = backtrace(array, 20);

	backtrace_symbols_fd(array, size, STDERR_FILENO);

	exit(1);
}

void registerSignals()
{
	signal(SIGINT, terminateHandler);
	signal(SIGTERM, terminateHandler);
	signal(SIGSEGV, segmentationHandler);
}

bool commandLineOptions(int argc, char *argv[])
{

	int opt = -1;

	while((opt = getopt(argc, argv, "m:v:fh?")) != -1)
	{
		switch(opt)
		{
		case 'v':
			if (!Log::setLogLevel(string(optarg)))
			{
				return false;
			}

			break;

		case 'm':
		{
			string strMode(optarg);

			transform(strMode.begin(), strMode.end(), strMode.begin(),
					  (int (*)(int))toupper);

			if (strMode == "DRM")
			{
				gDisplayMode = DisplayMode::DRM;
			}

			break;
		}
		case 'f':
			Log::setShowFileAndLine(true);
			break;

		default:
			return false;
		}
	}

	return true;
}

int main(int argc, char *argv[])
{
	try
	{
		registerSignals();

		if (commandLineOptions(argc, argv))
		{
			DisplayBackend displayBackend(gDisplayMode, XENDISPL_DRIVER_NAME,
														0, 0);

			InputBackend inputBackend(
				dynamic_pointer_cast<Wayland::Display>
						(displayBackend.getDisplay()), "vkbd", 0, 0);

			displayBackend.start();
			inputBackend.start();

			while (!gTerminate);
		}
		else
		{
			cout << "Usage: " << argv[0] << " [-m <mode>] [-v <level>]" << endl;
			cout << "\t-v -- verbose level "
				 << "(disable, error, warning, info, debug)" << endl;
			cout << "\t-m -- mode "
				 << "(drm, wayland)" << endl;
		}
	}
	catch(const exception& e)
	{
		LOG("Main", ERROR) << e.what();
	}

	return 0;
}
