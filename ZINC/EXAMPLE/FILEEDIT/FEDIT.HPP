#ifndef FILEEDIT_HPP
#define FILEEDIT_HPP

enum SEEK_LOCATION
{
	SEEK_FIRST = 0,
	SEEK_CURRENT = 1,
	SEEK_LAST = 2
};

class FILE_EDITOR : public UIW_WINDOW
{
public:
	FILE_EDITOR(char *pathName = NULL);
	~FILE_EDITOR(void);
	EVENT_TYPE Event(const UI_EVENT &event);

private:
	FILE *stream;
	DIRECTORY_SERVICE *provider;
	char pathName[MAXPATHLEN];

	virtual int Close(void);
	virtual int Unlink(void);
	virtual int Open(const char *mode);
	virtual unsigned Read(void *buffer, unsigned size, unsigned count);
	virtual int Seek(int offset, SEEK_LOCATION location);
	virtual long Tell(void);
	virtual unsigned Write(void *buffer, unsigned size, unsigned count);
};

class FILE_STATISTICS : public UIW_WINDOW
{
public:
	FILE_STATISTICS(DEVICE_INFO &info);
	EVENT_TYPE Event(const UI_EVENT &event);
};

class FILE_ERROR : public UIW_WINDOW
{
public:
	FILE_ERROR(EVENT_TYPE errorType, const char *fileName);
};

#endif

