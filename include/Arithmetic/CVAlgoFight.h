//
//		Copyright :					@2018, ***, All Rights Reserved
//
//		Author :						王科威
//		E-mail :						wangkw531@icloud.com
//		Date :							2019-07-19
//		Description :				CV打架检测算法类
//
//		History:						Author										Date													Description
//											王科威										2019-08-26									创建
//

#ifndef CV_ALGO_FIGHT_H
#define CV_ALGO_FIGHT_H

#include "Arithmetic/CVAlgo.h"
#include "BaseFightDll.h"

namespace framework
{
	namespace arithmetic
	{
		class CVAlgoFight : public CVAlgo
		{
		public:
			CVAlgoFight(void);
			~CVAlgoFight(void);

		protected:
			int initializeArithmetic() override;
			int deinitializeArithmetic(void) override;
			void arithmeticWorkerProcess(void) override;

		private:
			CFightAlgorithmClass fight;
			FeedBackFight feedback;
		};//class CVAlgoFight
	}//namespace arithmetic
}//namespace framework

#endif//CV_ALGO_FIGHT_H
