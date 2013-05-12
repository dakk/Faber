/*
 * Copyright (c) Casalinuovo Dario. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Dario Casalinuovo, b.vitruvio@gmail.com
 */
#ifndef FABER_SETTINGS_H
#define FABER_SETTINGS_H

#include <File.h>
#include <InterfaceDefs.h>
#include <Message.h>
#include <Path.h>
#include <String.h>


class Settings : public BMessage {
public:
						Settings(const char* path);
virtual					~Settings();

		status_t		InitCheck() const;

	//	static Settings*	Instance();

		status_t		ReadColor(const char* name, rgb_color* color);

		status_t		WriteColor(const char* name, rgb_color color);
		
		status_t		RemoveSetting(const char* name);



private:
		status_t		OpenSettings();
		status_t		FlattenSettings();
		status_t		DeleteSettings();

		void			_CheckSettings();
		void			_SetTo();

		BFile*			fSettingsFile;
		BPath			fSettingsPath;

		int32			fControl;

		status_t		fError;

	//static Settings*	fInstance;
}; 

#endif /* SETTINGS_H */
