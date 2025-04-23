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

from utility import FrameType, getFrameDir
import cv2

##
## FrameExtractor: Class dedicated to extracting frames as images from input videos
##
class FrameExtractor:

	##
	## FrameExtractor Constructor:
	## 	-> inputVideos: Array of Videos (Class - utility.Video) to extract
	# 					frames from				
	##	-> frameSamplePeriod: How often a frame is extracted from the video.
	##						  Example: If video is 30fps and this value = 30,
	##						  one frame will be extracted every second.			
	##
	def __init__(self, inputVideos, frameSamplePeriod):
		self.inputVideos = inputVideos
		self.frameSamplePeriod = frameSamplePeriod
  
	##
	## extractFrames: For given video, extracts and saves frames at
	##				  frameSamplePeriod rate.
	##
	def extractFrames(self, video):
     
		currentFrame = 0

		while (True):
	
			# Read one frame from the current video
			frameExists, frame = video.content.read()

			# If video has frames left to use
			if frameExists:
					
				# Writing the extracted frame to Folder
				frameDir = getFrameDir(FrameType.PROGRESSIVE, video.name, currentFrame)
				cv2.imwrite(frameDir, frame)

				# Moving to next frame using predetermined sample period rate
				currentFrame += self.frameSamplePeriod 
				video.content.set(1, currentFrame)

			else:
				break

	##
	## Run: Completes all frame extractions for videos in inputVideos array
	##
	def Run(self):

		for video in self.inputVideos:
      
			print('Extracting frames from ' + video.name)
			self.extractFrames(video)

			# Clean up: Release video data when done
			video.content.release()
   
		# Clean up: Destroy all opencv windows
		cv2.destroyAllWindows()