
#include "core/audio/Ringbuffer.hpp"

#include <cstdint>
#include <cstring>

RingbufferBase::RingbufferBase() :
    mInitialized(false),
    mRingbuffer(),
    mSize(0)
{

}

RingbufferBase::~RingbufferBase() {
    uninit();
}

void RingbufferBase::init(size_t buffersize, void *buffer) {
    uninit();
    auto result = ma_rb_init(buffersize, buffer, nullptr, &mRingbuffer);
    mInitialized = result == MA_SUCCESS;
    mSize = buffersize;
}

void RingbufferBase::uninit() {
    if (mInitialized) {
        ma_rb_uninit(&mRingbuffer);
        mInitialized = false;
    }
}

size_t RingbufferBase::size() const {
    return mSize;
}

size_t RingbufferBase::read(void *data, size_t bytes) {
    size_t bytesToRead = bytes;
    void *src;
    ma_rb_acquire_read(&mRingbuffer, &bytesToRead, &src);
    memcpy(data, src, bytesToRead);
    ma_rb_commit_read(&mRingbuffer, bytesToRead, src);
    return bytesToRead;
}

size_t RingbufferBase::write(void const *data, size_t bytes) {
    void *dest;
    size_t bytesToWrite = bytes;
    ma_rb_acquire_write(&mRingbuffer, &bytesToWrite, &dest);
    memcpy(dest, data, bytesToWrite);
    ma_rb_commit_write(&mRingbuffer, bytesToWrite, dest);
    return bytesToWrite;
}

size_t RingbufferBase::fullRead(void *buf, size_t bufsize) {
    size_t readAvail = ma_rb_available_read(&mRingbuffer);

    if (readAvail == 0) {
        return 0;
    }

    // do the first read
    size_t bytesRead = read(buf, bufsize);

    bufsize -= bytesRead;
    readAvail -= bytesRead;

    if (bufsize != 0 && readAvail) {
        // do the second read (the read pointer starts at the beginning)
        bytesRead += read(reinterpret_cast<uint8_t*>(buf) + bytesRead, bufsize);

    }

    return bytesRead;
}

size_t RingbufferBase::fullWrite(void const *buf, size_t bufsize) {
    size_t writeAvail = ma_rb_available_write(&mRingbuffer);

    if (writeAvail == 0) {
        return 0;
    }

    size_t bytesWritten = write(buf, bufsize);

    bufsize -= bytesWritten;
    writeAvail -= bytesWritten;

    if (bufsize != 0 && writeAvail) {
        bytesWritten += write(reinterpret_cast<uint8_t const*>(buf) + bytesWritten, bufsize);
    }

    return bytesWritten;
}

