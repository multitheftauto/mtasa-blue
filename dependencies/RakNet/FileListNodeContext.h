#ifndef __FILE_LIST_NODE_CONTEXT_H
#define __FILE_LIST_NODE_CONTEXT_H

struct FileListNodeContext
{
	FileListNodeContext() {}
	FileListNodeContext(unsigned char o, unsigned int f) : op(o), fileId(f) {}
	~FileListNodeContext() {}

	unsigned char op;
	unsigned int fileId;
};

#endif
