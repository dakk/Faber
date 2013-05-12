/*
 * Copyright (c) Casalinuovo Dario. All rights reserved.
 * Copyright (c) Davide Gessa. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Casalinuovo Dario, b.vitruvio@gmail.com
 *		Davide Gessa, dak.linux@gmail.com
 */
#include "SettingsView.h"

#include <Button.h>
#include <GroupLayout.h>
#include <GroupView.h>
#include <OS.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Slider.h>
#include <TextControl.h>
#include <TabView.h>
#include <View.h>
#include <Box.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <Input.h>
#include <LayoutBuilder.h>
#include <SpaceLayoutItem.h>

#include "Globals.h"
#include "PrefGeneral.h"
#include "PrefKeys.h"
#include "PrefColors.h"
#include "StringItem.h"


SettingsView::~SettingsView()
{

}


SettingsView::SettingsView()
	:
	BView("SettingsView", B_FOLLOW_ALL_SIDES)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// Main layout
	BGroupLayout* rootLayout = new BGroupLayout(B_VERTICAL);
	SetLayout(rootLayout);

	BRect r;
	BTabView *tabView;
	BTab *tab;
	
	tabView = new BTabView(r, " ");
	tabView->SetViewColor(216,216,216,0);

	r = tabView->Bounds();
	r.InsetBy(5,5);
	r.bottom -= tabView->TabHeight();
	rootLayout->AddView(tabView);


	// Interface
	BView *interfaceView = new PrefGeneral();
	
	tab = new BTab();
	tabView->AddTab(interfaceView, tab);
	tab->SetLabel(B_TRANSLATE("Interface"));

	// Interface
	BView *colorView = new PrefColors();
	
	tab = new BTab();
	tabView->AddTab(colorView, tab);
	tab->SetLabel(B_TRANSLATE("Color layout"));

	// Interface
	BView *hotkeyView = new PrefKeys();
	
	tab = new BTab();
	tabView->AddTab(hotkeyView, tab);
	tab->SetLabel(B_TRANSLATE("Key bindings"));



	// Audio I/O
	BView *audioView = new BView(" ", B_WILL_DRAW,  0);
	
	//engineCombo = new BComboBox(BRect(0,0, 200, 30), " ", "Engine", new BMessage(MSG_SETTINGS_DEFAULTS));
	//audioView->AddView(engineCombo);
	
	
	tab = new BTab();
	tabView->AddTab(audioView, tab);
	tab->SetLabel(B_TRANSLATE("Audio IO"));

	// Defaults and revert buttons
	BGroupView* buttonGroup = new BGroupView(B_HORIZONTAL);

	fDefaultsButton = new BButton(B_TRANSLATE("Defaults"),
		new BMessage(MSG_SETTINGS_DEFAULTS));						

	buttonGroup->GroupLayout()->AddView(fDefaultsButton);

	fRevertButton = new BButton(B_TRANSLATE("Revert"),
		new BMessage(MSG_SETTINGS_REVERT));							

	buttonGroup->GroupLayout()->AddView(fRevertButton);

	rootLayout->AddView(buttonGroup);
}


void SettingsView::AttachedToWindow()
{
	fDefaultsButton->SetTarget(this);
	fRevertButton->SetTarget(this);
	
	Window()->CenterOnScreen();
}


void SettingsView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case MSG_SETTINGS_REVERT:
		case MSG_SETTINGS_DEFAULTS:
		default:
			BView::MessageReceived(message);
	}			
}


