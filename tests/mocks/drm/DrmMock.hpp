#ifndef TESTS_MOCKS_DRM_DRMMOCK_HPP_
#define TESTS_MOCKS_DRM_DRMMOCK_HPP_

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

	static void reset();

	static void setErrorMode(bool errorMode) { sErrorMode = true; }
	static bool getErrorMode() { return sErrorMode; }
	static void setDisableZCopy(bool disableZCopy) { sDisableZCopy = disableZCopy; }
	static bool getDisableZCopy() { return sDisableZCopy; }

private:

	static bool sErrorMode;
	static bool sDisableZCopy;

	std::string mName;
	drm_magic_t mMagic;
	Pipe mPipe;

	std::vector<uint32_t> mConnectorIds;
	std::vector<drmModeConnector> mConnectors;
};

class DrmZCopyMock : public DrmMock
{
public:
	using DrmMock::DrmMock;

	drmModeResPtr getModeResources() override { return nullptr; }
};

#endif /* TESTS_MOCKS_DRM_DRMMOCK_HPP_ */
