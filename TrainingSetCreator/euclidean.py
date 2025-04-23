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

from utility import Float3
import numpy as np


##
## getDistance: Determines the euclidean distance in three dimensions (RGB)
## 			  	between an interpolated pixel and the ground truth pixel. 
## 			  	https://en.wikipedia.org/wiki/Euclidean_distance
## 
## ip = interpolated pixel, gt = ground truth
##
def getDistance(ip : Float3, gt : Float3):
	return np.sqrt((gt.x - ip.x)**2 + (gt.y - ip.y)**2 + (gt.z - ip.z)**2)
