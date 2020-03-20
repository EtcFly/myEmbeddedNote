SPIFFs移植和测试



##SPIFFS的移植
1、下载源码并加入到工程中
2、 定义spiffs_config spiffs_cfg 并提供read/write/erase的接口,同时配置falsh的一些参数,包括页大小等
3、 配置spiffs_config.h文件

步骤1：
	下载源码:

步骤2：移植结构和硬件参数配置
#define FLASH_SPIFFS_SECTOR_SIZE                (4096u)
#define FALSH_SPIFFS_TOTAL_SIZE                 (2048u)
#define FLASH_SPIFFS_BLOCK_SIZE                 (1u)
#define FLASH_SPIFFS_OFFEST                     (2048u)

const sfud_flash *spiffs_flash = 0;
/** 实现对flash的底层读操作
 *  add: 起始的读地址, 单位为Byte
 *  size: 读取的字节数
 *  dst:目的缓存存储地址
 *  return: SPIFFS_OK为成功  其他失败
**/
s32_t spiflash_spiffs_read(u32_t addr, u32_t size, u8_t *dst)
{
	if (0 == spiffs_flash) {
		spiffs_flash = sfud_get_device_table() + 0;
	} 
	
	if (0 == spiffs_flash)
	{
		return SPIFFS_ERR_INTERNAL;
	} else {
		if (sfud_read(spiffs_flash, addr, size, dst) == SFUD_SUCCESS) {
			return SPIFFS_OK;
		}
	}
	
	return SPIFFS_ERR_NOT_READABLE;
}

/** 实现对flash的底层写操作
 *  add: 起始的读地址, 单位为Byte
 *  size: 写入的字节数
 *  src: 需要写入的数据缓存地址
 *  return: SPIFFS_OK为成功  其他失败
**/
static s32_t spiflash_spiffs_write(u32_t addr, u32_t size, u8_t *src)
{
	if (0 == spiffs_flash) {
		spiffs_flash = sfud_get_device_table() + 0;
	} 
	
	if (0 == spiffs_flash)
	{
		return SPIFFS_ERR_INTERNAL;
	} else {
		if (sfud_write(spiffs_flash, addr, size, src) == SFUD_SUCCESS) {
			return SPIFFS_OK;
		}
	}
	
	return SPIFFS_ERR_NOT_WRITABLE;	
}

/** 实现对flash的底层擦除操作
 *  add: 起始的读地址, 单位为Byte
 *  size: 擦除的扇区数
 *  return: SPIFFS_OK为成功  其他失败
**/
static s32_t spiflash_spiffs_erase(u32_t addr, u32_t size)
{
	if (0 == spiffs_flash) {
		spiffs_flash = sfud_get_device_table() + 0;
	} 
	
	if (0 == spiffs_flash)
	{
		return SPIFFS_ERR_INTERNAL;
	} else {
		if (sfud_erase(spiffs_flash, addr, size) == SFUD_SUCCESS) {
			return SPIFFS_OK;
		}
	}
	
	return SPIFFS_ERR_ERASE_FAIL;	
}


spiffs_config spiffs_cfg=
{   
	.phys_size = FALSH_SPIFFS_TOTAL_SIZE*4096, /* 分配给文件系统的内存大小  单位Byte */
	.phys_addr = (4096+2048)*4096, /*  文件系统其实地址在当前flash的偏移地址 单位Byte */
	.phys_erase_block = 4096,      /*  物理擦除的块大小  单位Byte */
	/* 逻辑扇区，必须是物理扇区的整数倍
		最好是: log_block_size/log_page_size = （32-512）
	*/
	.log_block_size = 4096*4,  /* 逻辑块大小,适当加大可以加速读写 */
	.log_page_size = 256,      /* 写入的page大小 */

	.hal_read_f = spiflash_spiffs_read, /*  读写api 接口*/
	.hal_write_f = spiflash_spiffs_write,
	.hal_erase_f = spiflash_spiffs_erase,
};

3、spiffs_config.h的配置
   1> 定义类型
    typedef uint32_t u32_t;
	typedef uint16_t u16_t;
	typedef uint8_t  u8_t;
	typedef int32_t  s32_t;
	typedef int16_t  s16_t;
	typedef int8_t   s8_t;
	
  SPIFFS_OBJ_NAME_LEN：最大支持文件名的大小 单位字节
  SPIFFS_COPY_BUFFER_STACK： 拷贝缓存大小 
  SPIFFS_USE_MAGIC： 定义为1
  SPIFFS_USE_MAGIC_LENGTH： 魔术的大小 默认为1


##api及其参数说明

/**
 * 探测文件系统,返回文件系统的大小
 * @param config        配置接口
 * @return 实际文件系统的大小  否则返回<0的错误码
 */
s32_t SPIFFS_probe_fs(spiffs_config *config);

/**
 * 挂载文件系统 返回挂载的状态
 * @param fs            挂载的文件系统
 * @param config        配置接口
 * @param work          内存工作区缓存字节, 必须大于2*config->log_page_size
 * @param fd_space      文件描述缓存
 * @param fd_space_size 文件描述缓存大小
 * @param cache         memory for cache, may be null 大一点可以加速读写速度
 * @param cache_size    cache字段的大小
 * @param check_cb_f     callback function for reporting during consistency checks
 */
s32_t SPIFFS_mount(spiffs *fs, spiffs_config *config, u8_t *work,
    u8_t *fd_space, u32_t fd_space_size,
    void *cache, u32_t cache_size,
    spiffs_check_callback check_cb_f);

/**
 * 卸载文件系统
 * @param fs            the file system struct
 */
void SPIFFS_unmount(spiffs *fs);

/**
 * Creates a new file.
 * @param fs            the file system struct
 * @param path          the path of the new file("/"路径表示根目录下的"/"文件)
 * @param mode          ignored, for posix compliance(无效, 默认为0)
 * @return 返回创建状态
 */
s32_t SPIFFS_creat(spiffs *fs, const char *path, spiffs_mode mode);

/**
 * Opens/creates a file.
 * @param fs            the file system struct
 * @param path          the path of the new file
 * @param flags         the flags for the open command, can be combinations of
 *                      SPIFFS_O_APPEND, SPIFFS_O_TRUNC, SPIFFS_O_CREAT, SPIFFS_O_RDONLY,
 *                      SPIFFS_O_WRONLY, SPIFFS_O_RDWR, SPIFFS_O_DIRECT, SPIFFS_O_EXCL
 * @param mode          ignored, for posix compliance (无效, 默认为0)
 * @return 返回创建状态
 */
spiffs_file SPIFFS_open(spiffs *fs, const char *path, spiffs_flags flags, spiffs_mode mode);

/**
 * Opens a file by given dir entry.
 * Optimization purposes, when traversing a file system with SPIFFS_readdir
 * a normal SPIFFS_open would need to traverse the filesystem again to find
 * the file, whilst SPIFFS_open_by_dirent already knows where the file resides.
 * @param fs            the file system struct
 * @param e             the dir entry to the file
 * @param flags         the flags for the open command, can be combinations of
 *                      SPIFFS_APPEND, SPIFFS_TRUNC, SPIFFS_CREAT, SPIFFS_RD_ONLY,
 *                      SPIFFS_WR_ONLY, SPIFFS_RDWR, SPIFFS_DIRECT.
 *                      SPIFFS_CREAT will have no effect in this case.
 * @param mode          ignored, for posix compliance
 */
spiffs_file SPIFFS_open_by_dirent(spiffs *fs, struct spiffs_dirent *e, spiffs_flags flags, spiffs_mode mode);

/**
 * Opens a file by given page index.
 * Optimization purposes, opens a file by directly pointing to the page
 * index in the spi flash.
 * If the page index does not point to a file header SPIFFS_ERR_NOT_A_FILE
 * is returned.
 * @param fs            the file system struct
 * @param page_ix       the page index
 * @param flags         the flags for the open command, can be combinations of
 *                      SPIFFS_APPEND, SPIFFS_TRUNC, SPIFFS_CREAT, SPIFFS_RD_ONLY,
 *                      SPIFFS_WR_ONLY, SPIFFS_RDWR, SPIFFS_DIRECT.
 *                      SPIFFS_CREAT will have no effect in this case.
 * @param mode          ignored, for posix compliance
 */
spiffs_file SPIFFS_open_by_page(spiffs *fs, spiffs_page_ix page_ix, spiffs_flags flags, spiffs_mode mode);

/**
 * Reads from given filehandle.
 * @param fs            the file system struct
 * @param fh            the filehandle
 * @param buf           where to put read data
 * @param len           how much to read
 * @returns number of bytes read, or -1 if error
 */
s32_t SPIFFS_read(spiffs *fs, spiffs_file fh, void *buf, s32_t len);

/**
 * Writes to given filehandle.
 * @param fs            the file system struct
 * @param fh            the filehandle
 * @param buf           the data to write
 * @param len           how much to write
 * @returns number of bytes written, or -1 if error
 */
s32_t SPIFFS_write(spiffs *fs, spiffs_file fh, void *buf, s32_t len);

/**
 * Moves the read/write file offset. Resulting offset is returned or negative if error.
 * lseek(fs, fd, 0, SPIFFS_SEEK_CUR) will thus return current offset.
 * @param fs            the file system struct
 * @param fh            the filehandle
 * @param offs          how much/where to move the offset
 * @param whence        if SPIFFS_SEEK_SET, the file offset shall be set to offset bytes
 *                      if SPIFFS_SEEK_CUR, the file offset shall be set to its current location plus offset
 *                      if SPIFFS_SEEK_END, the file offset shall be set to the size of the file plus offse, which should be negative
 */
s32_t SPIFFS_lseek(spiffs *fs, spiffs_file fh, s32_t offs, int whence);

/**
 * Removes a file by path
 * @param fs            the file system struct
 * @param path          the path of the file to remove
 */
s32_t SPIFFS_remove(spiffs *fs, const char *path);

/**
 * Removes a file by filehandle
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to remove
 */
s32_t SPIFFS_fremove(spiffs *fs, spiffs_file fh);

/**
 * Gets file status by path
 * @param fs            the file system struct
 * @param path          the path of the file to stat
 * @param s             the stat struct to populate
 */
s32_t SPIFFS_stat(spiffs *fs, const char *path, spiffs_stat *s);

/**
 * Gets file status by filehandle
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to stat
 * @param s             the stat struct to populate
 */
s32_t SPIFFS_fstat(spiffs *fs, spiffs_file fh, spiffs_stat *s);

/**
 * Flushes all pending write operations from cache for given file
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to flush
 */
s32_t SPIFFS_fflush(spiffs *fs, spiffs_file fh); //将数据立即写入硬件

/**
 * Closes a filehandle. If there are pending write operations, these are finalized before closing.
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to close
 */
s32_t SPIFFS_close(spiffs *fs, spiffs_file fh);

/**
 * Renames a file
 * @param fs            the file system struct
 * @param old           path of file to rename
 * @param newPath       new path of file
 */
s32_t SPIFFS_rename(spiffs *fs, const char *old, const char *newPath);
/**
 * Returns last error of last file operation.
 * @param fs            the file system struct
 */
s32_t SPIFFS_errno(spiffs *fs);

/**
 * Clears last error.
 * @param fs            the file system struct
 */
void SPIFFS_clearerr(spiffs *fs);

/**
 * Opens a directory stream corresponding to the given name.
 * The stream is positioned at the first entry in the directory.
 * On hydrogen builds the name argument is ignored as hydrogen builds always correspond
 * to a flat file structure - no directories.
 * @param fs            the file system struct
 * @param name          the name of the directory
 * @param d             pointer the directory stream to be populated
 */
spiffs_DIR *SPIFFS_opendir(spiffs *fs, const char *name, spiffs_DIR *d);

/**
 * Closes a directory stream
 * @param d             the directory stream to close
 */
s32_t SPIFFS_closedir(spiffs_DIR *d);

/**
 * Reads a directory into given spifs_dirent struct.
 * @param d             pointer to the directory stream
 * @param e             the dirent struct to be populated
 * @returns null if error or end of stream, else given dirent is returned
 */
struct spiffs_dirent *SPIFFS_readdir(spiffs_DIR *d, struct spiffs_dirent *e);//可实现当前目录文件的遍历

/**
 * Runs a consistency check on given filesystem.(整个文件系统的完整性校验  非常耗时)
 * @param fs            the file system struct
 */
s32_t SPIFFS_check(spiffs *fs);
/**
 * Returns number of total bytes available and number of used bytes.
 * This is an estimation, and depends on if there a many files with little
 * data or few files with much data.
 * NB: If used number of bytes exceeds total bytes, a SPIFFS_check should
 * run. This indicates a power loss in midst of things. In worst case
 * (repeated powerlosses in mending or gc) you might have to delete some files.
 *
 * @param fs            the file system struct
 * @param total         total number of bytes in filesystem
 * @param used          used number of bytes in filesystem
 */
s32_t SPIFFS_info(spiffs *fs, u32_t *total, u32_t *used);

//格式化文件系统 返回格式化状态
s32_t SPIFFS_format(spiffs *fs);
//挂载文件系统 返回挂载状态
u8_t SPIFFS_mounted(spiffs *fs);
//文件系统碎片化快速整理合并 max_free_pages为一个块中最大空闲页的大小(小于该值将进行碎片合并) 单位为页
s32_t SPIFFS_gc_quick(spiffs *fs, u16_t max_free_pages);
//文件系统碎片化快速整理合并 size为最大碎片大小 单位为Byte
s32_t SPIFFS_gc(spiffs *fs, u32_t size);
//检测文件是否到达结尾 (内部原理就是查看文件偏移是否大于文件的大小)
s32_t SPIFFS_eof(spiffs *fs, spiffs_file fh);
//返回文件的偏移量
s32_t SPIFFS_tell(spiffs *fs, spiffs_file fh);
/*** 未知  **/
s32_t SPIFFS_set_file_callback_func(spiffs *fs, spiffs_file_callback cb_func); //未知
s32_t SPIFFS_ix_map(spiffs *fs, spiffs_file fh, spiffs_ix_map *map,
    u32_t offset, u32_t len, spiffs_page_ix *map_buf);
s32_t SPIFFS_ix_unmap(spiffs *fs, spiffs_file fh);
s32_t SPIFFS_ix_remap(spiffs *fs, spiffs_file fh, u32_t offs);
s32_t SPIFFS_bytes_to_ix_map_entries(spiffs *fs, u32_t bytes);
s32_t SPIFFS_ix_map_entries_to_bytes(spiffs *fs, u32_t map_page_ix_entries);


##读写性能测试
	主频72MHz SPI FLASH MX25L256  读写数据960k数据  每次读取256Byte   SPI主频40MHz
	--->  读: 328kByte/s  写: 75kByte/s
	
	
##example

#define LOG_PAGE_SIZE      256
	
static spiffs  fs;
spiffs_file    file_type;
spiffs_stat    stat;
spiffs_DIR     spiffs_dir;
struct spiffs_dirent  dirent;

static u8_t spiffs_work_buf[LOG_PAGE_SIZE*2];
static u8_t spiffs_fds[32*4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE+32)*4];
static uint8_t spiffs_writeBuff[512];
	
/* spiffs file system format */
void spiffs_sys_format(void)
{
	s32_t result;
	result = SPIFFS_probe_fs(&spiffs_cfg);
	if (result > 0){
		printf("spiffs file system is exist size:%d!\n", result);
	} else {
		printf("file system format!\n");
		/* format need to mount fs before, if mount fail, then called spiffs_format, 
		otherwise need called unmout and then called format function */
		result = SPIFFS_mount(&fs,
		  &spiffs_cfg,
		  spiffs_work_buf,
		  spiffs_fds,
		  sizeof(spiffs_fds),
		  spiffs_cache_buf,
		  sizeof(spiffs_cache_buf),
		  0);
		  
		if (result == SPIFFS_OK) {
			SPIFFS_unmount(&fs);
		}
		
		result = SPIFFS_format(&fs);
		if (result == SPIFFS_OK) {
			printf("spiffs format success\n");	
		} else printf("spiffs format fail!\n");
	}
}


void spiffs_readwirteSpeed_test(void)
{
	s32_t result;
	uint32_t size;
	uint32_t read_time, write_time;
	
	file_type = SPIFFS_open(&fs, "RDWR_test.txt", SPIFFS_O_RDWR | SPIFFS_O_CREAT, 0);
	if (file_type < 0) {
		printf("spiffs write/read speed test begin\n");
	}
	printf("/****************************write speed test ***************************************/");
	printf("write test start time:%d\n", write_time = HAL_GetTick());
	for (uint32_t i = 0; i < 256*15; i++)
	{
		size = SPIFFS_write(&fs, file_type, spiffs_writeBuff, 256);
		if (size != 256) {
			printf("write data fail! index:%d\n", i);
			break;
		}
	}
	printf("write test end time:%d\n", read_time = HAL_GetTick());
	result = SPIFFS_stat(&fs, "RDWR_test.txt", &stat);
	printf("get stat reasult:%d \n", result);
	
	printf("fileName:%s fileSize:%d writeSpeed:%d\n", stat.name, stat.size, stat.size*1000/(read_time-write_time));
	
	printf("/****************************read speed test ***************************************/");
	SPIFFS_lseek(&fs, file_type, 0, SPIFFS_SEEK_SET);	
	printf("write test start time:%d\n", write_time = HAL_GetTick());
	for (uint32_t i = 0; i < 256*15; i++)
	{
		size = SPIFFS_read(&fs, file_type, spiffs_writeBuff, 256);
		if (size != 256) {
			printf("read data fail index:%d size:%d!\n", i, size);
			break;
		}
	}
	printf("write test end time:%d\n", read_time = HAL_GetTick());
	result = SPIFFS_stat(&fs, "RDWR_test.txt", &stat);
	printf("get stat reasult:%d \n", result);
	
	printf("fileName:%s fileSize:%d writeSpeed:%d\n", stat.name, stat.size, stat.size*1000/(read_time-write_time));
	SPIFFS_close(&fs, file_type);
}

void spiffs_file_create_and_readWrite(void)
{
	s32_t result;
	s32_t read_size, write_size;
	char *s = "today is 2019/9/9 and tets spiffs file system by etcfly";
	
	result = SPIFFS_creat(&fs, "etcflt.csv", 0);
	result = SPIFFS_creat(&fs, "/update/data.cpp", 0);
	result = SPIFFS_creat(&fs, "easy.bat", 0);
	
	result = SPIFFS_creat(&fs, "hello_test.txt", 0);
	if (result == SPIFFS_ERR_FILE_EXISTS || result == SPIFFS_ERR_CONFLICTING_NAME) {
		printf("file is exist! \n");
	} else printf("create spiffs file %d\n", result);
	  
	file_type = SPIFFS_open(&fs, "hello.txt", SPIFFS_O_RDWR | SPIFFS_CREAT, 0);
	printf("file type:%d\n", file_type);
    write_size = SPIFFS_write(&fs, file_type, s, strlen((char *)s));
	printf("write size is %d\n", write_size);
	  
	printf("file offest:%d\n", SPIFFS_tell(&fs, file_type));
	SPIFFS_lseek(&fs, file_type, 0, SPIFFS_SEEK_SET);
	printf("file offest:%d\n", SPIFFS_tell(&fs, file_type));
	
	read_size = SPIFFS_read(&fs, file_type, spiffs_writeBuff, write_size);
	printf("read size is %d\n", read_size);
	printf("file:%s\n", spiffs_writeBuff);
	SPIFFS_close(&fs, file_type);
	
	result = SPIFFS_rename(&fs, "hello.txt", "/helloKity.txt");
	printf("spi rename result:%d\n", result);
	result = SPIFFS_remove(&fs, "/helloKity.txt");
	printf("spi remove result:%d\n", result);
}

void spiffs_list_file(const char *path)
{
	s32_t result;
	spiffs_DIR  *pdir;
	struct spiffs_dirent *pdirent;

	pdir = SPIFFS_opendir(&fs, path, &spiffs_dir);
	printf("pdir:%p dir:%p\n", pdir, &spiffs_dir);
	
	do {
		pdirent = SPIFFS_readdir(&spiffs_dir, &dirent);
		if (pdirent == 0) break;
		printf("fileName:%s size:%d type:%d id:%d\n", pdirent->name, pdirent->size, pdirent->type, pdirent->obj_id);
	} while (1);
	
	SPIFFS_closedir(&spiffs_dir);
	
//	result = SPIFFS_check(&fs);
//	printf("spiffs check result:%d\n", result);

}

void spiffs_test_used(void)
{
	s32_t result;
	uint32_t total, used;
	
	result = SPIFFS_info(&fs, &total, &used);
	printf("result:%d totalByte:%d usedByte:%d\n", result, total, used);
}

void spiffs_test(void)
{
	s32_t result;

	
	printf("spiffs_test begin!\n");
	spiffs_sys_format();
	
	
    result = SPIFFS_mount(&fs,
      &spiffs_cfg,
      spiffs_work_buf,
      spiffs_fds,
      sizeof(spiffs_fds),
      spiffs_cache_buf,
      sizeof(spiffs_cache_buf),
      0);
	  printf("spiffs mount :%d\n", result);

	 spiffs_test_used(); 
	 spiffs_list_file(0);
	 spiffs_file_create_and_readWrite();
	 spiffs_list_file(0);
//	 spiffs_readwirteSpeed_test(); //read and write speed test
	  
	 SPIFFS_unmount(&fs);
}

##资源消耗:
  典型值: FLASH: 23.5kbyte    RAM:2.5kbyte