// Copyright (c) 2019, *** Inc.
// All rights reserved.
//
// Author : ������
// E-mail : wangkw531@icloud.com
//
// Video and audio demuxer filter class.
//

#ifndef AV_DEMUXER_FILTER_H
#define AV_DEMUXER_FILTER_H

#include "MediaFilter/SourceMediaFilter.h"
#include "MediaDemuxer/MediaDemuxer.h"
using MediaDemuxerPtr = boost::shared_ptr<NS(demuxer, 1)::MediaDemuxer>;

NS_BEGIN(filter, 1)

class AVDemuxerFilter : public SourceMediaFilter
{
public:
	AVDemuxerFilter(void);
	virtual ~AVDemuxerFilter(void);

protected:
	int inputMediaData(MediaDataPtr mediaData) override;

private:
	int createNewMediaDemuxer(MediaDataPtr mediaData);
	int createNewOutputPin(void);

protected:
	MediaDemuxerPtr mediaDemuxerPtr;
};//class AVDemuxerFilter

NS_END

#endif//AV_DEMUXER_FILTER_H
