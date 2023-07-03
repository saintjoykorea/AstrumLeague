package kr.co.brgames.cdk;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

public class CSByteBufferInputStream extends InputStream {
    private final ByteBuffer _byteBuffer;

    public CSByteBufferInputStream(ByteBuffer byteBuffer) {
        _byteBuffer = byteBuffer;
    }

    @Override
    public int available() throws IOException {
        return _byteBuffer.remaining();
    }

    @Override
    public int read() throws IOException {
        if (!_byteBuffer.hasRemaining()) return -1;
        return _byteBuffer.get();
    }

    @Override
    public int read(byte[] bytes, int offset, int length) throws IOException {
        length = Math.min(_byteBuffer.remaining(), length);
        if (length == 0) return -1;
        _byteBuffer.get(bytes, offset, length);
        return length;
    }
}
