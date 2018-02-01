#ifndef TESTS_MOCKS_DRM_DRMMOCK_HPP_
#define TESTS_MOCKS_DRM_DRMMOCK_HPP_

#include <memory>
#include <string>
#include <vector>

#include <cstdlib>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "Pipe.hpp"

class DrmMock
{
public:
	DrmMock(const std::string& name);
	virtual ~DrmMock();

	std::string getName() const { return mName; }
	int getFd() const { return mPipe.getFd(); }
	virtual drm_magic_t getMagic() { mMagic = rand();  return mMagic; }

	virtual drmModeResPtr getModeResources();
	virtual drmModeConnectorPtr getModeConnector(uint32_t connectorId);
	virtual drmModeEncoderPtr getModeEncoder(uint32_t encoderId);
	virtual drmModeCrtcPtr getModeCrtc(uint32_t crtcId);

	int pageFlip(uint32_t crtcId, uint32_t fbId, uint32_t flags,
				 void* userData);
	int handleEvent(drmEventContextPtr evctx);

	virtual void setEncoderCrtcId(uint32_t encoderId, uint32_t crtcId);
	virtual void setConnectorEncoderId(uint32_t connectorId, uint32_t encoderId);
	virtual void setConnected(uint32_t connectorId, bool isConnected);


	static void reset();

	static void setErrorMode(bool errorMode) { sErrorMode = true; }
	static bool getErrorMode() { return sErrorMode; }
	static void setDisableZCopy(bool disableZCopy) { sDisableZCopy = disableZCopy; }
	static bool getDisableZCopy() { return sDisableZCopy; }
	static std::shared_ptr<DrmMock> getDrmMock(int fd);

private:

	static bool sErrorMode;
	static bool sDisableZCopy;

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
};

class DrmZCopyMock : public DrmMock
{
public:
	using DrmMock::DrmMock;

	drmModeResPtr getModeResources() override { return nullptr; }
};

#endif /* TESTS_MOCKS_DRM_DRMMOCK_HPP_ */
