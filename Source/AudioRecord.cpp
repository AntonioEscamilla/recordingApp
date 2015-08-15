/*
  ==============================================================================

    AudioRecorder.cpp
    Created: 15 Jun 2014 12:02:02pm
    Author:  Usuario

  ==============================================================================
*/

#include "AudioRecord.h"

//==============================================================================
/** A simple class that acts as an AudioIODeviceCallback and writes the
    incoming audio data to a WAV file.
*/

AudioRecord::AudioRecord(AudioThumbnail& thumbnailToUpdate): thumbnail (thumbnailToUpdate),backgroundThread ("Audio Recorder Thread"), activeWriter (nullptr){
	backgroundThread.startThread();
}

AudioRecord::~AudioRecord(){
    stop();
}

//==============================================================================
void AudioRecord::startRecording (const File& file){
    stop();

    if (sampleRate > 0){
        // Crear un OutputStream para escribir el archivo...
        file.deleteFile();
        ScopedPointer<FileOutputStream> fileStream (file.createOutputStream());

        if (fileStream != nullptr){
            // Se crea el objeto WAVwriter que escribe en el OutputStream...
            WavAudioFormat wavFormat;
            AudioFormatWriter* writer = wavFormat.createWriterFor (fileStream, sampleRate, 1, 16, StringPairArray(), 0);

            if (writer != nullptr){
                fileStream.release(); // Se pasa la responsabilidad de borrar el stream al writerObject que ahora lo esta usando

                // helperObject que escribe los datos en disco usando un backgroundThread.
                threadedWriter = new AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768);
				// Reset our recording thumbnail
                thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
                nextSampleNum = 0;

                // Se activa el writerpointer para que el audiocallback lo empiece a usar..
                const ScopedLock sl (writerLock);
                activeWriter = threadedWriter;
            }
        }
    }
}

void AudioRecord::stop(){
    // First, clear this pointer to stop the audio callback from using our writer object..
    {
        const ScopedLock sl (writerLock);
        activeWriter = nullptr;
    }

    // Now we can delete the writer object. It's done in this order because the deletion could
    // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
    // the audio callback while this happens.
    threadedWriter = nullptr;
}

bool AudioRecord::isRecording() const{
    return activeWriter != nullptr;
}

//==============================================================================
void AudioRecord::audioDeviceAboutToStart (AudioIODevice* device) {
    sampleRate = device->getCurrentSampleRate();
}

void AudioRecord::audioDeviceStopped(){
    sampleRate = 0;
}

void AudioRecord::audioDeviceIOCallback(const float** inputData,int InputChannels,float** outputData,int OutputChannels,int numSamples) {
    const ScopedLock sl (writerLock);

    if (activeWriter != nullptr){
        activeWriter->write (inputData, numSamples);

		// Create an AudioSampleBuffer to wrap our incomming data, note that this does no allocations or copies, it simply references our input data
        const AudioSampleBuffer buffer (const_cast<float**> (inputData), thumbnail.getNumChannels(), numSamples);
        thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
        nextSampleNum += numSamples;
    }

    // Se borran los outputBuffers, en caso de que esten llenos de basura..
    for (int i = 0; i < OutputChannels; ++i)
        if (outputData[i] != nullptr)
            FloatVectorOperations::clear (outputData[i], numSamples);
}