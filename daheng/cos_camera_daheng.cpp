#include "cos_camera_daheng.h"
#include <iostream>


std::string CameraDaheng::GetErrorString(GX_STATUS emErrorStatus)
{
	std::string result;
    size_t size = 0;
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    
    // Get length of error description
    emStatus = GXGetLastError(&emErrorStatus, NULL, &size);
    if(emStatus != GX_STATUS_SUCCESS)
    	return std::string("Error when calling GXGetLastError.");

    result.resize(size);
    
    // Get error description
    emStatus = GXGetLastError(&emErrorStatus, &result[0], &size);
    if (emStatus != GX_STATUS_SUCCESS)
        return std::string("Error when calling GXGetLastError");

    return result;
}

bool CameraDaheng::CloseAndPrintLastError(GX_STATUS emErrorStatus, const int line_macros_val)
{
        std::cout << "Error in file " << __FILE__ << " (line " << line_macros_val << "): " << GetErrorString(emErrorStatus) << std::endl;
        Close();
        return false;

}


CameraDaheng::CameraDaheng()
	: hDevice(NULL), height(0), width(0), img_buf_size(0), dev_id_(0)
{}

CameraDaheng::CameraDaheng(const int dev_id, const int channel_num)
	: hDevice(NULL), height(0), width(0), img_buf_size(0), dev_id_(0)
{

	/*
	memset(&image, 0, sizeof(image));
	CHANNEL_NUM = channel_num;
	image.size = sizeof(XI_IMG);
	*/
	
	dev_id_ = dev_id;
	printf("USER_API: Starting config..\n");
	ConfigureCamera(dev_id_);
}

bool CameraDaheng::ConfigureCamera(const int dev_id){

	#ifndef CHEAT_JSON
	json cfg;
	std::ifstream i("camera_cfg.json");
	i >> cfg;
	conf.exposure = (int)cfg["exposure"];
	conf.gain = (int)cfg["gain"];
	conf.framerate = (int)cfg["framerate"];
	conf.width = (int)cfg["width"];
	conf.height = (int)cfg["height"];
	conf.offset_x = (int)cfg["offset_X"];
	conf.offset_y = (int)cfg["offset_Y"];
	conf.img_data_format = (int)cfg["rgb"];
	conf.auto_wb = (int)cfg["auto_wb"];
	#else
	conf.exposure = 4000;
	conf.gain = 13;
	conf.framerate = 100;
	conf.width = 1024;
	conf.height = 1024;
	conf.offset_x = 0;
	conf.offset_y = 0;
	conf.img_data_format = 0;
	conf.auto_wb = 1;
	#endif

	ShowConf();

	hDevice = NULL;              ///< Device handle
	size_t nSize = 0;
	//unsigned char* g_pMonoImageBuf = NULL;       ///< Memory for Mono image
	//int64_t g_nPayloadSize = 0;                  ///< Payload size
    //Initialize libary
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    emStatus = GXInitLib(); 
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    //Get device enumerated number
    uint32_t ui32DeviceNum = 0;
    emStatus = GXUpdateDeviceList(&ui32DeviceNum, 1000);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);
    //If no device found, app exit
    if(ui32DeviceNum <= 0)
    {
        printf("<No device found>\n");
        Close();
        return false;
    }

    //Open first device enumerated
    emStatus = GXOpenDeviceByIndex(1, &hDevice);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    //Get string length of Serial number
    emStatus = GXGetStringLength(hDevice, GX_STRING_DEVICE_SERIAL_NUMBER, &nSize);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);
    //Alloc memory for Serial number
    char *pszSerialNumber = new char[nSize];
    //Get Serial Number
    emStatus = GXGetString(hDevice, GX_STRING_DEVICE_SERIAL_NUMBER, pszSerialNumber, &nSize);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        delete[] pszSerialNumber;
        pszSerialNumber = NULL;
        return CloseAndPrintLastError(emStatus, __LINE__);           
    }

    printf("<Serial Number : %s>\n", pszSerialNumber);
    //Release memory for Serial number
    delete[] pszSerialNumber;
    pszSerialNumber = NULL;

    // Set width and height
    width = conf.width;
    height = conf.height;
	GX_INT_RANGE camera_width, camera_height; // allowed value from camera
	emStatus = GXGetIntRange(hDevice, GX_INT_WIDTH, &camera_width);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);
	emStatus = GXGetIntRange(hDevice, GX_INT_HEIGHT, &camera_height);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);
    if(width < camera_width.nMin || width > camera_width.nMax || (width - camera_width.nMin) % camera_width.nInc !=0 )
    {
    	printf("Incorrect value of width : %i. Permitted range : min=%i, max=%i, step=%i\n", width, camera_width.nMin, camera_width.nMax, camera_width.nInc);
    	Close();
    	return false;
    }
    if(height < camera_height.nMin || height > camera_height.nMax || (height - camera_height.nMin) % camera_height.nInc !=0 )
    {
    	printf("Incorrect value of height : %i. Permitted range : min=%i, max=%i, step=%i\n", height, camera_height.nMin, camera_height.nMax, camera_height.nInc);
    	Close();
    	return false;
    }

    emStatus = GXSetInt(hDevice, GX_INT_WIDTH, conf.width);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);
    emStatus = GXSetInt(hDevice, GX_INT_HEIGHT, conf.height);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    emStatus = GXGetInt(hDevice, GX_INT_PAYLOAD_SIZE, &img_buf_size);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);
    img_buf.resize(img_buf_size);
    printf("<Payload size : %i>\n", img_buf_size);


    // Set exposure
    emStatus = GXSetEnum(hDevice, GX_ENUM_EXPOSURE_MODE, GX_EXPOSURE_MODE_TIMED);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);
    emStatus = GXSetFloat(hDevice, GX_FLOAT_EXPOSURE_TIME, conf.exposure);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    // Set framerate
    emStatus = GXSetFloat(hDevice, GX_FLOAT_ACQUISITION_FRAME_RATE, conf.framerate);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    // Set gain
    emStatus = GXSetFloat(hDevice, GX_FLOAT_GAIN, conf.gain);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    //Set acquisition mode
    emStatus = GXSetEnum(hDevice, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    //Set trigger mode
    emStatus = GXSetEnum(hDevice, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    //Set buffer quantity of acquisition queue
    uint64_t nBufferNum = ACQ_BUFFER_NUM;
    emStatus = GXSetAcqusitionBufferNumber(hDevice, nBufferNum);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    bool bStreamTransferSize = false;
    emStatus = GXIsImplemented(hDevice, GX_DS_INT_STREAM_TRANSFER_SIZE, &bStreamTransferSize);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    if(bStreamTransferSize)
    {
        //Set size of data transfer block
        emStatus = GXSetInt(hDevice, GX_DS_INT_STREAM_TRANSFER_SIZE, ACQ_TRANSFER_SIZE);
	    if(emStatus != GX_STATUS_SUCCESS)
	    	return CloseAndPrintLastError(emStatus, __LINE__);
    }

    bool bStreamTransferNumberUrb = false;
    emStatus = GXIsImplemented(hDevice, GX_DS_INT_STREAM_TRANSFER_NUMBER_URB, &bStreamTransferNumberUrb);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    if(bStreamTransferNumberUrb)
    {
        //Set qty. of data transfer block
        emStatus = GXSetInt(hDevice, GX_DS_INT_STREAM_TRANSFER_NUMBER_URB, ACQ_TRANSFER_NUMBER_URB);
	    if(emStatus != GX_STATUS_SUCCESS)
	    	return CloseAndPrintLastError(emStatus, __LINE__);
    }

    return true;

}

void CameraDaheng::ShowConf(){
	printf("USER_API: config\n\tExposure:\t%d\n\tGain:\t\t%d\n\tFPS:\t\t%d\n\tWidth x Height:\t%dx%d\n\toffX,offY:\t%d, %d\n\tRGB:\t\t%d\n\tauto_wb:\t%d\n",\
		conf.exposure, conf.gain, conf.framerate, conf.width, conf.height, conf.offset_x, conf.offset_y, conf.img_data_format, conf.auto_wb);
}

bool CameraDaheng::Start(){
	printf("USER_API: Starting acquisition...\n");
    
    //Allocate the memory for pixel format transform 
		//g_pMonoImageBuf = new unsigned char[g_nPayloadSize];

    //Device start acquisition
    GX_STATUS emStatus = GXStreamOn(hDevice);
    if(emStatus != GX_STATUS_SUCCESS)
    	return CloseAndPrintLastError(emStatus, __LINE__);

    return true;
}

int CameraDaheng::GetImgSize()
{
	return img_buf_size;
}

bool CameraDaheng::GetFrame(uint8_t* pixels_corrected){
	/*
	xiGetImage(xiH, 5000, &image);
	//printf("xiGetImage OK!\n");
	uint8_t * pixels = (uint8_t*)image.bp;
	//printf("pixels array def OK! waiting for memcpy...\n");
	//printf("first pixel value is: ");
	//printf("%d\n", pixels[0]);
	memcpy(pixels_corrected, pixels, GetHeight() * GetWidth() * CHANNEL_NUM * sizeof(uint8_t));
	*/

    GX_STATUS emStatus = GX_STATUS_SUCCESS;
	PGX_FRAME_BUFFER pFrameBuffer = NULL;
    time_t lInit;
    time_t lEnd;
    uint32_t ui32FrameCount = 0;
    uint32_t ui32AcqFrameRate = 0;
    uint8_t *pointer_of_result_buffer=NULL;
    // Get a frame from Queue
    emStatus = GXDQBuf(hDevice, &pFrameBuffer, 1000);
    if(emStatus != GX_STATUS_SUCCESS)
    {

        //if (emStatus != GX_STATUS_TIMEOUT)
        std::cout << GetErrorString(emStatus) << std::endl;
    }
    if(pFrameBuffer->nStatus != GX_FRAME_STATUS_SUCCESS)
    {
        printf("<Abnormal Acquisition: Exception code: %d>\n", pFrameBuffer->nStatus);
        return false;
    }

    width = pFrameBuffer->nWidth;
    height = pFrameBuffer->nHeight;
    printf("<Successful acquisition: FrameCount: %u Width: %d Height: %d FrameID: %llu>\n", 
        ui32FrameCount, pFrameBuffer->nWidth, pFrameBuffer->nHeight, pFrameBuffer->nFrameID);
    ui32FrameCount = 0;

    VxInt32 emDXStatus = DX_OK;

    // Convert RAW8 or RAW16 image to RGB24 image
    switch (pFrameBuffer->nPixelFormat)
    {
        case GX_PIXEL_FORMAT_MONO8:
        	pointer_of_result_buffer = (uint8_t*)pFrameBuffer->pImgBuf;
            break;
        case GX_PIXEL_FORMAT_MONO10:
        case GX_PIXEL_FORMAT_MONO12:
            //Convert to the Raw8 image
            emDXStatus = DxRaw16toRaw8((unsigned char*)pFrameBuffer->pImgBuf, &img_buf[0], pFrameBuffer->nWidth, pFrameBuffer->nHeight, DX_BIT_2_9);
            if (emDXStatus != DX_OK)
            {
                printf("DxRaw16toRaw8 Failed, Error Code: %d\n", emDXStatus);
                return false;
            }
            printf("<Image was converted to RAW8>\n");
            pointer_of_result_buffer = &img_buf[0];
            break;
        default:
            printf("Error : PixelFormat of this camera is not supported\n");
            return false;
    }
 

    //(pFrameBuffer->nWidth, pFrameBuffer->nHeight);
	memcpy(pixels_corrected, pointer_of_result_buffer, img_buf_size);

    emStatus = GXQBuf(hDevice, pFrameBuffer);
    if(emStatus != GX_STATUS_SUCCESS)
    {
        GetErrorString(emStatus);
        return false;
    }

    return true;
}

bool CameraDaheng::Stop(){
	printf("USER_API: Stopping acquisition...\n");

    //Device stop acquisition
    GX_STATUS emStatus = GXStreamOff(hDevice);
    if(emStatus != GX_STATUS_SUCCESS)
        return CloseAndPrintLastError(emStatus, __LINE__);
    return true;
}

int CameraDaheng::GetHeight(){
	return height;
	//return conf.height;
}
int CameraDaheng::GetWidth(){
	return width;
	//return conf.width;
}
void CameraDaheng::Close(){
	if(hDevice)
    	GXCloseDevice(hDevice);
    hDevice = NULL;
    GXCloseLib();
	printf("USER_API: Deleting camera.. id: %d\n", dev_id_);
}

CameraDaheng::~CameraDaheng(){
	Close();
}
