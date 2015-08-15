/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent():thread ("audio file preview"),recorder(recordingThumbnail.getAudioThumbnail()),meterThread ("Meter Thread"){

	addAndMakeVisible (recordButton  = new TextButton ("Record"));
    recordButton->addListener (this);
    recordButton->setColour (TextButton::buttonColourId, Colour (0xffff5c5c));
    recordButton->setColour (TextButton::textColourOnId, Colours::black);

	addAndMakeVisible (settingsButton  = new TextButton ("Audio Settings"));
    settingsButton->addListener (this);
    settingsButton->setColour (TextButton::buttonColourId, Colours::grey);
    settingsButton->setColour (TextButton::textColourOnId, Colours::black);

	addAndMakeVisible (&meterL);
    addAndMakeVisible (&meterR);
	meterThread.addTimeSliceClient (&meterL);
    meterThread.addTimeSliceClient (&meterR);
    meterThread.startThread (1);

	addAndMakeVisible (recordingThumbnail);

	// Format manager
	audioFormatManager.registerBasicFormats();

	// Device manager
	audioDeviceManager = new AudioDeviceManager();
	audioDeviceManager->initialise(1, 1, 0, true);
	audioDeviceManager->addAudioCallback(this);

    setSize (500, 200);
	thread.startThread (3);
}

MainContentComponent::~MainContentComponent(){
	audioTransportSource.stop();
	audioTransportSource.setSource(nullptr);
	
	audioDeviceManager->removeAudioCallback(this);
	audioDeviceManager->removeAudioCallback (&recorder);
	audioDeviceManager=nullptr;

	audioFormatReaderSource=nullptr;

	deleteAndZero (recordButton);
	deleteAndZero (settingsButton);
	meterThread.removeTimeSliceClient (&meterL);
    meterThread.removeTimeSliceClient (&meterR);
    meterThread.stopThread (500);
}

void MainContentComponent::paint (Graphics& g){
    g.fillAll (Colour (0xffeeddff));
}

void MainContentComponent::resized(){
	Rectangle<int> area (getLocalBounds());
	recordButton->setBounds (10, 10, 90, 30);
	settingsButton->setBounds (10, 50, 90, 30);
    recordingThumbnail.setBounds (area.removeFromBottom (100).reduced (8));
	meterL.setBounds(getWidth()-43,10,15,80);
	meterR.setBounds(meterL.getRight() + 5,10,15,80);
}

void MainContentComponent::buttonClicked (Button* buttonThatWasClicked){
	if (buttonThatWasClicked == recordButton){
        if (recorder.isRecording())
            stopRecording();
        else
            startRecording();
	}else if(buttonThatWasClicked == settingsButton){
		showAudioSettings();
	}
}

void MainContentComponent::audioDeviceIOCallback(const float** inputData,int InputChannels,float** outputData,int OutputChannels,int numSamples){	
	meterL.copySamples (inputData[0], numSamples);
	meterR.copySamples (inputData[0], numSamples);
	
	AudioSampleBuffer buffer(InputChannels, numSamples);
	AudioSourceChannelInfo info;	
	info.buffer = &buffer;
	info.numSamples = numSamples;
	info.startSample = 0;
	
	audioTransportSource.getNextAudioBlock(info);

	for(int i = 0; i < InputChannels; i++){
		for(int j = 0; j < numSamples; j++){
			float sample =  *buffer.getSampleData(i, j);
			outputData[i][j] =sample;
		}
	}
}

void MainContentComponent::audioDeviceAboutToStart (AudioIODevice* device){
    audioTransportSource.prepareToPlay (device->getCurrentBufferSizeSamples(),device->getCurrentSampleRate());
}

void MainContentComponent::startRecording(){
	audioTransportSource.stop();
    audioTransportSource.setSource (nullptr);
    audioFormatReaderSource = nullptr;

	File soundfile (File::getSpecialLocation (File::userDocumentsDirectory).getChildFile("03-Bongo_bong.wav"));
	
	AudioFormatReader* audioFormatReader = audioFormatManager.createReaderFor(soundfile);
	audioFormatReaderSource = new AudioFormatReaderSource(audioFormatReader, true);
	audioTransportSource.setSource(audioFormatReaderSource, 32768, &thread, audioFormatReader->sampleRate, 1);
	audioTransportSource.start();
	//audioDeviceManager->addAudioCallback(this);
	audioDeviceManager->addAudioCallback(&recorder);

    const File file (File::getSpecialLocation (File::userDocumentsDirectory).getNonexistentChildFile ("OutputRecording", ".wav"));
    recorder.startRecording (file);
    recordButton->setButtonText ("Stop");
	recordingThumbnail.setDisplayFullThumbnail (false);
	startTimer(10000);
}

void MainContentComponent::stopRecording(){
	audioTransportSource.stop();
    recorder.stop();
    recordButton->setButtonText ("Record");
	recordingThumbnail.setDisplayFullThumbnail (true);
}

void MainContentComponent::timerCallback(){
	audioTransportSource.stop();
    recorder.stop();
    recordButton->setButtonText ("Record");
	recordingThumbnail.setDisplayFullThumbnail (true);
}

void MainContentComponent::showAudioSettings(){
    AudioDeviceSelectorComponent settingsComp (*audioDeviceManager,1, 2,1, 2,false, false,true, false);
    settingsComp.setSize (500, 400);
    LookAndFeel_V3 settingsLaf;
    settingsLaf.setColour (Label::textColourId, Colours::white);
    settingsLaf.setColour (TextButton::buttonColourId, Colours::white);
    settingsLaf.setColour (TextButton::textColourOffId, Colours::black);
    settingsComp.setLookAndFeel (&settingsLaf);
    
    DialogWindow::showModalDialog ("Audio Settings", &settingsComp, nullptr,Colours::darkgrey,true, true, true);
}