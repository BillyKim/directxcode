#include "TextObject.h"
#include "Utilities.h"

TextObject::TextObject(void)
{
}

TextObject::TextObject(
	ID2D1Factory*			d2dFactory, 
	ID2D1HwndRenderTarget*	rendertarget, 
	IDWriteFactory*			dwriteFactory,
	wchar_t*		text, 
	float			fontSize,
	D2D1_COLOR_F	fillColor,
	D2D1_COLOR_F	outlineColor,
	D2D1_COLOR_F	boundaryFillColor,
	D2D1_COLOR_F    boundaryOutlineColor,
	float			outlineWidth
	)
	:
	d2dFactory_(d2dFactory),
	rendertarget_(rendertarget),
	dwriteFactory_(dwriteFactory),
	boundaryOutlineBrush_(NULL),
	boundaryFillBrush_(NULL),
	boundaryFillColor_(boundaryFillColor),
	boundaryOutlineColor_(boundaryOutlineColor),
	text_(NULL),
	fontSize_(fontSize),
	activeIndex_(0),
	isLive_(true),
    letterBuffer_(NULL)
{
	createText(text, d2dFactory_, rendertarget_, dwriteFactory_, fontSize);

	// Create boundary outline brush
	// Create outline brush
	HRESULT hr = rendertarget->CreateSolidColorBrush(boundaryOutlineColor, &boundaryOutlineBrush_);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create outline brush failed!", L"Error", 0);
		return;
	}

	// Create fill brush
	hr = rendertarget->CreateSolidColorBrush(boundaryFillColor_, &boundaryFillBrush_);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create fill brush failed!", L"Error", 0);
		return;
	}
	// Create boundary fill brush
}

void TextObject::createText(
	wchar_t* textString, 
	ID2D1Factory* d2dFactory, 
	ID2D1HwndRenderTarget*	rendertarget, 
	IDWriteFactory*			dwriteFactory,
	float fontSize
	)
{
	// Initialize each letter in the text to a LetterObject object
	length_ = wcslen(textString);

	text_ = new wchar_t[length_ + 1]; // Add one more space for '\0'
	for(int i = 0; i < length_; ++i)
	{
		text_[i] = textString[i];
	}

	text_[length_] = '\0';

	letterBuffer_ = new BaseLetter*[length_];
	
	// top left coordinates of current letter
	float currentX = 0; 
	float currentY = 0;
	letterSpace_ = fontSize_ / 10.0f; // space between adjacent letters.

	for(int i = 0; i < length_; ++i)
	{
		letterBuffer_[i] = new BaseLetter(
			d2dFactory,
			rendertarget,
			dwriteFactory,
			textString[i],		// here is a string instead
			fontSize
			);
		
		D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(currentX, currentY);
		letterBuffer_[i]->translate(currentX, currentY);

		// Get the boundary of the current letter
		D2D1_RECT_F rect = letterBuffer_[i]->getBoundRect();

		// Move the next letter horizontally, so that they were not overlapped.
		currentX += (rect.right - rect.left) + letterSpace_;
		currentY = 0;
	}
}

TextObject::~TextObject(void)
{
	SAFE_DELETE(text_);
	SAFE_DELETE_ARRAY(letterBuffer_);
}

void TextObject::reset(wchar_t* text, D2D1_POINT_2F& position, D2D_VECTOR_2F& velocity, D2D1_COLOR_F& fillColor)
{
	activeIndex_	= 0;
	isLive_			= true;
	SAFE_DELETE_ARRAY(letterBuffer_);
	SAFE_DELETE(text_);

	createText(text, d2dFactory_, rendertarget_, dwriteFactory_, fontSize_);

	setPosition(position);

	setVelocity(velocity);

	setFillColor(fillColor);
}

void TextObject::update(float timeDelta)
{
	for(int i = 0; i < length_; ++i)
	{
		letterBuffer_[i]->update(timeDelta);
	}
}

void TextObject::render()
{
	// draw boundary
	// drawBoundary();

	for(int i = 0; i < length_; ++i)
	{
		letterBuffer_[i]->render();
	}
}

void TextObject::setFillColorRange(int startIndex, int length, D2D1_COLOR_F& color)
{
	for(int i = startIndex; i < startIndex + length; ++i)
	{
		letterBuffer_[i]->setFillColor(color);
	}
}

void TextObject::setFillColor(D2D1_COLOR_F& color)
{
	setFillColorRange(0, length_, color);
}

void TextObject::setOutlineColorRange(int startIndex, int length, D2D1_COLOR_F& color)
{
	for(int i = startIndex; i < startIndex + length; ++i)
	{
		letterBuffer_[i]->setOutlineColor(color);
	}
}

void TextObject::setOutlineColor(D2D1_COLOR_F& color)
{
	setOutlineColorRange(0, length_, color);
}

void TextObject::setBoundaryOutlineColor(D2D1_COLOR_F& color)
{
	boundaryOutlineColor_ = color;
}

D2D1_COLOR_F TextObject::getBoundaryOutlineColor() const
{
	return boundaryOutlineColor_;
}

void TextObject::setBoundaryFillColor(D2D1_COLOR_F& color)
{
	boundaryFillColor_ = color;
}

D2D1_COLOR_F TextObject::getBoundaryFillColor() const
{
	return boundaryFillColor_;
}

void TextObject::setOutlineWidthRange(int startIndex, int length, float width)
{
	for(int i = 0; i < length_; ++i)
	{
		letterBuffer_[i]->setOutlineWidth(width);
	}
}

void TextObject::setOutlineWidth(float width)
{
	setOutlineWidthRange(0, length_, width);
}

void TextObject::setLiveState(bool state)
{
	isLive_ = state;
}

bool TextObject::isLive() const
{
	return isLive_;
}

void TextObject::setVelocityRange(int startIndex, int length, D2D_VECTOR_2F& velocity)
{
	for(int i = startIndex; i < startIndex + length_; ++i)
	{
		letterBuffer_[i]->setVelocity(velocity);
	}
}

void TextObject::setVelocity(D2D_VECTOR_2F& velocity)
{
	setVelocityRange(0, length_, velocity);
}

D2D_VECTOR_2F TextObject::getVelocity() const
{
	return letterBuffer_[0]->getVelocity();
}

void TextObject::setPostionRange(int startIndex, int length, D2D1_POINT_2F& position)
{
	float currentX = position.x;
	float currentY = position.y;

	for(int i = startIndex; i < length; ++i)
	{
		// Set the position for current letter
		letterBuffer_[i]->setPosition(currentX, currentY);

		// Get the boundary of current letter
		D2D1_RECT_F rect = letterBuffer_[i]->getBoundRect();

		// Calculate the position for next letter, since letters in a string are horizantaly aligned, so 
		// each letter has the same y-coordinate, but has an increament of the x-coordinate.
		currentX += (rect.right - rect.left) + letterSpace_;
	}
}

void TextObject::setPosition(D2D1_POINT_2F& position)
{
	setPostionRange(0, length_, position);
}

void TextObject::setTransfrom(D2D1_MATRIX_3X2_F& matrix)
{
	for(int i = 0; i < length_; ++i)
	{
		letterBuffer_[i]->setTransform(matrix);
	}
}

void TextObject::setActiveIndex(int index)
{
	activeIndex_ = index;
}

int TextObject::getActiveIndex() const
{
	return activeIndex_;
}

wchar_t* TextObject::getText() const
{
	return text_;
}

int TextObject::getTextLength() const
{
	return length_;
}

void TextObject::setLetterSpeedFactor(float speedFactor)
{
	for(int i = 0; i < length_; ++i)
	{
		letterBuffer_[i]->setSpeedFactor(speedFactor);
	}
}

BaseLetter* TextObject::getFirstActiveLetterObject() const
{
	return getLetter(activeIndex_);
}

void TextObject::onHit()
{
	// Change the color of the hit letter
	setFillColorRange(activeIndex_, 1, D2D1::ColorF(D2D1::ColorF::White));
	setOutlineColorRange(activeIndex_, 1, D2D1::ColorF(D2D1::ColorF::Black));

	// Update active index of text
	setActiveIndex(activeIndex_ + 1);

	// If all the letters in the text was hit, we treat the text as dead
	if(activeIndex_ == getTextLength())
	{
		setLiveState(false);
	}
}

D2D1_RECT_F TextObject::getBoundaryRect() const
{
	D2D1_RECT_F rect = {10000, 10000, -10000, -10000}; // result rectangle

	// Get the boundary rect for each letter in the text object
	// then compute the min top, max bottom, min left and max right value to build a new rect
	for(int i = 0; i < length_; ++i)
	{
		D2D1_RECT_F tempRect= letterBuffer_[i]->getBoundRect();

		rect.top	= min(rect.top, tempRect.top);
		rect.bottom = max(rect.bottom, tempRect.bottom);
		rect.left	= min(rect.left, tempRect.left);
		rect.right	= max(rect.right, tempRect.right);
	}

	return rect;
}

void TextObject::drawBoundary() const
{
	D2D1_RECT_F rect = getBoundaryRect();

	float width = rect.right - rect.left;
	float height = rect.bottom - rect.top;

	// extend the boundary by 10% each side
	rect.top	-= height * 0.1f;
	rect.bottom += height * 0.1f;
	rect.left	-= width * 0.1f;
	rect.right	+= width * 0.1f;

	rendertarget_->DrawRectangle(&rect, boundaryOutlineBrush_, 2.0f);
	rendertarget_->FillRectangle(&rect, boundaryFillBrush_);
}

bool TextObject::outofWindow(RECT& windowRect)
{
	D2D1_RECT_F rect = getBoundaryRect();

	// Check whether letter was out of window
	if( rect.bottom < windowRect.top 
		|| rect.top > windowRect.bottom
		|| rect.right < windowRect.left
		|| rect.left > windowRect.right )
	{
		return true;
	}
	else
	{
		return false;
	}
}

BaseLetter* TextObject::getLetter(int index) const
{
	return letterBuffer_[index];
}