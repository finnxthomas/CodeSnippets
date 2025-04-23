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

from utility import FrameType, FrameDirectories, createDir, getFrameDir
import os
import cv2
import numpy as np

##
## FrameExtractor: Class dedicated to extracting fields from image frames
##
class FieldExtractor:

	##
	## FieldExtractor Constructor:
	## 	-> inputVideos: Array of Videos (Class - utility.Video) to use when
	## 					finding frames to extract fields from.
	## 	-> videoName: Name of the video where the current frame came from
	##
	def __init__(self, inputVideos):
		self.inputVideos = inputVideos
		self.videoName: str


	##
	## saveCompressedImage: Saves frame with removed fields as an Image
	##
	def saveCompressedImage(self, frameType, frameNumber, frameArray):

		# Get correct directory for frame to be saved into
		# ODD = Even fields removed, EVEN = Odd fields removed
		if (frameType == FrameType.ODD):
			compressedFrameDir = getFrameDir(FrameType.ODD, self.videoName, frameNumber)
		else:
			compressedFrameDir = getFrameDir(FrameType.EVEN, self.videoName, frameNumber)

		# Save the frame
		cv2.imwrite(compressedFrameDir, frameArray)
		#compressedFrame.save(compressedFrameDir)


	##
	## removeFields: Given a video and it's frame, removes both even and odd
	##			 	 fields and saves into two images (one with only even
	##			 	 frames, and one with only odd).
	##
	def removeFields(self, progressiveFrame):

		# Get the frame number from the input progressive frame 
		frameNumber = str(progressiveFrame[6:-4]) # REPLACE WITH REGULAR EXPRESSIONS

		# Open current frame and store as an array
		frame = cv2.imread(FrameDirectories.progressive + '/' + self.videoName + '/' + progressiveFrame, cv2.IMREAD_COLOR)
		frameArray = np.asarray(frame)

		# Remove odd fields and save the even field frame we created
		frameEvenFieldArray = self.removeOddFields(frameArray)
		self.saveCompressedImage(FrameType.EVEN, frameNumber, frameEvenFieldArray)

		# Remove even fields and save the odd field frame we created
		frameOddFieldArray = self.removeEvenFields(frameArray)
		self.saveCompressedImage(FrameType.ODD, frameNumber, frameOddFieldArray)


	##
	## removeEvenFields: Remove all even lines of array (even fields).
	## 					 Starts at row 0, skips every other line.
	##
	def removeEvenFields(self, frameArray):
		return np.delete(frameArray, list(range(0, frameArray.shape[0], 2)), axis=0)


	##
	## removeOddFields: Remove all odd lines of array (odd fields).
	## 				   	Starts at row 1, skips every other line.
	##
	def removeOddFields(self, frameArray):
		return np.delete(frameArray, list(range(1, frameArray.shape[0], 2)), axis=0)


	##
	## Run: For all videos in inputVideos array, remove even/odd fields of the
	## 	   frame.
	##
	def Run(self):

		for video in self.inputVideos:

			self.videoName = video.name 

			createDir(FrameDirectories.even + '/' + self.videoName)
			createDir(FrameDirectories.odd + '/' + self.videoName)

			print('Removing fields from ' + self.videoName)
			for progressiveFrame in os.listdir(FrameDirectories.progressive + '/' + self.videoName):
				self.removeFields(progressiveFrame)
