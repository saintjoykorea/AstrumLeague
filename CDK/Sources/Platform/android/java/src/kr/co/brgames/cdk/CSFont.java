package kr.co.brgames.cdk;

import android.util.Log;
import android.graphics.Typeface;

import java.util.ArrayList;
import java.util.List;

public class CSFont {
    private static class CustomTypeface {
        public Typeface typeface;
        public String name;
        public int style;

        public CustomTypeface(Typeface typeface, String name, int style) {
            this.typeface = typeface;
            this.name = name;
            this.style = style;
        }
    }
    private static final List<CustomTypeface> customTypefaces = new ArrayList<>();

    private final List<Typeface> _typefaces;
    private final float _size;

    public static final int Normal = 0;
    public static final int Bold = 1;
    public static final int Italic = 2;
    public static final int Medium = 4;     //extension from here
    public static final int SemiBold = 5;
    
    private static final String TAG = "CSFont";
    
    public CSFont(float size, int style) {
        if (style >= Medium){
            Log.w(TAG, "unable to find system font style:" + style);
            
            style = Normal;
        }
        _typefaces = new ArrayList<>(1);
        _typefaces.add(Typeface.create((String)null, style));
        _size = size;
    }

    public CSFont(String name, float size, int style) {
        _typefaces = new ArrayList<>();
        if (name != null) {
            for (CustomTypeface e : customTypefaces) {
                if (e.name.equals(name) && e.style == style) {
                    _typefaces.add(e.typeface);
                }
            }
        }
        //glyph가 없을 수 있으므로 기본 폰트를 마지막에 추가
        if (style >= Medium) style = Normal;
        _typefaces.add(Typeface.create(name, style));
        _size = size;
    }

    public static void load(String name, int style, String path) {
        Typeface typeface;

        if (path.startsWith("assets/")) {
            typeface = Typeface.createFromAsset(CSActivity.sharedActivity().getAssets(), path.substring(7));
        }
        else {
            typeface = Typeface.createFromFile(path);
        }
        customTypefaces.add(new CustomTypeface(typeface, name, style));
    }

    public float size() {
        return _size;
    }

    public List<Typeface> typefaces() {
        return _typefaces;
    }

    public Typeface typeface() {
        return _typefaces.get(_typefaces.size() - 1);     //커스텀폰트가 아닌 단말의 기본 폰트
    }
}
