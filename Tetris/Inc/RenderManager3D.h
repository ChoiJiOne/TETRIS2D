#pragma once

#include "GameMath.h"
#include "IResource.h"
#include "RenderStateManager.h"

class Shader;

/** 3D ���� �Ŵ����� �̱����Դϴ�. */
class RenderManager3D
{
public:
	DISALLOW_COPY_AND_ASSIGN(RenderManager3D);

	static RenderManager3D& GetRef();
	static RenderManager3D* GetPtr();

private:
	friend class IApp;

	RenderManager3D() = default;
	virtual ~RenderManager3D() {}

	void Startup(); /** IApp ���ο����� ����ϴ� �޼����Դϴ�. */
	void Shutdown(); /** IApp ���ο����� ����ϴ� �޼����Դϴ�. */

private:
	static RenderManager3D instance_;

	RenderStateManager* renderStateMgr_ = nullptr;
};