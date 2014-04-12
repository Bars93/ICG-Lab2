#ifndef GLCOMPUTING_H
#define GLCOMPUTING_H
#include <CL/cl.h>
// non derivable 
class clComputing
{
	cl_program program;
	void loadFromFile(const char *filename);
	
public:
	bool init();
	void coreLoad();
	clComputing(void);
	~clComputing(void);
};
#endif

