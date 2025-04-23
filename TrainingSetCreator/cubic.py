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

from multipledispatch import dispatch
from utility import Float3, Float4, Float4x4

g_cubic = Float4x4(
	 0.0,  2.0,  0.0,  0.0,
	-1.0,  0.0,  1.0,  0.0,
	 2.0, -5.0,  4.0, -1.0,
	-1.0,  3.0, -3.0,  1.0
)


##
## cubic: Given interpolation factor (t), return Float4 of values of cubic
##        calculation
##
@dispatch(float)
def cubic(t):
    t2 = t * t
    return Float4(0.5, 0.5 * t, 0.5 * t2, 0.5 * t2 * t) * g_cubic


##
## cubic: Given pixels a, b, c, d, and interpolation factor (t), return Float4
## 		  cubic calculation. http://en.wikipedia.org/wiki/Bicubic_interpolation
##
@dispatch(Float4, Float4, Float4, Float4, float)
def cubic(a, b, c, d, t):
	Tcubic = cubic(t)
	return Float4(
		Tcubic.x * a.x + Tcubic.y * b.x + Tcubic.z * c.x + Tcubic.w * d.x,
		Tcubic.x * a.y + Tcubic.y * b.y + Tcubic.z * c.y + Tcubic.w * d.y,
		Tcubic.x * a.z + Tcubic.y * b.z + Tcubic.z * c.z + Tcubic.w * d.z,
		Tcubic.x * a.w + Tcubic.y * b.w + Tcubic.z * c.w + Tcubic.w * d.w
	)
	

##
## cubic: Given pixels a, b, c, d, and interpolation factor (t), return Float3
## 		  cubic calculation. http://en.wikipedia.org/wiki/Bicubic_interpolation
##
@dispatch(Float3, Float3, Float3, Float3, float)
def cubic(a, b, c, d, t):
	Tcubic = cubic(t)
	return Float3(
		Tcubic.x * a.x + Tcubic.y * b.x + Tcubic.z * c.x + Tcubic.w * d.x,
		Tcubic.x * a.y + Tcubic.y * b.y + Tcubic.z * c.y + Tcubic.w * d.y,
		Tcubic.x * a.z + Tcubic.y * b.z + Tcubic.z * c.z + Tcubic.w * d.z
	)
