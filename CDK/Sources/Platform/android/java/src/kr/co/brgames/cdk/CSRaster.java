package kr.co.brgames.cdk;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.Typeface;
import android.os.Build;
import android.text.TextPaint;
import android.util.Log;

import java.io.FileOutputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;

public class CSRaster {
    private static final String TAG = "CSRaster";

    private static final TextPaint _textPaint = new TextPaint(Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);

    static {
        _textPaint.setTextAlign(Align.LEFT);
        _textPaint.setColor(0xFFFFFFFF);
    }

    private static float characterWidth(CSFont font, String cc) {
        _textPaint.setTextSize(font.size());

        for (Typeface typeface : font.typefaces()) {
            _textPaint.setTypeface(typeface);

            boolean flag = true;

            if (Build.VERSION.SDK_INT >= 23) {
                if (cc.length() > 1) {
                    int i = 0;
                    do {
                        int cp = Character.codePointAt(cc, i);
                        int cplen = Character.charCount(cp);
                        flag = Character.isIdentifierIgnorable(cp) || _textPaint.hasGlyph(cc.substring(i, i + cplen));
                        if (!flag) break;
                        i += cplen;
                    } while (i < cc.length());
                } else flag = _textPaint.hasGlyph(cc);
            }
            float w = flag ? _textPaint.measureText(cc) : 0;

            if (w > 0) return w;
        }

        return 0;
    }

    public static void characterSize(CSFont font, String cc, float[] outSize) {
        outSize[0] = characterWidth(font, cc);

        if (outSize[0] > 0) {
            Paint.FontMetrics fm = _textPaint.getFontMetrics();
            outSize[1] = fm.descent - fm.ascent;
        }
        else outSize[1] = 0;
    }

    public static byte[] createRasterFromCharacter(CSFont font, String cc, float[] outSize) {
        int w = (int)Math.ceil(characterWidth(font, cc));

        if (w <= 0) return null;

        Paint.FontMetrics fm = _textPaint.getFontMetrics();
        int h = (int)Math.ceil(fm.descent - fm.ascent);

        if (h <= 0) return null;

        byte[] data = new byte[w * h * 4];

        Bitmap internalBitmap = Bitmap.createBitmap(w, h, Config.ARGB_8888);
        ByteBuffer internalPixelBuffer = ByteBuffer.wrap(data);

        Canvas canvas = new Canvas(internalBitmap);
        internalBitmap.eraseColor(0);

        canvas.drawText(cc, 0, -fm.ascent, _textPaint);

        internalPixelBuffer.position(0);
        internalBitmap.copyPixelsToBuffer(internalPixelBuffer);
        internalBitmap.recycle();

        outSize[0] = w;
        outSize[1] = h;
        outSize[2] = _textPaint.getTextSize() + fm.ascent;
        return data;
    }

    public static Bitmap createBitmapFromImageSource(ByteBuffer source) {
        return BitmapFactory.decodeStream(new CSByteBufferInputStream(source));
    }

    public static void recycleBitmap(Bitmap bitmap) {
        bitmap.recycle();
    }
    
    public static boolean saveImageFromRaster(String path, int[] raster, int width, int height) {
        Bitmap.CompressFormat format;
        if (path.endsWith("png") || path.endsWith("PNG")) {
            format = Bitmap.CompressFormat.PNG;
        }
        else if (path.endsWith("jpg") || path.endsWith("JPG")) {
            format = Bitmap.CompressFormat.JPEG;
        }
        else {
            Log.e(TAG, "unknown image format:" + path);
            return false;
        }
        try {
            Bitmap bitmap = Bitmap.createBitmap(raster, width, height, Config.ARGB_8888);
            
            OutputStream os = new FileOutputStream(path);
            bitmap.compress(format, 100, os);
            os.close();
            
            bitmap.recycle();

            return true;
        }
        catch (Throwable e) {
            Log.e(TAG, e.getMessage(), e);
            return false;
        }
    }
}
