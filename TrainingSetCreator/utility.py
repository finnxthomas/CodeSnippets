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

import os
from enum import Enum
from dataclasses import dataclass, fields
import cv2

'''
Utility: Classes, functions, etc. for global use in Training Creator project.
'''

##
## Float4x4: Struct for 4x4 float matrix. See shape below.
##
## Shape:
## _11, _12, _13, _14;
## _21, _22, _23, _24;
## _31, _32, _33, _34;
## _41, _42, _43, _44;
##
@dataclass
class Float4x4:
	_11: float
	_12: float
	_13: float
	_14: float
	
	_21: float
	_22: float
	_23: float
	_24: float

	_31: float
	_32: float
	_33: float
	_34: float

	_41: float
	_42: float
	_43: float
	_44: float


##
## Float3: Struct for 3x1 float matrix
##
@dataclass
class Float3:
	x: float
	y: float
	z: float
	
	def __add__(self, o):
		return Float3(self.x + o.x, self.y + o.y, self.z + o.z)

	def __truediv__(self, i):
		return Float3(self.x/i, self.y/i, self.z/i)
	
	def __mul__(self, i):
		return Float3(self.x*i, self.y*i, self.z*i)

	def clamp(self):
		minimum = 0
		maximum = 255

		if (self.x < minimum):
			self.x = minimum
		elif (self.x > maximum):
			self.x = maximum

		if (self.y < minimum):
			self.y = minimum
		elif (self.y > maximum):
			self.y = maximum

		if (self.z < minimum):
			self.z = minimum
		elif (self.z > maximum):
			self.z = maximum

		return self


##
## Float4: Struct for 4x1 float matrix
##
@dataclass
class Float4:
	x: float
	y: float
	z: float
	w: float


	def __mul__(self, other):
		l = self
		r = other

		if isinstance(other, Float4x4):
			return Float4(

				l.x * r._11 + l.y * r._21 + l.z * r._31 + l.w * r._41,
				l.x * r._12 + l.y * r._22 + l.z * r._32 + l.w * r._42,
				l.x * r._13 + l.y * r._23 + l.z * r._33 + l.w * r._43,
				l.x * r._14 + l.y * r._24 + l.z * r._34 + l.w * r._44
			
			)
		else:
			raise TypeError("unsupported operand type(s) for +: '{}' and '{}'").format(self.__class__, type(other))


##
## FrameType: Enum for frame types.
## 	-> PROGRESSIVE = full progressive frame
## 	-> EVEN = progressive frame with odd fields removed
## 	-> ODD = progressive frame with even fields removed
##
class FrameType(Enum):
	PROGRESSIVE = 1,
	ODD = 2,
	EVEN = 3

##
## FrameDirectories: Struct of folder names for where frames will be saved
##
@dataclass
class FrameDirectories:
    progressive : str = "Progressive_Frames"
    even : str = "Even_Frames"
    odd : str = "Odd_Frames"


##
## Video: Class for creating video objects from progressive video input
##
class Video:
	def __init__(self, content, name):
		self.content = content
		self.name = name


##
## createDir: Creates a new directory given a path.
##
def createDir(path):
	try:
		if not os.path.exists(path):
			os.makedirs(path)
	except OSError:
		print ('Error: Creating directory')


##
## getFrameDir: Give FrameType, returns a directory for a new frame that we want
## 			   to save
##
def getFrameDir(frameType, videoName, frameNumber):
	if (frameType == FrameType.PROGRESSIVE):
		return FrameDirectories.progressive + '/' + videoName  + '/' + "Frame_" + str(frameNumber) + ".png"
	if (frameType == FrameType.EVEN):
		return FrameDirectories.even + '/' + videoName  + '/' + "Frame_" +  str(frameNumber) + ".png"
	if (frameType == FrameType.ODD):
		return FrameDirectories.odd + '/' + videoName  + '/' + "Frame_" +  str(frameNumber) + ".png"


##
## getInputVideos: From the input directory, load in the videos to extract
##				   frames from
##
def getInputVideos(inputVideosDir):
	inputVideos = []

	# Read the video(s) from specified path
	for inputVideo in os.listdir(inputVideosDir):

		# Avoid Mac's annoying file
		if input == ".DS_Store":
			continue

		# Create directories to put extracted Progressive Frames for current video
		createDir(FrameDirectories.progressive + '/' + inputVideo)

		# Open videos in OpenCV, save into object
		newInputVideo = Video(cv2.VideoCapture(inputVideosDir + '/' + inputVideo), inputVideo)			
		inputVideos.append(newInputVideo)

	return inputVideos
