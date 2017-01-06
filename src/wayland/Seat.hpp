/*
 * Seat.hpp
 *
 *  Created on: Nov 30, 2016
 *      Author: al1
 */

#ifndef SRC_WAYLAND_SEAT_HPP_
#define SRC_WAYLAND_SEAT_HPP_

#include <memory>
#include <mutex>

#include <xen/be/Log.hpp>

#include "InputItf.hpp"

#include "Registry.hpp"
#include "SeatKeyboard.hpp"
#include "SeatPointer.hpp"
#include "SeatTouch.hpp"
#include "ShellSurface.hpp"

namespace Wayland {

/***************************************************************************//**
 * Wayland seat class.
 * @ingroup wayland
 ******************************************************************************/
class Seat : public Registry
{
public:

	~Seat();

	std::shared_ptr<SeatKeyboard> getKeyboard();
	std::shared_ptr<SeatPointer> getPointer();
	std::shared_ptr<SeatTouch> getTouch();

private:

//  Wayland 1.11
//	static const int cVersion = 5;
	static const int cVersion = 4;

	friend class Display;

	Seat(wl_registry* registry, uint32_t id, uint32_t version);

	wl_seat* mWlSeat;
	XenBackend::Log mLog;

	wl_seat_listener mWlListener;

	std::shared_ptr<SeatKeyboard> mSeatKeyboard;
	std::shared_ptr<SeatPointer> mSeatPointer;
	std::shared_ptr<SeatTouch> mSeatTouch;

	std::mutex mMutex;

	static void sReadCapabilities(void *data, wl_seat* seat,
								  uint32_t capabilities);
	static void sReadName(void *data, wl_seat* seat, const char *name);

	void readCapabilities(uint32_t capabilities);
	void readName(const std::string& name);

	void init();
	void release();
};

}

#endif /* SRC_WAYLAND_SEAT_HPP_ */
