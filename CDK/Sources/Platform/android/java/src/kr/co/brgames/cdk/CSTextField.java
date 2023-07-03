package kr.co.brgames.cdk;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Color;
import android.graphics.RectF;
import android.text.Editable;
import android.text.InputFilter;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

@SuppressLint("AppCompatCustomView")
class EditText extends android.widget.EditText {
    public EditText(Context context) {
        super(context);
    }

    public EditText(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
    }

    public boolean onKeyPreIme(int keyCode, KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (keyCode == KeyEvent.KEYCODE_BACK) {
                clearFocus();
            }
        }
        return super.onKeyPreIme(keyCode, event); // 시스템 default 함수 사용
    }

    @Override
    public void scrollTo(int x, int y) {
        if (getMaxLines() <= 1) {
            y = 0;
        }
        super.scrollTo(x, y);
    }
}

public class CSTextField implements OnEditorActionListener, OnKeyListener, TextWatcher {
    private EditText _editText;
    private RelativeLayout _layout;
    private int _maxLength;
    private int _maxLines;
    private long _target;
    private boolean _isFocused;
    private RectF _frame;
    private String _text;
    private int _textColor;
    private CSFont _font;
    private String _placeholder;
    private int _textAlignment;
    private int _returnKeyType;
    private int _keyboardType;
    
    private static final String TAG = "CSTextField";

    public CSTextField(long target) {
        _target = target;
        _maxLines = 1;
        _text = "";
        _placeholder = "";

        CSActivity.sharedActivity().queueEvent(() -> {
            _editText = new EditText(CSActivity.sharedActivity());
            _editText.setBackgroundColor(Color.TRANSPARENT);

            _editText.setPadding(0, 0, 0, 0);
            _editText.setGravity(Gravity.START | Gravity.CENTER_VERTICAL);
            _editText.setImeOptions(EditorInfo.IME_FLAG_NO_EXTRACT_UI | EditorInfo.IME_ACTION_DONE);
            _editText.setMaxLines(_maxLines);
            _editText.setHorizontallyScrolling(true);
            _editText.setInputType(InputType.TYPE_CLASS_TEXT);
            _editText.setTextColor(Color.BLACK);
            _editText.setOnFocusChangeListener((view, hasFocus) -> _isFocused = hasFocus);

            _layout = new RelativeLayout(CSActivity.sharedActivity());
            _layout.setBackgroundColor(Color.TRANSPARENT);
            _layout.setLayoutParams(new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

            _layout.addView(_editText);
        });
    }

    public void addToView() {
        CSActivity.sharedActivity().queueEvent(() -> {
            ViewGroup parent = CSActivity.sharedActivity().viewGroup();

            parent.addView(_layout);

            _editText.setOnEditorActionListener(CSTextField.this);
            _editText.setOnKeyListener(CSTextField.this);
            _editText.addTextChangedListener(CSTextField.this);
        });
    }

    public void removeFromView() {
        CSActivity.sharedActivity().queueEvent(() -> {
            InputMethodManager mgr = (InputMethodManager) CSActivity.sharedActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
            mgr.hideSoftInputFromWindow(_editText.getWindowToken(), 0);
            _editText.clearFocus();

            _editText.setOnEditorActionListener(null);
            _editText.setOnKeyListener(null);
            _editText.removeTextChangedListener(CSTextField.this);

            CSActivity.sharedActivity().viewGroup().removeView(_layout);
        });
    }

    public void release() {
        removeFromView();
        _target = 0;
    }

    public void setFrame(RectF frame) {
        _frame = frame;

        CSActivity.sharedActivity().queueEvent(() -> {
            CSView view = CSActivity.sharedActivity().view();
            RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams((int)_frame.width(), (int)_frame.height());
            params.setMargins((int)_frame.left, (int)_frame.top, view.width() - (int)_frame.right, view.height() - (int)_frame.bottom);
            _editText.setWidth((int)_frame.width());
            _editText.setHeight((int)_frame.height());
            _editText.setLayoutParams(params);
            _editText.setIncludeFontPadding(false);
        });
    }

    public void setFrame(float x, float y, float width, float height) {
        setFrame(new RectF(x, y, x + width, y + height));
    }

    public String text() { return _text; }

    public void setText(String text) {
        _text = text;

        CSActivity.sharedActivity().queueEvent(() -> {
            _editText.setText(_text);
            _editText.setSelection(_editText.length());
        });
    }
    
    public void clearText() {
        setText("");
    }
    
    public int textColor() {
        return _textColor;
    }

    public void setTextColor(int textColor) {
        _textColor = textColor;

        CSActivity.sharedActivity().queueEvent(() -> _editText.setTextColor(_textColor));
    }

    public void setFont(CSFont font) {
        _font = font;

        CSActivity.sharedActivity().queueEvent(() -> {
            DisplayMetrics dm = new DisplayMetrics();
            CSActivity.sharedActivity().getWindowManager().getDefaultDisplay().getMetrics(dm);

            float scaleDensity = dm.scaledDensity;

            _editText.setTypeface(_font.typeface());
            _editText.setTextSize(_font.size() / scaleDensity);
        });
    }

    public String placeholder() {
        return _placeholder;
    }

    public void setPlaceholder(String placeholder) {
        _placeholder = placeholder;

        CSActivity.sharedActivity().queueEvent(() -> _editText.setHint(_placeholder));
    }

    public int textAlignment() {
        return _textAlignment;
    }

    public void setTextAlignment(int textAlignment) {
        _textAlignment = textAlignment;
        CSActivity.sharedActivity().queueEvent(() -> {
            switch (_textAlignment) { // see also, CSTextField.h
                case 0:
                    _editText.setGravity(Gravity.START | Gravity.CENTER_VERTICAL);
                    break;
                case 1:
                    _editText.setGravity(Gravity.CENTER | Gravity.CENTER_VERTICAL);
                    break;
                case 2:
                    _editText.setGravity(Gravity.END | Gravity.CENTER_VERTICAL);
                    break;
            }
        });
    }

    public int returnKeyType() {
        return _returnKeyType;
    }

    public void setReturnKeyType(int returnKeyType) {
        _returnKeyType = returnKeyType;

        CSActivity.sharedActivity().queueEvent(() -> {
            int flag = EditorInfo.IME_FLAG_NO_EXTRACT_UI;
            switch (_returnKeyType) { // see also, CSTextField.h
                case 1:
                    flag |= EditorInfo.IME_ACTION_GO;
                    break;
                case 3:
                    flag |= EditorInfo.IME_ACTION_NEXT;
                    break;
                case 4:
                    flag |= EditorInfo.IME_ACTION_SEARCH;
                    break;
                case 5:
                    flag |= EditorInfo.IME_ACTION_SEND;
                    break;
                default:
                    flag |= EditorInfo.IME_ACTION_DONE;
                    break;
            }
            _editText.setImeOptions(flag);
        });
    }

    public int keyboardType() {
        return _keyboardType;
    }

    public void setKeyboardType(int keyboardType) {
        _keyboardType = keyboardType;

        CSActivity.sharedActivity().queueEvent(() -> {
            switch (_keyboardType) {    // see also, CSTextField.h
                case 1:
                    _editText.setInputType(InputType.TYPE_TEXT_VARIATION_URI | InputType.TYPE_CLASS_TEXT);
                    break;
                case 2:
                    _editText.setInputType(InputType.TYPE_CLASS_NUMBER);
                    break;
                case 3:
                    _editText.setInputType(InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS | InputType.TYPE_CLASS_TEXT);
                    break;
                case 4:
                    _editText.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_MULTI_LINE | InputType.TYPE_TEXT_FLAG_CAP_SENTENCES);
                    _editText.setSingleLine(false);
                    _editText.setGravity(Gravity.TOP | Gravity.START);
                    _editText.setVerticalScrollBarEnabled(true);
                    _editText.setScrollBarStyle(View.SCROLLBARS_INSIDE_INSET);
                    break;
                default:
                    _editText.setInputType(InputType.TYPE_CLASS_TEXT);
                    break;
            }
        });
    }

    public boolean isSecureText() {
        return false;
    }

    public void setSecureText(boolean secureText) {
        Log.e(TAG, "secureText not yet supported");
    }

    public int maxLength() {
        return _maxLength;
    }

    public void setMaxLength(int maxLength) {
        _maxLength = maxLength;

        CSActivity.sharedActivity().queueEvent(() -> {
            InputFilter[] filters = new InputFilter[1];
            filters[0] = new InputFilter.LengthFilter(_maxLength > 0 ? _maxLength : Integer.MAX_VALUE);
            _editText.setFilters(filters);
        });
    }

    public int maxLine() {
        return _maxLines;
    }

    public void setMaxLine(int maxLine) {
        _maxLines = maxLine;

        CSActivity.sharedActivity().queueEvent(() -> {
            _editText.setMaxLines(_maxLines);
            _editText.setMinLines(_maxLines);
        });
    }

    public boolean isFocused() {
        return _isFocused;
    }

    public void setFocus(final boolean focused) {
        CSActivity.sharedActivity().queueEvent(() -> {
            if (focused) {
                _editText.requestFocus();
                _editText.setSelection(_editText.length());

                InputMethodManager mgr = (InputMethodManager) CSActivity.sharedActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
                mgr.toggleSoftInputFromWindow(_editText.getApplicationWindowToken(), InputMethodManager.SHOW_FORCED, 0);
                mgr.showSoftInput(_editText, InputMethodManager.SHOW_FORCED);
                mgr.showSoftInputFromInputMethod(_editText.getApplicationWindowToken(), InputMethodManager.SHOW_FORCED);
            }
            else {
                InputMethodManager mgr = (InputMethodManager) CSActivity.sharedActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
                mgr.hideSoftInputFromWindow(_editText.getWindowToken(), 0);
                _editText.clearFocus();
            }
        });
    }

    @Override
    public void afterTextChanged(Editable editable) {
        String text = editable.toString();

        if ((_editText.getInputType() & InputType.TYPE_TEXT_FLAG_MULTI_LINE) == InputType.TYPE_TEXT_FLAG_MULTI_LINE) {
            int lines = _editText.getLineCount();
            if (lines > _maxLines) {
                int start = _editText.getSelectionStart();
                int end = _editText.getSelectionEnd();

                if (start == end && start < text.length() && start > 1) {
                    text = text.substring(0, start - 1) + text.substring(start);
                }
                else {
                    text = text.substring(0, text.length() - 1);
                }
                _editText.setText(text);
                _editText.setSelection(_editText.length());
            }
        }

        _text = text;

        if (_target != 0) {
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_target != 0) {
                    nativeTextFieldChanged(_target);
                }
            });
        }
    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {

    }

    @Override
    public void onTextChanged(CharSequence s, int start, int count, int before) {

    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN && keyCode == KeyEvent.KEYCODE_ENTER) {
            if (_target != 0) {
                if (_maxLines > 1) {
                    if (_editText.getLineCount() < _maxLines) {
                        _editText.setLines(_editText.getLineCount() + 1);
                        int p = _editText.getSelectionEnd();
                        _editText.getText().insert(p, "\n");
                        _editText.setSelection(p + 1);
                    }
                }
                else {
                    CSActivity.sharedActivity().view().queueEvent(() -> {
                        if (_target != 0 && _maxLines <= 1) {
                            nativeTextFieldReturn(_target);
                        }
                    });
                }
            }
            return true;
        }
        return false;
    }

    @Override
    public boolean onEditorAction(final TextView v, int actionId, KeyEvent event) {
        if (actionId == EditorInfo.IME_ACTION_DONE) {
            if (_target != 0) {
                CSActivity.sharedActivity().view().queueEvent(() -> {
                    if (_target != 0) {
                        nativeTextFieldReturn(_target);
                    }
                });
            }
            return true;
        }
        return false;
    }
    
    public static native void nativeTextFieldReturn(long target);
    public static native void nativeTextFieldChanged(long target);
}
