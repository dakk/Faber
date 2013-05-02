/*******************************************************
* Creator: Martin Johansson
* Portability: Non-Native
*-------------------------------------------------------
*
* This class contains the interface the eq presents to
* the dialog.
*
*******************************************************/

#ifndef __EM_EQUALIZER_INTERFACE
#define __EM_EQUALIZER_INTERFACE

class EMEqualizerInterface
{
public:
	EMEqualizerInterface() {};
	virtual ~EMEqualizerInterface() {};

	virtual void GetSettings(int8& p_vBase, int8& p_vMid, int8& p_vTreb) = 0;
	virtual void SetSettings(int8 p_vBase, int8 p_vMid, int8 p_vTreb) = 0;
};

#endif
