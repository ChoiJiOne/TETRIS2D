#pragma once

#include "InputManager.h"
#include "IEntityUI.h"

class ButtonUI;
class Camera2D;
class PanelUI;
class TTFont;
class TextUI;

/** UI �Ŵ����� �̱����Դϴ�. */
class UIManager
{
public:
	DISALLOW_COPY_AND_ASSIGN(UIManager);

	static UIManager& GetRef();
	static UIManager* GetPtr();

	ButtonUI* CreateButtonUI(const std::string& path, const Mouse& mouse, TTFont* font, const std::function<void()>& clickEvent);
	PanelUI* CreatePanelUI(const std::string& path, TTFont* font);
	TextUI* CreateTextUI(const std::string& path, TTFont* font);

	void BatchRenderUIEntity(IEntityUI** entities, uint32_t count);
	
private:
	friend class IApp;

	UIManager() = default;
	virtual ~UIManager() {}

	void Startup(); /** IApp ���ο����� ȣ���ؾ� �մϴ�. */
	void Shutdown(); /** IApp ���ο����� ȣ���ؾ� �մϴ�. */

	void PassRoundRect(IEntityUI** entities, uint32_t count);
	void PassRoundRectWireframe(IEntityUI** entities, uint32_t count);
	void PassString(IEntityUI** entities, uint32_t count);

private:
	static UIManager instance_;

	class EntityManager* entityMgr_ = nullptr;
	class RenderManager2D* render2dMgr_ = nullptr;
	Camera2D* uiCamera_ = nullptr;
};