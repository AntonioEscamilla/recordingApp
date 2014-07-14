/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "AppHeader.h"
#include "AudioRecord.h"
#include "RecordingThumbnail.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component,
							   public ButtonListener,
							   public AudioIODeviceCallback,
							   private Timer
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
	void buttonClicked (Button* buttonThatWasClicked);
	void audioDeviceIOCallback(const float** inputData,int InputChannels,float** outputData,int OutputChannels,int numSamples);
	void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped(){};
	void startRecording();
	void stopRecording();	
	void showAudioSettings();

private:
    //==============================================================================
	AudioFormatManager audioFormatManager;
	ScopedPointer<AudioFormatReaderSource> audioFormatReaderSource;
	ScopedPointer<AudioDeviceManager> audioDeviceManager;
	AudioTransportSource audioTransportSource;
	TimeSliceThread thread;
	AudioRecord recorder;
	RecordingThumbnail recordingThumbnail;
	TextButton* recordButton;
	TextButton* settingsButton;
	void timerCallback();

	TimeSliceThread meterThread;
    SegmentedMeter meterL, meterR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
