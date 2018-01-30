#include "DrmMock.hpp"

#include <algorithm>
#include <memory>
#include <unordered_map>

#include <cerrno>
#include <fcntl.h>
#include <cstring>

#include "drm/xen_zcopy_drm.h"

using std::find_if;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::unordered_map;

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

void *__wrap_mmap(void *__addr, size_t __len, int __prot,
				  int __flags, int __fd, __off_t __offset)
{
	return 0;
}

}

int drmOpen(const char *name, const char *busid)
{
	auto it = find_if(gDrmMap.begin(), gDrmMap.end(),
					  [&name](const pair<int, shared_ptr<DrmMock>>& value)
					  { return name == value.second->getName(); });

	if (it != gDrmMap.end())
	{
		errno = EEXIST;

		return -1;
	}

	shared_ptr<DrmMock> drmMock;

	if (strcmp(name, XENDRM_ZCOPY_DRIVER_NAME) == 0)
	{
		drmMock.reset(new DrmZCopyMock(name));
	}
	else
	{
		drmMock.reset(new DrmMock(name));
	}

	gDrmMap[drmMock->getFd()] = drmMock;

	return drmMock->getFd();
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
	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		errno = ENOENT;

		return nullptr;
	}

	return it->second->getModeConnector(connectorId);
}

void drmModeFreeConnector(drmModeConnectorPtr ptr)
{
	free(ptr);
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

DrmMock::DrmMock(const string& name) :
	mName(name),
	mMagic(0),
	mConnectorIds({1, 2})
{
	int count = 0;

	for(auto id : mConnectorIds)
	{
		drmModeConnector connector {};

		connector.connector_id = id;
		connector.connector_type = DRM_MODE_CONNECTOR_HDMIA;
		connector.connector_type_id = count++;

		mConnectors.push_back(connector);
	}
}

DrmMock::~DrmMock()
{

}

/*******************************************************************************
 * Public
 ******************************************************************************/

drmModeResPtr DrmMock::getModeResources()
{
	auto res = static_cast<drmModeResPtr>(malloc(sizeof(drmModeRes)));

	memset(res, 0, sizeof(drmModeRes));

	res->count_connectors = mConnectorIds.size();
	res->connectors = mConnectorIds.data();

	return res;
}

drmModeConnectorPtr DrmMock::getModeConnector(uint32_t connectorId)
{
	auto it = find_if(mConnectors.begin(), mConnectors.end(),
					  [connectorId](const drmModeConnector& connector)
					  { return connector.connector_id == connectorId; });

	if (it == mConnectors.end())
	{
		return nullptr;
	}

	auto res = static_cast<drmModeConnectorPtr>(malloc(sizeof(drmModeConnector)));

	*res = *it;

	return res;
}
