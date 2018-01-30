#ifndef TEST_MOCKS_DRM_DRMMOCK_HPP_
#define TEST_MOCKS_DRM_DRMMOCK_HPP_

#include <string>

#include <cstdlib>

#include <xf86drm.h>
#include <xf86drmMode.h>

class DrmMock
{
public:
	DrmMock(const std::string& name, int fd);
	virtual ~DrmMock();

	std::string getName() const { return mName; }
	int getFd() const { return mFd; }
	virtual drm_magic_t getMagic() { mMagic = rand();  return mMagic; }

	drmModeResPtr getModeResources();

private:
	std::string mName;
	int mFd;
	drm_magic_t mMagic;
};

class DrmZCopyMock : public DrmMock
{
public:
	using DrmMock::DrmMock;
};

#endif /* TEST_MOCKS_DRM_DRMMOCK_HPP_ */
