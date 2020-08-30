#pragma once
			

#include "GxIAPI.h"
#include "DxImageProc.h"
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <time.h>
#include <vector>
#include <memory>


#include <stdio.h>
//#include <m3api/xiApi.h> // Linux, OSX
#include <memory.h>
#include <string.h>
#include <string>

#define ACQ_BUFFER_NUM          5               ///< Acquisition Buffer Qty.
#define ACQ_TRANSFER_SIZE       (64 * 1024)     ///< Size of data transfer block
#define ACQ_TRANSFER_NUMBER_URB 64              ///< Qty. of data transfer block

#ifndef CHEAT_JSON
	#include <nlohmann/json.hpp>
	#include <fstream>
	using json = nlohmann::json;
#endif

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

class CameraDaheng{
protected:
	//XI_IMG image;
	//HANDLE xiH = NULL;
	GX_DEV_HANDLE hDevice;
	int height, width;
	struct Config conf;
	int dev_id_;
	int CHANNEL_NUM;
	std::vector<uint8_t> img_buf;
	int64_t img_buf_size;

	std::string GetErrorString(GX_STATUS emErrorStatus);
	bool CloseAndPrintLastError(GX_STATUS emErrorStatus, const int line_macros_val=0);
	
public:
	CameraDaheng(const int dev_id, const int channel_num);
	CameraDaheng();
	bool ConfigureCamera(const int dev_id);
	void ShowConf();

	bool Start();
	int GetImgSize();
	bool GetFrame(uint8_t* pixels_corrected);
	bool Stop();

	int GetHeight();
	int GetWidth();
	void Close();
	~CameraDaheng();
};


