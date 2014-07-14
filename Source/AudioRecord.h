/*
  ==============================================================================

    AudioRecorder.h
    Created: 15 Jun 2014 12:02:02pm
    Author:  Usuario

  ==============================================================================
*/

#ifndef AUDIORECORDER_H_INCLUDED
#define AUDIORECORDER_H_INCLUDED

#include "AppHeader.h"

//==============================================================================
/** A simple class that acts as an AudioIODeviceCallback and writes the 
	incoming audio data to a WAV file.
*/
class AudioRecord  : public AudioIODeviceCallback
{
public:
    //==============================================================================
    AudioRecord(AudioThumbnail& thumbnailToUpdate);
    ~AudioRecord();
	void startRecording (const File& file);
	void stop();
	bool isRecording() const;
	void audioDeviceAboutToStart (AudioIODevice* device);
	void audioDeviceIOCallback(const float** inputData,int InputChannels,float** outputData,int OutputChannels,int numSamples);
    void audioDeviceStopped();

private:
	//==============================================================================
	AudioThumbnail& thumbnail;
    TimeSliceThread backgroundThread; // thread que escribe audioData to disk
    ScopedPointer<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    double sampleRate;
	int64 nextSampleNum;

    CriticalSection writerLock;
    AudioFormatWriter::ThreadedWriter* volatile activeWriter;
};

#endif  // AUDIORECORDER_H_INCLUDED
