#include "vision_service.h"
#include "image_split.h"
#include "common.h"
#include "linux_list.h"
#include "time_util.h"
#include "vision_buff.h"
#include "glistWrapper.h"

#include "mpi_vo.h"
#include "hi_comm_video.h"


#define UPLOAD_FRAME_USE_THREAD (1)
#define PREALLOC_FRAME_NUM (16)


typedef struct {
    struct list_head list;
    int width;
    int height;
    uint64_t pts;
    uint64_t trigTime;
    void *leftImage;
    void *rightImage;
} stFrameElem;

typedef struct {
    struct list_head free;
    struct list_head available;
    struct list_head block; //a piece of memory
    int freeNum;
    int maxNum;
} stFramePool;

typedef struct {
    int running;
    pthread_t thdId;
    stFramePool pool;
    int channel;
    int isInit;

    /*for frame control*/
    uint64_t startTime;
    uint64_t endTime;
} stCalibrateTask;


static stCalibrateTask  calibTask = {0};


static void vo_show_image(VIDEO_FRAME_INFO_S vfi, stImage image, int chn)
{
    HI_S32 ret = HI_SUCCESS;
    vfi.stVFrame.u64PhyAddr[0] = image.u64Addr;
    vfi.stVFrame.u32Width = image.u32Width;
    vfi.stVFrame.u32Height = image.u32Height;
    vfi.stVFrame.u32Stride[0] = image.u32Stride;
    ret = HI_MPI_VO_SendFrame(0, chn, &vfi, 2000);
	ASSERT_EQUAL(ret, HI_SUCCESS);
}

static void* upload_vo_frame_proc(void* arg)
{
    VIDEO_FRAME_INFO_S *info = (VIDEO_FRAME_INFO_S *)arg;
    VIDEO_FRAME_S *vframe = &info->stVFrame;
    stImage image[3] = {0};

    image[0].u64Addr = vframe->u64PhyAddr[0];
    image[0].u32Width = vframe->u32Width;
    image[0].u32Height = vframe->u32Height;
    image[0].u32Stride = vframe->u32Stride[0];
    SplitMultipleImage(image, image+1,2,1,16);

    HI_U8 *LeftVirAddr = (HI_U8 * )HI_MPI_SYS_MmapCache(image[1].u64Addr, image[1].u32Stride * image[1].u32Height);
    HI_U8 *RightVirAddr = (HI_U8 * )HI_MPI_SYS_MmapCache(image[2].u64Addr, image[2].u32Stride * image[2].u32Height);

    vframe->u64PrivateData = get_time_us();
#if 1
    DualImageEncodeSend(LeftVirAddr,RightVirAddr,
				image[1].u32Width,image[1].u32Height,
				AUTEL_CALIB_FRAME_SRC,
				2,
				vframe->u64PrivateData,vframe->u64PTS);
#else
    vision_tcp_buf_insert_part(AUTEL_CALIB_GET_FRAMES,AUTEL_CALIB_FRAME_TYPE_RAW,AUTEL_CALIB_FRAME_SRC,
		1,
		vframe->u64PrivateData,vframe->u64PTS,
		LeftVirAddr,RightVirAddr,
		image[1].u32Width, image[1].u32Height, image[1].u32Stride);
#endif

#if 0
    char filename[128];
    sprintf(filename, "%s/left_%llu.png", "/tmp/SD0/image",vframe->u64PTS);
    SingleImageSave(filename,LeftVirAddr, image[1].u32Width, image[1].u32Height);
    //SingleImageEncodeSave(filename,LeftVirAddr, image[1].u32Width, image[1].u32Height);
#endif

   HI_MPI_SYS_Munmap(LeftVirAddr, image[1].u32Stride * image[1].u32Height);
   HI_MPI_SYS_Munmap(RightVirAddr, image[2].u32Stride * image[2].u32Height);

   return NULL;
}

static int copy_to_frame_elem(HI_U64 leftImg, HI_U64 rightImg, int size, stFrameElem *elem)
{
    HI_U8 *LeftVirAddr = (HI_U8 * )HI_MPI_SYS_MmapCache(leftImg, size);
    HI_U8 *RightVirAddr = (HI_U8 * )HI_MPI_SYS_MmapCache(rightImg, size);

    memcpy(elem->leftImage,LeftVirAddr,size);
    memcpy(elem->rightImage,RightVirAddr,size);

    HI_MPI_SYS_Munmap(LeftVirAddr, size);
    HI_MPI_SYS_Munmap(RightVirAddr, size);

    return 0;
}

static int expand_frame_pool(stFramePool *pool, int num, int imageSize)
{
    int i = 0;
    char *frame = NULL;
    stFrameElem *elem = NULL;
    struct list_head *block = NULL;
    
    frame = (char *)calloc(sizeof(struct list_head) + num*(imageSize+sizeof(stFrameElem)),sizeof(char));
    if (!frame) {
        return -1;
    }

    printf("<%s:%d> =========num:%d %p %x\n",__FUNCTION__,__LINE__,num,frame,sizeof(struct list_head) + num*(imageSize+sizeof(stFrameElem)));
    block = (struct list_head *)frame;
    list_add(block, &pool->block);
    frame += sizeof(struct list_head);

    elem = (stFrameElem *)(frame + num * imageSize);
    printf("<%s:%d> =========num:%d %p %p\n",__FUNCTION__,__LINE__,num,frame,elem);
    for (i=0; i<num; i++) {
        elem[i].leftImage = frame;
        elem[i].rightImage = frame + imageSize/2;
        list_add(&elem[i].list, &pool->free);
        frame += imageSize;
    }
    pool->freeNum += num;

    printf("<%s:%d> =========num:%d %p\n",__FUNCTION__,__LINE__,num,frame);

    return 0;
}

static void *upload_frame_task(void *arg)
{
    stCalibrateTask *calibTask = (stCalibrateTask *)arg;
    stFrameElem *elem = NULL;
    struct list_head *block = NULL;

    while (calibTask->running) {
        if (list_empty(&calibTask->pool.available)) {
            usleep(1000000);
            continue;
        }
        printf("<%s:%d> ==================\n",__FUNCTION__,__LINE__);
        elem = list_first_entry(&calibTask->pool.available, stFrameElem,list);
#if 0        
        printf("<%s:%d> ==================\n",__FUNCTION__,__LINE__);
        DualImageEncodeSend(elem->leftImage,elem->rightImage,
				elem->width,elem->height,
				AUTEL_CALIB_FRAME_SRC,
				calibTask->channel,
				elem->trigTime,elem->pts);
#else
    char filename[128];
    sprintf(filename, "%s/left_%llu.png", "/tmp/SD0/image",calibTask->pool.freeNum);
    SingleImageSave(filename,elem->leftImage, elem->width, elem->height);
    //SingleImageEncodeSave(filename,LeftVirAddr, image[1].u32Width, image[1].u32Height);
      
#endif
        printf("<%s:%d> =======[%d %d %x %x] chn[%d] freeNum[%d]\n",__FUNCTION__,__LINE__, \
            elem->width,elem->height,elem->trigTime,elem->pts,calibTask->channel,calibTask->pool.freeNum);
        list_move(&elem->list, &calibTask->pool.free);
        calibTask->pool.freeNum++;
        printf("<%s:%d> ==================\n",__FUNCTION__,__LINE__);
    }

    //free mem block
    while (!list_empty(&calibTask->pool.block)) {
        block = calibTask->pool.block.next;
        list_del(block);
        free(block);
    }
}

void send2pc(VIDEO_FRAME_INFO_S *info, calib_info_t* clbInfo, int dir)
{
    VIDEO_FRAME_S *vframe = &info->stVFrame;
    stImage image[3] = {0};

    image[0].u64Addr = vframe->u64PhyAddr[0];
    image[0].u32Width = vframe->u32Width;
    image[0].u32Height = vframe->u32Height;
    image[0].u32Stride = vframe->u32Stride[0];
    SplitMultipleImage(image, image+1,2,1,16);
    
    //vo_show_image(*info,image[1],0); //for test

    if (!(clbInfo->direction & dir)) { // dir send
        //printf("<%s:%d> #############dir[%d %d]###########\n",__FUNCTION__,__LINE__,dir,clbInfo->direction);
        return;
    }

    calibTask.startTime = get_time();
    if ((calibTask.startTime - calibTask.endTime) < (1000 / clbInfo->framerate[VISION_STREAM_REAR])) {
        //printf("<%s:%d> =======time [%d] \n",__FUNCTION__,__LINE__,calibTask.startTime - calibTask.endTime);
        return;
    }

#if UPLOAD_FRAME_USE_THREAD
    if (0 == calibTask.pool.freeNum) {
        expand_frame_pool(&calibTask.pool,PREALLOC_FRAME_NUM,vframe->u32Width*vframe->u32Height);
    }

    if (0 == calibTask.pool.freeNum) {
        printf("<%s:%d> =======\n",__FUNCTION__,__LINE__);
        return;
    }

    if (0 == calibTask.running) {
        //create thread
        calibTask.running = 1;
        VisionThreadCreate(&calibTask.thdId, 89, 0, "CALIBTASK", upload_frame_task, &calibTask);
    } else {
        printf("<%s:%d> ==================\n",__FUNCTION__,__LINE__);
        stFrameElem *elem = list_first_entry(&calibTask.pool.free, stFrameElem,list);
        printf("<%s:%d> ==================\n",__FUNCTION__,__LINE__);
        list_del(&elem->list);
        printf("<%s:%d> ==================\n",__FUNCTION__,__LINE__);
        calibTask.pool.freeNum--;
        printf("<%s:%d> ==================\n",__FUNCTION__,__LINE__);
        elem->pts = vframe->u64PTS;
        elem->trigTime = get_time();
        elem->height = image[1].u32Height;
        elem->width = image[1].u32Width;
        copy_to_frame_elem(image[1].u64Addr,image[2].u64Addr,image[1].u32Width*image[1].u32Height,elem);
        list_add_tail(&elem->list, &calibTask.pool.available);
        printf("<%s:%d> ==================\n",__FUNCTION__,__LINE__);
    }
#else
    printf("<%s:%d> ###########dir:%d %d##framerate:%d#interval:%d ms######\n",__FUNCTION__,__LINE__, \
        clbInfo->direction,dir,clbInfo->framerate[VISION_STREAM_FRONT],calibTask.startTime - calibTask.endTime);
    upload_vo_frame_proc(info);
#endif
    calibTask.endTime = calibTask.startTime;
}


void stop_calib_task()
{
    calibTask.running = 0;
}

int init_calib_task()
{
    if (0 == calibTask.isInit) {
        INIT_LIST_HEAD(&calibTask.pool.available);
        INIT_LIST_HEAD(&calibTask.pool.free);
        INIT_LIST_HEAD(&calibTask.pool.block);
        calibTask.pool.maxNum = PREALLOC_FRAME_NUM / 4;
        calibTask.isInit = 1;
        calibTask.channel = 2;
    }

    return 0;
}

