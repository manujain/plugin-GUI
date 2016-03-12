/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2014 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef JULIAPROCESSOR_H_INCLUDED
#define JULIAPROCESSOR_H_INCLUDED

#ifdef _WIN32
#include <Windows.h>
#endif

#include <ProcessorHeaders.h>
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Processors/GenericProcessor/GenericProcessor.h"
#include "JuliaProcessorEditor.h"

/**

  This class serves as a template for creating new processors.

  If this were a real processor, this comment section would be used to
  describe the processor's function.

  @see GenericProcessor

*/

class JuliaProcessor : public GenericProcessor

{
public:

    /** The class constructor, used to initialize any members. */
    JuliaProcessor();

    /** The class destructor, used to deallocate memory */
    ~JuliaProcessor();

    //AudioProcessorEditor* createEditor();

    /*bool hasEditor() const
    {
        return true;
    }*/

    /** Determines whether the processor is treated as a source. */
    bool isSource()
    {
        return false;
    }

    /** Determines whether the processor is treated as a sink. */
    bool isSink()
    {
        return false;
    }

	/** Indicates if the processor has a custom editor. Defaults to false */
	bool hasEditor() const
	{
		return true;
	}

	/** If the processor has a custom editor, this method must be defined to instantiate it. */
	AudioProcessorEditor* createEditor();

	/** Optional method that informs the GUI if the processor is ready to function. If false acquisition cannot start. Defaults to true */
	//bool isReady();

    /** Defines the functionality of the processor.

        The process method is called every time a new data buffer is available.

        Processors can either use this method to add new data, manipulate existing
        data, or send data to an external target (such as a display or other hardware).

        Continuous signals arrive in the "buffer" variable, event data (such as TTLs
        and spikes) is contained in the "events" variable.
         */
    void process(AudioSampleBuffer& buffer, MidiBuffer& events);

    /** The method that standard controls on the editor will call.
		It is recommended that any variables used by the "process" function 
		are modified only through this method while data acquisition is active. */
    void setParameter(int parameterIndex, float newValue);

    void setFile(String fullpath);
    void setBuffersize(int bufferSize);

    String getFile();

    void reloadFile();

    void saveCustomParametersToXml(XmlElement* parentElement);
    void loadCustomParametersFromXml();


	/** Optional method called every time the signal chain is refreshed or changed in any way.
		
		Allows the processor to handle variations in the channel configuration or any other parameter
		passed down the signal chain. The processor can also modify here the settings structure, which contains
		information regarding the input and output channels as well as other signal related parameters. Said
		structure shouldn't be manipulated outside of this method.
	*/
	//void updateSettings();

private:

    // private members and methods go here
    //
    // e.g.:
    //
    // float threshold;
    // bool state;

    String filePath;
    bool hasJuliaInstance; // functions as enabled switch

    int dataHistoryBufferSize; 
    int dataHistoryBufferNumChannels; 
    AudioSampleBuffer* dataHistoryBuffer; // sample wise data buffer for passing some more history to julia

    void run_julia_string(String juliaString);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuliaProcessor);

};

#endif  // JULIAPROCESSOR_H_INCLUDED
