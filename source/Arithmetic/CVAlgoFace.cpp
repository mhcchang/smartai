#include <windows.h>
#include <io.h>
#include "boost/algorithm/string.hpp"
#include "boost/checked_delete.hpp"
#include "boost/filesystem.hpp"
#include "boost/format.hpp"
#include "error.h"
#include "Arithmetic/CVAlgoFace.h"

NS_BEGIN(algo, 1)

CVAlgoFace::CVAlgoFace(CaptureFaceInfoHandler handler /* = NULL */)
	: CVAlgo(NULL, handler), largestRegisterFaceID{ 0 }
{
	InitializeCriticalSection(&criticalSection);
}

CVAlgoFace::~CVAlgoFace()
{
	DeleteCriticalSection(&criticalSection);
}

int CVAlgoFace::addFacePicture(const char* filePath /* = NULL */, const int faceID /* = 0 */)
{
	int status{ ERR_INVALID_PARAM };

	if (filePath && 0 < faceID)
	{
		status = face.RegisterFace((char*)filePath, faceID) ? ERR_OK : ERR_BAD_OPERATE;

		if (ERR_OK == status)
		{
			mtx.lock();
			registerFaceImageGroup.insert(std::make_pair(faceID, filePath));
			mtx.unlock();
		}
	}

	return status;
}

int CVAlgoFace::removeFacePicture(const long long uuid /* = -1 */)
{
	int status{ ERR_NOT_FOUND };

// 	mtx.lock();
// 	for(boost::unordered_map<int, const std::string>::iterator it = registerFaceImageGroup.begin(); it != registerFaceImageGroup.end(); ++it)
// 	{
// 		std::string uuidStr{ (boost::format("%lld") % uuid).str() };
// 		const std::string faceImagePath{ it->second };
// 		const char* isSubstr{ std::strstr(faceImagePath.c_str(), uuidStr.c_str()) };
// 
// 		if (isSubstr)
// 		{
// 			registerFaceImageGroup.erase(it);
// 			std::remove(faceImagePath.c_str());
// 			break;
// 		}
// 	}
// 	mtx.unlock();

	return status;
}

// int CVAlgoFace::queryFace(const long long uuid, char*& jpegData, int& jpegBytes)
// {
// 	mtx.lock();
// 	for (boost::unordered_map<int, const std::string>::iterator it = registerFaceImageGroup.begin(); it != registerFaceImageGroup.end(); ++it)
// 	{
// 		std::string uuidStr{ (boost::format("%lld") % uuid).str() };
// 		const char* isSubstr{ std::strstr(it->second.c_str(), uuidStr.c_str()) };
// 
// 		if (isSubstr)
// 		{
// 			FILE* jpegFile{ NULL };
// 			fopen_s(&jpegFile, it->second.c_str(), "rb+");
// 			
// 			if (jpegFile)
// 			{
// 				const long jpegDataBytes{ _filelength(_fileno(jpegFile)) };
// 				if (jpegData)
// 				{
// 					fread(jpegData, jpegDataBytes, 1, jpegFile);
// 					jpegBytes = jpegDataBytes;
// 				}
// 
// 				fclose(jpegFile);
// 			}
// 
// 			break;
// 		}
// 	}
// 	mtx.unlock();
// 
// 	return ERR_OK;
// }

int CVAlgoFace::initializeWithParameter(const char* configFilePath /* = NULL */, void* parameter /* = NULL */)
{
	const std::string cfgFile{ (boost::format("%s\\model\\face.cfg") % configFilePath).str() };
	const std::string weightFile{ (boost::format("%s\\model\\face.weights") % configFilePath).str() };
	StruInitParams* initParames{ reinterpret_cast<StruInitParams*>(parameter) };
	initParames->cfgfile = (char*)cfgFile.c_str();
	initParames->weightFile = (char*)weightFile.c_str();
	initParames->matchThreshold = 0.52f;
	int status{ face.InitModel(IMAGE_WIDTH, IMAGE_HEIGHT, CHANNEL_NUMBER, *initParames, &criticalSection) };

	if (status)
	{
		loadAndRegisterFacePicture(configFilePath);
	}

	return status;
}

void CVAlgoFace::arithmeticWorkerProcess()
{
	boost::winapi::ULONGLONG_ lastKnownTickTime{ 0 };

	while (1)
	{
		FeedBackFaceRecog feedback;
		MediaImagePtr bgr24ImagePtr{ BGR24ImageQueue.remove() };

		if (bgr24ImagePtr)
		{
			std::vector<FaceInfo> faceInfos;
//			boost::winapi::ULONGLONG_ mainProcTime{ GetTickCount64() };
			face.MainProcFunc((unsigned char*)bgr24ImagePtr->getImage(), feedback);
//			printf("=====  MainProcFunc run time = %lld.\r\n", GetTickCount64() - mainProcTime);

			for (int i = 0; i != feedback.vecShowInfo.size(); ++i)
			{
				DWORD postProcStart = GetTickCount64();
				if (face.PostProcessFunc(feedback))
				{
					for (int j = 0; j != feedback.vecFaceResult.size(); ++j)
					{
						FaceInfo faceInfo;
						faceInfo.type = AlarmType::ALARM_TYPE_FACE;
						faceInfo.similarity = feedback.vecFaceResult[j].similarValue;
						faceInfo.faceID = feedback.vecFaceResult[j].matchId;

						mtx.lock();
						boost::unordered_map<int, const std::string>::iterator it{ 
							registerFaceImageGroup.find(feedback.vecFaceResult[j].matchId) };
						if (registerFaceImageGroup.end() != it)
						{
							FILE* fd{ NULL };
							fopen_s(&fd, it->second.c_str(), "rb+");
							if (fd)
							{
								faceInfo.imageBytes = _filelength(_fileno(fd));
								faceInfo.imageData = new(std::nothrow) char[faceInfo.imageBytes];
								if (faceInfo.imageData)
								{
									fread(faceInfo.imageData, faceInfo.imageBytes, 1, fd);
								}
								fclose(fd);
							}
						}
						mtx.unlock();

						faceInfos.push_back(faceInfo);
//						boost::checked_array_delete(feedback.vecFaceResult[j].pUcharImage);
					}
				}
			}

			if (capturefaceInfoHandler && 0 < faceInfos.size())
			{
				boost::winapi::ULONGLONG_ currentTickTime{ GetTickCount64() };

				if (!lastKnownTickTime || 3000 < currentTickTime - lastKnownTickTime)
				{
					lastKnownTickTime = currentTickTime;
					capturefaceInfoHandler(bgr24ImagePtr, faceInfos);
				}
			}

			feedback.vecShowInfo.clear();
			feedback.mapMemory.clear();
		}
		else
		{
			break;
		}
	}
}

int CVAlgoFace::loadAndRegisterFacePicture(const char* directoryFilePath /* = NULL */)
{
	boost::filesystem::path recursiveDirPath((boost::format("%s\\Face") % directoryFilePath).str());
	boost::filesystem::recursive_directory_iterator endIter;

	for (boost::filesystem::recursive_directory_iterator it(recursiveDirPath); it != endIter; ++it)
	{
		if (!boost::filesystem::is_directory(*it))
		{
			const std::string faceImageFileName{ it->path().filename().string() };

			if (!faceImageFileName.empty())
			{
				std::vector<std::string> faceImageFileNameSegment;
				boost::split(faceImageFileNameSegment, faceImageFileName, boost::is_any_of("_"));
				const int currentRegisterFaceID{ atoi(faceImageFileNameSegment[0].c_str()) };
				registerFaceImageGroup.insert(std::make_pair(currentRegisterFaceID, it->path().string()));

				//Keep largestRegisterFaceID value is the newest to use for next time.
				if (currentRegisterFaceID > largestRegisterFaceID)
				{
					largestRegisterFaceID = currentRegisterFaceID;
				}

				const std::string jpegFileFullPath{ (boost::format("%s\\face\\%s") % directoryFilePath % faceImageFileName).str() };
				if (face.RegisterFace(const_cast<char*>(jpegFileFullPath.c_str()), currentRegisterFaceID))
				{
					mtx.lock();
					registerFaceImageGroup.insert(std::make_pair(currentRegisterFaceID, jpegFileFullPath.c_str()));
					mtx.unlock();
				}
			}
		}
	}

	return ERR_OK;
}

NS_END