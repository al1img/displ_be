#include "WaylandMock.hpp"

#include <errno.h>
#include <cstring>

using std::bind;
using std::mutex;
using std::lock_guard;

/*******************************************************************************
 * wayland-client interface
 ******************************************************************************/

extern "C" {

#ifdef WITH_IVI_EXTENSION
const wl_interface ivi_application_interface { "ivi_application", 1 };
const wl_interface ivi_surface_interface { "ivi_surface", 1 };
#endif
const wl_interface wl_buffer_interface { "wl_buffer", 1 };
const wl_interface wl_callback_interface { "wl_callback", 1 };
const wl_interface wl_compositor_interface { "wl_compositor", 1 };
const wl_interface wl_keyboard_interface { "wl_keyboard", 1 };
const wl_interface wl_pointer_interface { "wl_pointer", 1};
const wl_interface wl_registry_interface { "wl_regestry", 1 };
const wl_interface wl_seat_interface { "wl_seat", 1 };
const wl_interface wl_shell_interface { "wl_shell", 1 };
const wl_interface wl_shell_surface_interface { "wl_shell_surface", 1 };
const wl_interface wl_shm_interface { "wl_shm", 1 };
const wl_interface wl_shm_pool_interface { "wl_shm_pool", 1 };
const wl_interface wl_surface_interface { "wl_surface", 1 };
const wl_interface wl_touch_interface { "wl_touch", 1 };

void wl_proxy_marshal(struct wl_proxy *p, uint32_t opcode, ...)
{

}

wl_proxy* wl_proxy_marshal_constructor(wl_proxy *proxy, uint32_t opcode,
									   const wl_interface *interface, ...)
{
	va_list args;

	va_start(args, interface);

	auto result = proxy->marshalConstructor(opcode, interface, args);

	va_end(args);

	return result;
}

int wl_proxy_add_listener(wl_proxy *proxy, void (**implementation)(void),
						  void *data)
{

	return proxy->addListener(implementation, data);
}

void wl_proxy_destroy(struct wl_proxy *proxy)
{
	delete proxy;
}

int wl_display_flush(wl_display *display)
{
	return 0;
}

void wl_log_set_handler_client(wl_log_func_t handler)
{
}

wl_display* wl_display_connect(const char *name)
{
	if (wl_display::getErrorMode())
	{
		errno = ENOENT;
		return nullptr;
	}
	return new wl_display();
}

int wl_display_get_fd(wl_display *display)
{
	return display->getFd();
}

int wl_display_dispatch(wl_display *display)
{
	return -1;
}

int wl_display_roundtrip(wl_display *display)
{
	return display->processEvents();
}

void wl_display_disconnect(wl_display *display)
{
	delete display;
}

int wl_display_get_error(wl_display *display)
{
	return errno;
}

uint32_t wl_display_get_protocol_error(wl_display *display,
									   const wl_interface **interface,
									   uint32_t *id)
{
	return 0;
}

int wl_display_prepare_read(wl_display *display)
{
	return 0;
}

int wl_display_dispatch_pending(wl_display *display)
{
	return -1;
}

int wl_display_read_events(wl_display *display)
{
	return -1;
}

void wl_display_cancel_read(wl_display *display)
{

}

wl_proxy* wl_proxy_marshal_constructor_versioned(wl_proxy *proxy,
												 uint32_t opcode,
												 const wl_interface *interface,
												 uint32_t version,
												 ...)
{
	va_list args;

	va_start(args, version);

	auto result = proxy->marshalConstructor(opcode, interface, args);

	va_end(args);

	return result;
}

}

/*******************************************************************************
 * wl_display
 ******************************************************************************/

bool wl_display::sErrorMode = false;

void wl_display::reset()
{
	sErrorMode = false;
}

wl_display::wl_display()
{

}

wl_display::~wl_display()
{

}

wl_proxy* wl_display::marshalConstructor(uint32_t opcode,
										 const wl_interface *interface,
										 va_list args)
{
	if (sErrorMode)
	{
		errno = ENOENT;
		return nullptr;
	}

	switch(opcode)
	{
		case WL_DISPLAY_GET_REGISTRY:
			return new wl_registry(this);

		default:
			break;
	}

	return nullptr;
}

int wl_display::addListener(void (**implementation)(void), void *data)
{
	return -1;
}

void wl_display::addEvent(Event event)
{
	lock_guard<mutex> lock(mMutex);

	mEvents.push_back(event);
}

int wl_display::processEvents()
{
#if 0
	std::list<Event> localEvents;

	{
		lock_guard<mutex> lock(mMutex);

		localEvents = mEvents;

		mEvents.clear();
	}

	while(!localEvents.empty())
	{

		localEvents.front()();
		localEvents.pop_front();
	}

	return 0;
#endif

	lock_guard<mutex> lock(mMutex);

	while(!mEvents.empty())
	{

		auto event = mEvents.front();

		mMutex.unlock();

		event();

		mMutex.lock();

		mEvents.pop_front();

	}

	return 0;
}

/*******************************************************************************
 * wl_registry
 ******************************************************************************/

#ifdef WITH_IVI_EXTENSION
bool wl_registry::sEnableIviApplication = false;
#endif

#ifdef WITH_ZCOPY
bool wl_registry::sEnableDrm = false;
bool wl_registry::sEnableKms = false;
#endif

bool wl_registry::sEnableShell = false;
bool wl_registry::sEnableShmem = false;
bool wl_registry::sEnableSeat = false;

void wl_registry::reset()
{
#ifdef WITH_IVI_EXTENSION
	sEnableIviApplication = false;
#endif

#ifdef WITH_ZCOPY
	sEnableDrm = false;
	sEnableKms = false;
#endif

	sEnableShell = false;
	sEnableShmem = false;
	sEnableSeat = false;
}

wl_registry::wl_registry(wl_display* display) :
	mDisplay(display),
	mData(nullptr),
	mListener(nullptr)
{

}

wl_registry::~wl_registry()
{

}

wl_proxy* wl_registry::marshalConstructor(uint32_t opcode,
										 const wl_interface *interface,
										 va_list args)
{
	switch(opcode)
	{
		case WL_REGISTRY_BIND:

			if (strcmp(interface->name, "wl_compositor") == 0)
			{
				return new wl_compositor();
			}

			if (strcmp(interface->name, "wl_shell") == 0)
			{
				return new wl_shell();
			}

			if (strcmp(interface->name, "wl_shm") == 0)
			{
				return new wl_shm(mDisplay);
			}

			if (strcmp(interface->name, "wl_shm") == 0)
			{
				return new wl_shm(mDisplay);
			}

			if (strcmp(interface->name, "wl_seat") == 0)
			{
				return new wl_seat(mDisplay);
			}

#ifdef WITH_IVI_EXTENSION

			if (strcmp(interface->name, "ivi_application") == 0)
			{
				return new ivi_application();
			}

#endif

#ifdef WITH_ZCOPY

			if (strcmp(interface->name, "wl_drm") == 0)
			{
				return new wl_drm(mDisplay);
			}

			if (strcmp(interface->name, "wl_kms") == 0)
			{
				return new wl_kms(mDisplay);
			}

#endif

			return nullptr;

		default:
			break;
	}

	return nullptr;
}

int wl_registry::addListener(void (**implementation)(void), void *data)
{
	mData = data;
	mListener = reinterpret_cast<wl_registry_listener*>(implementation);

	if (mListener)
	{
		mDisplay->addEvent(bind(mListener->global, mData, this, 0,
								wl_compositor_interface.name, 1));

		if (sEnableShell)
		{
			mDisplay->addEvent(bind(mListener->global, mData, this, 0,
									wl_shell_interface.name, 1));
		}

		if (sEnableShmem)
		{
			mDisplay->addEvent(bind(mListener->global, mData, this, 0,
									wl_shm_interface.name, 1));
		}

		if (sEnableSeat)
		{
			mDisplay->addEvent(bind(mListener->global, mData, this, 0,
									wl_seat_interface.name, 1));
		}

#ifdef WITH_IVI_EXTENSION

		if (sEnableIviApplication)
		{
			mDisplay->addEvent(bind(mListener->global, mData, this, 0,
									ivi_application_interface.name, 1));
		}

#endif

#ifdef WITH_ZCOPY

		if (sEnableDrm)
		{
			mDisplay->addEvent(bind(mListener->global, mData, this, 0,
									wl_drm_interface.name, 1));
		}

		if (sEnableKms)
		{
			mDisplay->addEvent(bind(mListener->global, mData, this, 0,
									wl_kms_interface.name, 1));
		}

#endif
	}

	return 0;
}

/*******************************************************************************
 * wl_shm
 ******************************************************************************/

wl_shm::wl_shm(wl_display* display) :
	mDisplay(display),
	mData(nullptr),
	mListener(nullptr)
{

}

wl_shm::~wl_shm()
{

}

wl_proxy* wl_shm::marshalConstructor(uint32_t opcode,
									 const wl_interface *interface,
									 va_list args)
{
	switch(opcode)
	{
		case WL_SHM_CREATE_POOL:
			return new wl_shm_pool(mDisplay);

		default:
			break;
	}

	return nullptr;
}

int wl_shm::addListener(void (**implementation)(void), void *data)
{
	mData = data;
	mListener = reinterpret_cast<wl_shm_listener*>(implementation);

	if (mListener)
	{
		mDisplay->addEvent(bind(mListener->format, mData, this, 0));
		mDisplay->addEvent(bind(mListener->format, mData, this, 1));

		return 0;
	}

	return -1;
}

/*******************************************************************************
 * wl_shm_pool
 ******************************************************************************/

wl_shm_pool::wl_shm_pool(wl_display* display) :
	mDisplay(display)
{

}

wl_shm_pool::~wl_shm_pool()
{

}

wl_proxy* wl_shm_pool::marshalConstructor(uint32_t opcode,
										  const wl_interface *interface,
										  va_list args)
{
	switch(opcode)
	{
		case WL_SHM_POOL_CREATE_BUFFER:
			return new wl_buffer(mDisplay);

		default:
			break;
	}

	return nullptr;
}

/*******************************************************************************
 * wl_buffer
 ******************************************************************************/

wl_buffer::wl_buffer(wl_display* display) :
	mDisplay(display),
	mData(nullptr),
	mListener(nullptr)
{

}

wl_buffer::~wl_buffer()
{

}

int wl_buffer::addListener(void (**implementation)(void), void *data)
{
	mData = data;
	mListener = reinterpret_cast<wl_buffer_listener*>(implementation);

	return 0;
}

/*******************************************************************************
 * wl_seat
 ******************************************************************************/

uint32_t wl_seat::sCapabilities = 0;

void wl_seat::reset()
{
	sCapabilities = 0;
}

wl_seat::wl_seat(wl_display* display) :
	mDisplay(display),
	mData(nullptr),
	mListener(nullptr)
{

}

wl_seat::~wl_seat()
{

}

wl_proxy* wl_seat::marshalConstructor(uint32_t opcode,
									   const wl_interface *interface,
									   va_list args)
{
	switch(opcode)
	{
		case WL_SEAT_GET_POINTER:
			return new wl_buffer(mDisplay);

		case WL_SEAT_GET_KEYBOARD:
			return new wl_buffer(mDisplay);

		case WL_SEAT_GET_TOUCH:
			return new wl_buffer(mDisplay);

		default:
			break;
	}

	return nullptr;
}

int wl_seat::addListener(void (**implementation)(void), void *data)
{
	mData = data;
	mListener = reinterpret_cast<wl_seat_listener*>(implementation);

	if (mListener)
	{
		if (sCapabilities)
		{
			mDisplay->addEvent(bind(mListener->capabilities, mData, this, sCapabilities));
		}

		return 0;
	}

	return -1;
}

#ifdef WITH_ZCOPY

/*******************************************************************************
 * wl_drm
 ******************************************************************************/

wl_drm::wl_drm(wl_display* display) :
	mDisplay(display),
	mData(nullptr),
	mListener(nullptr)
{

}

wl_drm::~wl_drm()
{

}

wl_proxy* wl_drm::marshalConstructor(uint32_t opcode,
									 const wl_interface *interface,
									 va_list args)
{
	switch(opcode)
	{
		case WL_DRM_CREATE_BUFFER:
			return new wl_buffer(mDisplay);

		default:
			break;
	}

	return nullptr;
}

int wl_drm::addListener(void (**implementation)(void), void *data)
{
	mData = data;
	mListener = reinterpret_cast<wl_drm_listener*>(implementation);

	if (mListener)
	{
		mDisplay->addEvent(bind(mListener->device, mData, this, "/dev/dri/card0"));
		mDisplay->addEvent(bind(mListener->authenticated, mData, this));

		mDisplay->addEvent(bind(mListener->format, mData, this, 0));
		mDisplay->addEvent(bind(mListener->format, mData, this, 1));

		return 0;
	}

	return -1;
}

/*******************************************************************************
 * wl_kms
 ******************************************************************************/

wl_kms::wl_kms(wl_display* display) :
	mDisplay(display),
	mData(nullptr),
	mListener(nullptr)
{

}

wl_kms::~wl_kms()
{

}

wl_proxy* wl_kms::marshalConstructor(uint32_t opcode,
									 const wl_interface *interface,
									 va_list args)
{
	switch(opcode)
	{
		case WL_DRM_CREATE_BUFFER:
			return new wl_buffer(mDisplay);

		default:
			break;
	}

	return nullptr;
}

int wl_kms::addListener(void (**implementation)(void), void *data)
{
	mData = data;
	mListener = reinterpret_cast<wl_kms_listener*>(implementation);

	if (mListener)
	{
		mDisplay->addEvent(bind(mListener->device, mData, this, "/dev/dri/card0"));
		mDisplay->addEvent(bind(mListener->authenticated, mData, this));

		mDisplay->addEvent(bind(mListener->format, mData, this, 0));
		mDisplay->addEvent(bind(mListener->format, mData, this, 1));

		return 0;
	}

	return -1;
}

#endif
