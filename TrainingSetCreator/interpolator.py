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

from utility import FrameDirectories, Float3, createDir
from cubic import cubic
import euclidean
import os
from dataclasses import dataclass
from enum import Enum
import numpy as np
import cv2
from progress.bar import Bar

class PixelType(Enum):
	LEFT = 0,
	RIGHT = 1,
	VERTICAL = 2,
	HORIZONTAL = 3

@dataclass
class CubicInput:
	a : Float3
	b : Float3
	c : Float3
	d : Float3

@dataclass
class InterpolatedValues:
	left : Float3
	right : Float3
	vertical : Float3
	horizontal: Float3

@dataclass
class DistanceValues:
	left : float
	right : float
	vertical : float
	horizontal: float

	##
	## getMin: Returns the minimum euclidean distance to determine the best
	##		   interpolation type
	##
	def getMin(self):

		min = self.left
		if (min > self.right):
			min = self.right
		if (min > self.vertical):
			min = self.vertical
		if (min > self.horizontal):
			min = self.horizontal

		return min


##
## Interpolator: Class dedicated to interpolating pixel values, and comparing to
## 				 the progressive ground truth to generate output result.
##
class Interpolator():

	##
	## Interpolator Constructor:
	## 	-> inputVideos: Array of Videos (Class - utility.Video) to frames from
	## 	-> videoName: Name of the video where the current frame came from
	## 	-> numCol/Row: Number of columns, rows (width, height) of current frame
	## 	-> even/oddPixelFrame: Array of size (numCol, numRow, 3) of pixels
	## 	-> interpFactor: Variable for interpolation calculation.
	## 	-> progressBar: Visual progress bar in terminal for interpolation
	## 					calculation of a single frame
	##
	def __init__(self, inputVideos):
		self.inputVideos = inputVideos
		self.videoName : str
		self.numOddCol: int
		self.numOddRow: int
		self.numEvenCol = int
		self.numEvenRow = int
		self.oddPixelFrame : np.array
		self.evenPixelFrame : np.array
		self.interpFactor = float(0.5)
		self.progressBar: Bar


	##
	## getOddPixel: Given values row, col of oddPixelFrame, returns a 3x1 Float3
	## 			   	matrix to be used in cubic calculation.
	##
	def getOddPixel(self, row, col):
		return Float3(
			self.oddPixelFrame[row][col][0],
			self.oddPixelFrame[row][col][1],
			self.oddPixelFrame[row][col][2]
		)


	##
	## getGroundTruth: Given values row, col of even PixelFrame, returns a 3x1
	##				   Float3 matrix to be used in PixelType calculation.
	##
	def getGroundTruth(self, row, col):
		return Float3(
			self.evenPixelFrame[row][col][0],
			self.evenPixelFrame[row][col][1],
			self.evenPixelFrame[row][col][2]
		)


	##
	## getVerticalInterpPositions: Given (row, col) of an odd field image, do
	## 							   vertical interpolation to find a, b, c, d
	## 							   positions for cubic calculation of even field
	## 							   pixel right above it.
	##
	def getVerticalInterpPositions(self, row, col):
		a = self.getOddPixel(row - 2, col)
		b = self.getOddPixel(row - 1, col)
		c = self.getOddPixel(row, col)
		d = self.getOddPixel(row + 1, col)

		return CubicInput(a, b, c, d)


	##
	## getLeftInterpPositions: Given (row, col) of an odd field image, do left
	## 						   diagonal interpolation to find a, b, c, d
	## 						   positions for cubic calculation of even field
	## 						   pixel right above it.
	##
	def getLeftInterpPositions(self, row, col):

		a = self.getOddPixel(row-2, col-2)
		b = self.getOddPixel(row - 1, col - 1)
		c = self.getOddPixel(row, col + 1)
		d = self.getOddPixel(row + 1, col + 2)

		return CubicInput(a, b, c, d)


	##
	## getRightInterpPositions: Given (row, col) of an odd field image, do
	## 						    right diagonal interpolation to find a, b, c,
	## 						    d positions for cubic calculation of even
	## 						    field pixel right above it.
	##
	def getRightInterpPositions(self, row, col):
		a = self.getOddPixel(row-2, col+2)
		b = self.getOddPixel(row-1, col+1)
		c = self.getOddPixel(row, col-1)
		d = self.getOddPixel(row + 1, col-2)

		return CubicInput(a, b, c, d)


	##
	## getHorizontalInterpPositions: Given (row, col) of an odd field image, do
	## 								 right diagonal interpolation to find a1,
	## 								 b1, c1, d1, and a2, b2, c2, d2 positions
	## 								 for cubic calculation of even field pixel
	## 								 right above it.
	##
	def getHorizontalInterpPositions(self, row, col):
		a1 = self.getOddPixel(row - 1, col - 2)
		b1 = self.getOddPixel(row - 1, col - 1)
		c1 = self.getOddPixel(row - 1, col + 1)
		d1 = self.getOddPixel(row - 1, col + 2)

		a2 = self.getOddPixel(row, col - 2)
		b2 = self.getOddPixel(row, col - 1)
		c2 = self.getOddPixel(row, col + 1)
		d2 = self.getOddPixel(row, col + 2)

		return CubicInput(a1, b1, c1, d1), CubicInput(a2, b2, c2, d2)


	##
	## getInterpolationValues: Given left, right, vertical, and horizontal
	## 						   pixel input values (CubicInput) do the cubi
	## 						   calculation to determine the interpolated
	## 						   values for the current position.
	##
	def getInterpolationValues(self, left, right, vertical, horizontal):
		return InterpolatedValues(

			cubic(left.a, left.b, left.c, left.d, self.interpFactor),

			cubic(right.a, right.b, right.c, right.d, self.interpFactor),

			cubic(vertical.a, vertical.b, vertical.c, vertical.d, self.interpFactor),

			(cubic(horizontal[0].a, horizontal[0].b, horizontal[0].c, horizontal[0].d, self.interpFactor)
			+ cubic(horizontal[1].a, horizontal[1].b, horizontal[1].c, horizontal[1].d, self.interpFactor))/2
		)


	##
	## getDistanceValues: Given the InterpolatedValues and the groundTruth, the
	## 				      accuracy of the left, right, vertical, and horizontal
	## 				      interpolated values.
	##
	def getDistanceValues(self, interpolatedValues : InterpolatedValues, groundTruth : Float3):

		leftResult = euclidean.getDistance(interpolatedValues.left, groundTruth)
		rightResult = euclidean.getDistance(interpolatedValues.right, groundTruth)
		verticalResult = euclidean.getDistance(interpolatedValues.vertical, groundTruth)
		horizontalResult = euclidean.getDistance(interpolatedValues.horizontal, groundTruth)

		return DistanceValues(leftResult, rightResult, verticalResult, horizontalResult)


	##
	## saveResult: Saves the result of the current pixel interpolation
	##
	# def saveResult(self, row, col, result):
	# 	return


	##
	## determineSmallestDistance: Determines the bset interpolation result and
	##							  returns as that pixel type structure.
	##
	def determineSmallestDistance(self, groundTruthResult : DistanceValues):

		min = groundTruthResult.getMin()

		if (min == groundTruthResult.left):
			return PixelType.LEFT

		if (min == groundTruthResult.right):
			return PixelType.RIGHT

		if (min == groundTruthResult.vertical):
			return PixelType.VERTICAL

		if (min == groundTruthResult.horizontal):
			return PixelType.HORIZONTAL


	##
	## createResultArray: Creates a numpy array (frame) of all interpolation 
	## 					  results.
	##
	def createResultArrays(self):
		self.leftArray = np.zeros((self.numEvenRow, self.numEvenCol, 3), dtype=np.uint8, order='C')
		self.rightArray = np.zeros((self.numEvenRow, self.numEvenCol, 3), dtype=np.uint8, order='C')
		self.verticalArray = np.zeros((self.numEvenRow, self.numEvenCol, 3), dtype=np.uint8, order='C')
		self.horizontalArray = np.zeros((self.numEvenRow, self.numEvenCol, 3), dtype=np.uint8, order='C')
		self.bestArray = np.zeros((self.numEvenRow, self.numEvenCol, 3), dtype=np.uint8, order='C')


	##
	## displayCurrentResult: Displays the interpolation results as images.
	##
	def displayCurrentResult(self, frameNumber):

		rows = self.numEvenRow + self.numOddRow
		cols = self.numOddCol

		left = np.zeros((rows, cols, 3), dtype=np.uint8, order='C')
		right = np.zeros((rows, cols, 3), dtype=np.uint8, order='C')
		vertical = np.zeros((rows, cols, 3), dtype=np.uint8, order='C')
		horizontal = np.zeros((rows, cols, 3), dtype=np.uint8, order='C')
		best = np.zeros((rows, cols, 3), dtype=np.uint8, order='C')

		evenCount = 0
		oddCount = 0
		for row in range(rows):
			if (row%2==0):
				left[row] = self.leftArray[evenCount]
				right[row] = self.rightArray[evenCount]
				vertical[row] = self.verticalArray[evenCount]
				horizontal[row] = self.horizontalArray[evenCount]
				best[row] = self.bestArray[evenCount]
				evenCount += 1
			else:
				left[row] = self.oddPixelFrame[oddCount]*255
				right[row] = self.oddPixelFrame[oddCount]*255
				vertical[row] = self.oddPixelFrame[oddCount]*255
				horizontal[row] = self.oddPixelFrame[oddCount]*255
				best[row] = self.oddPixelFrame[oddCount]*255
				oddCount += 1


		# Put Text on Image so we know what interp type it is
		font = cv2.FONT_HERSHEY_COMPLEX
		cv2.putText(left, 'left', (10,100), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
		cv2.putText(right, 'right', (10,100), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
		cv2.putText(vertical, 'vertical', (10,100), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
		cv2.putText(horizontal, 'horizontal', (10,100), font, 1, (0, 255, 0), 2, cv2.LINE_AA)

		# Concat left/right/horizontal/vertical images
		leftright = np.concatenate((left, right), axis=1)
		verthorz = np.concatenate((vertical, horizontal), axis=1)
		lrvh = np.concatenate((leftright, verthorz), axis = 0)

		cv2.namedWindow("camera",1)
		cv2.imshow("Interpolation Results", lrvh)
		cv2.waitKey(0)
		cv2.imshow('Best (per pixel)', best)
		cv2.waitKey(0)
		cv2.destroyAllWindows()
		cv2.waitKey(1)


	##
	## getPixels: Returns pixels of an Float3 matrix as an array.
	##
	def getPixels(self, pixel : Float3):
		return [pixel.x, pixel.y, pixel.z]


	##
	## setInterpolationArray: Create result arrays and set the best result array
	##
	def setInterpolationArrays(self, row, col, interpolations : InterpolatedValues, bestDistanceResult):

		self.leftArray[row][col] = np.round(self.getPixels((interpolations.left*255).clamp()))
		self.rightArray[row][col] = np.round(self.getPixels((interpolations.right*255).clamp()))
		self.verticalArray[row][col] = np.round(self.getPixels((interpolations.vertical*255).clamp()))
		self.horizontalArray[row][col] = np.round(self.getPixels((interpolations.horizontal*255).clamp()))

		if (bestDistanceResult == PixelType.LEFT):
			self.bestArray[row][col] = self.leftArray[row][col]
		if (bestDistanceResult == PixelType.RIGHT):
			self.bestArray[row][col] = self.rightArray[row][col]
		if (bestDistanceResult == PixelType.VERTICAL):
			self.bestArray[row][col] = self.verticalArray[row][col]
		if (bestDistanceResult == PixelType.HORIZONTAL):
			self.bestArray[row][col] = self.horizontalArray[row][col]


	##
	## doFrameInterpolation: For each removed pixel in the current frame,
	## 						 interpolates in four ways (left, right, vert, horiz)
	## 						 and determines the accuracy of each.
	## 						 (NOT DONE: Saving results)
	##
	def doFrameInterpolation(self):

		interpolationResults = np.zeros((self.numEvenRow, self.numEvenCol), dtype=InterpolatedValues, order='C')
		distanceValues = np.zeros((self.numEvenRow, self.numEvenCol), dtype=DistanceValues, order='C')
		self.createResultArrays()

		# We don't want to iterate over every pixel since edges will be compromised.
		# Instead, lets start further inwards such that we created a 2 pixel border.
		for row in range(2, self.numOddRow-1):
			for col in range (2, self.numOddCol-2):

				interpolations = self.getInterpolationValues(
					self.getLeftInterpPositions(row, col),
					self.getRightInterpPositions(row, col),
					self.getVerticalInterpPositions(row, col),
					self.getHorizontalInterpPositions(row, col)
				)

				interpolationResults[row][col] = interpolations
				groundTruth = self.getGroundTruth(row, col)
				distanceValues[row][col] = self.getDistanceValues(interpolations, groundTruth)
				self.setInterpolationArrays(row, col, interpolations, self.determineSmallestDistance(distanceValues[row][col]))

				# TO DO : SAVE RESULT AS BINARY

				self.progressBar.next()

		self.progressBar.finish()


	##
	## processCurrentFrame: Open the frame image and extract relevant data
	## 					    including size of frame and pixels values in an array
	## 					    form.
	##
	def processCurrentFrame(self, oddFrame, evenFrame):

		# Open current image frames
		oddImage = cv2.imread(
			FrameDirectories.odd + '/' + self.videoName + '/' + oddFrame, cv2.IMREAD_COLOR
		)
		evenImage = cv2.imread(
			FrameDirectories.even + '/' + self.videoName + '/' + evenFrame, cv2.IMREAD_COLOR
		)

		# Get height, width of odd image frame
		self.numOddRow = oddImage.shape[0]
		self.numOddCol = oddImage.shape[1]

		# Get height, width of even image frame (ground truth)
		self.numEvenRow = evenImage.shape[0]
		self.numEvenCol = evenImage.shape[1]

		# Open odd and even images as array so that we can access the pixel values
		# if (oddImage.shape[2] == 3):
		# Normalize pixel values
		self.oddPixelFrame = np.asarray(oddImage)/255
		self.evenPixelFrame = np.asarray(evenImage)/255


	##
	## Run: Interpolate pixels and get ground truth results for all the
	## 	   extracted frames.
	##
	def Run(self):

		for video in self.inputVideos:

			# Get video name and location of its odd + even field frames
			self.videoName = video.name

			oddFrames = os.listdir(FrameDirectories.odd + '/' + self.videoName)
			evenFrames = os.listdir(FrameDirectories.even + '/' + self.videoName)

			print("Interpolating frames from " + self.videoName)
			for i in range(len(oddFrames)):

				self.processCurrentFrame(oddFrames[i], evenFrames[i])

				# Graphic Bar in terminal for visual progress of interpolation
				self.progressBar = Bar(
					"Processing " + oddFrames[i],
					max = (self.numOddRow-3)*(self.numOddCol-4),
					suffix='%(percent)d%%'
				)

				self.doFrameInterpolation()

				self.displayCurrentResult(oddFrames[i][6:-4]) # REPLACE WITH REGULAR EXPRESSION
