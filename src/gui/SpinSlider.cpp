/*
 * Copyright 2013 Dario Casalinuovo
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "SpinSlider.h"

#include <LayoutBuilder.h>

#define MSG_SPIN_CHANGED 	'spCh'


SpinSlider::SpinSlider(const char* name, const char* label,
	BMessage* message, int32 minValue, int32 maxValue)
	:
	SeekSlider(name, message, minValue, maxValue)
{
	SetLabel(label);
	_Init(minValue, maxValue);
}


SpinSlider::SpinSlider(BRect r, const char* name, const char* label,
	BMessage* message, int32 minValue, int32 maxValue)
	:
	SeekSlider(name, message, minValue, maxValue)
{
	SetLabel(label);
	_Init(minValue, maxValue);
}


SpinSlider::~SpinSlider()
{
}


void
SpinSlider::_Init(int32 minValue, int32 maxValue)
{
	fSpinControl = new SpinControl("SpinControl",
		NULL, new BMessage(MSG_SPIN_CHANGED),
		minValue, maxValue, Value(), 1);

	BLayoutBuilder::Group<>(this, B_HORIZONTAL, 1)
		.AddGlue()
		.Add(fSpinControl, 0)
	.End();
}
