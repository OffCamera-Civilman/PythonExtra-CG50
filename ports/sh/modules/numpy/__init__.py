"""Tiny NumPy-compatible subset for PythonUltra on the Casio fx-CG50.

This module is intentionally small and pure Python.  It targets common game,
geometry and classroom scripts rather than scientific-computing completeness.
Arrays are stored contiguously as Python values and support basic construction,
indexing, reshape and element-wise arithmetic.
"""

import math as _math

__version__ = "0.1.0-cg50"
pi = _math.pi
e = _math.e


def _product(shape):
    total = 1
    for value in shape:
        total *= int(value)
    return total


def _infer_shape(value):
    if isinstance(value, ndarray):
        return value.shape
    if not isinstance(value, (list, tuple)):
        return ()
    if len(value) == 0:
        return (0,)
    child = _infer_shape(value[0])
    for item in value[1:]:
        if _infer_shape(item) != child:
            raise ValueError("ragged arrays are not supported")
    return (len(value),) + child


def _flatten(value, output):
    if isinstance(value, ndarray):
        output.extend(value._data)
    elif isinstance(value, (list, tuple)):
        for item in value:
            _flatten(item, output)
    else:
        output.append(value)


def _normalize_shape(shape):
    if isinstance(shape, int):
        return (int(shape),)
    return tuple(int(v) for v in shape)


def _unflatten(data, shape, offset=0):
    if len(shape) == 0:
        return data[offset], offset + 1
    if len(shape) == 1:
        end = offset + shape[0]
        return list(data[offset:end]), end
    result = []
    for _ in range(shape[0]):
        item, offset = _unflatten(data, shape[1:], offset)
        result.append(item)
    return result, offset


class ndarray:
    def __init__(self, value, shape=None, dtype=None, _flat=False):
        self.dtype = dtype
        if _flat:
            self._data = list(value)
            self.shape = _normalize_shape(shape)
        elif isinstance(value, ndarray):
            self._data = list(value._data)
            self.shape = value.shape if shape is None else _normalize_shape(shape)
            if dtype is None:
                self.dtype = value.dtype
        else:
            inferred = _infer_shape(value)
            self._data = []
            _flatten(value, self._data)
            self.shape = inferred if shape is None else _normalize_shape(shape)
        if _product(self.shape) != len(self._data):
            raise ValueError("cannot reshape array of size %d into shape %r" % (len(self._data), self.shape))

    @property
    def size(self):
        return len(self._data)

    @property
    def ndim(self):
        return len(self.shape)

    def __len__(self):
        return self.shape[0] if self.shape else 0

    def __iter__(self):
        if self.ndim <= 1:
            return iter(self._data)
        stride = _product(self.shape[1:])
        rows = []
        for i in range(self.shape[0]):
            start = i * stride
            rows.append(ndarray(self._data[start:start + stride], self.shape[1:], self.dtype, True))
        return iter(rows)

    def __repr__(self):
        return "array(%r)" % self.tolist()

    def _flat_index(self, index):
        if not isinstance(index, tuple):
            index = (index,)
        if len(index) != self.ndim:
            raise IndexError("incorrect number of indices")
        offset = 0
        stride = self.size
        for axis, item in enumerate(index):
            dim = self.shape[axis]
            stride //= dim
            item = int(item)
            if item < 0:
                item += dim
            if item < 0 or item >= dim:
                raise IndexError("array index out of range")
            offset += item * stride
        return offset

    def __getitem__(self, index):
        if isinstance(index, slice):
            if self.ndim != 1:
                raise TypeError("slicing is supported for 1-D arrays")
            return ndarray(self._data[index], dtype=self.dtype)
        if isinstance(index, tuple) or self.ndim <= 1:
            return self._data[self._flat_index(index)]
        dim = self.shape[0]
        index = int(index)
        if index < 0:
            index += dim
        if index < 0 or index >= dim:
            raise IndexError("array index out of range")
        stride = _product(self.shape[1:])
        start = index * stride
        return ndarray(self._data[start:start + stride], self.shape[1:], self.dtype, True)

    def __setitem__(self, index, value):
        if isinstance(index, slice):
            if self.ndim != 1:
                raise TypeError("slicing is supported for 1-D arrays")
            replacement = value._data if isinstance(value, ndarray) else list(value)
            self._data[index] = replacement
            self.shape = (len(self._data),)
            return
        self._data[self._flat_index(index)] = value

    def tolist(self):
        result, _ = _unflatten(self._data, self.shape)
        return result

    def copy(self):
        return ndarray(self._data, self.shape, self.dtype, True)

    def flatten(self):
        return ndarray(self._data, (self.size,), self.dtype, True)

    def reshape(self, *shape):
        if len(shape) == 1 and isinstance(shape[0], (tuple, list)):
            shape = tuple(shape[0])
        shape = list(int(v) for v in shape)
        missing = None
        known = 1
        for i, dim in enumerate(shape):
            if dim == -1:
                if missing is not None:
                    raise ValueError("only one unknown dimension is allowed")
                missing = i
            else:
                known *= dim
        if missing is not None:
            if known == 0 or self.size % known:
                raise ValueError("cannot infer reshape dimension")
            shape[missing] = self.size // known
        if _product(shape) != self.size:
            raise ValueError("cannot reshape array")
        return ndarray(self._data, tuple(shape), self.dtype, True)

    def astype(self, converter):
        return ndarray([converter(v) for v in self._data], self.shape, converter, True)

    def _binary(self, other, operation):
        if isinstance(other, ndarray):
            if self.shape != other.shape:
                raise ValueError("array shapes must match")
            values = [operation(a, b) for a, b in zip(self._data, other._data)]
        else:
            values = [operation(a, other) for a in self._data]
        return ndarray(values, self.shape, self.dtype, True)

    def __add__(self, other):
        return self._binary(other, lambda a, b: a + b)

    __radd__ = __add__

    def __sub__(self, other):
        return self._binary(other, lambda a, b: a - b)

    def __rsub__(self, other):
        return ndarray([other - a for a in self._data], self.shape, self.dtype, True)

    def __mul__(self, other):
        return self._binary(other, lambda a, b: a * b)

    __rmul__ = __mul__

    def __truediv__(self, other):
        return self._binary(other, lambda a, b: a / b)

    def __rtruediv__(self, other):
        return ndarray([other / a for a in self._data], self.shape, self.dtype, True)

    def __neg__(self):
        return ndarray([-a for a in self._data], self.shape, self.dtype, True)

    def sum(self):
        return sum(self._data)

    def mean(self):
        return sum(self._data) / self.size if self.size else 0

    def min(self):
        return min(self._data)

    def max(self):
        return max(self._data)


def array(value, dtype=None):
    return ndarray(value, dtype=dtype)


def asarray(value, dtype=None):
    if isinstance(value, ndarray) and (dtype is None or dtype == value.dtype):
        return value
    return ndarray(value, dtype=dtype)


def zeros(shape, dtype=float):
    shape = _normalize_shape(shape)
    return ndarray([dtype(0)] * _product(shape), shape, dtype, True)


def ones(shape, dtype=float):
    shape = _normalize_shape(shape)
    return ndarray([dtype(1)] * _product(shape), shape, dtype, True)


def full(shape, value, dtype=None):
    shape = _normalize_shape(shape)
    if dtype is not None:
        value = dtype(value)
    return ndarray([value] * _product(shape), shape, dtype, True)


def arange(start, stop=None, step=1, dtype=None):
    if stop is None:
        start, stop = 0, start
    if step == 0:
        raise ValueError("step must not be zero")
    values = []
    current = start
    if step > 0:
        while current < stop:
            values.append(dtype(current) if dtype else current)
            current += step
    else:
        while current > stop:
            values.append(dtype(current) if dtype else current)
            current += step
    return ndarray(values, dtype=dtype)


def linspace(start, stop, num=50):
    num = int(num)
    if num <= 0:
        return ndarray([])
    if num == 1:
        return ndarray([float(start)])
    step = (stop - start) / (num - 1)
    return ndarray([start + step * i for i in range(num)])


def reshape(value, newshape):
    return asarray(value).reshape(newshape)


def concatenate(values):
    output = []
    for value in values:
        output.extend(asarray(value).flatten()._data)
    return ndarray(output)


def dot(a, b):
    a = asarray(a)
    b = asarray(b)
    if a.ndim == 1 and b.ndim == 1:
        if a.size != b.size:
            raise ValueError("vectors must have the same length")
        return sum(x * y for x, y in zip(a._data, b._data))
    if a.ndim == 2 and b.ndim == 1:
        rows, cols = a.shape
        if cols != b.size:
            raise ValueError("shapes are not aligned")
        result = []
        for row in range(rows):
            start = row * cols
            result.append(sum(a._data[start + col] * b._data[col] for col in range(cols)))
        return ndarray(result)
    raise NotImplementedError("dot currently supports 1-D dot and 2-D by 1-D")


def _unary(value, function):
    if isinstance(value, ndarray):
        return ndarray([function(v) for v in value._data], value.shape, value.dtype, True)
    return function(value)


def sqrt(value):
    return _unary(value, _math.sqrt)


def sin(value):
    return _unary(value, _math.sin)


def cos(value):
    return _unary(value, _math.cos)


def tan(value):
    return _unary(value, _math.tan)


def abs(value):
    return _unary(value, __builtins__.abs if hasattr(__builtins__, "abs") else lambda x: -x if x < 0 else x)


def sum(value):
    if isinstance(value, ndarray):
        return value.sum()
    total = 0
    for item in value:
        total += item
    return total


def mean(value):
    return asarray(value).mean()


def amin(value):
    return asarray(value).min()


def amax(value):
    return asarray(value).max()


float32 = float
float64 = float
int8 = int
int16 = int
int32 = int
int64 = int
uint8 = int
uint16 = int
uint32 = int
bool_ = bool
