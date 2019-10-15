//
//		Copyright :					@2019, ***, All Rights Reserved
//
//		Author :						王科威
//		E-mail :						wangkw531@icloud.com
//		Date :							2019-10-13
//		Description:					多媒体输出针脚类
//
//		History:						Author									Date														Description
//											王科威									2019-10-13										创建
//

#ifndef OUTPUT_MEDIA_PIN_H
#define OUTPUT_MEDIA_PIN_H

#include <vector>
#include "boost/thread/mutex.hpp"
#include "MediaPin.h"

NS_BEGIN(pin, 1)

class OutputMediaPin : public MediaPin
{
public:
	OutputMediaPin(void);
	virtual ~OutputMediaPin(void);

public:
	int connectInputPin(boost::shared_ptr<MediaPin> inputPinPtr) override;
	int inputData(MediaDataPtr dataPtr) override;
	virtual bool isOutputPin(void) const;

private:
	boost::mutex mtx;
	std::vector<boost::shared_ptr<MediaPin>> inputPinPtrGroup;
};//class OutputMediaPin

NS_END

#endif//OUTPUT_MEDIA_PIN_H
