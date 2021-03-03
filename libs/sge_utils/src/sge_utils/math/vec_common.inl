
/// Returns only the i-th axis being non-zero with value @axisLen
static SELF_TYPE getAxis(const unsigned int& axisIndex, const DATA_TYPE& axisLen = DATA_TYPE(1.0))
{
	SELF_TYPE result;
	for(int t = 0; t < NUM_ELEMS; ++t)
	{
		result[t] = (DATA_TYPE)(0.0);
	}
	result[axisIndex] = axisLen;
	return result;
}

static SELF_TYPE getY(const DATA_TYPE& axisLen = DATA_TYPE(1.0))
{
	SELF_TYPE result;
	for(int t = 0; t < NUM_ELEMS; ++t)
	{
		result[t] = (DATA_TYPE)(0.0);
	}
	result[1] = axisLen;
	return result;
}

/// Returns a vector containing only zeroes.
static SELF_TYPE getZero()
{
	SELF_TYPE result;
	for(int t = 0; t < NUM_ELEMS; ++t)
	{
		result[t] = DATA_TYPE(0.0);
	}

	return result;
}

/// Sets the data of the vector form assumingly properly sized c-array.
void set_data(const DATA_TYPE* const pData) {
	for(int t = 0; t < NUM_ELEMS; ++t) {
		data[t] = pData[t];
	}
}

// Less and greater operators.
friend bool operator<(const SELF_TYPE& a, const SELF_TYPE& b)
{
	for(int t = 0; t < NUM_ELEMS; ++t) {
		if(a[t] < b[t]) return true;
	}

	return false;
}

friend bool operator<=(const SELF_TYPE& a, const SELF_TYPE& b)
{
	for(int t = 0; t < NUM_ELEMS; ++t) {
		if(a[t] <= b[t]) return true;
	}

	return false;
}


friend bool operator>(const SELF_TYPE& a, const SELF_TYPE& b)
{
	for(int t = 0; t < NUM_ELEMS; ++t) {
		if(a[t] > b[t]) return true;
	}

	return false;
}

friend bool operator>=(const SELF_TYPE& a, const SELF_TYPE& b)
{
	for(int t = 0; t < NUM_ELEMS; ++t) {
		if(a[t] >= b[t]) return true;
	}

	return false;
}

// Indexing operators.
DATA_TYPE&       operator[](const unsigned int t)		{ return data[t]; }
const DATA_TYPE& operator[](const unsigned int t) const	{ return data[t]; }

// Operator == and != implemented by direct comparison.
bool operator==(const SELF_TYPE& v) const
{
	bool result = true;
	for(unsigned t = 0; t < NUM_ELEMS; ++t)
	{
		result = result && (data[t] == v[t]);
		//not autovect friendly,
		//if(data[t] != v[t]) return false;
	}

	return result;
}

bool operator!=(const SELF_TYPE& v) const
{
	return !((*this) == v);
}

// Unary operators - +
SELF_TYPE operator-() const
{
	SELF_TYPE result;
	for(unsigned int t = 0; t < NUM_ELEMS; ++t)
	{
		result[t] = - data[t];
	}
	return result;
}

SELF_TYPE operator+() const
{
	return *this;
}

// vec + vec
SELF_TYPE& operator+=(const SELF_TYPE& v)
{
	for(unsigned int t = 0; t < NUM_ELEMS; ++t)
	{
		data[t] += v[t];
	}
	return *this;
}

SELF_TYPE operator+(const SELF_TYPE& v) const
{
	SELF_TYPE r(*this);
	r += v;
	return r;
}

// vec - vec
SELF_TYPE& operator-=(const SELF_TYPE& v)
{
	for(unsigned int t = 0; t < NUM_ELEMS; ++t)
	{
		data[t] -= v[t];
	}
	return *this;
}


SELF_TYPE operator-(const SELF_TYPE& v) const
{
	SELF_TYPE r(*this);
	r -= v;
	return r;
}

// Vector * Scalar (and vice versa)
SELF_TYPE& operator*=(const DATA_TYPE& s)
{
	for(unsigned int t = 0; t < NUM_ELEMS; ++t)
	{
		data[t] *= s;
	}
	return *this;
}

SELF_TYPE operator*(const DATA_TYPE& s) const
{
	SELF_TYPE r(*this);
	r *= s;
	return r;
}

friend SELF_TYPE operator*(const DATA_TYPE& s, const SELF_TYPE& v)
{
	return v * s;
}


//Vector / Scalar
SELF_TYPE& operator/=(const DATA_TYPE& s)
{
	for(unsigned int t = 0; t < NUM_ELEMS; ++t)
	{
		data[t] /= s;
	}
	return *this;
}

SELF_TYPE operator/(const DATA_TYPE& s) const
{
	SELF_TYPE r(*this);
	r /= s;
	return r;
}

// Vector * Vector
SELF_TYPE& operator*=(const SELF_TYPE& v)
{
	for(unsigned int t = 0; t < NUM_ELEMS; ++t)
	{
		data[t] *= v.data[t];
	}
	return *this;
}

SELF_TYPE operator*(const SELF_TYPE& s) const
{
	SELF_TYPE r(*this);
	r *= s;
	return r;
}

// Vector / Vector
SELF_TYPE& operator/=(const SELF_TYPE& v)
{
	for(unsigned int t = 0; t < NUM_ELEMS; ++t)
	{
		data[t] /= v.data[t];
	}
	return *this;
}

SELF_TYPE operator/(const SELF_TYPE& s) const
{
	SELF_TYPE r(*this);
	r /= s;
	return r;
}

/// Performs a component-wise division of two vectors.
/// Assumes that non of the elements in @b are 0
friend SELF_TYPE cdiv(const SELF_TYPE& a, const SELF_TYPE& b)
{
	SELF_TYPE r;
	for(unsigned int t = 0; t < NUM_ELEMS; ++t)
	{
		r[t] = a[t] / b[t];
	}
	return r;
}

/// Returns a sum of all elements in the vector.
DATA_TYPE hsum() const
{
	DATA_TYPE r = data[0];
	for(unsigned int t = 1; t < NUM_ELEMS; ++t)
	{
		r += data[t];
	}
	return r;
}

friend DATA_TYPE hsum(const SELF_TYPE& v)
{
	return v.hsum();
}

/// Computes the dot product between two vectors.
DATA_TYPE dot(const SELF_TYPE& v) const
{
	DATA_TYPE r = data[0] * v[0];
	for(unsigned int t = 1; t < NUM_ELEMS; ++t)
	{
		r += data[t] * v[t];
	}
	return r;
}

friend DATA_TYPE dot(const SELF_TYPE& a, const SELF_TYPE& b)
{
	return a.dot(b);
}

/// Computes the length of the vector.
DATA_TYPE lengthSqr() const
{
	return dot(*this);
}

DATA_TYPE length() const
{
	// [TODO]
	return sqrtf(lengthSqr());
}

friend DATA_TYPE length(const SELF_TYPE& v)
{
	return v.length();
}

/// Computes the unsigned-volume of a cube.
DATA_TYPE volume() const {
	DATA_TYPE r = abs(data[0]);
	for(unsigned int t = 1; t < NUM_ELEMS; ++t) {
		r *= abs(data[t]);
	}

	return r;
}


/// Returns the normalized vector (with length 1.f).
/// Assumes that the vector current size is not 0.
SELF_TYPE normalized() const
{
	const DATA_TYPE invLength = DATA_TYPE(1.0) / length();

	SELF_TYPE result;
	for(unsigned int t = 0; t < SELF_TYPE::NUM_ELEMS; ++t)
	{
		result[t] = data[t] * invLength;
	}

	return result;
}

friend SELF_TYPE normalized(const SELF_TYPE& v)
{
	return v.normalized();
}

/// Returns the normalized vector (with length 1.f).
/// If the size of the vector is 0, the zero vector is returned.
SELF_TYPE normalized0() const
{
	if(lengthSqr() < 1e-6f) return SELF_TYPE(0);

	const DATA_TYPE invLength = DATA_TYPE(1.0) / length();

	SELF_TYPE result;
	for(unsigned int t = 0; t < SELF_TYPE::NUM_ELEMS; ++t)
	{
		result[t] = data[t] * invLength;
	}

	return result;
}

friend SELF_TYPE normalized0(const SELF_TYPE& v)
{
	return v.normalized0();
}

/// Interpolates two vectors with the a speed (defined in units).
friend SELF_TYPE speedLerp(const SELF_TYPE& a, const SELF_TYPE& b, const DATA_TYPE speed, const DATA_TYPE epsilon = 1e-6f)
{
	SELF_TYPE const diff = b - a;
	DATA_TYPE const diffLen = diff.length();

	// if the two points are too close together just return the target point.
	if(diffLen < epsilon) {
		return b;
	}

	
	float k = speed / diffLen;

	if(k > 1.f) {
		k = 1.f;
	}
	
	return a + k*diff;
	
}


/// Computes the reflected vector based on the normal specified. Assuming IOR=1
SELF_TYPE reflect(const SELF_TYPE& normal) const
{
	return (*this) + DATA_TYPE(2.0) * dot(normal) * normal;
}

friend DATA_TYPE reflect(const SELF_TYPE& incident, const SELF_TYPE& normal)
{
	return incident.reflect(normal);
}

/// Computes the refracted vector based on the normal specified. Assuming IOR=1
SELF_TYPE refract(const SELF_TYPE& normal, const DATA_TYPE& eta) const
{
	const DATA_TYPE one(1.0);
	const DATA_TYPE zero(0.0);

	const DATA_TYPE p = Dot(normal);
	const DATA_TYPE k = one - eta*eta * (one -  p*p);

	if (k < zero) return SELF_TYPE(zero);
	else return (*this) * eta - (eta * p + sqrt(k)) * normal;
}

friend SELF_TYPE refract(const SELF_TYPE& inc, const SELF_TYPE& n, DATA_TYPE& eta)
{
	return inc.refract(n, eta);
}

/// Computes the distance between two vectors.
DATA_TYPE distance(const SELF_TYPE& other) const
{
	return (*this - other).length();
}

friend DATA_TYPE distance(const SELF_TYPE& a, const SELF_TYPE& b)
{
	return a.distance(b);
}

/// Rentusn a vector containing min/max components from each vector.
SELF_TYPE ComponentMin(const SELF_TYPE& other) const
{
	SELF_TYPE result;

	for(unsigned int t = 0; t < SELF_TYPE::NUM_ELEMS; ++t) {
		result.data[t] = minOf(data[t], other.data[t]);
	}

	return result;
}

friend SELF_TYPE component_min(const SELF_TYPE& a, const SELF_TYPE& b)
{
	return a.ComponentMin(b);
}

SELF_TYPE ComponentMax(const SELF_TYPE& other) const
{
	SELF_TYPE result;

	for(unsigned int t = 0; t < SELF_TYPE::NUM_ELEMS; ++t) {
		result.data[t] = maxOf(data[t], other.data[t]);
	}

	return result;
}

friend SELF_TYPE component_max(const SELF_TYPE& a, const SELF_TYPE& b)
{
	return a.ComponentMax(b);
}


/// Returns the min/max component in the vector.
DATA_TYPE componentMin() const {
	DATA_TYPE retval = data[0];

	for(int t = 1; t < SELF_TYPE::NUM_ELEMS; ++t) {
		if(data[t] < retval) {
			retval = data[t];
		}
	}

	return retval;
}

friend SELF_TYPE component_min(const SELF_TYPE& v)
{
	return v.componentMin();
}

DATA_TYPE componentMinAbs() const
{
	DATA_TYPE retval = std::abs(data[0]);

	for(int t = 1; t < SELF_TYPE::NUM_ELEMS; ++t) {
		float abs = std::abs(data[t]);
		if(abs < retval) {
			retval = abs;
		}
	}

	return retval;
}

DATA_TYPE componentMax() const
{
	DATA_TYPE retval = data[0];

	for(int t = 1; t < SELF_TYPE::NUM_ELEMS; ++t) {
		if(data[t] > retval) {
			retval = data[t];
		}
	}

	return retval;
}

friend SELF_TYPE component_max(const SELF_TYPE& v)
{
	return v.componentMax();
}

DATA_TYPE componentMaxAbs() const
{
	DATA_TYPE retval = std::abs(data[0]);

	for(int t = 1; t < SELF_TYPE::NUM_ELEMS; ++t) {
		float abs = std::abs(data[t]);
		if(abs > retval) {
			retval = abs;
		}
	}

	return retval;
}

SELF_TYPE comonents_sorted_inc() const
{
	SELF_TYPE retval = *this;

	for(int t = 0  ; t < SELF_TYPE::NUM_ELEMS; ++t)
	for(int k = t+1; k < SELF_TYPE::NUM_ELEMS; ++k)
	{
		if(retval.data[t] > retval.data[k])
		{
			DATA_TYPE x = retval.data[t];
			retval.data[t] = retval.data[k];
			retval.data[k] = x;
		}
	}

	return retval;
}