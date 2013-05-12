/*
 * Copyright 2013 Dario Casalinuovo
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef SPIN_SLIDER_H
#define SPIN_SLIDER_H

#include "SeekSlider.h"
#include "SpinControl.h"


class SpinSlider : public SeekSlider {
public:
								SpinSlider(const char* name, const char* label,
									BMessage* message, int32 minValue,
									int32 maxValue);

								SpinSlider(BRect r, const char* name,
									const char* label, BMessage* message,
									int32 minValue, int32 maxValue);

	virtual						~SpinSlider();

private:
			void				_Init(int32 minValue, int32 maxValue);
			SpinControl*		fSpinControl;
};


#endif	//SEEK_SLIDER_H
