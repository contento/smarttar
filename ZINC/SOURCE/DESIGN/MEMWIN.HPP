//	MEMWIN.HPP (MEMORY_WINDOW) - Example of a derived device.
//	COPYRIGHT (C) 1990-1992.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

const int E_MEMORY_WINDOW = 10000;
const EVENT_TYPE SAVE_FREE_MEMORY = 10001;

class MEMORY_WINDOW : public UI_DEVICE, public UIW_WINDOW
{
public:
	UI_DEVICE *device;
	UIW_WINDOW *window;

	MEMORY_WINDOW(int left, int top);
	~MEMORY_WINDOW(void) {}

	static EVENT_TYPE Save(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);

private:
	UI_BIGNUM farCoreMemory;
	UI_BIGNUM freeMemory;
	UIW_BIGNUM *farCoreLeftField;
	UIW_BIGNUM *freeMemoryField;
	UIW_BIGNUM *saveField;

	EVENT_TYPE Event(const UI_EVENT &event);
	void Poll(void);
};
