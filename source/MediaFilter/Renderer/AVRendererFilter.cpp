#include "boost/make_shared.hpp"
#include "error.h"
#include "MediaData/MediaData.h"
#include "MediaModel/Renderer/D3D/VideoD3DRenderer.h"
// #include "MediaModel/Renderer/DXS/AudioDxSoundPlayer.h"
// using AudioDxSoundPlayer = NS(model, 1)::AudioDxSoundPlayer;
#include "MediaPin/MediaPin.h"
#include "MediaFilter/Renderer/AVRendererFilter.h"

namespace framework
{
	namespace multimedia
	{
		AVRendererFilter::AVRendererFilter(const RendererType type /* = RendererType::RENDERER_TYPE_NONE */)
			: MediaFilter(), rendererType{ type }
		{}

		AVRendererFilter::~AVRendererFilter()
		{}

		int AVRendererFilter::createNewFilter()
		{
			int status{ ERR_BAD_ALLOC };

			if (RendererType::RENDERER_TYPE_VIDEO == rendererType)
			{
				status = ERR_OK == MediaFilter::createNewInputPin(VideoStreamInputPinID) &&
					ERR_OK == MediaFilter::createNewOutputPin(VideoStreamOutputPinID) ? ERR_OK : ERR_BAD_ALLOC;
			}
			else if (RendererType::RENDERER_TYPE_AUDIO == rendererType)
			{
				status = ERR_OK == MediaFilter::createNewInputPin(AudioStreamInputPinID) &&
					ERR_OK == MediaFilter::createNewOutputPin(AudioStreamOutputPinID) ? ERR_OK : ERR_BAD_ALLOC;
			}

			return status;
		}

		int AVRendererFilter::createNewModel(MediaDataPtr mediaData)
		{
			int status{ mediaData ? ERR_OK : ERR_INVALID_PARAM };

			if (ERR_OK == status)
			{
				const MediaDataMainID mediaDataMainID{ mediaData->getMainID() };
				const MediaDataSubID mediaDataSubID{ mediaData->getSubID() };

				if (MediaDataMainID::MEDIA_DATA_MAIN_ID_VIDEO == mediaDataMainID)
				{
					MediaModelPtr dxVideoD3dRendererPtr{ 
						boost::make_shared<VideoD3DRenderer>(reinterpret_cast<HWND>(videoRendererWnd)) };
					if (dxVideoD3dRendererPtr)
					{
						mediaModelPtr.swap(dxVideoD3dRendererPtr);
					}
				}
				if (MediaDataMainID::MEDIA_DATA_MAIN_ID_AUDIO == mediaDataMainID)
				{
					// 				MediaModelPtr dxSoundPlayerPtr{ boost::make_shared<AudioDxSoundPlayer>() };
// 				if (dxSoundPlayerPtr)
// 				{
// 					mediaModelPtr.swap(dxSoundPlayerPtr);
// 				}
				}
			}

			return ERR_OK == status ? MediaFilter::createNewModel(mediaData) : status;
		}
	}//namespace multimedia
}//namespace framework
