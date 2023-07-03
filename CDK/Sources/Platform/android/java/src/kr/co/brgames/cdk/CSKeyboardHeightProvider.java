package kr.co.brgames.cdk;

import android.graphics.Rect;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager.LayoutParams;
import android.widget.PopupWindow;

public class CSKeyboardHeightProvider extends PopupWindow {
    private final View _popupView;

    public CSKeyboardHeightProvider() {
        super(CSActivity.sharedActivity());

        _popupView = new View(CSActivity.sharedActivity());
        _popupView.setVisibility(View.INVISIBLE);
        setContentView(_popupView);

        setSoftInputMode(LayoutParams.SOFT_INPUT_ADJUST_RESIZE | LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
        setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);

        setWidth(0);
        setHeight(LayoutParams.MATCH_PARENT);
        setBackgroundDrawable(null);

        _popupView.getViewTreeObserver().addOnGlobalLayoutListener(this::handleOnGlobalLayout);
    }

    public void start() {
        if (!isShowing() && CSActivity.sharedActivity().view().getWindowToken() != null) {
            showAtLocation(CSActivity.sharedActivity().view(), Gravity.NO_GRAVITY, 0, 0);
        }
    }

    public void close() {
        dismiss();
    }
    
    private void handleOnGlobalLayout() {
        Rect rect = new Rect();
        _popupView.getWindowVisibleDisplayFrame(rect);

        int screenHeight = CSActivity.sharedActivity().view().height();
        int keyboardHeight = screenHeight - rect.bottom;

        if (keyboardHeight <= screenHeight * 0.1f || keyboardHeight >= screenHeight * 0.9f) keyboardHeight = 0;

        CSActivity.sharedActivity().view().onKeyboardHeightChanged(keyboardHeight);
    }
}
