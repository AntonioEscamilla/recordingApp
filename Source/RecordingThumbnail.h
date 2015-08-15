/*
  ==============================================================================

    RecordingThumbnail.h
    Created: 16 Jun 2014 8:49:37am
    Author:  Administrador

  ==============================================================================
*/

#ifndef RECORDINGTHUMBNAIL_H_INCLUDED
#define RECORDINGTHUMBNAIL_H_INCLUDED

#include "AppHeader.h"

//==============================================================================
/*
*/
class RecordingThumbnail    : public Component,
						      public ChangeListener
{
public:
    RecordingThumbnail();
    ~RecordingThumbnail();
	AudioThumbnail& getAudioThumbnail();
	void setDisplayFullThumbnail (bool displayFull);
	void changeListenerCallback (ChangeBroadcaster* source);

    void paint (Graphics&);
    void resized();

private:
	AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    bool displayFullThumb;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordingThumbnail)
};


#endif  // RECORDINGTHUMBNAIL_H_INCLUDED
