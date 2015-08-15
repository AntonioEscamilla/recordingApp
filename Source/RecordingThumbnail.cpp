/*
  ==============================================================================

    RecordingThumbnail.cpp
    Created: 16 Jun 2014 8:49:37am
    Author:  Administrador

  ==============================================================================
*/
#include "RecordingThumbnail.h"

//==============================================================================
RecordingThumbnail::RecordingThumbnail():thumbnailCache (2),thumbnail (512, formatManager, thumbnailCache),displayFullThumb (false){
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener (this);
}

RecordingThumbnail::~RecordingThumbnail(){
	thumbnail.removeChangeListener (this);
}

void RecordingThumbnail::paint (Graphics& g){
    g.fillAll (Colours::darkgrey);
    g.setColour (Colours::lightgrey);

    if (thumbnail.getTotalLength() > 0.0){
        const double endTime = displayFullThumb ? thumbnail.getTotalLength()
                                                    : jmax (30.0, thumbnail.getTotalLength());

        Rectangle<int> thumbArea (getLocalBounds());
        thumbnail.drawChannels (g, thumbArea.reduced (2), 0.0, endTime, 1.0f);
    }else{
        g.setFont (14.0f);
        g.drawFittedText ("(No file recorded)", getLocalBounds(), Justification::centred, 2);
    }
}

void RecordingThumbnail::resized(){
}

AudioThumbnail& RecordingThumbnail::getAudioThumbnail(){ 
	return thumbnail;
}

void RecordingThumbnail::setDisplayFullThumbnail (bool displayFull){
    displayFullThumb = displayFull;
    repaint();
}

void RecordingThumbnail::changeListenerCallback (ChangeBroadcaster* source) {
    if (source == &thumbnail)
        repaint();
}