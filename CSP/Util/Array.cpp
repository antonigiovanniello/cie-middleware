#include "array.h"


ByteArray::ByteArray() {
	_data = nullptr;
	_size = 0;
}

ByteArray::ByteArray(uint8_t* data, size_t size) {
	_data = data;
	_size = size;
}

ByteDynArray::ByteDynArray(ByteDynArray &&src) {
	_data = src._data;
	_size = src._size;
	src._data = nullptr;
	src._size = 0;
}

ByteArray::ByteArray(const ByteArray& ba, size_t start) {
	if (start>ba.size())
		throw std::runtime_error("Array derivato troppo grande");
	_size = ba.size() - start;
	_data = ba._data + start;
}

ByteArray::ByteArray(const ByteArray& ba, size_t start, size_t size) {
	if (start + size>ba.size())
		throw std::runtime_error("Array derivato troppo grande");
	_size = size;
	_data = ba._data + start;
}

ByteArray::ByteArray(const ByteArray &src) {
	_size = src._size;
	_data = src._data;
};

ByteDynArray &ByteDynArray::operator = (ByteDynArray &&src) {
	_data = src._data;
	_size = src._size;
	src._data = nullptr;
	src._size = 0;
	return *this;
}

bool ByteArray::operator==(const ByteArray &src) const {
	if (_size != src._size)
		return false;
	return (memcmp(_data, src._data, _size) == 0);

};

int ByteArray::atoi() const {
	int val = 0;
	for (size_t i = 0; i<_size; i++) {
		if (val>(INT_MAX / 10))
			throw std::overflow_error("owerflow");
		val = (val * 10) + _data[i] - '0';
	}
	return val;
}
bool ByteArray::operator<(const ByteArray &src) const {
	if (_size<src._size)
		return true;
	return (memcmp(_data, src._data, _size)<0);
};

bool ByteArray::operator>(const ByteArray &src) const {

	if (_size>src._size)
		return true;
	return (memcmp(_data, src._data, _size)>0);
};

bool ByteArray::operator!=(const ByteArray &src) const {
	if (_size != src._size)
		return true;
	return (memcmp(_data, src._data, _size) != 0);
};

void ByteArray::copy(const ByteArray &src, size_t start) {
	if (src._size + start>_size)
		throw std::runtime_error(stdPrintf("Dimensione array da copiare %i troppo grande; dimensione massima %i", src._size + start, _size));
	::memcpy_s(_data + start, _size - (start), src._data, src._size);
}

void ByteArray::rightcopy(const ByteArray &src, size_t end) {
	if (src._size + end>_size)
		throw std::runtime_error(stdPrintf("Dimensione array da copiare %i troppo grande; dimensione massima %i", src._size + end, _size));
	::memcpy_s(_data + _size - end - src._size, (end + src._size), src._data, src._size);
}

ByteArray &ByteArray::fill(const uint8_t value) {
	for (size_t i = 0; i<_size; i++)
		_data[i] = value;
	return *this;
}

ByteArray &ByteArray::random() {
	RAND_bytes(_data, (int)_size);
	return *this;
}

ByteArray &ByteArray::reverse() {
	size_t dwHalfSize = _size >> 1;
	uint8_t temp;
	for (size_t i = 0; i<dwHalfSize; i++) {
		temp = _data[i];
		_data[i] = _data[_size - i - 1];
		_data[_size - i - 1] = temp;
	}
	return *this;
}

ByteArray ByteArray::right(size_t size) const {

	if (size>_size)
		throw std::runtime_error("Array derivato troppo grande");
	return(ByteArray(*this, _size - size, size ));

}

ByteArray ByteArray::left(size_t size) const {

	if (size>_size)
		throw std::runtime_error("Array derivato troppo grande");
	return(ByteArray(*this, 0, size));

}

ByteArray ByteArray::mid(size_t start) const {
	if (start >_size)
		throw std::runtime_error("Array derivato troppo grande");
	return(ByteArray(*this, start, _size - start));
}

ByteArray ByteArray::mid(size_t start, size_t size) const {
	if (start + size>_size)
		throw std::runtime_error("Array derivato troppo grande");
	return(ByteArray(*this, start, size ));
}

ByteArray ByteArray::revmid(size_t toend) const {
	if (toend >_size)
		throw std::runtime_error("Array derivato troppo grande");
	return(ByteArray(*this, 0, _size - toend));
}

ByteArray ByteArray::revmid(size_t toend, size_t size) const {
	if (toend + size>_size)
		throw std::runtime_error("Array derivato troppo grande");
	return(ByteArray(*this, _size - toend - size, size ));
}

ByteArray::~ByteArray() {
	_size = 0;
	_data = nullptr;
}

ByteDynArray ByteArray::getASN1Tag(unsigned int tag) const {
	
	size_t tl = ASN1TLength(tag);
	size_t ll = ASN1LLength(size());
	ByteDynArray result(tl + ll + size());
	putASN1Tag(tag, result);
	putASN1Length(size(), result.mid(tl));
	result.mid(tl + ll).copy(*this);
	return result;
}

void ByteDynArray::alloc_copy(const ByteArray &src) {
	clear();
	_data = new uint8_t[src.size()];
	_size = src.size();
	copy(src);
	return;
}

ByteDynArray::ByteDynArray()  {
	_size = 0;
	_data = nullptr;
};

ByteDynArray::ByteDynArray(const ByteDynArray &src) : ByteDynArray() {
	alloc_copy(src);
};

ByteDynArray::ByteDynArray(const ByteArray &src) : ByteDynArray() {
	alloc_copy(src);
};

ByteDynArray::ByteDynArray(const char *data) : ByteDynArray() {
	readHexData(data, *this);
}

ByteDynArray::ByteDynArray(size_t size) {
	_data = new uint8_t[size];
	_size = size;

};
ByteDynArray::~ByteDynArray() {
	clear();
}

ByteDynArray &ByteDynArray::operator = (const ByteDynArray &src) {
	alloc_copy(src);
	return *this;
}

void ByteDynArray::resize(size_t size, bool bKeepData) {
	if (!bKeepData) {
		clear();
		_data = new uint8_t[size];
		_size = size;
	}
	else {
		uint8_t* pbtNewData = new uint8_t[size];
		size_t dwMinSize = min(size, _size);
		if (dwMinSize>0)
			memcpy_s(pbtNewData, size, _data, dwMinSize);
		clear();
		_data = pbtNewData;
		_size = size;
	}
}

void ByteDynArray::clear() {
	if (_data != nullptr)
		delete[] _data;
	_data = nullptr;
	_size = 0;
}

ByteDynArray &ByteDynArray::append(const ByteArray &src) {
	if (src.size()>0) {
		resize(_size + src.size(), true);
		rightcopy(src);
	}
	return *this;
}

ByteDynArray &ByteDynArray::push(const uint8_t data) {
	resize(_size + 1, true);
	_data[_size - 1] = data;
	return *this;
}

uint8_t* ByteDynArray::detach() {
	return _data;
	_data = nullptr;
	_size = 0;
}
bool ByteArray::indexOf(ByteArray &data,size_t &position) const {
	if (data.size() == 0)
		return 0;
	if (_size < data.size())
		return false;
	size_t start = _size - data.size();
	for (size_t i = 0; i <= start; i++) {
		bool ok = true;
		for (unsigned int j = 0; j < data.size(); j++) {
			if (_data[i + j] != data[j]) {
				ok = false;
				break;
			}
		}
		if (ok) {
			position = i;
			return true;
		}
	}
	return false;
}