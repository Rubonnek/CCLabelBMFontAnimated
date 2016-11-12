//
//  CCLabelAnimated.cpp
//  CCLabelAnimated
//
//  Created by Steven Barnegren on 23/03/2015.
//
//

/*
   Copyright (c) 2015 Steve Barnegren

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "CCAnimatedLabel.h"

//CREATE FUNCTIONS

AnimatedLabel* AnimatedLabel::createWithBMFont(const std::string& bmfontFilePath, const std::string& text,const cocos2d::TextHAlignment& alignment /* = TextHAlignment::LEFT */, int maxLineWidth /* = 0 */, const cocos2d::Vec2& imageOffset /* = Vec2::ZERO */)
{
	auto ret = new AnimatedLabel();

	if (ret && ret->setBMFontFilePath(bmfontFilePath,imageOffset))
	{
		ret->setMaxLineWidth(maxLineWidth);
		ret->setString(text);
		ret->autorelease();

		return ret;
	}

	delete ret;
	return nullptr;
}

AnimatedLabel* AnimatedLabel::createWithTTF(const std::string& text, const std::string& fontFile, float fontSize, const cocos2d::Size& dimensions /* = Size::ZERO */, cocos2d::TextHAlignment hAlignment /* = TextHAlignment::LEFT */, cocos2d::TextVAlignment vAlignment /* = TextVAlignment::TOP */)
{
	auto ret = new (std::nothrow) AnimatedLabel();

	if (ret && cocos2d::FileUtils::getInstance()->isFileExist(fontFile))
	{
		cocos2d::TTFConfig ttfConfig(fontFile.c_str(),fontSize,cocos2d::GlyphCollection::DYNAMIC);
		if (ret->setTTFConfig(ttfConfig))
		{
			ret->setAlignment(hAlignment, vAlignment);
			ret->setDimensions(dimensions.width,dimensions.height);
			ret->setString(text);

			ret->autorelease();

			return ret;
		}
	}

	delete ret;
	return nullptr;
}

#pragma mark - Set Basic Character Properties

void AnimatedLabel::setCharScale(int index, float s)
{

	if (index >= getStringLength())
	{
		cocos2d::log("AnimatedLabel - Could not set character sprite scale, index out of bounds");
		return;
	}

	cocos2d::Sprite *charSprite = getLetter(index);
	charSprite->setScale(s);
}

void AnimatedLabel::setCharOpacity(int index, float o)
{

	if (index >= getStringLength())
	{
		cocos2d::log("AnimatedLabel - Could not set character sprite opacity, index out of bounds");
		return;
	}

	GLubyte opacity = o;
	cocos2d::Sprite *charSprite = getLetter(index);
	charSprite->setOpacity(opacity);
}

void AnimatedLabel::setCharRotation(int index, float r)
{

	if (index >= getStringLength())
	{
		cocos2d::log("AnimatedLabel - Could not set character sprite rotation, index out of bounds");
		return;
	}

	cocos2d::Sprite *charSprite = getLetter(index);
	charSprite->setRotation(r);
}


void AnimatedLabel::setAllCharsScale(float s)
{

	const int numChars = getStringLength();

	for (int i = 0; i < numChars; ++i)
	{
		//cocos2d::Sprite *charSprite = (cocos2d::Sprite*)getChildren().at(i);
		cocos2d::Sprite *charSprite = getLetter(i);

		//cocos2d::log("desc: %s, index: %d", _label->getDescription().c_str(), i);

		if (_utf16Text[i] != '\n')
			charSprite->setScale(s);
	}
}


void AnimatedLabel::setAllCharsOpacity(float o)
{

	GLubyte opacity = o;

	const int numChars = getStringLength();

	for (int i = 0; i < numChars; ++i)
	{
		cocos2d::Sprite *charSprite = getLetter(i);
		charSprite->setOpacity(opacity);
	}
}

void AnimatedLabel::setAllCharsRotation(float r)
{

	const int numChars = getStringLength();

	for (int i = 0; i < numChars; ++i)
	{
		cocos2d::Sprite *charSprite = getLetter(i);
		charSprite->setRotation(r);
	}
}

void AnimatedLabel::offsetAllCharsPositionBy(cocos2d::Vec2 offset)
{

	const int numChars = getStringLength();

	for (int i = 0; i < numChars; ++i)
	{
		cocos2d::Sprite *charSprite = getLetter(i);
		charSprite->setPosition(cocos2d::Vec2(charSprite->getPosition().x + offset.x, charSprite->getPosition().y + offset.y));
	}
}


#pragma mark - Run Custom Actions

void AnimatedLabel::runActionOnSpriteAtIndex(int index, cocos2d::FiniteTimeAction* action)
{

	if (index >= getStringLength() || index < 0)
	{
		cocos2d::log("AnimatedLabel::runActionOnSpriteAtIndex - index out of bounds");
		return;
	}

	cocos2d::Sprite *charSprite = getLetter(index);
	charSprite->runAction(action);

}


void AnimatedLabel::runActionOnAllSprites(cocos2d::Action* action, bool removeOnCompletion /* = false */, cocos2d::CallFunc *callFuncOnCompletion /* = nullptr */)
{

	const int numChars = getStringLength();

	for (int i = 0; i < numChars; ++i)
	{
		cocos2d::Action *actionCopy = action->clone();
		cocos2d::Sprite *charSprite = getLetter(i);

		if (i == numChars-1) //if i is the last character, run the call func actions
		{
			//cocos2d::Array *actionsArray = cocos2d::Array::create();
			cocos2d::Vector<cocos2d::FiniteTimeAction*> actionsArray;

			actionsArray.pushBack((cocos2d::FiniteTimeAction*)actionCopy);

			//actionsArray->addObject(actionCopy);

			if (callFuncOnCompletion != nullptr)
			{
				actionsArray.pushBack((cocos2d::FiniteTimeAction*)callFuncOnCompletion);
			}
			if (removeOnCompletion)
			{
				actionsArray.pushBack(cocos2d::CallFunc::create(CC_CALLBACK_0(AnimatedLabel::removeFromParent, this)));
			}
			cocos2d::Sequence *actionsSequence = cocos2d::Sequence::create(actionsArray);
			charSprite->runAction(actionsSequence);
		}
		else
		{
			//if i is not the last character, just run the action
			charSprite->runAction(actionCopy);
		}
	}
}

void AnimatedLabel::stopActionsOnAllSprites()
{

	const int numChars = getStringLength();

	for (int i = 0; i < numChars; ++i)
	{
		cocos2d::Sprite *charSprite = getLetter(i);
		charSprite->stopAllActions();
	}
}

// TODO: Some functions below can only work if the number of characters is 2 or
// more. I noticed this is not really necessary for all of these functions
// since this requirement can be dropped. Work on this later on.
// TL;DR: Remove the numChars < 2 requirement where possible.
void AnimatedLabel::runActionOnAllSpritesSequentially(cocos2d::FiniteTimeAction* action, float duration, bool removeOnCompletion /* = false */, cocos2d::CallFunc *callFuncOnCompletion /* = nullptr */)
{

	const int numChars = getStringLength();

	if (numChars < 2)
	{
		cocos2d::log("AnimatedLabel - runActionOnAllSpritesSequentially() requires at least 2 children to operate");
		return;
	}

	for (int i = 0; i < numChars; ++i)
	{
		cocos2d::DelayTime *delay = cocos2d::DelayTime::create((duration/(numChars-1)) *i);
		cocos2d::Action *actionCopy = action->clone();
		cocos2d::Sequence *delayAndAction = cocos2d::Sequence::create(delay, actionCopy, nullptr);

		cocos2d::Sprite *charSprite = getLetter(i);

		if (i == numChars-1) //if i is the last character, run the call func actions
		{

			cocos2d::Vector<cocos2d::FiniteTimeAction*> actionsArray;
			actionsArray.pushBack(delayAndAction);

			if (callFuncOnCompletion != nullptr)
			{
				actionsArray.pushBack(callFuncOnCompletion);
			}
			if (removeOnCompletion)
			{
				actionsArray.pushBack(cocos2d::CallFunc::create(CC_CALLBACK_0(AnimatedLabel::removeFromParent, this)));
			}

			// TODO: When the last character is a new line, we are not able to
			// run an action on it. I suppose we can't because there's nothing
			// getting rendered. In any case, I won't need to run an action on
			// that last newline character. Let's just cicunvent the issue for now.
			if (_utf16Text[i] == '\n')
			{
				charSprite = getLetter(i - 1);
				charSprite->stopAllActions();
			}

			cocos2d::Sequence *actionsSequence = cocos2d::Sequence::create(actionsArray);
			charSprite->runAction(actionsSequence);
		}
		else //if i is not the last character, just run the action
		{
			if (_utf16Text[i] != '\n') //both _label and _currentUTF16String has been made public!!!!
				charSprite->runAction(delayAndAction);
			//cocos2d::log("position: %f, %f, %d", charSprite->getPositionX(), charSprite->getPositionY(), i);
		}

	}
}

void AnimatedLabel::runActionOnAllSpritesSequentiallyReverse(cocos2d::FiniteTimeAction* action, float duration, bool removeOnCompletion /* = false */, cocos2d::CallFunc *callFuncOnCompletion /* = nullptr */)
{
	const int numChars = getStringLength();

	if (numChars < 2)
	{
		cocos2d::log("AnimatedLabel - runActionOnAllSpritesSequentiallyReverse() requires at least 2 children to operate");
		return;
	}

	for (int i = 0; i < numChars; ++i)
	{
		cocos2d::DelayTime *delay = cocos2d::DelayTime::create((duration/(numChars-1)) *((numChars-1)-i));
		cocos2d::Action *actionCopy = action->clone();
		cocos2d::Sequence *delayAndAction = cocos2d::Sequence::create(delay, actionCopy, nullptr);
		cocos2d::Sprite *charSprite = getLetter(i);

		if (i == 0) //if i is the first character, run the call func actions
		{

			cocos2d::Vector<cocos2d::FiniteTimeAction*> actionsArray;
			actionsArray.pushBack(delayAndAction);

			if (callFuncOnCompletion != nullptr)
			{
				actionsArray.pushBack(callFuncOnCompletion);
			}
			if (removeOnCompletion)
			{
				actionsArray.pushBack(cocos2d::CallFunc::create(CC_CALLBACK_0(AnimatedLabel::removeFromParent, this)));
			}
			cocos2d::Sequence *actionsSequence = cocos2d::Sequence::create(actionsArray);
			charSprite->runAction(actionsSequence);
		}
		else //if i is not the first character, just run the action
		{
			charSprite->runAction(delayAndAction);
		}
	}

}

#pragma mark Animations

void AnimatedLabel::flyPastAndRemove()
{

	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	float rescaleFactor = 1/getScale(); //if the label has been scaled down, all the action coordinates will be too small, rescale factor scales them up

	offsetAllCharsPositionBy(cocos2d::Vec2(-visibleSize.width * rescaleFactor, 0));

	float centrePortion = visibleSize.width*0.05;
	float centreSlowTime = 0.9;

	cocos2d::MoveBy *flyIn = cocos2d::MoveBy::create(0.5, cocos2d::Vec2((visibleSize.width* rescaleFactor) -((centrePortion* rescaleFactor)/2), 0));
	cocos2d::EaseExponentialInOut *flyInEase = cocos2d::EaseExponentialInOut::create(flyIn);

	cocos2d::MoveBy *centreSlowMove = cocos2d::MoveBy::create(centreSlowTime, cocos2d::Vec2(centrePortion * rescaleFactor, 0));
	cocos2d::ScaleTo *scaleUp = cocos2d::ScaleTo::create(centreSlowTime/2, 1.5);
	cocos2d::ScaleTo *scaleDown = cocos2d::ScaleTo::create(centreSlowTime/2, 1);
	cocos2d::Sequence *swell = cocos2d::Sequence::create(scaleUp, scaleDown, nullptr);
	cocos2d::Spawn *centreMoveAndSwell = cocos2d::Spawn::create(centreSlowMove, swell, nullptr);

	cocos2d::MoveBy *flyOut = cocos2d::MoveBy::create(0.5, cocos2d::Vec2((visibleSize.width*rescaleFactor) -((centrePortion*rescaleFactor)/2), 0));
	cocos2d::EaseExponentialInOut *flyOutEase = cocos2d::EaseExponentialInOut::create(flyOut);

	cocos2d::Sequence *flyPast = cocos2d::Sequence::create(flyInEase, centreMoveAndSwell, flyOutEase, nullptr);

	runActionOnAllSpritesSequentiallyReverse(flyPast, 0.7, true, nullptr);

}

void AnimatedLabel::animateInTypewriter(float duration, cocos2d::CallFunc *callFuncOnEach, cocos2d::CallFunc *callFuncOnCompletion /* = nullptr */)
{
	//set all the characters scale to zero
	setAllCharsScale(0);

	cocos2d::ScaleTo *appear = cocos2d::ScaleTo::create(0, 1);
	if ( callFuncOnEach != nullptr)
	{
		cocos2d::Sequence *appearAndAction = cocos2d::Sequence::create(appear, callFuncOnEach, nullptr);
		runActionOnAllSpritesSequentially(appearAndAction, duration, false, callFuncOnCompletion);
	}
	else
	{
		runActionOnAllSpritesSequentially(appear, duration, false, callFuncOnCompletion);
	}
}

void AnimatedLabel::animateInFlyInFromLeft(float duration)
{

	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	float rescaleFactor = 1/getScale(); //if the label has been scaled down, all the action coordinates will be too small, rescale factor scales them up
	float offsetX = visibleSize.width * rescaleFactor;
	offsetAllCharsPositionBy(cocos2d::Vec2(-offsetX, 0));

	cocos2d::MoveBy *flyIn = cocos2d::MoveBy::create(1, cocos2d::Vec2(offsetX, 0));
	cocos2d::EaseExponentialOut *flyInEase = cocos2d::EaseExponentialOut::create(flyIn);

	runActionOnAllSpritesSequentially(flyInEase, duration);


}

void AnimatedLabel::animateInFlyInFromRight(float duration)
{

	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	float rescaleFactor = 1/getScale(); //if the label has been scaled down, all the action coordinates will be too small, rescale factor scales them up
	float offsetX = visibleSize.width * rescaleFactor;
	offsetAllCharsPositionBy(cocos2d::Vec2(offsetX, 0));

	cocos2d::MoveBy *flyIn = cocos2d::MoveBy::create(1, cocos2d::Vec2(-offsetX, 0));
	cocos2d::EaseExponentialOut *flyInEase = cocos2d::EaseExponentialOut::create(flyIn);

	runActionOnAllSpritesSequentiallyReverse(flyInEase, duration);

}

void AnimatedLabel::animateInFlyInFromTop(float duration)
{

	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	float rescaleFactor = 1/getScale(); //if the label has been scaled down, all the action coordinates will be too small, rescale factor scales them up
	float offsetY = visibleSize.height * rescaleFactor;
	offsetAllCharsPositionBy(cocos2d::Vec2(0, offsetY));

	cocos2d::MoveBy *flyIn = cocos2d::MoveBy::create(1, cocos2d::Vec2(0, -offsetY));
	cocos2d::EaseExponentialOut *flyInEase = cocos2d::EaseExponentialOut::create(flyIn);

	runActionOnAllSpritesSequentially(flyInEase, duration);

}

void AnimatedLabel::animateInFlyInFromBottom(float duration)
{

	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	float rescaleFactor = 1/getScale(); //if the label has been scaled down, all the action coordinates will be too small, rescale factor scales them up
	float offsetY = visibleSize.height * rescaleFactor;
	offsetAllCharsPositionBy(cocos2d::Vec2(0, -offsetY));

	cocos2d::MoveBy *flyIn = cocos2d::MoveBy::create(1, cocos2d::Vec2(0, offsetY));
	cocos2d::EaseExponentialOut *flyInEase = cocos2d::EaseExponentialOut::create(flyIn);

	runActionOnAllSpritesSequentially(flyInEase, duration);

}


void AnimatedLabel::animateInDropFromTop(float duration)
{

	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	float rescaleFactor = 1/getScale(); //if the label has been scaled down, all the action coordinates will be too small, rescale factor scales them up
	float offsetY = visibleSize.height * rescaleFactor;
	offsetAllCharsPositionBy(cocos2d::Vec2(0, offsetY));

	cocos2d::MoveBy *flyIn = cocos2d::MoveBy::create(1, cocos2d::Vec2(0, -offsetY));
	cocos2d::EaseBounceOut *flyInEase = cocos2d::EaseBounceOut::create(flyIn);

	runActionOnAllSpritesSequentially(flyInEase, duration);

}

void AnimatedLabel::animateInSwell(float duration)
{

	setAllCharsScale(0);

	cocos2d::ScaleTo *scaleUp = cocos2d::ScaleTo::create(0.2, 1.5);
	cocos2d::ScaleTo *scaleDown = cocos2d::ScaleTo::create(0.2, 1);
	cocos2d::Sequence *scaleSeq = cocos2d::Sequence::create(scaleUp, scaleDown, nullptr);

	runActionOnAllSpritesSequentially(scaleSeq, duration);

}

void AnimatedLabel::animateInRevealFromLeft(float duration)
{

	//set all chars opacity to zero, apart from first
	setAllCharsOpacity(0);
	cocos2d::Sprite *firstChar = getLetter(0);
	firstChar->setOpacity(255);
	//make sure the first character has higher z order than the rest, reset after the animation
	cocos2d::DelayTime *delay = cocos2d::DelayTime::create(duration);
	cocos2d::CallFunc *resetZ = cocos2d::CallFunc::create(CC_CALLBACK_0(AnimatedLabel::reorderChild, this, firstChar, firstChar->getLocalZOrder()));

	cocos2d::Sequence *resetZAfterAnimation = cocos2d::Sequence::create(delay, resetZ, nullptr);
	this->reorderChild(firstChar, firstChar->getLocalZOrder()+10);
	firstChar->runAction(resetZAfterAnimation);

	//reveal each char from the behind the first
	for (int i = 0, numChars = getStringLength(); i < numChars; ++i)
	{

		cocos2d::Sprite *charSprite = getLetter(i);

		cocos2d::MoveTo *move = cocos2d::MoveTo::create(duration, charSprite->getPosition());
		cocos2d::EaseExponentialOut *moveEase = cocos2d::EaseExponentialOut::create(move);
		cocos2d::FadeIn *fadeIn = cocos2d::FadeIn::create(duration);
		cocos2d::EaseExponentialOut *fadeEase = cocos2d::EaseExponentialOut::create(fadeIn);
		cocos2d::Spawn *moveAndFade = cocos2d::Spawn::create(moveEase, fadeEase, nullptr);

		charSprite->setPosition(cocos2d::Vec2(firstChar->getPosition().x, charSprite->getPosition().y));
		charSprite->runAction(moveAndFade);

	}

}

void AnimatedLabel::animateSwell(float duration)
{

	cocos2d::ScaleTo *scaleUp = cocos2d::ScaleTo::create(0.2, 1.5);
	cocos2d::ScaleTo *scaleDown = cocos2d::ScaleTo::create(0.2, 1);
	cocos2d::Sequence *scaleSeq = cocos2d::Sequence::create(scaleUp, scaleDown, nullptr);

	runActionOnAllSpritesSequentially(scaleSeq, duration);

}

void AnimatedLabel::animateJump(float duration, float height)
{

	for (int i = 0, numChars = getStringLength(); i < numChars; ++i)
	{
		cocos2d::Sprite *charSprite = getLetter(i);

		cocos2d::DelayTime *delay = cocos2d::DelayTime::create((duration/(numChars-1)) *i);
		cocos2d::JumpTo *jump = cocos2d::JumpTo::create(0.5, charSprite->getPosition(), height, 1);
		cocos2d::Sequence *delayThenJump = cocos2d::Sequence::create(delay, jump, nullptr);
		charSprite->runAction(delayThenJump);
	}

}

void AnimatedLabel::animateStretchElastic(float stretchDuration, float releaseDuration, float stretchAmount)
{

	for (int i = 0, numChars = getStringLength(); i < numChars; ++i)
	{

		cocos2d::Sprite *charSprite = getLetter(i);

		cocos2d::MoveTo *stretch = cocos2d::MoveTo::create(stretchDuration,
				cocos2d::Vec2((charSprite->getPosition().x - (getContentSize().width/4)) * stretchAmount,
					charSprite->getPosition().y));
		cocos2d::MoveTo *release = cocos2d::MoveTo::create(releaseDuration, charSprite->getPosition());
		cocos2d::EaseElasticOut *releaseElastic = cocos2d::EaseElasticOut::create(release);
		cocos2d::Sequence *animation = cocos2d::Sequence::create(stretch, releaseElastic, nullptr);

		charSprite->runAction(animation);

	}

}

void AnimatedLabel::animateInSpin(float duration, int spins)
{

	setAllCharsOpacity(0);

	for (int i = 0, numChars = getStringLength(); i < numChars; ++i)
	{

		cocos2d::Sprite *charSprite = getLetter(i);

		cocos2d::MoveTo *moveToPosition = cocos2d::MoveTo::create(duration, charSprite->getPosition());
		cocos2d::EaseExponentialOut *moveToPositionEase = cocos2d::EaseExponentialOut::create(moveToPosition);
		float centreX = this->getContentSize().width/2;
		charSprite->setPosition(cocos2d::Vec2(centreX, charSprite->getPosition().y));
		charSprite->runAction(moveToPositionEase);

		cocos2d::RotateBy *counterRotate = cocos2d::RotateBy::create(duration, -360 * spins);
		cocos2d::EaseSineOut *counterRotateEase = cocos2d::EaseSineOut::create(counterRotate);
		charSprite->runAction(counterRotateEase);

		cocos2d::FadeIn *fadeIn = cocos2d::FadeIn::create(duration);
		charSprite->runAction(fadeIn);

	}


	//spin the label
	cocos2d::RotateBy *spin = cocos2d::RotateBy::create(duration, 360 * spins);
	cocos2d::EaseSineOut *spinEase = cocos2d::EaseSineOut::create(spin);
	this->runAction(spinEase);


}

void AnimatedLabel::animateInVortex(float duration, int spins, bool removeOnCompletion /* = false */, bool createGhosts /* = true */)
{

	//fade in the label
	float fadeDuration = duration * 0.25;
	cocos2d::FadeIn *fadeIn = cocos2d::FadeIn::create(fadeDuration);
	this->runAction(fadeIn);

	if (createGhosts)
	{
		int numGhosts = 3;
		float ghostMaxOpacity = 100;

		for (int i = 0; i < numGhosts; ++i)
		{

			AnimatedLabel *ghostLabel = AnimatedLabel::createWithBMFont(getBMFontFilePath(), getString(), cocos2d::TextHAlignment::CENTER, getContentSize().width*2, cocos2d::Vec2(0,0));

			// AnimatedLabel *ghostLabel = AnimatedLabel::create(getString(), getBMFontFilePath(), getContentSize().width*2, cocos2d::kCCTextAlignmentCenter);
			ghostLabel->setOpacity(ghostMaxOpacity/(i+1));
			ghostLabel->setPosition(this->getPosition());
			this->getParent()->addChild(ghostLabel);
			ghostLabel->animateInVortex(duration, spins, true, false);

		}
	}

	for (int i = 0, numChars = getStringLength(); i < numChars; ++i)
	{

		//Alter the number of spins on some characters for variation
		int charSpins = spins;
		if (i % 2 == 0)
		{
			charSpins--;
		}
		else if (i % 3 == 0)
		{
			charSpins++;
		}

		//randomly vary the speed of letters
		float staggerAmount = (cocos2d::random() % 10)/10.0f;
		float letterDuration = duration + staggerAmount;

		float spinDuration = letterDuration/charSpins;

		cocos2d::Sprite *charSprite = getLetter(i);

		float radius = fabs((this->getContentSize().width/2) - charSprite->getPosition().x);
		float squareLength = sqrtf((radius*radius)/2);
		float arcAmount = radius - squareLength;

		float segmentDuration = spinDuration*0.25;

		cocos2d::Vector<cocos2d::FiniteTimeAction*> spinActions;

		for (int s = 0; s < charSpins; s++)
		{

			/*
			   Spin in a circular motion. Circle is split into 4 segments.
			   Segment 1 is the NW arc of the circle
			   Segment 2 is the NE arc of the circle
			   Segment 3 is the SW arc of the circle
			   Segment 4 is the SE arc of the circle
			   */

			//segment 4
			cocos2d::MoveBy *segment4Straight = cocos2d::MoveBy::create(segmentDuration, cocos2d::Vec2(-radius, -radius));
			cocos2d::MoveBy *segment4PositiveArc = cocos2d::MoveBy::create(segmentDuration/2, cocos2d::Vec2(arcAmount, -arcAmount));
			cocos2d::EaseSineOut *segment4PositiveArcEase = cocos2d::EaseSineOut::create(segment4PositiveArc);
			cocos2d::MoveBy *segment4NegativeArc = cocos2d::MoveBy::create(segmentDuration/2, cocos2d::Vec2(-arcAmount, arcAmount));
			cocos2d::EaseSineIn *segment4NegativeArcEase = cocos2d::EaseSineIn::create(segment4NegativeArc);
			cocos2d::Sequence *segment4Arc = cocos2d::Sequence::create(segment4PositiveArcEase, segment4NegativeArcEase, nullptr);
			cocos2d::Spawn *segment4Action = cocos2d::Spawn::create(segment4Straight, segment4Arc, nullptr);

			//segment 3
			cocos2d::MoveBy *segment3Straight = cocos2d::MoveBy::create(segmentDuration, cocos2d::Vec2(-radius, radius));
			cocos2d::MoveBy *segment3PositiveArc = cocos2d::MoveBy::create(segmentDuration/2, cocos2d::Vec2(-arcAmount, -arcAmount));
			cocos2d::EaseSineOut *segment3PositiveArcEase = cocos2d::EaseSineOut::create(segment3PositiveArc);
			cocos2d::MoveBy *segment3NegativeArc = cocos2d::MoveBy::create(segmentDuration/2, cocos2d::Vec2(arcAmount, arcAmount));
			cocos2d::EaseSineIn *segment3NegativeArcEase = cocos2d::EaseSineIn::create(segment3NegativeArc);
			cocos2d::Sequence *segment3Arc = cocos2d::Sequence::create(segment3PositiveArcEase, segment3NegativeArcEase, nullptr);
			cocos2d::Spawn *segment3Action = cocos2d::Spawn::create(segment3Straight, segment3Arc, nullptr);

			//segment 1
			cocos2d::MoveBy *segment1Straight = cocos2d::MoveBy::create(segmentDuration, cocos2d::Vec2(radius, radius));
			cocos2d::MoveBy *segment1PositiveArc = cocos2d::MoveBy::create(segmentDuration/2, cocos2d::Vec2(-arcAmount, arcAmount));
			cocos2d::EaseSineOut *segment1PositiveArcEase = cocos2d::EaseSineOut::create(segment1PositiveArc);
			cocos2d::MoveBy *segment1NegativeArc = cocos2d::MoveBy::create(segmentDuration/2, cocos2d::Vec2(arcAmount, -arcAmount));
			cocos2d::EaseSineIn *segment1NegativeArcEase = cocos2d::EaseSineIn::create(segment1NegativeArc);
			cocos2d::Sequence *segment1Arc = cocos2d::Sequence::create(segment1PositiveArcEase, segment1NegativeArcEase, nullptr);
			cocos2d::Spawn *segment1Action = cocos2d::Spawn::create(segment1Straight, segment1Arc, nullptr);

			//segment 2
			cocos2d::MoveBy *segment2Straight = cocos2d::MoveBy::create(segmentDuration, cocos2d::Vec2(radius, -radius));
			cocos2d::MoveBy *segment2PositiveArc = cocos2d::MoveBy::create(segmentDuration/2, cocos2d::Vec2(arcAmount, arcAmount));
			cocos2d::EaseSineOut *segment2PositiveArcEase = cocos2d::EaseSineOut::create(segment2PositiveArc);
			cocos2d::MoveBy *segment2NegativeArc = cocos2d::MoveBy::create(segmentDuration/2, cocos2d::Vec2(-arcAmount, -arcAmount));
			cocos2d::EaseSineIn *segment2NegativeArcEase = cocos2d::EaseSineIn::create(segment2NegativeArc);
			cocos2d::Sequence *segment2Arc = cocos2d::Sequence::create(segment2PositiveArcEase, segment2NegativeArcEase, nullptr);
			cocos2d::Spawn *segment2Action = cocos2d::Spawn::create(segment2Straight, segment2Arc, nullptr);

			cocos2d::Sequence *spinAction;

			//Depending on if the letter is to the left or right of the centre of the label, the segments will need to be in different orders:

			if ((this->getContentSize().width/2) < charSprite->getPosition().x)
			{
				spinAction = cocos2d::Sequence::create(segment4Action, segment3Action, segment1Action, segment2Action, nullptr);
			}
			else
			{
				spinAction = cocos2d::Sequence::create(segment1Action, segment2Action, segment4Action, segment3Action, nullptr);
			}


			spinActions.pushBack(spinAction);


		}

		cocos2d::Sequence *animation = cocos2d::Sequence::create(spinActions);
		cocos2d::EaseSineOut *animationEase = cocos2d::EaseSineOut::create(animation);

		charSprite->runAction(animationEase);
	}

	if (removeOnCompletion)
	{

		cocos2d::DelayTime *waitForAnimation = cocos2d::DelayTime::create(duration * 3);
		cocos2d::CallFunc *remove = cocos2d::CallFunc::create(CC_CALLBACK_0(AnimatedLabel::removeFromParent, this));
		cocos2d::Sequence *waitThenRemove = cocos2d::Sequence::create(waitForAnimation, remove, nullptr);
		this->runAction(waitThenRemove);
	}

}

void AnimatedLabel::animateRainbow(float duration)
{
	const float tintDuration = 0.2;

	cocos2d::TintTo *red = cocos2d::TintTo::create(tintDuration, 255, 0, 0);
	cocos2d::TintTo *orange = cocos2d::TintTo::create(tintDuration, 255, 153, 51);
	cocos2d::TintTo *yellow = cocos2d::TintTo::create(tintDuration, 255, 255, 0);
	cocos2d::TintTo *green = cocos2d::TintTo::create(tintDuration, 0, 255, 0);
	cocos2d::TintTo *blue = cocos2d::TintTo::create(tintDuration, 0, 0, 255);
	cocos2d::TintTo *purple = cocos2d::TintTo::create(tintDuration, 102, 0, 204);
	cocos2d::TintTo *pink = cocos2d::TintTo::create(tintDuration, 255, 51, 255);
	cocos2d::TintTo *white = cocos2d::TintTo::create(tintDuration, 255, 255, 255);

	cocos2d::Sequence *rainbow = cocos2d::Sequence::create(red, orange, yellow, green, blue, purple, pink, white, nullptr);
	runActionOnAllSpritesSequentially(rainbow, duration);
}
