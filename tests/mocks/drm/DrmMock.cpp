#include "DrmMock.hpp"

#include <algorithm>
#include <unordered_map>

#include <cerrno>
#include <fcntl.h>
#include <cstring>

#include <drm/xen_zcopy_drm.h>

#include "drm/Exception.hpp"

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
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

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
		if (DrmMock::getDisableZCopy())
		{
			errno = ENOENT;
			return -1;
		}

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
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

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
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

	*value = 1;

	return 0;
}

int drmHandleEvent(int fd, drmEventContextPtr evctx)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		errno = ENOENT;

		return -1;
	}

	return it->second->handleEvent(evctx);
}

int drmIoctl(int fd, unsigned long request, void *arg)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

	return 0;
}

int drmModeAddFB2(int fd, uint32_t width, uint32_t height,
				  uint32_t pixel_format, uint32_t bo_handles[4],
				  uint32_t pitches[4], uint32_t offsets[4],
				  uint32_t *buf_id, uint32_t flags)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

	return 0;
}

int drmModeRmFB(int fd, uint32_t bufferId)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

	return 0;
}

drmModeResPtr drmModeGetResources(int fd)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return nullptr;
	}

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
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return nullptr;
	}

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
}

drmModeEncoderPtr drmModeGetEncoder(int fd, uint32_t encoder_id)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return nullptr;
	}

	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		errno = ENOENT;

		return nullptr;
	}

	return it->second->getModeEncoder(encoder_id);
}

void drmModeFreeEncoder(drmModeEncoderPtr ptr)
{
}

drmModeCrtcPtr drmModeGetCrtc(int fd, uint32_t crtcId)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return nullptr;
	}

	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		errno = ENOENT;

		return nullptr;
	}

	return it->second->getModeCrtc(crtcId);
}

int drmModeSetCrtc(int fd, uint32_t crtcId, uint32_t bufferId,
				   uint32_t x, uint32_t y, uint32_t *connectors, int count,
				   drmModeModeInfoPtr mode)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

	return 0;
}

void drmModeFreeCrtc(drmModeCrtcPtr ptr)
{
}

int drmModePageFlip(int fd, uint32_t crtc_id, uint32_t fb_id,
					uint32_t flags, void *user_data)
{
	if (DrmMock::getErrorMode())
	{
		errno = EINVAL;
		return -1;
	}

	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		errno = ENOENT;

		return -1;
	}

	return it->second->pageFlip(crtc_id, fb_id, flags, user_data);
}

/*******************************************************************************
 * Static
 ******************************************************************************/

bool DrmMock::sErrorMode = false;
bool DrmMock::sDisableZCopy = false;

void DrmMock::reset()
{
	sErrorMode = false;
	sDisableZCopy = false;
}

shared_ptr<DrmMock> DrmMock::getDrmMock(int fd)
{
	auto it = gDrmMap.find(fd);

	if (it == gDrmMap.end())
	{
		throw Drm::Exception("Can't get DrmMock", ENOENT);
	}

	return it->second;
}

/*******************************************************************************
 * DrmMock
 ******************************************************************************/

DrmMock::DrmMock(const string& name) :
	mName(name),
	mMagic(0),
	mUserData(nullptr),
	mConnectorIds({1, 2}),
	mEncoderIds({1, 2}),
	mCrtcIds({1, 2})
{
	drmModeModeInfo mode {};

	mode.hdisplay = 800;
	mode.vdisplay = 600;
	strcpy(mode.name, "800x600");

	mModes.push_back(mode);

	mode.hdisplay = 1024;
	mode.vdisplay = 768;
	strcpy(mode.name, "1024x768");

	mode.hdisplay = 1920;
	mode.vdisplay = 1080;
	strcpy(mode.name, "1920x1080");

	mModes.push_back(mode);

	int count = 0;

	for(auto id : mCrtcIds)
	{
		drmModeCrtc crtc {};

		crtc.crtc_id = id;

		mCrtcs.push_back(crtc);

		count++;
	}

	count = 0;

	for(auto id : mEncoderIds)
	{
		drmModeEncoder encoder {};

		encoder.encoder_id = id;
		encoder.encoder_type = DRM_MODE_ENCODER_LVDS;
		encoder.crtc_id = mCrtcIds[count];
		encoder.possible_crtcs = 0x03;

		mEncoders.push_back(encoder);

		count++;
	}

	count = 0;

	for(auto id : mConnectorIds)
	{
		drmModeConnector connector {};

		connector.connector_id = id;
		connector.connector_type = DRM_MODE_CONNECTOR_HDMIA;

		connector.connector_type_id = count;
		connector.connection = DRM_MODE_CONNECTED;

		connector.encoder_id = mEncoderIds[count];

		connector.count_modes = mModes.size();
		connector.modes = mModes.data();

		connector.count_encoders = mEncoderIds.size();
		connector.encoders = mEncoderIds.data();

		mConnectors.push_back(connector);

		count++;
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

	res->count_encoders = mEncoderIds.size();
	res->encoders = mEncoderIds.data();

	res->count_crtcs = mCrtcIds.size();
	res->crtcs = mCrtcIds.data();

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

	return &(*it);
}

drmModeEncoderPtr DrmMock::getModeEncoder(uint32_t encoderId)
{
	auto it = find_if(mEncoders.begin(), mEncoders.end(),
					  [encoderId](const drmModeEncoder& encoder)
					  { return encoder.encoder_id == encoderId; });

	if (it == mEncoders.end())
	{
		return nullptr;
	}

	return &(*it);
}

drmModeCrtcPtr DrmMock::getModeCrtc(uint32_t crtcId)
{
	auto it = find_if(mCrtcs.begin(), mCrtcs.end(),
					  [crtcId](const drmModeCrtc& crtc)
					  { return crtc.crtc_id == crtcId; });

	if (it == mCrtcs.end())
	{
		return nullptr;
	}

	return &(*it);
}

int DrmMock::pageFlip(uint32_t crtcId, uint32_t fbId, uint32_t flags,
					  void* userData)
{
	mUserData = userData;

	mPipe.write();

	return 0;
}

int DrmMock::handleEvent(drmEventContextPtr evctx)
{
	static int sequence = 0;

	mPipe.read();

	evctx->page_flip_handler(mPipe.getFd(), sequence++, 0, 0, mUserData);

	return 0;
}

void DrmMock::setEncoderCrtcId(uint32_t encoderId, uint32_t crtcId)
{
	auto it = find_if(mEncoders.begin(), mEncoders.end(),
					  [encoderId](const drmModeEncoder& encoder)
					  { return encoder.encoder_id == encoderId; });

	if (it == mEncoders.end())
	{
		throw Drm::Exception("Invalid encoder id", EINVAL);
	}

	it->crtc_id = crtcId;
}

void DrmMock::setConnectorEncoderId(uint32_t connectorId, uint32_t encoderId)
{
	auto it = find_if(mConnectors.begin(), mConnectors.end(),
					  [connectorId](const drmModeConnector& connector)
					  { return connector.connector_id == connectorId; });

	if (it == mConnectors.end())
	{
		throw Drm::Exception("Invalid connector id", EINVAL);
	}

	it->encoder_id = encoderId;
}

void DrmMock::setConnected(uint32_t connectorId, bool isConnected)
{
	auto it = find_if(mConnectors.begin(), mConnectors.end(),
					  [connectorId](const drmModeConnector& connector)
					  { return connector.connector_id == connectorId; });

	if (it == mConnectors.end())
	{
		throw Drm::Exception("Invalid connector id", EINVAL);
	}

	it->connection = isConnected ? DRM_MODE_CONNECTED : DRM_MODE_DISCONNECTED;
}

