#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(ZIL_MSDOS)
#	include <alloc.h>
#elif defined (sun)
#	include <unistd.h>
#endif
#include <sys/stat.h>

#include "ui_gen.hpp"
#if !defined(__WATCOMC__)
typedef long time_t;
#endif
#define _STORE_HDR
#define MAKESTRLEN
#if !defined(Z_STORE)
#	define Z_STORE "z_store.cpp"
#endif
#include Z_STORE

int verbose = 0;
int fileInError = 0;
long inodeCount = 0;
long usedInodes;
int freeInodes;
int lostInodes = 0;
//int sind_inodes;
//int dind_inodes;
int usedData = 0;
int freeData = 0;
int lostData = 0;
//int sind_data;
//int dind_data;

#define EMPTY		-1
#define FREELIST	-2
#define INODES		-3

int inum;
int try_to_fix = 1;
FILE *f;
int *ptrbuf;
struct superBlock sb;
UI_STORAGE *src;
char reply[40];

int check_and_mark_block(int blknum)
{
	if (blknum == 0)
		return 0;
	if (blknum >= usedData)
	{
		switch (inum)
		{
		case EMPTY:
			printf("Too long %d in EMPTY???", blknum);
			break;
		case FREELIST:
			printf("Too long %d in freelist", blknum);
			break;
		case INODES:
			printf("Too long %d in inodes", blknum);
			break;
		default:
			printf("Too long %d in file %d", blknum, inum);
			break;
		}
		printf(".  Try to fix [Nn]? ");
		fileInError = 1;
		gets(reply);
		return reply[0] == 'Y' || reply[0] == 'y';
	}
	/* write error if the bit is already set */
	if (ptrbuf[blknum] != EMPTY)
	{
		printf("Cross linked block %d between ", blknum);
		switch (ptrbuf[blknum])
		{
		case FREELIST:
			printf("freelist");
			break;
		case INODES:
			printf("inodes");
			break;
		default:
			printf("%d", inum);
			break;
		}
		printf(" and ");
		switch (inum)
		{
		case EMPTY:
			printf("EMPTY???");
			break;
		case FREELIST:
			printf("freelist");
			break;
		case INODES:
			printf("inodes");
			break;
		default:
			printf("%d", inum);
			break;
		}
		printf(".  Try to fix [Nn]? ");
		fileInError = 1;
		gets(reply);
		return reply[0] == 'Y' || reply[0] == 'y';
	}
	else /* mark the entry for this block */
		ptrbuf[blknum] = inum;
	return 0;
}

int mark_indirect_blocks(int blkptr)
{
	int k, fix;
	diskAddr idb[DISK_ADDR_PER_DATA_BLOCK];

	if (blkptr == 0)
		return 0;
	if (check_and_mark_block(blkptr))
		return 1;
	fseek(f, (long)blkptr*BYTES_PER_BLOCK, SEEK_SET);
	fread((char *)&idb, sizeof(idb), 1, f);
	fix = 0;
	for (k=0; k < LENGTHOF(idb); k++)
	{
		swapDiskAddr(&idb[k]);
		if (check_and_mark_block(idb[k]))
		{
			idb[k] = 0;
			fix |= 1;
		}
		swapDiskAddr(&idb[k]);
	}
	if (fix)
	{
		fseek(f, (long)blkptr*BYTES_PER_BLOCK, SEEK_SET);
		fwrite((char *)&idb, sizeof(idb), 1, f);
	}
	return 0;
}

void mark_each_inode(int baseinum, int iblk_num)
{
	int j, k, fix;
	struct inode ib[INODES_PER_INODE_BLOCK];

	if (iblk_num == 0)
		return;
	/* read an inode block */
	inum = INODES;
	fix = 0;
	for (j=0; j < BYTES_PER_INODE_BLOCK/BYTES_PER_BLOCK; j++)
		check_and_mark_block(j+iblk_num);
	fseek(f, (long)iblk_num*BYTES_PER_BLOCK, SEEK_SET);
	fread((char *)&ib, sizeof(ib), 1, f);
	fix = 0;
	for (j=0; j < LENGTHOF(ib); j++)
	{
		swapInode(&ib[j]);
		inum = j + baseinum;
		if (ib[j].useCount > 0 && inodeCount < inum)
			inodeCount = inum;
		for (k=0; k < LENGTHOF(ib[j].direct); k++)
			if (check_and_mark_block(ib[j].direct[k]))
			{
				ib[j].direct[k] = 0;
				fix |= 1;
			}
		/* do the single indirect blocks */
		if (mark_indirect_blocks(ib[j].sIndirect))
		{
			ib[j].sIndirect = 0;
			fix |= 1;
		}
		/* do the double indirect blocks */
		if (ib[j].dIndirect != 0)
		{
			if (check_and_mark_block(ib[j].dIndirect))
			{
				ib[j].dIndirect = 0;
				fix |= 1;
			}
			else
			{
				int fix2;
				diskAddr idb[DISK_ADDR_PER_DATA_BLOCK];

				fseek(f, (long)ib[j].dIndirect*BYTES_PER_BLOCK, SEEK_SET);
				fread((char *)&idb, sizeof(idb), 1, f);
				fix2 = 0;
				for (k=0; k < LENGTHOF(idb); k++)
				{
					swapDiskAddr(&idb[k]);
					if (mark_indirect_blocks(idb[k]))
					{
						idb[k] = 0;
						fix2 |= 1;
					}
					swapDiskAddr(&idb[k]);
				}
				if (fix2)
				{
					fseek(f, (long)ib[j].dIndirect*BYTES_PER_BLOCK, SEEK_SET);
					fwrite((char *)&idb, sizeof(idb), 1, f);
				}
			}
		}
		swapInode(&ib[j]);
	}
	if (fix)
	{
		fseek(f, (long)iblk_num*BYTES_PER_BLOCK, SEEK_SET);
		fwrite((char *)&ib, sizeof(ib), 1, f);
	}
}

void mark_indirect_inodes(int baseinum, int blkptr)
{
	int i;
	diskAddr iib[DISK_ADDR_PER_INODE_BLOCK];

	if (blkptr == 0)
		return;
	check_and_mark_block(blkptr);
	fseek(f, (long)blkptr*BYTES_PER_BLOCK, SEEK_SET);
	fread((char *)&iib, sizeof(iib), 1, f);
	for (i=0; i < LENGTHOF(iib); i++)
	{
		swapDiskAddr(&iib[i]);
		mark_each_inode(baseinum+i*DISK_ADDR_PER_INODE_BLOCK, iib[i]);
	}
}

void mark_free_list(int blkptr)
{
	int i;
	struct freeListBlock flb;

	inum = FREELIST;
	while (blkptr != 0)
	{
		check_and_mark_block(blkptr);
		fseek(f, (long)blkptr*BYTES_PER_BLOCK, SEEK_SET);
		fread((char *)&flb, sizeof(flb), 1, f);
		for (i=0; i < LENGTHOF(flb.freeBlocks); i++)
		{
			swapDiskAddr(&flb.freeBlocks[i]);
			check_and_mark_block(flb.freeBlocks[i]);
		}
		blkptr = flb.next;
	}
}

void get_useCount(int baseinum, int iblk_num)
{
	int j;
	struct inode ib[INODES_PER_INODE_BLOCK];

	if (iblk_num == 0)
		return;
	/* read an inode block */
	inum = INODES;
	fseek(f, (long)iblk_num*BYTES_PER_BLOCK, SEEK_SET);
	fread((char *)&ib, sizeof(ib), 1, f);
	for (j=0; j < LENGTHOF(ib); j++)
	{
		swapInode(&ib[j]);
		inum = j + baseinum;
		if (inum < inodeCount)
			ptrbuf[inum] = ib[j].useCount & ~DIRECTORY_TAG;
	}
}

void get_indirect_useCount(int baseinum, int blkptr)
{
	int i;
	diskAddr iib[DISK_ADDR_PER_INODE_BLOCK];

	if (blkptr == 0)
		return;
	fseek(f, (long)blkptr*BYTES_PER_BLOCK, SEEK_SET);
	fread((char *)&iib, sizeof(iib), 1, f);
	for (i=0; i < LENGTHOF(iib); i++)
	{
		swapDiskAddr(&iib[i]);
		get_useCount(baseinum+i*DISK_ADDR_PER_INODE_BLOCK, iib[i]);
	}
}

class EXPORT UI_STORAGE_SUPPORT
{
public:
	static void WalkTree(void);
	static int CheckInodeFreeList(void);
};

void UI_STORAGE_SUPPORT::WalkTree(void)
{
	struct directoryEntry dentry;

	UI_STORAGE_OBJECT *o = new UI_STORAGE_OBJECT(*src, ".", 0, 0);
	if (o->objectError != 0)
	{
		delete o;
		return;
	}
	while (o->Load(&dentry) > 0)
	{
		if (strcmp(dentry.stringID, ".") != 0 && 
		    strcmp(dentry.stringID, "..") != 0 && 
		    src->ChDir(dentry.stringID) >= 0)
		{
			WalkTree();
			src->ChDir("..");
		}
		if (dentry.inum < inodeCount)
			ptrbuf[dentry.inum]--;
	}
	delete o;
}

int UI_STORAGE_SUPPORT::CheckInodeFreeList(void)
{
	int i = 0;
	inum_t k;
	setInumWithFragment(k, src->sb->freeInodeListLo,
			    src->sb->freeInodeListHi);
	while (k > 0)
	{
		if (k < inodeCount)
		{
			if (ptrbuf[k] != 0)
			{
				printf("Free inode %d is used %d times\n", k, ptrbuf[k]);
				fileInError = 1;
			}
			ptrbuf[k] = -1;
		}
		struct inode inode;
		if (src->rwInode(k, &inode, ZIL_IREAD) == 0)
		{
			i++;	// it's a valid inode!
			k = inode.fragmentIndex;
		}
		else
			k = 0;
	}
	return i;
}

void usage(void)
{
	printf("usage: chkfile [-v] <filename>\n");
	exit(4);
}

int main(int argc, char **argv)
{
	int i, fix;
	struct stat statb;
	diskAddr iib[DISK_ADDR_PER_INODE_BLOCK];

	if (argc == 3 && strcmp(argv[1], "-v") == 0)
	{
		verbose = 1;
		argv++;
	}
	else if (argc != 2)
		usage();

	/*
	 * Check sizes of important structures.  Each should be
	 * a multiple of BYTES_PER_BLOCK.
	 */
	if (verbose)
	{
		printf("Superblock      %d\n", sizeof(sb));
		printf("Inode           %d\n", sizeof(struct inode));
		printf("Inode indirect  %d\n", sizeof(iib));
		printf("Free list       %d\n", sizeof(struct freeListBlock));
	
		inodeCount  = LENGTHOF(sb.inodeDirect);	// direct
		inodeCount += DISK_ADDR_PER_INODE_BLOCK;	// single indirect
		inodeCount += DISK_ADDR_PER_INODE_BLOCK * (long)DISK_ADDR_PER_INODE_BLOCK;
		inodeCount *= INODES_PER_INODE_BLOCK;
//		inodeCount = INODES_PER_INODE_BLOCK * (LENGTHOF(sb.inodeDirect) + DISK_ADDR_PER_INODE_BLOCK * (1 + (long)DISK_ADDR_PER_INODE_BLOCK));
		printf("Possible inodes %ld\n", inodeCount);
		inodeCount = 0;
	}

	/*
	 * Check the file for consistancy.  We do this by hand since
	 * we don't want the storage module bombing out if something
	 * isn't just right.  We also want to be able to "fix" things.
	 */
	if (try_to_fix)
		f = fopen(argv[1], "rb+");
	else
		f = fopen(argv[1], "rb");
	if (f == NULL)
		usage();
	fstat(fileno(f), &statb);
	/* allocate the buffer */
	usedData = (int)(statb.st_size / BYTES_PER_BLOCK);
	ptrbuf = new int[usedData];
	for (i=0; i < usedData; i++)
		ptrbuf[i] = EMPTY;
	/* mark the 0 bit */
	ptrbuf[0] = inum;
	fseek(f, 0L, SEEK_SET);
	fread((char *)&sb, sizeof(sb), 1, f);
	swapSuperBlock(&sb);
	if (sb.signature.magicNumber != MAGIC_NUMBER)
	{
		printf("Unknown file (marker 0x%x != 0x%x)\n",
		       sb.signature.magicNumber, MAGIC_NUMBER);
		return 2;
	}
	if (sb.signature.majorVersion == 3 && sb.signature.minorVersion == 0)
	{
		sb.freeInodeListHi = 0;
		sb.signature.majorVersion = MAJOR_VERSION;
		sb.signature.minorVersion = MINOR_VERSION;
	}
	if (sb.signature.majorVersion < MAJOR_VERSION)
	{
		printf("Wrong version (%d.%d < %d.%d) of storage\n",
		       sb.signature.majorVersion, sb.signature.minorVersion,
		       MAJOR_VERSION, MINOR_VERSION);
		return 3;
	}

	for (i=0; i < LENGTHOF(sb.inodeDirect); i++)
		mark_each_inode(i*INODES_PER_INODE_BLOCK, sb.inodeDirect[i]);
	/* do the single indirect inode blocks */
	mark_indirect_inodes(LENGTHOF(sb.inodeDirect)*INODES_PER_INODE_BLOCK,
			     sb.inodeSIndirect);
	/* do the double indirect inode blocks */
	if (sb.inodeDIndirect != 0)
	{
		fix = 0;
		if (check_and_mark_block(sb.inodeDIndirect))
		{
			sb.inodeDIndirect = 0;
			fix |= 1;
		}
		else
		{
			fseek(f, (long)sb.inodeDIndirect*BYTES_PER_BLOCK, SEEK_SET);
			fread((char *)&iib, sizeof(iib), 1, f);
			for (i=0; i < LENGTHOF(iib); i++)
			{
				swapDiskAddr(&iib[i]);
				mark_indirect_inodes(LENGTHOF(sb.inodeDirect)*INODES_PER_INODE_BLOCK+
						     (i+1)*DISK_ADDR_PER_INODE_BLOCK,
						     iib[i]);
			}
		}
		if (fix)
		{
			fseek(f, 0L, SEEK_SET);
			swapSuperBlock(&sb);
			fwrite((char *)&sb, sizeof(sb), 1, f);
			swapSuperBlock(&sb);
		}
	}
	for (i=0; i < usedData; i++)
		if (ptrbuf[i] == EMPTY)
			freeData++;
	mark_free_list(sb.freeDataList);
	/* write out any bits not set */
	for (i=0; i < usedData; i++)
		if (ptrbuf[i] == EMPTY)
		{
			printf("Missing block %d\n", i);
			fileInError = 1;
			lostData++;
		}
	if (verbose)
		printf("%d blocks free, %d blocks missing\n", freeData, lostData);
	inodeCount++;
#if defined(ZIL_DELETE)
	delete []ptrbuf;
#else
	delete [usedData]ptrbuf;
#endif
	/*
	 * Read all the inodes, saving the useCount into an array.  Check the
	 * inode free list against the 0s in the useCount array.  Walk down
	 * the directory tree, decrement each inum's useCount.  Report all
	 * negative useCounts (used too many times) and all positive
	 * useCounts (not used enough).  Directory entries for negative
	 * counts should be deleted.  Inode useCounts for positive counts
	 * should decremented by the leftover count.  If that useCount goes
	 * to 0 then we should free up the data.
	 */
	if (inodeCount * sizeof(int) > 65500L)
	{
		printf("\nSorry, the directory structure can't be checked.\n");
		printf("Do you want to continue the garbage collect? ");
		gets(reply);
		if (reply[0] != 'Y' && reply[0] != 'y')
			return 1;
	}
	else
	{
		// Get the useCounts
		ptrbuf = new int[(int)inodeCount];
		for (i=0; i < LENGTHOF(sb.inodeDirect); i++)
		{
			get_useCount(i*INODES_PER_INODE_BLOCK, sb.inodeDirect[i]);
			get_indirect_useCount(LENGTHOF(sb.inodeDirect)*INODES_PER_INODE_BLOCK,
					       sb.inodeSIndirect);
			if (sb.inodeDIndirect != 0)
			{
				fseek(f, (long)sb.inodeDIndirect*BYTES_PER_BLOCK, SEEK_SET);
				fread((char *)&iib, sizeof(iib), 1, f);
				for (i=0; i < LENGTHOF(iib); i++)
				{
					swapDiskAddr(&iib[i]);
					get_indirect_useCount(LENGTHOF(sb.inodeDirect)*INODES_PER_INODE_BLOCK+
							       (i+1)*DISK_ADDR_PER_INODE_BLOCK,
							       iib[i]);
				}
			}
		}

		// Check the free list
		int j = 0;
		for (i=0; i < inodeCount; i++)
			if (ptrbuf[i] == 0) j++;
		usedInodes = inodeCount-j;
		src = new UI_STORAGE(argv[1], UIS_READ);
		freeInodes = UI_STORAGE_SUPPORT::CheckInodeFreeList();
		delete src;
		for (i=0; i < inodeCount; i++)
		{
			if (ptrbuf[i] == 0)
			{
				printf("Lost inode %d\n", i);
				fileInError = 1;
				lostInodes++;
			}
			if (ptrbuf[i] == -1) ptrbuf[i] = 0;
		}
		if (verbose)
		{
			printf("Inodes used %ld", usedInodes);
			printf(", free tweener inodes %d\n", j);
			printf("Inodes on freelist %d", freeInodes);
			printf(", inodes lost %d\n", lostInodes);
		}

		// Check the directory entries
		src = new UI_STORAGE(argv[1], UIS_READ);
		UI_STORAGE_SUPPORT::WalkTree();
		for (i=0; i < inodeCount; i++)
			if (ptrbuf[i] < 0)
			{
				printf("Inode %d is overused %d times\n", i, -ptrbuf[i]);
				fileInError = 1;
			}
			else if (ptrbuf[i] > 0)
			{
				printf("Inode %d is underused %d times\n", i, ptrbuf[i]);
				fileInError = 1;
			}
		delete src;
#if defined(ZIL_DELETE)
		delete []ptrbuf;
#else
		delete [(int)inodeCount]ptrbuf;
#endif
	}
	usedData -= (usedInodes + freeInodes + lostInodes) *
		sizeof(struct inode) / BYTES_PER_BLOCK +
		freeData;
	/*
	 * Now we are done with the consistancy checks.  If there were no
	 * errors we can garbage collect.  First, find the length of the
	 * final file.  Foreach block in the free list that resides before
	 * the end of the final file and before any data blocks; move the
	 * last used data block to that free block.  Pack any inode blocks
	 * in.  Truncate file at the final end point.
	 */
	if (fileInError)
		printf("WARNING: Your file failed the consistancy checks.%c%c\n", 7, 7);
	else
		printf("Your file passed the consistancy checks.\n");
	printf("Do you want to continue the garbage collect? [Nn]");
	gets(reply);
	if (reply[0] != 'Y' && reply[0] != 'y')
		return 1;

	// On to the garbage collection

	long totalsize =
		(usedInodes + freeInodes) * (long)sizeof(struct inode) +
			(usedData) * (long)BYTES_PER_DATA_BLOCK;
	printf("file should be %ld bytes long\n", totalsize);



	return 0;
}
