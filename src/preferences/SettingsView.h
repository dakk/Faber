/*
 * Copyright (c) Casalinuovo Dario. All rights reserved.
 * Copyright (c) Davide Gessa. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Davide Gessa, dak.linux@gmail.com
 */
#ifndef SETTINGS_VIEW_H
#define SETTINGS_VIEW_H

#include <View.h>
#include <MenuBar.h>
#include <Menu.h>
#include <Button.h>
#include <Slider.h>
#include <TabView.h>
#include <TextControl.h>

#define QUIT		'quit'
#define SET_FACTORY	'fact'


class SettingsView : public BView {
public:
					SettingsView();
					~SettingsView();
			void	AttachedToWindow();	
	virtual	void	MessageReceived(BMessage* message);

private:
	BButton*		fDefaultsButton;
	BButton*		fRevertButton;
	BTabView*		fTabView;
};



#endif
