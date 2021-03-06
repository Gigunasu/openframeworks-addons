// =============================================================================
//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofx/IO/ByteBufferWriter.h"
#include <string.h>


namespace ofx {
namespace IO {


ByteBufferWriter::ByteBufferWriter(ByteBuffer& buffer, std::size_t offset):
    _buffer(buffer),
    _offset(offset)
{
}


std::size_t ByteBufferWriter::_write(const void* source, std::size_t size)
{
    if (_offset + size <= _buffer.size())
    {
        memcpy(_buffer.getPtr() + _offset, source, size);
        _offset += size;
        return size;
    }
    else
    {
        return 0;
    }
}


void ByteBufferWriter::setOffset(size_t offset)
{
    if (offset < _buffer.size()) _offset = offset;
}


std::size_t ByteBufferWriter::getOffset() const
{
    return _offset;
}


std::size_t ByteBufferWriter::remaining() const
{
    return _buffer.size() - _offset;
}


} }  // namespace ofx::IO
