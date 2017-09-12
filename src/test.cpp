/*
 * test.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: al1
 */

#include <signal.h>

#include "xen/be/Log.hpp"

#include "wayland/Display.hpp"

using std::bind;

using XenBackend::Log;
using Wayland::Display;

using DisplayItf::ConnectorPtr;
using DisplayItf::DisplayBufferPtr;
using DisplayItf::FrameBufferPtr;

void waitSignals()
{
	sigset_t set;
	int signal;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigprocmask(SIG_BLOCK, &set, nullptr);

	sigwait(&set,&signal);
}

void conFlipCbk(ConnectorPtr con, FrameBufferPtr fb0, FrameBufferPtr fb1)
{
//	LOG("CBK", ERROR) << "Flip";

	con->pageFlip(fb0, bind(conFlipCbk, con, fb1, fb0));
}

int main()
{
	Log::setLogLevel("Debug");

	try
	{
		LOG("Test", DEBUG) << "Start";

		Display display;

		display.start();

		DisplayBufferPtr db;

		db = display.createDisplayBuffer(400, 300, 32);
		auto fb0_0 = display.createFrameBuffer(db, 400, 300, 0x34325258);
		db = display.createDisplayBuffer(400, 300, 32);
		auto fb0_1 = display.createFrameBuffer(db, 400, 300, 0x34325258);

		db = display.createDisplayBuffer(400, 300, 32);
		auto fb1_0 = display.createFrameBuffer(db, 400, 300, 0x34325258);
		db = display.createDisplayBuffer(400, 300, 32);
		auto fb1_1 = display.createFrameBuffer(db, 400, 300, 0x34325258);

		for (int i = 0; i < fb0_0->getDisplayBuffer()->getSize() / sizeof(uint32_t); i++)
		{
			((uint32_t*)fb0_0->getDisplayBuffer()->getBuffer())[i] = 0xFF;
			((uint32_t*)fb0_1->getDisplayBuffer()->getBuffer())[i] = 0xFFFFFF;
			((uint32_t*)fb1_0->getDisplayBuffer()->getBuffer())[i] = 0xFFFF00;
			((uint32_t*)fb1_1->getDisplayBuffer()->getBuffer())[i] = 0xFF0000;
		}

		auto con0 = display.createConnector("Window1");

		con0->init(400, 300, fb0_0);
		//con0->pageFlip(fb0_0, bind(conFlipCbk, con0, fb0_0, fb0_1));

		auto con1 = display.createConnector("Window2");

		con1->init(400, 300, fb1_0);
		//con1->pageFlip(fb1_0, bind(conFlipCbk, con1, fb1_0, fb1_1));

#if 0

		auto connector2 = display.createConnector("Window2");
		auto displayBuffer2 = display.createDisplayBuffer(400, 300, 32);
		auto frameBuffer2 = display.createFrameBuffer(displayBuffer2, 400, 300, 0x34325258);

		connector2->init(400, 300, frameBuffer2);

		connector1->pageFlip();
		connector2->pageFlip();
#endif

		waitSignals();

		con0->release();
#if 0
		connector2->release();

//		display.clearConnectors();

		usleep(1000000);

		connector1->init(400, 300, frameBuffer1);

		usleep(1000000);

		connector1->release();
#endif
		display.stop();

		LOG("Test", DEBUG) << "Stop";
	}
	catch(const std::exception& e)
	{
		LOG("Test", ERROR) << e.what();
	}

	return 0;
}


