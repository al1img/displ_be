#ifndef TESTS_MOCKS_WAYLAND_WAYLANDMOCK_HPP_
#define TESTS_MOCKS_WAYLAND_WAYLANDMOCK_HPP_

#include <functional>
#include <list>
#include <mutex>

#include <stdio.h>
#include <stdint.h>

#include <wayland-client.h>

#ifdef WITH_IVI_EXTENSION
#include <ilm/ivi-application-client-protocol.h>
#endif

#ifdef WITH_ZCOPY
#include "wayland-drm-client-protocol.h"
#include "wayland-kms-client-protocol.h"
#endif

#include "Pipe.hpp"

class wl_display;

class wl_proxy
{
public:
	wl_proxy(wl_display *display) : mDisplay(display) {}
	virtual ~wl_proxy() {}

	virtual wl_proxy* marshalConstructor(uint32_t opcode,
										 const wl_interface *interface,
										 va_list args)
	{
		return nullptr;
	}

	virtual int addListener(void (**implementation)(void), void *data)
	{
		return -1;
	}

protected:

	wl_display* mDisplay;
};

class wl_display : public wl_proxy
{
public:

	typedef std::function<void()> Event;

	wl_display() :
		wl_proxy(this), mListener(nullptr), mData(nullptr) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;

	int getFd() const { return mPipe.getFd(); }

	void addEvent(Event event);

	int processEvents();

	static void reset();
	static void setErrorMode(bool errorMode) { sErrorMode = errorMode; }
	static bool getErrorMode() { return sErrorMode; }

private:

	static bool sErrorMode;

	wl_display_listener* mListener;
	void* mData;

	std::mutex mMutex;
	std::list<Event> mEvents;

	Pipe mPipe;
};

class wl_registry : public wl_proxy
{
public:

	wl_registry(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;

	int addListener(void (**implementation)(void), void *data) override;

	static void reset();

#ifdef WITH_IVI_EXTENSION
	static void enableIviApplication(bool value) { sEnableIviApplication = value; }
#endif

#ifdef WITH_ZCOPY
	static void enableDrm(bool value) { sEnableDrm = value; }
	static void enableKms(bool value) { sEnableKms = value; }
#endif

	static void enableShell(bool value) { sEnableShell = value; }
	static void enableShmem(bool value) { sEnableShmem = value; }
	static void enableSeat(bool value) { sEnableSeat = value; }

private:

#ifdef WITH_IVI_EXTENSION
	static bool sEnableIviApplication;
#endif

#ifdef WITH_ZCOPY
	static bool sEnableDrm;
	static bool sEnableKms;
#endif

	static bool sEnableShell;
	static bool sEnableShmem;
	static bool sEnableSeat;

	wl_registry_listener* mListener;
	void* mData;
};

class wl_compositor : public wl_proxy
{
public:

	wl_compositor(wl_display *display) : wl_proxy(display) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;
};

class wl_shell : public wl_proxy
{
public:

	wl_shell(wl_display *display) : wl_proxy(display) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;
};

class wl_shell_surface : public wl_proxy
{
public:

	wl_shell_surface(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

	int addListener(void (**implementation)(void), void *data) override
	{
		return 0;
	}

private:

	wl_shell_surface_listener* mListener;
	void* mData;
};

class wl_shm : public wl_proxy
{
public:

	wl_shm(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;

	int addListener(void (**implementation)(void), void *data) override;

private:

	wl_shm_listener* mListener;
	void* mData;
};

class wl_surface : public wl_proxy
{
public:

	wl_surface(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

private:

	wl_surface_listener* mListener;
	void* mData;
};

class wl_shm_pool : public wl_proxy
{
public:

	wl_shm_pool(wl_display *display) : wl_proxy(display) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;
};

class wl_buffer : public wl_proxy
{
public:

	wl_buffer(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

	int addListener(void (**implementation)(void), void *data) override;

private:

	wl_buffer_listener* mListener;
	void* mData;

};

class wl_seat : public wl_proxy
{
public:

	wl_seat(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;

	int addListener(void (**implementation)(void), void *data) override;

	static void reset();
	static void setCapabilities(uint32_t capabilities)
	{
		sCapabilities = capabilities;
	}

private:

	static uint32_t sCapabilities;

	wl_seat_listener* mListener;
	void* mData;
};

class wl_pointer : public wl_proxy
{
public:

	wl_pointer(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

private:

	wl_pointer_listener* mListener;
	void* mData;
};

class wl_keyboard : public wl_proxy
{
public:

	wl_keyboard(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

private:

	wl_keyboard_listener* mListener;
	void* mData;
};

class wl_touch : public wl_proxy
{
public:

	wl_touch(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

private:

	wl_touch_listener* mListener;
	void* mData;
};

#ifdef WITH_IVI_EXTENSION

class ivi_application : public wl_proxy
{
public:

	ivi_application(wl_display *display) : wl_proxy(display) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;
};

class ivi_surface : public wl_proxy
{
public:

	ivi_surface(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

private:

	ivi_surface_listener* mListener;
	void* mData;
};

#endif

#ifdef WITH_ZCOPY

class wl_drm : public wl_proxy
{
public:

	wl_drm(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;

	int addListener(void (**implementation)(void), void *data) override;

private:

	wl_drm_listener* mListener;
	void* mData;
};

class wl_kms : public wl_proxy
{
public:

	wl_kms(wl_display *display) :
		wl_proxy(display), mListener(nullptr), mData(nullptr) {}

	wl_proxy* marshalConstructor(uint32_t opcode,
								 const wl_interface *interface,
								 va_list args) override;

	int addListener(void (**implementation)(void), void *data) override;

private:

	wl_kms_listener* mListener;
	void* mData;
};

#endif

#endif /* TESTS_MOCKS_WAYLAND_WAYLANDMOCK_HPP_ */
