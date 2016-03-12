
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


#include <stdio.h>
#include "JuliaProcessor.h"

//If the processor uses a custom editor, it needs its header to instantiate it
#include "JuliaProcessorEditor.h"
#include <julia.h>
//#include "../../../../open_source/julia/src/julia.h"

JuliaProcessor::JuliaProcessor()
    : GenericProcessor("Julia Processor") //, threshold(200.0), state(true)

{
	hasJuliaInstance=false;
    dataHistoryBufferNumChannels=256;

    dataHistoryBuffer = new AudioSampleBuffer(dataHistoryBufferNumChannels, 60000);
    dataHistoryBuffer->clear();

	//Without a custom editor, generic parameter controls can be added
    //parameters.add(Parameter("thresh", 0.0, 500.0, 200.0, 0));

}

JuliaProcessor::~JuliaProcessor()
{
	deleteAndZero(dataHistoryBuffer);
}

/**
	If the processor uses a custom editor, this method must be present.
*/

AudioProcessorEditor* JuliaProcessor::createEditor()
{
	editor = new JuliaProcessorEditor(this, true);

	std::cout << "Creating editor." << std::endl;

	return editor;
}

void JuliaProcessor::setParameter(int parameterIndex, float newValue)
{
    //Parameter& p =  parameters.getReference(parameterIndex);
    //p.setValue(newValue, 0);

    //threshold = newValue;

    //std::cout << float(p[0]) << std::endl;
    editor->updateParameterButtons(parameterIndex);
}

void JuliaProcessor::setFile(String fullpath)
{
    
    hasJuliaInstance=true;
    filePath = fullpath;

    jl_init("/home/manu/julia/usr/bin"); // make this user configurable
    //JL_SET_STACK_BASE;
    
    jl_eval_string("include(\"juliaProcessor.jl\")"); // this runs the function definition in the file

    

    //String juliaString="include(\"";  // this runs the function definition in the file
    //juliaString+=filePath;
    //juliaString+="\")";
    
   // run_julia_string(juliaString);
}

void JuliaProcessor::setBuffersize(int bufferSize)
{
    if (bufferSize>1)
    {
        dataHistoryBufferSize=bufferSize;
        printf("Setting history buffer size to %d samples \n", dataHistoryBufferSize);
        dataHistoryBuffer->setSize(dataHistoryBufferNumChannels, dataHistoryBufferSize, false, true, false);
    } else {
        printf("History buffer size has to be at least 1");
    }
}

void JuliaProcessor::reloadFile()
{

    //jl_exit(0);
    //jl_init("/home/jvoigts/julia/usr/bin");
    //JL_SET_STACK_BASE;
    if (hasJuliaInstance) 
    {
        
        String juliaString="reload(\"";  // this re-runs the function definition in the file
        juliaString+=filePath;
        juliaString+="\")";
        
        run_julia_string(juliaString);

    } else {

        std::cout << "No julia instance running - cant refresh" << std::endl;
    }
}


void JuliaProcessor::run_julia_string(String juliaString)
{
    // need to convert from juce String to char array
    int nchars = juliaString.length();
    //printf("got %d chars\n", nchars);    
    char jstr[nchars];
    strncpy(jstr, juliaString.getCharPointer(), nchars);
    jstr[nchars]='\0'; // properly terminate the char array 

    printf("executing julia cmd: %s\n", jstr);

    jl_eval_string(jstr);

    if (jl_exception_occurred())
    printf("%s \n", jl_typeof_str(jl_exception_occurred()));
    
}

String JuliaProcessor::getFile()
{
    return filePath;
}

void JuliaProcessor::process(AudioSampleBuffer& buffer,
                               MidiBuffer& events)
{
	
	if (hasJuliaInstance){

    	jl_function_t *func = jl_get_function(jl_main_module, "oe_process!");    
        //jl_function_t *func  = jl_get_function(jl_base_module, "reverse!");

        // pass buffer to function
        // Create 2D array of float64 type

    
        jl_value_t *array_type = jl_apply_array_type(jl_float32_type, 1); // last arg is nDims
        //jl_array_t *x  = jl_alloc_array_2d(array_type, buffer->getNumChannels(), buffer->getNumSamples());
        
        for (int n = 0; n < getNumOutputs(); n++)
         {
            float* ptr = buffer.getWritePointer(n); // to perform in-place edits to the buffer
            //float* ptr = buffer.getReadPointer(n);
            jl_array_t *x = jl_ptr_to_array_1d(array_type, ptr , buffer.getNumSamples(), 0);

            JL_GC_PUSH1(&x);

            //jl_array_t *y = (jl_array_t*) jl_call1(func, (jl_value_t*)x);
            //buffer= jl_array_data(y);
            jl_call1(func, (jl_value_t*)x);

            JL_GC_POP();
        }
        /*
        // just test by running on one sample at a time
    	for (int i = 2; i < buffer.getNumChannels(); i++)
        {
        	for (int j = 0; j < nSamples; j++)
        	{
        		jl_value_t* argument = jl_box_float64((float)buffer.getSample(i,j));
        		//jl_value_t* argument = jl_box_float64(2.0);
        		jl_value_t* ret = jl_call1(func, argument);
                JL_GC_PUSH2(&argument,&ret);
        		double j_out=0;
        		if (jl_is_float64(ret)) {
        			j_out= jl_unbox_float64(ret);
        		}
    			if (jl_exception_occurred())
    			    printf("%s \n", jl_typeof_str(jl_exception_occurred()));
                
                JL_GC_POP();
                
        		buffer.setSample(i,j,j_out); //ch, sample,val
        	}
        }
        */

    }
	/**
	Generic structure for processing buffer data 
	*/
	/*int nChannels = buffer.getNumChannels();
	for (int chan = 0; chan < nChannels; chan++)
	{
		int nSamples = getNumSamples(chan);*/
		/**
		Do something here.
		
		To obtain a read-only pointer to the n sample of a channel:
		float* samplePtr = buffer.getReadPointer(chan,n);
		
		To obtain a read-write pointer to the n sample of a channel:
		float* samplePtr = buffer.getWritePointer(chan,n);

		All the samples in a channel are consecutive, so this example is valid:
		float* samplePtr = buffer.getWritePointer(chan,0);
		for (i=0; i < nSamples; i++)
		{
			*(samplePtr+i) = (*samplePtr+i)+offset;
		}
		
		See also documentation and examples for buffer.copyFrom and buffer.addFrom to operate on entire channels at once.

		To add a TTL event generated on the n-th sample:
		addEvents(events, TTL, n);

		
		*/
	//}

	/** Simple example that creates an event when the first channel goes under a negative threshold

    for (int i = 0; i < getNumSamples(channels[0]->sourceNodeId); i++)
    {
        if ((*buffer.getReadPointer(0, i) < -threshold) && !state)
        {
    
	        // generate midi event
            addEvent(events, TTL, i);
    
	        state = true;
    
	    } else if ((*buffer.getReadPointer(0, i) > -threshold + bufferZone)  && state)
        {
            state = false;
        }
    
	}
	*/


}

void JuliaProcessor::saveCustomParametersToXml(XmlElement* parentElement)
{

    XmlElement* childNode = parentElement->createNewChildElement("FILENAME");
    childNode->setAttribute("path", getFile());

}

void JuliaProcessor::loadCustomParametersFromXml()
{

    if (parametersAsXml != nullptr)
    {
        // use parametersAsXml to restore state

        forEachXmlChildElement(*parametersAsXml, xmlNode)
        {
            if (xmlNode->hasTagName("FILENAME"))
            {
                String filepath = xmlNode->getStringAttribute("path");
                JuliaProcessorEditor* fre = (JuliaProcessorEditor*) getEditor();
                fre->setFile(filepath);

            }
        }
    }

}
