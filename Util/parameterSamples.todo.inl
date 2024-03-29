/////////////////////
// Animation stuff //
/////////////////////
#include <math.h>

template <class Vector>
void ParameterSamples<Vector>::setCurrentValue(double t,int type){
	double b0,b1,b2,b3;
	int i0;
	switch (type){
		case LINEAR:
			i0 = floor(t * this->count);
			if (t != 1)
				currentValue = samples[i0] * (1 - t) + samples[i0 + 1] * t;
			else
				currentValue = samples[i0];
			break;
		case CATMULL_ROM:
			b0 = (1.0 / 6.0) * pow(1 - t, 3);
			b1 = (1.0 / 6.0) * (3 * pow(t,3) - 6 * pow(t,2) + 4);
			b2 = (1.0 / 6.0) * (-3 * pow(t,3) + 3 * pow(t,2) + 3 * t + 1);
			b3 = (1.0 / 6.0) * pow(t, 3);

		    i0 = floor(t * (this->count - 4));
			currentValue=samples[i0] * b0 + samples[i0 + 1] * b1 + samples[i0 + 2] * b2 + samples[i0 + 3] * b3;
			break;
		case UNIFORM_CUBIC_B_SPLINE:
			b0 = -0.5 * t + pow(t,2) - 0.5 * pow(t,3);
			b1 = 1 - 2.5 * pow(t,2) + 1.5 * pow(t,3);
			b2 = 0.5 * t + 2 * pow(t,2) -1.5 * pow(t,3);
			b3 = -0.5 * pow(t,2) + 0.5 * pow(t,3);

			i0 = floor(t * (this->count - 4));
			currentValue=samples[i0] * b0 + samples[i0 + 1] * b1 + samples[i0 + 2] * b2 + samples[i0 + 3] * b3;
			break;
	};
}
