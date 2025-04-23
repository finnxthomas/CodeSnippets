'''
~~~~~~

Code Sample License Agreement
Effective Date: 4/15/2025
Cinnafilm, Inc. ("Licensor") grants Finn Thomas ("Licensee") a non-exclusive, 
non-transferable, revocable license to use the provided code samples ("Code") 
under the following terms:
- Permitted Use: Licensee may use the Code solely for personal, non-commercial 
purposes, such as inclusion in a portfolio or demonstration during job 
interviews.
- Restrictions: Licensee may not: (a) use the Code for any commercial purpose; 
(b) distribute, sell, sublicense, or otherwise share the Code with third 
parties; (c) modify the Code for purposes beyond personal demonstration; or 
(d) claim ownership of the Code.
- Ownership: The Code remains the exclusive property of Cinnafilm, Inc.
- Termination: This license may be terminated by Licensor at any time with 
written notice to Licensee, after which Licensee must cease all use of the Code.
No Warranty: The Code is provided "as is," with no warranties of any kind.
By using the Code, Licensee agrees to these terms.

~~~~~
'''

from frame_extractor import FrameExtractor
from field_extractor import FieldExtractor
from interpolator import Interpolator
from utility import getInputVideos

##
## inputDir (str): Specify the location of input progressive video(s) (Use full
## 				   path). Please make sure this directory is created and contains
## 				   at least 1 progressive video.
##
inputDir = "PutInputDirHere"

##
## frameSamplePeriod (int): Controls how often a frame is extracted from the
## 						    input progressive video.
## 	-> Examples for a video with 25fps:
## 		1. If frameSamplePeriod = 1, all the frames in the video will be
## 		extracted.
## 		2. If frameSamplePeriod = 25, one frame will per extracted per second.
## 		3. If there are 100 frames in the video, and frameSamplePeriod = 125,
## 		then only one frame (the first frame) will be extracted.
##
frameSamplePeriod = 300

def main():
	inputVideos = getInputVideos(inputDir)

	frameExtractor = FrameExtractor(inputVideos, frameSamplePeriod)
	frameExtractor.Run()

	fieldExtractor = FieldExtractor(inputVideos)
	fieldExtractor.Run()

	interpolator = Interpolator(inputVideos)
	interpolator.Run()


main()
