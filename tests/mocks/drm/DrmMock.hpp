#ifndef TESTS_MOCKS_DRM_DRMMOCK_HPP_
#define TESTS_MOCKS_DRM_DRMMOCK_HPP_

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

#include <cstdlib>

#include <xf86drm.h>
#include <xf86drmMode.h>

#ifdef WITH_ZCOPY
#include <drm/xen_zcopy_drm.h>
#endif

#include "Pipe.hpp"

class DumbMock
{
public:
	DumbMock(uint64_t size);
	~DumbMock();

	void* getBuffer() const { return mBuffer; }
	uint32_t getName() const { return mName; }

	void map(uint64_t size);
	void unmap(void* buffer, uint64_t size);

	void setPrimeFd(int fd) { mPrimeFd = fd; }
	int getPrimeFd() { return mPrimeFd; }

private:
	uint32_t mName;
	void* mBuffer;
	uint64_t mDumbSize;
	uint64_t mMapSize;
	int mPrimeFd;

};

class DrmMock
{
public:
	DrmMock(const std::string& name);
	virtual ~DrmMock();

	std::string getName() const { return mName; }
	int getFd() const { return mPipe.getFd(); }
	drm_magic_t getMagic() { mMagic = rand();  return mMagic; }

	drmModeResPtr getModeResources();
	drmModeConnectorPtr getModeConnector(uint32_t connectorId);
	drmModeEncoderPtr getModeEncoder(uint32_t encoderId);
	drmModeCrtcPtr getModeCrtc(uint32_t crtcId);

	int pageFlip(uint32_t crtcId, uint32_t fbId, uint32_t flags,
				 void* userData);
	int handleEvent(drmEventContextPtr evctx);

	int ioCtl(unsigned long request, void* arg);

	void setEncoderCrtcId(uint32_t encoderId, uint32_t crtcId);
	void setConnectorEncoderId(uint32_t connectorId, uint32_t encoderId);
	void setConnected(uint32_t connectorId, bool isConnected);

	static void reset();

	static void setErrorMode(bool errorMode) { sErrorMode = true; }
	static bool getErrorMode() { return sErrorMode; }
	static void setDisableZCopy(bool disableZCopy) { sDisableZCopy = disableZCopy; }
	static bool getDisableZCopy() { return sDisableZCopy; }
	static std::shared_ptr<DrmMock> getDrmMock(int fd);

	static std::shared_ptr<DumbMock> getMapDumb();
	static std::shared_ptr<DumbMock> getDumbByBuffer(void* buffer);
	static std::shared_ptr<DumbMock> getDumbByHandle(uint32_t handle);
	static bool isDumbExist(uint32_t handle);

#ifdef WITH_ZCOPY
	static bool isPrimeFdExist(int fd);
	static int closePrimeFd(int fd);
#endif

protected:

	static int sDumbHandle;
	static std::mutex sMutex;
	static std::shared_ptr<DumbMock> sMapDumb;
	static std::unordered_map<uint32_t, std::shared_ptr<DumbMock>> sDumbs;

private:

	static bool sErrorMode;
	static bool sDisableZCopy;

#ifdef WITH_ZCOPY
	static std::list<int> sPrimeFds;
#endif

	std::string mName;
	drm_magic_t mMagic;
	Pipe mPipe;

	void* mUserData;

	std::vector<uint32_t> mConnectorIds;
	std::vector<drmModeConnector> mConnectors;

	std::vector<uint32_t> mEncoderIds;
	std::vector<drmModeEncoder> mEncoders;

	std::vector<uint32_t> mCrtcIds;
	std::vector<drmModeCrtc> mCrtcs;

	std::vector<drmModeModeInfo> mModes;

	int createDumb(drm_mode_create_dumb *req);
	int mapDumb(drm_mode_map_dumb* req);
	int destroyDumb(drm_mode_destroy_dumb* req);
	int getDumbName(drm_gem_flink* req);

#ifdef WITH_ZCOPY
	int createDumbFromRefs(drm_xen_zcopy_dumb_from_refs* req);
	int primeHandleToFd(drm_prime_handle* req);
	int primeFdToHandle(drm_prime_handle* req);
	int closeGem(drm_gem_close* req);
#endif
};

#endif /* TESTS_MOCKS_DRM_DRMMOCK_HPP_ */
