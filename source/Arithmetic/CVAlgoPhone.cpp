#include "boost/filesystem.hpp"
#include "boost/format.hpp"
#include "boost/winapi/time.hpp"
#include "boost/checked_delete.hpp"
#include "error.h"
#include "Arithmetic/CVAlgoPhone.h"

NS_BEGIN(algo, 1)

CVAlgoPhone::CVAlgoPhone() : CVAlgo()
{}

CVAlgoPhone::~CVAlgoPhone()
{}

int CVAlgoPhone::initializeArithmetic()
{
	const std::string executePath{
				boost::filesystem::initial_path<boost::filesystem::path>().string() };
	const std::string cfgFile{ (boost::format("%s\\model\\phone.cfg") % executePath).str() };
	const std::string weightFile{ (boost::format("%s\\model\\phone.weights") % executePath).str() };
	StruInitParams parameters;
	parameters.gpu_id = 0;
	parameters.detectThreshold = 0.80f;
	parameters.trackThreshold = 0.20f;
	parameters.cfgfile = (char*)cfgFile.c_str();
	parameters.weightFile = (char*)weightFile.c_str();

	const std::string weightFile2{ (boost::format("%s\\model\\phonesecond.weights") % executePath).str() };
	StruInitParams ap_second;
	ap_second.gpu_id = 0;
	ap_second.detectThreshold = 0.9;
	ap_second.cfgfile = (char*)cfgFile.c_str();
	ap_second.weightFile = (char*)weightFile2.c_str();

	return phone.InitAlgoriParam(
		IMAGE_WIDTH, IMAGE_HEIGHT, CHANNEL_NUMBER, parameters) ? ERR_OK : ERR_BAD_OPERATE;
}

int CVAlgoPhone::deinitializeArithmetic()
{
	return ERR_OK;
}

void CVAlgoPhone::arithmeticWorkerProcess()
{
//	boost::winapi::ULONGLONG_ lastKnownTickTime{ 0 };

	while (1)
	{
		MediaDataPtr bgr24ImagePtr{ BGR24ImageQueue.remove() };

		if (bgr24ImagePtr)
		{
			std::vector<AlarmInfo> alarmInfos;
//			boost::winapi::ULONGLONG_ mainProcTime{ GetTickCount64() };
			phone.MainProcFunc((unsigned char*)bgr24ImagePtr->getData(), feedback);
//			printf("=====  MainProcFunc run time = %lld.\r\n", GetTickCount64() - mainProcTime);

			typedef std::map<int, StruMemoryInfo>::iterator Iterator;
			for (Iterator it = feedback.mapMemory.begin(); it != feedback.mapMemory.end();)
			{
// 				if (it->second.bDone)
// 				{
					int nonePhoneNumber{ 0 }, havePhoneNumber{ 0 };
					float maxConfidence{ 0.0f };
					int nSaveId{ (int)(it->second.vecSaveMat.size() -1) };

					for (int i = 0; i < it->second.vecSaveMat.size(); i++)
					{
						if (0 == it->second.vecSaveMat[i].nLabel)
						{
							havePhoneNumber++;
						}
						else
						{
							nonePhoneNumber++;
						}

// 						if (it->second.vecSaveMat[i].detectConfidence > maxConfidence)
// 						{
// 							maxConfidence = it->second.vecSaveMat[i].detectConfidence;
// 							nSaveId = i;
// 						}
					}

					if (havePhoneNumber > nonePhoneNumber && -1 < nSaveId)
					{
						AlarmInfo alarmInfo;
						alarmInfo.type = AlarmType::ALARM_TYPE_PHONE;
						alarmInfo.x = it->second.vecSaveMat[nSaveId].rRect.x;
						alarmInfo.y = it->second.vecSaveMat[nSaveId].rRect.y;
						alarmInfo.w = it->second.vecSaveMat[nSaveId].rRect.width;
						alarmInfo.h = it->second.vecSaveMat[nSaveId].rRect.height;
						alarmInfo.status = it->second.vecSaveMat[nSaveId].nLabel;
						alarmInfos.push_back(alarmInfo);

// 						bgr24ImagePtr->setOriginImage(
// 							(const unsigned char*)(it->second.vecSaveMat[nSaveId].pUcharImage), IMAGE_WIDTH * IMAGE_HEIGHT * 3);
						boost::checked_array_delete(it->second.vecSaveMat[nSaveId].pUcharImage);
						it = feedback.mapMemory.erase(it);
					}
					else
					{
						++it;
					}

// 					if (0 < alarmInfos.size())
// 					{
// //						boost::winapi::ULONGLONG_ currentTickTime{ GetTickCount64() };
// 
// // 						if (!lastKnownTickTime || 3000 < currentTickTime - lastKnownTickTime)
// // 						{
// // 							lastKnownTickTime = currentTickTime;
// // 							captureAlarmInfoHandler(bgr24ImagePtr, alarmInfos);
// // 						}
// 					}

					if (0 < alarmInfos.size() && postDetectAlarmInfoCallback)
					{
						postDetectAlarmInfoCallback(alarmInfos, bgr24ImagePtr->getData(), bgr24ImagePtr->getDataBytes());
					}
// 				}
// 				else
// 				{
// 					++it;
// 				}
			}
		}
		else
		{
			break;
		}
	}
}

NS_END