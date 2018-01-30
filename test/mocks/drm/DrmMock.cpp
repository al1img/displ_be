#include "DrmMock.hpp"

#include <algorithm>
#include <memory>
#include <unordered_map>

#include <cerrno>
#include <fcntl.h>
#include <cstring>

#include "drm/xen_zcopy_drm.h"

using std::pair;
using std::string;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;

unordered_map<int, shared_ptr<DrmMock>> gDrmMap;

/*******************************************************************************
 * libdrm interface
 ******************************************************************************/
extern "C" {

int __real_open(const char *__file, int __oflag, ...);

int __wrap_open(const char *__file, int __oflag, ...)
{
	const char* devStr = "/dev/dri/";

	if (strncmp(__file, devStr, strlen(devStr)) == 0)
	{
		return drmOpen(__file, nullptr);
	}

	va_list args;

	va_start(args, __oflag);

	if (__oflag & (O_CREAT | O_TMPFILE))
	{
		auto mode = va_arg(args, mode_t);

		return __real_open(__file, __oflag, mode);
	}

	return __real_open(__file, __oflag);
}

}

int drmOpen(const char *name, const char *busid)
{
	static int fd = 1;

	auto it = find_if(gDrmMap.begin(), gDrmMap.end(),
					  [&name](const pair<int, shared_ptr<DrmMock>>& value)
					  { return name == value.second->getName(); });

	if (it != gDrmMap.end())
	{
		errno = EEXIST;

		return -1;
	}

	if (strcmp(name, XENDRM_ZCOPY_DRIVER_NAME) == 0)
	{
		gDrmMap[fd] = shared_ptr<DrmMock>(new DrmZCopyMock(name, fd));
	}
	else
	{
		gDrmMap[fd] = shared_ptr<DrmMock>(new DrmMock(name, fd));
	}

	return fd++;
}

int drmClose(int fd)
{
	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		errno = ENOENT;

		return -1;
	}

	gDrmMap.erase(it);

	return 0;
}

int drmGetMagic(int fd, drm_magic_t * magic)
{
	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		errno = ENOENT;

		return -1;
	}

	*magic = it->second->getMagic();

	return 0;
}

int drmGetCap(int fd, uint64_t capability, uint64_t *value)
{
	*value = 1;

	return 0;
}

int drmHandleEvent(int fd, drmEventContextPtr evctx)
{
	return 0;
}

int drmIoctl(int fd, unsigned long request, void *arg)
{
	return 0;
}

int drmModeAddFB2(int fd, uint32_t width, uint32_t height,
				  uint32_t pixel_format, uint32_t bo_handles[4],
				  uint32_t pitches[4], uint32_t offsets[4],
				  uint32_t *buf_id, uint32_t flags)
{
	return 0;
}

int drmModeRmFB(int fd, uint32_t bufferId)
{
	return 0;
}

drmModeResPtr drmModeGetResources(int fd)
{
	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		errno = ENOENT;

		return nullptr;
	}

	return it->second->getModeResources();
}

void drmModeFreeResources(drmModeResPtr ptr)
{
	free(ptr);
}

drmModeConnectorPtr drmModeGetConnector(int fd, uint32_t connectorId)
{
	return nullptr;
}

void drmModeFreeConnector(drmModeConnectorPtr ptr)
{

}

drmModeEncoderPtr drmModeGetEncoder(int fd, uint32_t encoder_id)
{
	return nullptr;
}

void drmModeFreeEncoder(drmModeEncoderPtr ptr)
{

}

drmModeCrtcPtr drmModeGetCrtc(int fd, uint32_t crtcId)
{
	return nullptr;
}

int drmModeSetCrtc(int fd, uint32_t crtcId, uint32_t bufferId,
				   uint32_t x, uint32_t y, uint32_t *connectors, int count,
				   drmModeModeInfoPtr mode)
{
	return 0;
}

void drmModeFreeCrtc(drmModeCrtcPtr ptr)
{

}

int drmModePageFlip(int fd, uint32_t crtc_id, uint32_t fb_id,
					uint32_t flags, void *user_data)
{
	return 0;
}

/*******************************************************************************
 * DrmMock
 ******************************************************************************/

DrmMock::DrmMock(const string& name, int fd) : mName(name), mFd(fd)
{
}

DrmMock::~DrmMock()
{

}

/*******************************************************************************
 * Public
 ******************************************************************************/

drmModeResPtr DrmMock::getModeResources()
{
	drmModeResPtr res = static_cast<drmModeResPtr>(malloc(sizeof(drmModeRes)));

	memset(res, 0, sizeof(drmModeRes));

	return res;
}
