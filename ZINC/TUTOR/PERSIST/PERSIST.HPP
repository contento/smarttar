//	PERSIST.HPP (PERSIST) - Definition file for persistent graphic objects.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

const short ID_CIRCLE		= 0;
const short ID_RECTANGLE	= 1;
const short ID_TRIANGLE		= 2;

typedef unsigned short LOAD_FLAGS;
const LOAD_FLAGS L_NO_FLAGS 		= 0x0000;
const LOAD_FLAGS L_SKIP_TYPE 		= 0x0001;

class GRAPHIC_OBJECT
{
public:
	virtual void Draw(void) = 0;
	static GRAPHIC_OBJECT *New(FILE *file);
	virtual void Store(FILE *file)
		{ fwrite((char *)&type, sizeof(type), 1, file); }

protected:
	short type;

	GRAPHIC_OBJECT(short _type) : type(_type) { }
	GRAPHIC_OBJECT(FILE *file, LOAD_FLAGS flags = L_NO_FLAGS)
		{ if (!(flags & L_SKIP_TYPE)) fread((char *)&type, sizeof(type), 1, file); }

private:
	struct JUMP_ELEMENT
	{
		short type;
		GRAPHIC_OBJECT *(*newFunction)(FILE *file, LOAD_FLAGS flags);
	};

	static JUMP_ELEMENT _jumpTable[];
};

class CIRCLE : public GRAPHIC_OBJECT
{
public:
	CIRCLE(short _column, short _line, short _radius) :
		GRAPHIC_OBJECT(ID_CIRCLE), column(_column), line(_line),
		radius(_radius) {	}
	CIRCLE(FILE *file, LOAD_FLAGS flags = L_NO_FLAGS) :
		GRAPHIC_OBJECT(file, flags)
		{ fread((char *)&column, sizeof(column), 1, file);
		  fread((char *)&line, sizeof(line), 1, file);
		  fread((char *)&radius, sizeof(radius), 1, file); }

	virtual void Draw(void)
		{ DrawCircle(column, line, radius); }
	static GRAPHIC_OBJECT *New(FILE *file, LOAD_FLAGS flags = L_NO_FLAGS)
		{ return (new CIRCLE(file, flags)); }
	void Store(FILE *file)
		{ GRAPHIC_OBJECT::Store(file);
		  fwrite((char *)&column, sizeof(column), 1, file);
		  fwrite((char *)&line, sizeof(line), 1, file);
		  fwrite((char *)&radius, sizeof(radius), 1, file); }

private:
	short column, line, radius;
};

class RECTANGLE : public GRAPHIC_OBJECT
{
public:
	RECTANGLE(short _left, short _top, short _right, short _bottom) :
		GRAPHIC_OBJECT(ID_RECTANGLE), left(_left), top(_top),
		right(_right), bottom(_bottom) { }
	RECTANGLE(FILE *file, LOAD_FLAGS flags = L_NO_FLAGS) :
		GRAPHIC_OBJECT(file, flags)
		{ fread((char *)&left, sizeof(left), 1, file);
		  fread((char *)&top, sizeof(top), 1, file);
		  fread((char *)&right, sizeof(right), 1, file);
		  fread((char *)&bottom, sizeof(bottom), 1, file); }

	virtual void Draw(void)
		{ DrawRectangle(left, top, right, bottom); }
	static GRAPHIC_OBJECT *New(FILE *file, LOAD_FLAGS flags = L_NO_FLAGS)
		{ return (new RECTANGLE(file, flags)); }
	void Store(FILE *file)
		{ GRAPHIC_OBJECT::Store(file);
		  fwrite((char *)&left, sizeof(left), 1, file);
		  fwrite((char *)&top, sizeof(top), 1, file);
		  fwrite((char *)&right, sizeof(right), 1, file);
		  fwrite((char *)&bottom, sizeof(bottom), 1, file); }

private:
	short left, top, right, bottom;
};

class TRIANGLE : public GRAPHIC_OBJECT
{
public:
	TRIANGLE(short column1, short line1, short column2, short line2,
		short column3, short line3) :
		GRAPHIC_OBJECT(ID_TRIANGLE)
		{ triangle[0] = triangle[6] = column1,
		  triangle[1] = triangle[7] = line1,
		  triangle[2] = column2, triangle[3] = line2,
		  triangle[4] = column3, triangle[5] = line3; }
	TRIANGLE(FILE *file, LOAD_FLAGS flags = L_NO_FLAGS) :
		GRAPHIC_OBJECT(file, flags)
		{ fread((char *)triangle, sizeof(triangle), 1, file); }

	virtual void Draw(void)
		{ DrawTriangle(triangle); }
	static GRAPHIC_OBJECT *New(FILE *file, LOAD_FLAGS flags = L_NO_FLAGS)
		{ return (new TRIANGLE(file, flags)); }
	void Store(FILE *file)
		{ GRAPHIC_OBJECT::Store(file);
		  fwrite((char *)triangle, sizeof(triangle), 1, file); }

private:
	short triangle[8];
};
