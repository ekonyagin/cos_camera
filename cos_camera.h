#pragma once
					
#include <stdio.h>
#include <m3api/xiApi.h> // Linux, OSX
#include <memory.h>
#include <string.h>
#include <string>

#include <nlohmann/json.hpp>
#include <fstream> 

using json = nlohmann::json;

struct Config{
	int exposure;
	int gain;
	int framerate;
	int width;
	int height;
	int offset_x;
	int offset_y;
	int img_data_format;
	int auto_wb;
	int buffer_size;
	char SerialNr[16];
};

class Camera{
private:
	XI_IMG image;
	HANDLE xiH = NULL;
	int height, width;
	struct Config conf;
	int dev_id_;
	int CHANNEL_NUM;
	
public:
	Camera(const int dev_id, const int channel_num){
		memset(&image, 0, sizeof(image));
		CHANNEL_NUM = channel_num;
		image.size = sizeof(XI_IMG);
		dev_id_ = dev_id;
		printf("USER_API: Starting config..\n");
		ConfigureCamera(dev_id_);
	}
	//Camera(){}
	void ConfigureCamera(const int dev_id){
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

		ShowConf();

		xiOpenDevice(dev_id, &xiH);

		xiGetParamString(xiH, XI_PRM_DEVICE_SN, conf.SerialNr, sizeof(conf.SerialNr));
		printf("USER_API: Serial number of the camera is: %s\n",conf.SerialNr);

		xiSetParamInt(xiH, XI_PRM_EXPOSURE, conf.exposure);
		xiSetParamInt(xiH, XI_PRM_GAIN, conf.gain);
		
		if (conf.img_data_format == 1) {
			xiSetParamInt(xiH, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24);
			xiSetParamInt(xiH, XI_PRM_AUTO_WB, conf.auto_wb);
		} else
			xiSetParamInt(xiH, XI_PRM_IMAGE_DATA_FORMAT, XI_RAW8);

		int buffer_size;
		xiGetParamInt(xiH, XI_PRM_ACQ_TRANSPORT_BUFFER_SIZE XI_PRM_INFO_MAX, &buffer_size);
		xiSetParamInt(xiH, XI_PRM_ACQ_TRANSPORT_BUFFER_SIZE, buffer_size);
		
		xiSetParamInt(xiH, XI_PRM_WIDTH, conf.width);
		xiSetParamInt(xiH, XI_PRM_OFFSET_X, conf.offset_x);

		xiSetParamInt(xiH, XI_PRM_HEIGHT, conf.height);
		xiSetParamInt(xiH, XI_PRM_OFFSET_Y, conf.offset_y);

		xiSetParamInt(xiH,XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE_LIMIT);
		xiSetParamInt(xiH,XI_PRM_FRAMERATE,conf.framerate);
	}
	void ShowConf(){
		printf("USER_API: config\n\tExposure:\t%d\n\tGain:\t\t%d\n\tFPS:\t\t%d\n\tWidth x Height:\t%dx%d\n\toffX,offY:\t%d, %d\n\tRGB:\t\t%d\n\tauto_wb:\t%d\n",\
			conf.exposure, conf.gain, conf.framerate, conf.width, conf.height, conf.offset_x, conf.offset_y, conf.img_data_format, conf.auto_wb);
	}

	void Start(){
		printf("USER_API: Starting acquisition...\n");
		xiStartAcquisition(xiH);
	}
	void GetFrame(uint8_t* pixels_corrected){
		xiGetImage(xiH, 5000, &image);
		//printf("xiGetImage OK!\n");
		uint8_t * pixels = (uint8_t*)image.bp;
		//printf("pixels array def OK! waiting for memcpy...\n");
		//printf("first pixel value is: ");
		//printf("%d\n", pixels[0]);
		memcpy(pixels_corrected, pixels, GetHeight() * GetWidth() * CHANNEL_NUM * sizeof(uint8_t));
		//printf("memcpy OK!\n");
	}
	void Stop(){
		printf("USER_API: Stopping acquisition...\n");
		xiStopAcquisition(xiH);
	}

	int GetHeight(){ return conf.height;}
	int GetWidth(){ return conf.width;}
	~Camera(){
		xiCloseDevice(xiH);
		printf("USER_API: Deleting camera.. id: %d\n", dev_id_);
	}
};